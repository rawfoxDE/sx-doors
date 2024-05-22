
/*****  IMPORTANT NOTE  ******  IMPORTANT NOTE  ******  IMPORTANT NOTE  ******


This source code uses the tag base TAGBASE_BLIZZY, which is defined as
(TAG_USER | (1465UL << 16)).

PLEASE *DO* *NOT* *USE* THIS TAG BASE IN YOUR OWN APPLICATIONS!!!!!!!!!!!!
It is a personalized tag base!!!!!!!!!!

If you need such a tag base for creating attributes and methods identifiers,
you *MUST* write to mccreg@sasg.com and ask for a new tag base for you!!!!!

This is really important!!!


******  IMPORTANT NOTE  ******  IMPORTANT NOTE  ******  IMPORTANT NOTE  *****/



/*** include files ***/
#include <exec/types.h>
#if defined __MAXON__ || defined __STORM__
#include <pragma/exec_lib.h>
#else
#include <proto/exec.h>
#endif
#include <exec/libraries.h>
#if defined __MAXON__ || defined __STORM__
#include <pragma/muimaster_lib.h>
#else
#include <proto/muimaster.h>
#endif
#include <libraries/mui.h>
#include <intuition/classusr.h>
#if defined __MAXON__ || defined __STORM__
#include <pragma/utility_lib.h>
#else
#include <proto/utility.h>
#endif
#include <utility/utility.h>
#include <libraries/iffparse.h>
#include <clib/alib_protos.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined __MAXON__ || defined __STORM__
#include <wbstartup.h>
#endif

#include "OOPExample.h"


/*** constants ***/
/* attributes */
#define MUIA_OOPWindow_ButtonTitle         (TAGBASE_BLIZZY + 1) /* [I.G]  STRPTR   */
#define MUIA_OOPWindow_ButtonShortHelp     (TAGBASE_BLIZZY + 2) /* [I.G]  STRPTR   */
#define MUIA_OOPWindow_WindowToOpenOrClose (TAGBASE_BLIZZY + 3) /* [ISG]  Object * */


/* methods */
#define MUIM_OOPWindow_OpenOrCloseWindow   (TAGBASE_BLIZZY + 1)

/* miscellaneous */
#define STR_VERSI_OOPEXAMPLE "1.1"
#define STR_DATE_OOPEXAMPLE  "06-Jun-1998"
#define STR_VDATE_OOPEXAMPLE "6.6.98"


/*** structures ***/
/* class instance data structure */
struct OOPWindowData
{
  Object *wiWindowToOpenOrClose;
  char   *button_title;
  char   *button_shorthelp;
};


/*** macros ***/
/* this macro simplifies creation of OOPWindow objects */
#define OOPWindowObject NewObject(clOOPWindow->mcc_Class, NULL


/*** prototypes ***/
BOOL  gui_init(void);
void  gui_exit(void);
BOOL  classes_init(void);
void  classes_exit(void);

ULONG ASM SAVEDS OOPWindow_Dispatcher(REGS(a0, struct IClass *), REGS(a2, Object *), REGS(a1, Msg));

ULONG DoSuperNew(struct IClass *, Object *, ULONG, ...);
ULONG xget(Object *, ULONG);


/*** global identifiers ***/
Object                 *apMain        = NULL;
Object                 *  owWindow1;
Object                 *  owWindow2;

struct MUI_CustomClass *clOOPWindow   = NULL;

struct Library         *MUIMasterBase = NULL;
struct UtilityBase     *UtilityBase   = NULL;


/*** program ***/
int main(void)
{
  int ret = RETURN_FAIL;

  /* open all necessary libraries */
  if (!(UtilityBase = (struct UtilityBase *)OpenLibrary(UTILITYNAME, 37)))
  {
    printf("Couldn't open %s V37!\n", UTILITYNAME);
    goto bye;
  }
  if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME, MUIMASTER_VMIN)))
  {
    printf("Couldn't open %s V%lu!\n", MUIMASTER_NAME, (ULONG) MUIMASTER_VMIN);
    goto bye;
  }

  /* create custom classes */
  if (!classes_init())
  {
    printf("Couldn't create custom classes!\n");
    goto bye;
  }

  /* create application */
  if (!gui_init())
  {
    printf("Couldn't create application!\n");
    goto bye;
  }

  /* This is the ideal input loop for an object oriented MUI
  ** application. Everything is encapsulated in classes, no
  ** return IDs need to be used, we just check if the program
  ** shall terminate.
  **
  ** (Quote and source part copied from the original MUI distribution.)
  */
  {
    ULONG sigs = 0UL;

    while (DoMethod(apMain, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit)
    {
      if (sigs)
      {
        sigs = Wait(sigs | SIGBREAKF_CTRL_C);
        if (sigs & SIGBREAKF_CTRL_C)
          break;
      }
    }
  }

  ret = RETURN_OK;

  bye:
  /* free all resources */
  gui_exit();
  classes_exit();
  if (MUIMasterBase) CloseLibrary(MUIMasterBase);
  if (UtilityBase)   CloseLibrary((struct Library *)UtilityBase);
  exit(ret);
}

