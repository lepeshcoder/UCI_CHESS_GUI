#define WIN32_LEAN_AND_MEAN 

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>

//��������� ������ ��� ������ ������� ��������� � ����� ������
typedef struct
{
    //����������� �������
    HANDLE  hePipeStart;
    HANDLE  hePipeReply;
    //����������� ������
    HANDLE  child_in;
    HANDLE  parent_out;
    HANDLE  child_out;
    HANDLE  parent_in;
    //������
    char    read[1024];     //����� ��� ������ ������
    char    send[1024];     //������������ ��������
    std::string Response;
}TheadParms;

//���� ������
BOOL FAR PASCAL PipesProc(LPSTR lpData)
{
    TheadParms* ptp = (TheadParms*)lpData;     //���������� �������� - ����� ����� ������
    DWORD       num;                            //���������� ���������/���������� ����

    while (1)                                   //����������� ����
    {
        WaitForSingleObject(ptp->hePipeStart, INFINITE);   //���� ���������� �������

        //�������� ������
        WriteFile(ptp->child_in, ptp->send, lstrlenA(ptp->send), &num, NULL);   //�������� ������� ������
        std::cout << "������ ��������:\n " << ptp->send << "\n";                            //������� ���������� ������ �����

        //������ ������
        
        BOOL bsuccses;
        do {
            bsuccses = ReadFile(ptp->child_out, ptp->read, 1023, &num, NULL);                  //������ ����� � �����                                                       //� ������� ������� ����������
           // std::cout << "������ ��������:\n " << ptp->read<<"\n";
            for (int i = 0; i < 1023; i++)
            {
                ptp->Response.push_back(ptp->read[i]);
            }
            ZeroMemory(ptp->read, 1024);
            std::cout << "\nnum = " << num << std::endl;
            if (num < 1023) break;
        } while ( num > 0 );
        SetEvent(ptp->hePipeReply);
        std::cout << "������� �����������";
       
        
    }
}

int main()
{
    setlocale(0, "");

    SECURITY_ATTRIBUTES sa;//�������� ������ ������
    sa.lpSecurityDescriptor = NULL; //������ �� ���������
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;//��������� ������������ ������������

//��������� ������
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX | SEM_NOALIGNMENTFAULTEXCEPT);

    TheadParms tp = {};         //��������� ����� ������

    //������� ��������� ������
    CreatePipe(&tp.parent_out, &tp.child_in, &sa, 0); //����� ��� stdin ��������� ��������
    CreatePipe(&tp.child_out, &tp.parent_in, &sa, 0); //����� ��� stdout ��������� ��������

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    //  si.wShowWindow = SW_HIDE;//�������� ���� ��������� ��������
        //��������� �����������
    si.hStdOutput = tp.parent_in;
    si.hStdError = tp.parent_in;
    si.hStdInput = tp.parent_out;

    //������ �������� �������
    WCHAR ModuleName[] = L"C:\\Users\\user\\Desktop\\stockfish\\stockfish.exe";
    PROCESS_INFORMATION pi;
    CreateProcess(ModuleName, NULL, NULL, NULL, TRUE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);                // ����� ����� ����������� � �������������� ������ �������� � ��� ���������� ������

    //������� ������� ��� �������������
    tp.hePipeStart = CreateEvent(NULL,   // no security
        FALSE,  // explicit auto reset req
        FALSE,  // initial event reset
        NULL);  // no name  

    tp.hePipeReply = CreateEvent(NULL,   // no security
        FALSE,  // explicit auto reset req
        FALSE,  // initial event reset
        NULL);  // no name


//������� ���� ������
    DWORD dwPipesThreadID;
    HANDLE hPipesThread = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0,
        (LPTHREAD_START_ROUTINE)PipesProc, &tp, 0, &dwPipesThreadID);  //���������� �������� ����� ��������� ������

    std::string arr[] = { "isready\n","uci\n","go\n","stop\n" };
    int value = 0;

    while (1)
    {

        std::string a;
        std::cin >> a;
        a += '\n';


        wsprintfA(tp.send,a.c_str());        //��������� �������� ������ � �������� '\n' � �����
       


        SetEvent(tp.hePipeStart);                   //���� ������� ��� ����� ������
       
       
        switch (WaitForSingleObject(tp.hePipeReply, 5000))     //���� ����� 1 ���
        {
        case WAIT_OBJECT_0:                                 //���� �����
            std::cout <<"\n�����:\n" << tp.Response << std::endl;
            tp.Response = "";
            break;
            

        case WAIT_TIMEOUT:                                  //���, �������
            std::cout << "Time out!" << std::endl;
        }
        
        //if (_getch() == 0x1b)                                   //���� ������� �� �������
        //    break;                                              //����� �� Esc
    }

    //��������� ���, ��� �������������...
    CloseHandle(tp.child_in);//��������� ����� stdin ��������� ��������
    CloseHandle(tp.parent_out);//��������� ������������ ����������� ����
    CloseHandle(tp.parent_in);
    CloseHandle(tp.child_out);

    CloseHandle(tp.hePipeReply);
    CloseHandle(tp.hePipeStart);
    TerminateThread(hPipesThread, 0);

    TerminateProcess(pi.hProcess, 0);//������� �������� �������
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}