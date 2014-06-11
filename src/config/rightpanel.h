/****************************************************************************

    X-IvAp Configuration tool  -  Guillaume SUDRE
    Copyright (C) 2009 by Guillaume SUDRE

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

#include <wx/wx.h>
#include <wx/panel.h>
/*
For LINUX compilation, some other include may be required
to use "SELCAL" (beeeeeep) :
#include <stdlib.h>
#include <fcntl.h>
#include <linux/kd.h>
*/

const int  ID_CHKMP             = 501;
const int  ID_CHKWX             = 502;
const int  ID_CHKCOM            = 503;
const int  ID_CHKDARK           = 504;
//const int  ID_RSERVER           = 505;
const int  ID_CHKMSG            = 506;
const int  ID_TXTMSG            = 507;
const int  ID_TXTNAME           = 511;
const int  ID_TXTVID            = 512;
const int  ID_TXTPWD            = 513;
const int  ID_TXTCALL           = 514;
const int  ID_TXTBASE           = 515;
const int  ID_BRSTUSR           = 516;
const int  ID_CHKTS             = 521;
const int  ID_CHKQUITTS         = 522;
const int  ID_CHKSELCALPC       = 523;
const int  ID_BSELCALPC         = 524;
const int  ID_CHKP2P            = 531;
const int  ID_TXTP2PPORT        = 532;
const int  ID_TXTP2PSEND        = 533;
const int  ID_TXTP2PRECV        = 534;
const int  ID_TXTP2PSTUN        = 535;

class RightPanel : public wxPanel
{
public:
	RightPanel(wxPanel *parent);

	wxStaticBox* staticbox_gen;
	wxCheckBox* chk_mp;
    wxCheckBox* chk_wx;
    wxCheckBox* chk_com;
	wxCheckBox* chk_dark;
//	wxRadioBox* r_server; 
	wxCheckBox* chk_msg;
	wxStaticText* s_msg;
	wxTextCtrl* txt_msg;

	wxStaticBox* staticbox_usr;
	wxStaticText* s_usrnote;
	wxStaticText* s_call;
	wxTextCtrl* txt_call;
	wxStaticText* s_name;
	wxTextCtrl* txt_name;
	wxStaticText* s_base;
	wxTextCtrl* txt_base;
	wxStaticText* s_vid;
	wxTextCtrl* txt_vid;
	wxStaticText* s_pwd;
	wxTextCtrl* txt_pwd;
	wxButton* b_resetusr;

	wxStaticBox* staticbox_ts;
	wxCheckBox* chk_ts;
	wxCheckBox* chk_quitts;
#ifndef APPLE // = #ifdef WIN32 || LINUX
	wxStaticBox* staticbox_selcal;
    wxCheckBox* chk_selcalpc;
	wxButton* b_selcal;
#ifdef LINUX
	wxStaticBitmap* i_selcal;
	wxStaticText* s_selcal;
#endif
#endif

	wxStaticBox* staticbox_p2p;
	wxStaticText* s_p2pnote;
    wxCheckBox* chk_p2p;
	wxStaticText* s_p2pport;
	wxTextCtrl* txt_p2pport;
	wxStaticText* s_p2psendmax;
	wxTextCtrl* txt_p2psendmax;
	wxStaticText* s_p2precvmax;
	wxTextCtrl* txt_p2precvmax;
	wxStaticText* s_p2pstun;
	wxTextCtrl* txt_p2pstun;
  
	bool reset;
	void ShowGeneral();
    void HideGeneral();
	void ShowUser();
	void HideUser();
	void ShowSound();
	void HideSound();
	void ShowP2p();
	void HideP2p();
	void LoadSettings(wxString path);
	bool CheckP2pSettings(wxString *message);
	void SaveSettings();
	wxString GetConfigPath();
private:
#ifndef APPLE
	void OnSelcal(wxCommandEvent& event);
#endif
	void OnResetUsr(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnOk(wxCommandEvent & event);
	void OnP2p(wxCommandEvent & event);
};

