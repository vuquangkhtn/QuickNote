// DLLKeyBoard.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <Windowsx.h>

#define EXPORT  __declspec(dllexport)
#define VK_E 0x45
#define VK_N 0x4E
#define VK_S 0x53
#define VK_T 0x54

HHOOK hHook = NULL;
HINSTANCE hinstLib;
int code =-1;

LRESULT CALLBACK LowLevelKeyboardProc(
  _In_ int    nCode,
  _In_ WPARAM wParam,
  _In_ LPARAM lParam
)
{
	code = -1;
	if(nCode < 0)
		return CallNextHookEx(hHook, nCode, wParam, lParam);
	// xử lý message
	if (GetAsyncKeyState(VK_LWIN) != 0 && GetAsyncKeyState(VK_SPACE) != 0)
	{	
		code = 1;
	}
	else
	if(GetAsyncKeyState(VK_LWIN) != 0 && GetAsyncKeyState(VK_T) != 0)
	{	
		code = 2;
	}
	else
	if (GetAsyncKeyState(VK_LWIN) != 0 && GetAsyncKeyState(VK_S) != 0)
	{	
		code = 3;
	}
	else
	if(GetAsyncKeyState(VK_LWIN) != 0 && GetAsyncKeyState(VK_E) != 0)
	{	
		code = 4;
	}
	else
		return CallNextHookEx(hHook, nCode, wParam, lParam);
}


EXPORT void _doInstallHook(HWND hwnd)
{
	if (hHook!=NULL) return;
	hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)LowLevelKeyboardProc, hinstLib, 0);
	/*if (hHook)
		MessageBox(hwnd, L"Setup hook successfully", L"Result", MB_OK);
	else
		MessageBox(hwnd, L"Setup hook fail", L"Result", MB_OK);*/
}

EXPORT void _doRemoveHook(HWND hWnd)
{
	if (hHook==NULL) return;
	UnhookWindowsHookEx(hHook);
	hHook = NULL;
	//MessageBox(hWnd, L"Remove hook successfully", L"Result", MB_OK);
}

EXPORT int getCode()
{
	return code;
}
