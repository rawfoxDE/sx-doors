#include <clib/alib_protos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <libraries/mui.h>

/* A few test items */
STRPTR TestEntries[] =
{
	"Smoke City",
	"Tori Amos",
	"Fluke",
	"Garbage",
	"Depeche Mode",
	"Madonna",
	"Eclipse",
	"TIP",
	"Hole",
	"Nine Inch Nails",
	"Lamb",
	"Moloco",
	"Sorten Muld",
	"L7",
	"Morcheeba",
	"Portishead",
	NULL
};

VOID main ()
{
	Object *app, *win, *listview, *list;

	app = ApplicationObject,
		MUIA_Application_Author,		"Allan Odgaard",
		MUIA_Application_Base,			"ListInputTest",
		MUIA_Application_Copyright,	"®1998 Allan Odgaard",
		MUIA_Application_Window, win = WindowObject,
			MUIA_Window_Title,			"ListInputTest",
			MUIA_Window_ID,				'MAIN',
			WindowContents, GroupObject,

				MUIA_Background, MUII_WindowBack,

				Child, listview = ListviewObject,
					MUIA_Listview_List, list = ListObject,
						InputListFrame,
						MUIA_Background, MUII_ListBack,
						/* Initial contents...*/
						MUIA_List_SourceArray, TestEntries,
						End,
					End,

				End,
			End,
		End;

	if(app)
	{
		DoMethod(win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE, MUIV_Notify_Application, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
		DoMethod(list, MUIM_List_Sort);
		SetAttrs(win,
			MUIA_Window_DefaultObject, listview,
			MUIA_Window_Open, TRUE,
			TAG_DONE);

		/* Add an extra item... */
		DoMethod(list, MUIM_List_InsertSingle, "Sneaker Pimps", MUIV_List_Insert_Sorted);

		ULONG sigs = 0;
		while(DoMethod(app, MUIM_Application_NewInput, &sigs) != MUIV_Application_ReturnID_Quit)
		{
			if(sigs)
			{
				if((sigs = Wait(sigs | SIGBREAKF_CTRL_C)) & SIGBREAKF_CTRL_C)
					break;
			}
		}
		MUI_DisposeObject(app);
	}
}
