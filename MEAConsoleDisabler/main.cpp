#include "main.h"

BYTE pattern[] = {0xC7, 0x43, 0x08, 0x02, 0x00, 0x00, 0x00, 0x48, 0x83, 0xC4, 0x20, 0x5B, 0xC3};

bool DataCompare(const BYTE* OpCodes, const BYTE* Mask, const char* StrMask)
{
	while (*StrMask)
	{
		if(*StrMask == 'x' && *OpCodes != *Mask )
			return false;
		++StrMask;
		++OpCodes;
		++Mask;
	}
	return true;
}

DWORD64 FindPattern(DWORD64 StartAddress, DWORD CodeLen, BYTE* Mask, const char* StrMask, unsigned short ignore)
{
	unsigned short Ign = 0;
	DWORD i = 0;
	while (Ign <= ignore)
	{
		if(DataCompare((BYTE*)(StartAddress + i++), Mask,StrMask))
			++Ign;
		else if (i>=CodeLen)
			return 0;
	}
	return StartAddress + i - 1;
}

DWORD WINAPI Start(LPVOID lpParam)
{
    Sleep(1000);
    DWORD64 game = (DWORD64)GetModuleHandle(NULL);
    DWORD64 target = FindPattern(game, 0xA000000, pattern, "xxxxxxxxxxxxx", 0);
    if (!target)
    {
        printf("MEAConsoleDisabler: byte pattern not found!\n");
        return 0;
    }
    printf("MEAConsoleDisabler: target located at %p\n", (void*)target);
    DWORD dwProtect;
    VirtualProtect((void*)(target + 3), 0x1, PAGE_READWRITE, &dwProtect );
    *(BYTE*)(target + 3) = 0;
    VirtualProtect((void*)(target + 3), 0x1, dwProtect, &dwProtect );
    printf("MEAConsoleDisabler: done.\n");
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start, 0, 0, 0);
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // successful
}
