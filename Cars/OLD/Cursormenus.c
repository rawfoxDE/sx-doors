#include "Cursormenus.h"
void MainMenu(void)
{
        int i=1;
 menu:       
        XIMFunction(501,0,0);
        XIMFunction(6,0,0);
        switch(themsg.String[0])
        {
                case '5':       // crs up
                {
                        i--;
                        if(i==0) i++;
                }
                case '4':       // crs dwn
                {
                        i++;
                        if(i==6) i--;        
                }
         }
         switch(i)
         {
                case '1':
                {
                        PS("[9;1h");
                        PS("[0;33;44m(D) - Docs       ");
                        XIMFunction(6,0,0);
                        switch(themsg.String[0])
                        {
                                case '\r':
                                {
                                        PS("\033[2J");
                                        XIMFunction(8,0,"hd1:sx/developer/cars/car_doc.ans");
                                        XIMFunction(6,0,"\n\rReturn to go back...: ");
                                        break;
                                }
                         }       
                 }
                 case '2':                 
                 {
                        PS("[11;1h");
                        PS("[0;33;44m(C) - Cars      ");
                        XIMFunction(6,0,0);
                        switch(themsg.String[0]
                        {
                                case 'c':
                                case 'C':
                                {
                                        Selection(carflag);
                                        goto menu;  
                                }
                        }       
                 }
                 case '3':
                 {
                        PS("[13;1h");
                        PS("[0;33;44m(R) - Race      ");
                        XIMFunction(6,0,0);
                        switch(themsg.String[0])
                        {
                                case '\r':
                                case 'r':
                                case 'R':
                                {
                                        Race(carflag);
                                        goto menu;
                                }
                        }        
                 }      
                 case '4':
                 {
                        PS(("15;1h");
                        PS("[0;33;44m(T) - Topdriver ");  
                        XIMFunction(6,0,0);
                        switch(themsg.String[0])
                        {
                                case '\r':
                                case 't':
                                case 'T':
                                {
                                        topten();
                                        goto menu;
                                }
                        } 
                 }
                 case '5':
                 {
                        return;
                 }                     
         }
         return;
}