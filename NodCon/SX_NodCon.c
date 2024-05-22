static char __version[] ="\0$VER: SX_NodCon 1.06 [eSSeXX] ("__DATE__")";
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <dos.h>
#include <fcntl.h> 
#include <sxstructs.h>

void NodCon(long number,long nodeact);
void help(void);
int main(int argc, char *argv[])
{
        long nodenumber,argument;
        char nodenum[8],argnum[8],buf[256];
        
        if(!argv[1] || argv[1][0]==0 || argv[1][0]=='?') 
        {
                help();
        } else
        {                                                 
                 sprintf(nodenum,"%c",argv[1][0]);
                 sprintf(argnum,"%c",argv[1][1]);
                 
                 nodenumber=atol(nodenum);
                 argument=atol(argnum);
                 
                 if(argument==6)
                 {
                        sprintf(buf,"run >nil: sx:sx %ld",nodenumber);
                        Execute(buf,0,0);
                 } else NodCon(nodenumber,argument);
        }
}        
        

void NodCon(long number,long nodeact)
{
	char portname[32];

	struct MsgPort *nport;
	struct NodeMessage
	{
       	 struct Message Msg;
       	 UWORD command;
       	 UWORD misc;
       	 long data1;
    	 long data2;
       	 long data3;
       	 long data4;
	} themsg;

	sprintf(portname, "SX-Node%ld", number);

	nport = FindPort(portname);
	if(nport)
	{
		if(themsg.Msg.mn_ReplyPort = CreateMsgPort())
		{
			themsg.Msg.mn_Length = sizeof(struct NodeMessage);
			themsg.command = nodeact; // the variable for the action
			PutMsg(nport, (struct Message *)&themsg);
			WaitPort(themsg.Msg.mn_ReplyPort);
			GetMsg(themsg.Msg.mn_ReplyPort);
			DeleteMsgPort(themsg.Msg.mn_ReplyPort);
		}
	}
}
void help(void)
{
        printf("\n\r[0;31;43mSX_NodCon V1.06 (eSSeXX)[0m\n\r");
        printf("\n\r[1;32mNodCon <Node num.><Action num.>[0m");
   printf("\n\r\n\r[34mAction Commands :[31m");
   printf("\n\rNode Number - give the Number of the Node here");
   printf("\n\rNode Action - give the Number of the Action here");
   printf("\n\r\n\r              1 = Close Node Screen");
   printf("\n\r              2 = Open Node Screen");
   printf("\n\r              3 = ShutDown Node");
   printf("\n\r              4 = Kick User on Node");
   printf("\n\r              5 = Chat to User on Node");
   printf("\n\r              6 = Launch selected Node");
   printf("\n\r\n\rExample:  `NodCon 12` - Open Screen on Node 1");
   printf("\n\r          `NodCon 33` - ShutDown Node 3");
   printf("\n\r          `NodCon 24` - Kick User on Node 2");
   printf("\n\r\n\r-> ");
}
