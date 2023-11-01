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
	int health;	int armour;
	int rifleMag; int rifleAmmo;
	int pistolMag; int pistolAmmo;
	float position[3];
public:
	Player(DWORD, uintptr_t, HANDLE);
	void updatePlayer();
	void setHealth(int);
	void setArmour(int);
	void setCurrentWeaponAmmo(int);
	int getPlayerHealth();
	int getPlayerArmour();
};

namespace mem {
	void PatchEx(BYTE* dst, BYTE* src, unsigned int, HANDLE);
	void NopEx(BYTE* dst, unsigned int, HANDLE);
};

DWORD GetProcId(const wchar_t* procName);
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
uintptr_t findDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);

#endif