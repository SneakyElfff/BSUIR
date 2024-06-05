#include <iomanip>
#include <iostream>

#include "hexioctrl.h"

using namespace std;

#define CHANNEL 0

#define IDENTIFY_DEVICE 0xEC    //for ATA devices
#define IDENTIFY_PACKET_DEVICE 0xA1    //for ATAPI devices

const WORD AS[2] = {0x3F6, 0x376},    //alternative state register
           DR[2] = {0x1F0, 0x170},    //data register
		   DH[2] = {0x1F6, 0x176},    //device/head register
		   CR[2] = {0x1F7, 0x177};    //command register
WORD data[256];

BOOL GetDeviceInfo(int channel, int device);

int main()
{
    ALLOW_IO_OPERATIONS;

	for (int device=0; device<2; device++) {
		cout << endl << "Channel " << CHANNEL << ", ";
		cout << (device == 0 ? "Master" : "Slave");

        if (GetDeviceInfo(CHANNEL, device)) {
            cout << endl << endl << "Model: ";
            for (int i=27; i<=46; i++)
                cout << (CHAR)(data[i] >> 8) << (CHAR)(data[i] & 0x00FF);    //0x00FF - 0000000011111111
            
            cout << endl << "Serial number: ";
            for (int i=10; i<=19; i++)
                cout << (CHAR)(data[i] >> 8) << (CHAR)(data[i] & 0x00FF);

			cout << endl << "Firmware: ";
            for (int i=23; i<=26; i++)
                cout << (CHAR)(data[i] >> 8) << (CHAR)(data[i] & 0x00FF);
            
            cout << endl << "Size of memory: ";
            cout << (long double)(((ULONG *)data)[30]) * 512 / 1024 / 1024 / 1024 << " Gb";

			cout << endl << "Interface: ";
            cout << (CHAR)(data[15] >> 8) << (CHAR)(data[15] & 0x00FF);
			cout << (data[15] & 0x80 ? "ATAPI" : "ATA");

			cout << endl << "Multiword DMA support: ";
			cout << (data[63] & 0x1 ? "\n   MWDMA 0" : "");
            cout << (data[63] & 0x2 ? "\n   MWDMA 1" : "");
            cout << (data[63] & 0x4 ? "\n   MWDMA 2" : "");
			cout << (data[63] & 0x8 ? "\n   MWDMA 3" : "");
            cout << (data[63] & 0x10 ? "\n   MWDMA 4" : "");
            cout << (data[63] & 0x20 ? "\n   MWDMA 5" : "");
            cout << (data[63] & 0x40 ? "\n   MWDMA 6" : "");
            cout << (data[63] & 0x80 ? "\n   MWDMA 7" : "");
            
            cout << endl << "PIO modes: ";
            cout << (data[64] & 0x1 ? "\n   PIO 3" : "");
            cout << (data[64] & 0x2 ? "\n   PIO 4" : "");

			cout << endl << "ATA support: ";
            cout << (data[80] & 0x2 ? "\n   ATA 1" : "");
            cout << (data[80] & 0x4 ? "\n   ATA 2" : "");
            cout << (data[80] & 0x8 ? "\n   ATA 3" : "");
            cout << (data[80] & 0x10 ? "\n   ATA 4" : "");
            cout << (data[80] & 0x20 ? "\n   ATA 5" : "");
            cout << (data[80] & 0x40 ? "\n   ATA 6" : "");
            cout << (data[80] & 0x80 ? "\n   ATA 7" : "");

			cout << endl;
        }

		else cout << endl << "No device found" << endl;
	}

	cout << endl;

    system("pause");

    return 0;
}

BOOL GetDeviceInfo(int channel, int device)
{
    BYTE state;
    BOOL flag = false;
    WORD port, value;
    BYTE commands[] = {
		IDENTIFY_DEVICE,
        IDENTIFY_PACKET_DEVICE,
    };

    for (int i=0; i<sizeof(commands); i++) {
		//check, whether the device is occupied
		port = AS[channel];
        do {
            __asm {
                mov DX, port
                in AL, DX
                mov state, AL
            }
        }
        while (state & 0x80);    //0x80 = 10000000, bit 7 - BSY
        
		//choose a device: bit 4 - 0/1 (Master/Slave)
		port = DH[channel];
		value = device << 4;
        __asm {
			mov DX, port
			in AX, DX
			and AX, 0xEF    //reset bit 4
			or AX, value
			out DX, AX
        }

		//check, whether the device is ready for commands
        for (int i=0; i<1000; i++) {
			port = AS[channel];
            __asm {
                mov DX, port
                in AL, DX
                mov state, AL
            }
            
            if (state & 0x40)    //0x40 - 01000000, bit 6 - DRDY
                flag = true;
        }
        
        if(!flag) return false;

		//write the command into CR
		port = CR[channel];
		state = commands[i];
        __asm {
            mov DX, port
            mov AL, state
            out DX, AL
        }

		//check, whether the device is occupied
        do {
			port = AS[channel];
            __asm {
                mov DX, port
                in AL, DX
                mov state, AL
            }
        }
        while (state & 0x80);
        
		//check, whether the device is ready for word of data transmission (only for ATAPI)
		port = AS[channel];
        __asm {
            mov DX, port
            in AL, DX
            mov state, AL
        }

		if (!(state & 0x08)) {    //0x08 - 00001000, bit 3 - DRQ
            if (i == 1)    //for channel 1 and IDENTIFY_PACKET_DEVICE
                return false;

            continue;
        } 
        else
            break;
	}

	//read data
    for (int i=0; i<256; i++) {
		port = DR[channel];

        __asm {
            mov DX, port
            in AX, DX
            mov value, AX
        }
        
        data[i] = value;
    }

    return true;
}
