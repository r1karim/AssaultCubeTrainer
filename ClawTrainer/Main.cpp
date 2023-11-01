#include "Utils.h"
#include <iostream>



int main() {
	DWORD ProcessId = GetProcId(L"ac_client.exe");
	if (!ProcessId) { std::cout << "Process not found" << std::endl;  abort(); }
	uintptr_t modBaseAddr = GetModuleBaseAddress(ProcessId, L"ac_client.exe");
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);

	Player player(ProcessId, modBaseAddr, hProcess);

	player.setHealth(501);
	player.setArmour(69);
	std::cout << "Player health: " << player.getPlayerHealth() << std::endl;


	return 0;
}