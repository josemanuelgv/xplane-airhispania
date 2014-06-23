# Change history:
#  30-12-2009 TDG - Changed compiler [options] to compile on OS X 10.6 / Darwin
#                   10.x
#  12-12-2011 SD2/FJa - End of PPC support
#  28-09-2012 SD2/FJa - OSX 10.8.1 support (Xcode 4.5)
#  24-12-2012 changed makefiles to x64 support; inserted new 2.1 libs
#  10-11-2013 BvK adapted makesfile for 64 bit support

all: X-IvAp

OS:=$(shell uname)
CC="g++${GCCVER}" # Mind specifics for Darwin 10.x
LD="gcc${GCCVER}" # Mind specifics for Darwin 10.x

TOP=../..
WD=$(shell cd ${TOP};echo `pwd`)

# X-IvAp source files
cpp_SRC+=${WD}/src/XIvAp/atcPosition.cpp
cpp_SRC+=${WD}/src/XIvAp/configFile.cpp
cpp_SRC+=${WD}/src/XIvAp/helpers.cpp
cpp_SRC+=${WD}/src/XIvAp/connectForm.cpp
cpp_SRC+=${WD}/src/XIvAp/disconnectForm.cpp
cpp_SRC+=${WD}/src/XIvAp/flightplanForm.cpp
cpp_SRC+=${WD}/src/XIvAp/fsdapi.cpp
cpp_SRC+=${WD}/src/XIvAp/fsdmessage.cpp
cpp_SRC+=${WD}/src/XIvAp/IVAOsecurity.cpp
cpp_SRC+=${WD}/src/XIvAp/systemcallbacks.cpp
cpp_SRC+=${WD}/src/XIvAp/tcpsocket.cpp
cpp_SRC+=${WD}/src/XIvAp/TeamSpeak.cpp
cpp_SRC+=${WD}/src/XIvAp/uiWindow.cpp
cpp_SRC+=${WD}/src/XIvAp/weatherGod.cpp
cpp_SRC+=${WD}/src/XIvAp/weatherPosition.cpp
cpp_SRC+=${WD}/src/XIvAp/xivap.cpp
cpp_SRC+=${WD}/src/XIvAp/interpolator.cpp
cpp_SRC+=${WD}/src/XIvAp/multiplayer.cpp
cpp_SRC+=${WD}/src/XIvAp/multiplayerPilot.cpp
cpp_SRC+=${WD}/src/XIvAp/HTTPclient.cpp
cpp_SRC+=${WD}/src/XIvAp/messageWindow.cpp
cpp_SRC+=${WD}/src/XIvAp/chatWindow.cpp
cpp_SRC+=${WD}/src/XIvAp/awyFinder.cpp
cpp_SRC+=${WD}/src/XIvAp/aircraftDB.cpp
cpp_SRC+=${WD}/src/XIvAp/airlinesDB.cpp
cpp_SRC+=${WD}/src/XIvAp/FMCform.cpp
cpp_SRC+=${WD}/src/XIvAp/XPmessageBox.cpp
cpp_SRC+=${WD}/src/XIvAp/refInspector.cpp
cpp_SRC+=${WD}/src/XIvAp/TLVpacket.cpp
cpp_SRC+=${WD}/src/XIvAp/messageHandler.cpp
cpp_SRC+=${WD}/src/XIvAp/navQuery.cpp
cpp_SRC+=${WD}/src/XIvAp/WristWatch.cpp
cpp_SRC+=${WD}/src/XIvAp/planeposition.cpp
cpp_SRC+=${WD}/src/XIvAp/SoundGraphics.cpp
cpp_SRC+=${WD}/src/XIvAp/Tcas.cpp
cpp_SRC+=${WD}/src/stund/StunClient.cpp



