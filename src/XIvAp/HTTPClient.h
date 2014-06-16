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

#ifndef HTTPCLIENT_H_
#define HTTPCLIENT_H_

#include "common.h"
#include "tcpsocket.h"

// STL headers
#include <list>

namespace HTTP {

enum Status {
	Status_Connecting,
	Status_Downloading,
	Status_Finished,
	Status_Error
};

class HTTPClient
{
public:
	HTTPClient();

	Status Download(const string& url, const string& file, float timestamp);

	// call this once in a while until it returns Status_Finished or Status_Error
	Status status(float timestamp, float timeout);

	// returns a error message in user readable form
	string message() { return _message; };

private:
	string _filename, _message;
	tcpsocket _socket;

	typedef std::list<string> Content;
	Content _content;
	Status _status;
	bool _ok;
	bool _headers;
	float _starttime;

	void poll();
};

} // namespace

#endif