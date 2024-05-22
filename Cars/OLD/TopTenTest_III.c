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

struct MsgPort *bbsport;
struct UserData *User;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;

struct PlayerStruct
{
        char name[32];
        int  races;
} Player; 
        
void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Door(void);
void read_data(char *player);
void write_data(char *player);

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
          
        char buf[256],oldname[32];
        
        BPTR fhpl;
 
        PS("\033[2J");
        XIMFunction(8,0,"sx:developer/cars/car_topten.ans");
        PS("[0;0H");
        PS("[6B");
        //oldname=="EMPTY";
     
        fhpl = Open("sx:developer/cars/car_log", MODE_OLDFILE);
        if(fhpl)
        {                        
                        
                Seek(fhpl,0,OFFSET_BEGINNING);
                while(Read(fhpl, &Player, sizeof(struct PlayerStruct)))
                {
                        if(strcmpi(oldname,Player.name)==0) { Close(fhpl);  break; }
                        strcpy(oldname,Player.name); 
                        sprintf(buf,"\r[2B[5C%-12s[18C%6ld",Player.name,Player.races);   
                        PS(buf);
                        
                }           
                Close(fhpl);              
                PS("[25;0H");        
         }                 
        
}
