
#include "dcirc.h"

static const char __version[] = "\0$VER: "CLIENTNAME" "VERSION" ("__DATE__")";

#define SERVER 	"jp.elitenet.org"
#define PORT		6667
#define	NICK		"[r]FoX"
#define USERNAME	"rfx"
#define REALNAME	"DCIRC 0.02"
#define HOSTNAME	"house.dyn.ml.org"

#define AMITCP_NEW_NAMES
#define _OPTINLINE

#define GAD_TEXTINPUT	1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <graphics/displayinfo.h>
#include <exec/memory.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/graphics_protos.h>
#include <libraries/gadtools.h>
#include <clib/gadtools_protos.h>
#include <sys/errno.h>
#include <netdb.h>
#include <bsdsocket.h>

void SendCmd(char *string);
void ConWrite(char *data, long len);
void LocalPrint(char *data);
void GetWindowMsg(void);
UWORD Connect_To_Server(char *servername, UWORD port);
void Shut_Server(void);

extern void ProcessOutLine(char *text);
extern void ProcessTextLine(char *text, UWORD len);

struct Library *IntuitionBase, *GfxBase, *GadToolsBase, *SocketBase;
struct Window *win, *ewin;
struct Screen *scr;
struct Gadget *glist, *gad, *egad;
struct NewGadget ng;
struct TextAttr topaz = {"topaz.font",8,0,0};
struct IOStdReq ioreq;
struct hostent *HostAddr;
struct sockaddr_in INetSocketAddr;

/*struct DCPrefsStruct
{
	char	nick1[10],
		nick2[10],
		nick3[10],
		nick4[10],
		username[16],
		realname[32];
	long	flags;
};
struct DCPrefsStruct DCPrefs;

struct DCIRCWindowsStruct
{
	struct Window *win;
	char channel[64];
	char title[96];
};
struct DCIRCWindowsStruct *DCIRCWin[64];*/

char buf[1024], server[64], tbuf[256], channel[52], nick[32], title[128];
long sok, WinTop, WLeft=0, WTop=0, WLeftNew, WTopNew, deltaX, deltaY;
void *vi;
BOOL done = FALSE, connected = FALSE;

