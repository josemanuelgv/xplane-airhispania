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

#ifndef TCPSOCKET_H_
#define TCPSOCKET_H_

#include "common.h"
#include "fsdmessage.h"

#define TCPSOCKET_BUFSIZE 2048

class tcpsocket {
  public:
    tcpsocket();
    ~tcpsocket();
  
    bool open(const string& host, const string& port);
	void close();

    string readln();
    bool writeln(const string& str);

	string ip();
	const ipaddress get_myip() { return _socket.get_myip(); };

	bool connected();
	bool hasdata() { return _socket.waitfor(0); };

	string errorMessage() { return _error; };
	bool eof() { return _eof; };

  private:
	ipstream _socket;
    string _host, _port;
	string _error;
	bool _opened;
	bool _eof;
        
	const static int _maxtoken = 4096;
};

#endif
