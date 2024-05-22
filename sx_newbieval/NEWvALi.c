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

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;
struct AccessStruct Axx;
struct UserIndexStruct *Uid;

void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Door(void);

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
        char newuser[256],useraxx[256],curruser[256];
        int newlevel,userlevel;
        
        BPTR fhax,fh; 
    
        fhax = Open("SX:Prefs/Access.DAT",MODE_OLDFILE);
        fh = Open("SX:User.Index",MODE_OLDFILE);
        
                 
        Seek(fhax,0,OFFSET_BEGINNING);                
        Read(fhax,&Axx,sizeof(struct AccessStruct));
        sprintf(newuser,"%ld",Axx.access);
        PS(newuser);
               
        newlevel=atoi(newuser);
        
           Seek(fh,0,OFFSET_BEGINNING);              
                                                  
                while(fh)
                {
                        //Seek(fh,0,OFFSET_CURRENT);
                                                //XIMFunction(1503,0,0);
                      //  Uid = (struct UserIndexStruct * ) themsg.Data;                             
                        Read(fh,0,sizeof(struct UserIndexStruct *));
                        sprintf(curruser,"\n\rUsername: %s ",Uid->handle);
                        PS(curruser);
                                    
                        PS("\n\r");
                        XIMFunction(105,1,0);
                        sprintf(useraxx,"%s",themsg.String);
                        PS(useraxx);
                        userlevel=atoi(useraxx);
      
                        PS("\n\r");
                                
                        if(newlevel==userlevel)
                        {
                                PS("\n\ryou are NEW USER !");
                        } else
                        {
                                PS("\n\ryou are MEMBER ! ");
                        }        
                        XIMFunction(6,0,"Enter");
                        if(themsg.String[0]=='q') break;
                
                         
                }      
	        PS("\r\n\r\nExiting...\r\n\r\n");
        
Close(fh);   
Close(fhax);


}
