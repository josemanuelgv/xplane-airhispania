#ifndef SoundGraphics_H_
#define SoundGraphics_H_

/****************************************************************************
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

		code: KLMVA  XFMC Project
		textures: Max XFMC Project
		www.x-fmc.com/
****************************************************************************/
int InitSound(void);
void Playsound (char * );
void CloseAudioSystem(void);
int InitGraphics (void);
void CloseGauge(void);

#define RESOURCES		"Resources" 
#define PLUGINS			"plugins"
#define RESOURCES_DIR	"X-IvAp Resources"
#define SOUND_DIR		"sound"
#define TEXTURE_DIR		"textures"
#define PANEL_FILENAMEDAY	"xivap.png" 
#define TransponderOff	"squawk_sby.png"
#define TransponderOn	"squawk_tx.png"
#define ConnectON		"net_conn.png"
#define ConnectOFF		"net_disc.png"
#define FLPSEND			"fpl_sent.png"
#define NOFPLSEND		"fpl_err.png"
#define IDENTON			"squawk_id.png"
#define TRANSPONDER		"transponder.png"
#define TCAS			"tcas.png"
#define MAXTEXTURE 24	//17
#define IVPANEL_TEXTURE 0
#define	IVTRANS_OFF_TEXTURE	1
#define	IVTRANS_ON_TEXTURE	2
#define	IVCONN_OFF_TEXTURE	3
#define	IVCONN_ON_TEXTURE	4
#define	IVNO_FPL_TEXTURE	5
#define	IVFLP_SEND_TEXTURE	6
#define	IVIDENT_OFF_TEXTURE	7
#define	IVIDENT_ON_TEXTURE	8
#define IVTCAS				9
#define	IVTCAS_WHITE		10
#define IVTCAS_YELLOW		11
#define IVTCAS_OWNPLANE		12
#define	IVTCAS_RED			13
#define IVTCAS_CYAN			14
#define IVTCAS_RANGE		15
#define	IVDIGITS_TEXTURES	16


int LoadGLTexture(char *pFileName, int TextureId);
int IvaoLoadGLTexture(char *, int);
static unsigned char*
	SOIL_load_image
	(
		const char *filename,
		int *width, int *height, int *channels,
		int force_channels
	);

static int IvaoDrawGLScene(void );
void KillFont(void);	

#endif // header