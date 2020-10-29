#ifndef COPPERVIEW_H
#define COPPERVIEW_H

/*
** CopperView.h
**
** by Samuel D. Crow
*/

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#ifndef EXEC_NODES_H
#include <exec/nodes.h>
#endif /* EXEC_NODES_H */

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif /* EXEC_LISTS_H */

#ifndef GRAPHICS_GFX_H
#include <graphics/gfx.h>
#endif /* GRAPHICS_GFX_H */

#ifndef HARDWARE_CUSTOM_H
#include <hardware/custom.h>
#endif /* HARDWARE_CUSTOM_H */

#ifndef UTILITY_HOOKS_H
#include <utility/hooks.h>
#endif

extern struct Custom custom;

/* assume big endianness on 68k code */
struct CH_Coordinate
{
    UWORD y;
    UWORD x;
};

union CH_CoordSort
{
    struct CH_Coordinate CH_Point;
    ULONG CH_Item;
};

/* Copper Wait is implied at the start of the series */
struct CH_Series
{
    struct MinNode CH_Node;
    struct hook *CH_Render;
    union CH_CoordSort CH_CS;
    UBYTE CH_VertWrap;
    UBYTE CH_HorizWrap;
    UWORD CH_SeriesFlags;
    ULONG CH_WordCount;
    UWORD CH_Moves[];
};

/* values for CH_SeriesFlags */
/* If the Odd and Even flags are both 0 structure is ignored */
#define CSB_EVEN 0
#define CSF_EVEN 1L<<CSB_EVEN
#define CSB_ODD 1
#define CSF_ODD 1L<<CSB_ODD

struct CH_Region
{
    struct MinNode CH_RegionQueue;
    struct MinList CH_Cargo;
};

struct CH_ViewPort
{
    struct MinNode CH_vp;
    /* Backlink */
    IPTR CH_MasterScreen;
    IPTR CH_ColorTable;
    struct CH_Series *CH_CopperSeriesEven;
    struct CH_Series *CH_CopperSeriesOdd;
    struct hook *CH_PortRenderEven;
    struct hook *CH_PortRenderOdd;
    struct hook *CH_Color;
    ULONG CH_ViewPortFlags;
    ULONG CH_ScreenMode;
    struct BitMap *CH_pf0;
    struct BitMap *CH_pf1;
    UWORD PxOffset0, PyOffset0;
    UWORD PxOffset1, PyOffset1;
};

#define VPB_DUALPF 0
#define VPF_DUALPF 1L<<VPB_DUALPF

struct CH_SpriteTable
{
    /* Maximum number of sprites */
    UBYTE CH_NumSprites;
    UBYTE CH_SpriteResolution;
    UWORD CH_SpriteWidth;
    ULONG CH_SpriteAllocation;
    /* number of CH_Sprite structs equal to number listed in CH_NumSprites */
    struct MinList CH_SpriteChannel[];
};

struct CH_View
{
    struct MinList CH_ViewPorts;
    struct cprlist *LOFCprList;
    struct cprlist *SHFCprList;
    WORD DxOffset, DyOffset;
    ULONG CH_ViewFlags;
    struct CH_CopperSeries *CH_MasterLOF;
    struct CH_CopperSeries *CH_MasterSHF;
    /* Can be NULL if sprites are unsupported in current screenmode */
    struct CH_SpriteTable *SprTab;
    struct hook *CH_RenderSpritesEven;
    struct hook *CH_RenderSpritesOdd;
};

/* Values for CH_ViewFlags */
/* LOF and SHF indicate which copper lists to sort */
#define CVB_LOF 0
#define CVF_LOF 1L<<CVB_LOF
#define CVB_SHF 1
#define CVF_SHF 1L<<CVB_SHF

/* CopWord replaces CMOVE and CopLong is a 32 bit version */
#define CopWord(reg,val) custom.##reg,(val)
#ifdef __SAGA__
#define CopLong(reg,val) (custom.##reg|0x8000),(val)>>16,(val)& 0xffff
#else
#define CopLong(reg,val) custom.##reg,(val)>>16,custom.##reg+2,(val)& 0xffff
#endif

/* Define 24-bit color move */
/* Note: Sets custom.bplcon3 to bplcon3bits other than ones used */
#if defined(__AGA__) || defined(__SAGA__)
/* TODO break SetBplcon3bits() into individual settings for color banks */
UWORD bplcon3bits=0;
#define SetBplcon3bits(val) bplcon3bits=(val) & 0x1cf7
#define EvenBits(byteComp) ((byteComp)&128)>>4|((byteComp)&32)>>3|((byteComp)&8)>>2|((byteComp)&2)>>1
#define OddBits(byteComp2) ((byteComp2)&64)>>2|((byteComp2)&16)>>1|((byteComp2)&4)>>1|(byteComp2)&1
#define CopColor(entry,r,g,b) custom.bplcon3, bplcon3bits|((entry)& 0xe0)<<8, \
    custom.color[(entry)& 0x1f], (EvenBits(r))<<8|(EvenBits(g))<<4|(EvenBits(b)), \
    custom.bplcon3, bplcon3bits|((entry)& 0xe0)<<8|1<<9, \
    custom.color[(entry)& 0x1f], (OddBits(r))<<8|(OddBits(g))<<4|(OddBits(b))
#else
/* ECS or OCS jams only most significan nybbles */
#define CopColor(entry,r,g,b) custom.color[entry], ((r)& 0xf0)<<4|((g)& 0xf0)|((b)& 0xf0)>>4
#endif

#endif
