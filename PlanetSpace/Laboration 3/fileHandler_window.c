#define _CRT_SECURE_NO_WARNINGS
#include "fileHandler_window.h"
#include "addPlanet_window.h"

int sentCounter = 0;
LRESULT CALLBACK PLANET_DIALOG_Proc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case BTN_SAVE:
		case ID_FILE_SAVE:
			SavePlanetsToFile(hwnd);
			break;
		case BTN_LOAD:
		case ID_FILE_LOAD:
			LoadPlanetsFromFile(hwnd);
			break;
		case BTN_SEND:
			SendPlanetsToServer(hwnd);
			break;
		case BTN_AddPlanetWindow:
			ShowWindow(ADD_DIALOG_hwnd, SW_SHOWNORMAL);
			break;
		case BTN_CLEARSERVER:
			RemovePlanetsFromServerList();
			ResetPlanetCounter(hwnd);
			KillServerplanets();
			ClearListbox(hwnd, LBX_SERVER);
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

void ResetPlanetCounter(HWND hwnd)
{
	char countbuf[20];
	sentCounter = 0;
	sprintf(countbuf, "Planets: %d", sentCounter);
	SetDlgItemText(hwnd, LBL_PLANETCOUNTER, sentCounter);
}

void SavePlanetsToFile(HWND hwnd)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Planet Files (*.dat)\0*.dat\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = "dat";
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

	if (GetSaveFileName(&ofn))
	{
		SavePlanetFile(hwnd, ofn);
	}
}
void LoadPlanetsFromFile(HWND hwnd)
{
	OPENFILENAME ofn;
	char szFileName[MAX_PATH] = "";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFilter = "Text Files (*.dat)\0*.dat\0All Files (*.*)\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "dat";

	if (GetOpenFileName(&ofn))
	{
		ReadPlanetFile(hwnd, ofn);
	}
}
void ReadPlanetFile(HWND hwnd, OPENFILENAME ofn)
{
	FILE *file = fopen(ofn.lpstrFile, "rb");

	if (file)
	{
		planet_type planet;

		fseek(file, 0, SEEK_END);
		int end = ftell(file);
		rewind(file);
		while (ftell(file) != end)
		{
			fread(&planet, sizeof(planet_type), 1, file);
			planet.next = NULL;
			char pid[30];
			sprintf_s(pid, sizeof(pid), "%lu", _getpid());
			strcpy_s(planet.pid, 30, pid);
			if (CheckDuplicate(planet.name, 1) == 1)
			{
				AddToListbox(hwnd, planet.name, LBX_LOCAL);
				AddPlanetToLocalList(&planet);
			}
		}
		fclose(file);
	}
}
void SavePlanetFile(HWND hwnd, OPENFILENAME ofn)
{
	HANDLE lbxhwnd = GetDlgItem(hwnd, LBX_LOCAL);
	char buffer[BIGBUFF];
	int selCount = SendMessage(lbxhwnd, LB_GETSELCOUNT, 0, 0);
	int listCount = SendMessage(lbxhwnd, LB_GETCOUNT, 0, 0);

	int selInd = -1;

	FILE *file = fopen(ofn.lpstrFile, "wb");

	if (file)
	{
		if (selCount > 0)
		{
			for (int i = 0; i < listCount; i++)
			{
				if (SendMessage(lbxhwnd, LB_GETSEL, i, 0) > 0) // LB_GETSELITEMS for list of items
				{
					SendMessage(lbxhwnd, LB_GETTEXT, (WPARAM)i, (LPARAM)buffer);

					// Loop through all planets in local list
					planet_type *current = localroot;
					while (current != NULL)
					{
						if (strcmp(current->name, buffer) == 0)
						{
							fwrite(current, sizeof(planet_type), 1, file);
						}
						// Continue iteration
						current = current->next;
					}
				}
			}

			AddToListbox(GetPlanetDialogHandle(), "Saved selected planetes.", LBX_MSG);
		}
		else
		{
			planet_type *current = localroot;
			while (current != NULL)
			{
				fwrite(current, sizeof(planet_type), 1, file);
				current = current->next;
			}
			AddToListbox(GetPlanetDialogHandle(), "Saved all planets.", LBX_MSG);
		}
		fclose(file);
	}
}

