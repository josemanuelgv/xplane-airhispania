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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stb_image_aug.h"
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include "XPLMDataAccess.h"
#include "XPLMUtilities.h"
#include "XPLMNavigation.h"
#include "XPLMProcessing.h"
#include "fmod.h"
#include "fmod_errors.h"
#include "SoundGraphics.h"
#include "Tcas.h"
#include <math.h>
/// Sandy Barbour
/// Handles all platform OpenGL headers.
#if IBM
#include <windows.h>
#include <gl\gl.h>
#include <gl\glu.h>

#elif LIN
#define TRUE 1
#define FALSE 0
#include <GL/gl.h>
#include <GL/glu.h>
#else
#define TRUE 1
#define FALSE 0
#if __GNUC__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <gl.h>
#include <glu.h>
#endif
#endif

//revisions:
//1.00 08-10-2012 first beta release 
//1.01 09-10-2012 added transpondermode 4 to the switch, adaption for the Qpac A320 using 1,2,4
//1.02 12-10-2012 reattempt to use the old carbon lib for convertpath
//1.03 13-10-2012 fix for transpondercodes <1000
//1.04 15-10-2012 first attemt to build a extern tcasbox
//1.05 2-01-2013 integration in x64 system

// OS X: we use this to convert our file path.
#if APL
#include <Carbon/Carbon.h>

int ConvertPath(const char * inPath, char * outPath, int outPathMaxLen) {

	CFStringRef inStr = CFStringCreateWithCString(kCFAllocatorDefault, inPath ,kCFStringEncodingMacRoman);
	if (inStr == NULL)
		return -1;
	CFURLRef url = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, inStr, kCFURLHFSPathStyle,0);
	CFStringRef outStr = CFURLCopyFileSystemPath(url, kCFURLPOSIXPathStyle);
	if (!CFStringGetCString(outStr, outPath, outPathMaxLen, kCFURLPOSIXPathStyle))
		return -1;
	CFRelease(outStr);
	CFRelease(url);
	CFRelease(inStr);
	return 0;
}
#endif

char gPluginDataFileIV[255];
extern int width, height;
float soundlevel = 0.6f;
GLint IVbase;				// Base Display List For The Font
char *pFileName;
XPLMTextureID IvaoTexture[30]; //maxtexture

int	Ui_width, Ui_height;
int	xPanelWindowLeft, xPanelWindowBottom;
char Graphics = 0;

XPLMDataRef	RED = NULL, GREEN = NULL, BLUE = NULL;
XPLMDataRef gXpdrCode, gXpdrMode, gXpdrId;
FMOD_SOUND    *sound1;
FMOD_CHANNEL  *channel = 0;
FMOD_RESULT    result;
int            key;
unsigned int   version;
FMOD_DSP       *dsp = 0;
FMOD_SYSTEM    *csystem;
Tcas	Tcasbox;

int ERRCHECK(FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		XPLMDebugString(FMOD_ErrorString(result));

		return 0;

	}
	return 1;
}
////////////////////////////////////////////////

int InitSound(void)
{


	/*        Create a System object and initialize.
	*/
	result = FMOD_System_Create(&csystem);
	ERRCHECK(result);

	result = FMOD_System_GetVersion(csystem, &version);
	ERRCHECK(result);

	if (version < FMOD_VERSION)
	{

		XPLMDebugString("SoundInit Error!  You are using an old version of FMOD\r\n");
		return 0;
	}

	result = FMOD_System_Init(csystem, 32, FMOD_INIT_NORMAL, NULL);
	ERRCHECK(result);
	//XPLMDebugString(SOUNDDIR);
	//XPLMDebugString(TEXTURESDIR);
	XPLMGetSystemPath(gPluginDataFileIV);
	
	strcat(gPluginDataFileIV, RESOURCES);
	strcat(gPluginDataFileIV, XPLMGetDirectorySeparator());
	strcat(gPluginDataFileIV, PLUGINS);
	strcat(gPluginDataFileIV, XPLMGetDirectorySeparator());
	strcat(gPluginDataFileIV, RESOURCES_DIR);
	strcat(gPluginDataFileIV, XPLMGetDirectorySeparator());

	XPLMDebugString("SoundInit done\r\n");

	return 1;
}
void Playsound (char * voice)
{
	char			DirPath[1024];
	strcpy(DirPath, gPluginDataFileIV);
	strcat(DirPath, SOUND_DIR);
	strcat(DirPath, XPLMGetDirectorySeparator());
	strcat(DirPath, voice);
#if APL
	ConvertPath(DirPath, DirPath, sizeof(DirPath));
#endif

	result = FMOD_System_CreateSound(csystem, DirPath, FMOD_HARDWARE, 0, &sound1);

	if (ERRCHECK(result) == 0)
		return;

	result = FMOD_System_PlaySound(csystem, FMOD_CHANNEL_FREE, sound1, 0, &channel);
	ERRCHECK(result);

	result= FMOD_Channel_SetVolume(channel, soundlevel);
	ERRCHECK(result);
}

