#ifndef MUI_MAIN_H
#define MUI_MAIN_H

#define DOUBLESTART             1
#define REALLYQUIT              1
   
extern struct Library *MUIMasterBase;
extern struct ObjApp * CreateApp(void);
extern void DisposeApp(struct ObjApp *);
extern char TESTVARIABEL;

struct App
{
   Object *app;
}

struct App           *createApplication(void);
Object               *createAppObj(     struct App *);
void                  createNotifys(    struct App *);
ULONG __saveds __asm  ds_Hook_Func(     register __a0 struct Hook *,
                                        register __a2 APTR,
                                        register __a1 APTR);

void privateNotifies(struct App *);
void handleComArgs(int, char *[]);

struct ObjApp
{
	APTR	App;
	APTR	WI_label_0;
	APTR	STR_label_0;
};


#endif
