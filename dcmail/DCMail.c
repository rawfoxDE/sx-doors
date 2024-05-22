
static const char __version[] = "\0$VER: DCMail 0.01 ("__DATE__")";

#define _OPTINLINE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>
#include <intuition/intuition.h>
#include <libraries/gadtools.h>
#include <clib/alib_protos.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <netdb.h>
#include <bsdsocket.h>
#include <libraries/reqtools.h>
#include <proto/reqtools.h>
#include "DCMail_gui.h"


extern void ShowMessage(char *dir, long num);


BOOL done = FALSE;

long popsock = -1;

ULONG lastsecs = 0, lastmics = 0;

UWORD lastcode;

char cbuf[512], buf[256];


struct ReqToolsBase *ReqToolsBase;
struct Library *SocketBase;
struct List mainlist;
struct PrefsStruct
{
	char	pop3[72],
		login[52],
		pass[24];
} Prefs;
struct ListStruct
{
	struct Node entry_node;
	char name[86];
};
struct IndexStruct
{
	char	from[42],
		to[42],
		subj[52],
		date[6],
		time[6];
	long	size;
	char	res[40];
} Index;


void CheckDir(char *dir)
{
	BPTR lck = Lock(dir, ACCESS_READ);
	if(!lck) lck = CreateDir(dir);
	if(lck) UnLock(lck);
}

void AddToList(char *text)
{
	struct ListStruct *new = AllocMem(sizeof(struct ListStruct), MEMF_CLEAR|MEMF_PUBLIC);
	if(new)
	{
		strcpy(new->name, text);
		new->entry_node.ln_Name = new->name;
		AddTail(&mainlist, new);
	}
}

void FlushList(void)
{
	struct ListStruct *worknode, *nextnode;

	if (mainlist.lh_TailPred != (struct Node *)&mainlist)
	{
		worknode = (struct ListStruct *)(mainlist.lh_Head); /* First node */
		if(worknode)
		{
			while (nextnode = (struct ListStruct *)(worknode->entry_node.ln_Succ))
			{
				FreeMem(worknode, sizeof(struct ListStruct));
				worknode = nextnode;
			}
		}
	}
	NewList(&mainlist);
      	GT_SetGadgetAttrs(Project0Gadgets[GD_MSGLIST],Project0Wnd,0,GTLV_Labels,&mainlist,TAG_END);
}

long ScanDir(char *dir, BOOL from)
{
	char tbuf[256], date[10];
	BPTR ifh;
	BOOL indexfin = FALSE;
	long j = 0;
	char *po;

	FlushList();

	sprintf(tbuf, "%s/.index", dir);
	ifh = Open(tbuf, MODE_OLDFILE);
	if(ifh)
	{
		while(!indexfin)
		{
			j++;
			if(Read(ifh, &Index, sizeof(struct IndexStruct)))
			{
				date[0] = Index.date[4];
				date[1] = Index.date[5];
				date[2] = '/';
				date[3] = Index.date[2];
				date[4] = Index.date[3];
				date[5] = '/';
				date[6] = Index.date[0];
				date[7] = Index.date[1];
				date[8] = 0;
				if(from)
				{
					po = strstr(Index.from, " <");
					if(po) po[0] = 0;
					sprintf(tbuf, "%3ld %-20.20s %-25.25s %8ld %s", j, Index.from, Index.subj, Index.size, date);
				} else {
					po = strstr(Index.to, " <");
					if(po) po[0] = 0;
					sprintf(tbuf, "%3ld %-20.20s %-25.25s %8ld %s", j, Index.to, Index.subj, Index.size, date);
				}
				AddToList(tbuf);
			} else {
				indexfin = TRUE;
			}
		}
		Close(ifh);
     	 	GT_SetGadgetAttrs(Project0Gadgets[GD_MSGLIST],Project0Wnd,0,GTLV_Labels,&mainlist,TAG_END);
	}
	return(j);
}

void SetGraph(struct Window *win, WORD percent)
{
	UWORD gx;
	long WinTop;
	struct Screen *scr = win->WScreen;

	if(percent > 100) percent = 100;

	WinTop = (scr->WBorTop)+(scr->Font->ta_YSize)+1;

	EraseRect(win->RPort, 7, WinTop+4, 193, WinTop+19);

	gx = (percent*186)/100;
	SetAPen(win->RPort, 3);
	RectFill(win->RPort, 7, WinTop+4, gx+7, WinTop+19);

	SetAPen(win->RPort, 1);
	Move(win->RPort, 84, WinTop+16);
	sprintf(buf, "%d%%", percent);
	Text(win->RPort, buf, strlen(buf));
}

struct Window * OpenGraph(char *title)
{
	struct Screen *scr;
	struct Window *win;
	long WinTop;

