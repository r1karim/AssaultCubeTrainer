#ifndef UTILS_H
#define UTILS_H

#include<Windows.h>
#include<TlHelp32.h>
#include<vector>

class Player {
private:
	uintptr_t entityPtr;
	uintptr_t entityAddr;
	DWORD processid;
	HANDLE hprocess;
	uintptr_t moduleBase;
	int health;
	int rifleMag;
	int rifleAmmo;
	int pistolMag;
	int pistolAmmo;
	float position[3];
	int armour;
public:
	Player(DWORD, uintptr_t, HANDLE);
	void updatePlayer();
	void setHealth(int);
	void setArmour(int);
	int getPlayerHealth();
	int getPlayerArmour();
};

DWORD GetProcId(const wchar_t* procName);
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
uintptr_t findDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

#endif