void main(int argc, char *argv[])
{
	struct timeval timer;
	fd_set rd;
	long i;
	char *inbuf;
	UWORD co=0, j;
	ULONG sigmask;
	BOOL LOG = FALSE;
	BPTR fh;

	if(argc>0 && argv[1][0]!=0)
		strcpy(server, argv[1]);
	else
		strcpy(server, SERVER);

	strcpy(nick, NICK);
	channel[0] = 0;

	if(stricmp(argv[2], "LOG")==0) LOG = TRUE;

	SocketBase = OpenLibrary("bsdsocket.library", 3);

	if(!SocketBase)
	{
		PutStr("Requires AmiTCP or compatable TCP stack.\n");
		exit(10);
	}

	IntuitionBase = OpenLibrary("intuition.library",37);
	if (IntuitionBase)
	{
		GfxBase = OpenLibrary("graphics.library", 37);
		GadToolsBase = OpenLibrary("gadtools.library", 37);

		scr = LockPubScreen(0L);
		vi = GetVisualInfo(scr, TAG_END);
		gad = CreateContext(&glist);

		WinTop = (scr->WBorTop)+(scr->Font->ta_YSize)+1;
          
        	ng.ng_TextAttr   = &topaz;
        	ng.ng_VisualInfo = vi;
        	ng.ng_LeftEdge   = 10;
        	ng.ng_TopEdge    = WinTop+280;
        	ng.ng_Width      = 860;
        	ng.ng_Height     = 12;
        	ng.ng_GadgetText = "";
        	ng.ng_GadgetID   = GAD_TEXTINPUT;
		ng.ng_Flags      = 0;
          
		egad = CreateGadget(STRING_KIND,  gad, &ng,
 	            GT_Underscore,	'_', 
                 GTST_MaxChars,	119,
			  GTST_String,	"",
           TAG_END);

		ewin = OpenWindowTags(NULL,
			WA_Title,		"DCIRC Amiga IRC-Client",
			WA_Left,		0,
			WA_Top,		14,
			WA_Width,		880,
			WA_InnerHeight,300,
			WA_Borderless,		FALSE,
			WA_CloseGadget,	TRUE,
			WA_DragBar,		   TRUE,
			WA_DepthGadget,	TRUE,
			WA_SizeGadget,		FALSE,
			WA_Gadgets,		glist,
			WA_SmartRefresh,	TRUE,
			WA_IDCMP,		IDCMP_CLOSEWINDOW | IDCMP_REFRESHWINDOW | BUTTONIDCMP | IDCMP_CHANGEWINDOW,
                TAG_END);

		
          
          WLeft = ewin->BorderLeft;
          WTop  = ewin->BorderTop; 
          
		win = OpenWindowTags(NULL,
			WA_Title,		NULL,
			WA_Left,		WLeft+20,
			WA_Top,		WTop+20,
			WA_Width,		840,
			WA_InnerHeight,250,
			WA_MinWidth,		500,
			WA_MinHeight,		100,
			WA_MaxWidth,		860,
			WA_MaxHeight,		512,
			WA_Borderless,		TRUE,
			WA_CloseGadget,	FALSE,
			WA_DragBar,		FALSE,
			WA_DepthGadget,	FALSE,
			WA_SizeGadget,		FALSE,
               WA_SmartRefresh,  TRUE,
               WA_IDCMP,   WFLG_GIMMEZEROZERO |  IDCMP_REFRESHWINDOW  ,
                TAG_END);
 
      UnlockPubScreen(0L, scr);       
         
		if(win)
		{
         sprintf(title, "DCIRC %s - Not Connected", VERSION);
      
         GT_RefreshWindow(win, NULL);
         GT_RefreshWindow(ewin, NULL);
			ioreq.io_Data = win;
			ioreq.io_Length = sizeof(struct Window);
			i = OpenDevice("console.device", 3, (struct IORequest *)&ioreq, 0);
			if(i == 0)	
         {
				Connect_To_Server(server, PORT);

				timer.tv_sec = 30;
				timer.tv_usec = 0;
	
/* ------ main loop ------ */

	inbuf = AllocMem(2048, MEMF_PUBLIC|MEMF_CLEAR);
	if(!inbuf) done = TRUE;

	if(LOG) fh = Open("ram:DCIRC.log", MODE_NEWFILE);

	while(!done)
	{
		if(connected)
		{
			FD_ZERO(&rd);
			FD_SET(sok, &rd);
			sigmask = 1L<<ewin->UserPort->mp_SigBit;
			switch (WaitSelect(sok + 1, &rd, 0, 0, &timer, &sigmask))
			{
			case 0:
				GetWindowMsg();
				break;
			default:
				GetWindowMsg();
				if(connected)
				{
					i = Recv(sok, buf, sizeof buf, 0);
					if(i < 1) done = TRUE;
					j=0;
					while(j < i)
					{
						if(co == 2048)
						{
							inbuf[co] = 0;
							if(LOG) Write(fh, inbuf, co);
							ProcessTextLine(inbuf, co);
							co = 0;
							inbuf[co] = 0;
						}
						switch(buf[j])
						{
						case 10:
							inbuf[co] = 10;
							co++;
							inbuf[co] = 0;
							if(LOG) Write(fh, inbuf, co);
							ProcessTextLine(inbuf, co);
							co = 0;
							inbuf[co] = 0;
							break;
						case 22: /* underlined */
						case 2:  /* bold */
							break;
						default:
							inbuf[co] = buf[j];
							co++;
						}
						j++;
					}
				}
				break;
			}
		} else {
			sigmask = 1L<<ewin->UserPort->mp_SigBit;
			Wait(sigmask);
			GetWindowMsg();
		}
	}

	if(LOG && fh) Close(fh);

	if(inbuf) FreeMem(inbuf, 2048);

/* -- end of main loop -- */

				Shut_Server();
				CloseDevice((struct IORequest *)&ioreq);
			} /* if opendev */
			CloseWindow(ewin);
			CloseWindow(win);
		} /* if win */
        	FreeGadgets(glist);
        	FreeVisualInfo(vi);
		CloseLibrary(GadToolsBase);
		CloseLibrary(IntuitionBase);
		CloseLibrary(GfxBase);
	} /* if intui */
	CloseLibrary(SocketBase);
}

/*void LoadPrefs(void)
{
	BPTR fh = Open("PROGDIR:DCIRC.Prefs", MODE_OLDFILE);
	if(fh)
	{
		Read(fh, &DCPrefs, sizeof(struct DCPrefsStruct));
		Close(fh);
	}
}*/

void SendCmd(char *string)
{
	if(connected)
		Send(sok, string, strlen(string), 0);
	else
		LocalPrint("Not connected. Type /Server to connect.\n");
}

void ConWrite(char *data, long len)
{
	ioreq.io_Data = data;
	ioreq.io_Length = len;
	ioreq.io_Command = CMD_WRITE;
	DoIO((struct IORequest *)&ioreq);
}

void LocalPrint(char *data)
{
	ConWrite(data, strlen(data));
}

