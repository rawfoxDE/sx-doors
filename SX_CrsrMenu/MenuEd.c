#include <stdio.h>
#include <stdlib.h>
#include <exec/memory.h>
#include <proto/all.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/gadtools.h>
#include <dos/dos.h>
#include <dos/dostags.h>
#include <workbench/workbench.h>
#include <libraries/reqtools.h>
#include <proto/reqtools.h>
#include <proto/gadtools.h>
#include <exec/types.h>
#include <intuition/classes.h>
#include <intuition/classusr.h>
#include <intuition/imageclass.h>
#include <intuition/gadgetclass.h>
#include <libraries/gadtools.h>
#include <graphics/displayinfo.h>
#include <graphics/gfxbase.h>
#include <clib/exec_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/utility_protos.h>
#include <string.h>
#include <pragmas/exec_pragmas.h>
#include <pragmas/intuition_pragmas.h>
#include <pragmas/gadtools_pragmas.h>
#include <pragmas/graphics_pragmas.h>
#include <pragmas/utility_pragmas.h>
#include "menued_gui.h"

//struct Library *IntuitionBase, *GfxBase, *GadToolsBase;
//struct ReqToolsBase *ReqToolsBase;

extern void CloseProject5Window(void);
extern int OpenProject5Window(void);

void main( int argc, char *argv[])
{
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
				if(OpenProject5Window()==0)
				{
         		if(Project5Wnd) CloseProject5Window();
			   	CloseProject5Window();
				}
				CloseDownScreen();
			}
			CloseLibrary((struct Library *)ReqToolsBase);
		}

		CloseLibrary(GadToolsBase);
		CloseLibrary(GfxBase);
		CloseLibrary(IntuitionBase);
	}
}
