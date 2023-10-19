/*
** Copper.c
**
** by Samuel D. Crow
*/
#include <exec/types.h>
#include <exec/memory.h>
#include <graphics/gfxbase.h>
#include <graphics/view.h>
#include <graphics/videocontrol.h>
#include <intuition/intuition.h>
#include <intuition/preferences.h>
#include <hardware/custom.h>
#include <libraries/dos.h>
#include <utility/hooks.h>

#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdarg.h>
#include <SDI_compiler.h>

#include <videofx/copperview.h>"

/* prototypes */
int SortCop();
struct VFX_ViewPort *CreateViewPort();
struct VFX_View *CreateView();

/* globals */
struct ViewPort vp;
struct MinList copperNodes;

/*
** Sorts all VFX_Series structures in a view
** Returns 0 on success, 1 on failed allocation, 2 on empty region list
** Result left in copperNodes
*/
int SortCop(struct VFX_View *view)
{
    APTR regionPool=CreatePool(MEMF_ANY, 16*sizeof(struct VFX_Region), sizeof(struct VFX_Region));
    struct MinList regionHead, regionTail, *region, *region2, *out;
    struct VFX_Series *iter, *iter2, *iterOut;
    ULONG previousCoord;

    if (NULL==regionPool) return 1;
    region=(struct VFX_Region *)AllocPooled(regionPool, sizeof(struct VFX_Region));
    iterOut=region->VFX_Cargo;

    /* Initialize iterOut, regionHead, and regionTail */
    iterOut.mlh_tail=NULL;
    iterOut->mlh_head=&iterOut->mlh_tail;
    iterOut->mlh_tailpred=&iterOut->mlh_head;
    
    regionHead.mlh_tail=NULL;
    regionHead->mlh_head=&regionHead->mlh_tail;
    regionHead->mlh_tailpred=&regionHead->mlh_head;

    regionTail.mlh_tail=NULL;
    regionTail->mlh_head=&regionTail->mlh_tail;
    regionTail->mlh_tailpred=&regionTail->mlh_head;

    iter=(VFX_Node)RemHead(copperNodes);
    if (NULL==iter)
    {
        FreePool(regionPool);
        return 2;
    }
    /* build region list */
    previousCoord=iter->VFX_CS.VFX_Sort;
    do
    {
        if (iter->VFX_CS.VFX_Sort < previousCoord)
        {
            region=(VFX_Region *)AllocPooled(regionPool, sizeof(struct VFX_Region));
            if (NULL==region)
            {
                /* memory allocation failed */
                FreePool(regionPool);
                return 1;
            }
            iterOut=region->VFX_Cargo;
            iterOut.mlh_tail=NULL;
            iterOut->mlh_head=&iterOut->mlh_tail;
            iterOut->mlh_tailpred=&iterOut->mlh_head;
            AddTail(regionHead, region);
            previousCoord=VFX_CS.VFX_Sort;
        }
        AddTail(iterOut, iter);
        iter=(VFX_Node *)RemHead(copperNodes);
    } while (iter);
    
    /* proceed with natural mergesort */
    do
    {
        do
        {
            region=RemHead(regionHead);

            /* Are there 2 regions left to merge? */
            while (region && ((region2=RemHead(regionHead))))
            {
                /* Yes, merge 2 regions into out */
                out=(struct VFX_Region *)AllocPooled(regionPool, sizeof(struct VFX_Region));
                iterOut=out->VFX_Cargo;
                IterOut.mlh_tail=NULL;
                IterOut->mlh_head=&IterOut->mlh_tail;
                IterOut->mlh_tailpred=&IterOut->mlh_head;
                iter=(VFX_Series *)region->VFX_Cargo;
                iter2=(VFX_Series *)region2->VFX_Cargo;
                while (!isListEmpty(iter.VFX_Node) && !isListEmpty(iter2.VFX_Node))
                {
                    if (iter->VFX_CS.VFX_Sort < iter2->VFX_CS.VFX_Sort)
                    {
                        AddTail(iterOut, RemHead(iter.VFX_Node));
                    } else
                    {
                        AddTail(iterOut, RemHead(iter2.VFX_Node));
                    }
                }
                /* Only one region has entries left, move to out. */
                if (isListEmpty(iter))
                {
                    while (!isListEmpty(region2->VFX_Cargo)) AddTail(iterOut, RemHead(region2->VFX_Cargo));
                } else
                {
                    while (!isListEmpty(region->VFX_Cargo)) AddTail(iterOut, RemHead(region->VFX_Cargo));
                }
                /* Link to regionHead */
                AddTail(regionTail, out);
                
                /* Deallocate */
                FreePooled(region);
                region=RemHead(regionHead);
                FreePooled(region2);
            }
            /* No, only one region left in the current list. */
            if(region) AddTail(regionTail->VFX_RegionQueue, region);

        } while (!isListEmpty(regionHead));
        /* move contents to regionHead and clear regionTail*/
        regionHead.mlh_head=regionTail.mlh_head;
        regionHead.mlh_tailpred=regionTail.mlh_tailpred;
        regionTail.mlh_head=&regionTail.mlh_tail;
        regionTail.mlh_tailpred=&regionTail.mlh_head;
        /* repeat until there is one region left */
    } while(regionHead.mlh_head!=regionHead.mlh_tailpred);

    /* put results back in the global list */
    iter=((VFX_Series *)regionHead.mlh_head)->VFX_Cargo;
    copperNodes.mlh_head=iter->VFX_Node.mlh_head;
    copperNodes.mlh_tailpred=iter->VFX_Node.mlh_tailpred;
    FreePool(regionPool);
    return 0;
}

