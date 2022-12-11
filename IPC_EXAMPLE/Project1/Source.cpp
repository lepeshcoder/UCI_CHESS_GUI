#define WIN32_LEAN_AND_MEAN 

#include <windows.h>
#include <iostream>
#include <conio.h>
#include <vector>

//структура данных для общего доступа основного и треда пайпов
typedef struct
{
    //дескрипторы событий
    HANDLE  hePipeStart;
    HANDLE  hePipeReply;
    //дескрипторы пайпов
    HANDLE  child_in;
    HANDLE  parent_out;
    HANDLE  child_out;
    HANDLE  parent_in;
    //буфера
    char    read[1024];     //буфер для чтения данных
    char    send[1024];     //передаваемое значение
    std::string Response;
}TheadParms;

//тред пайпов
BOOL FAR PASCAL PipesProc(LPSTR lpData)
{
    TheadParms* ptp = (TheadParms*)lpData;     //переданный параметр - адрес общих данных
    DWORD       num;                            //количество переданых/прочитаных байт

    while (1)                                   //бесконечный цикл
    {
        WaitForSingleObject(ptp->hePipeStart, INFINITE);   //ждем бесконечно событие

        //передача данных
        WriteFile(ptp->child_in, ptp->send, lstrlenA(ptp->send), &num, NULL);   //передаем готовые данные
        std::cout << "Данные переданы:\n " << ptp->send << "\n";                            //выведем переданные данные здесь

        //чтение данных
        
        BOOL bsuccses;
        do {
            bsuccses = ReadFile(ptp->child_out, ptp->read, 1023, &num, NULL);                  //читаем ответ в буфер                                                       //и взводим событие готовности
           // std::cout << "Данные получены:\n " << ptp->read<<"\n";
            for (int i = 0; i < 1023; i++)
            {
                ptp->Response.push_back(ptp->read[i]);
            }
            ZeroMemory(ptp->read, 1024);
            std::cout << "\nnum = " << num << std::endl;
            if (num < 1023) break;
        } while ( num > 0 );
        SetEvent(ptp->hePipeReply);
        std::cout << "Событие выставилось";
       
        
    }
}

int main()
{
    setlocale(0, "");

    SECURITY_ATTRIBUTES sa;//атрибуты защиты канала
    sa.lpSecurityDescriptor = NULL; //защита по умолчанию
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = true;//разрешаем наследование дескрипторов

//обработка ошибок
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX | SEM_NOALIGNMENTFAULTEXCEPT);

    TheadParms tp = {};         //структура общих данных

    //создаем анонимные каналы
    CreatePipe(&tp.parent_out, &tp.child_in, &sa, 0); //канал для stdin дочернего процесса
    CreatePipe(&tp.child_out, &tp.parent_in, &sa, 0); //канал для stdout дочернего процесса

    STARTUPINFO si;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    //  si.wShowWindow = SW_HIDE;//скрываем окно дочернего процесса
        //подменяем дескрипторы
    si.hStdOutput = tp.parent_in;
    si.hStdError = tp.parent_in;
    si.hStdInput = tp.parent_out;

    //Создаём дочерний процесс
    WCHAR ModuleName[] = L"C:\\Users\\user\\Desktop\\stockfish\\stockfish.exe";
    PROCESS_INFORMATION pi;
    CreateProcess(ModuleName, NULL, NULL, NULL, TRUE, ABOVE_NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);                // здесь будут дескрипторы и идентификаторы нового процесса и его первичного потока

    //Создаем события для синхнонизации
    tp.hePipeStart = CreateEvent(NULL,   // no security
        FALSE,  // explicit auto reset req
        FALSE,  // initial event reset
        NULL);  // no name  

    tp.hePipeReply = CreateEvent(NULL,   // no security
        FALSE,  // explicit auto reset req
        FALSE,  // initial event reset
        NULL);  // no name


//Создаем тред пайпов
    DWORD dwPipesThreadID;
    HANDLE hPipesThread = CreateThread((LPSECURITY_ATTRIBUTES)NULL, 0,
        (LPTHREAD_START_ROUTINE)PipesProc, &tp, 0, &dwPipesThreadID);  //параметром передаем адрес структуры данных

    std::string arr[] = { "isready\n","uci\n","go\n","stop\n" };
    int value = 0;

    while (1)
    {

        std::string a;
        std::cin >> a;
        a += '\n';


        wsprintfA(tp.send,a.c_str());        //формируем числовую строку с символом '\n' в конце
       


        SetEvent(tp.hePipeStart);                   //даем отмажку для треда пайпов
       
       
        switch (WaitForSingleObject(tp.hePipeReply, 5000))     //ждем ответ 1 сек
        {
        case WAIT_OBJECT_0:                                 //есть ответ
            std::cout <<"\nОтвет:\n" << tp.Response << std::endl;
            tp.Response = "";
            break;
            

        case WAIT_TIMEOUT:                                  //увы, таймаут
            std::cout << "Time out!" << std::endl;
        }
        
        //if (_getch() == 0x1b)                                   //ждем нажатие на клавишу
        //    break;                                              //выход по Esc
    }

    //закрываем все, что понаоткрывали...
    CloseHandle(tp.child_in);//закрываем канал stdin дочернего процесса
    CloseHandle(tp.parent_out);//закрываем родительский принимающий пайп
    CloseHandle(tp.parent_in);
    CloseHandle(tp.child_out);

    CloseHandle(tp.hePipeReply);
    CloseHandle(tp.hePipeStart);
    TerminateThread(hPipesThread, 0);

    TerminateProcess(pi.hProcess, 0);//убиваем дочерний процесс
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}