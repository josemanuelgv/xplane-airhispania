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
#include <wx/snglinst.h>
#include "form.h"

/*
Dunno what is APL ?
#if	IBM
	#define DIR_CHAR	'\\'
	#define DIR_STR		"\\"
#elif (APL && __MACH__) || LIN
		#define	DIR_CHAR	'/'
		#define DIR_STR		"/"
#elif APL
		#define	DIR_CHAR	':'
		#define DIR_STR		":"
#else
	#error PLATFORM NOT DEFINED
#endif	

*/

class Config : public wxApp
{
  public:
    virtual bool OnInit();
	virtual int OnExit();
private :
	wxSingleInstanceChecker *m_checker;
};
