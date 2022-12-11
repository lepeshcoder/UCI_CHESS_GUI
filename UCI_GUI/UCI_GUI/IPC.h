#pragma once
#include"framework.h"


class IPC
{
private:
   // Дочерний процесс
   PROCESS_INFORMATION pi;
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
   

public:

	IPC();

	void SendRequest(string message);

	string ReceiveResponse();

	~IPC();
};

