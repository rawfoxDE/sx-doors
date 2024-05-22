/* *
 * Compilation notes:
 * Just use the supplied "makefile.manx" for Aztec C 3.4(b) or later. 
 * Lattice C 4.0 users just execute "lmake" or use "makefile.lattice".
 * The whole compilation and link procedure (including timer.c) from RAM
 * took me 50 seconds with Lattice 4.0 and 67 seconds with Aztec 3.4b.
 * Please note that you need to link timer.o for the TimeDelay() function
 * if you don't use make or execute lmake.
 *
 * Well, the Lattice folks claim that LATTICE get's defined by their 
 * compiler, but it didn't work for me. So, if you don't use make or lmake, 
 * or define LATTICE, you'll get blasted by (my) Lattice if ya don't remove 
 * all the lines marked with "LK" below...
 *						<CB>
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <proto/all.h>
#include <time.h>


#define TRUE 1
#define FALSE 0

#define CRONTAB "S:CronTab"	/* Our default path */

#define MAXLINE	132
#define SIZE	64

#define TITLE "SXCron v1.0 by [r]FoX \n"
#define WHERE "http: amichurch.home.ml.org\n"
 
int	eof;
int	sleepy;
char	min[SIZE], hour[SIZE], day[SIZE],
	month[SIZE], wday[SIZE], command[SIZE];
char	*tokv[] = { min, hour, day, month, wday };
char	crontabs[200];  /* You might define quite a path! <CB> */
FILE	*fd;
 
 
/*
 *	This is the basics, ready for bells and whistles
 *
 */


void main(int argc, char *argv[])
{
	void showhelp(void);
	void wakeup(void);

	/* Below we use the same functions & variables as in wakeup()
	   just to get the current second  <CB>*/

	register struct tm *tm;
	struct tm *localtime();
	long cur_time;
 	
	/* Tell the guy out there what he might do with this deamon
	 * and who made it. <CB>*/

	printf(TITLE);
	printf(WHERE);
	printf("Usage: %s [Path & Filename]\n",argv[0]);
	printf("       or '%s ?' for help\n",argv[0]);

	/* Now let's see if the user told us to look someplace else
	 * for our input file or if he want's some more usage hints <CB> */
  
	if (argc == 2) 
     {
		if (argv[1][0] == '?') 
          { 
		 	showhelp();
		     exit(10);
		}
		else strcpy(crontabs, argv[1]);
		
	}
	else strcpy(crontabs, CRONTAB);	
	/* If there isn't a user path, we'll use the default <CB>*/

	/*Now tell the user with what path & filename we wound up*/
	printf("CronTab path = %s \n",crontabs);

	for (;;)  
     {
		wakeup();

		time(&cur_time);		/* get the current time */
		tm = localtime(&cur_time);	/* break it down */

		// Now we calculate the amount of seconds
		// till the next minute <CB>*/

		sleepy = (60-tm->tm_sec);	
		// PLEASE don't decrease the 60 above, believe me,
		// I know what I'm doing there! <CB> */ 

		TimeDelay(sleepy,0,0);	
		/* sleep 'till next minute */
	}
}

void wakeup(void)
{
	register struct tm *tm;
	struct tm *localtime();
	long cur_time;
	
	char doit[80];
 	
	time(&cur_time);		/* get the current time */
	tm = localtime(&cur_time);	/* break it down */
 
	/* Now let's see if there is a CronTab file out there <CB> */ 	

	if ((fd = fopen(crontabs, "r")) == NULL) {
	fprintf(stderr, "Can't open %s\nTry again\n", crontabs);
	exit(1);
	}
     
	eof = FALSE;
 
	while (!eof)  {
		if (getline() && match(min,tm->tm_min) &&
		   match(hour,tm->tm_hour) && match(day,tm->tm_mday) &&
		   match(month,tm->tm_mon+1) && match(wday,tm->tm_wday))  {
		/* Weird localtime months ^ range from 0-11 !!! <CB>*/
			strcpy(doit,"RUN ");
			strcat(doit,command);
			Execute(doit,NULL,NULL);
			}
		}
	fclose(fd);
}
 
 
/*
 *	A line consists of six fields.  The first five are:
 *
 *		minute:         0-59
 *		hour:           0-23
 *		day:            1-31
 *		month:          1-12
 *		weekday:        0-6 (Sunday = 0)
 *
 *	The fields are seperated by spaces or tabs, with the
 *	first field left justified (no leading spaces or tabs).
 *	See below for optional field syntax.
 *
 *	The last field is the command field.  This command will
 *	be executed by the CLI just as if typed from a console.
 */
 
int getline(void)
{
	register char *p;
	register int   i;
	char    buffer[MAXLINE], *scanner(), *fgets();
 
	if (fgets(buffer, sizeof buffer, fd) == NULL)  {
		eof = TRUE;
		return(FALSE);
		}
 
	for (p = buffer, i = 0; i < 5; i++)  {
		if ((p = scanner(tokv[i], p)) == (char *)NULL)
			return(FALSE);
		}
 
	strcpy(command, p);     /* scoop the command */
	return(TRUE);
}
 
 
char *scanner(token, offset)
register char   *token;		/* target buffer to receive scanned token */
register char   *offset;	/* place holder into source buffer */
{
	while ((*offset != ' ') && (*offset != '\t') && *offset)
		*token++ = *offset++;
 
	/*
	 *      Check for possible error condition
	 */
         
	if (!*offset)
		return ((char *)NULL);
 
	*token = '\0';
        
	while ((*offset == ' ') || (*offset == '\t'))
		offset++;
 
	return (offset);
}
 
 
/*
 *	This routine will match the left string with the right number.
 *
 *	The string can contain the following syntax:
 *
 *	*		This will return TRUE for any number
 *	x,y [,z, ...]	This will return TRUE for any number given.
 *	x-y		This will return TRUE for any number within
 *			the range of x thru y.
 */
 
int match(left, right)
register char   *left;
register int    right;
{
	register int	n;
	register char	c;
 
	n = 0;

	if (!strcmp(left, "*"))
		return(TRUE);
 
	while ((c = *left++) && (c >= '0') && (c <= '9'))
		n  =  (n * 10) + c - '0';
 
	switch (c)  {
		case '\0':
			return (right == n);
 
		case ',':
			if (right == n)
				return(TRUE);
			do {
				n = 0;
				while ((c = *left++) && (c >= '0') && (c <= '9'))
					n = (n * 10) + c - '0';
 				if (right == n)
					return(TRUE);
				} while (c == ',');
			return(FALSE);
 
		case '-':
			if (right < n)
				return(FALSE);
 
			n = 0;
			while ((c = *left++) && (c >= '0') && (c <= '9'))
				n = (n * 10) + c - '0';
 
			return(right <= n);
		}
}

void showhelp()		/* by <CB> */

{ 
	printf("\nWell, you really should read the .doc file,\n");
	printf("but here are some more hints:\n");
	printf("You might specify only ONE command line parameter,\n");
	printf("the path were AmiCron should look for it's command table,\n");
	printf("including the filename of the command table.\n");
	printf("If you don't supply a path, AmiCron will use it's default\n");
	printf("path (S:CronTab).\n");
}
