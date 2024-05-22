/*
 *    mkfscript: create a shell script to retreive a specified
 *	    "wishlist" of files from a named ftp site.
 *
 *    history:
 *	    begun 30JUL94
 *		  31JUL94 added r, c, d and i options
 *		  01AUG94 added s option
 *		  02AUG94 added a option
 *
 *    Copyright (c)1994 by Eric R. Augustine.  Absolute freeware -  do
 *    with it what you wish except sell it or remove this copyright notice.
 *
 *    I may be contacted via internet e-mail: uairk@mcl.mcl.ucsb.edu
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stat.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAXLINE 128
#define TRUE  1
#define FALSE 0

char *progname = "mkfscript";   

typedef int bool;

struct filestr {
   const char *filename;
   char	 *fileplace;
   char	 *filedesc;
};

main(int argc, char *argv[]) {
   int	  c;
   int	  d;
   int	  cc;
   int	  delay = 1;
   int	  length;
   int	  linecount;
   bool	  reverse = FALSE;
   bool	  prout	  = FALSE;
   bool	  stdprt  = FALSE;
   bool	  fdesc	  = FALSE;
   bool	  rdstdin = FALSE;
   bool	  addlns  = FALSE;
   char	  *infile;
   char	  *outfile;
   char	  *site;
   char	  *curstring;
   char	  buffer[MAXLINE];
   char	  *NewName = "test.get";                             
   char	  *tempname;
   char	  *dstring;
   FILE	  *ifp;
   FILE	  *tfp;
   FILE	  *ofp;
   void	  usage(void);
   void	  error(char *, char *);
   struct filestr entry;
   struct stat stbuf;
   struct tm *local_time;
   size_t dlen = 0;
   time_t now, time();

   progname = argv[0];						     /* whatever program renamed to */

   if(argc < 3 ) {						     /* make sure minimal usage valid */
      usage();
      exit(1);
   }

   while(--argc > 0 && (*++argv)[0] == '-') {                        /* process command line options */
      while(d = *++argv[0]) {
	 switch(d) {
	    case 'r':                                                /* reverse file name and place */
	       reverse = TRUE;
	       break;
	    case 'i':                                                /* count output and print count */
	       prout   = TRUE;
	       break;
	    case 'c':                                                /* print to stdout */
	       stdprt  = TRUE;
	       break;
	    case 'd':                                                /* print file descriptions */
	       fdesc   = TRUE;
	       break;
	    case 's':                                                /* read from stdin */
	       rdstdin = TRUE;
	       break;
	    case 'a':                                                /* append more lines */
	       addlns  = TRUE;
	       break;
	    default:						     /* user error */
	       fprintf(stderr,"%s: unknown option '%c'\n",progname,d);
	       usage();
	       exit(2);
	       break;
	 }
      }
   }

/*  get minimal args from cli
 */
   if(rdstdin == TRUE)
      site   = argv[0];
   else {
      infile = argv[0];
      site   = argv[1];
   }

/*  catch a few possible problems
 */
   if(addlns == TRUE)
      stdprt = FALSE;

/*  get outfile if option not stdout also check for rdstdin since this alters the command line args
 */
   if((stdprt == FALSE) && (rdstdin == FALSE))
      outfile = argv[2];
   else if((stdprt == FALSE) && (rdstdin == TRUE))
      outfile = argv[1];

/*  open needed files for preprocess
 */
   if(rdstdin == TRUE)
      ifp = stdin;
   else {
      stat(infile,&stbuf);					     /* get file status */
      if((stbuf.st_mode & S_IFMT) == S_IFDIR) {			     /* ooops, file is a directory */
	 error("named infile is a directory","");
	 exit(4);						     /* goon warrior */
      }
      if(!(ifp = fopen(infile,"r"))) {                               /* source file */
	 error("unable to open file",infile);
	 exit(2);						     /* user error */
      }
   }
   tempname = tmpnam(NULL);					     /* create temp file */
   if(!(tfp = fopen(tempname,"w"))) {
      error("unable to open temporary file stream","");
      exit(3);							     /* system error */
   }

