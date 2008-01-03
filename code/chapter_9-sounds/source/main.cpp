#include <nds.h>
#include <assert.h>
#include "sprites.h"
#include "ship.h"

/* Backgrounds */
#include "starField.h"
#include "planet.h"
#include "splash.h"
/* Sprites */
#include "orangeShuttle.h"
#include "moon.h"
/* Sounds */
#include "thrust_1_22050_8_s_raw.h"

/* Select a low priority DMA channel to perform our sprite and background
 * copying. */
static const int DMA_CHANNEL = 3;

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

    /*  Place main screen background 2 an interesting place. */
    BG2_CX = -(SCREEN_WIDTH / 2 - 32) << 8;
    BG2_CY = -32 << 8;

    /*  Set up affine background 3 on the sub screen as a 16-bit color
     *  background
     */
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

void initSprites(tOAM *oam, SpriteInfo *spriteInfo) {
    /* Define some sprite configuration specific constants. We will use these
     * to compute the proper index into memory for certain tiles or palettes */
    static const int BYTES_PER_16_COLOR_TILE = 32;
    static const int COLORS_PER_PALETTE = 16;
    static const int OFFSET_MULTIPLIER = 16;

    /* Keep track of the available tiles */
    int nextAvailableTileIdx = 0;

    /* Create the ship sprite. */
    static const int SHUTTLE_AFFINE_ID = 0;
    assert(SHUTTLE_AFFINE_ID < SPRITE_COUNT);
    SpriteInfo * shuttleInfo = &spriteInfo[SHUTTLE_AFFINE_ID];
    SpriteEntry * shuttle = &oam->spriteBuffer[SHUTTLE_AFFINE_ID];

    /* Initialize shuttleInfo */
    shuttleInfo->affineId = SHUTTLE_AFFINE_ID;
    shuttleInfo->width = 64;
    shuttleInfo->height = 64;
    shuttleInfo->angle = 462;
    shuttleInfo->entry = shuttle;

    /*
     *  Configure attribute 0. 
     *
     *  OBJCOLOR_16 will make a 16-color sprite. We specify that we want an
     *  affine sprite (via isRotoscale) here because we would like to rotate
     *  the ship.
     */
    shuttle->posY = SCREEN_HEIGHT / 2 - shuttleInfo->height;
    shuttle->isRotoscale = true;
    /* This assert is a check to see a matrix is available to store the affine
     * transformation matrix for this sprite. Of course, you don't have to have
     * the matrix id match the affine id, but if you do make them match, this
     * assert can be helpful. */
    assert(!shuttle->isRotoscale || (shuttleInfo->affineId < MATRIX_COUNT));
    shuttle->rsDouble = false;
    shuttle->objMode = OBJMODE_NORMAL;
    shuttle->isMosaic = false;
    shuttle->colMode = OBJCOLOR_16;
    shuttle->objShape = OBJSHAPE_SQUARE;

    /*
     *  Configure attribute 1.
     *
     *  rsMatrixId refers to the loation of affine transformation matrix. We
     *  set it to a location computed with a macro. OBJSIZE_64, in our case
     *  since we are making a square sprite, creates a 64x64 sprite.
     */
    shuttle->posX = SCREEN_WIDTH / 2 - shuttleInfo->width * 2 +
                    shuttleInfo->width / 2;
    shuttle->rsMatrixIdx = ATTR1_ROTDATA(shuttleInfo->affineId);
    shuttle->objSize = OBJSIZE_64;

    /* 
     *  Configure attribute 2.
     * 
     *  Configure which tiles the sprite will use, which priority layer it will
     *  be placed onto, which palette the sprite should use, and whether or not
     *  to show the sprite.
     */
    shuttle->tileIdx = nextAvailableTileIdx;
    nextAvailableTileIdx += orangeShuttleTilesLen / BYTES_PER_16_COLOR_TILE;
    shuttle->objPriority = OBJPRIORITY_0;
    shuttle->objPal = shuttleInfo->affineId;

    /* Rotate the sprite */
    rotateSprite(&oam->matrixBuffer[shuttleInfo->affineId],
                 shuttleInfo->angle);

    /*************************************************************************/

    /* Create the moon sprite. */
    static const int MOON_AFFINE_ID = 1;
    assert(MOON_AFFINE_ID < SPRITE_COUNT);
    SpriteInfo * moonInfo = &spriteInfo[MOON_AFFINE_ID];
    SpriteEntry * moon = &oam->spriteBuffer[MOON_AFFINE_ID];

    /* Initialize moonInfo */
    moonInfo->affineId = MOON_AFFINE_ID;
    moonInfo->width = 32;
    moonInfo->height = 32;
    moonInfo->angle = 462;
    moonInfo->entry = moon;

    /*
     *  Configure attribute 0.
     *
     *  OBJCOLOR_16 will make a 16-color sprite. We won't specify that we want
     *  an affine sprite here because we don't want one this time.
     */
    moon->posY = SCREEN_WIDTH / 2 + moonInfo->height / 2;
    moon->isRotoscale = false;
    /* This assert is a check to see a matrix is available to store the affine
     * transformation matrix for this sprite. Of course, you don't have to have
     * the matrix id match the affine id, but if you do make them match, this
     * assert can be helpful. */
    assert(!moon->isRotoscale || (moonInfo->affineId < MATRIX_COUNT));
    moon->isHidden = false;
    moon->objMode = OBJMODE_NORMAL;
    moon->isMosaic = false;
    moon->colMode = OBJCOLOR_16;
    moon->objShape = OBJSHAPE_SQUARE;

    /*
     * Configure attribute 1.
     *
     * OBJSIZE_32 will create a sprite of size 32x32, since we are making a
     * square sprite. Since we are using a non-affine sprite, attribute 1
     * doesn't have an rsMatrixIdx anymore. Instead, it has the ability to flip
     * the sprite vertically or horizontally.
     */
    moon->posX = SCREEN_WIDTH / 2 + moonInfo->width + moonInfo->width / 2;
    moon->hFlip = false;
    moon->vFlip = false;
    moon->objSize = OBJSIZE_32;

    /* 
     *  Configure attribute 2.
     * 
     *  Configure which tiles the sprite will use, which priority layer it will
     *  be placed onto, which palette the sprite should use, and whether or not
     *  to show the sprite.
     */
    moon->tileIdx = nextAvailableTileIdx;
    nextAvailableTileIdx += moonTilesLen / BYTES_PER_16_COLOR_TILE;
    moon->objPriority = OBJPRIORITY_2;
    moon->objPal = moonInfo->affineId;

    /*************************************************************************/

    /* Copy over the sprite palettes */
    dmaCopyHalfWords(DMA_CHANNEL,
                     orangeShuttlePal,
                     &SPRITE_PALETTE[shuttleInfo->affineId *
                                     COLORS_PER_PALETTE],
                     orangeShuttlePalLen);
    dmaCopyHalfWords(DMA_CHANNEL,
                     moonPal,
                     &SPRITE_PALETTE[moonInfo->affineId * COLORS_PER_PALETTE],
                     moonPalLen);

    /* Copy the sprite graphics to sprite graphics memory */
    dmaCopyHalfWords(DMA_CHANNEL,
                     orangeShuttleTiles,
                     &SPRITE_GFX[shuttle->tileIdx * OFFSET_MULTIPLIER],
                     orangeShuttleTilesLen);
    dmaCopyHalfWords(DMA_CHANNEL,
                     moonTiles,
                     &SPRITE_GFX[moon->tileIdx * OFFSET_MULTIPLIER],
                     moonTilesLen);
}

