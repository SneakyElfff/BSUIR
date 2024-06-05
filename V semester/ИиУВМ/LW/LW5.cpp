 //LW5.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <Windows.h>
#include <Dbt.h>
#include <vector>
#include <initguid.h>
#include <Usbiodef.h>
#include <thread>
#include <setupapi.h>
#include <cfgmgr32.h>
#include <chrono>

using namespace std;

struct Device
{
    wchar_t *name;
    wstring name_friendly;
    HANDLE handle;
    bool safety;
    bool is_removable;
    DEVINST id;
};

vector<Device> list_of_devices;

bool getUSBParams(wchar_t *name, wstring &name_friendly, DEVINST &id) {
    HDEVINFO info;
    WCHAR USB_name[MAX_PATH];
    BYTE buffer[512];
    DEVINST device_instance_id;
    DWORD capability;

    if (name) {
        info = SetupDiCreateDeviceInfoList(NULL, NULL);

        SP_DEVICE_INTERFACE_DATA deviceInterfaceData;
        SetupDiOpenDeviceInterfaceW(info, name, NULL, &deviceInterfaceData);
    }
    else {
        info = SetupDiGetClassDevs(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (info == INVALID_HANDLE_VALUE) {
            cerr << "Error: failed to get information about devices." << endl;
            return false;
        }
    }

    SP_DEVINFO_DATA device_data;
    ZeroMemory(&device_data, sizeof(SP_DEVINFO_DATA));
    device_data.cbSize = sizeof(SP_DEVINFO_DATA);

    for (DWORD i = 0; SetupDiEnumDeviceInfo(info, i, &device_data); i++) {
        SetupDiGetDeviceRegistryProperty(info, &device_data, SPDRP_DEVICEDESC, NULL, reinterpret_cast<PBYTE>(USB_name), sizeof(USB_name), NULL);

        SetupDiGetDeviceInstanceId(info, &device_data, reinterpret_cast<PWSTR>(buffer), sizeof(buffer), NULL);

        wstring buffer_ws(reinterpret_cast<PWSTR>(buffer));
        string buffer_s(buffer_ws.begin(), buffer_ws.end());

        CM_Locate_DevNodeA(&device_instance_id, const_cast<DEVINSTID_A>(buffer_s.c_str()), CM_LOCATE_DEVNODE_NORMAL);

        SetupDiGetDeviceRegistryProperty(info, &device_data, SPDRP_CAPABILITIES, NULL, reinterpret_cast<PBYTE>(&capability), sizeof(capability), NULL);
                
        if (name) {
            name_friendly = USB_name;
            id = device_instance_id;

            if (capability & CM_DEVCAP_REMOVABLE)
                return true;
            else return false;
        }

        Device temp;

        temp.name_friendly = USB_name;
        temp.safety = false;
        temp.is_removable = capability & CM_DEVCAP_REMOVABLE;
        temp.id = device_instance_id;

        list_of_devices.push_back(temp);
    }

    SetupDiDestroyDeviceInfoList(info);
}

LRESULT CALLBACK detectDevices(HWND handle, UINT code_msg, WPARAM param_w, LPARAM param_l) {    //обрабатывает событи€ подключени€/отключени€ устройств
    if (code_msg == WM_DEVICECHANGE) {
        PDEV_BROADCAST_HDR device_info = (PDEV_BROADCAST_HDR)param_l;    //param_l - структура данных, описывающа€ событие

        switch (param_w) {    //тип событи€
        case DBT_DEVICEARRIVAL:    //подключено новое устройство
            if (device_info->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {    //если подключено USB-устройство
                PDEV_BROADCAST_DEVICEINTERFACE usb_info = (PDEV_BROADCAST_DEVICEINTERFACE)device_info;

                HANDLE handle_dev = CreateFileW((wchar_t*)usb_info->dbcc_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                
                DEV_BROADCAST_HANDLE filter;
                filter.dbch_size = sizeof(filter);
                filter.dbch_devicetype = DBT_DEVTYP_HANDLE;
                filter.dbch_handle = handle_dev;

                RegisterDeviceNotificationW(handle, &filter, DEVICE_NOTIFY_WINDOW_HANDLE);

                CloseHandle(handle_dev);

                Device temp;
                temp.name = usb_info->dbcc_name;

                wstring name_friendly;
                DEVINST id;
                temp.is_removable = getUSBParams(temp.name, name_friendly, id);
                temp.name_friendly = name_friendly;
                temp.id = id;

                temp.handle = handle_dev;
                temp.safety = false;

                list_of_devices.push_back(temp);

                wcout << endl << "USB device \"" << temp.name_friendly << "\" inserted" << endl;
            }
            break;

        case DBT_DEVICEQUERYREMOVE:    //устройство подлежит извлечению
            if (device_info->dbch_devicetype == DBT_DEVTYP_HANDLE)
            {
                PDEV_BROADCAST_HANDLE usb_info = (PDEV_BROADCAST_HANDLE)device_info;

                for (int j = 0; j < list_of_devices.size(); j++)
                    if (list_of_devices[j].handle == usb_info->dbch_handle)
                        list_of_devices[j].safety = true;
            }
            break;

        case DBT_DEVICEQUERYREMOVEFAILED:    //неудачна€ попытка извлечени€ устройства
            if (device_info->dbch_devicetype == DBT_DEVTYP_HANDLE)
            {
                PDEV_BROADCAST_HANDLE usb_info = (PDEV_BROADCAST_HANDLE)device_info;

                for (int j = 0; j < list_of_devices.size(); j++)
                    if (list_of_devices[j].handle == usb_info->dbch_handle) {
                        list_of_devices[j].safety = false;

                        wcout << endl << "Failed to safely remove device \"" << list_of_devices[j].name_friendly << "\"" << endl;
                    }
            }
            break;

        case DBT_DEVICEREMOVECOMPLETE:    //отключено устройство
            if (device_info->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE) {
                PDEV_BROADCAST_DEVICEINTERFACE_A usb_info = (PDEV_BROADCAST_DEVICEINTERFACE_A)device_info;

                for (int j = 0; j < list_of_devices.size(); j++)
                    if (list_of_devices[j].name == (wchar_t*)usb_info->dbcc_name) {
                        wcout << endl << "USB device \"" << list_of_devices[j].name_friendly << (list_of_devices[j].safety == true ? "\" safely " : "\" unsafely ") << "removed from drive " << endl;

                        list_of_devices.erase(list_of_devices.begin() + j);
                    }
            }
            break;
        }
    }

    return DefWindowProc(handle, code_msg, param_w, param_l);    //возвращение к стандартной обработке сообщений окна
}

void input() {
    char choice;

    while (1) {
        cout << "Choose an option: " << endl << "1. Get the list of devices;" << endl << "2. Eject a device;" << endl << "3. Finish." << endl;
        cout << ">>> ";
        cin >> choice;

        if (choice == '1') {
            cout << endl;
            for (int i = 0; i < list_of_devices.size(); i++) {
                wcout << i+1 << ") " << list_of_devices[i].name_friendly << ": " << (list_of_devices[i].is_removable ? "removable" : "not removable") << endl;
            }
        }

        else if (choice == '2') {
            int device_number;
            
            cout << "Enter a device number from the list above: ";
            cin >> device_number;

            if (!list_of_devices[device_number - 1].is_removable) {
                cerr << "The device you chose isn't removable." << endl;
                break;
            }

            CM_Request_Device_Eject(list_of_devices[device_number - 1].id, NULL, NULL, 0, 0);
        }

        else if (choice == '3') {
            exit(0);
        }

        else {
            cout << endl << "Wrong parameter" << endl;
        }

        cout << endl;

        this_thread::sleep_for(chrono::seconds(1));
    }
}

int main() {
    setlocale(LC_ALL, "Russian");

    wstring empty;
    DEVINST zero;
    getUSBParams(0, empty, zero);

    WNDCLASS win_class = {};
    HWND handle;
    MSG msg;

    win_class.lpfnWndProc = detectDevices;    //обработчик сообщений
    win_class.hInstance = GetModuleHandle(0);    //дескриптор текущего экземпл€ра программы
    win_class.lpszClassName = L"USBMonitoringClass";
    RegisterClass(&win_class);

    handle = CreateWindowEx(
        0,    //без доп. стилей
        L"USBMonitoringClass",
        L"USB Monitoring Window",
        0,    //без стилей окна
        CW_USEDEFAULT, CW_USEDEFAULT,    //позици€ окна на экране
        CW_USEDEFAULT, CW_USEDEFAULT,    //размеры окна
        0,    //дескриптор родительского окна
        0,    //дескриптор меню
        GetModuleHandle(0), 
        0    //доп. параметры
    );

    GUID guid = GUID_DEVINTERFACE_USB_DEVICE;

    DEV_BROADCAST_DEVICEINTERFACE monitoring = {};    //дл€ уведомлений об изменении устройств интерфейса
    monitoring.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
    monitoring.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    monitoring.dbcc_classguid = guid;

    HDEVNOTIFY hDevNotify = RegisterDeviceNotification(    //регистраци€ окна дл€ получени€ уведомлений
        handle,
        &monitoring,
        DEVICE_NOTIFY_WINDOW_HANDLE
    );

    // Create a thread for manual input
    thread thread_input(input);

    while (GetMessage(&msg, 
        0,    //дескриптор окна (0 - дл€ всех)
        0,    //мин. є
        0)) {    //макс. є
        TranslateMessage(&msg);
        DispatchMessage(&msg);    //-> detectDevices()
    }

    // Wait for the input thread to finish
    thread_input.join();

    UnregisterDeviceNotification(hDevNotify);

    return 0;
}