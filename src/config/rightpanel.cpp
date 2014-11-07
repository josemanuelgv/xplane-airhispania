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

#include "rightpanel.h"
#include <wx/filename.h>
RightPanel::RightPanel(wxPanel * parent)
         : wxPanel(parent, wxID_ANY, wxDefaultPosition, 
         wxSize(270, 150), wxBORDER_SUNKEN)
{
	wxButton* b_ok = new wxButton(this, wxID_OK, wxT("OK"),
	wxPoint(120, 290), wxDefaultSize);
	wxButton* b_quit = new wxButton(this, wxID_EXIT, wxT("Exit"),
	wxPoint(20, 290), wxDefaultSize);

//subscribe to events

	Connect(wxID_OK, wxEVT_COMMAND_BUTTON_CLICKED, 
	  wxCommandEventHandler(RightPanel::OnOk));
	Connect(wxID_EXIT, wxEVT_COMMAND_BUTTON_CLICKED, 
	  wxCommandEventHandler(RightPanel::OnQuit));

//----------------------------------------------------------------------
// Build "General" elements
//----------------------------------------------------------------------

	staticbox_gen = new wxStaticBox(this, wxID_STATIC,
	wxT("&Global Preferences"), wxPoint(7, 7), wxSize(205, 180));

	chk_mp = new wxCheckBox(this, ID_CHKMP, wxT("Enable Multiplayer"), 
                        wxPoint(20, 30));

	chk_wx = new wxCheckBox(this, ID_CHKWX, wxT("Enable Weather"), 
                    wxPoint(20, 55));

	chk_com = new wxCheckBox(this, ID_CHKCOM, wxT("Enable Radio Coupling"), 
                    wxPoint(20, 80));

	chk_dark = new wxCheckBox(this, ID_CHKDARK, wxT("Enable Dark Console"), 
                    wxPoint(20, 105));

  /*  wxArrayString strings;
	strings.Add(wxT("Pre-select the last server used"));
	strings.Add(wxT("Pre-select the best server"));
	r_server = new wxRadioBox(this, ID_RSERVER,
	wxT("Connection"), wxPoint(7, 145), wxSize(205, -1),
	strings, 1, wxRA_SPECIFY_COLS);
*/
	chk_msg = new wxCheckBox(this, ID_CHKMSG, wxT("Display Messages"), 
                    wxPoint(20, 130));

	txt_msg = new wxTextCtrl(this, ID_TXTMSG,
	wxEmptyString, wxPoint(150, 153), wxSize(40, 20), 0, wxTextValidator(wxFILTER_NUMERIC));
	txt_msg->SetMaxLength(2);

	s_msg = new wxStaticText(this, wxID_STATIC, wxT("Time to show a message :"),
	wxPoint(20, 155), wxSize(130, 20));

//----------------------------------------------------------------------
// Build User panel
//----------------------------------------------------------------------

	staticbox_usr = new wxStaticBox(this, wxID_STATIC,
	wxT("User Credentials"), wxPoint(7, 7), wxSize(205, 270));

	s_usrnote = new wxStaticText(this, wxID_STATIC, wxT("Note: These settings are not tunable via this tool. They are set on the connect window.")
	wxT("To reset user credentials push the button at the bottom."),
	wxPoint(14, 20), wxSize(197, 65));

	s_call = new wxStaticText(this, wxID_STATIC, wxT("Callsign: "),
	wxPoint(20, 88), wxSize(40, 20));

	txt_call = new wxTextCtrl(this, ID_TXTCALL,
	wxEmptyString, wxPoint(90, 85), wxSize(70, 20),wxTE_READONLY);

	s_name = new wxStaticText(this, wxID_STATIC, wxT("Real Name:"),
	wxPoint(20, 118), wxSize(60, 20));

	txt_name = new wxTextCtrl(this, ID_TXTNAME,
	wxEmptyString, wxPoint(90, 115), wxSize(110, 20),wxTE_READONLY);
	//txt_name->ChangeValue(wxT("Guillaume SUDRE 123"));

	s_base = new wxStaticText(this, wxID_STATIC, wxT("Base Airport:"),
	wxPoint(20, 148), wxSize(70, 20));

	txt_base = new wxTextCtrl(this, ID_TXTBASE,
	wxEmptyString, wxPoint(90, 145), wxSize(70, 20),wxTE_READONLY);

	s_vid = new wxStaticText(this, wxID_STATIC, wxT("VID:"),
	wxPoint(20, 178), wxSize(70, 20));

	txt_vid = new wxTextCtrl(this, ID_TXTVID,
	wxEmptyString, wxPoint(90, 175), wxSize(70, 20),wxTE_READONLY);

	s_pwd = new wxStaticText(this, wxID_STATIC, wxT("Password:"),
	wxPoint(20, 208), wxSize(70, 20));

	txt_pwd = new wxTextCtrl(this, ID_TXTPWD,
	wxEmptyString, wxPoint(90, 205), wxSize(70, 20),wxTE_READONLY | wxTE_PASSWORD);

	b_resetusr = new wxButton(this, ID_BRSTUSR, wxT("Reset user credentials"),
	wxPoint(20, 245), wxDefaultSize);

	Connect(ID_BRSTUSR, wxEVT_COMMAND_BUTTON_CLICKED, 
	  wxCommandEventHandler(RightPanel::OnResetUsr));
//----------------------------------------------------------------------
// Build Sound elements
//----------------------------------------------------------------------

	staticbox_ts = new wxStaticBox(this, wxID_STATIC,
	wxT("&Teamspeak settings"), wxPoint(7, 7), wxSize(205, 80));

	chk_ts = new wxCheckBox(this, ID_CHKTS, wxT("Enable Teamspeak"), 
                    wxPoint(19, 30));

	chk_quitts = new wxCheckBox(this, ID_CHKQUITTS, wxT("Exit teamspeak when closing X-IvAp"), 
                    wxPoint(19, 55));
#ifndef APPLE // = #ifdef WIN32 OR LINUX
	chk_selcalpc = new wxCheckBox(this, ID_CHKSELCALPC, wxT("Use internal PC speaker on SELCAL"), 
                    wxPoint(19, 130));

	b_selcal = new wxButton(this, ID_BSELCALPC, wxT("Play SELCAL"),
	wxPoint(20, 150), wxDefaultSize);

#ifdef WIN32
	staticbox_selcal = new wxStaticBox(this, wxID_STATIC,
	wxT("&SELCAL"), wxPoint(7, 110), wxSize(205, 80));
#else // = #ifdef LINUX
#include "error.xpm"         
#include "warning.xpm"
	staticbox_selcal = new wxStaticBox(this, wxID_STATIC,
	wxT("&SELCAL"), wxPoint(7, 110), wxSize(205, 150));

	int fd = open("/dev/tty10", O_RDONLY);
	if (fd == -1) 
	{
	i_selcal = new wxStaticBitmap(this, wxID_ANY, wxBitmap(error_xpm), wxPoint(10, 180), wxDefaultSize);

    s_selcal = new wxStaticText(this, wxID_STATIC, wxT("To enable the SELCAL feature, you need write access to /dev/tty10 or you have to run X-Plane and this program as root."),
	wxPoint(55, 180), wxSize(150, 65));

	b_selcal->Disable();
	}
	else
	{
	i_selcal = new wxStaticBitmap(this, wxID_ANY, wxBitmap(warning_xpm), wxPoint(10, 180), wxDefaultSize);

    s_selcal = new wxStaticText(this, wxID_STATIC, wxT("To enable the SELCAL feature, you need write access to /dev/tty10 or you have to run X-Plane as root."),
	wxPoint(55, 180), wxSize(150, 65));
	}

#endif

//subscribe to events

	Connect(ID_BSELCALPC, wxEVT_COMMAND_BUTTON_CLICKED, 
	  wxCommandEventHandler(RightPanel::OnSelcal));

#endif //#ifndef APPLE

//----------------------------------------------------------------------
// Build "p2p" elements 
//----------------------------------------------------------------------

   	staticbox_p2p = new wxStaticBox(this, wxID_STATIC,
	wxT("Peer 2 Peer configuration"), wxPoint(7, 7), wxSize(205, 220));

	s_p2pnote = new wxStaticText(this, wxID_STATIC, wxT("Note: You should NOT change these settings unless you know what you are doing.")
	wxT("Please refer to the peer2peer documentation for more information."),
	wxPoint(18, 20), wxSize(193, 65));
    
	chk_p2p = new wxCheckBox(this, ID_CHKP2P, wxT("Enable Peer 2 Peer"), 
                        wxPoint(20, 80));

	Connect(ID_CHKP2P, wxEVT_COMMAND_CHECKBOX_CLICKED, 
	wxCommandEventHandler(RightPanel::OnP2p));

	s_p2pport = new wxStaticText(this, wxID_STATIC, wxT("Port:"),
	wxPoint(20, 108), wxSize(70, 20));

	txt_p2pport = new wxTextCtrl(this, ID_TXTP2PPORT,
	wxEmptyString, wxPoint(120, 105), wxSize(80, 20), 0, wxTextValidator(wxFILTER_NUMERIC));
	txt_p2pport->SetMaxLength(5);

	s_p2psendmax = new wxStaticText(this, wxID_STATIC, wxT("Maximum upload data rate:"),
	wxPoint(20, 131), wxSize(80, 30));

	txt_p2psendmax = new wxTextCtrl(this, ID_TXTP2PSEND,
	wxEmptyString, wxPoint(120, 135), wxSize(80, 20), 0, wxTextValidator(wxFILTER_NUMERIC));
	txt_p2psendmax->SetMaxLength(5);

	s_p2precvmax = new wxStaticText(this, wxID_STATIC, wxT("Maximum download data rate:"),
	wxPoint(20, 161), wxSize(100, 30));

	txt_p2precvmax = new wxTextCtrl(this, ID_TXTP2PRECV,
	wxEmptyString, wxPoint(120, 165), wxSize(80, 20), 0, wxTextValidator(wxFILTER_NUMERIC));
	txt_p2precvmax->SetMaxLength(5);

	s_p2pstun = new wxStaticText(this, wxID_STATIC, wxT("Stun server:"),
	wxPoint(20, 198), wxSize(100, 20));

	txt_p2pstun = new wxTextCtrl(this, ID_TXTP2PSTUN,
	wxEmptyString, wxPoint(120, 195), wxSize(80, 20));


// Hide User, Sound & p2p elements
	HideUser();
	HideSound();
	HideP2p();
	LoadSettings(GetConfigPath());
}

