#include <windows.h>
#include <iostream>

#define PIPE_NAME "\\\\192.168.100.100\\pipe\\MyPipe"
#define BUFFER_SIZE 1024

HANDLE eventHandle;

DWORD WINAPI SendThread(LPVOID lpParam)
{
    // イベントの待機
    WaitForSingleObject(eventHandle, INFINITE);

    // サーバーへの接続
    HANDLE hPipe = CreateFileA(PIPE_NAME, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hPipe == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to connect to the server. Error Code: " << GetLastError() << std::endl;
        return 1;
    }

    // メッセージの送信
    const char* message = "Hello, Server!";
    DWORD bytesWritten;
    if (!WriteFile(hPipe, message, strlen(message) + 1, &bytesWritten, NULL))
    {
        std::cout << "Failed to send message to the server. Error Code: " << GetLastError() << std::endl;
        CloseHandle(hPipe);
        return 1;
    }

    std::cout << "Message sent to the server." << std::endl;

    // リソースの解放
    CloseHandle(hPipe);

    return 0;
}

int main()
{
    // イベントの作成
    eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (eventHandle == NULL)
    {
        std::cout << "Failed to create event. Error Code: " << GetLastError() << std::endl;
        return 1;
    }

    // サーバーへのメッセージ送信を行うスレッドの作成
    HANDLE hThread = CreateThread(NULL, 0, SendThread, NULL, 0, NULL);
    if (hThread == NULL)
    {
        std::cout << "Failed to create thread. Error Code: " << GetLastError() << std::endl;
        return 1;
    }

    // イベントのシグナル
    std::cout << "Signaling the event." << std::endl;
    SetEvent(eventHandle);

    // スレッドの終了待機
    WaitForSingleObject(hThread, INFINITE);

    // リソースの解放
    CloseHandle(eventHandle);
    CloseHandle(hThread);

    return 0;
}
