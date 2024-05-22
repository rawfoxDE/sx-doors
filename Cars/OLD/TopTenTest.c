/*

----------------------------------------------------
 Programming doors for System-X using EXEC-MESSAGES
----------------------------------------------------

See the botton of this source!

*/
//#include <libraries/dos.h> 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <sxstructs.h>

struct MsgPort *bbsport;
struct UserData *User;
//struct FileHandle *fh;
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
        char name[32],
        actualcar[1],
        lastwinner[32];
        int  points,
        won,
        races;
} Player;         

void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Door(void);
void read_data(char *player, char actualcar, char lastwinner, int points, int won, int races);

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
        char buf[256],player[32],actualcar[1],lastwinner[32];
        int points,won,races;
        XIMFunction(1501,0,0);
        User=(struct UserData *) themsg.Data;
        sprintf(player,"%s",User->Name);       
        
        read_data(player,actualcar,lastwinner,points,won,races);
        
        sprintf(buf,"%s - %s - %s - %ld - %ld - %ld",player,Player.actualcar,Player.lastwinner,Player.points,Player.won,Player.races);
        
        PS(buf);         
      
}        

void read_data(char *player, char actualcar, char lastwinner, int points, int won, int races)
{       
        BPTR fhpl;        
        
        char buf[64];         // player, playnum, win, lost, money
        int i=0;
                             
        fhpl = Open("sx:developer/cars/cars_log", MODE_OLDFILE);
        if(fhpl==NULL) fhpl = Open("sx:developer/cars/cars_log", MODE_NEWFILE);
        if(fhpl)
        {
                Seek(fhpl,0,OFFSET_BEGINNING);
                while(Player.name!=player || i>=10)
                {
                        Read(fhpl, &Player, sizeof(struct PlayerStruct));
                        i++;
                }
                if(i>=10)   // playerentry not found ! ...try BOOL !
                {
                        Seek(fhpl, - sizeof(struct PlayerStruct), OFFSET_END);
                        Read(fhpl, &Player, sizeof(struct PlayerStruct));
                        sprintf(Player.lastwinner,"%s","NONE");
                        strcpy(Player.name,player);
                        strcpy(Player.actualcar,"A");
                        Player.points=200;
                        Player.races=0;
                        Player.won=0;
                        Write(fhpl,sizeof(struct PlayerStruct),OFFSET_END);
                        Close(fhpl);
                }        
         }      
                       
     /*   if(fh)
        {           
                          
                strcpy(Player.name,player);
                 
                XIMFunction(5,6,"\n\rEnter points to bet : ");  
         
                Player.points=atoi(themsg.String);                
                  
                Seek(fh,0,OFFSET_BEGINNING);                    
                Write(fh,&Player,sizeof(struct PlayerStruct)); 
                  
                XIMFunction(6,0,"\n\rdata has been written...now try to read..\n\r");
                  
                Seek(fh,0,OFFSET_BEGINNING);
                Read(fh,&Player,sizeof(struct PlayerStruct));
         
                sprintf(buf,"\n\rName: %s  Pojtz : %ld",Player.name,Player.points);
                PS(buf);
                  
         } */       
               
         Close(fhpl);        
         PS("\r\n\r\nExiting...\r\n\r\n");
}
