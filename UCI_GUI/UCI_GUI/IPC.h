#pragma once
#include"framework.h"


class IPC
{
private:


public:

	IPC();

	void SendRequest(string message);

	string ReceiveResponse();

	~IPC();
};

