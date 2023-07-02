#include <windows.h>
#include <iostream>

#define PIPE_NAME "\\\\.\\pipe\\MyPipe"
#define BUFFER_SIZE 1024

int main()
{
    // パイプの作成
    HANDLE hPipe = CreateNamedPipeA(
        PIPE_NAME,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        BUFFER_SIZE,
        BUFFER_SIZE,
        0,
        NULL);
    if (hPipe == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to create named pipe. Error Code: " << GetLastError() << std::endl;
        return 1;
    }

    // クライアントの接続待機
    std::cout << "Waiting for client connection..." << std::endl;
    BOOL isConnected = ConnectNamedPipe(hPipe, NULL);
    if (!isConnected)
    {
        std::cout << "Failed to establish client connection. Error Code: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    std::cout << "Client connected." << std::endl;

    // メッセージの受信
    char buffer[BUFFER_SIZE];
    DWORD bytesRead;
    if (ReadFile(hPipe, buffer, sizeof(buffer), &bytesRead, NULL))
    {
        std::cout << "Received message from client: " << buffer << std::endl;

        // 応答の送信
        const char* response = "Hello, Client!";
        DWORD bytesWritten;
        if (!WriteFile(hPipe, response, strlen(response) + 1, &bytesWritten, NULL))
        {
            std::cout << "Failed to send response to client. Error Code: " << GetLastError() << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to receive message from client. Error Code: " << GetLastError() << std::endl;
    }

    // リソースの解放
    FlushFileBuffers(hPipe);
    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);

    return 0;
}
