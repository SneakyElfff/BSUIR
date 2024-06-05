#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <iostream>

#pragma comment(lib, "setupapi.lib")

using namespace std;
using namespace cv;

int counter = 0;
VideoCapture cap(0);    //������ ������ VideoCapture ��� ������� ����� � ����������

int counter2 = 0;

LRESULT CALLBACK detectHooks(int notif_code, WPARAM param_w, LPARAM param_l) {    //������� ��������� ������ ��� ��������� ������� ����������
    if (notif_code == HC_ACTION) {    //��� ����������� => ��������� ���������� - ���
        KBDLLHOOKSTRUCT *keyboard_params = (KBDLLHOOKSTRUCT*)param_l;    //��������� KBDLLHOOKSTRUCT �������� ���������� � ��������

        if (param_w == WM_KEYDOWN) {    //���������� ������� - ������ �������
            switch (keyboard_params->vkCode) {
            case 'P':
            {
                Mat frame;
                cap >> frame;

                if (!frame.empty()) {
                    string filename = "image" + to_string(counter) + ".jpg";

                    if (imwrite(filename, frame)) {
                        cout << endl << "--------------------------------------------------------------------------------------------------------------" << endl;
                        cout << "Image captured and saved as " << filename << " successfully." << endl;
                        cout << "--------------------------------------------------------------------------------------------------------------" << endl << endl;
                        counter++;
                    }
                    else
                        cerr << "Error: Unable to capture image." << endl;
                }
                else
                    cerr << "Error: Unable to get a frame." << endl;

            }
            break;

            case 'R':
            {
                Mat frame;
                cap >> frame;

                if (!frame.empty()) {
                    int frame_width = cap.get(CAP_PROP_FRAME_WIDTH);
                    int frame_height = cap.get(CAP_PROP_FRAME_HEIGHT);

                    string filename = "video" + to_string(counter2) + ".avi";

                    VideoWriter video(filename, VideoWriter::fourcc('M', 'J', 'P', 'G'), 15, Size(frame_width, frame_height));

                    while (true) {
                        cap.read(frame);
                        video.write(frame);
                        waitKey(30);

                        if (GetAsyncKeyState('V') & 0x8000) {
                            cout << endl << "--------------------------------------------------------------------------------------------------------------" << endl;
                            cout << "Video recorded and saved as " << filename << " successfully." << endl;
                            cout << "--------------------------------------------------------------------------------------------------------------" << endl << endl;
                            counter2++;

                            break;
                        }
                    }

                    video.release();
                }
                else
                    cerr << "Error: Unable to get a frame." << endl;
            }
            break;

            case 'H':
            {
                ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
            }
            break;

            case 'S':
            {
                ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
            }
            break;

            case 'Q':
                PostQuitMessage(0);    //�������� ��������� � ����� ���������� � ���� ��������� ���������
            break;
            }
        }
    }

    return CallNextHookEx(NULL, notif_code, param_w, param_l);    //�������� ���������� ���������� ����
}

int main() {
    MSG msg;
    void getCameraName();

    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);

    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, detectHooks, GetModuleHandle(NULL), 0);    //���������: 1 - ��������� ����, 2 - ��������� �� �������, 3 - ���������� ������� ��������� (��������), 4 - ���������� ���
    if (hook == NULL) {
        cerr << "Error: failed to install a keyboard hook." << endl;
        return -1;
    }

    cout << "Choose an option:" << endl;
    cout << "                 p - take a photo;" << endl;
    cout << "                 r - start recording;" << endl;
    cout << "                 v - stop recording;" << endl;
    cout << "                 h - enter the spy mode;" << endl;
    cout << "                 s - exit the spy mode;" << endl;
    cout << "                 q - finish the program." << endl;

    if (!cap.isOpened()) {
        cerr << "Error: failed to open the webcam." << endl;
        return -1;
    }

    cout << endl << "--------------------------------------------------------------------------------------------------------------" << endl;
    cout << "Webcam information:" << std::endl;
    getCameraName();
    cout << "- width: " << cap.get(CAP_PROP_FRAME_WIDTH) << endl;
    cout << "- height: " << cap.get(CAP_PROP_FRAME_HEIGHT) << endl;
    cout << "--------------------------------------------------------------------------------------------------------------" << endl;

    while (GetMessage(&msg, NULL, 0, 0)) {    //2-4 - ������ � ��������
        TranslateMessage(&msg);    //������������ ����������� ���������
        DispatchMessage(&msg);    //���������� ��������� ����
    }

    UnhookWindowsHookEx(hook);

    destroyAllWindows();

    return 0;
}

void getCameraName() {
    HDEVINFO info = SetupDiGetClassDevs(&GUID_DEVCLASS_CAMERA, 0, 0, DIGCF_PRESENT | DIGCF_PROFILE);    //����� ���������� � �������������� �����������: 0 - 1) ����������, 2) ��������� �����. ����
    if (info == INVALID_HANDLE_VALUE) {
        cerr << "Error: failed to get information about devices." << endl;
        return;
    }

    SP_DEVINFO_DATA device_data;
    ZeroMemory(&device_data, sizeof(SP_DEVINFO_DATA));    //�������������
    device_data.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i=0; SetupDiEnumDeviceInfo(info, i, &device_data); i++) {
        WCHAR camera_name[MAX_PATH];

        if (SetupDiGetDeviceRegistryProperty(info, &device_data, SPDRP_FRIENDLYNAME, NULL, reinterpret_cast<PBYTE>(camera_name), sizeof(camera_name), NULL)) {    //NULL - 1) ��� ������, 2) ����. ������
            wprintf(L"- name: %s\n", camera_name);
        }
    }

    SetupDiDestroyDeviceInfoList(info);
}