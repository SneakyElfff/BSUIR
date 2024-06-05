//#include <WinSock2.h>
//#include <windows.h>
//#include <ws2bth.h>
//#include <iostream>
//#include <fstream>
//
//#pragma comment(lib, "ws2_32.lib")
//
//using namespace std;
//
//int main() {
//    WSADATA wsaData;
//    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
//        cerr << "Failed to initialize WinSock." << endl;
//
//        return -1;
//    }
//
//    SOCKET serverSocket = socket(AF_BTH, SOCK_STREAM, BTHPROTO_RFCOMM);
//    if (serverSocket == INVALID_SOCKET) {
//        cerr << "Failed to create a Bluetooth socket." << endl;
//
//        WSACleanup();
//        return -1;
//    }
//
//    SOCKADDR_BTH serverAddress = { 0 };
//    serverAddress.addressFamily = AF_BTH;
//    serverAddress.port = BT_PORT_ANY;
//
//    if (bind(serverSocket, (SOCKADDR*)&serverAddress, sizeof(serverAddress)) == SOCKET_ERROR) {
//        cerr << "Failed to bind a socket." << endl;
//
//        closesocket(serverSocket);
//        WSACleanup();
//
//        return -1;
//    }
//
//    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
//        cerr << "Failed to listen on the socket." << endl;
//
//        closesocket(serverSocket);
//        WSACleanup();
//
//        return 1;
//    }
//
//    cout << "Waiting for incoming Bluetooth connection..." << endl;
//
//    SOCKADDR_BTH clientAddress = { 0 };
//    int clientAddressSize = sizeof(clientAddress);
//    SOCKET clientSocket = accept(serverSocket, (SOCKADDR*)&clientAddress, &clientAddressSize);
//
//    if (clientSocket == INVALID_SOCKET) {
//        cerr << "Failed to accept incoming connection." << endl;
//
//        closesocket(serverSocket);
//        WSACleanup();
//
//        return 1;
//    }
//
//    cout << "Bluetooth connection established." << endl;
//
//    const int bufferSize = 1024;
//    char buffer[bufferSize];
//
//    ofstream outputFile("received_file.txt", ios::binary);
//
//    while (true) {
//        int bytesRead = recv(clientSocket, buffer, bufferSize, 0);
//
//        if (bytesRead <= 0)
//            break;
//
//        outputFile.write(buffer, bytesRead);
//    }
//
//    outputFile.close();
//
//    closesocket(clientSocket);
//    closesocket(serverSocket);
//    WSACleanup();
//
//    cout << "File received and saved as \"received_file.txt\"." << endl;
//
//    return 0;
//}