# source files from the multiplayer SDK
cpp_SRC+=${WD}/SDK/multiplayer/src/BitmapUtils.cpp
#cpp_SRC+=${WD}/SDK/multiplayer/src/PlatformUtils.lin.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/TexUtils.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XObjDefs.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XObjReadWrite.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XOGLUtils.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPMultiplayer.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPMultiplayerCSL.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPMultiplayerObj.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPMultiplayerVars.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XPMPPlaneRenderer.cpp
cpp_SRC+=${WD}/SDK/multiplayer/src/XUtils.cpp
# cpp_SRC+=${WD}/SDK/multiplayer/src/CSLLoaderThread.cpp


cpp_SRC+=${WD}/src/XIvAp/stb_image_aug.cpp

OBJS+=$(c_SRC:.c=.c.o)

OBJS+=$(cpp_SRC:.cpp=.cpp.o)

CFLAGS+= -DXPLM200 -DIVAO -iquote. -iquote.. -iquote../.. -iquote../../.. \
 -iquote../../SDK/CHeaders/XPLM \
 -iquote../../SDK/CHeaders/Widgets \
 -iquote../XIvAp \
 -iquote../ptypes/include \
 -iquote../stund \
 -iquote../../SDK \
 -iquote../../SDK/multiplayer \
 -iquote../../SDK/multiplayer/src \
 -iquote../../SDK/multiplayer/include \
 -I/usr/include \

DBG=-g

CFLAGS+=-fpermissive -fno-stack-protector -Wall -w -x c++ -ansi

LIBS+=${WD}/src/stund/stun.o
LIBS+=${WD}/src/stund/udp.o  

#LIBS+=-lGL -lGLU -L${WD}/src/ptypes/lib -lptypesn -lpng
#LIBS+=-L${WD}/src/ptypes/lib -L${WD}/src/png/ -lptypes -lpng -lz
LIBS+=-lz ${WD}/src/ptypes/lib/libptypes.a

XIVAP_DEPENDS=ptypes stund zlib png



# Handle Linux/OS X specific stuff here.
ifeq (${OS}, Linux)
	ifeq (${ARCH}, 32)
	FMODEX=fmodex
	endif
	ifeq (${ARCH}, 64)
	FMODEX=fmodex64
	CFLAGS+=-DX64
	endif
	cpp_SRC+=${WD}/SDK/multiplayer/src/PlatformUtils.lin.cpp

	CFLAGS+=-m${ARCH} -DLINUX -DLIN=1 -Dlinux -O3 -Wall -fno-strict-aliasing -iquote../FMOD/FMODLinux/api/inc
    #LDFLAGS+=-shared -m${ARCH} -rdynamic -nodefaultlibs -fvisibility=hidden -lGL -lGLU 
	LDFLAGS+=-shared -m${ARCH} -rdynamic -fvisibility=hidden -lGL -lGLU
	LIBS+=-lpng -L../png/
    LIBS+=-lz -L../zlib/
	LIBS+=../../WidgetLibrary/Binaries/LinuxGCC/XPWidgetsEx.lib
	LIBS+=-Wl,-rpath,'$$ORIGIN' -shared
	LIBS+= -L../FMOD/FMODLinux/api/lib -l${FMODEX}
    # link libstdc++ statically so we don't conflict with ATI OpenGL
    # this may require a libstdc++.a or gcc 4.0, not sure which
    #LIBS+=-Wl,-Bstatic -lstdc++ -Wl,-Bdynamic
	LIBS+=-Wl,--version-script=${WD}/src/Linux/linker.script
	FINAL=-MMD

    # Gcc 3.4 doesn't seem to like -iquote. We'll assume linux systems to be 3.x, or if thay are
    # 4.x, able to tollerate -I includes.
	CFLAGS := $(subst -iquote,-I,$(CFLAGS))
	XIVAP_DEPENDS+= XPWidgetsEx_lib
endif