	scr = LockPubScreen(0);
	if(scr)
	{
		WinTop = (scr->WBorTop)+(scr->Font->ta_YSize)+1;
		win = OpenWindowTags(NULL,
			WA_Title,		title,
			WA_Left,		(scr->Width-200)/2,
			WA_Top,			(scr->Height-30)/2,
			WA_Width,		200,
			WA_InnerHeight,		26,
			WA_DragBar,		TRUE,
			WA_DepthGadget,		TRUE,
			WA_SizeGadget,		FALSE,
			WA_CloseGadget,		FALSE,
			WA_CustomScreen,	scr,
			WA_SmartRefresh,	TRUE,
			WA_Activate,		TRUE,
	       		TAG_END);
		UnlockPubScreen(0, scr);

		SetAPen(win->RPort, 1);
		Move(win->RPort, 6, WinTop+3);
		Draw(win->RPort, 194, WinTop+3);
		Move(win->RPort, 6, WinTop+3);
		Draw(win->RPort, 6, WinTop+20);

		SetAPen(win->RPort, 2);
		Move(win->RPort, 6, WinTop+20);
		Draw(win->RPort, 194, WinTop+20);
		Move(win->RPort, 194, WinTop+20);
		Draw(win->RPort, 194, WinTop+3);

		return(win);
	}
	return(0);
}

void SendCmd(long sok, char *string)
{
	send(sok, string, strlen(string), 0);
}

void Shut_Server(long sok)
{
	SendCmd(sok, "QUIT\r\n");
	shutdown(sok, 2);
	CloseSocket(sok);
}

long Connect_To_Server(char *servername, UWORD port)
{
	struct hostent *HostAddr;
	struct sockaddr_in INetSocketAddr;
	long sok;

	if(!SocketBase)
	{
		SocketBase = OpenLibrary("bsdsocket.library", 3);
		if(!SocketBase)
		{
			rtEZRequestTags("AmiTCP or compatable TCP stack\n"
					"   must be started first.",
					"OK", NULL, NULL,
					RTEZ_ReqTitle,	"Error",
					TAG_DONE);
			return(-1);
		}
	}

	sprintf(buf, "Looking up ›32m%s›m...\n", servername);
	PutStr(buf);

	HostAddr = gethostbyname(servername);
	if(!HostAddr)
	{
		sprintf(buf, "Cannot find %s!\n", servername);
		rtEZRequestTags(buf,
				"Doh!", NULL, NULL,
				RTEZ_ReqTitle,	"Error",
				TAG_DONE);
		return(-1);
	}

	bzero(&INetSocketAddr, sizeof(INetSocketAddr));

	INetSocketAddr.sin_len = sizeof(INetSocketAddr);
	INetSocketAddr.sin_family = AF_INET;
	INetSocketAddr.sin_port = port;
	INetSocketAddr.sin_addr.s_addr = 0;

	memcpy(&INetSocketAddr.sin_addr, HostAddr->h_addr, HostAddr->h_length);

	sprintf(buf, "Connecting to ›32m%s›m...\n", HostAddr->h_name);
	PutStr(buf);

	sok = socket(HostAddr->h_addrtype, SOCK_STREAM, 0);
	if(sok == -1)
	{
		rtEZRequestTags("Cannot open socket.\n",
				"Doh!", NULL, NULL,
				RTEZ_ReqTitle,	"Error",
				TAG_DONE);
		return(-1);
	}
 
	if(connect(sok, (struct sockaddr const *)&INetSocketAddr, sizeof(INetSocketAddr)) == -1)
	{
		shutdown(sok, 2);
		CloseSocket(sok);
		rtEZRequestTags("Connection refused.\n",
				"Shit!", NULL, NULL,
				RTEZ_ReqTitle,	"Error",
				TAG_DONE);
		return(-1);
	}

	SetWindowTitles(Project0Wnd, (UBYTE *)"DC Mail 0.01 - POP3: Connected", (UBYTE *)-1);

	PutStr("Connected.\n");

	return(sok);
}

BOOL GetLine(long sok)
{
	WORD i = 0, valid = 1;
	char c;

	while(valid)
	{
		if(recv(sok, &c, 1, 0) < 1)
		{
			PutStr("Connection Lost.\n");
			return(0);
		}
		if(i<511 && c != '\n') cbuf[i++]=c; else valid=0;
	}
	cbuf[i]='\n';
	cbuf[i+1]=0;
	return(1);
}

UWORD POP3Login(long sok)
{
	PutStr("Logging in...\n");

	if(!GetLine(sok)) return(1);	/* welcome message */

	sprintf(buf, "USER %s\r\n", Prefs.login);
	SendCmd(sok, buf);
	if(!GetLine(sok)) return(1);
	if(strnicmp(cbuf, "+OK", 3) != 0) return(2);

	sprintf(buf, "PASS %s\r\n", Prefs.pass);
	SendCmd(sok, buf);
	if(!GetLine(sok)) return(1);
	if(strnicmp(cbuf, "+OK", 3) != 0) return(3);

	PutStr("Login complete.\n");

	return(0);
}

