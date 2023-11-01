#include "Utils.h"
#include <iostream>

int main() {
	DWORD ProcessId = GetProcId(L"ac_client.exe");
	
	if (ProcessId) {
		std::cout << "Found the process" << std::endl;
	}

	return 0;
}