ifeq (${OS}, Darwin)
	ifeq (${ARCH}, 32)
	FMODEX=fmodex
	endif
	ifeq (${ARCH}, 64)
	FMODEX=fmodex
	CFLAGS+=-DX64
	endif
        OS_VER=$(shell uname -r | sed 's/\([0-9]*\).*/\1/')
        ifeq (${OS_VER}, 10)
            CC=g++-4.0
            LD=gcc-4.0
        endif
	LIBS+=${WD}/src/png/libpng.a
	LIBS+= -L../FMOD/FMODMac/api/lib -l${FMODEX} 
	cpp_SRC+=${WD}/SDK/XPListBox.cpp
	cpp_SRC+=${WD}/SDK/XPPopups.cpp
	cpp_SRC+=${WD}/SDK/multiplayer/src/PlatformUtils.mac.cpp
	# teamspeex can be driven by URL
	#cpp_SRC+=${WD}/src/XIvAp/TeamSpeak.cpp
	CFLAGS+= -m${ARCH} -DAPPLE -DAPL=1 -DLIN=0 -DIBM=0 -O3 -fpascal-strings \
	 -I../MacIncludes \
	  -funsigned-char \
	  -mmacosx-version-min=10.6 \
	  -iquote../FMOD/FMODMac/api/inc \
	  -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.5.sdk 
	LDFLAGS+=-m${ARCH} -flat_namespace -undefined suppress
	LDFLAGS+= -bundle -framework Carbon -framework OpenGL -framework ApplicationServices
	LDFLAGS+= -mmacosx-version-min=10.6
	LDFLAGS+= -Wl,-syslibroot,/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.5.sdk
	FINAL=
endif


clean:
	rm -f ${OBJS}
	rm -f $(cpp_SRC:.cpp=.cpp.d)
	rm -f Release/Resources/plugins/X-IvAp-32.xpl
	rm -f Release/Resources/plugins/X-IvAp-64.xpl

# This cleans the supporting libraries too.
distclean: clean
ifeq (${OS}, Darwin)
	cp ${WD}/src/png/scripts/makefile.darwin ${WD}/src/png/Makefile
endif
ifeq (${OS}, Linux)
	cp ${WD}/src/png/scripts/makefile.linux ${WD}/src/png/Makefile
endif
	cd ${WD}/src/ptypes/ ; ${MAKE} clean
	cd ${WD}/src/png/ ; ${MAKE} clean
	cd ${WD}/src/stund/ ; ${MAKE} clean
	cd ${WD}/src/zlib/ ; ${MAKE} clean
	rm -f ${WD}/src/png/Makefile

X-IvAp: $(XIVAP_DEPENDS)
	$(MAKE) X-IvAp.xpl TARGET=X-IvAp.xpl\
	AR="ar -crs"  SIZE="size"

X-IvAp.xpl: ${OBJS}
	${LD} ${LDFLAGS} -o Release/Resources/plugins/X-IvAp-${ARCH}.xpl ${OBJS} ${LIBS}

%.cpp.o: %.cpp
	${CC} -c -fPIC ${CFLAGS} $< -o $@ ${FINAL}

# Supporting libraries:
ptypes:	${WD}/src/ptypes/lib/libptypes.a

stund:
	@echo "Building stun objects"
	cd ${WD}/src/stund/ ; ${MAKE}

${WD}/src/ptypes/lib/libptypes.a:
	@echo "Building ptypes library"
	cd ${WD}/src/ptypes/ ; ${MAKE}

png: ${WD}/src/png/libpng.a

${WD}/src/png/libpng.a:
ifeq (${OS}, Darwin)
	cp ${WD}/src/png/scripts/makefile.darwin ${WD}/src/png/Makefile
endif
ifeq (${OS}, Linux)
	cp ${WD}/src/png/scripts/makefile.linux ${WD}/src/png/Makefile
endif
	cd ${WD}/src/png/ ; ${MAKE}

XPWidgetsEx_lib:
	@echo "Building WidgetLibrary objects"
	cd ${WD}/WidgetLibrary/Projects/LinuxGCC ; ${MAKE}

zlib: ${WD}/src/zlib/libz.a

${WD}/src/zlib/libz.a:
ifeq (${OS}, Linux)
	cd ${WD}/src/zlib ; ${MAKE}
endif
