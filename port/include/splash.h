#pragma once

// Shows a fullscreen splash image very early at startup, before the game's
// own video/audio subsystems come up, then tears itself down completely.
// Currently only does anything on Nintendo Switch (used to credit the NRO
// port); it's a no-op on every other platform.
void splashShow(void);
