#ifndef addPlanet_window
#define addPlanet_window

#include <windows.h>
#include <string.h>
#include "wrapper.h"
#include "resource.h"
#include "PlanetLists.h"
#include <process.h>

#define BIGBUFF 8192

extern BOOL CALLBACK ADD_PLANET_DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
extern void CreatePlanet(HWND hwnd);
extern void Get_Edit_String(HWND hwnd, INT_PTR edit, char *buff);
extern double Get_Edit_Value(HWND hwnd, INT_PTR edit);
extern HWND get_ADD_DIALOG_HANDLE();
extern BOOL numbersOnly(const char *s);
void ClearTextboxes(HWND hwnd);

HWND ADD_DIALOG_hwnd;

#endif