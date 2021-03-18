/*
 NAME: Sarah Anderson
 CLASS: ECE 4680-Embedded Computing
 LAB: 1 - Image Display
 DESCRIPTION: The purpose of this file is to display a greyscale or RGB ppm image in Windows
 
 NOTES: Make sure to add images that you want to display in the project folder in Visual Studio
 */

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <string.h>
#include <stdlib.h>

LRESULT CALLBACK EventProcessor (HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
    return(DefWindowProc(hWnd,uMsg,wParam,lParam));
}

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR     lpCmdLine,int nCmdShow){
	WNDCLASS			wc;
	HWND				WindowHandle;
	int					ROWS,COLS,BYTES;
	unsigned char		*displaydata, *imagedata;
	unsigned char		header[80];
	BITMAPINFO			*bm_info;
	FILE				*fpt;
	HDC					hDC;
	int 				i,j;

	//Opens up file for reading from commandline
	fpt = fopen(lpCmdLine, "rb");

	if(fpt == NULL){
		MessageBox(NULL, (L"Unable to open file"), (""), MB_OK | MB_APPLMODAL);
		exit(0);
	}

	fscanf(fpt, "%s %d %d %d ", header, &COLS, &ROWS, &BYTES); //!!!!!reads the last white space

	//8 bit greyscale image 
	//goes from 8 bits to 8 bits so you dont have to do any bit shifting 
	if (strcmp(header, "P5") == 0){
		displaydata = (unsigned char *)calloc(ROWS*COLS, sizeof(unsigned char));

		fread(displaydata, 1, ROWS*COLS, fpt);
        
        fclose(fpt);

        wc.style=CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc=(WNDPROC)EventProcessor;
        wc.cbClsExtra=wc.cbWndExtra=0;
        wc.hInstance=hInstance;
        wc.hIcon=NULL;
        wc.lpszMenuName=NULL;
        wc.hCursor=LoadCursor(NULL,IDC_ARROW);
        wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
        wc.lpszClassName=(L"Image Window Class");
        if (RegisterClass(&wc) == 0)
          exit(0);

        WindowHandle=CreateWindow((L"Image Window Class"),(L"ECE4680 Lab1_Windows"),
                                  WS_OVERLAPPEDWINDOW,
                                  10,10,COLS,ROWS,
                                  NULL,NULL,hInstance,NULL);
        if (WindowHandle == NULL){
              MessageBox(NULL,(L"No window"),(L"Try again"),MB_OK | MB_APPLMODAL);
              exit(0);
        }
        ShowWindow (WindowHandle, SW_SHOWNORMAL);

        bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
        hDC=GetDC(WindowHandle);
		 
        /* ... set up bmiHeader field of bm_info ... */
        bm_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bm_info->bmiHeader.biWidth = COLS;
        bm_info->bmiHeader.biHeight = -ROWS; 
		bm_info->bmiHeader.biPlanes = 1;
		bm_info->bmiHeader.biBitCount = 8; //greyscale image 
        bm_info->bmiHeader.biSizeImage = 0;
		bm_info->bmiHeader.biClrUsed = 0;
		bm_info->bmiHeader.biClrImportant = 0;
        bm_info->bmiHeader.biCompression = BI_RGB;
		bm_info->bmiHeader.biXPelsPerMeter = 0;
		bm_info->bmiHeader.biYPelsPerMeter = 0;

        //this is the color map
        for (i=0; i<256; i++){
              bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
              bm_info->bmiColors[i].rgbReserved=0;
        }
	
        SetDIBitsToDevice(hDC,0,0,COLS,ROWS,0,0,
                      0, /* first scan line */
                      COLS, /* number of scan lines */
                      displaydata,bm_info,DIB_RGB_COLORS);
        
        free(bm_info);
        free(displaydata);
	}
	else if (strcmp(header, "P6") == 0){
		//24 bit RGB image
		imagedata = (unsigned char *)calloc(ROWS*COLS*3, sizeof(unsigned char));
		displaydata = (unsigned char *)calloc(ROWS*COLS*2, sizeof(unsigned char));

		fread(imagedata, 1, ROWS*COLS*3, fpt);

		//24 to 16 bits
		j = 0;
        for (i=0, j=0; i<ROWS*COLS*3, j<ROWS*COLS*2; i+=3, j+=2){
			//little endian so that is why it is flipped and last bit is the dont care bit 
			//XRRRRRGG GGGBBBBB
			displaydata[j+1] = ((imagedata[i] & 248) >> 1) | ((imagedata[i+1] & 248) >> 6);
            		displaydata[j]	= ((imagedata[i+1] & 248) << 2) | ((imagedata[i+2] & 248) >> 3);
		}
        
        fclose(fpt);

        wc.style=CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc=(WNDPROC)EventProcessor;
        wc.cbClsExtra=wc.cbWndExtra=0;
        wc.hInstance=hInstance;
        wc.hIcon=NULL;
        wc.lpszMenuName=NULL;
        wc.hCursor=LoadCursor(NULL,IDC_ARROW);
        wc.hbrBackground=(HBRUSH)(COLOR_WINDOW+1);
        wc.lpszClassName=(L"Image Window Class");
        if (RegisterClass(&wc) == 0)
          exit(0);

        WindowHandle=CreateWindow((L"Image Window Class"),(L"ECE4680 Lab1 Windows"),
                                  WS_OVERLAPPEDWINDOW,
                                  10,10,COLS,ROWS,
                                  NULL,NULL,hInstance,NULL);
        if (WindowHandle == NULL){
              MessageBox(NULL,(L"No window"),(L"Try again"),MB_OK | MB_APPLMODAL);
              exit(0);
        }
        ShowWindow (WindowHandle, SW_SHOWNORMAL);

        bm_info=(BITMAPINFO *)calloc(1,sizeof(BITMAPINFO) + 256*sizeof(RGBQUAD));
        hDC=GetDC(WindowHandle);

        /* ... set up bmiHeader field of bm_info ... */
        bm_info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bm_info->bmiHeader.biWidth = COLS;
        bm_info->bmiHeader.biHeight = -ROWS;  
		bm_info->bmiHeader.biPlanes = 1;
		bm_info->bmiHeader.biBitCount = 16; //RGB Image
        bm_info->bmiHeader.biSizeImage = 0;
		bm_info->bmiHeader.biClrUsed = 0;
		bm_info->bmiHeader.biClrImportant = 0;
        bm_info->bmiHeader.biCompression = BI_RGB;
		bm_info->bmiHeader.biXPelsPerMeter = 0;
		bm_info->bmiHeader.biYPelsPerMeter = 0;
		
        //this is the color map
        for (i=0; i<256; i++){
              bm_info->bmiColors[i].rgbBlue=bm_info->bmiColors[i].rgbGreen=bm_info->bmiColors[i].rgbRed=i;
              bm_info->bmiColors[i].rgbReserved=0;
        }

        SetDIBitsToDevice(hDC,0,0,COLS,ROWS,0,0,
                      0, /* first scan line */
                      ROWS, /* number of scan lines */
                      displaydata,bm_info,DIB_RGB_COLORS);
        
        free(bm_info);
        free(displaydata);
	}

	ReleaseDC(WindowHandle,hDC);
	Sleep(5000); //5 secs
}
