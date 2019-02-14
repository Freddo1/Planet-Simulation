#ifndef fileHandler_window_H
#define fileHandler_window_H

#include <windows.h>
#include <string.h>
#include "wrapper.h"
#include "PlanetLists.h"
#include <stdio.h>
#include "resource.h"
#include <stdlib.h>
#include <process.h>

#define BIGBUFF 8192

LRESULT CALLBACK PLANET_DIALOG_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam);
void SavePlanetsToFile(HWND hwnd);
void LoadPlanetsFromFile(HWND hwnd);
extern void SendPlanetsToServer(HWND hwnd);
extern void SendSelected(HWND hwnd, HANDLE mailSlot);
extern void AddToListbox(HWND hwnd, char* msg, int listbox);
extern void ClearListbox(HWND hwnd, int listbox);
extern void ReadPlanetFile(HWND hwnd, OPENFILENAME ofn);
extern void SavePlanetFile(HWND hwnd, OPENFILENAME ofn);
extern HWND GetPlanetDialogHandle();
extern void UpdateListbox(HWND hwnd, int Listbox, planet_type *root);
extern void ResetPlanetCounter(HWND hwnd);

HWND LIST_DIALOG_hwnd;

#endif