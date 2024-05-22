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


void PS(char * str);
void XIMFunction(int func, long data, char * str);
void Race(void);
int rnd(int);
void cara(int); // die MOVE-Routinen...
void carb(int);
void carc(int);
void card(int);
void care(int);
void carf(int);

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

			Race();

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

/* ============ CARMOVE ROUTINE ============== */

void Race(void)
{
        int camov,cbmov,ccmov;
        int cdmov,cemov,cfmov;                        // t = random start
        int t,z,won;
        char buf[256];                                // y = counter foro mainloop        
                                                      // z = zufallszahl -> welches auto bewegen    
        won=0;
        camov=0;
        cbmov=0;
        ccmov=0;
        cdmov=0;
        cemov=0;
        cfmov=0;
                                       
        PS("\033[2J");                
        PS("Hello! Welcome to a test door\r\n\r\n");
        
	XIMFunction(6,0,"press to go...(ret/q) \n\r\n\r");

        PS("3 !\n\r");
        Delay(40);
        PS("2 !\n\r");
        Delay(40);
        PS("1 !\n\r");
        Delay(40);
        PS("G O O ooo!\n\r");
        
        t=80;
        while(t)                                // der Block fuer die Startberechnung
        {
               XIMFunction(17,0,0);  
               if(themsg.Command == '\r') break;
               sprintf(buf,"\r%ld",t);
               PS(buf);
               t--;
               Delay(0);	
        } 
                   
                                              // die Autos fahren los...
 
        PS("\033[2J");
 
        while(won<1)                           //...mer fahren bis zum Sieg !
        {
                if(t==0) // Sicherheits Abfrage, sonst endlos Loop
                {
                        PS(" LOOOSER !! zu lahm ...");
                        Delay(50)
                        return;
                        
                }
                           
                z=rnd(t);
             
                if(z==1)                                                                        // AUTO 1   
                     
                {
                         camov++;
                         cara(camov);    
                         if(camov==50) won++; // camov schon 20 ?..gewonnwn ?
                }         
                                              
                if(z==2)                                                                         // AUTO 2  
                {
                         cbmov++;
                         carb(cbmov);
                         if(cbmov==50) won++;
                }
                
                if(z==3)                                                                        // AUTO 3  
                {
                         ccmov++;
                         carc(ccmov);
                         if(ccmov==50) won++;
                }
        
                if(z==4)                                                                        // AUTO 4  
                {
                        cdmov++;
                        card(cdmov);
                        if(cdmov==50) won++;
                }
        
                if(z==5)                                                                        // auTO 5  
                {
                        cemov++;
                        care(cemov);
                        if(cemov==50) won++;
                }
                
                if(z==6)                                                                        // AUTO 6  
                {
                        cfmov++;
                        carf(cfmov);
                        if(cfmov==50) won++;
                }        
        
        }
         
        XIMFunction(6,0,"\n\rA Car has Won !!!...press a key..."); 
        PS("\033[20;0H\r\nExiting...\r\n\r\n");
}

int rnd(int x)
{
       	int i;
	if(x <= 0) return(0);
	do
        {
	       i = rand()%x;
	}
	while(i > x);
	return(i);
} 

void cara(int camov)   // CAR 1 Mover
{
        int q;
        PS("\033[4;4H");
        for(q=0; q<camov; q++)
        {
                PS(" ");
        }       
        PS("\033[32m.------\\");  // oberteil
        PS("\033[1B\033[9D");     // cursor pos
        PS(" |-o--o-'");          // unterteil
        PS("\033[1A\033[9D ");    // cursor pos
        Delay(1);  // TESTPAUSE
}  


void carb(int cbmov)   // CAR 2 Mover
{
        int q;
        PS("\033[7;4H");
        for(q=0; q<cbmov; q++)
        {
              PS(" ");           
        }       
        PS("\033[36m .___,--.");
        PS("\033[1B\033[9D");
        PS(" `-o--o-'");
        PS("\033[1A\033[9D ");
     
        Delay(1);  // TESTPAUSE
} 


void carc(int ccmov)   // CAR 3 Mover
{
        int q;
        PS("\033[10;4H");
        for(q=0; q<ccmov; q++)
        {
              PS(" ");           
        }       
        PS("\033[37m ----.. ");
        PS("\033[1B\033[9D");
        PS(" `-O--o-'");
        PS("\033[1A\033[9D ");
           
        Delay(1);  // TESTPAUSE
} 

void card(int cdmov)   // CAR 4 Mover
{
        int q;
        PS("\033[13;4H");
        for(q=0; q<cdmov; q++)
        {
              PS(" ");           
        }       
        PS("\033[31m ____.. ");
        PS("\033[1B\033[9D");
        PS(" `-o--o-\\");
        PS("\033[1A\033[9D ");
           
        Delay(1);  // TESTPAUSE
} 

void care(int cemov)   // CAR 5 Mover
{
        int q;
        PS("\033[16;4H");
        for(q=0; q<cemov; q++)
        {
              PS(" ");           
        }       
        PS("\033[35m -__,._ ");
        PS("\033[1B\033[9D");
        PS(" `-O--O-\\");
        PS("\033[1A\033[9D ");
           
        Delay(1);  // TESTPAUSE
} 

void carf(int cfmov)   // CAR 6 Mover
{
        int q;
        PS("\033[19;4H");
        for(q=0; q<cfmov; q++)
        {
              PS(" ");           
        }       
        PS("\033[34m ____\\. ");
        PS("\033[1B\033[9D");
        PS(" `-o--o-|");
        PS("\033[1A\033[9D ");
           
        Delay(1);  // TESTPAUSE
} 
