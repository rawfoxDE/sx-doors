
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

extern char server[64], tbuf[256], channel[52], nick[32];

/* ======================================= */
/* == Processing IRC Kludge from server == */
/* ======================================= */

void ProcessTextLine(char *text, UWORD len)
{
	char *textcopy, *outbuf, *po, *po2, *adrs, type[16], *wword[512];
	UWORD outlen, number, i = 0;

	outbuf = AllocMem(len+256, MEMF_CLEAR|MEMF_PUBLIC);
	if(!outbuf) return;

	textcopy = AllocMem(len, MEMF_PUBLIC);
	if(!textcopy) { FreeMem(outbuf, len+256); return; }

	memcpy(textcopy, text, len);

	wword[i] = strtok(textcopy, " ");
	while(1)
	{
		i++;
		wword[i] = strtok(NULL, " ");
		if(!wword[i]) break;
	}

	switch(text[0])
	{
	case ':':
		number = atoi(wword[1]);
		if(number > 0)
		{
			switch(number)
			{
			case 1:
			case 2:
			case 3:
			case 4: strcpy(type, "WELCOME"); break;
			case 251:
			case 252:
			case 253:
			case 254:
			case 255: strcpy(type, "LUSERS"); break;
			case 301: strcpy(type, "AWAY"); break;
			case 311:
				po = strstr(text, " * :");
				po = (char *)(long)po+4;
				po2 = strstr(po, "\r");
				if(po2) po2[0] = 0;
				sprintf(outbuf, "›32m<WHOIS>›m User: %s %s@%s (%s)\n", wword[3], wword[4], wword[5], po);
				outlen = strlen(outbuf);
				goto out;
			case 312:
			case 313:
			case 317:
			case 318:
			case 319: strcpy(type, "WHOIS"); break;
			case 331:
			case 332:
			case 333: strcpy(type, "TOPIC"); break;
			case 353:
			case 366: strcpy(type, "NAMES"); break;
			case 375:
			case 372:
			case 376: strcpy(type, "MOTD"); break;
			case 392:
			case 393:
			case 394:
			case 395: strcpy(type, "USERS"); break;
			case 451:
			case 401:
			case 402: strcpy(type, "ERROR"); break;
			case 484: strcpy(type, "WARN"); break;
			default: sprintf(type, "%u", number);
			}
			po = (char *)(long)strstr(text, " ")+1;
			po = (char *)(long)strstr(po, " ")+1;
			if(strnicmp(po, nick, strlen(nick))==0) po = (char *)(long)strstr(po, " ")+1;
			if(po[0] == ':') po = (char *)(long)po+1;
			sprintf(outbuf, "›32m<%s>›m %s", type, po);
			outlen = strlen(outbuf);
			goto out;
		}
		po = (char *) (long)strstr(text, " ")+1;
		if(strcmp(wword[1], "PRIVMSG")==0)
		{
			po2 = strstr(text, "!");
			if(po2) po2[0] = 0;
			po2 = (char *)((long)strstr((char *)&po[9], " :")+2);
			if(wword[2][0]=='#')
			{
				if(strncmp(po2, "\001ACTION ", 8)==0)
				{
/* action */				sprintf(outbuf, "›32m*›m %s %s", (char *)((long)text+1), (char *)(long)po2+8);
					goto preout;
				}
/* msg to channel */		sprintf(outbuf, "›32m(%s)›m %s", (char *)((long)text+1), po2);
			} else {
				if(strncmp(po2, "\001PING ", 6)==0)
				{
					sprintf(outbuf, "NOTICE %s :%s", (char *)(long)text+1, po2);
/* user ping */				SendCmd(outbuf);
					sprintf(outbuf, "Replying to PING from: %s\n", (char *)(long)text+1);
					goto preout;
				}
				if(strncmp(po2, "\001VERSION\001", 9)==0)
				{
/* version */				sprintf(outbuf, "NOTICE %s :\001VERSION %s %s for AmigaOS. Written By Zed and Zer0.\001\r\n", (char *)(long)text+1, CLIENTNAME, VERSION);
					SendCmd(outbuf);
					sprintf(outbuf, "Replying to VERSION request from: %s\n", (char *)(long)text+1);
					goto preout;
				}
				if(po2[0]=='\001')
				{
					po = strstr((char *)&po2[1], "\001");
					if(po) po[0] = 0;
/* unknown CTCP */			sprintf(outbuf, "Unknown CTCP '%s' from: %s\n", (char *)&po2[1], (char *)(long)text+1);
					goto preout;
				}
/* private msg */		sprintf(outbuf, "›32m*%s*›m %s", (char *)((long)text+1), po2);
			}
preout:
			outlen = strlen(outbuf);
			goto out;
		}

		po2 = (char *)((long)po-1);

		if(strncmp(po, "JOIN", 4)==0)
		{
			po2[0] = 0;
			po2 = strstr(text, "!");
			if(po2) po2[0] = 0;
			adrs = (char *)((long)po2+1);
			po2 = (char *)((long)po+6);
			if(strcmp((char *)(long)text+1, nick)==0)
			{
				po = strstr(po2, "\r");
				if(po)
				{
					po[0] = 0;
					strcpy(channel, po2);
					sprintf(outbuf, "›32mYou have joined channel ›33m%s›m\r\n", channel);
				}
			} else {
				sprintf(outbuf, "›32m%s›m (%s) has joined channel %s", (char *)((long)text+1), adrs, po2);
			}
			goto preout;
		}
		if(strncmp(po, "QUIT", 4)==0)
		{
			po2[0] = 0;
			po2 = strstr(text, "!");
			if(po2) po2[0] = 0;
			adrs = (char *)((long)po2+1);
			po2 = (char *)((long)po+6);
			sprintf(outbuf, "›32m%s›m (%s) has signed off: %s", (char *)((long)text+1), adrs, po2);
			goto preout;
		}
		if(strncmp(po, "PART", 4)==0)
		{
			po2[0] = 0;
			po2 = strstr(text, "!");
			if(po2) po2[0] = 0;
			adrs = (char *)((long)po2+1);
			po2 = (char *)((long)po+5);
			sprintf(outbuf, "›32m%s›m (%s) has left channel %s", (char *)((long)text+1), adrs, po2);
			goto preout;
		}
		if(strncmp(po, "NOTICE", 6)==0)
		{
			po2[0] = 0;
			po2 = strstr(text, "!");
			if(po2) po2[0] = 0;
			po2 = (char *)((long)po+8);
			po = (char *)(long)strstr(po2, " ")+1;
			sprintf(outbuf, "›32m-%s-›m %s", (char *)((long)text+1), po);
			goto preout;
		}
		if(strncmp(po, "NICK ", 5)==0)
		{
			po2 = strstr(text, "!");
			if(po2) po2[0] = 0;
			if(stricmp((char *)((long)text+1), nick)==0)
			{
				po2 = strstr((char *)&po[6], "\r");
				if(po2)
				{
					po2[0] = 0;
					strcpy(nick, (char *)&po[6]);
					sprintf(outbuf, "›32mYou are now known as %s›m\r\n", nick);
				}
			} else {
				sprintf(outbuf, "›32m%s›m is now known as %s", (char *)((long)text+1), (char *)&po[6]);
			}
			goto preout;
		}
		if(strncmp(po, "TOPIC ", 6)==0)
		{
			po2 = strstr(text, "!");
			if(po2)
			{
				po2[0] = 0;
				po = (char *)(long)strstr(po, " :")+2;
				sprintf(outbuf, "%s has changed the topic to %s", (char *)&text[1], po);
				goto preout;
			}
		}
		if(strncmp(po, "KICK ", 5)==0)
		{
			po2 = strstr(text, "!");
			if(po2) po2[0] = 0;
			po2 = strstr((char *)&po[6], " ");
			if(po2)
			{
				po2[0] = 0;
				po = (char *)(long)wword[4]+1;
				po2 = strstr(po, "\r");
				if(po2)
				{
					po2[0] = 0;
					if(stricmp(wword[3], nick)==0)
					{
						channel[0] = 0;
						sprintf(outbuf, "›33mYou have been kicked out of %s by %s (%s)›m\r\n", wword[2], (char *)&text[1], po);
					} else {
						sprintf(outbuf, "›33m%s›m has been kicked out of %s by %s (%s)\r\n", wword[3], wword[2], (char *)&text[1], po);
					}
					outlen = strlen(outbuf);
				}
 				goto out;
			}
		}
		strcpy(outbuf, text);
		outlen = len;
		break;
	default:
		if(strncmp(text, "PING", 4)==0)
		{
			po = (char *)(long)text+6;
			sprintf(outbuf, "PONG %s", po);
/* server ping */	SendCmd(outbuf);
			/*sprintf(outbuf, "Replying to PING from: %s", po);
			outlen = strlen(outbuf);*/
			outlen = 0;
		} else {
			strcpy(outbuf, text);
			outlen = len;
		}
	}
out:
	if(outlen) ConWrite(outbuf, outlen);

	FreeMem(outbuf, len+256);
	FreeMem(textcopy, len);
}
/*
:irc.stealth.net 484 zed_ :Your connection is restricted!
:Micro^dC!medielnj@tonsberg708.online.no PRIVMSG #amielite :ALL :D
:gERDY^rYL!gerdy@a12c.mtalo.ton.tut.fi JOIN :#amielite
:vostok!-vos@194.188.79.114 QUIT :Leaving
:BeatniK!+tzu@rz42.rz.hs-bremen.de PART #Amielite :BeatniK
:[MoNSTeR]!Alex@galileo3-a6.racsa.co.cr NOTICE #amielite :(*) ELITE STATI
:wHiZaRd!geonico@ppp2nicosia.logos.cy.net NICK :whiz
:Zed!~zed@pc15-slip.da.deakin.edu.au KICK #amiga_warez Zed :test
:GK^OTL!genghiskha@newtown24.dynanet.com TOPIC #amiga_warez :¤°`°¤ø,¸¸,ø¤° HF
PING :irc.stealth.net
*/
