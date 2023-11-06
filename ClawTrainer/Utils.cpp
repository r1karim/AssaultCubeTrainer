#include "Utils.h"

void Enemy::getStats() {
	//uintptr_t baseAddr = modBase + 0x10F4F8;
	char pname[16];
	//uintptr_t enemyBase = findDMAAddy(hProcess, baseAddr, { 0x10 });
	health = findDMAAddy(hProcess, enemyBase, {0xF8, 0x0});
	ReadProcessMemory(hProcess, (LPVOID)findDMAAddy(hProcess, enemyBase, { 0x225,0x0 }) , &pname, 12, NULL); name = pname; //
	ReadProcessMemory(hProcess, (LPVOID)findDMAAddy(hProcess, enemyBase, { 0x34 }), &position[0], sizeof(float), 0);
	ReadProcessMemory(hProcess, (LPVOID)findDMAAddy(hProcess, enemyBase, { 0x34 + 0x4}), &position[1], sizeof(float), 0);
	ReadProcessMemory(hProcess, (LPVOID)findDMAAddy(hProcess, enemyBase, { 0x34 + 0x8 }), &position[2], sizeof(float), 0);

}

float distance3d(float position1[3], float position2[3]) {
	return sqrt(pow(double(position1[0] - position2[0]), 2.0) + pow(double(position1[1] - position2[1]), 2.0) + pow(double(position1[2] - position2[2]), 2.0));
}

Enemy::Enemy() {

}
Enemy::Enemy(HANDLE hProcess, uintptr_t modBase, unsigned int id) {
	this-> hProcess = hProcess;  this->id = id;
	int count = 0;
	unsigned int i=4;
	for (; ; ) {
		if (count == this->id) break;
		if ((i + 4) % 4 == 0) i = i + 4;
		else i = i + 2;
		count++;
	}
	enemyBase = findDMAAddy(hProcess, (modBase + 0x10F4F8),{  i });
}

int Enemy::getHealth() {
	return this->health;
}

std::string Enemy::getName() {
	return this->name;
}

Player::Player(DWORD procId, uintptr_t moduleBase, HANDLE hProcess) {
	this->moduleBase = moduleBase; this->processid = procId;
	this->hprocess = hProcess; this->entityAddr = moduleBase + 0x10F4F4;
	ReadProcessMemory(hProcess, (LPVOID)(entityAddr), &this->entityPtr, sizeof(this->entityPtr), 0);
	this->updatePlayer();
}

void Player::updatePlayer() {
	health = findDMAAddy(hprocess, entityAddr, {0xF8, 0x0});
	armour = findDMAAddy(hprocess, entityAddr, { 0xFC, 0x0 });
	ReadProcessMemory(hprocess, (LPVOID)findDMAAddy(hprocess, entityAddr, { 0x34 }), &position[0], sizeof(float), 0);
	ReadProcessMemory(hprocess, (LPVOID)findDMAAddy(hprocess, entityAddr, { 0x34 + 0x4 }), &position[1], sizeof(float), 0);
	ReadProcessMemory(hprocess, (LPVOID)findDMAAddy(hprocess, entityAddr, { 0x34 + 0x8 }), &position[2], sizeof(float), 0);

}

void Player::setArmour( int value ) {
	WriteProcessMemory(hprocess, (BYTE*)(entityPtr + 0xFC), &value, sizeof(value), nullptr);
}

void Player::setHealth(int value) {
	WriteProcessMemory(hprocess, (BYTE*)(entityPtr + 0xF8), &value, sizeof(value), nullptr);
}

void Player::setCurrentWeaponAmmo(int value) {
	WriteProcessMemory(
		hprocess,
		(BYTE*)findDMAAddy(hprocess, entityAddr, { 0x374, 0x14, 0x0 }),
		&value,
		sizeof(value),
		nullptr
	);
}

void Player::setViewAngle(float x, float y) {
	WriteProcessMemory(hprocess, (BYTE*)findDMAAddy(hprocess, entityAddr, { 0x40 }),&x,sizeof(x),nullptr);
	WriteProcessMemory(hprocess, (BYTE*)findDMAAddy(hprocess, entityAddr, { 0x44 }), &x, sizeof(x), nullptr);

}

void CalcAngle(float* src, float* dst, float* angle) {
	double delta[3] = { (src[0] - dst[0]),(src[1] - dst[1]) ,(src[2] - dst[2]) };
	double hyp = sqrt(delta[0] * delta[0] + delta[1] * delta[1]);
	angle[0] = (float)(asinf(delta[2] / hyp) * 57.295779513082f);
	angle[1] = (float)(atanf(delta[1] / delta[0]) * 57.295779513082f);
	angle[2] = 0;

	if (delta[0] >= 0.0) {
		angle[1] += 180.0f;
	}
}

int Player::getPlayerHealth() { return this->health; }
int Player::getPlayerArmour() { return this->armour; }

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess) {
	DWORD oldprotect;
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
	WriteProcessMemory(hProcess, dst, src, size, nullptr);
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);
}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess) {
	BYTE* nopArr = new BYTE[size];
	memset(nopArr, 0x90, size);
	mem::PatchEx(dst, nopArr, size, hProcess);
	delete[] nopArr;
}

DWORD GetProcId(const wchar_t* procName) {
	DWORD procId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnap != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);
		if (Process32First(hSnap, &procEntry)) {
			do {
				if (!_wcsicmp(procEntry.szExeFile, procName)) {
					procId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
	}
	CloseHandle(hSnap);
	return procId;
}


uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);

	if (hSnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hSnap, &modEntry)) {
			do {
				if (!_wcsicmp(modEntry.szModule, modName)) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnap, &modEntry));
		}
	}
	CloseHandle(hSnap);
	return modBaseAddr;
}

uintptr_t findDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets) {
	uintptr_t addr = ptr;
	for (int i = 0; i < offsets.size(); i++) {
		ReadProcessMemory(hProc, (LPVOID)addr, &addr, sizeof(addr), 0);
		addr += offsets[i];
	}
	return addr;
}