void StartPOP3(void)
{
	popsock = Connect_To_Server(Prefs.pop3, 110);
	if(popsock != -1)
	{
		switch(POP3Login(popsock))
		{
		case 1:
			Shut_Server(popsock);
			popsock = -1;
			rtEZRequestTags("Connection lost.",
					"Shit!", NULL, NULL,
					RTEZ_ReqTitle,	"Error",
					TAG_DONE);
			break;
		case 2:
			Shut_Server(popsock);
			popsock = -1;
			rtEZRequestTags("Bad login name for that POP3 server.",
					"OK", NULL, NULL,
					RTEZ_ReqTitle,	"Error",
					TAG_DONE);
			break;
		case 3:
			Shut_Server(popsock);
			popsock = -1;
			rtEZRequestTags("Bad password for that username.",
					"OK", NULL, NULL,
					RTEZ_ReqTitle,	"Error",
					TAG_DONE);
			break;
		}
	}
}

void Stat(void)
{
	if(popsock == -1) StartPOP3();
	if(popsock != -1)
	{
		SendCmd(popsock, "STAT\r\n");
		GetLine(popsock);
		if(strnicmp(cbuf, "+OK", 3) == 0)
		{
			sprintf(buf, "There are %ld messages waiting for you.", atol((char *)&cbuf[4]));
			rtEZRequestTags(buf,
					"OK", NULL, NULL,
					TAG_DONE);
		} else {
			rtEZRequestTags("Could not obtain message stats.",
					"OK", NULL, NULL,
					RTEZ_ReqTitle,	"Error",
					TAG_DONE);
		}
	}
}

void GetMail(void)
{
	long msgs, cmsg = 0, highest;
	BOOL msgfin;
	BPTR fh, ifh;
	struct Window *win;
	char date[10], title[64], *po;

	win = OpenGraph("Getting Mail..");
	if(!win) return;

	if(popsock == -1) StartPOP3();
	if(popsock != -1)
	{
		CheckDir("PROGDIR:InBound");

		ifh = Open("PROGDIR:InBound/.index", MODE_READWRITE);
		if(!ifh)
		{
			CloseWindow(win);
			return;
		}
		Seek(ifh, 0, OFFSET_END);
		highest = Seek(ifh, 0, OFFSET_CURRENT) / sizeof(struct IndexStruct);

		SendCmd(popsock, "STAT\r\n");
		if(!GetLine(popsock)) { CloseWindow(win); Close(ifh); return; }
		if(strnicmp(cbuf, "+OK", 3) != 0) { CloseWindow(win); Close(ifh); return; }

		msgs = atol((char *)&cbuf[4]);

		if(msgs < 1)
		{
			CloseWindow(win);
			Close(ifh);
			rtEZRequestTags("There are no new messages for you.",
					"OK", NULL, NULL,
					RTEZ_ReqTitle,	"Error",
					TAG_DONE);
			return;
		}

		while(cmsg < msgs)
		{
			cmsg++;
			sprintf(buf, "RETR %ld\r\n", cmsg);
			SendCmd(popsock, buf);

			SetGraph(win, (cmsg*100)/msgs);
			sprintf(title, "Getting Msg: %ld / %ld", cmsg, msgs);
			SetWindowTitles(win, title, (UBYTE *)-1);

			if(!GetLine(popsock)) { CloseWindow(win); Close(ifh); return; }
			if(strnicmp(cbuf, "+OK", 3) == 0)
			{
				sprintf(buf, "PROGDIR:InBound/msg.%ld", (cmsg+highest));
				fh = Open(buf, MODE_NEWFILE);
				msgfin = FALSE;
				Index.size = 0;
				while(!msgfin)
				{
					if(!GetLine(popsock))
					{
						CloseWindow(win);
						if(fh) Close(fh);
						Close(ifh);
						return;
					}
					if(cbuf[0]=='.' && cbuf[1]=='\r' && cbuf[2]=='\n') msgfin = TRUE; else
					{
						if(strnicmp(cbuf, "From: ", 6) == 0)
						{
							strcpy(Index.from, (char *)&cbuf[6]);
							po = strstr(Index.from, "\r");
							if(po) po[0] = 0;
							po = strstr(Index.from, "\n");
							if(po) po[0] = 0;
						}
						if(strnicmp(cbuf, "To: ", 4) == 0)
						{
							strcpy(Index.to, (char *)&cbuf[4]);
							po = strstr(Index.to, "\r");
							if(po) po[0] = 0;
							po = strstr(Index.to, "\n");
							if(po) po[0] = 0;
						}
						if(strnicmp(cbuf, "Subject: ", 9) == 0)
						{
							strcpy(Index.subj, (char *)&cbuf[9]);
							po = strstr(Index.subj, "\r");
							if(po) po[0] = 0;
							po = strstr(Index.subj, "\n");
							if(po) po[0] = 0;
						}
						if(strnicmp(cbuf, "Date: ", 6) == 0)
						{
							Index.date[0] = cbuf[20];
							Index.date[1] = cbuf[21];
							Index.date[2] = 'm';
							Index.date[3] = 'm';
							Index.date[4] = cbuf[11];
							Index.date[5] = cbuf[12];
/*Date: Tue, 27 May 1997 03:06:45*/
						}
						Index.size += strlen(cbuf);
						if(fh) Write(fh, cbuf, strlen(cbuf));
					}
				}
				if(fh) Close(fh);
				strcpy(Index.time, "hhmmss");
				Write(ifh, &Index, sizeof(struct IndexStruct));

				date[0] = Index.date[4];
				date[1] = Index.date[5];
				date[2] = '/';
				date[3] = Index.date[2];
				date[4] = Index.date[3];
				date[5] = '/';
				date[6] = Index.date[0];
				date[7] = Index.date[1];
				date[8] = 0;
				po = strstr(Index.from, " <");
				if(po) po[0] = 0;
				sprintf(buf, "%3ld %-20.20s %-25.25s %8ld %s", (cmsg+highest), Index.from, Index.subj, Index.size, date);
				AddToList(buf);
		     	 	GT_SetGadgetAttrs(Project0Gadgets[GD_MSGLIST],Project0Wnd,0,GTLV_Labels,&mainlist,TAG_END);
			}
		}
		Close(ifh);
	}
	CloseWindow(win);
}

