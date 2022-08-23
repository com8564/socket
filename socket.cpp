#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdio.h>
#include <string>
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;
#pragma comment(lib,"ws2_32.lib")

void parseUrl(char* mUrl, string& serverName, string& port, string& filepath);
SOCKET connectToServer(char* szServerName, WORD portNum=80);
bool saveFile(char* webSiteHTML, string filepath, int nDataLength);

int main(void) {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed.\n";
        system("pause");
    }

    SOCKET Socket;
    SOCKADDR_IN SockAddr;
    int lineCount = 0;
    int rowCount = 0;
    char buffer[10000];
    int i = 0;
    int nDataLength;
    unsigned short portNum=0;
    string website_HTML;
    string serverName, filepath, port="";

    // website url
    char url[1000];

    printf("URL : ");
    scanf_s("%s", url, (unsigned int)sizeof(url));

    parseUrl(url, serverName, port, filepath);
    string temp = port.substr(port.find(':')+1);
    portNum = stoul(temp, nullptr, 0);

    //HTTP GET
    string get_http = "GET " + filepath + " HTTP/1.1\r\nHost: " + serverName + port + "\r\nConnection: keep-alive\r\n\r\n";
    Socket = connectToServer((char*)serverName.c_str(), portNum);
    if (Socket == NULL) {
        printf("connect error");
    }

    // send GET / HTTP
    send(Socket, get_http.c_str(), get_http.length(), 0);

    // recieve html
    nDataLength = recv(Socket, buffer, 10000, 0);
    closesocket(Socket);
    WSACleanup();
    
    // Display HTML source 
    bool success = saveFile(buffer, filepath, nDataLength);
    if (success == false) { printf("save fail"); }

    // pause
    printf("\nPress ANY key to close.\n");
    cin.ignore(); cin.get();

    return 0;
}

void parseUrl(char* mUrl, string& serverName, string& port, string& filepath) {
    string::size_type n;
    string url = mUrl;
    
    if (url.substr(0, 7) == "http://")
        url.erase(0, 7);
    
    if (url.substr(0, 8) == "https://")
        url.erase(0, 8);
    
    n = url.find('/');

    if (n != string::npos)
    {
        serverName = url.substr(0, n);

        if (serverName.find(':') != string::npos) {
            string temp = serverName;
            serverName = temp.substr(0, serverName.find(':'));
            port = temp.substr(temp.find(':'));
        }
        else { port = ":80"; }

        filepath = url.substr(n);
    }
    
    else
    {
        serverName = url;

        if (serverName.find(':') != string::npos) {
            string temp = serverName;
            serverName = temp.substr(0, serverName.find(':'));
            port = temp.substr(temp.find(':'));
        }
        else { port = ":80"; }

        filepath = "/";
    }
}

SOCKET connectToServer(char* szServerName, WORD portNum)
{
    struct hostent* host;
    unsigned int addr;
    struct sockaddr_in server;
    SOCKET conn;

    conn = socket(AF_INET, SOCK_STREAM, 0);

    if (conn == INVALID_SOCKET) { return NULL; }

    server.sin_addr.s_addr = inet_addr(szServerName);
    server.sin_family = AF_INET;
    server.sin_port = htons(portNum);

    if (connect(conn, (SOCKADDR*)&server, sizeof(server))){

        closesocket(conn);
        return NULL;
    }

    return conn;
}

bool saveFile(char* buffer, string filepath, int nDataLength) {
    std::string subtext;
    string filename = filepath.substr(filepath.rfind('/')+1);

    std::ofstream out(filename, std::ios::binary | std::ios::out);
    char* ptr = strstr(buffer, "\r\n\r\n");
    int len = 0;
    len = ptr - buffer + 4;

    for (int i = len; i < nDataLength; i++) {
        out << buffer[i];
    }

    out.close();
    return true;
}