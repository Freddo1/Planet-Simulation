#ifndef PLANET_DIALOG_HELPER_H
#define PLANET_DIALOG_HELPER_H
#include <windows.h>
#include <string.h>
#include "wrapper.h"
#include "resource.h"

BOOL CALLBACK PLANET_DIALOG_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void SavePlanetsToFile(HWND hwnd);
void LoadPlanetsFromFile(HWND hwnd);
void SendPlanetsToServer(HWND hwnd);

#endif