/* setup application and do all necessary initializations */
BOOL gui_init(void)
{
  if (apMain = ApplicationObject,
    MUIA_Application_Title,       "OOPExample",
    MUIA_Application_Version,     "$VER: OOPExample " STR_VERSI_OOPEXAMPLE " (" STR_VDATE_OOPEXAMPLE ") © by Maik \"bZ!\" Schreiber, "
#ifdef __SASC
                                  "SAS/C"
#elif defined __MAXON__
                                  "MaxonC++"
#elif defined __STORM__
                                  "StormC"
#elif defined _DCC
                                  "Dice C"
#elif defined __GNUC__
                                  "GNU C"
#else
                                  "unknown compiler"
#endif
                                  " version",
    MUIA_Application_Copyright,   "Copyright © " STR_DATE_OOPEXAMPLE " by Maik Schreiber <bZ@iq-computing.de>",
    MUIA_Application_Author,      "Maik Schreiber <bZ@iq-computing.de>",
    MUIA_Application_Description, "Learn OOP with MUI!",
    MUIA_Application_Base,        "OOPEXAMPLE",

    SubWindow, owWindow1 = OOPWindowObject,
      MUIA_Window_Title,              "OOPExample · Window 1",
      MUIA_Window_ID,                 MAKE_ID('W', 'I', 'N', '1'),
      MUIA_OOPWindow_ButtonTitle,     "Open/close window _2",
      MUIA_OOPWindow_ButtonShortHelp, "This button opens or closes window 2.",
    End,

    SubWindow, owWindow2 = OOPWindowObject,
      MUIA_Window_Title,              "OOPExample · Window 2",
      MUIA_Window_ID,                 MAKE_ID('W', 'I', 'N', '2'),
      MUIA_OOPWindow_ButtonTitle,     "Open/close window _1",
      MUIA_OOPWindow_ButtonShortHelp, "This button opens or closes window 1.",
    End,

  End)
  {
    /* specify which window's button should open/close which window */
    set(owWindow1, MUIA_OOPWindow_WindowToOpenOrClose, owWindow2);
    set(owWindow2, MUIA_OOPWindow_WindowToOpenOrClose, owWindow1);

    /* this lets us quit the program by closing one of the windows directly */
    DoMethod(owWindow1, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, apMain, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    DoMethod(owWindow2, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, apMain, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);

    /* open window 1 and let MUI do the work :-) */
    set(owWindow1, MUIA_Window_Open, TRUE);

    return(TRUE);
  }
  return(FALSE);
}
/* dispose application */
void gui_exit(void)
{
  if (apMain) MUI_DisposeObject(apMain);
}
/* create custom classes */
BOOL classes_init(void)
{
  /* This is one of the essential parts when fiddling with custom classes.
  ** Here we create our own custom classes, which can be derived from
  ** any MUI class. Our new classes can also be derived from themselves,
  ** e.g. a new window class derived from Window.mui and a second new
  ** window class which is derived from that first window class.
  **
  ** In this OOPExample program, we create a new window class which is
  ** derived from Window.mui.
  **
  ** A note about the datasize parameter of MUI_CreateCustomClass():
  ** This parameter is used to tell MUI, how much memory a class instance
  ** of this class needs for its data. MUI allocates a memory block of
  ** that given size, which you may use for anything you want.
  **
  ** You can always get a pointer to an object's instance data using
  **
  **   INST_DATA(class, obj);   // see <intuition/classes.h>
  */
  clOOPWindow = MUI_CreateCustomClass(NULL, MUIC_Window, NULL, sizeof(struct OOPWindowData), OOPWindow_Dispatcher);
  if (clOOPWindow)
    return(TRUE);
  return(FALSE);
}
/* dispose custom classes */
void classes_exit(void)
{
  if (clOOPWindow) MUI_DeleteCustomClass(clOOPWindow);
}

/* This is the constructor of our newly created OOPWindow custom class. */
ULONG OOPWindow_NEW(struct IClass *cl, Object *obj, struct opSet *msg)
{
  Object         *button;
  struct TagItem *tag;
  char           *button_title;
  char           *button_shorthelp;

  /* The MUIA_OOPWindow_ButtonTitle attribute needs a bit more overhead... */
  if (tag = FindTagItem(MUIA_OOPWindow_ButtonTitle, msg->ops_AttrList))
    button_title = (char *) tag->ti_Data;
  else
    button_title = "";

  /* ... as does the MUIA_OOPWindow_ButtonShortHelp attribute... */
  if (tag = FindTagItem(MUIA_OOPWindow_ButtonShortHelp, msg->ops_AttrList))
    button_shorthelp = (char *) tag->ti_Data;
  else
    button_shorthelp = "";


  /* This is the point where we create the new object from our class.
  ** There exist two ways for doing this:
  **
  ** 1. Just creating the new object.
  **    This is done via DoSuperMethodA():
  **
  **    if (obj = (Object *) DoSuperMethodA(cl, obj, (Msg)))
  **    {
  **      ...
  **    }
  **
  ** 2. Creating the object plus adding some new tags to its initial
  **    taglist.
  **    This is done via DoSuperNew(), which is an extra function calling
  **    DoSuperMethodA() again:
  **
  **
  **    if (obj = (Object *) DoSuperNew(cl, obj,
  **      ... add new ("hardcoded") tags here ...
  **      TAG_MORE, msg->ops_AttrList))
  **    {
  **      ...
  **    }
  **
  **
  ** In OOPExample, we want to create a button inside the window object.
  ** Since the main program should not create the button but we want to
  ** create it ourselves, we have to do a call to DoSuperNew().
  */
  if (obj = (Object *) DoSuperNew(cl, obj,
    WindowContents, VGroup,
      Child, TextObject,
        MUIA_Text_Contents, "\033c\033b\033uOOPExample " STR_VERSI_OOPEXAMPLE,
        MUIA_Font, MUIV_Font_Big,
      End,
      Child, TextObject,
        MUIA_Text_Contents, "\033cCopyright © " STR_DATE_OOPEXAMPLE " by Maik \"bZ!\" Schreiber/IQ Computing",
      End,
      Child, TextObject,
        MUIA_Text_Contents, "\033cPlease visit \033bThe IQ Computing Web Site \033nat \033bwww.IQ-Computing.de",
      End,
      Child, TextObject,
        MUIA_Text_Contents, "\033c(This version was compiled using "
#ifdef __SASC
                            "SAS/C."
#elif defined __MAXON__
                            "MaxonC++."
#elif defined __STORM__
                            "StormC."
#elif defined _DCC
                            "Dice C."
#elif defined __GNUC__
                            "GNU C."
#else
                            "an unknown C compiler?!"
#endif
                            ")",
        MUIA_Font, MUIV_Font_Tiny,
      End,
      Child, button = SimpleButton(button_title),
    End,
    InitialAttrs()))
  {
    /* get instance data of the newly created object */
    struct OOPWindowData *data = GetInstData();

    /* set default values */
    ClearInstData();
    data->button_title     = button_title;
    data->button_shorthelp = button_shorthelp;

    /* use a trick to set the initial values */
    InitObjAttrs();

    /* set cycle chain... */
    set(button, MUIA_CycleChain, TRUE);

    /* ... and bubble help */
    set(button, MUIA_ShortHelp, button_shorthelp);

    /* Setup notification for the button to open/close a window.
    ** We simply send ourselves the MUIM_OOPWindow_OpenOrCloseWindow
    ** method.
    */
    DoMethod(button, MUIM_Notify, MUIA_Pressed, FALSE, obj, 1, MUIM_OOPWindow_OpenOrCloseWindow);

    /* return address of the new object */
    return((ULONG) obj);
  }
  /* return failure indicator */
  return(0);
}
/* This is the OM_SET method handler of our class. */
ULONG OOPWindow_SET(struct IClass *cl, Object *obj, struct opSet *msg)
{
  /* get instance data of our object */
  struct OOPWindowData *data      = GetInstData();
  struct TagItem       *tags      = msg->ops_AttrList;
  struct TagItem       *tag;

  /* The identifier no_notify gives us the possibility to handle
  ** the MUIA_NoNotify attribute in our class. If you need this feature
  ** in your class, simple remove the comment markers here and a few
  ** lines below. Then, you have to write
  **
  **   SetAttrs(obj, MUIA_NoNotify, no_notify, ...);
  **
  ** to take care of MUIA_NoNotify.
  */
/*
  BOOL                  no_notify = FALSE;
*/

  /* There are many ways to find out what tag items provided by set()
  ** we do know. The best way should be using NextTagItem() and simply
  ** browsing through the list.
  */
  while (tag = NextTagItem(&tags))
  {
    switch (tag->ti_Tag)
    {
      /* handle MUIA_NoNotify in our class */
/*
      case MUIA_NoNotify:
        no_notify = (BOOL) tag->ti_Data;
        break;
*/

      /* specify the window, which should be opened/closed on pressing
      ** the button
      */
      case MUIA_OOPWindow_WindowToOpenOrClose:
        data->wiWindowToOpenOrClose = (Object *) tag->ti_Data;
        break;
    }
  }
  return(CallSuperClass());
}
/* Here comes the OM_GET method handler. */
ULONG OOPWindow_GET(struct IClass *cl, Object *obj, struct opGet *msg)
{
  /* The OM_GET handler also has a very special behaviour:
  ** Since you can only get() one attribute value at a time,
  ** it has to return() TRUE if it does "understand" your
  ** attribute identifier.
  ** If not, the handler must pass the attribute identifier
  ** to the superclass via DoSuperMethodA().
  */

  /* get instance data of our object */
  struct OOPWindowData *data = GetInstData();

  switch(msg->opg_AttrID)
  {
    /* Return button title of our button.
    ** This attribute isn't used in this example program,
    ** but it shows another time how the technique is done.
    **
    ** NOTE: In this example, the returned string must not be
    ** modified!
    */
/*
    ReturnAttrValue(MUIA_OOPWindow_ButtonTitle, data->button_title);
*/

    /* return bubble help string of our button */
/*
    ReturnAttrValue(MUIA_OOPWindow_ButtonShortHelp, data->button_shorthelp);
*/

    /* return the specified window, which should be opened/closed
    ** on pressing the button
    */
    ReturnAttrValue(MUIA_OOPWindow_WindowToOpenOrClose, data->wiWindowToOpenOrClose);
  }

  /* our handler didn't understand the attribute, we simply pass
  ** it to our superclass now
  */
  return(CallSuperClass());
}
/* MUIM_OOPWindow_OpenOrCloseWindow method handler */
ULONG OOPWindow_OpenOrCloseWindow(Object *obj)
{
  /* This is the "main" method of our OOPWindow class.
  ** Here's what it does:
  **
  ** 1. Get the window which should be opened or closed.
  ** 2. Open/close the window, depending on its current state
  **    (ie. if it's open, close it, otherwise open it).
  **
  ** Since this is a new method, we don't have anything to pass to
  ** our superclass here. Instead, we return() TRUE to indicate that
  ** all went ok.
  **
  ** NOTE: In this example, the return value of this method isn't used,
  ** so we could also return() FALSE here. When writing custom classes,
  ** it can sometimes be very handy to have return values. You can get
  ** them from your DoMethod() call to this method.
  */

  Object *window;

  if (window = (Object *) xget(obj, MUIA_OOPWindow_WindowToOpenOrClose))
    set(window, MUIA_Window_Open, xget(window, MUIA_Window_Open) ? FALSE : TRUE);
  return(TRUE);
}
/* Here you see the dispatcher of our class. */
ULONG ASM SAVEDS OOPWindow_Dispatcher(REGS(a0, struct IClass *cl), REGS(a2, Object *obj), REGS(a1, Msg msg))
{
  /* The class dispatcher is the function that will be called when a
  ** method is sent to your class. The dispatcher has to determine
  ** if it understands the method. If yes, it calls the according
  ** method handler (you can also write the handler here instead of
  ** return()ing a function return value - that's a style question).
  ** If not, it has to pass the method to the superclass.
  */

  /* watch out for methods we do understand */
  switch (msg->MethodID)
  {
    /* Whenever an object shall be created using NewObject(), it will be
    ** sent a OM_NEW method.
    */
    case OM_NEW:                           return(OOPWindow_NEW(cl, obj, (struct opSet *) msg));

    /* SetAttrs() sends a OM_SET method */
    case OM_SET:                           return(OOPWindow_SET(cl, obj, (struct opSet *) msg));

    /* GetAttr() sends a OM_GET method */
    case OM_GET:                           return(OOPWindow_GET(cl, obj, (struct opGet *) msg));

    /* one of our own methods */
    case MUIM_OOPWindow_OpenOrCloseWindow: return(OOPWindow_OpenOrCloseWindow(obj));
  }

  /* we didn't understand the last method, so call our superclass for help */
  return(CallSuperClass());
}

/* This is a small function to add some new capabilities to
** DoSuperMethodA().
*/
ULONG DoSuperNew(struct IClass *cl, Object *obj, ULONG tag1, ...)
{
  return(DoSuperMethod(cl, obj, OM_NEW, &tag1, NULL));
}
/* This simplifies get()ting values of attributes. */
ULONG xget(Object *obj, ULONG attr)
{
  ULONG ret;

  get(obj, attr, &ret);
  return(ret);
}

