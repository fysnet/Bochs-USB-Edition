/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015, 2016, 2017 The Regents of the
University of California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "specialize.h"
#include "primitives.h"
#include "softfloat.h"

uint8_t f16_to_ui8(float16 a, uint8_t roundingMode, bool exact, bool saturate, struct softfloat_status_t *status)
{
    bool sign;
    int8_t exp;
    uint16_t frac;
    int shiftDist;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sign = signF16UI(a);
    exp  = expF16UI(a);
    frac = fracF16UI(a);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if ((exp == 0x1F) && frac) {
        softfloat_raiseFlags(status, softfloat_flag_invalid);
        return saturate ? 0 : ui8_fromNaN;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    shiftDist = 0x16 - exp;
    if (shiftDist < 0) {
        const uint8_t NegOverflowResponse = saturate ? ui8_minValue : ui8_fromNegOverflow;
        const uint8_t PosOverflowResponse = saturate ? ui8_maxValue : ui8_fromPosOverflow;

        softfloat_raiseFlags(status, softfloat_flag_invalid);
        return sign ? NegOverflowResponse : PosOverflowResponse;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (exp) frac = (frac | 0x0400) << 5;
    else if (softfloat_denormalsAreZeros(status)) return 0;
    if (0 < shiftDist) frac = softfloat_shiftRightJam32(frac, shiftDist);
    return softfloat_roundToUI8(sign, frac, roundingMode, exact, saturate, status);
}