/*
** Creates VFX_View structure and initializes it 
**   along with the VFX_SpriteTable structure referred to by it, if supported.
** Returns pointer to VFX_View or NULL if memory allocation fails
*/
struct VFX_View *CreateView()
{
    struct MinList *iter;
#ifndef __RTG__
    struct VFX_SpriteTable *sprites;
    int spriteCount;
#endif
    struct VFX_View *view=(struct VFX_View *)AllocMem(MEMF_PUBLIC | MEMF_CLEAR, sizeof(struct VFX_View));
    if (NULL==view) return NULL;
    /* Initialize ViewPort list */
    iter=view->VFX_ViewPort;
    iter.mlh_tail=NULL;
    iter->mlh_head=&iter->mlh_tail;
    iter->mlh_tailpred=&iter->mlh_head;
#ifndef __RTG__
#ifdef __SAGA__
#define NUMSPRITES 16
#else
#define NUMSPRITES 8
#endif
    /* ALlocate and initialize VFX_SpriteTable structure */
    sprites=(struct VFX_SpriteTable *)AllocMem(
        sizeof(struct VFX_SpriteTable)+NUMSPRITES*sizeof(struct MinList),
        MEMF_PUBLIC |MEMF_CLEAR);
    if (NULL==sprites)
    {
        FreeMem(view, sizeof(struct VFX_View));
        return NULL;
    }
    sprites->VFX_NumSprites=NUMSPRITES;
    view->VFX_SpriteTable=sprites;    
    for (spriteCount=NUMSPRITES; spriteCount>0; --spriteCount)
    {
        iter=sprites->VFX_SpriteChannel[spriteCount-1];
        iter.mlh_tail=NULL;
        iter->mlh_head=&iter->mlh_tail;
        iter->mlh_tailpred=&iter->mlh_head;
    }
#undef NUMSPRITES
#endif /* RTG undef */
    return view;
}

/*
** Creates and initializes VFX_ViewPort structure.
** Accepts pointer to VFX_ColorTable 
**  which can be NULL if part of a VFX_ViewPort group sharing a palette.
** Returns pointer to VFX_ViewPort or NULL if allocation fails.
*/
struct VFX_ViewPort *CreateViewPort(IPTR *colorTab)
{
    struct VFX_ViewPort *vp=AllocMem(sizeof(struct VFX_ViewPort), MEMF_PUBLIC | MEMF_CLEAR);
    if (NULL==vp) return NULL;
    vp->VFX_ColorTable=colorTab;
    
    return vp;
}
