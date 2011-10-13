#include <stdio.h>
#include <process.h>
#include <graphics.h>
#include <ctype.h>
#include <dos.h>
#include <string.h>

#include "c:\tc\bgi\twk256.h"

#define get_byte() fgetc(pictfile)

void mouse_init() {
	union REGS u;

	u.x.ax=0;
	int86(0x33,&u,&u);
}

void mouse_show() {
	union REGS u;

	u.x.ax=1;
	int86(0x33,&u,&u);
}

void mouse_hide() {
	union REGS u;

	u.x.ax=2;
	int86(0x33,&u,&u);
}

int mouse_x() {
	union REGS u;

	u.x.ax=3;
	int86(0x33,&u,&u);
	return u.x.cx;
}

int mouse_y() {
	union REGS u;

	u.x.ax=3;
	int86(0x33,&u,&u);
	return u.x.dx;
}

int mouse_b() {
	union REGS u;

	u.x.ax=3;
	int86(0x33,&u,&u);
	return u.x.bx;
}

int huge DetectTwk256()
{
  return 3;
};

char message[255];
int x,y,c,cline,raw;
FILE *pictfile;

void getline(FILE *file) {
	int i=0;
	char c;

	while(((c=fgetc(file))!='\n') && !feof(file)) {
		message[i]=c;
		++i;
	}
	message[i]='\0';
}

void writeout (char *s) {
	outtextxy(x,y,s);
	y+=textheight('M')+2;
}

void putline(char pixels[], int linelen) {
	int i;
	for(i=0;i<linelen;i++) {
		if(raw==0){
			i++;
			putpixel((i/2)+x,y+cline,pixels[i]);
		}else{
			putpixel(i+x,y+cline,pixels[i]);
		}

	}
}

int picture(char *filename) {
	int counter;
	register char flags;
	int xmin,xmax,ymin,ymax,width,height;
	unsigned int currentbyte=0;

	unsigned char data,reps,count;
	unsigned char buffer[768];

	if((pictfile=fopen(filename,"rb"))==NULL) return -1;

	if((get_byte())!=10) return -1;
	get_byte();
	if((get_byte())!=1) return -1;
	if((get_byte())!=8) return -1;
	xmin=get_byte()+get_byte()*256;
	ymin=get_byte()+get_byte()*256;
	xmax=get_byte()+get_byte()*256;
	ymax=get_byte()+get_byte()*256;
	for(counter=1;counter<=53;counter++) get_byte();
	if((c=get_byte())!=1) return -1;
	get_byte(); get_byte();
	if((get_byte())>10) return -1;
	for(counter=1;counter<=58;counter++) get_byte();
	width=xmax-xmin+1;
	height=ymax-ymin+1;

	cline=0;
	while(cline<height) {
		data=get_byte();
		if((data&0xc0)==0xc0) {
			reps=data&0x3f;
			data=get_byte();
			for(count=0;count<reps;count++) {
				buffer[currentbyte++]=data;
				if(currentbyte>=width) {
					putline(buffer,width);
					currentbyte=0;
					cline++;
				}
			}
		} else buffer[currentbyte++]=data;
		if(currentbyte>=width) {
			putline(buffer,width);
			currentbyte=0;
			cline++;
		}
	}
	fclose(pictfile);
}

typedef struct {
	int x1,y1;
	int x2,y2;
	char link[80];
	char ltype;
} button_type;

int cbrk_hndl() {
	return 1;
}

