/*

hi peter, hgere i try to do i/o with sequential files...

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <fcntl.h>          // i read i must include fcntl.h ...must i (i.e. for Open ) ?
#include <ctype.h>
#include <sxstructs.h>
#include <libraries/dos.h>
#include <exec/memory.h>

struct MsgPort *bbsport;

struct JHMessage
{
  struct Message Msg;
  char String[200];
  int Data;
  int Command;
} themsg;


void PS(char * str);
void XIMFunction(int func, long data, char * str);
void AReq(void);
void pause(void);
void cursor(int zeich,BPTR fh);

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

			AReq();

			XIMFunction(2, 0, 0); 	/* function 2 = shutdown */
		}
	}
}

void PS(char * str)
{
	XIMFunction(1500, (long)str, 0);  /* XIM Function to send some text to SX */
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

void AReq(void)
{
          
	BPTR fh;
	char listpath[128], *fullbuf[80];
     
     long size=0,i=0;
          
     PS("\n\r[0;33;44m AmiNet Requester - Select List to View [0m\n\r");
     PS("[33m----------------------------------------\n\r");
     PS("[32m      1 - Aminet Charts \n\r");
     PS("      2 - Aminet Recent \n\r");
     XIMFunction(6,0," ");
     if(themsg.String[0]=='1') sprintf(listpath,"aminet:CHARTS");
     if(themsg.String[0]=='2') sprintf(listpath,"aminet:RECENT");                  
                         
	fh = Open(listpath, MODE_OLDFILE);
     PS("[2J[31m");
     if(!fh)
	{
        PS("Cannot Open the Filelist !\n\r");
        return;
     } else
     {   
          PS("\n\r");
          Seek(fh,0,OFFSET_END);
          size = (Seek, fh, 0, OFFSET_BEGINNING);
          fullbuf[size] = AllocMem(size+4, MEMF_PUBLIC|MEMF_CLEAR);
          if(!fullbuf[size]) { Close(fh); exit(0); }                       
        	Read(fh, fullbuf[size] , sizeof(fullbuf[size]));
          Close(fh);               
          
          while(i<=size+4)
          {
                i++; 
                printf(fullbuf[i]);
                //if(fullbuf[i]==13) PS("\r");              
          }          
          FreeMem(fullbuf[size], size+4);                  
   	} 
}

void pause(void)
{
        XIMFunction(5,1,"\n\rPaused....");
        if(themsg.String[0]=='q') exit(0);
        PS("[2J");
}
void cursor(int zeich,BPTR fh)
{
        // First call RAWARROWXIM (501).
        // The value of themsg.String[0] after a JH_HK is
        // 4 - UP
        // 5 - DOWN
        // 2 - LEFT
        // 3 - RIGHT

      char linebuf[79],zeichbuf[1],markline[79],newline[79];
      int zeichcount=0; 
      PS("[0;0h");
newline:
     Seek(fh,-zeich,OFFSET_CURRENT);
nextline:     
     while(Read(fh, linebuf, sizeof(zeichbuf)))
     {
        sprintf(zeichbuf,"%c",linebuf[0]);
        zeichcount++;
        strncat(newline,zeichbuf,1);
        if(zeichbuf[0]=='\n')
        {
                sprintf(markline,"\r[0;33;44m%s[0m\n\r",newline);
                PS(markline);
                zeich=zeich-zeichcount;
                zeichcount=0;
                linebuf[0]=0;
                zeichbuf[0]=0;
                break;
        }        
     }      
      
     for( ;; )
     {
          XIMFunction(501, 0, 0);
          XIMFunction(6, 0, 0);
          switch(themsg.String[0])
          {
               case 4:
                    PS("[AYou hit UP!!");
                    break;
               case 5:   // Down 
                    goto nextline;
                    break;
               case 3:
                    PS("[CYou hit RIGHT!");
                    break;
               case 2:   // Left
                    return;                    
               case 'q':
                    exit(0);     
          }
     }
               

}     

   




