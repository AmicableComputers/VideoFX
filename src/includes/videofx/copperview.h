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

#ifndef HARDWARE_CUSTOM_H
#include <hardware/custom.h>
#endif /* HARDWARE_CUSTOM_H */

#include <SDI_Compiler.h>
#include <SDI_Hook.h>

extern struct Custom custom;

/* assume big endianness on 68k code */
struct VFX_Coordinate
{
	UWORD y;
	UWORD x;
};

union VFX_CoordSort
{
	struct VFX_Coordinate VFX_Point;
	ULONG VFX_Item;
};

/* Copper Wait is implied at the start of the series */
struct VFX_CopperSeries
{
	struct hook *VFX_Render;
	union VFX_CoordSort VFX_CS;
	/* Private!  Initialize to 0 */
	UBYTE VFX_VertWrap;
	UBYTE VFX_HorizWrap;
	/* public */
	UWORD VFX_SeriesFlags;
	ULONG VFX_WordCount;
	UWORD VFX_Moves[];
};

/* values for VFX_SeriesFlags */
/* If the Odd and Even flags are both 0 the series structure is ignored */
#define CSB_EVEN 0
#define CSF_EVEN 1s<<CSB_EVEN
#define CSB_ODD 1
#define CSF_ODD 1s<<CSB_ODD

#define INITCOPSERIES(name, x, y, flags, renderHook) \
struct VFX_CopperSeries name \
{\
	(renderHook),\
	{(y),(x)}, /* Coordinate */ \
	0,0, /* initialize private fiedlds to 0 */\
	(flags),\
	(sizeof(name)-sizeof(struct VFX_CopperSeries))/sizeof(UWORD), /* word count */

#define ENDCOPSERIES };

struct VFX_SeriesList
{
	/* Maxemum number of series in entries array */
	UWORD MaxSeries;
	/* Current tally of series */
	UWORD NumSeries;
	struct VFX_CopperSeries *Entries[];
};

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
#define CopColor(entry,r,g,b) \
	custom.bplcon3, bplcon3bits|((entry)& 0xe0)<<8, \
	custom.color[(entry)& 0x1f], (EvenBits(r))<<8|(EvenBits(g))<<4|(EvenBits(b)), \
	custom.bplcon3, bplcon3bits|((entry)& 0xe0)<<8|1<<9, \
	custom.color[(entry)& 0x1f], (OddBits(r))<<8|(OddBits(g))<<4|(OddBits(b))
#else
/* ECS or OCS jams only most significan nybbles */
#define CopColor(entry,r,g,b) custom.color[entry], ((r)& 0xf0)<<4|((g)& 0xf0)|((b)& 0xf0)>>4
#endif

#endif