void main(int argc, char *argv[])
{
  button_type quartz;
  int q=0;

  int  Gd = DETECT, Gm;
  int  Drv;
  char GrErr;
  char ch;

  int EURO_FONT;
  int COMPLEX_FONT;
  int BOLD_FONT;
  int size;
  int x1,y1,j;
  int cbut=0;

  FILE *fp;

  char cmd[80];
  char sndfile[63];

  button_type buttons[20];

  ctrlbrk(cbrk_hndl);

  inittop:

  installuserdriver("Twk256",DetectTwk256);

/*  If driver is linked with file, remove comments */
/*          registerfarbgidriver(Twk256_fdriver);  */

  initgraph(&Gd,&Gm,"");

  /* Test if mode was initialized successfully */
  GrErr = graphresult();
  if (GrErr != grOk) {
      printf("Graphics error: %s\n",grapherrormsg(GrErr));
      exit(1);
  }

  mouse_init();

  top:

  mouse_hide();
  cleardevice();
  cbut=0;

  if((fp=fopen(argv[1],"rt"))==NULL) {
	closegraph();
	printf("File error: %s not loaded.",argv[1]);
	exit(1);
  }

  EURO_FONT=installuserfont("EURO.CHR");
  COMPLEX_FONT=installuserfont("LCOM.CHR");
  BOLD_FONT=installuserfont("BOLD.CHR");

  settextstyle(COMPLEX_FONT,HORIZ_DIR,USER_CHAR_SIZE);
  setusercharsize(1,2,2,3);
  raw=0;

  while(!feof(fp)) {
	fscanf(fp,"%s",cmd);
	if(!feof(fp)) {
	switch(tolower(cmd[1])) {
		case 'h':
			fscanf(fp,"%d,%d",&y,&c);
			setcolor(c);
			getline(fp);
			getline(fp);
			settextjustify(CENTER_TEXT,TOP_TEXT);
			outtextxy(188,y,message);
			settextjustify(LEFT_TEXT,TOP_TEXT);
			message[0]=' ';
			break;
		case 'w':
			fscanf(fp,"%d,%d,%d",&x,&y,&c);
			setrgbpalette(0,x,y,c);
			break;
		case 'r':raw=1;
		case 'p':
			fscanf(fp,"%s",cmd);
			fscanf(fp,"%d,%d",&x,&y);
			picture(cmd);
			break;
		case 'c':
			getline(fp);
			system(message);
			break;
		case 'q':
			fscanf(fp,"%d,%d,%d,%d",&x,&y,&x1,&y1);
			fgetc(fp);
			getline(fp);
			quartz.x1=x;
			quartz.y1=y;
			quartz.x2=x1;
			quartz.y2=y1;
			strcpy(quartz.link,message);
			q=1;
			break;
		case 'b':
			fscanf(fp,"%d,%d,%d,%c",&x,&y,&c,&buttons[cbut].ltype);
			fgetc(fp);
			getline(fp);
			setcolor(7);
			x1=x+textwidth(message)+3;
			y1=y+textheight(message)+10;
			buttons[cbut].x1=x;
			buttons[cbut].y1=y;
			buttons[cbut].x2=x1;
			buttons[cbut].y2=y1;
			setfillstyle(SOLID_FILL,7);
			bar(x,y,x1,y1);
			setcolor(8);
			rectangle(x,y,x1,y1);
			rectangle(x+1,y+1,x1-1,y1-1);
			setcolor(15);
			line(x,y,x,y1-1);
			line(x+1,y,x+1,y1-2);
			line(x,y,x1,y);
			line(x,y+1,x1-1,y+1);
			setcolor(c);
			outtextxy(x+3,y+3,message);
			getline(fp);
			strcpy(buttons[cbut].link,message);
			cbut++;
			break;
		case 't':
			fscanf(fp,"%d,%d,%d",&x,&y,&c);
			setcolor(c);
			gotoxy(x,y);
			while(message[0]!='*') {
				getline(fp);
				if(message[0]!='*') writeout(message);
				if(feof(fp)) break;
			}
			message[0]=' ';
			break;
		case 's':
			fscanf(fp,"%s",sndfile);
			strcpy(cmd,"PCPLAY ");
			strcat(cmd,sndfile);
			strcat(cmd," > NUL");
			system(cmd);
			break;
		case 'f':
			fscanf(fp,"%s",cmd);
			fscanf(fp,"%d",&size);
			switch(tolower(cmd[0])) {
				case 'b':
					settextstyle(BOLD_FONT,HORIZ_DIR,USER_CHAR_SIZE);
					break;
				case 'c':
					settextstyle(COMPLEX_FONT,HORIZ_DIR,USER_CHAR_SIZE);
					break;
				case 't':
					settextstyle(TRIPLEX_FONT,HORIZ_DIR,USER_CHAR_SIZE);
					break;
				case 'e':
					settextstyle(EURO_FONT,HORIZ_DIR,USER_CHAR_SIZE);
					break;
				case 's':
					settextstyle(SANS_SERIF_FONT,HORIZ_DIR,USER_CHAR_SIZE);
					break;
				case 'g':
					settextstyle(GOTHIC_FONT,HORIZ_DIR,USER_CHAR_SIZE);
					break;
				case 'd':
					settextstyle(DEFAULT_FONT,HORIZ_DIR,1);
					break;
			}
			setusercharsize(size,2,2*size,3);
			break;
		case 'l':
			fscanf(fp,"%d,%d,%d,%d,%d",&x,&y,&x1,&y1,&c);
			setcolor(c);
			line(x,y,x1,y1);
			break;

	}}
  }

  fclose(fp);

  mouse_init();
  mouse_show();

  ch=0;
  while(ch!=27) {
	if(kbhit()) ch=getch();
	if(mouse_b()) {
		x=mouse_x();
		y=mouse_y();
		if(q==1) { if(x>=quartz.x1) { if(x<=quartz.x2) {
			if(y>=quartz.y1) { if(y<=quartz.y2) {
				strcpy(cmd,"PCPLAY ");
				strcat(cmd,quartz.link);
				strcat(cmd," > NUL");
				system(cmd);
			}}
		}}}
		for(j=0;j<=cbut;j++) {  if(x>=buttons[j].x1) {
			if(x<=buttons[j].x2) {	if(y>=buttons[j].y1) {
			if(y<=buttons[j].y2) {
				if(j<=buttons[j].y2) {
					sound(20000);
					delay(40);
					nosound();
					x=buttons[j].x1;
					y=buttons[j].y1;
					x1=buttons[j].x2;
					y1=buttons[j].y2;
					setcolor(15);
					rectangle(x,y,x1,y1);
					rectangle(x+1,y+1,x1-1,y1-1);
					setcolor(8);
					line(x,y,x,y1-1);
					line(x+1,y,x+1,y1-2);
					line(x,y,x1,y);
					line(x,y+1,x1-1,y+1);
					delay(100);
					setcolor(8);
					sound(17000);
					delay(40);
					nosound();
					rectangle(x,y,x1,y1);
					rectangle(x+1,y+1,x1-1,y1-1);
					setcolor(15);
					line(x,y,x,y1-1);
					line(x+1,y,x+1,y1-2);
					line(x,y,x1,y);
					line(x,y+1,x1-1,y+1);
					switch(tolower(buttons[j].ltype)) {
						case 'f':
							strcpy(argv[1],buttons[j].link);
							goto top;
						case 'c':
							mouse_hide();
							closegraph();
							system(buttons[j].link);
							goto inittop;
					}
				}}
			}}
		}}
	}
  }

  closegraph();
}
