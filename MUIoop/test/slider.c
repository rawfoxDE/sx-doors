#include "/sx-config.h"

#define DemoWindow(name,id,info) WindowObject, MUIA_Window_Title, name, MUIA_Window_ID, id, WindowContents, VGroup
#define Image(nr)                ImageObject, MUIA_Image_Spec, nr, End
#define ScaledImage(nr,s,x,y)    ImageObject, MUIA_Image_Spec, nr, MUIA_FixWidth, x, MUIA_FixHeight, y, MUIA_Image_FreeHoriz, TRUE, MUIA_Image_FreeVert, TRUE, MUIA_Image_State, s, End
#define HProp                    PropObject, PropFrame, MUIA_Prop_Horiz, TRUE, MUIA_FixHeight, 8, MUIA_Prop_Entries, 111, MUIA_Prop_Visible, 10, End
#define VProp                    PropObject, PropFrame, MUIA_Prop_Horiz, FALSE, MUIA_FixWidth , 8, MUIA_Prop_Entries, 111, MUIA_Prop_Visible, 10, End


//sx-config mainmenu
struct NewMenu Menu[] =
{
	{ NM_TITLE, "Project"    , 0 ,0,0,(APTR)0            },
	{ NM_ITEM , "About..."   ,"?",0,0,(APTR)ID_ABOUT     },
	{ NM_ITEM , NM_BARLABEL  , 0 ,0,0,(APTR)0            },
	{ NM_ITEM , "Quit"       ,"Q",0,0,(APTR)MUIV_Application_ReturnID_Quit },
	{ NM_TITLE, "Configs"    , 0 ,0,0,(APTR)0            },
        { NM_ITEM , "Main Config","M",0,0,(APTR)ID_MCFG      },
        { NM_END  , NULL         , 0 ,0,0,(APTR)0            },                   
};

//My SystemX structs here...
struct MainStruct
{
	char BBSName[64];
	char BBSPath[64];
	char BBSLoc[64];
	char Sysop[64];
	char DNPath[64];
	char ULPath[64];
	long nodes;
	UWORD inac_timeout;
	UWORD inac_warn;
	ULONG minspace;
} Main;

//some globals
static char mBBSName[64],mSYSOP[64],mLOC[64],mDPATH;
static ULONG mMINHD;
long mNODES;
  
//Functions Prototypes here
void LoadMain(void);

//SX-Config window pointers
static APTR AP_Demo;
static APTR WI_Master,WI_Main,WI_MenCom;

//MainConfig window pointers
static APTR BBSobj,SYSOPobj,PATHobj,LOCobj,DLPATHobj,ULPATHobj,INAC_OUTobj,NODESobj;
static APTR INAC_WARNobj,MINHDobj;
static APTR BT_Main,BT_MenCom,BT_IntMap,BT_AxxSet,BT_NetSet,BT_LocDis,BT_DisMod;
static APTR BT_ArcTyp,BT_Protocol,BT_SerMod,BT_Doors,BT_This,BT_Quit;
static APTR BT_Save,BT_Cancel;
static APTR ST_BBS,ST_SYSOP,ST_PATH,ST_LOC,POP_UPATH,POP_DPATH,ST_INAC_OUT,ST_INAC_WARN;
static APTR SL_INAC_OUT,SL_INAC_WARN,SL_NODES,SL_MINHD;

//hook structs & functions 
void SAVEDS ASM SaveMainFunc(void)
{
                
	BPTR fh = Open("SX:Prefs/Main.DAT", MODE_NEWFILE);
	if(fh)
        {                                    
                sprintf(Main.BBSName,"%s" ,mBBSName);
		sprintf(Main.Sysop  ,"%s" ,mSYSOP);
                sprintf(Main.BBSLoc ,"%s" ,mLOC);
                sprintf(Main.DNPath ,"%s/",mDPATH);
                
                //Main.minspace = mMINHD*1000000;
                //Main.nodes = mNODES;
                
                Write(fh, &Main, sizeof(struct MainStruct));
		Close(fh);
	}
        MUI_Request(AP_Demo, WI_Master, 0, NULL, "OK", "Config was successfull saved !");
	DoMethod(WI_Main,MUIM_Set,MUIA_Window_Open,FALSE);

}


