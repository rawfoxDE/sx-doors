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
                        
        char buf[256],player[32];
       
        
        XIMFunction(1501,0,0);
        User=(struct UserData *) themsg.Data;
        sprintf(player,"%s",User->Name);       
        
        read_data(player);
        
        PS("\n\r--------------------------------------------");
        sprintf(buf,"\n\rPLAYER : %s \n\rRACES  : %ld\n\r", Player.name, Player.races);
        PS(buf);         
        PS("\n\r--------------------------------------------"); 
                                   
        XIMFunction(6,0,"\n\rgo writing now");
        write_data(player);
        
        sprintf(buf,"\n\rData after writing :  Name: %s  , Races: %ld ",Player.name,Player.races);
        PS(buf);
        
        PS("\n\rall written...leaving now...");
      
      
}        

void read_data(char *player)
{       
        BPTR fhpl;        
        char buf[256];
         int i=0;
                             
        fhpl = Open("sx:developer/cars/car_log", MODE_OLDFILE);
        if(fhpl==NULL) 
        {
                fhpl = Open("sx:developer/cars/car_log", MODE_NEWFILE);
        
                PS("\n\redit first Entry...");
                sprintf(Player.name,"%s",player);
                Player.races=0;

                Seek(fhpl, 0, OFFSET_BEGINNING);                                             
                Write(fhpl, &Player, sizeof(struct PlayerStruct));
                Close(fhpl);
        } else if(fhpl)
        {
                Seek(fhpl,0,OFFSET_BEGINNING);
                while(Read(fhpl, &Player, sizeof(struct PlayerStruct)))
                {
                        i++;
                        if(strcmpi(Player.name,player)==NULL || i>=10) { Close(fhpl); break; } 
                } 
                sprintf(buf,"\n\rPlayer num.: %ld \n\rname: %s \n\rraces: %ld",i,Player.name,Player.races);
                PS(buf);      
                if(strcmpi(Player.name,player) != NULL)
                {
                        PS("\n\rPlayername is not Player !");
                        if(fhpl)
                        {
                                                                
                                sprintf(buf,"\n\redit first Entry for %s ...",player);
                                PS(buf);
                                
                                sprintf(Player.name,"%s",player);
                                
                                Player.races=0;
                                
                                Seek(fhpl,sizeof(struct PlayerStruct),OFFSET_END);
                                Write(fhpl, &Player, sizeof(struct PlayerStruct));
                                Close(fhpl); 
                        }        
                }                  
         }                      
         PS("\r\nExiting reading...");
}

void write_data(char *player)
{       
        BPTR fhpl;        
        
        char buf[256];         // player, playnum, win, lost, money
        int i=0;
        
        XIMFunction(6,0,"\n\rwrite data now ...");
                             
        fhpl = Open("sx:developer/cars/car_log", MODE_OLDFILE);
        if(fhpl)
        {                                
                Seek(fhpl,0,OFFSET_BEGINNING);
                do
                {
                        Read(fhpl, &Player, sizeof(struct PlayerStruct));
                        i++;
                        
                } while(strcmpi(Player.name,player)!=NULL || i>=10); 
                
                if(strcmpi(Player.name,player) != NULL)                 
                {
                        PS("\n\rPlayer not found ! maybe the Datafile is corrupt ?");
                        return;
                }        
                              
                sprintf(Player.name,"%s",player);
                Player.races++;
                   
                sprintf(buf,"\n\rNAmE : %s \n\rRACeS : %ld",Player.name,Player.races);   
                PS(buf);
                           
                Seek(fhpl, -sizeof(struct PlayerStruct), OFFSET_CURRENT);
                Write(fhpl, &Player, sizeof(struct PlayerStruct));
                Close(fhpl);              
                        
         }                             
         PS("\r\n\r\nDATA has been written...\r\n\r\n");
}







