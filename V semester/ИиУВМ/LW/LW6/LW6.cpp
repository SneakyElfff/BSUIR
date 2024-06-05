//// LW6.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <WinSock2.h>
#include <windows.h>

#include "BluetoothAPIs.h"
#include <iostream>
#include <set>
#include <string>
#include <conio.h>
#include <ws2bth.h>

#pragma comment(lib, "Bthprops.lib")

using namespace std;

struct Device {
    wstring name;
    bool is_connected;

    bool operator<(const Device& other) const {
        return name < other.name;
    }
};

set<Device> list_of_devices;

bool GetBluetoothAddress(const wstring& deviceName, BTH_ADDR& address) {
    BLUETOOTH_DEVICE_SEARCH_PARAMS searchParams = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS) };
    BLUETOOTH_DEVICE_INFO deviceInfo = { sizeof(BLUETOOTH_DEVICE_INFO) };
    HBLUETOOTH_DEVICE_FIND hFind;

    searchParams.fReturnAuthenticated = TRUE;
    searchParams.fReturnRemembered = TRUE;
    searchParams.fReturnConnected = TRUE;

    hFind = BluetoothFindFirstDevice(&searchParams, &deviceInfo);
    if (hFind) {
        do {
            if (deviceInfo.szName == deviceName) {
                address = deviceInfo.Address.ullLong;
                BluetoothFindDeviceClose(hFind);
                return true;
            }
        } while (BluetoothFindNextDevice(hFind, &deviceInfo));

        BluetoothFindDeviceClose(hFind);
    }

    wcerr << "Error: Failed to find Bluetooth device with name " << deviceName << endl;
    return false;
}

bool SendFileViaBluetooth(const wstring& deviceName, const wstring& filePath) {
    BTH_ADDR address;

    if (!GetBluetoothAddress(deviceName, address)) {
        return false;
    }

    // Find the device by name
    auto it = list_of_devices.find(Device{ deviceName, false });
    if (it == list_of_devices.end()) {
        cerr << "Error: Device not found." << endl;
        return false;
    }

    // Check if the device is connected
    if (!it->is_connected) {
        cerr << "Error: Device is not connected." << endl;
        return false;
    }

    // Initialize WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Error: Failed to initialize WinSock." << endl;
        return false;
    }

    // Create a Bluetooth socket
    SOCKET s = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
    if (s == INVALID_SOCKET) {
        cerr << "Error: Failed to create Bluetooth socket." << endl;
        WSACleanup();
        return false;
    }

    // Set the Bluetooth address of the remote device
    SOCKADDR_BTH sa = { 0 };
    sa.addressFamily = AF_BTH;
    sa.btAddr = address;
    sa.serviceClassId = RFCOMM_PROTOCOL_UUID;
    sa.port = BT_PORT_ANY;

    // Connect to the remote device
    if (connect(s, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR) {
        cerr << "Error: Failed to connect to the remote device." << endl;
        closesocket(s);
        WSACleanup();
        return false;
    }

    // Open the file for reading
    FILE* file;
    if (_wfopen_s(&file, filePath.c_str(), L"rb") != 0 || file == nullptr) {
        cerr << "Error: Failed to open the file." << endl;
        closesocket(s);
        WSACleanup();
        return false;
    }

    // Read and send the file content
    const int bufferSize = 1024;
    char buffer[bufferSize];
    int bytesRead;

    while ((bytesRead = fread(buffer, 1, bufferSize, file)) > 0) {
        if (send(s, buffer, bytesRead, 0) == SOCKET_ERROR) {
            cerr << "Error: Failed to send data." << endl;
            fclose(file);
            closesocket(s);
            WSACleanup();
            return false;
        }
    }

    // Clean up
    fclose(file);
    closesocket(s);
    WSACleanup();

    return true;
}

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "Choose an option: " << endl << "1. Send a file;" << endl << "2. Finish." << endl << endl;

    BLUETOOTH_DEVICE_SEARCH_PARAMS bdsp;
    BLUETOOTH_DEVICE_INFO bdi;
    HBLUETOOTH_DEVICE_FIND hbf;

    ZeroMemory(&bdsp, sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS));

    bdsp.dwSize = sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS);
    bdsp.fReturnAuthenticated = TRUE;
    bdsp.fReturnRemembered = TRUE;
    bdsp.fReturnUnknown = TRUE;
    bdsp.fReturnConnected = FALSE;
    bdsp.fIssueInquiry = TRUE;
    bdsp.cTimeoutMultiplier = 4;
    bdsp.hRadio = NULL;

    bdi.dwSize = sizeof(bdi);

    while (true) {
        bool flag = false;

        if (_kbhit()) {
            char choice = _getch();

            if (choice == '2') {
                break;
            }
            else if (choice == '1') {
                wstring deviceName, filePath;
                cout << "Enter the device name to send the file to: ";
                wcin >> deviceName;
                cout << "Enter the path to the file: ";
                wcin >> filePath;

                if (SendFileViaBluetooth(deviceName, filePath)) {
                    cout << "File sent successfully." << endl;
                }
                else {
                    cerr << "Error: Failed to send the file." << endl;
                }

                break;
            }
        }

        hbf = BluetoothFindFirstDevice(&bdsp, &bdi);

        if (hbf) {
            do {
                Device temp = { bdi.szName, bdi.fConnected };

                if ((list_of_devices.insert(temp)).second)
                    flag = true;
               
                else {    //если устройство уже в коллекции
                    auto it = list_of_devices.find(Device{ (list_of_devices.insert(temp)).first->name, false });

                    if (it != list_of_devices.end()) {    //если он не был только что добавлен
                        if (it->is_connected != temp.is_connected) {    //если статус о подключении изменился, данные обновляются
                            list_of_devices.erase(it);
                            list_of_devices.insert(temp);

                            flag = true;
                        }
                    }
                }

            } while (BluetoothFindNextDevice(hbf, &bdi));

            BluetoothFindDeviceClose(hbf);
        }
        else {
            cerr << "Error: failed to find Bluetooth devices." << endl;
            break;
        }

        if (flag) {
            system("cls");

            cout << "Choose an option: " << endl << "1. Send a file;" << endl << "2. Finish." << endl << endl;

            for (const auto &device : list_of_devices)
                wcout << device.name << ": " << (device.is_connected ? " connected" : " disconnected") << endl;
        }

        Sleep(5000);
    }

    return 0;
}