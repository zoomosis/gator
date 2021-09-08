#include "coreleft.h"

unsigned long coreleft(void)
{
    unsigned short npara;

    /* Request impossibly large number of 16-byte paragraphs from DOS */

    _asm
    {
        mov ah, 48h
        mov bx, 0FFFFh
        int 21h
        mov [npara], bx
    }

    return (unsigned long) npara * 16L;
}
