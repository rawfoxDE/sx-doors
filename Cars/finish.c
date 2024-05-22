/*

----------------------------------------------------
 at end of race routine
----------------------------------------------------


*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/exec.h>
#include <proto/dos.h>

#include "sxstructs.h"
#include "sxcars.h"


void raceend(int winner, int second, int third, int carflag)
{
   
	PS("[2J");
      
   PS("[5;5H");
   eins();
   PS("\n\n\r   ");
   switch(winner)
   {
     case 1:
          Auto1();
          break;
     case 2:
          Auto2();
          break;     
     case 3:
          Auto3();
          break;     
     case 4:
          Auto4();
          break;     
     case 5:
          Auto5();
          break;
     case 6:
          Auto6();
          break;
   }            
          
          

   PS("[5;25H");
   zwei();

   PS("\n\n[D");

   switch(second)
   {
     case 1:
          Auto1();
          break;
     case 2:
          Auto2();
          break;     
     case 3:
          Auto3();
          break;     
     case 4:
          Auto4();
          break;     
     case 5:
          Auto5();
          break;
     case 6:
          Auto6();
          break;
   }  

   PS("[5;45H");
   drei();
   PS("\n\n[D");

   switch(third)
   {
     case 1:
          Auto1();
          break;
     case 2:
          Auto2();
          break;     
     case 3:
          Auto3();
          break;     
     case 4:
          Auto4();
          break;     
     case 5:
          Auto5();
          break;
     case 6:
          Auto6();
          break;
   }  

}
