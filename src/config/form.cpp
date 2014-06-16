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

#include "form.h"

Form::Form()
       : wxFrame(NULL, wxID_ANY, "X-IvAp configurator", wxDefaultPosition, wxSize(350, 370), wxSYSTEM_MENU | wxCAPTION | wxCLOSE_BOX)
{
	m_parent = new Panel(this, wxID_ANY);

  wxBoxSizer *hbox = new wxBoxSizer(wxHORIZONTAL);

  m_lp = new LeftPanel(m_parent);
  m_rp = new RightPanel(m_parent);
  hbox->Add(m_lp, 1, wxEXPAND | wxALL, 4);
  hbox->Add(m_rp, 2, wxEXPAND | wxALL, 4);

  m_parent->SetSizer(hbox);
	   	Connect(wxID_ANY, wxEVT_COMMAND_LISTBOX_SELECTED, 
	  wxCommandEventHandler(Form::OnClick));
	   	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, 
	  wxCommandEventHandler(Form::OnOk));
		Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, 
	  wxCommandEventHandler(Form::OnQuit));
#ifdef WIN32
	HMENU pSysMenu = (HMENU )GetSystemMenu((HWND)this->m_hWnd,  FALSE);
	if (pSysMenu != NULL)
	{
		AppendMenu(pSysMenu, MF_SEPARATOR, NULL, NULL);
		AppendMenu(pSysMenu, MF_STRING|MF_BYPOSITION, ID_ABOUT, "&About...");
	}
#endif
   Centre();
   Show();
}

void Form::OnClick(wxCommandEvent & event)
{
	switch(event.GetInt())
	{
		case ID_CHOICE_GENERAL:
            {
				m_rp->ShowGeneral();
				m_rp->HideUser();
				m_rp->HideSound();
				m_rp->HideP2p();
				break;
			}
		case ID_CHOICE_USER:
            {
				m_rp->HideGeneral();
				m_rp->ShowUser();
				m_rp->HideSound();
				m_rp->HideP2p();
				break;
            }
		case ID_CHOICE_SOUND:
            {
				m_rp->HideGeneral();
				m_rp->HideUser();
				m_rp->ShowSound();
				m_rp->HideP2p();
				break;
            }
		case ID_CHOICE_P2P:
			{
				m_rp->HideGeneral();
				m_rp->HideUser();
				m_rp->HideSound();
				m_rp->ShowP2p();
				break;
            }
	} //switch
}

void Form::OnOk(wxCommandEvent & WXUNUSED(event))
{
Close(true);
}

void Form::OnQuit(wxCommandEvent & WXUNUSED(event))
{
Close(true);
}


#ifdef WIN32
WXLRESULT Form::MSWWindowProc ( WXUINT message, WXWPARAM wParam, WXLPARAM lParam )
{
        switch ( message )
        {
		case WM_SYSCOMMAND:
			if(wParam == ID_ABOUT)
			{
				OnAbout();
			}
			break;
        } // switch
        return wxFrame::MSWWindowProc ( message, wParam, lParam );
}
#endif


void Form::OnAbout()
{
	wxMessageBox(_T("Idea and programming:")
		         _T("\n")
                 _T("Guillaume Sudre")
				 _T("\n")
				 _T("\n")
				 _T("Many Thanks to:")
				 _T("\n")
				 _T("- Martin Domig for making a GPL version of X-IvAp.")
				 _T("\n")
				 _T("- Kenny Moens for his help and his trust in my project.")
				 _T("\n")
				 _T("\n")
				 _T("This program was written in C++ with WxWidgets.")
				 _T("\n")
				 _T("\n")
				 _T("'It is possible to fly without motors, but not without knowledge and skill.' Wilbur Wright")
				 _T("\n")
				 _T("\n")
                 _T("� 2009 Guillaume Sudre"),
                 _T("About X-IvAp configurator"),
                 wxOK | wxICON_INFORMATION,
                 this);
}

Panel::Panel(wxWindow *parent, wxWindowID winid)
      :wxPanel(parent, wxID_ANY)
{
	   	Connect(wxID_ANY, wxEVT_COMMAND_LISTBOX_SELECTED, 
	  wxCommandEventHandler(Panel::OnClick));
		Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, 
	  wxCommandEventHandler(Panel::OnOk));
		Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, 
	  wxCommandEventHandler(Panel::OnQuit));
}

void Panel::OnClick(wxCommandEvent &event)
{
	event.Skip(); //bring up the event to the Form class	
}

void Panel::OnOk(wxCommandEvent &event)
{
	event.Skip(); //bring up the event to the Form class	
}

void Panel::OnQuit(wxCommandEvent &event)
{
	event.Skip(); //bring up the event to the Form class	
}