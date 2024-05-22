/*

----------------------------------------------------
 Programming doors for System-X using EXEC-MESSAGES
----------------------------------------------------

See the botton of this source!

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>
#include <time.h>

struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;


void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Door(void);
void myctime(long secs,char *outbuf);
void mycdate(long secs,char *outbuf);

BOOL sx;


int main(int argc, char *argv[])
{
	char portname[16];

	if(argv[1][0]==0)
	{
		PutStr("This program requires System-X BBS Software\n");
	} else {
		sprintf(portname, "AEDoorPort%s", argv[1]);
		bbsport = FindPort(portname);
		if(bbsport)
		{
			XIMFunction(1, 0, 0); 	/* function 1 = register */

			/* find out if we are under SYSTEM-X or AmiExpress */

			if(strcmp(themsg.String,"SX")==0) sx=TRUE; else sx=FALSE;

			Door();

			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}

void PS(char * str)
{
	if(sx) XIMFunction(1500, (long)str, 0); else XIMFunction(4, 0, str);
}

void XIMFunction(int func, long data, char * str)
{
	struct MsgPort *replyport;

	replyport = CreateMsgPort();
	if(replyport)
	{
		themsg.Msg.mn_Length	= sizeof(struct JHMessage);
		themsg.Msg.mn_ReplyPort	= replyport;
		themsg.Data 		= data;
		themsg.Command 		= func;
		if(str && str[0]!=0) strcpy(themsg.String, str);
		PutMsg(bbsport, (struct Message *)&themsg);
		WaitPort(replyport);
		DeleteMsgPort(replyport);
	}
}

/* ============ PUT YOUR DOOR IN HERE ============== */

void Door(void)
{
        char buf[256],login[32];
        char curdate[]=""__DATE__"";
        
        int i=0;
        BPTR fhcl;
         
       // struct UserData *User;
         
        struct CallersLog
   	{
		UWORD	user_slot,
	        	node;
                long    time_login,
                        time_logout,
                        seconds_online,
                        bytes_uploaded,
                        bytes_downloaded;
                UWORD   files_uploaded,
                        files_downloaded,
                        messages;
                long    baud,
                        flags;
                UBYTE   logout_mode;
                char    res[25]; 
        };   
        struct CallersLog CLog;

       fhcl = Open("sx:logfiles/callers.log", MODE_OLDFILE);
      //  fhu = Open("sx:user.data",MODE_OLDFILE);

   //     XIMFunction(1501,0,0);
   //     User = (struct UserData *) themsg.Data;
       
        if(fhcl)      // File Handle Callers Log
	{
                Seek(fhcl,0,OFFSET_END);
                Seek(fhcl,sizeof(struct CallersLog)* -10,OFFSET_CURRENT);
		
                PS(curdate);
                               
                // brauche login.datum login.zeit login.dauer login.baud
                while(i<=10)
                {
                        //Seek(fhcl,0,OFFSET_CURRENT);    
                        Read(fhcl, &CLog,sizeof(struct CallersLog)); 
                        
                        mycdate(CLog.time_login,login);        
                        
                        sprintf(buf,"\n\r TOTAL : %-4ld Mins. , Date : %-10s , Baud : %-5ld ",CLog.seconds_online/60,login,CLog.baud);
                        PS(buf);
                        i++;
                } 
        PS("\n\r\n\r");
        
        }
   
Close(fhcl);
        
}


void myctime(long secs, char *outbuf)
{
	struct DateStamp *tp;
	struct DateTime tostr;

	tp = __timecvt((time_t)secs);

	memcpy(&tostr, tp, 12);

	tostr.dat_Format  = FORMAT_USA;
	tostr.dat_StrDay  = "2222222222";
	tostr.dat_StrDate = "000000000";
	tostr.dat_StrTime = "111111111";
	tostr.dat_Flags   = 0;

	DateToStr(&tostr);
	sprintf(outbuf, "%s", tostr.dat_StrTime);       //, tostr.dat_StrDate);
}
void mycdate(long secs, char *outbuf)
{
	struct DateStamp *tp;
        struct DateTime tostr;
        
	tp = __timecvt((time_t)secs);

	memcpy(&tostr, tp, 12);

	tostr.dat_Format  = FORMAT_USA;
	tostr.dat_StrDay  = "2222222222";
	tostr.dat_StrDate = "000000000";
	tostr.dat_StrTime = "111111111";
	tostr.dat_Flags   = 0;

	DateToStr(&tostr);
	sprintf(outbuf, "%s", tostr.dat_StrDate);       //, tostr.dat_StrDate);
}

