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

#include "fsdmessage.h"
#include "helpers.h"

namespace FSD{

string nextToken(string& str)
{
	int p = pos(':', str);
	if(p == -1 && length(str) > 0) {
		string result = str;
		str = "";
		return result;
	}
	string result = copy(str, 0, p);
	del(str, 0, p+1);
	return result;
}

Message::Message(const string& str):
	encoded(str), source(""), dest(""), type(0)
{
	if(length(str) > 0)
		decompose(str);
}

string Message::compose(bool colon_check)
{
	if(type <= 0 || type >= NUM_FSD_HEADS)
		return "";
	string result = FSD_HEADS[type] + strupcase(trim(source)) + ":" + strupcase(trim(dest));
	if(colon_check) {
		for(unsigned i = 0; i < tokens.size(); ++i)
			result += ":" + stripcolons(tokens[i]);
	} else {
		for(unsigned i = 0; i < tokens.size(); ++i)
			result += ":" + tokens[i];
	}
	return result;
}

bool Message::decompose(string str)
{
	encoded = str;
	str += ":";	// append one ":", this makes sure that we get all tokens
				// out of a packet if the last field should be empty
	// find head
	type = 0;
	for(int i = 1; i < NUM_FSD_HEADS && type == 0; ++i)
		// if str begins with...
		if(copy(str, 0, length(string(FSD_HEADS[i]))) == FSD_HEADS[i]) type = i;
	if(type == 0)
		return false;
	del(str, 0, length(FSD_HEADS[type]));

	source = nextToken(str);
	dest = nextToken(str);

	tokens.clear();
	while(length(str) > 0)
		tokens.push_back(nextToken(str));

	return true;
}

} // namespace FSD