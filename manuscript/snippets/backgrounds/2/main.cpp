/*
 *  main.cpp
 *
 *  Created by Jaeden Amero on 11/12/07.
 *  Copyright 2007. All rights reserved.
 *
 */

#include <nds.h>

#include "planet.h"
#include "splash.h"
#include "starField.h"

/* Select a low priority DMA channel to perform our background copying. */
static const int DMA_CHANNEL = 3;

// ANCHOR: init_video
void initVideo() {
  /*
   *  Map VRAM to display a background on the main and sub screens.
   *
   *  The vramSetPrimaryBanks function takes four arguments, one for each of
   *  the major VRAM banks. We can use it as shorthand for assigning values
   *  to each of the VRAM bank's control registers.
   *
   *  We map banks A and B to main screen  background memory. This gives us
   *  256KB, which is a healthy amount for 16-bit graphics.
   *
   *  We map bank C to sub screen background memory.
   *
   *  We map bank D to LCD. This setting is generally used for when we aren't
   *  using a particular bank.
   */
  vramSetPrimaryBanks(VRAM_A_MAIN_BG_0x06000000, VRAM_B_MAIN_BG_0x06020000,
                      VRAM_C_SUB_BG_0x06200000, VRAM_D_LCD);

  /*  Set the video mode on the main screen to mode 5. */
  videoSetMode(MODE_5_2D);

  /*  Set the video mode on the sub screen to mode 5. */
  videoSetModeSub(MODE_5_2D);
}
// ANCHOR_END: init_video

// ANCHOR: init_backgrounds
void displayStarField() {
  /*  Set up affine background 3 on main screen as a 16-bit color background. */
  int id = bgInit(3,
                  BgType_Bmp16,
                  BgSize_B16_256x256,
                  0, // Map base: The starting place in memory
                  0); // Tile base: Not used in 16-bit backgrounds

  /* Use the lowest possible priority */
  bgSetPriority(id, 3);

  /* Copy graphics data */
  dmaCopyHalfWords(DMA_CHANNEL, starFieldBitmap, /* This variable is generated
                                                  * for us by grit. */
                   bgGetGfxPtr(id),              /* Our address for main
                                                  * background 3 */
                   starFieldBitmapLen);          /* This length (in bytes) is
                                                  * generated from grit. */
}

void displayPlanet() {
  /*  Set up affine background 2 on main as a 16-bit color background. */
  int id = bgInit(2,
                  BgType_Bmp16,
                  BgSize_B16_128x128,
                  8,
                  0);

  /* Set a low priority, but higher than priority 3 */
  bgSetPriority(id, 2);

  /*  Place main screen background 2 in an interesting place. */
  bgSetScroll(id,
              -(SCREEN_WIDTH / 2 - 32),
              -32);

  /* Copy graphics data */
  dmaCopyHalfWords(DMA_CHANNEL, planetBitmap,
                   bgGetGfxPtr(id),
                   planetBitmapLen);
}

void displaySplash() {
  /*  Set up affine background 3 on the sub screen as a 16-bit color
   *  background.
   */
  int id = bgInitSub(3,
                     BgType_Bmp16,
                     BgSize_B16_256x256,
                     0,
                     0);

  /* Use the lowest possible priority */
  bgSetPriority(id, 3);

  /* Copy graphics data */
  dmaCopyHalfWords(DMA_CHANNEL, splashBitmap,
                   bgGetGfxPtr(id),
                   splashBitmapLen);
}

void initBackgrounds() {
  /* Display the backgrounds. */
  displayStarField();
  displayPlanet();
  displaySplash();

  /* Refresh background registers */
  bgUpdate();
}
// ANCHOR_END: init_backgrounds

int main() {
  /* Turn on the 2D graphics core. */
  powerOn(POWER_ALL_2D);

  /* Configure the VRAM and background control registers. */
  lcdMainOnBottom(); // Place the main screen on the bottom physical screen
  initVideo();
  initBackgrounds();

  /* Loop forever so that the Nintendo DS doesn't reboot upon program
   * completion. */
  for (;;)
    ;

  return 0;
}
