# Change history:
#  30-12-2009 TDG - Added Darwin version check to call correct compiler on 10.6
#  08-01-2010 TDG - deployment target specified.

TARS= client.cxx  server.cxx  stun.cxx  stun.h  tlsServer.cxx  udp.cxx  udp.h \
	Makefile rfc3489.txt\
	client.sln  client.vcproj  server.sln  server.vcproj  Stun.sln \
	id.pem id_key.pem root.pem \
	nattestwarning.txt nattest wnattest.bat \
	WinStun/resource.h WinStun/stdafx.cpp WinStun/stdafx.h \
	WinStun/WinStun.cpp WinStun/WinStunDlg.cpp WinStun/WinStunDlg.h WinStun/WinStun.h \
	WinStun/WinStun.rc WinStun/WinStun.vcproj \
	WinStun/res/WinStun.ico WinStun/res/WinStun.manifest WinStun/res/WinStun.rc2 \
	WinStunSetup/WinStunSetup.vdproj

# if you chnage this version, change in stun.h too 
VERSION=0.96

CFLAGS+=-fno-stack-protector 
CXXFLAGS+=-O2 -m${ARCH} -fPIC -fno-stack-protector -Wall -x c++ -ansi
LDFLAGS+=-O2 -Wall -m${ARCH}
STUNLIB=libstun.a

#
# Alternatively, for debugging.
#
#CXXFLAGS+=-g -O -Wall -m${ARCH} -fPIC
#LDFLAGS+=-g -O -Wall
# for solaris
#LDFLAGS+= -lnsl -lsocket

OS:=$(shell uname)
ifeq (${OS}, Darwin)
    CXXFLAGS+= -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk -mmacosx-version-min=10.6
    CFLAGS+= -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk -mmacosx-version-min=10.6
    LDFLAGS+= -Wl,-syslibroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk -mmacosx-version-min=10.6
    
    OS_VER=$(shell uname -r | sed 's/\([0-9]*\).*/\1/')
    ifeq (${OS_VER}, 10)
        CC=gcc-4.0
        CXX=g++-4.0
    endif
endif

all: server client watchdog StunClient.o

clean:
	- rm -f *.o server client tlsServer watchdog

tar: $(TARS)
	cd ..; tar cvfz `date +"stund/stund_$(VERSION)_$(PROG)%b%d.tgz"` \
			 $(addprefix stund/, $(TARS))

server: server.o stun.o udp.o 
	$(CXX) $(LDFLAGS) -o $@  $^

tlsServer: tlsServer.o stun.o udp.o
	$(CXX) $(LDFLAGS) -o $@  $^

client: client.o stun.o udp.o 
	$(CXX) $(LDFLAGS) -o $@  $^

%.o:%.cxx
	$(CXX)  -c $(CPPFLAGS) $(CXXFLAGS) $< -o $@

libstun.a: stun.o udp.o
	ar rv $@ $<

%:RCS/%
	co $@

# Dependancies
server.o: stun.h udp.h 
client.o: stun.h udp.h 
stun.o: stun.h udp.h
udp.o: stun.h udp.h 
tlsServer.o: stun.h udp.h 

watchdog: watchdog.c
	$(CC) $(LDFLAGS) $(CFLAGS) watchdog.c -o watchdog

StunClient.o:
	$(CXX) -c -DNO_PTYPES $(CPPFLAGS) $(CXXFLAGS) StunClient.cpp -o StunClient.o
