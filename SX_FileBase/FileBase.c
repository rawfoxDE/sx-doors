
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <libraries/dos.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <fcntl.h>
#include "sxstructs.h"
#include "filebase.h"

struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
};
struct JHMessage themsg;

struct FileBaseStruct
{
        char pathname[64],
             reserved[64];
         int axx;    
} FBS;                         
             
void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Door(void);
void cursor(void);
void LoadConfig(void);
BOOL sx;

char *dirpaths[256];
int p=1;

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
        char name[128], buf[256];
        int i;            
        PS("Hello! Welcome to a test door\r\n\r\n");
        
        //dirpaths[p] = AllocMem(64,MEMF_PUBLIC|MEMF_CLEAR);
        //LoadConfig();
                
//prompt:

        //PS(dirpaths[p]);
/*
        for(i=0;p==0;p--)
	     {
			  sprintf("\n\r%s", dirpaths[p]);
      		  PS(buf);            
		}
*/

        XIMFunction( 5, 64, "\n\rEnter Path: " );
        strcpy(name, themsg.String);
        tree(name);              

                    
        return;      
        
}

void cursor(void)
{
        // First call RAWARROWXIM (501).
        // The value of themsg.String[0] after a JH_HK is
        // 4 - UP
        // 5 - DOWN
        // 2 - LEFT
        // 3 - RIGHT

     for( ;; )
     {
          XIMFunction(501, 0, 0);
          XIMFunction(6, 0, 0);
          switch(themsg.String[0])
          {
               case 4:
                    PS("[AYou hit UP!!");
                    break;
               case 5:
                    PS("[BYou hit DOWN!");
                    break;
               case 3:
                    PS("[CYou hit RIGHT!");
                    break;
               case 2:
                    PS("[26DYou hit LEFT!");         
                    break;
                    
               case 'q':
                    return;     
          }
     }
}     
void LoadConfig(void)
{
        
        BPTR fh;
        long len;
        char buf[256];
        
 newf:      
        fh = Open("sx:doors/filebase/fbn.cfg",MODE_OLDFILE);
        if(!fh)     
        {
                fh = Open("sx:doors/filebase/fbn.cfg", MODE_NEWFILE);
                
                strcpy(FBS.pathname, "ram:");
                FBS.pathname[64]=0;
                
                
                FBS.axx=255;                 

                Write(fh, &FBS , sizeof(struct FileBaseStruct));
                Close(fh);
                goto newf;
                
        }                               
                                                 
        // if .config exists  
        else
        {
                
               Read(fh, &FBS, sizeof(struct FileBaseStruct));
                                 
               //p++;
                               
               strcpy(dirpaths[p], FBS.pathname);
                                                 
        }
        Close(fh);
}
