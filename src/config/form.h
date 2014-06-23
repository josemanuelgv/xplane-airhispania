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

#include <wx/wx.h>
#include "leftpanel.h"
#include "rightpanel.h"

const int  ID_CHOICE_GENERAL    = 0;
const int  ID_CHOICE_USER       = 1;
const int  ID_CHOICE_SOUND      = 2;
const int  ID_CHOICE_P2P        = 3;
const int  ID_ABOUT             = 1001;
class Form : public wxFrame
{
public:
    Form();
    LeftPanel *m_lp;
    RightPanel *m_rp;
    wxPanel *m_parent;
    void OnClick(wxCommandEvent & event);
	void OnOk(wxCommandEvent & event);
	void OnQuit(wxCommandEvent & event);
	void OnAbout();
	void LoadSettings(wxString path);

#ifdef WIN32
    WXLRESULT MSWWindowProc ( WXUINT message, WXWPARAM wParam, WXLPARAM lParam );
#endif
};

class Panel : public wxPanel
{
public:
	Panel(wxWindow *parent, wxWindowID winid = wxID_ANY);
    void OnClick(wxCommandEvent & event);
	void OnOk(wxCommandEvent & event);
	void OnQuit(wxCommandEvent & event);
};