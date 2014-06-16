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

#include "tcpsocket.h"

tcpsocket::tcpsocket():
_socket("127.0.0.1", 0),
_host(""), _port(""), _opened(false), _eof(false)
{
}

tcpsocket::~tcpsocket()
{
    close();
}

string tcpsocket::ip()
{
	return iptostring(_socket.get_myip());
}

bool tcpsocket::open(const string& host, const string& port)
{
	_eof = false;
	int p = static_cast<int>(stringtoi(port));
	if(p <= 0 || p > 0x0FFFF)
		return false;

	_socket.set_host(host);
	_socket.set_port(p);
    _host = host;
    _port = port;

	try {
		_socket.open();
	} catch (estream* e) {
		close();
		_error = e->get_message();
		delete e;
		return false;
	}

	_opened = true;
	return true;
}

void tcpsocket::close()
{
	try {
		_socket.close();
	} catch (estream* e) {
		delete e;
	}
	_opened = false;
}

string tcpsocket::readln()
{
	string result = "";
	if(!_opened) return result;

	if(_socket.waitfor(0)) {
		try {
			result = _socket.line(_maxtoken);
		} catch (estream* e) {
			close();
			_error = e->get_message();
			delete e;
			_eof = true;
			return "";
		}
	}

	return result;
}

bool tcpsocket::connected()
{
	int status = _socket.get_status();
	return _opened && !(status == IO_EOF || status == IO_CLOSING || status == IO_CLOSED);
}

bool tcpsocket::writeln(const string& str)
{
	if(!connected())
		return false;

	try {
		_socket.put(str + _FSD_SEP_);
		_socket.flush();
	} catch (estream* e) {
		close();
		_error = e->get_message();
		delete e;
		return false;
	}

	return true;
}