void GetWindowMsg(void)
{
	struct IntuiMessage *message;
	struct Gadget *gad;
	UWORD code;

	while (message = GT_GetIMsg(ewin->UserPort))
	{
	   gad   = (struct Gadget *)message->IAddress;
		code  = message->Code;
		GT_ReplyIMsg(message);
		switch (message->Class)
		{
		case IDCMP_GADGETUP:
			switch(gad->GadgetID)
			{
			case GAD_TEXTINPUT:
				strcpy(buf, ((struct StringInfo *)gad->SpecialInfo)->Buffer);
				GT_SetGadgetAttrs(egad, ewin, 0, GTST_String, "");
				ActivateGadget(egad, ewin, 0);
				ProcessOutLine(buf);
				break;
			}
			break;	               
          case IDCMP_CHANGEWINDOW:                
               WLeftNew = ewin->BorderLeft;
               WTopNew  = ewin->BorderTop; 
               deltaX   = (WLeftNew-WLeft);
               deltaY   = (WTopNew-WTop);
					MoveWindow(win, deltaX, deltaY);  
               GT_RefreshWindow(win, NULL);
               GT_RefreshWindow(ewin, NULL);
               break;
          case IDCMP_CLOSEWINDOW:
			done=TRUE;     
		}
	}
}

void CheckError(void)
{
	long en = Errno();

	switch(en)
	{
	case EACCES:
	case EPERM:
		LocalPrint("ERROR: Operation not permitted.\n"); break;
	case EINTR:
		LocalPrint("Failed. Interrupted system call.\n"); break;
	case EIO:
		LocalPrint("ERROR: I/O Error.\n"); break;
	case ECONNREFUSED:
		LocalPrint("ERROR: Connection refused.\n"); break;
	case EFAULT:
		LocalPrint("ERROR: Bad address.\n"); break;
	case EPIPE:
		LocalPrint("ERROR: Broken pipe.\n"); break;
	case ETIMEDOUT:
		LocalPrint("Connection timeout.\n"); break;
	default:
		sprintf(tbuf, "Connection failed. Error %ld.\n", en);
		LocalPrint(tbuf);
	}
}

UWORD Connect_To_Server(char *servername, UWORD port)
{
	char hostname[128];

	if(connected)
	{
		sprintf(tbuf, "Closing connection to %s.\n", server);
		LocalPrint(tbuf);
		Shut_Server();
		sprintf(title, "DCIRC %s - Not Connected", VERSION);
		SetWindowTitles(ewin, (UBYTE *)title, (UBYTE *)-1);
	}

	strcpy(server, servername);

	connected = FALSE;

	sprintf(tbuf, "Looking up ›32m%s›m...\n", server);
	LocalPrint(tbuf);

	HostAddr = GetHostByName(server);
	if(!HostAddr)
	{
		LocalPrint("Unknown Server. Maybe You Misspelt It?\n");
		return(1);
	}

	INetSocketAddr.sin_len = sizeof(INetSocketAddr);
	INetSocketAddr.sin_family = AF_INET;
	INetSocketAddr.sin_port = port;
	INetSocketAddr.sin_addr.s_addr = 0;

	memcpy(&INetSocketAddr.sin_addr, HostAddr->h_addr, HostAddr->h_length);

	sprintf(tbuf, "Connecting to ›32m%s›m port ›32m%ld›m...\n", HostAddr->h_name, port);
	LocalPrint(tbuf);

	sok = Socket(HostAddr->h_addrtype, SOCK_STREAM, 0);
	if(sok == -1)
	{
		LocalPrint("Cannot Open Socket.\n");
		return(2);
	}
 
	if(Connect(sok, (struct sockaddr const *)&INetSocketAddr, sizeof(INetSocketAddr)) == -1)
	{
		Shutdown(sok, 2);
		CloseSocket(sok);
		CheckError();
		/*LocalPrint("Cannot connect to that server.\n");*/
		return(3);
	}

	sprintf(title, "DCIRC %s - Server: %s    Port: %u", VERSION, server, port);
	SetWindowTitles(ewin, (UBYTE *)title, (UBYTE *)-1);

	LocalPrint("Connected. Logging in...\n");

	connected = TRUE;

	sprintf(tbuf, "NICK %s\r\n", nick);
	SendCmd(tbuf);

	sprintf(tbuf, "USER %s %s - :%s\r\n", USERNAME, HOSTNAME, REALNAME);
	SendCmd(tbuf);

	GetHostName(hostname, 128);
	sprintf(tbuf, "Username: ›32m%s@%s›m   Nickname: ›32m%s›m\n", USERNAME, hostname, nick);
	LocalPrint(tbuf);

	return(0);
}

void Shut_Server(void)
{
	if(connected)
	{
		SendCmd("QUIT :DCIRC Exiting...\r\n");
		Shutdown(sok, 2);
		CloseSocket(sok);
	}
}

/* ftp.funet.fi:/pub/unix/networking/ident/ */

