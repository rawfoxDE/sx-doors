#include "protos.h"

struct Library *MUIMasterBase;

//start "These functions do not need to be modified.  Main is in here."

//start "Main Function"
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
      while(DoMethod(app->app, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit)
         if(sigs)
            if((sigs = Wait(sigs|SIGBREAKF_CTRL_C)) & SIGBREAKF_CTRL_C) break;
#if REALLYQUIT == 1
            GetAttr(MUIA_Application_ForceQuit, app->app, &quit);
         if(!quit) quit = (EasyRequest(NULL, &req, NULL) == 1);
     }while(!quit);
#endif
   MUI_DisposeObject(app->app);
   free(app);
   return(RETURN_OK);
}
//end

#if DOUBLESTART == 1
//start "Hook function - ds_Hook_Func()"
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
//end
#endif


//start "struct App *CreateApp(void)"

struct App * createApplication(void)
{
   struct App *app;

   app = malloc(sizeof(struct App));
   if(!app)
   {
      printf("Could not allocate memory do struct App!!!");
      exit(RETURN_FAIL);
   }

   app->app = createApp(app);

   /* do notifies and all other methods */
   createNotifys(app);

   return(app);
}
//end

//start "void privateNotifies(struct App *)"
void privateNotifies(struct App *app)
{
//start "These notifications do not need to be modified"
#if DOUBLESTART == 1
   static struct Hook ds_Hook =
          {{NULL, NULL}, ds_Hook_Func, NULL, NULL};

   DoMethod(app->app, MUIM_Notify, MUIA_Application_DoubleStart, MUIV_EveryTime,
            app->app, 3, MUIM_CallHook, &ds_Hook,app->app);
#endif

//end
}
//end

//end







