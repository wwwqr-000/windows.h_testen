#include <iostream>
#include <vector>
#include <thread>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

//Config
std::string server_ip = "192.168.0.102";
int server_port = 12345;
int clientLimit = 4;
//
bool serverIsOn = false;

std::vector<SOCKET> clientSockList;
std::vector<std::thread> clientHandler;
std::vector<std::thread> listenerThread;

bool wsaKill(SOCKET serverSocket) {
    closesocket(serverSocket);
    WSACleanup();
    return true;
}
void threadKill() {
    for (auto& thread : clientHandler) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    for (auto& thread : listenerThread) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

int clientIndexNoConst(SOCKET clientSocket) {//Initial clientSocket index for data vectors. Not always the same, so no constant var...
    int tmpCount = -1;
    for (auto& client : clientSockList) {
        ++tmpCount;
        if (clientSocket == client) {
            break;
        }
    }
    return tmpCount;
}

void console(SOCKET serverSocket) {
    while (serverIsOn) {
        std::string input;
        std::cin >> input;
        if (input == "exit") {
            return;
        }
    }
}

void clienthandlerFunc(SOCKET clientSocket, sockaddr_in clientAddr) {
    std::string clientUserName = "";
    if (clientSockList.size() == clientLimit) {
        std::string tmpFullMsg = "%serverIsFull%";
        send(clientSocket, tmpFullMsg.c_str(), tmpFullMsg.length(), 0);
        closesocket(clientSocket);
        return;
    }
    std::cout << "Connection accepted from " << inet_ntoa(clientAddr.sin_addr) << ".\n";
    clientSockList.push_back(clientSocket);
    int tmpClientIndex = clientIndexNoConst(clientSocket);
    std::string okJoinMsg = "%welcomeToServer%";
    send(clientSocket, okJoinMsg.c_str(), okJoinMsg.length(), 0);
    while (serverIsOn) {
        int recBuff = 3000;
        char buffer[recBuff];
        int bytesRead = recv(clientSocket, buffer, recBuff, 0);
        if (bytesRead > 0) {
            std::string msgFromClient = std::string(buffer, bytesRead);
            std::cout << msgFromClient << "\n";
        }
        else {
            std::cout << clientUserName << "left the server.\n";
            break;
        }
    }
}

void listener(SOCKET serverSocket) {//Listening for incoming client connections...
    while (serverIsOn) {
        SOCKET clientSocket = accept(serverSocket, 0, 0);//Wait for incoming client...
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "An invalid client socket tried to connect to the server.\n";
            continue;
        }
        sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        getpeername(clientSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        clientHandler.emplace_back([clientSocket, clientAddr]() { clienthandlerFunc(clientSocket, clientAddr); });
    }
    wsaKill(serverSocket);
}


int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cout << "Could not create winsock setup. Press enter to exit.\n";
        system("set /p end=");
        return 1;
    }
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cout << "This serverSocket is an invalid Socket type. Press enter to exit.\n";
        system("set /p end=");
        return 2;
    }
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(server_ip.c_str());
    serverAddr.sin_port = htons(server_port);
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == INVALID_SOCKET) {
        std::cout << "Could not bind the serverSocket. Press enter to exit.\n";
        system("set /p end=");
        return 0;
    }
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        std::cout << "Could not start listening to connections. Press enter to exit.\n";
        system("set /p end=");
        return 0;
    }
    serverIsOn = true;
    std::cout << "Success: Server is running on " << server_ip << ":" << server_port << "\n";
    console(serverSocket);
    threadKill();
    wsaKill(serverSocket);
    return 0;
}
