#pragma once

BOOL InitApplication(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

extern HWND hWndMain;
extern STRUCT_MOB g_pBaseSet[MAX_CLASS];