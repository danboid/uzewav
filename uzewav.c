//          uzewav
// A wav player for the Uzebox

//     By Dan MacDonald
//           2024

// SD init and wav playback code borrowed from Uzetherm by Hartmut Wendt

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdbool.h>
#include <stdlib.h>
#include <uzebox.h>
#include <bootlib.h>

#include "data/tileset.inc"


#define BYTES_PER_SECTOR 512 	//fixed for regular SD
//#define MAX_WAVES	32

const char *numbers[10] = {n0, n1, n2, n3, n4, n5, n6, n7, n8, n9};

int frames, seconds, minutes, hours, hundreds, btnprev, btnheld = 0;

bool active = false;

bool cardDetected;

char sector_buf[512];
sdc_struct_t sd_struct;

const char boing[] PROGMEM ={
    0,PC_WAVE,8,
    0,PC_ENV_VOL,0xE0,
    0,PC_ENV_SPEED,-20,
    2,PC_NOTE_DOWN,6,
    2,PC_NOTE_DOWN,6,
    2,PC_NOTE_CUT,0,
    0,PATCH_END
};

const struct PatchStruct patches[] PROGMEM = {
    {0, NULL, boing, 0, 0},
};

// DrawDigits is a function to easily draw a double digit number between 00 and 99 using the large numbers.
// xoffset represents the first column to use for the first (10s) number of the pair.

void DrawDigits(int number, int xoffset);

void DrawDigits(int number, int xoffset) {
    // First we draw the units (rightmost) digit. It gets drawn at xoffset + 5 tiles
    DrawMap2((xoffset + 5), 8, (numbers[number % 10]));
    // Draw the tens (leftmost) digit or a zero
    DrawMap2(xoffset, 8, (numbers[number / 10 % 10]));
}

void DrawColon(int xoffset);

void DrawColon(int xoffset) {
    SetTile(xoffset,9,14);
    SetTile(xoffset,11,14);
}

int main()
{
    SetSpritesTileTable(tileset);
    InitMusicPlayer(patches);
    SetTileTable(tileset);
    ClearVram();
    while (1)
    {
        WaitVsync(1); // This is key to keeping accurate time.

        Print(12,2,PSTR("UZEWAV"));
        Print(7,4,PSTR("BY DAN MACDONALD"));

        Print(1,8,PSTR("INITIALIZING SD..."));
        cardDetected=false;

        sd_struct.bufp = &(sector_buf[0]);
        if(FS_Init(&sd_struct)){//error
            for(uint8_t i=0; i<7; i++){
                Print(1,11,(i&1)?PSTR("                  "):PSTR("INITIALIZATION FAILED"));
                WaitVsync(20);
            }
            cardDetected = false;

        }else{
            if(sd_struct.flags & SDC_FLAGS_SDHC)
                Print(19,8,PSTR("SDHC OK"));
            else
                Print(19,8,PSTR("SDSC OK"));
            Print(1,11,PSTR("SEARCHING FOR UZETHERM.DAT"));

            uint32_t t32 = FS_Find(&sd_struct,
                                   ((u16)('U') << 8) |
                                   ((u16)('Z')     ),
                                   ((u16)('E') << 8) |
                                   ((u16)('T')     ),
                                   ((u16)('H') << 8) |
                                   ((u16)('E')     ),
                                   ((u16)('R') << 8) |
                                   ((u16)('M')     ),
                                   ((u16)('D') << 8) |
                                   ((u16)('A')     ),
                                   ((u16)('T') << 8) |
                                   ((u16)(0)       ));
            if(t32 == 0U)
                cardDetected = false;

            else
                cardDetected = true;
        }
    }
}
