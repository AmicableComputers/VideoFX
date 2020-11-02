***VideoFX Usage***

This library is designed to supplement Graphics.library on AmigaOS 3.x and AROS 68k (otherwise distributed as ApolloOS).

**Chipset Indication Macros**

The following preprocessor macros should be defined, one at a time, to identify the least capable Amiga chipset supported by the program using this library.

* \_\_OCS\_\_ is the original chipset

* \_\_ECS\_\_ is the enhanced chipset

* \_\_AGA\_\_ is the advanced graphics architecture

* \_\_SAGA\_\_ is the super-advanced graphics architecture supported by the Apollo Vampire series computer

These macros must be defined before the headers of this library are included.  Otherwise \_\_OCS\_\_ will be assumed.  If capabilities vary from one chipset to the next, be sure to guard dependent code with #ifdef directives to unlock the potential of the current level of capability and compatibility.  For example, if \_\_SAGA\_\_ is enabled you will have all 16 spirtes while \_\_AGA\_\_ supports only 8 but they still can be as wide as 64 pixels.  \_\_ECS\_\_ and \_\_OCS\_\_ limit the sprite width to 16 pixels.

**Copper Series**

A node in a Copper Series adds a single wait unstruction (or more if wraparound conditions require it) followed by one or more move instructions that will be copied into the copper list buffer of the current view structure.  To initiate a Copper Series invoke the INITCOPSERIES macro.  An example using the macro with the CopWord macro is as follows:

> INITCOPSERIES(nodeName, x, y, flags, renderHook)\
> CopWord(regname, value)\
> ENDCOPSERIES

As indicated in the previous example, the CopWord macro inserts a word move into the series.  Additional macros include CopLong and CopColor.  The advantage of these macros over just feeding in the values manually is that different levels of capability are atainable using the same source code but defining a different chipset indication macro.

The classic example of this is the CopColor macro which generates a 12-bit color change on OCS and ECS but uses a full 24-bit color change on AGA and SAGA.  Another example is the fact that the SAGA version of the CopLong instruction does a single long write rather than 2 word writes as in the older chipsets.

Expect additional macro functions for sub-pixel-precision scrolling on AGA and SAGA.