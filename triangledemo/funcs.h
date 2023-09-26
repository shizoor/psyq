
#define OT_LENGTH 1       // the ordertable length
#define PACKETMAX 18      // the maximum number of objects on the screen
#define SCREEN_WIDTH 320  // screen width
#define SCREEN_HEIGHT 240 // screen height (240 NTSC, 256 PAL)

GsOT myOT[2];                         // ordering table header
GsOT_TAG myOT_TAG[2][1 << OT_LENGTH]; // ordering table unit
PACKET GPUPacketArea[2][PACKETMAX];   // GPU packet data

u_long _ramsize = 0x00200000;   // force 2 megabytes of RAM
u_long _stacksize = 0x00004000; // force 16 kilobytes of stack

// --------
// INTEGERS
// --------
short CurrentBuffer = 0; // holds the current buffer number

// ----------
// PROTOTYPES
// ----------
void graphics(); // inits the GPU
void display();  // updates the GPU (IE: VRAM/SGRAM/framebuffer)

const DEBUG = 1; // debugging (1=on, 0=off)





char *ultoa(unsigned long value, char *str, int base)
{
char *ptr = str, *ptr1 = str, tmp_char;
unsigned long tmp_value;

if (base<2 || base > 36){
	*str='0';
	return str;
}

    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[tmp_value - value * base];
    } while (value);
	
    // Null-terminate the string
    *ptr-- = '\0';

    // Reverse the string
    while (ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }

    return str;

}

    
	
	
void graphics()
{
    SetVideoMode(1); // PAL mode
    // SetVideoMode(0); // NTSC mode

    GsInitGraph(SCREEN_WIDTH, SCREEN_HEIGHT, GsINTER | GsOFSGPU, 1, 0); // set the graphics mode resolutions (GsNONINTER for NTSC, and GsINTER for PAL)
    GsDefDispBuff(0, 0, 0, SCREEN_HEIGHT);                              // tell the GPU to draw from the top left coordinates of the framebuffer

    // init the ordertables
    myOT[0].length = OT_LENGTH;
    myOT[1].length = OT_LENGTH;
    myOT[0].org = myOT_TAG[0];
    myOT[1].org = myOT_TAG[1];

    // clear the ordertables
    GsClearOt(0, 0, &myOT[0]);
    GsClearOt(0, 0, &myOT[1]);
}

void display()
{
    // refresh the font
    FntFlush(-1);

    // get the current buffer
    CurrentBuffer = GsGetActiveBuff();

    // setup the packet workbase
    GsSetWorkBase((PACKET *)GPUPacketArea[CurrentBuffer]);

    // clear the ordering table
    GsClearOt(0, 0, &myOT[CurrentBuffer]);

    // wait for all drawing to finish
    DrawSync(0);

    // wait for v_blank interrupt
    VSync(0);

    // flip the double buffers
    GsSwapDispBuff();

    // clear the ordering table with a background color (R,G,B)
    GsSortClear(0, 0, 0, &myOT[CurrentBuffer]);

    // draw the ordering table
    GsDrawOt(&myOT[CurrentBuffer]);
}

	

