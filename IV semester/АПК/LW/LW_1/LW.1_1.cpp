// APC.LW.1.cpp : Определяет точку входа для приложения.
// works

#include <windows.h>
#include <iostream>

using namespace std;

HANDLE port1, port2;

int main()
{
    char buffer = 'c';
    DWORD num_of_written, num_of_read;

    port1 = ::CreateFile(L"COM1", GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);    //function opens the COM-port
    port2 = ::CreateFile(L"COM2", GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if(!port1)
        cout << "Opening the COM-port 1 is impossible" << endl;

    DCB properties = {0};    //DCB-structure describes main properties of the COM-port
    properties.DCBlength = sizeof(properties);
    if(!GetCommState(port1, &properties))    //function read DCB structure
        cout << "Error" << endl;
    properties.BaudRate = CBR_9600;
    properties.ByteSize = 8;
    properties.StopBits = ONESTOPBIT;
    properties.Parity = NOPARITY;

    if(!SetCommState(port2, &properties))    //function configures the COM-port
        cout << "Configuring the COM-port is impossible" << endl;

    WriteFile(port1, &buffer, sizeof(buffer), &num_of_written, NULL);

    cout << sizeof(buffer) << "/" << num_of_written << " bytes written." << endl;

    buffer = ' ';
    ReadFile(port2, &buffer, num_of_written, &num_of_read, 0);
    if(num_of_read > 0)
        cout << "Read data: " << buffer << endl;
    else cout << "The buffer is empty" << endl;

    CloseHandle(port1);
    CloseHandle(port2);

    return 0;
}
