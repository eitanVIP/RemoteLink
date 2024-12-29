#pragma once
#include <string>
#include <WinSock2.h>

namespace Application {
	int Start();
	void Update();
	void Log(std::string msg, BOOL isServer = 2);
	void Close();
}
