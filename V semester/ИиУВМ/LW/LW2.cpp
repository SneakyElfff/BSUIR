// LW2.cpp : Defines the entry point for the console application.

#include <iostream>
#include "hexioctrl.h"
#include "(PCI_DEVS)pci_codes.h"

int main()
{
	ALLOW_IO_OPERATIONS;

	unsigned long config_adr = 1, reg_contents, DeviceID, VendorID;

	for (int bus=0; bus<256; bus++)
		for (int device=0; device<32; device++)
			for (int function=0; function<8; function++) {
				config_adr = (1U << 31) | (bus << 16) | (device << 11) | (function << 8) | 0x00;

				reg_contents = 0;

				__asm
				{
					mov eax, config_adr
					mov dx, 0CF8h
					out dx, eax    //write into port 0CF8h (CONFIG_ADDRESS) device register's address
					mov dx, 0CFCh
					in eax, dx    //read register's contents from 0CFh (CONFIG_DATA)
					mov reg_contents, eax
				}

				//_outp(0xCF8, config_adr);
				//reg_contents = _inp(0xCFC);

				if (reg_contents == -1)    //if there is no such address
					continue;
				
				//from 0x00
				DeviceID = reg_contents >> 16;
				VendorID = reg_contents - (DeviceID << 16);

				cout << "PCI bus:   " << bus << "Device:   " << device << "Function:   " << function << "DeviceID   " << "VendorID   " << endl;
				cout << bus << "          " << device << "         " << function << "           " << hex << DeviceID << "        " << hex << VendorID << endl << endl;
					
				//name and description of the device
				for(int i=0; i<PCI_DEVTABLE_LEN; i++)
					if(PciDevTable[i].VenId == VendorID && PciDevTable[i].DevId == DeviceID)			
						cout << PciDevTable[i].Chip << ", " << PciDevTable[i].ChipDesc << endl;

				//name of the vendor
				for(int i=0; i<PCI_VENTABLE_LEN; i++)
					if(PciVenTable[i].VenId == VendorID)
						cout << PciVenTable[i].VenFull << endl;

				cout << "---------------------------------------------------------------" << endl;		
			}
	
	cout << endl;
	system("pause");

	return 0;
}
