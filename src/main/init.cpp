#include <cstdint>

#include "platform.h"

/* CRT init function, called by __libc_init_array() before passing control to main */
extern "C" void __attribute__ ((weak)) _init(void)
{
}

/* Prevent demangle from being pulled in */
extern "C" void __cxa_pure_virtual(void)
{
    while (true);
}
