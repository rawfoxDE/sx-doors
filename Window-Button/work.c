#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <exec/memory.h>
#include <exec/types.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/gadtools.h>

#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>

#include <libraries/gadtools.h>
#include <libraries/reqtools.h>

#include <dos/dos.h>
#include <dos/dostags.h>

#include <workbench/workbench.h>

#include <proto/reqtools.h>

#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>

#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/utility_protos.h>

#include <pragmas/exec_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/utility_pragmas.h>

#include "workwin.h"

BOOL runnin = TRUE;

void HandleProject0IDCMP( void );

void main( int argc, char *argv[])
{
   struct Library *IntuitionBase, *GfxBase, *GadToolsBase;
   struct ReqToolsBase *ReqToolsBase;

	IntuitionBase = OpenLibrary("intuition.library", 37);
	if(IntuitionBase)
	{
   	GfxBase = OpenLibrary("graphics.library", 37);
		GadToolsBase = OpenLibrary("gadtools.library", 37);

 		ReqToolsBase = (struct ReqToolsBase *)OpenLibrary (REQTOOLSNAME, REQTOOLSVERSION);
		if(ReqToolsBase)
		{
         if(SetupScreen()==0)
			{
				if(OpenProject0Window()==0)
				{                 
					  while(1)
                 {
                       HandleProject0IDCMP();
                       if(runnin == FALSE)    
                       { 
                           if(Project0Wnd) CloseProject0Window();
                           CloseProject0Window();
                           break;
			   	        }    
                       
                 }
                                         
				}
				CloseDownScreen();
			}
			CloseLibrary((struct Library *)ReqToolsBase);
		}

		CloseLibrary(GadToolsBase);
		CloseLibrary(GfxBase);
		CloseLibrary(IntuitionBase);
	}
   exit(0);
}

void HandleProject0IDCMP( void )
{
	struct IntuiMessage	*m;
	struct Gadget *gad;
   UWORD code;

	while( m = GT_GetIMsg( Project0Wnd->UserPort )) 
   {
		gad = (struct Gadget *) m->IAddress;
      code = m->Code; 

		GT_ReplyIMsg(m);

		switch ( m->Class ) 
      {

			case IDCMP_REFRESHWINDOW:
						GT_BeginRefresh( Project0Wnd );
						GT_EndRefresh( Project0Wnd, TRUE );
						break;

			case IDCMP_CLOSEWINDOW:
						runnin = FALSE;
                  CloseProject0Window();
                  break;

			case	IDCMP_GADGETUP:
                  runnin = FALSE;
                  CloseProject0Window(); 
                  break;
		}
	}
   return;
}
