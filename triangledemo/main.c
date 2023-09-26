/*
===========================================================
                Sony PlayStation 1 Source Code
===========================================================
                         FONT EXAMPLE
Displays text on the screen using the built in GPU routines
-----------------------------------------------------------

    Developer / Programmer..............: SCEI & PSXDEV.net
    Software Ddevelopment Kit...........: PSY-Q
    Last Updated........................: 04/MAY/2017

    Original code by SCEI | Edited by PSXDEV.net

    NOTE: This example uses double buffering.

  Copyright (C) 1994,1995 by Sony Computer Entertainment Inc.
   Sony Computer Entertainment Inc. Development Department.
                   All Rights Reserved.

                    http://psxdev.net/

-----------------------------------------------------------*/
//Just rotates a triangle, eventual goal is to have a rotating 2d
// logo on screen, defined in logo.h 

#include <stdio.h>
#include <stdlib.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include <string.h>
#include <libmath.h>
#include "funcs.h"
#include "logo.h"

u_long frameCount=0;
char textMessage[40];
float lutrotMatrix[6284][2][2];

short x1, y1, x2, y2, x3, y3;

typedef struct {
    short x, y;
} Point;

void rotateTriangle(Point triangle[3], Point rotationPoint, float angleInRadians)
{
	int i;
    for (i = 0; i < 3; i++)
    {
        short x = triangle[i].x;
        short y = triangle[i].y;

        triangle[i].x = rotationPoint.x + (x - rotationPoint.x) * cos(angleInRadians) - (y - rotationPoint.y) * sin(angleInRadians);
        triangle[i].y = rotationPoint.y + (x - rotationPoint.x) * sin(angleInRadians) + (y - rotationPoint.y) * cos(angleInRadians);
    }
}

void matrixRotateTriangle(Point triangle[3], Point rotationPoint, float rotMatrix[2][2]) {
    int i;
    for (i = 0; i < 3; i++) {
        float x = triangle[i].x - rotationPoint.x;
        float y = triangle[i].y - rotationPoint.y;

        triangle[i].x = rotationPoint.x + x * rotMatrix[0][0] + y * rotMatrix[0][1];
        triangle[i].y = rotationPoint.y + x * rotMatrix[1][0] + y * rotMatrix[1][1];
    }
}

void lutMatrixRotateTriangle(Point triangle[3], Point rotationPoint, int lookup) {    //lookup = theta * 1000
    int i;
    for (i = 0; i < 3; i++) {
        float x = triangle[i].x - rotationPoint.x;
        float y = triangle[i].y - rotationPoint.y;

        triangle[i].x = rotationPoint.x + x * lutrotMatrix[lookup][0][0] + y * lutrotMatrix[lookup][0][1];
        triangle[i].y = rotationPoint.y + x * lutrotMatrix[lookup][1][0] + y * lutrotMatrix[lookup][1][1];
    }
}