int main(int argc,char *argv[])
{
    static struct Hook SaveMainHook={ {NULL,NULL}, (void *)SaveMainFunc, NULL,NULL };

	init();         //function in sx-config.h
        LoadMain(); 
// The complete                                        
// Application Tree                                    

        //Main Config Window
        BBSobj = Label2("          BBS Name");
      SYSOPobj = Label2("        Sysop Name");
        LOCobj = Label2("      BBS Location");
       PATHobj = Label2("        Users Path");  
     DLPATHobj = KeyLabel2("     Sysop DL Path",'d');
     ULPATHobj = KeyLabel2("     Sysop UL Path",'u');
   INAC_OUTobj = Label2("Inactivity Timeout");
  INAC_WARNobj = Label2("   Inactivity Warn");
      MINHDobj = Label2("   Min MB HD Space");
      NODESobj = Label2("     Nodes visible");
   

	AP_Demo = ApplicationObject,
		MUIA_Application_Title         , "SX-Config",
		MUIA_Application_Version       , "$VER: SX-Config 1.07a (__DATE__)",
		MUIA_Application_Copyright     , "Copyright ©1998-2000 - Digital Corruption",
		MUIA_Application_Author        , "[r]FoX",
		MUIA_Application_Description   , "System-X [eSSeXX] Main Configuration.",
		MUIA_Application_Base          , "SX-CONFIG",
		MUIA_Application_Menustrip     , MUI_MakeObject(MUIO_MenustripNM,Menu,0),

		
		
		SubWindow,                                 // SX-CONFIG WINDOW
			WI_Master = WindowObject,
			MUIA_Window_Title, "SX-Config",
			MUIA_Window_ID   , MAKE_ID('M','A','I','N'),
			WindowContents, VGroup,
															                					
                                        Child, SliderObject,MUIA_Group_Horiz,TRUE, MUIA_Slider_Min, 1, MUIA_Slider_Max,32, MUIA_Slider_Level,1,End, 
                                        End,      
                                End,
                                                               
                                Child, VGroup,                                
                                        Child, BT_Quit  = SimpleButton("_Quit"),
                                        End,
                                End,        
                        End,
		End;
        
            //load Main.CFG
  
        if (!AP_Demo) fail(AP_Demo,"Failed to create application.");

        

        //Min MB HD Space Slider
       // DoMethod(SL_NODES,MUIM_Notify, MUIA_Slider_Level,MUIV_EveryTime,
       //          SL_NODES,3,MUIM_WriteLong,MUIV_TriggerValue,mNODES);

  
    
        // Closing the master window forces a complete shutdown of the application.
	DoMethod(WI_Master,MUIM_Notify,MUIA_Window_CloseRequest,TRUE,AP_Demo,2,
        MUIM_Application_ReturnID,MUIV_Application_ReturnID_Quit);


        set(SL_NODES      ,MUIA_Slider_Level,Main.nodes);

 
        // open the master window now.
       	set(WI_Master,MUIA_Window_Open,TRUE);
        //MainLoop
	{
		ULONG signal;
		BOOL running = TRUE;
		//char *buf;

		while (running)
		{
			switch (DoMethod(AP_Demo,MUIM_Application_Input,&signal))
			{
				case MUIV_Application_ReturnID_Quit:
					running = FALSE;
					break;

				case ID_ABOUT:
					MUI_Request(AP_Demo, WI_Master, 0, NULL, "OK", "SX-Config\n© 1999 by [r]FoX");
					break;
                                                               
			}
			if (running && signal) Wait(signal);
		}
	}
	fail(AP_Demo,NULL); //close all
}
//load Main.CFG
void LoadMain(void)
{
	BPTR fh = Open("SX:Prefs/Main.DAT", MODE_OLDFILE);
	if(fh)
	{
		Main.minspace = 0;
		Read(fh, &Main, sizeof(struct MainStruct));
		Close(fh);
	}

}             
