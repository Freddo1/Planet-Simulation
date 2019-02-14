#define _CRT_SECURE_NO_WARNINGS
#include "wrapper.h"
#include "addPlanet_window.h"
#include "fileHandler_window.h"

HDC hDC;

void* ReadMailboxThread(void *params);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL ret;

	ADD_DIALOG_hwnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(IDD_ADD_PLANET), 0, ADD_PLANET_DlgProc, 0);
	LIST_DIALOG_hwnd = CreateDialogParam(hInstance, MAKEINTRESOURCE(PLANET_DIALOG), 0, PLANET_DIALOG_Proc, 0);

	//ShowWindow(ADD_DIALOG_hwnd, nCmdShow);
	ShowWindow(LIST_DIALOG_hwnd, nCmdShow);

	threadCreate((void *)ReadMailboxThread, NULL);

	while (ret = GetMessage(&msg, NULL, 0, 0) != 0)
	{
		if (ret == -1)
			return -1;

		if (!IsDialogMessage(LIST_DIALOG_hwnd, &msg) && !IsDialogMessage(ADD_DIALOG_hwnd, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}

void* ReadMailboxThread(void *params)
{
	char buffer[1024];
	HANDLE mailBox;
	DWORD bytesRead;

	// Create mailbox
	char buf[100], pid[30], nameFromServer[256];
	sprintf_s(pid, 30 + 1, "%lu", _getpid());
	strcpy_s(buf, sizeof(buf), "\\\\.\\mailslot\\");
	strcat_s(buf, sizeof(buf), pid);
	mailBox = mailslotCreate(buf);

	while (TRUE) // Client reads message. 
	{
		bytesRead = mailslotRead(mailBox, buffer, sizeof(buffer));

		if (bytesRead != 0)
		{
			// Get message and add it to listbox
			char *msg = (char *)buffer;
			AddToListbox(GetPlanetDialogHandle(), msg, LBX_MSG);

			// Update planets in server root
			planet_type *iterator = serverroot;

			while (iterator != NULL)
			{
				int planetNameLength = strlen(iterator->name);
				strncpy(nameFromServer, buffer, planetNameLength);
				nameFromServer[planetNameLength] = '\0';

				if (strcmp(nameFromServer, iterator->name) == 0)
				{
					RemovePlanetFromServerList(iterator);

					// Update listbox
					ClearListbox(GetPlanetDialogHandle(), LBX_SERVER);
					planet_type *it = serverroot;
					int counter = 0;
					while (it != NULL)
					{
						AddToListbox(GetPlanetDialogHandle(), it->name, LBX_SERVER);
						counter++;
						it = it->next;
					}


					char countbuf[20];
					sprintf(countbuf, "Planets: %d", counter);
					SetDlgItemText(GetPlanetDialogHandle(), LBL_PLANETCOUNTER, countbuf);

					break;

				}
				iterator = iterator->next;
			}

		}
	}
}