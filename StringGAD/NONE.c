#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#ifdef _DCC
#define __inline
#endif

#include "protos.h"

struct ObjApp * CreateApp(void)
{
	struct ObjApp * Object;

	APTR	GROUP_ROOT_0;

	if (!(Object = AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
		return(NULL);

	Object->STR_label_0 = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "Teststring",
		MUIA_HelpNode, "STR_label_0",
		MUIA_String_Contents, "TestString",
		MUIA_String_Format, MUIV_String_Format_Center,
	End;

	GROUP_ROOT_0 = GroupObject,
		Child, Object->STR_label_0,
	End;

	Object->WI_label_0 = WindowObject,
		MUIA_Window_Title, "window_title",
		MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
		WindowContents, GROUP_ROOT_0,
	End;

	Object->App = ApplicationObject,
		MUIA_Application_Author, "NONE",
		MUIA_Application_Base, "NONE",
		MUIA_Application_Title, "NONE",
		MUIA_Application_Version, "$VER: NONE XX.XX (XX.XX.XX)",
		MUIA_Application_Copyright, "NOBODY",
		MUIA_Application_Description, "NONE",
		SubWindow, Object->WI_label_0,
	End;


	if (!Object->App)
	{
		FreeVec(Object);
		return(NULL);
	}

	DoMethod(Object->STR_label_0,
		MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		Object->STR_label_0,
		3,
		MUIM_Set, MUIA_String_Contents, TESTVARIABEL
		);

	DoMethod(Object->WI_label_0,
		MUIM_Window_SetCycleChain, Object->STR_label_0,
		0
		);

	set(Object->WI_label_0,
		MUIA_Window_Open, TRUE
		);


	return(Object);
}

void DisposeApp(struct ObjApp * Object)
{
	MUI_DisposeObject(Object->App);
	FreeVec(Object);
}