void RightPanel::OnQuit(wxCommandEvent& event)
{
event.Skip();
}

void RightPanel::LoadSettings(wxString path)
{	         
	wxChar *buffer;
	if(wxGetResource("ACCOUNT", "CALLSIGN", &buffer, path)) // _buffer should be a wxString
	{
		txt_call->SetValue(wxString(buffer,wxSTRING_MAXLEN)); // Bug in WxWidgets documentation ?
	}
	else
	{
		txt_call->SetValue(""); //hardcoded default value
	}

    if(wxGetResource("ACCOUNT", "REALNAME", &buffer, path))
	{
		txt_name->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_name->SetValue("");
	}

	if(wxGetResource("ACCOUNT", "BASE", &buffer, path))
	{
		txt_base->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_base->SetValue("");
	}

	if(wxGetResource("ACCOUNT", "VID", &buffer, path))
	{
		txt_vid->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_vid->SetValue("");
	}

	if(wxGetResource("ACCOUNT", "PASSWORD", &buffer, path))
	{
		txt_pwd->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_pwd->SetValue("");
	}

	if(wxGetResource("P2P", "FORCED_PORT", &buffer, path))
	{
		txt_p2pport->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_p2pport->SetValue("0");
	}

	if(wxGetResource("P2P", "MAXSENDBPS",&buffer, path))
	{
		txt_p2psendmax->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_p2psendmax->SetValue("1024");
	}
	
	if(wxGetResource("P2P", "MAXRECVBPS", &buffer, path))
	{
		txt_p2precvmax->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_p2precvmax->SetValue("5000");
	}

	if(wxGetResource("P2P", "STUNSERVER", &buffer, path))
	{
		txt_p2pstun->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_p2pstun->SetValue("stun.ivao.aero");
	}

	int buf;
	if(wxGetResource("PREFERENCES", "MULTIPLAYER", &buf, path))
	{
		if (buf==1) chk_mp->SetValue(true);
	}
	else //default value
	{
		chk_mp->SetValue(true);
	}

	if(wxGetResource("PREFERENCES", "WEATHER", &buf, path))
	{
		if (buf==1) chk_wx->SetValue(true);
	}
	else //default value
	{
		chk_wx->SetValue(true);
	}

	if(wxGetResource("PREFERENCES", "COUPLING", &buf, path))
	{
		if (buf==1) chk_com->SetValue(true);
	}
	else //default value
	{
		chk_com->SetValue(true);
	}

	if(wxGetResource("PREFERENCES", "DARK", &buf, path))
	{
		if (buf==1) chk_dark->SetValue(true);
	}
	else //default value
	{
		chk_dark->SetValue(false);
	}
/*
	if(wxGetResource("PREFERENCES", "DYNAMICSERVER", &buf, path))
	{
		if (buf==1) r_server->Select(1);
	}
	else //default value
	{
		r_server->Select(1);
	}
*/
	if(wxGetResource("PREFERENCES", "MESSAGESVISIBLE", &buf, path))
	{
		if (buf==1) chk_msg->SetValue(true);
	}
	else //default value
	{
		chk_msg->SetValue(true);
	}

	if(wxGetResource("PREFERENCES", "MESSAGESSHOWDURATION", &buffer, path))
	{
		txt_msg->SetValue(wxString(buffer,wxSTRING_MAXLEN));
	}
	else
	{
		txt_msg->SetValue("3");
	}

	if(wxGetResource("SOUND", "VOICE", &buf, path))
	{
		if (buf==1) chk_ts->SetValue(true);
	}
	else //default value
	{
		chk_ts->SetValue(true);
	}

	if(wxGetResource("SOUND", "QUITVOICE", &buf, path))
	{
		if (buf==1) chk_quitts->SetValue(true);
	}
	else //default value
	{
		chk_quitts->SetValue(true);
	}

#ifndef APPLE
	if(wxGetResource("SOUND", "PCSELCAL", &buf, path))
	{
		if (buf==1) chk_selcalpc->SetValue(true);
	}
	else //default value
	{
		chk_selcalpc->SetValue(true);
	}
#endif
	if(wxGetResource("P2P", "ENABLE", &buf, path))
	{
		if (buf==1)
		{ 
			chk_p2p->SetValue(true);
		}
		else // if p2p is disabled, disable the other fields.
		{
			txt_p2pport->Disable();
			txt_p2psendmax->Disable();
			txt_p2precvmax->Disable();
			txt_p2pstun->Disable();
		}
	}
	else //default value
	{
		chk_p2p->SetValue(true);
	}
}

