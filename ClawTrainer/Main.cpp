#include "Utils.h"
#include <iostream>



int main() {
	DWORD ProcessId = GetProcId(L"ac_client.exe");
	bool bHealth=false, bAmmo = false, bRecoil = false;
	
	if (!ProcessId) { std::cout << "Process not found" << std::endl;  abort(); }
	uintptr_t modBaseAddr = GetModuleBaseAddress(ProcessId, L"ac_client.exe");
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);

	Player player(ProcessId, modBaseAddr, hProcess);
	Enemy test(hProcess, modBaseAddr);
	//Enemy Array[8];

	player.setHealth(501);
	player.setArmour(69);
	player.setCurrentWeaponAmmo(69);
	std::cout << "Player health: " << player.getPlayerHealth() << std::endl;
	// {1100f8+0+0} {110D90+4+5} {1100f8+30+0}
	DWORD dwExit = 0;
	
	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE) {
		if (GetAsyncKeyState(VK_NUMPAD1) & 1) {
			bHealth = !bHealth;
			if (bHealth) {
				mem::PatchEx((BYTE*)(modBaseAddr + 0x29D1F), (BYTE*)"\x01", 1, hProcess); //5130944 scan for this value!!!!
				std::cout << "Health hack enabled" << std::endl;
				test.getStats();

				std::cout << test.getName() << std::endl;

			}
			else {
				mem::PatchEx((BYTE*)(modBaseAddr + 0x29D1F), (BYTE*)"\x29", 1, hProcess);
				std::cout << "Health hack disabled" << std::endl;
			}
		}
		if (GetAsyncKeyState(VK_NUMPAD2) & 1 ) { // 60HEX 2C0 320 830HEX C60HEX 430
			bAmmo = !bAmmo;
			if (bAmmo) {
				mem::NopEx((BYTE*)(modBaseAddr + 0x637E9), 2, hProcess);//FF 0E
				std::cout << "Ammo hack enabled" << std::endl;
			}
			else {
				mem::PatchEx((BYTE*)(modBaseAddr + 0x637E9), (BYTE*)"\xFF\x0E", 2, hProcess);
				std::cout << "Ammo hack disabled" << std::endl;

			}
		}
		if (GetAsyncKeyState(VK_NUMPAD3) & 1) {
			bRecoil = !bRecoil;
			if (bRecoil) {
				mem::NopEx((BYTE*)(modBaseAddr + 0x63786), 10, hProcess);
				std::cout << "Recoil cheat enabled" << std::endl;
			}
			else {
				mem::PatchEx((BYTE*)(modBaseAddr + 0x63786), (BYTE*)"\x50\x8D\x4C\x24\x1C\x51\x8B\xCE\xFF\xD2", 10, hProcess);
				std::cout << "Recoil cheat disabled" << std::endl;

			}
		}
		if (GetAsyncKeyState(VK_INSERT) & 1) {
			return 0;
		}
	}

	return 0;
}