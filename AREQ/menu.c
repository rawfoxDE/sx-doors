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
void Menu(void);
void pause(void);

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

			Menu();

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

void Menu(void)
{
	PS("[2J");

 
PS("                                                  [34m.--------------\n\r");
PS("                    [36m--[ [33maMInET aCCESs [36m]--         [34m|  [31m[ [32m1[31m] [36m- [37mbiz\n\r");
PS("                                                  [34m|  [31m[ [32m2[31m] [36m- [37mcomm\n\r");
PS("         [34m---------- - ----------------- ----- ----'  [31m[ [32m3[31m] [36m- [37mdemo\n\r");
PS("                                                     [31m[ [32m4[31m] [36m- [37mdev\n\r");
PS("           [32mI [36m- [37mAminet Index                          [31m[ [32m5[31m] [36m- [37mdisk\n\r");
PS("                                                     [31m[ [32m6[31m] [36m- [37mdocs\n\r");
PS("           [32mR [36m- [37mAminet Recent (7Days)                 [31m[ [32m7[31m] [36m- [37mgame\n\r");
PS("                                                     [31m[ [32m8[31m] [36m- [37mgfx\n\r");
PS("           [32mC [36m- [37mAminet Charts                         [31m[ [32m9[31m] [36m- [37mhard\n\r");
PS("                                                     [31m[[32m10[31m] [36m- [37minfo\n\r");
PS("           [32mT [36m- [37mAminet Tree                           [31m[[32m11[31m] [36m- [37mmisc\n\r");
PS("                                                     [31m[[32m12[31m] [36m- [37mmods\n\r");
PS("         [32mREQ [36m- [37mRequest File to List                  [31m[[32m13[31m] [36m- [37mmus\n\r");
PS("                                                     [31m[[32m14[31m] [36m- [37mpics\n\r");
PS("         [34m---------- ------------  ----------------.  [31m[[32m15[31m] [36m- [37mtext\n\r");
PS("                                                  [34m|  [31m[[32m16[31m] [36m- [37mutil\n\r");
PS("                                                  [34m|  [31m[[32mCR[31m] [36m- [37mquit\n\r");
PS("                                                  [34m`--------------\n\r");
   
	XIMFunction(5,3,"Select : ");
	switch(themsg.String[0])
	{
		case 'i':
		{

		}
		case 'q':
		{
			return;
		}
	}
}    

void pause(void)
{
        XIMFunction(5,0,"\n\rPaused....");
        if(themsg.String[0]=='q') return;
        PS("[2J");
}
