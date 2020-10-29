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
#include <SDI_compiler.h>

#include "copperview.h"

/* prototypes */
int SortCop();
struct CH_ViewPort *CreateViewPort();
struct CH_View *CreateView();

/* globals */
struct ViewPort vp;
struct MinList copperNodes;

/*
** Sorts all CH_Series structures in a view
** Returns 0 on success, 1 on failed allocation, 2 on empty region list
** Result left in copperNodes
*/
int SortCop(struct CH_View *view)
{
    APTR regionPool=CreatePool(MEMF_ANY, 16*sizeof(struct CH_Region), sizeof(struct CH_Region));
    struct MinList regionHead, regionTail, *region, *region2, *out;
    struct CH_Series *iter, *iter2, *iterOut;
    ULONG previousCoord;

    if (NULL==regionPool) return 1;
    region=(struct CH_Region *)AllocPooled(regionPool, sizeof(struct CH_Region));
    iterOut=region->CH_Cargo;

    /* Initialize iterOut, regionHead, and regionTail */
    iterOut.mlh_tailpred=NULL;
    iterOut->mlh_head=&iterOut->mlh_tailpred;
    iterOut->mlh_tail=&iterOut->mlh_head;
    
    regionHead.mlh_tailpred=NULL;
    regionHead->mlh_head=&regionHead->mlh_tailpred;
    regionHead->mlh_tail=&regionHead->mlh_head;

    regionTail.mlh_tailpred=NULL;
    regionTail->mlh_head=&regionTail->mlh_tailpred;
    regionTail->mlh_tail=&regionTail->mlh_head;

    iter=(CH_Node)RemHead(copperNodes);
    if (NULL==iter)
    {
        FreePool(regionPool);
        return 2;
    }
    /* build region list */
    previousCoord=iter->CH_CS.CH_Sort;
    do
    {
        if (iter->CH_CS.CH_Sort < previousCoord)
        {
            region=(CH_Region *)AllocPooled(regionPool, sizeof(struct CH_Region));
            if (NULL==region)
            {
                /* memory allocation failed */
                FreePool(regionPool);
                return 1;
            }
            iterOut=region->CH_Cargo;
            iterOut.mlh_tailpred=NULL;
            iterOut->mlh_head=&iterOut->mlh_tailpred;
            iterOut->mlh_tail=&iterOut->mlh_head;
            AddTail(regionHead, region);
            previousCoord=CH_CS.CH_Sort;
        }
        AddTail(iterOut, iter);
        iter=(CH_Node *)RemHead(copperNodes);
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
                out=(struct CH_Region *)AllocPooled(regionPool, sizeof(struct CH_Region));
                iterOut=out->CH_Cargo;
                IterOut.mlh_tailpred=NULL;
                IterOut->mlh_head=&IterOut->mlh_tailpred;
                IterOut->mlh_tail=&IterOut->mlh_head;
                iter=(CH_Series *)region->CH_Cargo;
                iter2=(CH_Series *)region2->CH_Cargo;
                while (!isListEmpty(iter.CH_Node) && !isListEmpty(iter2.CH_Node))
                {
                    if (iter->CH_CS.CH_Sort < iter2->CH_CS.CH_Sort)
                    {
                        AddTail(iterOut, RemHead(iter.CH_Node));
                    } else
                    {
                        AddTail(iterOut, RemHead(iter2.CH_Node));
                    }
                }
                /* Only one region has entries left, move to out. */
                if (isListEmpty(iter))
                {
                    while (!isListEmpty(region2->CH_Cargo)) AddTail(iterOut, RemHead(region2->CH_Cargo));
                } else
                {
                    while (!isListEmpty(region->CH_Cargo)) AddTail(iterOut, RemHead(region->CH_Cargo));
                }
                /* Link to regionHead */
                AddTail(regionTail, out);
                
                /* Deallocate */
                FreePooled(region);
                region=RemHead(regionHead);
                FreePooled(region2);
            }
            /* No, only one region left in the current list. */
            if(region) AddTail(regionTail->CH_RegionQueue, region);

        } while (!isListEmpty(regionHead));
        /* move contents to regionHead and clear regionTail*/
        regionHead.mlh_head=regionTail.mlh_head;
        regionHead.mlh_tail=regionTail.mlh_tail;
        regionTail.mlh_head=&regionTail.mlh_tailpred;
        regionTail.mlh_tail=&regionTail.mlh_head;
        /* repeat until there is one region left */
    } while(regionHead.mlh_head!=regionHead.mlh_tail);

    /* put results back in the global list */
    iter=((CH_Series *)regionHead.mlh_head)->CH_Cargo;
    copperNodes.mlh_head=iter->CH_Node.mlh_head;
    copperNodes.mlh_tail=iter->CH_Node.mlh_tail;
    FreePool(regionPool);
    return 0;
}

/*
** Creates CH_View structure and initializes it 
**   along with the CH_SpriteTable structure referred to by it, if supported.
** Returns pointer to CH_View or NULL if memory allocation fails
*/
struct CH_View *CreateView()
{
    struct MinList *iter;
#ifndef __RTG__
    struct CH_SpriteTable *sprites;
    int spriteCount;
#endif
    struct CH_View *view=(struct CH_View *)AllocMem(MEMF_PUBLIC | MEMF_CLEAR, sizeof(struct CH_View));
    if (NULL==view) return NULL;
    /* Initialize ViewPort list */
    iter=view->CH_ViewPort;
    iter.mlh_tailpred=NULL;
    iter->mlh_head=&iter->mlh_tailpred;
    iter->mlh_tail=&iter->mlh_head;
#ifndef __RTG__
#ifdef __SAGA__
#define NUMSPRITES 16
#else
#define NUMSPRITES 8
#endif
    /* ALlocate and initialize CH_SpriteTable structure */
    sprites=(struct CH_SpriteTable *)AllocMem(
        sizeof(struct CH_SpriteTable)+NUMSPRITES*sizeof(struct MinList),
        MEMF_PUBLIC |MEMF_CLEAR);
    if (NULL==sprites)
    {
        FreeMem(view, sizeof(struct CH_View));
        return NULL;
    }
    sprites->CH_NumSprites=NUMSPRITES;
    view->CH_SpriteTable=sprites;    
    for (spriteCount=NUMSPRITES; spriteCount>0; --spriteCount)
    {
        iter=sprites->CH_SpriteChannel[spriteCount-1];
        iter.mlh_tailpred=NULL;
        iter->mlh_head=&iter->mlh_tailpred;
        iter->mlh_tail=&iter->mlh_head;
    }
#undef NUMSPRITES
#endif /* RTG undef */
    return view;
}

/*
** Creates and initializes CH_ViewPort structure.
** Accepts pointer to CH_ColorTable 
**  which can be NULL if part of a CH_ViewPort group sharing a palette.
** Returns pointer to CH_ViewPort or NULL if allocation fails.
*/
struct CH_ViewPort *CreateViewPort(IPTR *colorTab)
{
    struct CH_ViewPort *vp=AllocMem(sizeof(struct CH_ViewPort), MEMF_PUBLIC | MEMF_CLEAR);
    if (NULL==vp) return NULL;
    vp->CH_ColorTable=colorTab;
    
    return vp;
}
