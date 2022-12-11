#include "IPC.h"

IPC::IPC()
{
    SECURITY_ATTRIBUTES sa;//атрибуты защиты канала
    sa.lpSecurityDescriptor = NULL; //защита по умолчанию
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;//разрешаем наследование дескрипторов

    //создаем анонимные каналы
    CreatePipe(&parent_out, &child_in, &sa, 0); //канал для stdin дочернего процесса
    CreatePipe(&child_out, &parent_in, &sa, 0); //канал для stdout дочернего процесса

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;//скрываем окно дочернего процесса
        //подменяем дескрипторы
    si.hStdOutput = parent_in;
    si.hStdError = parent_in;
    si.hStdInput = parent_out;

    //Создаём дочерний процесс
    WCHAR ModuleName[] = L"C:\\Users\\user\\Desktop\\Курсач\\stockfish\\stockfish.exe";
    BOOL bsucces = CreateProcess(ModuleName, NULL, NULL, NULL, TRUE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);  

}

void IPC::SendRequest(string message)
{
    // количество записанных байт
    DWORD num;
    // записываем в буфер сообщение
    wsprintfA(send, message.c_str());
    // пишем в пайп
    WriteFile(child_in, send, lstrlenA(send), &num, NULL);
}

string IPC::ReceiveResponse()
{
    // Строка ответа
    string response;
    // количество прочитанных байт
    DWORD num;
    BOOL bsuccses;
    do {
        bsuccses = ReadFile(child_out, read, 1023, &num, NULL);                  //читаем ответ в буфер                                                       //и взводим событие готовности
        for (int i = 0; i < 1023; i++)
            response.push_back(read[i]);      
        ZeroMemory(read, 1024);
        if (num < 1023) break;
    } while (num > 0);
    return response;
}

IPC::~IPC()
{
    CloseHandle(child_in);//закрываем канал stdin дочернего процесса
    CloseHandle(parent_out);//закрываем родительский принимающий пайп
    CloseHandle(parent_in);
    CloseHandle(child_out);

    CloseHandle(hePipeReply);
    CloseHandle(hePipeStart);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}
