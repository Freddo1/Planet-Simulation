#include "addPlanet_window.h"

// Message handler for dialog 'ADD_PLANET'
BOOL CALLBACK ADD_PLANET_DlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	switch (Message)
	{
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_ADD:
			CreatePlanet(hwnd);
			break;
		}
		break;
	case WM_CLOSE:
		ShowWindow(hwnd, SW_HIDE);
		return TRUE;
	default:
		return FALSE;
	}
	return FALSE;
}

void ClearTextboxes(HWND hwnd)
{
	SetDlgItemText(hwnd, IDC_EDIT_MASS, "");
	SetDlgItemText(hwnd, IDC_EDIT_PX, "");
	SetDlgItemText(hwnd, IDC_EDIT_PY, "");
	SetDlgItemText(hwnd, IDC_EDIT_VX, "");
	SetDlgItemText(hwnd, IDC_EDIT_VY, "");
	SetDlgItemText(hwnd, IDC_EDIT_LIFE, "");
	SetDlgItemText(hwnd, IDC_EDIT_NAME, "");
}

void CreatePlanet(HWND hwnd)
{
	planet_type* newPlanet = malloc(sizeof(planet_type));
	char buff[25];
	char pid[30];
	int test = 0;
	sprintf_s(pid, sizeof(pid), "%lu", _getpid());

	if (GetWindowTextLength(GetDlgItem(hwnd, IDC_EDIT_NAME)) > 24)
	{
		MessageBox(hwnd, "Planet name to long, keep it less than 25 charathers", NULL, MB_OK);
		ClearTextboxes(hwnd);
		return;
	}

	Get_Edit_String(hwnd, IDC_EDIT_NAME, buff);
	if (GetWindowTextLength(GetDlgItem(hwnd, IDC_EDIT_NAME)) < 1)
	{
		strcpy_s(buff, sizeof(buff), "A planet has no name");
		strcpy_s(newPlanet->name, sizeof(buff), buff);
	}
	else
		strcpy_s(newPlanet->name, sizeof(newPlanet->name), buff);

	newPlanet->mass = Get_Edit_Value(hwnd, IDC_EDIT_MASS);
	newPlanet->sx = Get_Edit_Value(hwnd, IDC_EDIT_PX);
	newPlanet->sy = Get_Edit_Value(hwnd, IDC_EDIT_PY);
	newPlanet->vx = Get_Edit_Value(hwnd, IDC_EDIT_VX);
	newPlanet->vy = Get_Edit_Value(hwnd, IDC_EDIT_VY);
	newPlanet->life = (int)Get_Edit_Value(hwnd, IDC_EDIT_LIFE);
	newPlanet->next = NULL;
	strcpy_s(newPlanet->pid, 30, pid);

	if (CheckDuplicate(newPlanet->name, 1) == 0)
	{
		ClearTextboxes(hwnd);
		return;
	}
	AddPlanetToLocalList(newPlanet);
	AddToListbox(GetPlanetDialogHandle(), newPlanet->name, LBX_LOCAL);
	ClearTextboxes(hwnd);

	free(newPlanet);
}
void Get_Edit_String(HWND hwnd, INT_PTR edit, char* buff)
{
	int len = GetWindowTextLength(GetDlgItem(hwnd, edit));
	if (len > 0)
	{
		GetDlgItemText(hwnd, edit, buff, len + 1);
	}
}


double Get_Edit_Value(HWND hwnd, INT_PTR edit)
{
	double value = 0;
	int len = GetWindowTextLength(GetDlgItem(hwnd, edit));
	if (len > 0)
	{
		char buff[20];

		GetDlgItemText(hwnd, edit, buff, 20);
		if (numbersOnly(buff))
		{
			value = atof(buff);
			return value;
		}
		return 0.0; // 0 if the if statement doesnt go through
	}
	AddToListbox(GetPlanetDialogHandle(), "Missing fields have been set to 0.0", LBX_MSG);
	return 0.0; // 0
}

BOOL numbersOnly(const char *s)
{
	while (*s)
	{
		if (isdigit(*s++) == 0)
			return FALSE;

	}return TRUE;
}

HWND get_ADD_DIALOG_HANDLE()
{
	return ADD_DIALOG_hwnd;
}
