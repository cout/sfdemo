#include <stdio.h>
#include <dos.h>

#ifndef PICT_IO

#define PICT_IO 1

/* These are global variable declarations that are necessary to run all
 * of the programs.  If this file is corrupted, none of the picture
 * loaders will work properly.
 */

FILE *pictfile;
int currentline=0;
unsigned char far *ScreenMemory=(unsigned char far *)0xa0000000;

/* pictopen()
 *
 * This function initializes the file pointer pictfile so that we can
 * read from the file that was entered on the command line
 */

int pictopen(char *filename) {
	if((pictfile=fopen(filename,"rb"))==NULL) {
		fprintf(stderr,"Unable to open file %s\n",filename);
		exit(1);
	}
}

/* get_byte()
 *
 *   - This declaration is a pointer to fgetc.
 */

#define get_byte() fgetc(pictfile)

/* graphic_mode and text_mode
 *
 * These functions put the computer in 256-color 320x200 pixel graphic mode,
 * or 16-color 80x25 character text mode, respectively.  For reasons of
 * simplicity, there is no error checking to ensure that the video card is
 * capable of supporting the mode.  DO NOT RUN ANY OF THESE PROGRAMS IF YOU
 * DO NOT HAVE A VGA/SVGA/XGA CARD AND MONITOR!  The results would be
 * unpredictable.
 */

void graphic_mode() {
	union REGS u;
	u.h.ah=0;
	u.h.al=0x13;
	int86(0x10,&u,&u);
}

void text_mode() {
	union REGS u;
	u.h.ah=0 ;
	u.h.al=3;
	int86(0x10,&u,&u);
}

/* int Putline(pixels, linelen)
 *
 *   - This function takes a full line of pixels (one byte per pixel) and
 * displays them (or does whatever your program wants with them...).  It
 * should return zero, or negative if an error or some other event occurs
 * which would require aborting the decode process...  Note that the length
 * passed will almost always be equal to the line length passed to the
 * decoder function, with the sole exception occurring when an ending code
 * occurs in an odd place in the GIF file...  In any case, linelen will be
 * equal to the number of pixels passed...
 */

int Putline(char pixels[], int linelen) {
	int i;
	for(i=0;i<linelen;i++) *ScreenMemory++=pixels[i];
	if(linelen<320) for(i=0;i<320-linelen;i++) ScreenMemory++;
}

#endif

/* Palette_load
 *
 * This loads the palette that will be used with the GIF file.
 */

void Palette_load() {
	int counter;
	char pnum;

	outp(0x3c8,0);                      /* Read the palette */
	for (counter=0; counter<768; counter++) {
		pnum=(char)get_byte();
		outp(0x3c9,pnum>>2);
	}
}