POLY_F3 pf3;
// ----
// MAIN
// ----
int main()
{	
	float radangle;
	Point triangle[3];
	Point rotationPoint;
	float mradangle=0.03;
	float rotMatrix[2][2];
	int i;
	rotationPoint.x = 160.0;
	rotationPoint.y = 120.0;
	triangle[0].x=50;
	triangle[0].y=50;
	triangle[1].x=100;
	triangle[1].y=50;
	triangle[2].x=75;
	triangle[2].y=100;
	
	//Set up rotation matrix
	
	rotMatrix[0][0] = cos(mradangle);
	rotMatrix[0][1] = -sin(mradangle);
	rotMatrix[1][0] = sin(mradangle);
	rotMatrix[1][1] = cos(mradangle);
	

	
    graphics();                                   // setup the graphics (seen below)
    FntLoad(960, 256);                            // load the font from the BIOS into the framebuffer
    SetDumpFnt(FntOpen(5, 20, 320, 240, 0, 512)); // screen X,Y | max text length X,Y | autmatic background clear 0,1 | max characters
	
		//Set up rotation matrix lookup table
	
	for(i=0; i<6284; i++){							//Major speedups can be achieved here, a sin wave's second half is the first half inverted in the y axis, and the second quarter is the mirror image in the x axis, etc.
		mradangle=(float)i/1000;
		sprintf(textMessage, "%d", i);
		FntPrint(textMessage);
		lutrotMatrix[i][0][0] = cos(mradangle);
		lutrotMatrix[i][0][1] = -sin(mradangle);
		lutrotMatrix[i][1][0] = sin(mradangle);
		lutrotMatrix[i][1][1] = cos(mradangle);
		display();
	}
	

    if (DEBUG) // should debug be true (equal 1)...
    {
        // print to the TTY stream (only visible if you're using one)
        printf("\n\nHello World\n");
        printf("\nhttp://psxdev.net/");
    }

    while (1) // draw and display forever
    {
        int i;
		int intAngle;
		intAngle = frameCount % 6283;

		//DrawSolidTri(50, 50, 100, 50, 75, 100, 20, 30, 255, 0);
		/*for(i=0; i<(sizeof(logo))/4; i+=6){
			x1=(short)logo[i];
			y1=(short)logo[i+1];
			x2=(short)logo[i+2];
			y2=(short)logo[i+3];
			x3=(short)logo[i+4];
			y3=(short)logo[i+5];
			DrawSolidTri(x1, y1, x2, y2, x3, y3, 20, 30, 255, 0);
		}
		*/
		
		triangle[0].x=50;
		triangle[0].y=50;
		triangle[1].x=100;
		triangle[1].y=50;
		triangle[2].x=75;
		triangle[2].y=100;
		
		
		//matrixRotateTriangle(triangle, rotationPoint, rotMatrix);
		lutMatrixRotateTriangle(triangle, rotationPoint, intAngle);
		
		sprintf(textMessage, "Framecount : %u %d   triangle0x %d triangle0y %d triangle1x %d triangle1y %d triangle2x %d triangle2y %d", frameCount, intAngle, triangle[0].x, triangle[0].y, triangle[1].x, triangle[1].y, triangle[2].x, triangle[2].y);
		FntPrint(textMessage);
		/*
		radangle=(double)intAngle/10;
		
		for (i = 0; i < 3; i++) {
			short x = triangle[i].x;
			short y = triangle[i].y;
			triangle[i].x = (short)(rotationPoint.x + (x - rotationPoint.x) * (float)cos((double)radangle) - (y - (float)rotationPoint.y) * (float)sin((double)radangle));
			triangle[i].y = (short)(rotationPoint.y + (x - rotationPoint.x) * (float)sin((double)radangle) + (y - (float)rotationPoint.y) * (float)cos((double)radangle));
		}
		
		//rotateTriangle(triangle, rotationPoint, radangle);
		*/
		DrawSolidTri((short)triangle[0].x, (short)triangle[0].y, (short)triangle[1].x, (short)triangle[1].y, (short)triangle[2].x, (short)triangle[2].y, 20, 30, 255, 0); //Needs shorts or blank screen etc.
		
		
		frameCount++;
		display();
    }

    return 0; // this will never be reached because we're in a while loop above
}



void DrawSolidTri(short x1, short y1, short x2, short y2, short x3, short y3, int r, int g, int b, int ot)
{
    // draw a solid triangle and fill it with rgb colour
    // EG: DrawSolidTri(200,0, 170,30, 230,30, 255,0,255, 0);
    // check the Psy-Q documentation for the coordinates!
 
    SetPolyF3(&pf3);                            // initialise the primitive
    setRGB0(&pf3, r, g, b);                     // set the fill colour (RGB)
    setXY3(&pf3, x1,y1, x2,y2, x3,y3);          // draw 3 lines (3 sets of x/y co-ords)
    DrawPrim(&pf3);                             // copy shape to frame buffer
    GsSortPoly(&pf3, &myOT[CurrentBuffer], ot); // put the polygon into the order table
}
 
