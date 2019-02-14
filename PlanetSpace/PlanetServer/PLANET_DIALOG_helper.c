#include "PLANET_DIALOG_helper.h"

BOOL CALLBACK PLANET_DIALOG_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_COMMAND:
		// switch med de olika knapparna och funktionerna för dialogen. 
		switch (LOWORD(wParam))
		{
		case BTN_SAVE:
			SavePlanetsToFile(hwnd);
			break;
		case BTN_LOAD:
			LoadPlanetsFromFile(hwnd);
			break;
		case BTN_SEND:
			SendPlanetsToServer(hwnd);
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return TRUE;

	case WM_DESTROY:
		PostQuitMessage(0);
		return TRUE;
	default:
		return FALSE;
	}
	return TRUE;
}
void SavePlanetsToFile(HWND hwnd)
{
	MessageBox(hwnd, "SAVE", "SAVE TITLE", NULL);
}
void LoadPlanetsFromFile(HWND hwnd)
{
	MessageBox(hwnd, "LOAD", "LOAD TITLE", NULL);
}
void SendPlanetsToServer(HWND hwnd)
{
	MessageBox(hwnd, "SEND", "SEND TITLE", NULL);
}