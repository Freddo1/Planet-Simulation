#ifndef WRAPPER_H
#define WRAPPER_H

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>
// Wrappers for windowhandling


extern HWND windowCreate (HINSTANCE hpI, HINSTANCE hI, int ncs, char *title, WNDPROC callbackFunc, int bgcolor);

extern void windowRefreshTimer (HWND hWnd, int updateFreq);

// Mailslot handling:
extern DWORD threadCreate (LPTHREAD_START_ROUTINE threadFunc, LPVOID threadParams);

extern HANDLE mailslotCreate (char *name);
extern HANDLE mailslotConnect (char * name);

extern int mailslotWrite (HANDLE mailSlot, void * msg, int msgSize);
extern int mailslotRead (HANDLE mailSlot, void * msg, int msgSize);

extern int mailslotClose (HANDLE mailSlot);
extern HANDLE OpenFileDialog(char* string, DWORD accessMode, DWORD howToCreate);


// Struct for planet data will be used in lab 2 and 3 !!!!!
// Just ignore in lab1 or you can try to send it on your mailslot, 
// will be done in lab 2 and 3

typedef struct pt {
	char		name[20];	// Name of planet
	double		sx;			// X-axis position
	double		sy;			// Y-axis position
	double		vx;			// X-axis velocity
	double		vy;			// Y-axis velocity
	double		mass;		// Planet mass
	struct pt*	next;		// Pointer to next planet in linked list
	int			life;		// Planet life
	char		pid[30];	// String containing ID of creating process
} planet_type;

#endif /* WRAPPER_H */