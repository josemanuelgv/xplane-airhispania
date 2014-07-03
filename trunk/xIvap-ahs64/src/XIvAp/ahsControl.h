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

	int numDep();

	string findDep(string freq);

	int getStatus();

private:
	
	string html;

	std::list<string> deps;
	std::list<string> freqs;

	int status;
};


#endif