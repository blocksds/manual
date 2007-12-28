#include <nds.h>
#include "sprites.h"

/* Backgrounds */
#include "starField.h"
#include "planet.h"
#include "splash.h"
/* Sprites */
#include "orangeShuttle.h"
#include "moon.h"

void initVideo() {
    /*
     *  Map VRAM to display a background on the main and sub screens.
     * 
     *  The vramSetMainBanks function takes four arguments, one for each of the
     *  major VRAM banks. We can use it as shorthand for assigning values to
     *  each of the VRAM bank's control registers.
     *
     *  We map banks A and B to main screen  background memory. This gives us
     *  256KB, which is a healthy amount for 16-bit graphics.
     *
     *  We map bank C to sub screen background memory.
     *
     *  We map bank D to LCD. This setting is generally used for when we aren't
     *  using a particular bank.
     *
     *  We map bank E to main screen sprite memory (aka object memory).
     */
    vramSetMainBanks(VRAM_A_MAIN_BG_0x06000000,
                     VRAM_B_MAIN_BG_0x06020000,
                     VRAM_C_SUB_BG_0x06200000,
                     VRAM_D_LCD);

    vramSetBankE(VRAM_E_MAIN_SPRITE);

    /*  Set the video mode on the main screen. */
    videoSetMode(MODE_5_2D | // Set the graphics mode to Mode 5
                 DISPLAY_BG2_ACTIVE | // Enable BG2 for display
                 DISPLAY_BG3_ACTIVE | // Enable BG3 for display
                 DISPLAY_SPR_ACTIVE | // Enable sprites for display
                 DISPLAY_SPR_1D       // Enable 1D tiled sprites
                 );

    /*  Set the video mode on the sub screen. */
    videoSetModeSub(MODE_5_2D | // Set the graphics mode to Mode 5
                    DISPLAY_BG3_ACTIVE); // Enable BG3 for display
}

void initBackgrounds() {
    /*  Set up affine background 3 on main as a 16-bit color background */
    BG3_CR = BG_BMP16_256x256 |
             BG_BMP_BASE(0) | // The starting place in memory
             BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    BG3_XDX = 1 << 8;
    BG3_XDY = 0;
    BG3_YDX = 0;
    BG3_YDY = 1 << 8;

    /*  Place main screen background 3 at the origin (upper left of the screen)
     */
    BG3_CX = 0;
    BG3_CY = 0;

    /*  Set up affine background 2 on main as a 16-bit color background */
    BG2_CR = BG_BMP16_128x128 |
             BG_BMP_BASE(8) | // The starting place in memory
             BG_PRIORITY(2);  // A higher priority

    /*  Set the affine transformation matrix for the main screen background 3
     *  to be the identity matrix.
     */
    BG2_XDX = 1 << 8;
    BG2_XDY = 0;
    BG2_YDX = 0;
    BG2_YDY = 1 << 8;

    /*  Place main screen background 2 an interesting place
     */
    BG2_CX = -(SCREEN_WIDTH / 2 - 32) << 8;
    BG2_CY = -32 << 8;

    /*  Set up affine background 3 on sub as a 16-bit color background */
    SUB_BG3_CR = BG_BMP16_256x256 |
                 BG_BMP_BASE(0) | // The starting place in memory
                 BG_PRIORITY(3); // A low priority

    /*  Set the affine transformation matrix for the sub screen background 3
     *  to be the identity matrix.
     */
    SUB_BG3_XDX = 1 << 8;
    SUB_BG3_XDY = 0;
    SUB_BG3_YDX = 0;
    SUB_BG3_YDY = 1 << 8;

    /*
     *  Place main screen background 3 at the origin (upper left of the screen)
     */
    SUB_BG3_CX = 0;
    SUB_BG3_CY = 0;
}

typedef struct {
    int id;
    int width;
    int height;
    int x;
    int y;
    int angle;
    tObjPriority priority;
    int tileId;
    int palId;
} SpriteInfo;