void SendPlanetsToServer(HWND hwnd)
{
	HANDLE lbxhwnd = GetDlgItem(hwnd, LBX_LOCAL);
	HANDLE mailSlot;
	if (localroot == NULL)
	{
		MessageBox(hwnd, "You have to have minimum 1 planet to send it to the server", "Notice", MB_OK);
		return;
	}

	// Connect to server mailbox
	while ((mailSlot = mailslotConnect("\\\\.\\mailslot\\mailbox")) == INVALID_HANDLE_VALUE)
	{
		//Do nothing
	}

	if (SendMessage(lbxhwnd, LB_GETSELCOUNT, 0, 0) > 0)
	{
		SendSelected(hwnd, mailSlot);
	}
	else
	{

		// Loop through all planets in local list
		planet_type *current = localroot;
		while (current != NULL)
		{
			if (CheckDuplicate(current->name, 0) == 1)
			{
				mailslotWrite(mailSlot, (void *)current, sizeof(planet_type));	// Write planet to server
				AddPlanetToServerList(current);									// Add planet to server list
				AddToListbox(hwnd, current->name, LBX_SERVER);					// Add planet to server listbox
				sentCounter++;
			}
			current = current->next;
		}

		char countbuf[20];
		sprintf(countbuf, "Planets: %d", sentCounter);
		SetDlgItemText(hwnd, LBL_PLANETCOUNTER, countbuf);

		// Remove everything from local
		int count = SendMessage(lbxhwnd, LB_GETCOUNT, 0, 0);
		current = localroot;
		for (int i = 0; i < count; i++)
		{
			while (current != NULL)
			{
				RemovePlanetFromLocalList(localroot);
				break;
			}
		}
		ClearListbox(GetPlanetDialogHandle(), LBX_LOCAL);
	}

	// Disconnect from server mailbox
	mailslotClose(mailSlot);
	AddToListbox(GetPlanetDialogHandle(), "Send Complete!", LBX_MSG);
}
void SendSelected(HWND hwnd, HANDLE mailSlot)
{
	HANDLE lbxhwnd = GetDlgItem(hwnd, LBX_LOCAL);
	char buffer[BIGBUFF];
	int lbxcount = SendMessage(lbxhwnd, LB_GETCOUNT, 0, 0);

	for (int i = 0; i < lbxcount; i++)
	{
		if (SendMessage(lbxhwnd, LB_GETSEL, i, 0)) // If we have selected an item
		{
			SendMessage(lbxhwnd, LB_GETTEXT, (WPARAM)i, (LPARAM)buffer); // Get string of selected item

			// Find a planet in the list with the same name as the selected item string
			planet_type *current = localroot;
			while (current != NULL)
			{
				if (!strcmp(current->name, buffer)) // If there is no difference between strings
				{
					AddToListbox(hwnd, current->name, LBX_SERVER);
					mailslotWrite(mailSlot, (void *)current, sizeof(planet_type));// Write planet to server
					AddPlanetToServerList(current);// Add planet to server list
					RemovePlanetFromLocalList(current);
					sentCounter++;
					current = localroot;
				}
				else
					current = current->next;

				char countbuf[20];
				sprintf(countbuf, "Planets: %d", sentCounter);
				SetDlgItemText(hwnd, LBL_PLANETCOUNTER, countbuf);

			}
		}
	}

	UpdateListbox(hwnd, LBX_LOCAL, localroot);
}
void AddToListbox(HWND hwnd, char* msg, int listbox)
{
	HWND hwndList = GetDlgItem(hwnd, listbox);
	SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)msg);
}
void ClearListbox(HWND hwnd, int listbox)
{
	HWND hwndList = GetDlgItem(hwnd, listbox);
	SendMessage(hwndList, LB_RESETCONTENT, 0, 0);
}
HWND GetPlanetDialogHandle()
{
	return LIST_DIALOG_hwnd;
}
void UpdateListbox(HWND hwnd, int Listbox, planet_type *root)
{
	ClearListbox(hwnd, Listbox);
	planet_type *iterator = root;
	while (iterator != NULL)
	{
		AddToListbox(hwnd, iterator->name, Listbox);
		iterator = iterator->next;
	}
}