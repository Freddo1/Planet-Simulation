#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "wrapper.h"
#include <time.h>
#include <string.h>

#define UPDATE_FREQ 1
#define G 6.67259e-11
#define DT 10

LRESULT WINAPI MainWndProc(HWND, UINT, WPARAM, LPARAM);
DWORD WINAPI mailThread(LPVOID);

/* Server client functions */
void * ThreadPlanetFunc(void * params);
void AddPlanetToList(planet_type*);
void RemovePlanetFromList(planet_type*);
void * InfiniteThread(void * params);
void RemovePlanetsFromServerList();
HDC hDC;

planet_type* root;
int planet_counter;
CRITICAL_SECTION database_mod;

/********************************************************************\
*  Function: int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)    *
*                                                                    *
*   Purpose: Initializes Application                                 *
*                                                                    *
*  Comments: Register window class, create and display the main      *
*            window, and enter message loop.                         *
*                                                                    *
*                                                                    *
\********************************************************************/

/* NOTE: This function is not too important to you, it only */
/*       initializes a bunch of things.                     */
/* NOTE: In windows WinMain is the start function, not main */

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) {

	HWND hWnd;
	DWORD threadID;
	MSG msg;
	InitializeCriticalSection(&database_mod);

	hWnd = windowCreate(hPrevInstance, hInstance, nCmdShow, "SolarSystem", MainWndProc, COLOR_WINDOW + 1);

	windowRefreshTimer(hWnd, UPDATE_FREQ);

	threadID = threadCreate(mailThread, NULL);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}


/********************************************************************\
* Function: mailThread                                               *
* Purpose: Handle incoming requests from clients                     *
* NOTE: This function is important to you.                           *
/********************************************************************/
DWORD WINAPI mailThread(LPVOID arg)
{
	char buffer[1024];
	DWORD bytesRead;
	static int posY = 0;
	HANDLE mailbox;

	mailbox = mailslotCreate("\\\\.\\mailslot\\mailbox");


	SetThreadPriority(CreateThread(NULL, 1024, (void *)InfiniteThread, NULL, 0, NULL), 31);

	while (TRUE) // server reads message, creates new thread with new planet. 
	{
		bytesRead = mailslotRead(mailbox, buffer, sizeof(buffer));

		if (strcmp(buffer, "KILL") == 0)
			RemovePlanetsFromServerList();
		else if (bytesRead != 0)
		{
			planet_type *data = malloc(sizeof(planet_type));
			memcpy(data, buffer, sizeof(planet_type));
			data->next = NULL;
			AddPlanetToList(data);
		}
	}

	return 0;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	static int posX = 10;
	int posY;
	HANDLE context;
	static DWORD color = 0;
	planet_type *current = root;

	switch (msg) {
	case WM_CREATE:
		hDC = GetDC(hWnd);
		break;
	case WM_TIMER:
		EnterCriticalSection(&database_mod);
		while (current != NULL)
		{
			/*TextOut(hDC, current->sx, current->sy, current->name, strlen(current->name));*/
			SetPixel(hDC, (int)current->sx, (int)current->sy, (COLORREF)color);
			current = current->next;
		}
		LeaveCriticalSection(&database_mod);
		break;
	case WM_PAINT:
		/* NOTE: The code for this message can be removed. It's just */
		/*       for showing something in the window.                */
		context = BeginPaint(hWnd, &ps); /* (you can safely remove the following line of code) */
		EndPaint(hWnd, &ps);
		break;
		/**************************************************************\
		*     WM_DESTROY: PostQuitMessage() is called                  *
		*     (received when the user presses the "quit" button in the *
		*      window)                                                 *
		\**************************************************************/
	case WM_DESTROY:
		PostQuitMessage(0);
		/* NOTE: Windows will automatically release most resources this */
		/*       process is using, e.g. memory and mailslots.           */
		/*       (So even though we don't free the memory which has been*/
		/*       allocated by us, there will not be memory leaks.)      */

		ReleaseDC(hWnd, hDC); /* Some housekeeping */
		break;
		/**************************************************************\
		*     Let the default window proc handle all other messages    *
		\**************************************************************/
	default:
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return 0;
}

void RemovePlanetsFromServerList()
{
	if (root != NULL)
	{
		planet_type *current = root;
		planet_type *planetToFree;
		while (current->next != NULL)
		{
			planetToFree = current;
			current = current->next;
			planetToFree->next = NULL;
			free(planetToFree);
		}
		free(current);
		root = NULL;
	}
}

void AddPlanetToList(planet_type *data)
{
	if (root == NULL)
	{
		root = data;
	}
	else
	{
		planet_type *current = root;
		while (current->next != NULL)
		{
			current = current->next;
		}
		current->next = data;
	}

	threadCreate((void *)ThreadPlanetFunc, (void *)data);
}

void RemovePlanetFromList(planet_type* element)
{
	EnterCriticalSection(&database_mod);

	planet_type *current = root;
	planet_type *swapper = NULL;
	if (element == root)
	{
		root = root->next;
	}
	while (current->next != NULL)
	{
		if (element == current->next)
		{
			if (current->next != NULL)
			{
				swapper = current->next->next;
				free(current->next);
				current->next = swapper;
			}
			else
			{
				free(current->next);
				current->next = NULL;
			}
		}
		else
		{
			current = current->next;
		}
	}
	LeaveCriticalSection(&database_mod);
}

void * ThreadPlanetFunc(void *params)
{
	planet_type *my_planet = (planet_type *)params;
	HANDLE mailSlot;

	int dieFlag = 0; // 0 = out of life, 1 = out of bounds.

	while (my_planet->life > 0)
	{
		double atot_x, atot_y;
		atot_x = atot_y = 0;

		EnterCriticalSection(&database_mod);
		planet_type *current = root;
		while (current != NULL)
		{
			if (current != my_planet)
			{
				double r = sqrt(pow((current->sx - my_planet->sx), 2) + pow((current->sy - my_planet->sy), 2));


				double a1 = G * (current->mass / (r*r));

				atot_x += a1*((current->sx - my_planet->sx) / r);
				atot_y += a1*((current->sy - my_planet->sy) / r);
			}
			current = current->next;
		}
		LeaveCriticalSection(&database_mod);

		my_planet->vx += (atot_x * DT);
		my_planet->sx += (my_planet->vx * DT);

		my_planet->vy += (atot_y * DT);
		my_planet->sy += (my_planet->vy * DT); 

		// Kill if out of bounds
		if (my_planet->sx < 0 || my_planet->sx > 1000 || my_planet->sy < 0 || my_planet->sy > 800)
		{
			my_planet->life = 0;
			dieFlag = 1;
		}

		my_planet->life--;

		Sleep(UPDATE_FREQ);
	}

	// Send message that planet died
	char buf[255];
	char msg[255];
	strcpy_s(msg, sizeof(msg), my_planet->name);
	strcat_s(msg, sizeof(msg), " died because of: ");
	strcat_s(msg, sizeof(msg), (!dieFlag) ? "Out of Life" : "Out of bounds");

	strcpy_s(buf, sizeof(buf), "\\\\.\\mailslot\\");
	strcat_s(buf, sizeof(buf), my_planet->pid);

	while ((mailSlot = mailslotConnect(buf)) == INVALID_HANDLE_VALUE)
	{
	}

	mailslotWrite(mailSlot, (void*)msg, sizeof(msg));

	RemovePlanetFromList(my_planet);
}

void * InfiniteThread(void * params)
{
	while (1)
	{

	}
}