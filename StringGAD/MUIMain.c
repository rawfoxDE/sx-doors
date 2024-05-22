#include "protos.h"

#include <stdlib.h>
#include <stdio.h>
#include <dos/dos.h>
#include <exec/exec.h>
#include <exec/memory.h>
#include <clib/alib_protos.h>
#include <exec/libraries.h>
#include <clib/exec_protos.h>
#include <exec/types.h>
#include <clib/intuition_protos.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>
#include <utility/hooks.h>

struct Library *MUIMasterBase;

int main(int argv, char *argc[])
{
   struct App *app;
   ULONG sigs = 0;

  #if REALLYQUIT == 1
   ULONG quit;
   static struct EasyStruct req =
   {
      sizeof(struct EasyStruct), 0,
      "Really?",
      "Really quit?",
      "Yes|No"
   };
  #endif

   handleComArgs(argv, argc);

   MUIMasterBase = (struct Library *)OpenLibrary(MUIMASTER_NAME,
                                                 MUIMASTER_VMIN);
   if(!MUIMasterBase)
   {
      printf("Unable to open MUIMaster.library!\n");
      exit(RETURN_FAIL);
   }

   app = createApplication();

   /* Do the intuiloop */
  #if REALLYQUIT == 1
   	do{
  #endif
      	while(DoMethod(app->app, MUIM_Application_NewInput, &sigs)
              != MUIV_Application_ReturnID_Quit)
         if(sigs)
            if((sigs = Wait(sigs|SIGBREAKF_CTRL_C)) & SIGBREAKF_CTRL_C)
               break;
#if REALLYQUIT == 1
      GetAttr(MUIA_Application_ForceQuit, app->app, &quit);
      if(!quit)
         quit = (EasyRequest(NULL, &req, NULL) == 1);
   }while(!quit);
#endif
   MUI_DisposeObject(app->app);
   free(app);
   return(RETURN_OK);
}

#if DOUBLESTART == 1

ULONG __saveds __asm ds_Hook_Func(register __a0 struct Hook *hook,
                                  register __a2 APTR         object,
                                  register __a1 APTR         contents)
{
   static struct EasyStruct req =
   {
      sizeof(struct EasyStruct), 0,
      "MUIA_Applicaton_DoubleStart error!",
      "Application may be started\nonly once!",
      "OK"
   };

   EasyRequest(NULL, &req, NULL);

   return((ULONG)object);
}

#endif


struct App *createApplication(void)
{
   struct App *app;

   app = malloc(sizeof(struct App));
   if(!app)
   {
      printf("Could not allocate memory do struct App!!!");
      exit(RETURN_FAIL);
   }

   app->app = createAppObj(app);

   /* do notifies and all other methods */
   createNotifys(app);

   return(app);
}

void privateNotifies(struct App *app)
{

#if DOUBLESTART == 1
   static struct Hook ds_Hook =
          {{NULL, NULL}, ds_Hook_Func, NULL, NULL};

   DoMethod(app->app, MUIM_Notify, MUIA_Application_DoubleStart, MUIV_EveryTime,
            app->app, 3, MUIM_CallHook, &ds_Hook,app->app);
#endif


}
void handleComArgs(int argc, char *argv[])
{
   

}

Object *createAppObj(struct App *app)
{
   Object *appObj;

   appObj = ApplicationObject,
      MUIA_Application_Title,       "DCIRC Setup",
      MUIA_Application_Version,     "$VER 1.0 (5/24/97)",
      MUIA_Application_Copyright,   "Copyright by DigitalCorruption",
      MUIA_Application_Author,      "[r]FoX/DC",
      MUIA_Application_Description, "Amiga IRC-Client",
      MUIA_Application_Base,        "DCIRC.1",
#if DOUBLESTART == 1
      MUIA_Application_SingleTask,  TRUE,
#endif

   CreateApp();

   End;
   if(!appObj) exit(RETURN_FAIL);

   return(appObj);
}

void  createNotifys(struct App *app)
{
   privateNotifies(app);

}
