/*

SXVIEW (VIEWER)



//static const char __version[] = "\0$VER: SXVIEW 1.0 ("__DATE__")";
*/

#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>

struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
  int NodeID;
  int LineNum;
  unsigned long signal;
  struct Process *task;
  APTR *Semi;
} themsg;

void PS(char * str);
void XIMFunction(int func, long data, char * str);
void view(char *filename);
void shutdown(void);

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

			view((char *)themsg.Data);

			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}

void shutdown(void)
{
	XIMFunction(2, 0, 0);
	exit(0);
}

void PS(char * str)
{
	XIMFunction(1500, (long)str, 0);
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

void view(char *filename)
{
	char buf[512], *arg;
	BPTR fh,lck;
	
	PS("[0m\r\nSystem-X VIEW [36m1.0 [0mBy [r]FoX\r\n\r\n");

	XIMFunction(1507, 0, 0);
	arg = (char *)themsg.Data;
	if(arg[0]!=0)
	{
		strcpy(filename, arg);
	} else 
        {
		XIMFunction(5, 34, "[36mEnter filename to View[34m: [0m");
		PS("\r\n");
		if(themsg.Data==-1) shutdown();
		if(themsg.String[0]==0) return;
		strcpy(filename, themsg.String);
	}

	lck = Lock(filename, SHARED_LOCK);
	if(!lck) return;	/* == IF IT DOESN'T, EXIT QUIETLY == */
	UnLock(lck);

	fh = Open("SX:Prefs/Confs.DAT", MODE_OLDFILE);

        PS("\n\r go to the loop now...");
        Delay(20);

	if(fh)
	{
                struct ConfStruct Conf;
		                
		while(1)
		{
                        PS("\n\rread the conference.dat now...");
                        Delay(20);
                                                
                        if(Read(fh, &Conf, sizeof(struct ConfStruct)))
			{

				/* == LET'S CHECK THIS CONFERENCE FOR THE FILE TO SHOW == */
           		                                                           
				if(Conf.filepath[0] == 0) lck = 0;
				else 
                                {
				        sprintf(buf, "%s%s", Conf.filepath, filename);
                                        lck = Lock(buf, SHARED_LOCK);
                                        PS(buf);
                                }
                        }



			if(lck)
			{
                                PS("\n\rFile is Found !!");
                                Delay(25);
				UnLock(lck);
				/* == FILE IS FOUND !! == */		
           			PS("....typing...");	
  			        //PS(filename);
	
				Close(fh);
				return;
			}			
		}	
	}
	Close(fh);
}


