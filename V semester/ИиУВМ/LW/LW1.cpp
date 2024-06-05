// LW1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <locale.h>
#include <windows.h>
#include <string.h>
#include <powrprof.h>

using namespace std;

void timeConverter(DWORD time)
{
    int remaining = time;
    int hours = remaining / 3600;
    int mins = (remaining % 3600) / 60;
    int secs = remaining - (3600 * hours + 60 * mins);

    cout << hours << " hours, " << mins << " minutes and " << secs << " seconds";
}

int main()
{
    setlocale(LC_ALL, "Russian");

    SYSTEM_POWER_STATUS battery;
    char choice, buffer[128];
    string result = "";

    while (1)
    {
        cout << endl << "Choose an option: " << endl << "                 1. Get info about battery;" << endl << "                 2. Enter a sleep state;" << endl << "                 3. Enter hibernation" << endl << "                 4. Finish." << endl;
        cin >> choice;

        if (choice == '1')
        {
            if (GetSystemPowerStatus(&battery))
            {
                cout << endl << "Type of power supply: ";
                if (battery.ACLineStatus == 255) cout << "not defined";
                else if (battery.ACLineStatus == 0) cout << "battery";
                else if (battery.ACLineStatus == 1) cout << "plugged into the power outlet";

                cout << endl << "Battery status: ";
                if (battery.BatteryLifePercent != 255)
                    cout << (int)battery.BatteryLifePercent << "%";
                else cout << "not defined";

                cout << endl << "Powersaving mode: ";
                if (battery.SystemStatusFlag) cout << "battery saver on. Save energy where possible.";
                else if (!battery.SystemStatusFlag) cout << "battery saver is off";

                if (battery.ACLineStatus == 0)
                {
                    cout << endl << "Remaining time before power-off: ";
                    timeConverter(battery.BatteryLifeTime);

                    cout << endl << "Remaining time after charging: ";
                    timeConverter(battery.BatteryFullLifeTime);
                }
                else cout << endl << "The computer is charging";

            }

            cout << endl << "Battery type: ";

            FILE *pipe = _popen("wmic path Win32_Battery get Chemistry /value", "r");    //выполняется в командной строке 
            if (!pipe)
            {
                cerr << "Error" << endl;
                return -1;
            }

            while (fgets(buffer, sizeof(buffer), pipe) != NULL)
                result += buffer;

            _pclose(pipe);

            size_t pos = result.find("Chemistry=");
            if (pos != string::npos)    //npos - max size_t => not found
            {
                string battery_type = result.substr(pos + 10);

                char type = battery_type[0];
                switch (type)
                {
                case '1':
                    cout << "lead acid";
                    break;

                case '2':
                    cout << "lithium ion";
                    break;

                case '3':
                    cout << "lithium polymer";
                    break;

                case '4':
                    cout << "lithium iron phosphate";
                    break;

                case '5':
                    cout << "nickel metal hydride";
                    break;

                default:
                    cout << "unknown value";
                }
            }
            else
                cerr << "Not found" << endl;
        }

        else if (choice == '2')
        {
            if (SetSuspendState(false, true, false))
                cout << endl << "The system is in sleep mode now.";

            else cout << endl << "Failed to suspend the system.";
        }

        else if (choice == '3')
        {
            if (IsPwrHibernateAllowed())
            {
                if (SetSuspendState(true, true, false))
                    cout << endl << "The system is in hibernation mode now.";

                else cout << endl << "Failed to hibernate the system.";
            }
            else cout << endl << "This system can't be hibernated.";
        }

        else if (choice == '4') break;

        else cout << endl << "Wrong parameter";

        cout << endl;

    }

    return 0;
}