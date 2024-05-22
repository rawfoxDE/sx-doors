
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <exec/memory.h>
#include "dcirc.h"

extern void ConWrite(char *data, long len);
extern void SendCmd(char *string);
extern void LocalPrint(char *data);
extern UWORD Connect_To_Server(char *servername, UWORD port);

extern char buf[1024], server[64], tbuf[256], channel[52], nick[32];

/* ========================================= */
/* == Processing Outbound command or text == */
/* ========================================= */

void ProcessOutLine(char *text)
{
	UWORD len = strlen(text);
	char *outbuf, *po, *po2;

	outbuf = AllocMem(len+128, MEMF_PUBLIC);
	if(!outbuf) return;

	switch(text[0])
	{
	case '/':
		if(strnicmp((char *)&text[1], "BAN", 3)==0)
		{
			po = strstr(text, " ");
			if(po)
			{
				sprintf(outbuf, "MODE %s +b%s\r\n", channel, po);
				SendCmd(outbuf);
			} else {
				LocalPrint("/Ban <usermask>\n");
			}
			goto fin;
		}
		if(strnicmp((char *)&text[1], "BANLIST", 7)==0)
		{
			sprintf(outbuf, "MODE %s +b\r\n", channel);
			SendCmd(outbuf);
			goto fin;
		}
		if(strnicmp((char *)&text[1], "RAW", 3)==0)
		{
			strcat(text, "\r\n");
			if(text[4]==' ' && text[5]!=0)
				SendCmd((char *)&text[5]);
			else
				LocalPrint("/Raw <command>\n");
			goto fin;
		}
		if(strnicmp((char *)&text[1], "JOIN", 4)==0)
		{
			if(text[5]==' ' && text[6]!=0)
			{
				sprintf(outbuf, "JOIN %s\r\n", (char *)&text[6]);
				SendCmd(outbuf);
			} else {
				LocalPrint("/Join <channel>\n");
			}
			goto fin;
		}
		if(strnicmp((char *)&text[1], "NICK ", 5)==0)
		{
			sprintf(outbuf, "NICK %s\r\n", (char *)&text[6]);
			SendCmd(outbuf);
			goto fin;
		}
		if(strnicmp((char *)&text[1], "WHOIS ", 6)==0)
		{
			sprintf(outbuf, "WHOIS %s\r\n", (char *)&text[7]);
			SendCmd(outbuf);
			goto fin;
		}
		if(strnicmp((char *)&text[1], "WHOWAS ", 7)==0)
		{
			sprintf(outbuf, "WHOWAS %s\r\n", (char *)&text[8]);
			SendCmd(outbuf);
			goto fin;
		}
		if(strnicmp((char *)&text[1], "ME", 2)==0)
		{
			if(text[3]==' ' && text[4]!=0)
			{
				sprintf(outbuf, "›32m*›m %s %s\n", nick, (char *)&text[4]);
				LocalPrint(outbuf);
				sprintf(outbuf, "PRIVMSG %s :\001ACTION %s\001\r\n", channel, (char *)&text[4]);
				SendCmd(outbuf);
			} else {
				LocalPrint("/Me <action>\n");
			}
			goto fin;
		}
		if(strnicmp((char *)&text[1], "M ", 2)==0 || strnicmp((char *)&text[1], "MSG ", 4)==0)
		{
			po = strstr(text, " ");
			if(po)
			{
				po = (char *)(long)po+1;
				po2 = strstr(po, " ");
				if(po2)
				{
					po2[0] = 0;
					sprintf(outbuf, "›32m%s›m ›33m<-›m %s\n", po, (char *)&po2[1]);
					LocalPrint(outbuf);
					sprintf(outbuf, "PRIVMSG %s :%s\r\n", po, (char *)&po2[1]);
					SendCmd(outbuf);
					goto fin;
				}
			}
		}
		if(strnicmp((char *)&text[1], "QUIT", 4)==0)
		{
			po = strstr(text, " ");
			if(po)
			{
				po = (char *)(long)po+1;
				sprintf(outbuf, "QUIT :%s", po);
			} else {
				strcpy(outbuf, "QUIT :DCIRC Exiting...\r\n");
			}
			SendCmd(outbuf);
			goto fin;
		}
		if(strnicmp((char *)&text[1], "HELP", 4)==0)
		{
			LocalPrint("Commands:  BAN   BANLIST JOIN    KICK  ME     MSG\n"
				   "           NICK  RAW     SERVER  QUIT  WHOIS  WHOWAS\n");
			goto fin;
		}
		if(strnicmp((char *)&text[1], "SERVER", 6)==0)
		{
			po = strstr(text, " ");
			if(po)
			{
				po = (char *)(long)po+1;
				po2 = strstr(po, " ");
				if(po2)
				{
					po2[0] = 0;
					po2 = (char *)(long)po2+1;
					Connect_To_Server(po, atoi(po2));
				} else {
					Connect_To_Server(po, 6667);
				}
			} else {
				LocalPrint("/Server <server> [<port>]\n");
			}
			goto fin;
		}
/*		if(strnicmp((char *)&text[1], "TOPIC", 5)==0)
		{
		}*/
		if(strnicmp((char *)&text[1], "KICK ", 5)==0)
		{
			po = strstr(text, " ");
			if(po)
			{
				po = (char *)(long)po+1;
				po2 = strstr(po, " ");
				if(po2)
				{
					po2[0] = 0;
/* kick with reason */			sprintf(outbuf, "KICK %s %s :%s\r\n", channel, po, (char *)&po2[1]);
					SendCmd(outbuf);
				} else {
					sprintf(outbuf, "KICK %s %s\r\n", channel, po);
/* kick */				SendCmd(outbuf);
				}
				goto fin;
			}
		}	
		LocalPrint("Bad Command!\n");
		break;
	default:
		if(channel[0]!=0)
		{
			sprintf(outbuf, "›32m(%s)›m %s\n", nick, text);
			LocalPrint(outbuf);
/* msg to channel */	sprintf(outbuf, "PRIVMSG %s :%s\r\n", channel, text);
			SendCmd(outbuf);
		} else {
			LocalPrint("Join a channel first!\n");
		}
	}
fin:
	FreeMem(outbuf, len+128);
}
