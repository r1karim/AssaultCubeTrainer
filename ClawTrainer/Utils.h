#ifndef UTILS_H
#define UTILS_H

#include<Windows.h>
#include<TlHelp32.h>
#include<vector>
#include <string>
#include <iostream>
#include <math.h>

class Enemy {
private:
	int id, health; HANDLE hProcess; std::string name;
	uintptr_t enemyBase; 
public:
	float position[3] = { 0,0,0 }; float distance;
	Enemy(HANDLE, uintptr_t, unsigned int);
	Enemy();
	void getStats();
	int getHealth();
	std::string getName();
};

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
public:
	float position[3];

	Player(DWORD, uintptr_t, HANDLE);
	void updatePlayer();
	void setHealth(int);
	void setArmour(int);
	void setCurrentWeaponAmmo(int);
	int getPlayerHealth();
	int getPlayerArmour();
	void setViewAngle(float x, float y);
};

namespace mem {
	void PatchEx(BYTE* dst, BYTE* src, unsigned int, HANDLE);
	void NopEx(BYTE* dst, unsigned int, HANDLE);
};

DWORD GetProcId(const wchar_t* procName);
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);
uintptr_t findDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets);
void CalcAngle(float* src, float* dst, float* angle);

float distance3d(float position1[3], float position2[3]);

#endif