/*
For all operations on the list of planets, both for local in client side and
for the server side operations.
*/
#include "PlanetLists.h"

planet_type *localroot = NULL;
planet_type *serverroot = NULL;

//option int value 1 = localroot and 0 = serverroot
int CheckDuplicate(char * planet, int option)
{
	planet_type *root;
	if (option == 1)
		root = localroot;
	else
		root = serverroot;

	ConvertStringToLower(planet);
	int flag;
	if (root == NULL)
	{
		return 1;
	}
	planet_type *current = root;
	while (current->next != NULL)
	{
		flag = strcmp(current->name, planet);
		if (flag == 0)
			return 0;
		current = current->next;
	}
	flag = strcmp(current->name, planet);
	if (flag == 0)
		return 0;
	return 1;
}

void ConvertStringToLower(char* string)
{
	for (; *string; ++string) *string = tolower(*string);
}

void AddPlanetToLocalList(planet_type* planet)
{
	planet_type *newPlanet = malloc(sizeof(planet_type));
	memcpy(newPlanet, planet, sizeof(planet_type));
	if (localroot == NULL)
	{
		localroot = newPlanet;
	}
	else
	{
		planet_type *current = localroot;
		while (current->next != NULL)
		{
			current = current->next;
		}
		current->next = newPlanet;
	}
}

void AddPlanetToServerList(planet_type* planet)
{
	planet_type *newPlanet = malloc(sizeof(planet_type));
	memcpy(newPlanet, planet, sizeof(planet_type));
	
	newPlanet->next = NULL; // Lets not add a whole friggen list here

	if (serverroot == NULL)
	{
		serverroot = newPlanet;
	}
	else
	{
		planet_type *current = serverroot;
		while (current->next != NULL)
		{
			current = current->next;
		}
		current->next = newPlanet;
	}
}

void RemovePlanetFromLocalList(planet_type* planet)
{
	planet_type *current = localroot;
	planet_type *swapper = NULL;
	if (planet == localroot)
	{
		localroot = localroot->next;
		// TODO: free föregående root här?
	}
	else
	{
		while (current->next != NULL)
		{
			if (planet == current->next)
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
	}
}

void RemovePlanetFromServerList(planet_type* planet)
{
	planet_type *current = serverroot;
	planet_type *swapper = NULL;
	if (planet == serverroot)
	{
		serverroot = serverroot->next;
		// TODO: free föregående root här?
	}
	else
	{
		while (current->next != NULL)
		{
			if (planet == current->next)
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
	}
}

void RemovePlanetsFromServerList()
{
	if (serverroot != NULL)
	{
		planet_type *current = serverroot;
		planet_type *planetToFree;
		while (current->next != NULL)
		{
			planetToFree = current;
			current = current->next;
			planetToFree->next = NULL;
			free(planetToFree);
		}
		free(current);
		serverroot = NULL;
	}
}