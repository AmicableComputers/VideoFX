#ifndef VIDEO_VIEW_H
#define VIDEO_VIEW_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif /* EXEC_TYPES_H */

#ifndef EXEC_NODES_H
#include <exec/nodes.h>
#endif /* EXEC_NODES_H */

#ifndef EXEC_LISTS_H
#include <exec/lists.h>
#endif /* EXEC_LISTS_H */

#ifndef SPRITEPORT_H
#include <videofx/spriteport.h>
#endif /* VIDEOFX SPRITEPORT_H */

#ifndef COPPERVIEW_H
#include <videofx/copperview.h>
#endif /* VIDEOFX COPPERVIEW_H */

#include <SDI_Compiler.h>
#include <SDI_Hook.h>

struct VFX_ViewPort
{
	struct MinNode VFX_vp;
	/* Backlink */
	IPTR VFX_MasterScreen;
	IPTR VFX_ColorTable;
	struct VFX_SeriesList *VFX_CopperSeriesEven;
	struct VFX_SeriesList *VFX_CopperSeriesOdd;
	struct hook *VFX_PortRenderEven;
	struct hook *VFX_PortRenderOdd;
	struct hook *VFX_Color;
	ULONG VFX_ViewPortFlags;
	ULONG VFX_ScreenMode;
	struct BitMap *VFX_pf0;
	struct BitMap *VFX_pf1;
	UWORD PxOffset0, PyOffset0;
	UWORD PxOffset1, PyOffset1;
};

#define VPB_DUALPF 0
#define VPF_DUALPF 1L<<VPB_DUALPF
/* indicate no copper palette changes */
#define VPB_CONSTPALETTE 1
#define VPF_CONSTPALETTE 1L<<VPB_CONSTPALETTE

struct VFX_View
{
	struct MinList VFX_ViewPorts;
	struct cprlist *LOFCprList;
	struct cprlist *SHFCprList;
	WORD DxOffset, DyOffset;
	ULONG VFX_ViewFlags;
	struct VFX_CopperSeries *VFX_MasterLOF;
	struct VFX_CopperSeries *VFX_MasterSHF;
	/* Can be NULL if sprites are unsupported in current screenmode */
	struct VFX_SpriteTable *SprTab;
	struct hook *VFX_RenderSpritesEven;
	struct hook *VFX_RenderSpritesOdd;
};

/* Values for VFX_ViewFlags */
/* LOF and SHF indicate which copper lists to sort */
#define CVB_LOF 0
#define CVF_LOF 1L<<CVB_LOF
#define CVB_SHF 1
#define CVF_SHF 1L<<CVB_SHF

#endif