bool RightPanel::CheckP2pSettings(wxString *message) 
{ // return true if there is smthg wrong with the parameters.
// Check port
	long p2pport;
	if((txt_p2pport->GetValue()).ToLong(&p2pport))
	{ //check value
		if(p2pport<0 || p2pport>65535)
		{
		*message = "The peer 2 peer port you entered seems to be wrong. Please ensure that you entered a value between 0 and 65535.";
		return true;
		}
	}
	else // the whole string is not a number
	{
		*message = "The peer 2 peer port you entered seems to be wrong. Please ensure that you entered a value between 0 and 65535.";
		return true;
	}
// Check max send bds
	long p2psendmax;
	if((txt_p2psendmax->GetValue()).ToLong(&p2psendmax))
	{
		if(p2psendmax<1 || p2psendmax>50000)
		{
		*message = "The maximum data rate (bytes per second) used to send p2p position you entered seems to be wrong. Please ensure that you entered a value between 1 and 50000.";
		return true;
		}
	}
	else // the whole string is not a number
	{
		*message = "The maximum data rate (bytes per second) used to send p2p position you entered seems to be wrong. Please ensure that you entered a value between 1 and 50000.";
		return true;
	}

// Check max recv bds
	long p2precvmax;
	if((txt_p2precvmax->GetValue()).ToLong(&p2precvmax))
	{
		if(p2precvmax<1 || p2precvmax>50000)
		{
		*message = "The maximum data rate (bytes per second) used to receive p2p position you entered seems to be wrong. Please ensure that you entered a value between 1 and 50000.";
		return true;
		}
	}
	else // the whole string is not a number
	{
		*message = "The maximum data rate (bytes per second) used to receive p2p position you entered seems to be wrong. Please ensure that you entered a value between 1 and 50000.";
		return true;
	}
	return false;
}