void displaySprites(SpriteEntry * spriteEntry, SpriteRotation * spriteRotation) {
    //XXX check the nds_examples to make sure this is a good way to do things (it isn't currently)
    int nextAvailableTileIdx = 0;
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16; //how many colors there are in a palette
    static const int BOUNDARY_VALUE = 16;

    static const int SHUTTLE_AFFINE_ID = 0;
    static const int SHUTTLE_WIDTH = 64;
    static const int SHUTTLE_HEIGHT = 64;
    static const int SHUTTLE_X_POS = SCREEN_WIDTH / 2 - SHUTTLE_WIDTH;
    static const int SHUTTLE_Y_POS = SCREEN_HEIGHT / 2 - SHUTTLE_HEIGHT;
    static const int SHUTTLE_ANGLE = 0;
    static const tObjPriority SHUTTLE_PRIORITY = OBJPRIORITY_0;
    static const int SHUTTLE_TILE_ID = nextAvailableTileIdx;
    nextAvailableTileIdx += orangeShuttleTilesLen / BYTES_PER_16_COLOR_TILE; //XXX MAN NOTE orangeShuttle tiles is length in bytes

    static const int MOON_AFFINE_ID = 1;
    static const int MOON_WIDTH = 16;
    static const int MOON_HEIGHT = 16;
    static const int MOON_X_POS = SCREEN_WIDTH / 2 + MOON_WIDTH * 3 + MOON_WIDTH / 2;
    static const int MOON_Y_POS = SCREEN_WIDTH / 2 + MOON_HEIGHT;
    static const tObjPriority MOON_PRIORITY = OBJPRIORITY_2;
    static const int MOON_TILE_ID = nextAvailableTileIdx;
    nextAvailableTileIdx += moonTilesLen / BYTES_PER_16_COLOR_TILE;

    initOAM(spriteEntry, spriteRotation);

    //copy in the sprite palettes
    dmaCopyHalfWords(3, orangeShuttlePal, &SPRITE_PALETTE[SHUTTLE_AFFINE_ID * COLORS_PER_PALETTE], orangeShuttlePalLen);
    dmaCopyHalfWords(3, moonPal, &SPRITE_PALETTE[MOON_AFFINE_ID * COLORS_PER_PALETTE], moonPalLen);

    //copy the sprite graphics in obj graphics mem
    dmaCopyHalfWords(3, orangeShuttleTiles, &SPRITE_GFX[SHUTTLE_TILE_ID * BOUNDARY_VALUE], orangeShuttleTilesLen);
    dmaCopyHalfWords(3, moonTiles, &SPRITE_GFX[MOON_TILE_ID * BOUNDARY_VALUE], moonTilesLen);

    //////////////////////////////////////////////

    //create the ship sprite
    SpriteEntry * shuttle = &spriteEntry[SHUTTLE_AFFINE_ID];
    shuttle->attribute[0] = ATTR0_COLOR_16 | //16 color sprite
                            ATTR0_ROTSCALE_DOUBLE; //affine transformable
    shuttle->attribute[1] = ATTR1_ROTDATA(SHUTTLE_AFFINE_ID) | //location of affine transformation matrix
                            ATTR1_SIZE_64; //size 64x64
    moveSprite(shuttle, SHUTTLE_X_POS, SHUTTLE_Y_POS);
    rotateSprite(&spriteRotation[SHUTTLE_AFFINE_ID], SHUTTLE_ANGLE);
    shuttle->tileIdx = SHUTTLE_TILE_ID;
    setSpritePriority(shuttle, SHUTTLE_PRIORITY);
    shuttle->objPal = SHUTTLE_AFFINE_ID;
    shuttle->isHidden = false;

    //create the moon sprite
    SpriteEntry * moon = &spriteEntry[MOON_AFFINE_ID];
    moon->attribute[0] = ATTR0_COLOR_16; //16 color sprite (not affine transformable)
    moon->attribute[1] = ATTR1_SIZE_32; //size 32x32
    moveSprite(moon, MOON_X_POS, MOON_Y_POS);
    moon->tileIdx = MOON_TILE_ID;
    setSpritePriority(moon, MOON_PRIORITY);
    moon->objPal = MOON_AFFINE_ID;
    moon->isHidden = false;
}

void displayStarField() {
    dmaCopyHalfWords(3, starFieldBitmap, // This variable is generated for us by grit
            (uint16 *)BG_BMP_RAM(0), // Our address for main background 3
            starFieldBitmapLen);
}

void displayPlanet() {
    dmaCopyHalfWords(3, planetBitmap, // This variable is generated for us by grit
            (uint16 *)BG_BMP_RAM(8), // Our address for main background 2
            planetBitmapLen);
}

void displaySplash() {
    dmaCopyHalfWords(3, splashBitmap, //This variable is generated for us by grit
            (uint16 *)BG_BMP_RAM_SUB(0), // Our address for sub background 3
            splashBitmapLen);
}

int main() {

    /*  Turn on the 2D graphics core. */
    powerON(POWER_ALL_2D);
    /*
     *  Set up interrupts.
     *
     *  We don't really get into what these do exactly at this point in the
     *  manual, but we still need to do them for now.
     */
    irqInit();
    irqSet(IRQ_VBLANK, 0);

    /*  Configure the VRAM and background control registers */
    lcdMainOnBottom(); // Place the main screen on the bottom physical screen
    initVideo();
    initBackgrounds();

    /*  Display the backgrounds */
    displayStarField();
    displayPlanet();
    displaySplash();

    /* Display a few sprites */
	SpriteEntry *spritesMain = new SpriteEntry[128];
	SpriteRotation *spriteRotationsMain = (SpriteRotation *)spritesMain;
    displaySprites(spritesMain, spriteRotationsMain);

    for (;;) {
        swiWaitForVBlank();
        updateOAM(spritesMain); /* We have to copy our copy of OAM data into the actual OAM during VBlank (writes to it are locked during other times) */
    }

    return 0;
}