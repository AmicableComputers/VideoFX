/* Sort CopperSeries structures by coordinate position of the start */

#include <videofx/copperview.h>

void swap(struct VFX_CopperSeries **a, struct VFX_CopperSeries **b)
{
	struct VFX_CopperSeries *temp = *a;
	*a = *b;
	*b = temp;
}

void heapify(struct VFX_CopperSeries **range, UWORD n, ULONG i)
{
	/* Find largest among root, left child and right child */
	ULONG largest = i;
	UWORD left = i + i + 1;
	UWORD right = i + i + 2;

	if (left < n && range[left]->VFX_CS.VFX_Item > range[largest]->VFX_CS.VFX_Item)
	{
		largest = left;
	}

	if (right < n && range[right]->VFX_CS.VFX_Item > range[largest]->VFX_CS.VFX_Item)
	{
		largest = right;
	}

	/* Swap and continue heapifying if root is not largest */
	if (largest != i)
	{
		swap(&range[i], &range[largest]);
		heapify(range, n, largest);
	}
}

/*
** Sorts all VFX_Series structures in a view
** Returns 0 on success, 1 on empty region list
** Result left in copperNodes
*/
UWORD CopSort(struct VFX_SeriesList *list)
{
	if (list->NumSeries==0) return 1;
	/* Build max heap */
	for (UWORD i = list->NumSeries / 2 - 1; i >= 0; i--)
	{
		heapify(&list->Entries[0], list->NumSeries, i);
	}

	/* Heap sort */
	for (UWORD i = list->NumSeries - 1; i >= 0; i--)
	{
		swap(&list->Entries[0], &list->Entries[i]);

		// Heapify root element to get highest element at root again
		heapify(list->Entries, i, 0);
	}
	return 0;
}
