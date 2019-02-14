#ifndef PLANETLISTS_H
#define PLANETLISTS_H
#include "wrapper.h"
#include "resource.h"

extern planet_type *localroot;
extern planet_type *serverroot;

void ConvertStringToLower(char* string);
extern int CheckDuplicate(char* planet, int option);
void AddPlanetToLocalList(planet_type* planet);
void AddPlanetToServerList(planet_type* planet);
extern void RemovePlanetFromServerList(planet_type* planet);
extern void RemovePlanetFromLocalList(planet_type* planet);
extern void RemovePlanetsFromServerList();


#endif