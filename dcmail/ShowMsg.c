
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <proto/dos.h>
#include <dos/dos.h>
#include <exec/types.h>
#include <exec/memory.h>

void ShowMessage(char *dir, long num)
{
	char tbuf[512], file[256];
	BPTR fh;
	long l, output;

	strcpy(file, dir);
	l = strlen(file) - 1;
	if(file[l]!='/' && file[l]!=':') strcat(file, "/");
	sprintf(tbuf, "%ld", num);
	strcat(file, "msg.");
	strcat(file, tbuf);

	fh = Open(file, MODE_OLDFILE);
	if(fh)
	{
		output = Open("con:0/110/400/100/Output", MODE_NEWFILE);
		while( FGets(fh, tbuf, 512) )
		{
			Write(output, tbuf, strlen(tbuf));
		}
		Close(output);
		Close(fh);
	}
}
