#ifndef MUI_MAIN_H
#define MUI_MAIN_H

//start "Project definition"
//**************************

// Define this to enable MUIA_Applicaton_DoubleStart checking
#define DOUBLESTART             1

// Define this to enable the 'Really Quit?' requester
#define REALLYQUIT              1

//end

//start "System Includes"
//***********************

   #include<stdlib.h>
   #include<stdio.h>
   #include<dos/dos.h>
   #include<exec/exec.h>
   #include<exec/libraries.h>
   #include<clib/exec_protos.h>
   #include<exec/types.h>
   #include<clib/alib_protos.h>
   #include<clib/intuition_protos.h>
   #include<proto/muimaster.h>
   #include<libraries/mui.h>
   #include<utility/hooks.h>
   #include <exec/memory.h>

//end

extern struct Library *MUIMasterBase;

//start "struct App Application's Objects"
//****************************************

struct ObjApp
{
	APTR	App;          // configwin
	APTR	WI_label_0;   // 1. vir. group
	APTR	STR_label_0C; // Nickname
	APTR	STR_label_1C; // Server
	APTR	STR_label_2C; // Port
	APTR	STR_label_3C; // Channel
	APTR	STR_label_4C; // Realname
};

/***************************************************************************/
/*          Declare any other variables, structures or what not            */
/***************************************************************************/

extern struct ObjApp * CreateApp(void);
extern void DisposeApp(struct ObjApp *);

extern int NICK;
extern int SERVER;
extern int PORT;
extern int CHANNEL;
extern int REALNAME;


//start " Program Function Prototypes"
//************************************

  /*************************************************************************/
 /* Program funtion prototypes begin here                                 */
/*************************************************************************/

//start "These prototypes do not need to be modified"
struct App           *createApplication(void);
Object               *createAppObj(     struct App *);
void                  createNotifys(    struct App *);
ULONG __saveds __asm  ds_Hook_Func(     register __a0 struct Hook *,
                                        register __a2 APTR,
                                        register __a1 APTR);
void privateNotifies(struct App *);
void handleComArgs(int, char *[]);

#endif