void displayStarField() {
    dmaCopyHalfWords(DMA_CHANNEL,
                     starFieldBitmap, /* This variable is generated for us by
                                       * grit. */
                     (uint16 *)BG_BMP_RAM(0), /* Our address for main
                                               * background 3 */
                     starFieldBitmapLen);
}

void displayPlanet() {
    dmaCopyHalfWords(DMA_CHANNEL,
                     planetBitmap, /* This variable is generated for us by
                                    * grit. */
                     (uint16 *)BG_BMP_RAM(8), /* Our address for main
                                               * background 2 */
                     planetBitmapLen);
}

void displaySplash() {
    dmaCopyHalfWords(DMA_CHANNEL,
                     splashBitmap, /* This variable is generated for us by
                                    * grit. */
                     (uint16 *)BG_BMP_RAM_SUB(0), /* Our address for sub
                                                     background 3 */
                     splashBitmapLen);
}

void updateInput(touchPosition * touch) {
	//updates the key registers with current values
	scanKeys();

    //update the touch screen read values
    *touch = touchReadXY();
}

void handleInput(Ship * ship, MathVector2D<int> * moonPos,
                 SpriteInfo * moonInfo, TransferSoundData * soundData,
                 touchPosition * touch) {

    /* Handle up and down parts of D-Pad. */
    if (keysDown() & KEY_UP) {
        // Play our sound only when the button is initially pressed
		playSound(soundData);
    }
	if (keysHeld() & KEY_UP) {
		//accelerate ship
		ship->accelerate();
	} else if (keysHeld() & KEY_DOWN) {
		//reverse ship direction
		ship->reverseTurn();
    }
	
    /* Handle left and right parts of D-Pad. */
	if (keysHeld() & KEY_LEFT) {
		//rotate counter clockwise
		ship->turnCounterClockwise();
    } else if (keysHeld() & KEY_RIGHT) {
		//rotate clockwise
		ship->turnClockwise();
	}

    /* Handle the touch screen. */
    static MathVector2D<int> moonGrip;
    if (keysDown() & KEY_TOUCH) {
        /* Record the grip */
        moonGrip.x = touch->px;
        moonGrip.y = touch->py;
    } else if (keysHeld() & KEY_TOUCH) {
        int newX = moonPos->x + touch->px - moonGrip.x;
        int newY = moonPos->y + touch->py - moonGrip.y;

        /* Prevent dragging off the screen */
        if (newX < 0) {
            moonPos->x = 0;
        } else if (newX > (SCREEN_WIDTH - moonInfo->width)) {
            moonPos->x = SCREEN_WIDTH - moonInfo->width;
        } else {
            moonPos->x = newX;
        }
        if (newY < 0) {
            moonPos->y = 0;
        } else if (newY > (SCREEN_HEIGHT - moonInfo->height)) {
            moonPos->y = SCREEN_HEIGHT - moonInfo->height;
        } else {
            moonPos->y = newY;
        }

        /* Record the grip again. */
        moonGrip.x = touch->px;
        moonGrip.y = touch->py;
    }
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

    /*
     *  Configure the VRAM and background control registers.
     *
     *  Place the main screen on the bottom physical screen. Then arrange the
     *  VRAM banks. Next, confiure the background control registers.
     */
    lcdMainOnBottom();
    initVideo();
    initBackgrounds();

    /* Set up a few sprites. */
    SpriteInfo spriteInfo[SPRITE_COUNT];
    tOAM *oam = new tOAM();
    initOAM(oam);
    initSprites(oam, spriteInfo);

    /* Display the backgrounds. */
    displayStarField();
    displayPlanet();
    displaySplash();

    /*************************************************************************/

    /* Keep track of the touch screen coordinates */
    touchPosition touch;

    /* Make the ship object */
    static const int SHUTTLE_AFFINE_ID = 0;
    SpriteEntry * shipEntry = &oam->spriteBuffer[SHUTTLE_AFFINE_ID];
    SpriteRotation * shipRotation = &oam->matrixBuffer[SHUTTLE_AFFINE_ID];
    Ship * ship = new Ship(&spriteInfo[SHUTTLE_AFFINE_ID]);

    /* Make the moon */
    static const int MOON_AFFINE_ID = 1;
    SpriteEntry * moonEntry = &oam->spriteBuffer[MOON_AFFINE_ID];
    SpriteInfo * moonInfo = &spriteInfo[MOON_AFFINE_ID];
    MathVector2D<int> * moonPos = new MathVector2D<int>();
    moonPos->x = moonEntry->posX;
    moonPos->y = moonEntry->posY;

    /*  Set up sound data.*/
	TransferSoundData * thrust_sound = new TransferSoundData();
	thrust_sound->data = thrust_1_22050_8_s_raw;
	thrust_sound->len = thrust_1_22050_8_s_raw_size;
	thrust_sound->rate = 22050; //22050Hz Sample Rate
	thrust_sound->vol = 127; //Volume can be from 0 to 127
	thrust_sound->pan = 64; //Sound comes equally from both speakers (0-127, left to right)
	thrust_sound->format = 1; //1 - 8bit, 0 - 16bit

    for (;;) {
        /* Update the game state. */
		updateInput(&touch);
		handleInput(ship, moonPos, moonInfo, thrust_sound, &touch);
        ship->moveShip();

        /* Update ship sprite attributes. */
        MathVector2D<float> position = ship->getPosition();
        shipEntry->posX = (int)position.x;
        shipEntry->posY = (int)position.y;
        rotateSprite(shipRotation, ship->getAngleDeg512());
        /* Update moon sprite attributes. */
        moonEntry->posX = (int)moonPos->x;
        moonEntry->posY = (int)moonPos->y;

        /*
         *  Update the OAM.
         *
         *  We have to copy our copy of OAM data into the actual OAM during
         *  VBlank (writes to it are locked during other times).
         */
        swiWaitForVBlank();
        updateOAM(oam);
    }

    return 0;
}