void CloseAudioSystem(void)
{

	result = FMOD_System_Release(csystem);
	ERRCHECK(result);

}
//////////////////////////////
/////////////////////
void	IvBuildTextures(void)
{
	
	IVbase=glGenLists(MAXTEXTURE);
	//build lnav lit
	

	glNewList(IVbase + IVPANEL_TEXTURE, GL_COMPILE);
	glPushMatrix();
	Ui_width = 670;
	Ui_height = 130;
	XPLMBindTexture2d(IvaoTexture[IVPANEL_TEXTURE], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f((float)Ui_width, 0);          // Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);                 // Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, (float)Ui_height);         // Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f((float)Ui_width,(float) Ui_height);  // Top Right Of The Texture and Quad
	glEnd();
	glPopMatrix();
	glEndList();

	glNewList(IVbase + IVTRANS_OFF_TEXTURE, GL_COMPILE);
	glPushMatrix();
	XPLMBindTexture2d(IvaoTexture[IVTRANS_OFF_TEXTURE], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f(32, 0);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, 32);	// Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f(32, 32);	// Top Right Of The Texture and Quad
	glEnd();
	glPopMatrix();
	glEndList();

	glNewList(IVbase + IVTRANS_ON_TEXTURE, GL_COMPILE);
	glPushMatrix();
	XPLMBindTexture2d(IvaoTexture[IVTRANS_ON_TEXTURE], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f(32, 0);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, 32);	// Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f(32, 32);	// Top Right Of The Texture and Quad
	glEnd();
	glPopMatrix();
	glEndList();

	glNewList(IVbase + IVCONN_ON_TEXTURE, GL_COMPILE);
	glPushMatrix();
	XPLMBindTexture2d(IvaoTexture[IVCONN_ON_TEXTURE], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f(34, 0);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, 34);	// Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f(34, 34);	// Top Right Of The Texture and Quad
	glEnd();
	glPopMatrix();
	glEndList();

	glNewList(IVbase + IVCONN_OFF_TEXTURE, GL_COMPILE);
	glPushMatrix();
	XPLMBindTexture2d(IvaoTexture[IVCONN_OFF_TEXTURE], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f(34, 0);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, 34);	// Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f(34, 34);	// Top Right Of The Texture and Quad
	glEnd();
	glPopMatrix();
	glEndList();

	glNewList(IVbase + IVNO_FPL_TEXTURE, GL_COMPILE);
	glPushMatrix();
	XPLMBindTexture2d(IvaoTexture[IVNO_FPL_TEXTURE], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f(34, 0);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, 34);	// Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f(34, 34);	// Top Right Of The Texture and Quad
	glEnd();
	glPopMatrix();
	glEndList();

	glNewList(IVbase + IVFLP_SEND_TEXTURE, GL_COMPILE);

	XPLMBindTexture2d(IvaoTexture[IVFLP_SEND_TEXTURE], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f(34, 0);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, 34);	// Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f(34, 34);	// Top Right Of The Texture and Quad
	glEnd();

	glEndList();
////////////////////////////
	glNewList(IVbase + IVIDENT_ON_TEXTURE, GL_COMPILE);

	XPLMBindTexture2d(IvaoTexture[IVIDENT_ON_TEXTURE], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f(11, 0);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, 11);	// Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f(11, 11);	// Top Right Of The Texture and Quad
	glEnd();

	glEndList();
/////////////////				
	glNewList(IVbase + IVTCAS, GL_COMPILE);

	XPLMBindTexture2d(IvaoTexture[IVTCAS], 0);
	glBegin(GL_QUADS);
	glTexCoord2f(+1, 1.0f); glVertex2f((float)Tcasbox.Size(),0);	// Bottom Right Of The Texture and Quad
	glTexCoord2f(+0, 1.0f); glVertex2f(0, 0);	// Bottom Left Of The Texture and Quad
	glTexCoord2f(+0, 0.0f); glVertex2f(0, (float)Tcasbox.Size());	// Top Left Of The Texture and Quad
	glTexCoord2f(+1, 0.0f); glVertex2f((float)Tcasbox.Size(),(float) Tcasbox.Size());	// Top Right Of The Texture and Quad
	glEnd(); 

	glEndList();
/////////////////////
	glNewList(IVbase + IVTCAS_RED, GL_COMPILE);

	glBegin(GL_QUADS);
	glColor4f(1.0f,0.0f,0.0f,1.0f); //paint red
	glVertex2i(-5, -5);
	glVertex2i(-5, 5);
	glVertex2i(5, 5);
	glVertex2i(5, -5);
	glEnd();

	glEndList();
////////
	glNewList(IVbase + IVTCAS_WHITE, GL_COMPILE);

	glBegin(GL_QUADS);
	glColor4f(1.0f,1.0f,1.0f,1.0f); //paint white
 	glVertex2i(0, -5);
 	glVertex2i(-5, 0);
 	glVertex2i(0, 5);
	glVertex2i(5, 0);
	glEnd();

	glEndList();
////////
	glNewList(IVbase + IVTCAS_RANGE, GL_COMPILE);
	glBegin(GL_POINTS);
	glPointSize(1.0f);
	glColor4f(1.0f,0.0f,0.0f,1.0f); //paint red
	glVertex2i(0, 50);	
 	glVertex2i(15, 47);	
	glVertex2i(29, 40);	
	glVertex2i(40, 29);
	glVertex2i(47, 15);
	glVertex2i(50, 0);
	glVertex2i(47, -15);
	glVertex2i(40, -29);
	glVertex2i(29, -40);
	glVertex2i(15, -47);
	glVertex2i(0, -50);
	glVertex2i(-15, -47);
	glVertex2i(-29, -40);
	glVertex2i(-40, -29);
	glVertex2i(-47, -15);
	glVertex2i(-50, 0);
	glVertex2i(-47, +15);
	glVertex2i(-40, 29);
	glVertex2i(-29, 40);
	glVertex2i(-15, 47);
	glEnd();
 	glEndList();
///////////////////////
	glNewList(IVbase + IVTCAS_YELLOW, GL_COMPILE);
	glBegin(GL_TRIANGLE_FAN);
	glColor4f(0.0f,1.0f,0.0f,1.0f);//Change the object color to yellow
 	glVertex2f(1.54f, 4.75f);	
	glVertex2f(2.93f, 4.04f);	
	glVertex2f(4.04f, 2.93f);
	glVertex2f(4.75f, 1.54f);
	glVertex2f(5, 0);
	glVertex2f(4.75f, -1.54f);
	glVertex2f(4.04f, -2.93f);
	glVertex2f(2.93f, -4.04f);
	glVertex2f(1.54f, -4.75f);
	glVertex2f(0, -5);
	glVertex2f(-1.54f, -4.75f);
	glVertex2f(-2.93f, -4.04f);
	glVertex2f(-4.04f, -2.93f);
	glVertex2f(-4.75f, -1.54f);
	glVertex2f(-5, 0);
	glVertex2f(-4.75f, +1.54f);
	glVertex2f(-4.04f, 2.93f);
	glVertex2f(-2.93f, 4.04f);
	glVertex2f(-1.54f, 4.75f);
	glEnd();
 	glEndList();
///////////////////////
	glNewList(IVbase + IVTCAS_CYAN, GL_COMPILE);

	glBegin(GL_LINE_LOOP); 
	glColor4f(0.0f,1.0f,1.0f,1.0f); //paint cyan
 	glVertex2i(0, -5);
 	glVertex2i(-5, 0);
 	glVertex2i(0, 5);
	glVertex2i(5, 0);
	glEnd(); 

	glEndList();
////////////////////
	glNewList(IVbase + IVTCAS_OWNPLANE, GL_COMPILE);
	glBegin(GL_LINES); 

	glVertex2f(0, -7); //body
 	glVertex2f(0, 7);
	glVertex2f(-5, 3); //wing
 	glVertex2f(6, 3);
	glVertex2f(-3, -4); //tail
 	glVertex2f(4, -4);
	glEnd(); 

	glEndList();
	///generate the textures for digits 0-7
	//code source: NeHe productions
	// Loop Until All 7 Are Built
	float	cx;											// Holds Our X Character Coord
	char loop;

	XPLMBindTexture2d(IvaoTexture[IVDIGITS_TEXTURES], 0);
    // Select Our Font Texture
	for (loop = 0; loop < 8; loop++)					// Loop Through All 256 Lists
	{
		cx = float(loop % 8) / 8.0f;					// X Position Of Current Character
		
		glNewList(IVbase + IVDIGITS_TEXTURES + loop, GL_COMPILE); // Start Building A List
			glBegin(GL_QUADS);							// Use A Quad For Each Character
				glTexCoord2f(cx + 0.125f,1); glVertex2f(25, 0); // Vertex Coord (Bottom Left)
				glTexCoord2f(cx, 1); glVertex2f(0, 0); 	// Vertex Coord (Bottom Right)
				glTexCoord2f(cx, 0); glVertex2f(0, 25);	// Vertex Coord (Top Right)
				glTexCoord2f(cx + 0.125f, 0); glVertex2f(25, 25); // Vertex Coord (Top Left)
			glEnd();									// Done Building Our Quad (Character)
			glTranslated(25, 0, 0);						// Move To The Right Of The Character
		glEndList();									// Done Building The Display List
	}
}
//////////////////graphics part


static int	IvaoDrawCallback(
							 XPLMDrawingPhase     inPhase,
							 int                  inIsBefore,
							 void *               inRefcon);

int InitGraphics (void)
{
		
	char xTextureFileName[255];
	gXpdrCode = XPLMFindDataRef("sim/cockpit/radios/transponder_code");
	gXpdrMode = XPLMFindDataRef("sim/cockpit/radios/transponder_mode");
	strcpy(xTextureFileName, gPluginDataFileIV);
	strcat(xTextureFileName, TEXTURE_DIR);
	strcat(xTextureFileName, XPLMGetDirectorySeparator());
	strcat(xTextureFileName, PANEL_FILENAMEDAY);
	
	//results of LoadTexture are too bad... switched to soilimage loading
	//if (!LoadTextureFromFile(xTextureFileName,IVPANEL_TEXTURE, true, false, true, NULL, NULL,3)) {XPLMDebugString("No PngFile found|r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(PANEL_FILENAMEDAY, IVPANEL_TEXTURE)) 	{XPLMDebugString("No Basetexture found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(TransponderOff, IVTRANS_OFF_TEXTURE)) 	{XPLMDebugString("No TXOFF PngFile found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(TransponderOn, IVTRANS_ON_TEXTURE)) 	{XPLMDebugString("No TXON PngFile found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(ConnectON, IVCONN_ON_TEXTURE)) 	{XPLMDebugString("No ConnectOn PngFile found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(ConnectOFF, IVCONN_OFF_TEXTURE)) 	{XPLMDebugString("No ConnectOFF PngFile found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(FLPSEND, IVFLP_SEND_TEXTURE)) 	{XPLMDebugString("No fpl off PngFile found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(NOFPLSEND, IVNO_FPL_TEXTURE)) 	{XPLMDebugString("No fpl send PngFile found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(IDENTON, IVIDENT_ON_TEXTURE)) 	{XPLMDebugString("No ident on PngFile found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(TCAS, IVTCAS)) {XPLMDebugString("No TCas PngFile found\r\n");return 0;}// message should be given
	if (!IvaoLoadGLTexture(TRANSPONDER, IVDIGITS_TEXTURES)) 	{XPLMDebugString("No Digittable PngFile found\r\n");return 0;}// message should be given
	//buidling the OpenGldisplay list
	IvBuildTextures();


	RED = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_r");
	GREEN = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_g");
	BLUE = XPLMFindDataRef("sim/graphics/misc/cockpit_light_level_b");
	//register the Texture callback
	XPLMRegisterDrawCallback(
		IvaoDrawCallback,
		xplm_Phase_Window,	 //xplm_Phase_LastCockpit,
		1,
		NULL);

	XPLMDebugString("Graphics Init done\r\n");
	return 1;
}
///this is needed to delete all textures in case the plugin is disabled
void KillFont(void) // Delete The Font From Memory
{
	glDeleteLists(IVbase, MAXTEXTURE); // Delete All 119 Display Lists
}


int IvaoLoadGLTexture(char *xpFileName, int IvaoTextureId)
{
	static int Status = FALSE;
	char xTextureFileName[255];
	int sxWidth, sxHeight,xchannels;
	int error;
	unsigned char* ximg;

	/// Need to get the actual texture path
	/// and append the filename to it.
	strcpy(xTextureFileName, gPluginDataFileIV);
	strcat(xTextureFileName, TEXTURE_DIR);
	strcat(xTextureFileName, XPLMGetDirectorySeparator());
	strcat(xTextureFileName, xpFileName);
//	XPLMDebugString(xTextureFileName);
#if APL
	ConvertPath(xTextureFileName, xTextureFileName, sizeof(xTextureFileName));
#endif
	//CreateBitmapFromPNG(xTextureFileName, ximg);
//	XPLMDebugString("loading");
//	XPLMDebugString(xTextureFileName);
//	XPLMDebugString("\r\n");
	ximg = SOIL_load_image(xTextureFileName, &sxWidth, &sxHeight, &xchannels, 3);
	XPLMGenerateTextureNumbers(&IvaoTexture[IvaoTextureId], 1);
	XPLMBindTexture2d(IvaoTexture[IvaoTextureId], 0);
	error = gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8  , sxWidth, sxHeight, GL_RGB, GL_UNSIGNED_BYTE, ximg); 
	if (error != 0)
    {
        XPLMDebugString("\r\ngluBuild2DMipmaps\r\n");
        return FALSE;
    }
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Status = TRUE;

	return Status;
}
///Squawk digit painter
GLvoid glPrint1(GLint x, GLint y, char *str)	// Where The Printing Happens
{
	XPLMBindTexture2d(IvaoTexture[IVDIGITS_TEXTURES], 0);
	XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
	glTranslated(x, y, 0);                                // Position The Text (0,0 - Bottom Left)
	glListBase(IVbase - 48);                              // Choose The Font Set (0 or 1)
	glCallLists(strlen(str), GL_UNSIGNED_BYTE, str);      // Write The Text To The Screen

}

