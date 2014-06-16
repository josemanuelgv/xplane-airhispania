/***************************************************************************
 *   Copyright (C) 2006 by Martin Domig                                    *
 *   martin@domig.net                                                      *
 *                                                                         *
 *   For license information see LICENSE.xivap in the root folder of the   *
 *   source code.                                                          *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 ***************************************************************************/

#ifndef P2PTESTER_H
#define P2PTESTER_H

#include "ui_p2ptester.h"

#define NO_PTYPES
#include "../../stund/StunClient.h"

// Main application class -----------------------

class P2PTester: public QMainWindow, private Ui::P2PTester {
Q_OBJECT

public:
	P2PTester(QWidget *parent = 0);

private slots:
	void on_pushButton_clicked();

private:
	void sendResult(Stun::StunType type, bool hairpin, bool presport);
};

#endif

