/***************************************************************************
 *   Inspección del estado de la red de AHS por ahsControl                 *
 *                                                                         *
 *   Sólo para Windows                                                     *
 *   Por Jose Manuel GV (AHS018D)                                          *
 ***************************************************************************/

#ifndef AHSCONTROL_H_
#define AHSCONTROL_H_

#include <iostream>
#include <winsock2.h>
#include "common.h"
#include <list>


class AhsControl
{
public:
	AhsControl();

	void parse();

	void saveToFile();

	int countDep();

	int countTSChannels();

	string findChannel(string freq);

	int getStatus();


private:
	
	string html;	// Contenido de la página web de estado actual

	std::list<string> deps;			// Lista de dependencias abiertas
	std::list<string> freqs;		// Lista de frecuencias de cada dependencia
	std::list<string> tschannels;	// Lista de canales de TS
	std::list<string> chDep;		// Lista de canales de TS que corresponden con cada dependencia

	int status;

	const int& minimo(const int& a, const int& b);

	unsigned int lvdistance(const string& s1, const string& s2);

};


#endif