void GetWinMsg(void)
{
	struct IntuiMessage *message;
	struct Gadget *gad;
	UWORD code;
	ULONG secs, mics;

	while (message = GT_GetIMsg(Project0Wnd->UserPort))
	{
	        gad   = (struct Gadget *)message->IAddress;
		code  = message->Code;
		secs  = message->Seconds;
		mics  = message->Micros;
		GT_ReplyIMsg(message);
		switch (message->Class)
		{
		case IDCMP_GADGETUP:
		case IDCMP_GADGETDOWN:
			switch(gad->GadgetID)
			{
			case GD_STAT:
				rtSetWaitPointer(Project0Wnd);
				Stat();
				ClearPointer(Project0Wnd);
				break;
			case GD_GETMAIL:
				rtSetWaitPointer(Project0Wnd);
				GetMail();
				ClearPointer(Project0Wnd);
				break;
			case GD_MSGLIST:
				if(DoubleClick(lastsecs, lastmics, secs, mics) && lastcode == code)
				{
					ShowMessage("PROGDIR:InBound", code);
				}
				lastsecs = secs;
				lastmics = mics;
				lastcode = code;
				break;
			case GD_FOLDER:
				rtSetWaitPointer(Project0Wnd);
				switch(code)
				{
				case 0:
					ScanDir("PROGDIR:InBound", TRUE);
					break;
				case 1:
					ScanDir("PROGDIR:OutBound", FALSE);
					break;
				case 2:
					ScanDir("PROGDIR:Sent", FALSE);
					break;
				case 3:
					ScanDir("PROGDIR:Archived", TRUE);
					break;
				}
				ClearPointer(Project0Wnd);
				break;
			}
			break;
		case IDCMP_CLOSEWINDOW:
			done = TRUE;
		}
	}
}

void main(void)
{
	strcpy(Prefs.pop3, "mail.luebeck.netsurf.de"); // EDIT THIS
	strcpy(Prefs.login, "clasen"); // EDIT THIS
	strcpy(Prefs.pass, "0016c240667"); // EDIT THIS
	NewList(&mainlist);

	SocketBase = OpenLibrary("bsdsocket.library", 0);

	if(ReqToolsBase = (struct ReqToolsBase *)OpenLibrary (REQTOOLSNAME, REQTOOLSVERSION))
	{
		if(SetupScreen()==0)
		{
			if(OpenProject0Window()==0)
			{
				CheckDir("PROGDIR:InBound");
				ScanDir("PROGDIR:InBound", TRUE);
				while(!done)
				{
					WaitPort(Project0Wnd->UserPort);
					GetWinMsg();
				}
				CloseProject0Window();
			}
			CloseDownScreen();
			if(popsock != -1) Shut_Server(popsock);
		}
		CloseLibrary((struct Library *)ReqToolsBase);
	}

	if(SocketBase) CloseLibrary(SocketBase);
}
