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

#include "leftpanel.h"

LeftPanel::LeftPanel(wxPanel * parent)
       : wxPanel(parent, -1, wxPoint(-1, -1), wxSize(-1, -1), wxBORDER_SUNKEN)
{
      m_hlbox = new wxSimpleHtmlListBox(this, wxID_ANY, wxDefaultPosition, wxSize(120,410),
                                          0, NULL, wxLB_SINGLE);
        
        // unlike wxHtmlListBox which is abstract, wxSimpleHtmlListBox is a
        // concrete control and doesn't support virtual mode, this we need
        // to add all of its items from the beginning
        wxArrayString arr;
            wxString label = wxString::Format(_T("General<br />"));
            arr.Add(label);
            label = wxString::Format(_T("User<br />"));
            arr.Add(label);
            label = wxString::Format(_T("Sound<br />"));
            arr.Add(label);
            label = wxString::Format(_T("Peer 2 Peer<br />"));
            arr.Add(label); 
       wxDynamicCast(m_hlbox, wxSimpleHtmlListBox)->Append(arr);
	   	Connect(wxID_ANY, wxEVT_COMMAND_LISTBOX_SELECTED, 
	  wxCommandEventHandler(LeftPanel::OnClick));
}
void LeftPanel::OnClick(wxCommandEvent & event)
{
event.Skip(); //bring up the event to the Panel class
}