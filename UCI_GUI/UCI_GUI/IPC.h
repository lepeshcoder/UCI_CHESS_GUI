#pragma once
#include"framework.h"


class IPC
{
private:
   // �������� �������
   PROCESS_INFORMATION pi;
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
   

public:

	IPC();

	void SendRequest(string message);

	string ReceiveResponse();

	~IPC();
};

