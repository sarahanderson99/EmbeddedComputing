/*
NAME: Sarah Anderson
CLASS: ECE 4680-Embedded Computing
LAB: 1 - Image Display
DESCRIPTION: The purpose of this file is to display a greyscale or RGB ppm image in Windows
*/

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

Display			*Monitor;
Window			ImageWindow;
GC				ImageGC;
XWindowAttributes		Atts;
XImage			*Picture;
int				ROWS, COLS, BYTES, i;
unsigned char		*displaydata, *filedata;
char header[80];
FILE		*fpt;
int 	a = 0;

/* ... */
if (argc != 2){
  printf("Usage: ./display [image name]\n");
  exit(0);
  }

/* open image for reading */
fpt=fopen(argv[1],"r");
if (fpt == NULL)
  {
  printf("Unable to open %s for reading\n",argv[1]);
  exit(0);
  }

/* read image header (greyscale or RGB) */
i=fscanf(fpt,"%s %d %d %d ",header,&COLS,&ROWS,&BYTES); //need extra space at the end of fscanf to get rid of the extra bit 

//if a greyscale image 
if (strcmp(header, "P5") == 0){
	filedata=(unsigned char *)calloc(ROWS*COLS,sizeof(unsigned char)); //8 bits
	displaydata=(unsigned char *)calloc(ROWS*COLS*2,sizeof(unsigned char)); //16 bits
	
	fread(filedata,1,ROWS*COLS,fpt);
	fclose(fpt);

	//convert from 8 bits to 16 bits
	//since this is greyscale image, all "colors" will be the same bit value
	for (a = 0; a < ROWS*COLS; a++){
		displaydata[a*2+1] = (filedata[a] & 0b11111000) | ((filedata[a] & 0b11100000) >> 5); //5 bits red and 3 bits green
		displaydata[a*2+0] = ((filedata[a] & 0b00011100) << 3) | ((filedata[a] & 0b11111000) >> 3); //3 bits green and 5 bits blue 
		//printf("%d %d \n", displaydata[a*2+0], displaydata[a*2+1]);	
	}
}

//if a RGB image 
else if(strcmp(header, "P6") == 0){
	filedata=(unsigned char *)calloc(ROWS*COLS*3,sizeof(unsigned char)); //24 bits
	displaydata=(unsigned char *)calloc(ROWS*COLS*2,sizeof(unsigned char)); //16 bits
	
	fread(filedata,1,ROWS*COLS*3,fpt);
	fclose(fpt);
	
	//convert from 24 bit image to 16 bit image
	//different bit value for each color because this is RGB
	for(a = 0; a < ROWS*COLS; a++){
		displaydata[a*2+1] = (filedata[a*3+0] & 0b11111000) | ((filedata[a*3+1] & 0b11100000) >> 5); //5 bits red and 3 bits green
		displaydata[a*2+0] = ((filedata[a*3+1] & 0b00011100) << 3) | ((filedata[a*3+2] & 0b11111000) >> 3); //3 bits green and 5 bits blue 
		//printf("%d %d \n", displaydata[a*2+0], displaydata[a*2+1]);
	}
}

//Displays image 
Monitor=XOpenDisplay(NULL);
if (Monitor == NULL)
  {
  printf("Unable to open graphics display\n");
  exit(0);
  }

ImageWindow=XCreateSimpleWindow(Monitor,RootWindow(Monitor,0),
	50,10,		/* x,y on screen */
	COLS,ROWS,	/* width, height */
	2, 		/* border width */
	BlackPixel(Monitor,0),
	WhitePixel(Monitor,0));

ImageGC=XCreateGC(Monitor,ImageWindow,0,NULL);

XMapWindow(Monitor,ImageWindow);
XFlush(Monitor);
while(1)
  {
  XGetWindowAttributes(Monitor,ImageWindow,&Atts);
  if (Atts.map_state == IsViewable /* 2 */)
    break;
  }

Picture=XCreateImage(Monitor,DefaultVisual(Monitor,0),
		DefaultDepth(Monitor,0),
		ZPixmap,	/* format */
		0,		/* offset */
		displaydata,/* the data */
		COLS,ROWS,	/* size of the image data */
		16,		/* pixel quantum (8, 16 or 32) */
		0);		/* bytes per line (0 causes compute) */

XPutImage(Monitor,ImageWindow,ImageGC,Picture,
		0,0,0,0,	/* src x,y and dest x,y offsets */
		COLS,ROWS);	/* size of the image data */

XFlush(Monitor);
//waits 5 secs then closes
sleep(5);
XCloseDisplay(Monitor);
}