void RightPanel::SaveSettings()
{
	wxString path = GetConfigPath();
	bool exist = wxFileExists(path); //check if the file exist
	wxWriteResource("PREFERENCES", "MULTIPLAYER", chk_mp->GetValue(), path);
	wxWriteResource("PREFERENCES", "WEATHER", chk_wx->GetValue(), path);
	wxWriteResource("PREFERENCES", "COUPLING", chk_com->GetValue(), path);
	wxWriteResource("PREFERENCES", "DARK", chk_dark->GetValue(), path);
//	wxWriteResource("PREFERENCES", "DYNAMICSERVER", r_server->GetSelection(), path);
	wxWriteResource("PREFERENCES", "MESSAGESVISIBLE", chk_msg->GetValue(), path);
	wxWriteResource("PREFERENCES", "MESSAGESSHOWDURATION", txt_msg->GetValue(), path);
	wxWriteResource("SOUND", "VOICE", chk_ts->GetValue(), path);
	wxWriteResource("SOUND", "QUITVOICE", chk_quitts->GetValue(), path);
#ifndef APPLE // #ifdef LINUX OR WIN
	wxWriteResource("SOUND", "PCSELCAL", chk_selcalpc->GetValue(), path);
#endif
	if(exist==false || reset==true) //if reset credits OR file doesn't exist creat
	{                              // these 3 entries otherwise do not edit them.
		wxWriteResource("ACCOUNT", "HIDEADM", "0", path);
		wxWriteResource("ACCOUNT", "REMEMBER", "1", path);
		wxWriteResource("ACCOUNT", "PORT", "6809", path);
		wxWriteResource("ACCOUNT", "SERVER", "", path);
	}
	wxWriteResource("ACCOUNT", "BASE", txt_base->GetValue(), path);
	wxWriteResource("ACCOUNT", "REALNAME", txt_name->GetValue(), path);
	wxWriteResource("ACCOUNT", "PASSWORD", txt_pwd->GetValue(), path);
	wxWriteResource("ACCOUNT", "VID", txt_vid->GetValue(), path);
	wxWriteResource("ACCOUNT", "CALLSIGN", txt_call->GetValue(), path);
	wxWriteResource("P2P", "ENABLE", chk_p2p->GetValue(), path);
	wxWriteResource("P2P", "FORCED_PORT", txt_p2pport->GetValue(), path);
	wxWriteResource("P2P", "MAXSENDBPS", txt_p2psendmax->GetValue(), path);
	wxWriteResource("P2P", "MAXRECVBPS", txt_p2precvmax->GetValue(), path);
	wxWriteResource("P2P", "STUNSERVER", txt_p2pstun->GetValue(), path);
}

