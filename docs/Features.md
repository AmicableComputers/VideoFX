***VideoFX Features***

VideoFX.library unlocks a number of features that weren't present on the original Graphics.library in AmigaOS.

**Sprite Ports**

Sprites on the original Graphics.library routines required an external bitmap to be copied to the sprite buffer by the CPU.  VideoFX instead allocates a bitmap the size of the area in the buffer to be used.  Once allocated, you are free to add the bitmap structure to its own RastPort structure and draw to the sprite buffer directly.

Some modes allow you use the CPU to do scaling and inverting images horizontally or vertically.  If you use SF_BLIT flag when defining the current SpritePort, you will be limited only to the options that use the blitter when drawing to the sprite DMA.  These include standard size, interlacing or pixel doubling vertically (but not at the same time).

**Multi Blitting**

In order to speed the blitter usage, VideoFX allows you to define multiple passes to the blitter all at once and queue them in one swipe.  Due to the sequential nature of the blitter interrupt handling, only the last blit node in the sequence needs to do any cleanup at all and all prior blits can safely assume that the blitter registers are untouched from the previous node's usage.

Note:  In order to make this capabiltiy work, the OwnBlitter and DisownBlitter functions should not be used with this library.  Normally QBlit takes precedence over OwnBlitter anyway so it may not matter but the blitter interrupt may be optimized to not need as much time as the default interrupt handlers would require.  One way to insure that the blitter interrupt doesn't get overloaded with time waste is to make sure that the interrupt service routine stores only the registers acutally used by the routine instead of arbitrarily storing all of them.