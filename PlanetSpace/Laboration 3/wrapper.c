#include "wrapper.h"

#define TIMERID	100  /* id for timer that is used by the thread that manages the window where graphics is drawn */
#define DEFAULT_STACK_SIZE	1024
#define TIME_OUT MAILSLOT_WAIT_FOREVER 

/* ATTENTION!!! calls that require a time out, use TIME_OUT constant, specifies that calls are blocked forever */

DWORD threadCreate (LPTHREAD_START_ROUTINE threadFunc, LPVOID threadParams) 
{
	/* Creates a thread running threadFunc 
	 * optional parameters (NULL otherwise)and returns its id! 
	 */

	DWORD threadID;
	HANDLE myHandle = CreateThread(NULL, DEFAULT_STACK_SIZE, threadFunc, threadParams, 0, &threadID);
	return threadID;
}

HANDLE mailslotCreate (char *name) 
{
	/* Creates a mailslot with the specified name and returns the handle 
	 * Should be able to handle a messages of any size 
	 */
	HANDLE hSlot = CreateMailslotA(name, 0, TIME_OUT, NULL);
	return hSlot;
}

HANDLE mailslotConnect (char *name) 
{
	/* Connects to an existing mailslot for writing
	 * and returns the handle upon success     
	 */
	HANDLE h = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	return h;
}

int mailslotWrite(HANDLE mailSlot, void *msg, int msgSize) 
{
	/* Write a msg to a mailslot, return nr 
	 * of successful bytes written         
	 */
	DWORD cbWritten;
	WriteFile(mailSlot, msg, msgSize, &cbWritten, NULL);
	return TRUE;
}

int	mailslotRead (HANDLE mailSlot, void *msg, int msgSize) 
{
	/* Read a msg from a mailslot, return nr
	 * of successful bytes read             
	 */
	DWORD bytesRead;
	ReadFile(mailSlot, msg, msgSize, &bytesRead, NULL);
	return bytesRead;
}

//Kills all the planets on the server by a simple message, rest of the code resides on server.c
void KillServerplanets()
{
	HANDLE mailSlot;
	char killMessage[5] = "KILL";
	while ((mailSlot = mailslotConnect("\\\\.\\mailslot\\mailbox")) == INVALID_HANDLE_VALUE)
	{
		//Do nothing
	}
	mailslotWrite(mailSlot, killMessage, sizeof(killMessage));

	mailslotClose(mailSlot);
}

int mailslotClose(HANDLE mailSlot)
{
	// Close a mailslot, returning whatever the service call returns 
	CloseHandle(mailSlot);
}

/* Wrappers for window management, used for lab 2 and 3 
 * DONT CHANGE!!! JUST FYI
 */

HWND windowCreate (HINSTANCE hPI, HINSTANCE hI, int ncs, char *title, WNDPROC callbackFunc, int bgcolor) 
{
  HWND hWnd;
  WNDCLASS wc; 

  if( !hPI) {
	 wc.lpszClassName = "GenericAppClass";
	 wc.lpfnWndProc = callbackFunc;
	 wc.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	 wc.hInstance = hI;
	 wc.hIcon = LoadIcon( NULL, IDI_APPLICATION );
	 wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	 wc.hbrBackground = (HBRUSH) bgcolor;
	 wc.lpszMenuName = "GenericAppMenu";

	 wc.cbClsExtra = 0;
	 wc.cbWndExtra = 0;

	 RegisterClass( &wc );
  }

  hWnd = CreateWindow( "GenericAppClass",
				 title,
				 WS_OVERLAPPEDWINDOW|WS_HSCROLL|WS_VSCROLL,
				 0,
				 0,
				 CW_USEDEFAULT,
				 CW_USEDEFAULT,
				 NULL,
				 NULL,
				 hI,
				 NULL
				 );

  ShowWindow( hWnd, ncs );
 

  return hWnd;
}

void windowRefreshTimer (HWND hWnd, int updateFreq) 
{
  if(SetTimer(hWnd, TIMERID, updateFreq, NULL) == 0) {
	 /* NOTE: Example of how to use MessageBoxes, see the online help for details. 
	  */
	 MessageBox(NULL, "Failed setting timer", "Error!!", MB_OK);
	 exit (1);
  }
}

/* Wrappers for window management, used for lab  3
 * Lab 3: Check in MSDN GetOpenFileName and GetSaveFileName
 *  what the parameters mean, and what you must call this function with
 */

HANDLE OpenFileDialog(char* string, DWORD accessMode, DWORD howToCreate)
{

	OPENFILENAME opf;
	char szFileName[_MAX_PATH]="";

	opf.Flags				= OFN_SHOWHELP | OFN_OVERWRITEPROMPT; 
	opf.lpstrDefExt			= "dat";
	opf.lpstrCustomFilter	= NULL;
	opf.lStructSize			= sizeof(OPENFILENAME);
	opf.hwndOwner			= NULL;
	opf.lpstrFilter			= NULL;
	opf.lpstrFile			= szFileName;
	opf.nMaxFile			= _MAX_PATH;
	opf.nMaxFileTitle		= _MAX_FNAME;
	opf.lpstrInitialDir		= NULL;
	opf.lpstrTitle			= string;
	opf.lpstrFileTitle		= NULL ; 
	
	if(accessMode == GENERIC_READ)
		GetOpenFileName(&opf);
	else
		GetSaveFileName(&opf);

	return CreateFile(szFileName, 
		accessMode, 
		0, 
		NULL, 
		howToCreate, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);
}