void RightPanel::OnOk(wxCommandEvent& event)
{
	if(chk_p2p->GetValue() == true)
	{
		wxString message;
		if(CheckP2pSettings(&message))
		{
		    wxMessageBox(_T(message),
             _T("Error in p2p settings"),
             wxOK | wxICON_WARNING,
             this);
		}
		else //p2p settings are correct
		{
			SaveSettings();
			event.Skip();
		}
	}
}
#ifndef APPLE 
void RightPanel::OnSelcal(wxCommandEvent& WXUNUSED(event))
{
#ifdef WIN32
	Beep(300,500);
	Beep(1400,500);
#else // = #ifdef LINUX
	int fd = open("/dev/tty10", O_RDONLY);
	ioctl(fd, KDMKTONE, (atoi(500)<<16)+(1193180/atoi(300)));
	ioctl(fd, KDMKTONE, (atoi(500)<<16)+(1193180/atoi(1400)));
#endif
}
#endif

void RightPanel::OnResetUsr(wxCommandEvent& WXUNUSED(event))
{
	txt_base->SetValue("");
	txt_name->SetValue("");
	txt_pwd->SetValue("");
	txt_vid->SetValue("");
	txt_call->SetValue("");
	reset = true;
}

wxString RightPanel::GetConfigPath()
{
	wxFileName fname;
	static wxString separator = fname.GetPathSeparators();
#ifdef WIN32 // on WIN32 GetPathSeparators method returns "\/". Thus we remove the "/".
	separator = separator.Truncate(1);
#endif
	wxString path;
	path = wxGetCwd() + separator + "X-IvAp.conf";
	return path;
}

