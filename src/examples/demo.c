/*
** Demonstration of VideoFX.library
*/

#include <exec/types.h>)
#define __AGA__
#include <videofx/copperview.h>>
#include <videofx/spriteport.h>>

#include <proto/videofx.h>

INITCOPSERIES(series1, 0, 0, CSF_EVEN|CSF_ODD, NULL)
    VFX_CopColor(0,48,48,48) /* set color index 0 to grey */
ENDCOPSERIES

int main(int argc, char *argv[])
{

    return 0;
}