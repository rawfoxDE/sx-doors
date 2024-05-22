#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#ifdef _DCC
#define __inline
#endif

#include "protos.h"

struct AppObj * CreateApp(void)
{
	struct App * Object;

	APTR	GROUP_ROOT_0, GR_grp_1;

	if (!(Object = AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
		return(NULL);

	Object->STR_label_0C = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "Nick Name",
		MUIA_HelpNode, "STR_label_0C",
		MUIA_String_Contents, "DCIRC",
		MUIA_String_Format, MUIV_String_Format_Center,
	End;

	Object->STR_label_1C = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "Server",
		MUIA_HelpNode, "STR_label_1C",
		MUIA_String_Contents, "jp.elitenet.org",
		MUIA_String_Format, MUIV_String_Format_Center,
	End;

	Object->STR_label_2C = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "Port",
		MUIA_HelpNode, "STR_label_2C",
		MUIA_String_Contents, "6667",
		MUIA_String_Format, MUIV_String_Format_Center,
	End;

	Object->STR_label_3C = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "Channel",
		MUIA_HelpNode, "STR_label_3C",
		MUIA_String_Contents, "#elitecafe",
		MUIA_String_Format, MUIV_String_Format_Center,
	End;

	Object->STR_label_4C = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "RealName",
		MUIA_HelpNode, "STR_label_4C",
		MUIA_String_Contents, "DigitalCorruption",
		MUIA_String_Format, MUIV_String_Format_Center,
	End;

	GR_grp_1 = GroupObject,
		MUIA_HelpNode, "GR_grp_1",
		MUIA_Background, MUII_SHADOWSHINE,
		MUIA_Frame, MUIV_Frame_Button,
		MUIA_Group_Rows, 5,
		Child, Object->STR_label_0C,
		Child, Object->STR_label_1C,
		Child, Object->STR_label_2C,
		Child, Object->STR_label_3C,
		Child, Object->STR_label_4C,
	End;

	GROUP_ROOT_0 = GroupObject,
		MUIA_Background, MUII_SHADOW,
		MUIA_Group_HorizSpacing, 2,
		MUIA_Group_VertSpacing, 2,
		Child, GR_grp_1,
	End;

	Object->WI_label_0 = WindowObject,
		MUIA_Window_Title, "DCIRC Setup",
		MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
		MUIA_Window_NoMenus, TRUE,
		WindowContents, GROUP_ROOT_0,
	End;

	Object->App = ApplicationObject,
		MUIA_Application_Author, "[r]FoX",
		MUIA_Application_Base, "DCIRC",
		MUIA_Application_Title, "DCIRC",
		MUIA_Application_Version, "$VER: DCIRC v0.3",
		MUIA_Application_Copyright, "DigitalCorruption",
		MUIA_Application_Description, "Amiga IRC Client",
		MUIA_HelpFile, "dcirc.guide",
		SubWindow, Object->WI_label_0,
	End;


	if (!Object->App)
	{
		FreeVec(Object);
		return(NULL);
	}

	DoMethod(Object->STR_label_0C,
		MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		Object->STR_label_0C,
		3,
		MUIM_Set, MUIA_String_Contents, NICK
		);

	DoMethod(Object->STR_label_1C,
		MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		Object->STR_label_1C,
		3,
		MUIM_Set, MUIA_String_Contents, SERVER
		);

	DoMethod(Object->STR_label_2C,
		MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		Object->STR_label_2C,
		3,
		MUIM_Set, MUIA_String_Contents, PORT
		);

	DoMethod(Object->STR_label_3C,
		MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		Object->STR_label_3C,
		3,
		MUIM_Set, MUIA_String_Contents, CHANNEL
		);

	DoMethod(Object->STR_label_4C,
		MUIM_Notify, MUIA_String_Contents, MUIV_EveryTime,
		Object->STR_label_4C,
		3,
		MUIM_Set, MUIA_String_Contents, REALNAME
		);

	DoMethod(Object->WI_label_0,
		MUIM_Window_SetCycleChain, Object->STR_label_0C,
		Object->STR_label_1C,
		Object->STR_label_2C,
		Object->STR_label_3C,
		Object->STR_label_4C,
		0
		);

	set(Object->WI_label_0,
		MUIA_Window_Open, TRUE
		);


	return(Object);
}

void DisposeApp(struct App * Object)
{
	MUI_DisposeObject(Object->App);
	FreeVec(Object);
}