void RightPanel::OnP2p(wxCommandEvent& event)
{ //enable or disable p2p textfields when the user (un)check "enable p2p"
	if(event.GetInt()==1)
	{
		txt_p2pport->Enable();
		txt_p2psendmax->Enable();
		txt_p2precvmax->Enable();
		txt_p2pstun->Enable();
	}
	else
	{
		txt_p2pport->Disable();
		txt_p2psendmax->Disable();
		txt_p2precvmax->Disable();
		txt_p2pstun->Disable();
	}
}


void RightPanel::ShowGeneral()
{
	staticbox_gen->Show();
	chk_mp->Show();
	chk_wx->Show();
	chk_com->Show();
	chk_dark->Show();
//	r_server->Show();
	chk_msg->Show();
	s_msg->Show();
	txt_msg->Show();
}

void RightPanel::HideGeneral()
{
	staticbox_gen->Hide();
	chk_mp->Hide();
	chk_wx->Hide();
	chk_com->Hide();
	chk_dark->Hide();
//	r_server->Hide();
	chk_msg->Hide();
	s_msg->Hide();
	txt_msg->Hide();
}

void RightPanel::ShowUser()
{
	staticbox_usr->Show();
	s_usrnote->Show();
	s_name->Show();
	txt_name->Show();
	s_vid->Show();
	txt_vid->Show();
	s_pwd->Show();
	txt_pwd->Show();
	s_call->Show();
	txt_call->Show();
	s_base->Show();
	txt_base->Show();
	b_resetusr->Show();
}

void RightPanel::HideUser()
{
	staticbox_usr->Hide();
	s_usrnote->Hide();
	s_name->Hide();
	txt_name->Hide();
	s_vid->Hide();
	txt_vid->Hide();
	s_pwd->Hide();
	txt_pwd->Hide();
	s_call->Hide();
	txt_call->Hide();
	s_base->Hide();
	txt_base->Hide();
	b_resetusr->Hide();
	
}

void RightPanel::ShowSound()
{
	staticbox_ts->Show();
	chk_ts->Show();
	chk_quitts->Show();
#ifndef APPLE
	staticbox_selcal->Show();
	chk_selcalpc->Show();
	b_selcal->Show();
#ifdef LINUX 
	i_selcal->Show();
	s_selcal->Show();
#endif
#endif
}

void RightPanel::HideSound()
{
	staticbox_ts->Hide();
	chk_ts->Hide();
	chk_quitts->Hide();
#ifndef APPLE
	staticbox_selcal->Hide();
	chk_selcalpc->Hide();
	b_selcal->Hide();
#ifdef LINUX 
	i_selcal->Hide();
	s_selcal->Hide();
#endif
#endif
}

void RightPanel::ShowP2p()
{
	staticbox_p2p->Show();
	s_p2pnote->Show();
	chk_p2p->Show();
	s_p2pport->Show();
	txt_p2pport->Show();
	s_p2psendmax->Show();
	txt_p2psendmax->Show();
	s_p2precvmax->Show();
	txt_p2precvmax->Show();
	s_p2pstun->Show();
	txt_p2pstun->Show();
}

void RightPanel::HideP2p()
{
	staticbox_p2p->Hide();
	s_p2pnote->Hide();
	chk_p2p->Hide();
	s_p2pport->Hide();
	txt_p2pport->Hide();
	s_p2psendmax->Hide();
	txt_p2psendmax->Hide();
	s_p2precvmax->Hide();
	txt_p2precvmax->Hide();
	s_p2pstun->Hide();
	txt_p2pstun->Hide();
}