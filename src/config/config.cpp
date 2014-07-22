/****************************************************************************

    X-IvAp Configuration tool  -  Guillaume SUDRE
    Copyright (C) 2009 by Guillaume SUDRE

    For license information see LICENSE.xivap in the root folder of the
    source code.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

****************************************************************************/

#include "config.h"
#include <wx/utils.h> // ressource
IMPLEMENT_APP(Config)

bool Config::OnInit()
{
	const wxString name = wxString::Format(wxT("Config-%s"),wxGetUserId().c_str());
	m_checker = new wxSingleInstanceChecker(name); // Check is the program is already running
	if (m_checker->IsAnotherRunning()) // if the program is already running....
	{
		wxLogError(_("Program already running, aborting."));
		return false;
	}
	else
	{
	Form *form = new Form(); //call the config form...
	return true;
	}
}

int Config::OnExit()
{
delete m_checker;
return 0;
}

