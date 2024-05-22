#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <libraries/dos.h>
#include <exec/memory.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <fcntl.h>
#include "sxstructs.h"
#include "filebase.h"


extern struct JHMessage themsg;
extern void PS(char *str);
extern void XIMFunction(int func, long data, char * str);

void tree(char *name)  // declare the root: -dir
{
              
        struct FileInfoBlock *fib = AllocMem(sizeof(struct FileInfoBlock), MEMF_PUBLIC | MEMF_CLEAR);
        
        BPTR lock;
        char buf[256], oldname[256];
        int i=0;
        BOOL ok;
        
        // sprintf(name,"ami-bin:");
        //if(name[0]='q') return;        
        if(!fib) goto xit;    
        if(!(lock=Lock(name, SHARED_LOCK))) { PS("..can`t Lock anything !"); goto xit; }
        
        ok = Examine(lock, fib);
        if(!ok)  // if no root: was found
        {
           goto xit;
        }
        else
        {      // if root: is examinable so print the name           
               sprintf(buf,"\n\rCurrent Path: [1;31m%s[0m \n\r\n\r",fib->fib_FileName);
               PS(buf);

               while(ExNext(lock, fib))  // subdirectories im rootdir ?
               {               
                         if(fib->fib_DirEntryType > -1) // wenn ja, dann namen merkn
                         {                              // und neuen namen an sub(name) weitergeben
                              strcpy(oldname,name);
                              sprintf(name,"%s%s/", oldname, fib->fib_FileName);
                              PS("\n\r");
                              PS(name);
                              //sub(name); // hier wirds schnell !!!!!!!!!
                              strcpy(name,oldname);      // go parent
                              i++;  // DeviceCounter
                         } else
                         {    
                              // wenn kein dir/device dann is ein File                              
                              //sprintf(buf,"[36m%24s[0m FILE\n\r", fib->fib_FileName);
                              //PS(buf);
                         }
               } 
               
               UnLock(lock);
        xit:
               sprintf(buf,"\n\r Dirs Total: %ld \n\r",i);
               PS(buf);
               PS("\r\n...exiting now ....\r\n");
               FreeMem(fib,sizeof(struct FileInfoBlock));
               return;
        }
}
void sub(char *name)
{
        struct FileInfoBlock *fib = AllocMem(sizeof(struct FileInfoBlock), MEMF_PUBLIC | MEMF_CLEAR);
        
        BPTR lock;
        char buf[256], oldname[256];
        
        BOOL ok;
                  
        if(!fib) return;    
        if(!(lock=Lock(name, SHARED_LOCK))) { PS("..can`t Lock SubDirs !\r"); return; }
        
        ok = Examine(lock, fib);
        if(!ok) { FreeMem(fib,sizeof(struct FileInfoBlock)); return; }
        else
        {       
               // ok=erstes Subdir gefunden, namen ausgeben           
               sprintf(buf,"[34m - [1;37m%s\r\n",fib->fib_FileName);
               PS(buf);

               while(ExNext(lock, fib))
               {             
                         // hat subdir weitere subdir`s ?                  
                         if(fib->fib_DirEntryType > -1)  
                         {
                              strcpy(oldname,name); // neuen namen vorbereiten und an files(name)
                              sprintf(name,"%s/%s", oldname, fib->fib_FileName); // weitergeben   
                              //files(name);
                              strcpy(name,oldname); // files() hat ende, wieder weiter...
                              
                         } else  
                         {
                              if(fib->fib_DirEntryType==NULL) PS("  [35mNo New Files here !\n\r");
                              strcpy(name,oldname);  
                              //sprintf(buf,"[34m%s  ", fib->fib_FileName);
                              //PS(buf);                              
                                                                       
                         }
               } 

               UnLock(lock);
       
        }  
        FreeMem(fib,sizeof(struct FileInfoBlock));
        return;

}






