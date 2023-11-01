#include "Utils.h"

Player::Player(DWORD procId, uintptr_t moduleBase, HANDLE hProcess) {
	this->moduleBase = moduleBase; this->processid = procId;
	this->hprocess = hProcess; this->entityAddr = moduleBase + 0x10F4F4;
	ReadProcessMemory(hProcess, (LPVOID)(entityAddr), &this->entityPtr, sizeof(this->entityPtr), 0);
	this->updatePlayer();
}

void Player::updatePlayer() {
	health = findDMAAddy(hprocess, entityAddr, {0xF8, 0x0});
	armour = findDMAAddy(hprocess, entityAddr, { 0xFC, 0x0 });
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