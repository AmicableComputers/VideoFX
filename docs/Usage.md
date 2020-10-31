***VideoFX Usage ***

This library is designed to supplement Graphics.library on AmigaOS 3.x and AROS 68k (otherwise distributed as ApolloOS).

**Preprocessor Macros**

The following preprocessor macros should be defined, one at a time, to identify the least capable Amiga chipset supported by the program using this library.

* \_\_OCS\_\_ is the original chipset

* \_\_ECS\_\_ is the enhanced chipset

* \_\_AGA\_\_ is the advanced graphics architecture

* \_\_SAGA\_\_ is the super-advanced graphics architecture supported by the Apollo Vampire series computer

These macros must be defined before the headers of this library are included.  Otherwise \_\_OCS\_\_ will be assumed.  If capabilities vary from one chipset to the next, be sure to guard dependent code with #ifdef directives to unlock the potential of the current level of capability and compatibility.  For example, if \_\_SAGA\_\_ is enabled you will have all 16 spirtes while \_\_AGA\_\_ supports only 8 but they still can be as wide as 64 pixels.  \_\_ECS\_\_ and \_\_OCS\_\_ limit the sprite width to 16 pixels.