void glPrint(int x, int y, char *str)	// Where The Printing Happens
{
	
	float modelview_matrix[16];
	glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT  | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	glMatrixMode(GL_MODELVIEW);

	XPLMSetGraphicsState(0, 1, 0, 0, 1, 0, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	XPLMBindTexture2d(IvaoTexture[IVDIGITS_TEXTURES], 0);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix);

	glPushMatrix();

	glLoadIdentity();

	glTranslated(x, y, 0);
	glListBase(IVbase + IVDIGITS_TEXTURES - 48); // Choose The Font Set (0 or 1)
	glMultMatrixf(modelview_matrix);

	glCallLists(strlen(str), GL_UNSIGNED_BYTE, str); // Write The Text To The Screen
	glPopMatrix();
	glPopAttrib();


}
/// Draws the textures that make up the gauges
static int IvaoDrawGLScene(void )
{
	float xRed = XPLMGetDataf (RED);
	float xGreen = XPLMGetDataf (GREEN);
	float xBlue = XPLMGetDataf (BLUE);
	float xPanelLeft,  xPanelBottom;
	// Transponder mode (off=0,stdby=1,on=2,test=3)
	int xm = XPLMGetDatai(gXpdrMode);
	//coordinates are taking from UI window

	xPanelLeft = (float) xPanelWindowLeft;
	xPanelBottom = (float) xPanelWindowBottom;
	
	glPushMatrix();
	glColor3f(xRed, xGreen, xBlue);
	
	glTranslated(xPanelLeft, xPanelBottom, 0);
	/// Tell Xplane what we are doing
	XPLMSetGraphicsState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 0/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);
	
	// Draw Panel
	glCallList(IVbase + IVPANEL_TEXTURE);


	//draw the subtextures using alpha blend
	XPLMSetGraphicsState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 1/*AlphaBlending*/, 0/*DepthTesting*/, 1/*DepthWriting*/);
	// Enable Smooth Shading
	glBlendFunc(GL_ONE, GL_ONE);
	
	glPushMatrix();
	glTranslated(621, 10, 0);
	//added alt report special for Qpac A320
   	if ((xm & 2)| (xm & 4)) glCallList(IVbase+IVTRANS_ON_TEXTURE); else glCallList(IVbase+IVTRANS_OFF_TEXTURE);
    
	glPopMatrix();
	glPushMatrix();
	glTranslated(526,38,0);
    
	if (Graphics & 4) glCallList(IVbase + IVCONN_ON_TEXTURE);
    else              glCallList(IVbase + IVCONN_OFF_TEXTURE);
    
	glPopMatrix();
	glPushMatrix();
	glTranslated(566, 38, 0);
    
	if (Graphics & 8) glCallList(IVbase + IVFLP_SEND_TEXTURE);
    else              glCallList(IVbase + IVNO_FPL_TEXTURE);
    
	glPopMatrix();
	
	glPushMatrix();
	glTranslated(536, 98, 0);
	if (Graphics & 16) glCallList(IVbase + IVIDENT_ON_TEXTURE);
	glPopMatrix();
	glPushMatrix();
	char text[10];
	sprintf(text, "%4.4d", XPLMGetDatai(gXpdrCode)); //fix for transpondercodes <1000
	glPrint(558, 90, text);
	glPopMatrix();
	glLoadIdentity();
	//glPushMatrix();
	glTranslated(Tcasbox.left,Tcasbox.bottom, 0);
	if (Tcasbox.ShowTcas) glCallList(IVbase + IVTCAS); //tcas picture

	glTranslated(100,100, 0); //set to center
	//disable texture drawing and switch
	XPLMSetGraphicsState(0/*Fog*/, 0/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 0/*AlphaBlending*/, 0/*DepthTesting*/, 0/*DepthWriting*/);

	glPushMatrix();
	if ((Tcasbox.range==Tcasbox.NM3) & Tcasbox.ShowTcas) glCallList(IVbase+IVTCAS_RANGE);
	glPopMatrix();
	if ((Tcasbox.tcasmode!=STBY) & Tcasbox.ShowTcas) for (char i=0;i<10;i++) if (Tcasbox.Tcaslist[i].show)  {
																glPushMatrix();
																glTranslated(Tcasbox.Tcaslist[i].x_scale,Tcasbox.Tcaslist[i].y_scale, 0);
																switch (Tcasbox.Tcaslist[i].displaymode)
																{
																	case	T_CYAN:default:	glCallList(IVbase+IVTCAS_CYAN);break;
																	case	T_YELLOW:glCallList(IVbase+IVTCAS_YELLOW);break;
																	case	T_WHITE:glCallList(IVbase+IVTCAS_WHITE);break;
																	case	T_RED:glCallList(IVbase+IVTCAS_RED);break;
																}
														
																glPopMatrix();
																}


	glColor4f(0.0f,1.0f,0.0f,1.0f);//Change the object color to yellow
	if (Tcasbox.ShowTcas) glCallList(IVbase+IVTCAS_OWNPLANE);
	/////////////////////////////
	//set everything back as we found it
	glPopMatrix();
	XPLMSetGraphicsState(0/*Fog*/, 1/*TexUnits*/, 0/*Lighting*/, 0/*AlphaTesting*/, 0/*AlphaBlending*/, 1/*DepthTesting*/, 0/*DepthWriting*/);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glFlush();

	return TRUE;
}
//////////////////////////
static unsigned char* SOIL_load_image (
                                       const char *filename,
                                       int *width,
                                       int *height,
                                       int *channels,
                                       int force_channels
                                       )
{
	unsigned char *result = stbi_load( filename, width, height, channels, force_channels );
	return result;
}

/////////////////////////////////////////////////////////////
/*
* This will draw our gauge during the Xplane gauge drawing phase.
* 
*/
static int	IvaoDrawCallback(
							 XPLMDrawingPhase inPhase,    
							 int inIsBefore,    
							 void *inRefcon
                             )

{
	if (Graphics & 1)
        IvaoDrawGLScene();

	return 1;
}
//////////////////////////////////////////////////////
//we must not forget to clean up our work before quitting
void CloseGauge()
{
	KillFont();
	XPLMUnregisterDrawCallback(
                               IvaoDrawCallback,
                               xplm_Phase_Gauges,
                               0,
                               NULL);
}
