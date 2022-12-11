#include "IPC.h"

IPC::IPC()
{
    SECURITY_ATTRIBUTES sa;//�������� ������ ������
    sa.lpSecurityDescriptor = NULL; //������ �� ���������
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;//��������� ������������ ������������

    //������� ��������� ������
    CreatePipe(&parent_out, &child_in, &sa, 0); //����� ��� stdin ��������� ��������
    CreatePipe(&child_out, &parent_in, &sa, 0); //����� ��� stdout ��������� ��������

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;//�������� ���� ��������� ��������
        //��������� �����������
    si.hStdOutput = parent_in;
    si.hStdError = parent_in;
    si.hStdInput = parent_out;

    //������ �������� �������
    WCHAR ModuleName[] = L"C:\\Users\\user\\Desktop\\������\\stockfish\\stockfish.exe";
    BOOL bsucces = CreateProcess(ModuleName, NULL, NULL, NULL, TRUE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);  

}

void IPC::SendRequest(string message)
{
    // ���������� ���������� ����
    DWORD num;
    // ���������� � ����� ���������
    wsprintfA(send, message.c_str());
    // ����� � ����
    WriteFile(child_in, send, lstrlenA(send), &num, NULL);
}

string IPC::ReceiveResponse()
{
    // ������ ������
    string response;
    // ���������� ����������� ����
    DWORD num;
    BOOL bsuccses;
    do {
        bsuccses = ReadFile(child_out, read, 1023, &num, NULL);                  //������ ����� � �����                                                       //� ������� ������� ����������
        for (int i = 0; i < 1023; i++)
            response.push_back(read[i]);      
        ZeroMemory(read, 1024);
        if (num < 1023) break;
    } while (num > 0);
    return response;
}

IPC::~IPC()
{
    CloseHandle(child_in);//��������� ����� stdin ��������� ��������
    CloseHandle(parent_out);//��������� ������������ ����������� ����
    CloseHandle(parent_in);
    CloseHandle(child_out);

    CloseHandle(hePipeReply);
    CloseHandle(hePipeStart);

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}
