#include <windows.h>
#include "MinHook.h"

typedef void(__cdecl* CTimer__Update) ();
CTimer__Update fpCTimerHook{ NULL };
static WNDPROC OrigWnd;
bool bActive{ true };

auto __cdecl AddMessageJumpQ(const char* text, unsigned int time = 1000, unsigned short flag = NULL, bool bPreviousBrief = false) -> void {
	((void(__cdecl*)(const char*, unsigned int, unsigned short, bool))0x69F1E0)(text, time, flag, bPreviousBrief);
}

auto __cdecl nop(const bool state) -> void {
	DWORD oldProt;
	if (state) {
		VirtualProtect(reinterpret_cast<LPVOID>(0x73DEA4), 2, PAGE_READWRITE, &oldProt);
		memset(reinterpret_cast<void*>(0x73DEA4), 0x90, 0x2);
		VirtualProtect(reinterpret_cast<LPVOID>(0x73DEA4), 2, oldProt, &oldProt);
	} else {
		VirtualProtect(reinterpret_cast<LPVOID>(0x73DEA4), 2, PAGE_READWRITE, &oldProt);
		memcpy(reinterpret_cast<void*>(0x73DEA4), "\x76\x0F", 0x2);
		VirtualProtect(reinterpret_cast<LPVOID>(0x73DEA4), 2, oldProt, &oldProt);
	} return;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (uMsg == WM_KEYUP && wParam == VK_F12) {
		bActive ^= true;
		nop(bActive);
		AddMessageJumpQ(bActive ? "NOP WR-SiF: Activated" : "NOP WR-SiF: Deactivated");
	} return CallWindowProcA(OrigWnd, hwnd, uMsg, wParam, lParam);
}

auto __cdecl hk_CTimerUpdate(void) -> void {
	static bool init{ false };
	if (!init) {
		OrigWnd = (WNDPROC)SetWindowLongA(**reinterpret_cast<HWND**>(0xC17054), GWL_WNDPROC, (LONG)WindowProc);
		init = true;
	} return fpCTimerHook();
}

struct stMain {
	stMain() {
		nop(true);
		MH_Initialize();
		MH_CreateHook(reinterpret_cast<void*>(0x561B10), &hk_CTimerUpdate, reinterpret_cast<void**>(&fpCTimerHook));
		MH_EnableHook(reinterpret_cast<void*>(0x561B10));
	};
	~stMain() {
		MH_Uninitialize();
		nop(false);
	};
} plugin;