/*  preprocess source file
 */
   while((c = fgetc(ifp)) != EOF) {
      if(c == '\t') putc(' ',tfp);                                   /* remove tabs */ 
      else putc(c,tfp);						     /* tabs cause errors due to strtok() */
   }
   fclose(ifp);							     /* close source file */
   fclose(tfp);							     /* reopen temp stream */
   if(!(tfp = fopen(tempname,"r"))) {                                /* with mode "read" */
      error("unable to open temporary file stream","");
      exit(3);
   }

/*  open output file based on command line opts
 */
   if(stdprt == TRUE)						     /* print to terminal */
      ofp = stdout;
   else

/*  append new lines to end of pre-existing file
 */
      if(addlns == TRUE) {
	 if(stat(outfile,&stbuf) == -1)				     /* is old file there? */
	    addlns = FALSE;					     /* no -> reset flag */
	 if((stbuf.st_mode & S_IFMT) == S_IFDIR) {		     /* ooops, file is a directory */
	    error("named outfile is a directory","");
	    exit(5);						     /* goon error */
	 }
	 if(!(ofp = fopen(outfile,"a"))) {                           /* open file mode append */
	    error("unable to open old file",outfile);
	    exit(1);
	 }
      }
   else {
      stat(outfile,&stbuf);					     /* check filetype */
      if((stbuf.st_mode & S_IFMT) == S_IFDIR) {			     /* file is a directory */
	 error("named outfile is a directory","");
	 exit(5);						     /* goon alert */
      }
      if(!(ofp = fopen(outfile,"w"))) {                              /* print to a new file */
	 error("unable to open outfile",outfile);
	 exit(1);
      }
   }

/*  make script file
 */
   (void)time(&now);						     /* get date info for header */
   local_time = localtime(&now);
   dstring = asctime(local_time);
   if(addlns == FALSE) {
      cc = fprintf(ofp,"#!/bin/sh -f\n# generated by %s %s",progname,dstring);
      linecount = 2;
   }
   else if(addlns == TRUE) {
      cc = fprintf(ofp,"# added %s",dstring);
      linecount = 2;
   }
   while((curstring = fgets(buffer,MAXLINE,tfp)) != NULL) {	     /* parse lines */
      entry.filename  = strtok(curstring," ");
      curstring	      = strchr(curstring,' ');
      entry.fileplace = strtok(curstring," ");
      if(fdesc == TRUE) {					     /* process file description */
	 curstring	 = strchr(curstring,'\n');
	 entry.filedesc	 = strtok(curstring,"\n");
      }
      if(reverse == TRUE)					     /* swap for 'reverse' */
	 strcpy(NewName,entry.fileplace);
      else
	 strcpy(NewName,entry.filename);
      strtok(NewName,".");
      if(fdesc == TRUE) {					     /* print and size description */
	 dlen = fprintf(ofp,"\n# %s: ",NewName) + strlen(entry.filedesc) + 1;
	 fputs(entry.filedesc,ofp);
	 fputc('\n',ofp);
	 linecount = linecount + 2;
      }
      if(reverse == TRUE)
	 length = fprintf(ofp,"ncftp >& %s.ncftp -d %d -r %s/%s/%s &\n",NewName,delay,site,entry.filename,entry.fileplace);
      else
	 length = fprintf(ofp,"ncftp >& %s.ncftp -d %d -r %s/%s/%s &\n",NewName,delay,site,entry.fileplace,entry.filename);
      if(prout == TRUE) {
	 linecount = linecount + 2;
	 cc = cc + length + dlen;
      }
   }
   if((prout == TRUE) && (addlns == FALSE))			     /* print info on made file */
      fprintf(stderr,"characters: %d\n     lines: %d\n",cc,linecount);
   else
      if((addlns == TRUE) && (prout == TRUE))
	 fprintf(stderr,"characters added: %d\n  lines appended: %d\n",cc,linecount);
   if(stdprt == FALSE)						     /* don't try to close stout */
      fclose(ofp);						     /* close out file */
   fclose(tfp);
   remove(tempname);
}

void usage() {
   fprintf(stderr,"Usage: %s -[acdirs] [index-file] <site:path> [script-file]\n", progname);
   return;
}

void error(char *EString, char *FString) {
   fprintf(stderr,"%s: ERROR: %s %s\n", progname, EString, FString);
   return;
}

