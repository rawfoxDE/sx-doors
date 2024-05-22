#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/dos.h>


void main(int argc, char *argv[])
{
     struct PTable
     {
          char rname[64];
     };
     struct PTable PT;   
           
     char buf[256], buff[64],e[1];
     int i=0;
     BPTR fh=Open("aa-dev:sx_filebase/fb.cfg",MODE_OLDFILE);       

eing:         
              
     printf(" a - table schreiben\n\r"
            " b - table lesen\n\r"
            " q - quit\n\r");   
     scanf("%c",e);      
     switch(e[0])
     {       
          case 'a':
          {
               if(!fh) 
               {          
                    fh=Open("aa-dev:sx_filebase/fb.cfg",MODE_NEWFILE);
                    printf("\n\rneues configfile wurde erzeugt.\n\r");
                    
                    break;
          
               } else
               {      
    neu:    
                    i++;
                    sprintf(buf,"\n\rBitte %ld. Pfad eingeben : ",i);
                    printf(buf);
                    scanf("%s",buff);
                    if(buff[0]=='q') break;
                    strcpy(PT.rname,buff);
                    Seek(fh,0,OFFSET_END);
                    Write(fh, &PT, sizeof(struct PTable));
                    Close(fh);
                    goto neu;
               }
          } 
          case 'b':
          {
               if(!fh) 
               {
                    printf("\n\rtable not availlable!\n\r");
                    
               } else
               {     
                    printf("\n\rThe Downloadtable:\n\r");
                    while(Read(fh, &PT, sizeof(struct PTable)))
                    {
                         sprintf(buf,"%s\n\r",PT.rname);
                         printf(buf);
                    }                     
                    Close(fh);
               }         
               //break;
          }
          case 'q': exit(0);
          break;
     }
     printf("\n\rgoto eingabe now\n\r");
     goto eing;       
}     
