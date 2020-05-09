
/******************************************************************************


        !  DO NOT EDIT THIS FILE AS IT MAY BE OVERWRITTEN BY
        THE DESIGNER RESULTING IN YOUR CHANGES BEING LOST  !

DISTRIBUTION.  This software may NOT be distributed to a third party without
prior written approval of the copyright holder. Please see the FFT Designer
End User Licence Agreement for details about distribution in object-code form.

DISCLAIMER.  This software is provided "AS IS", without express or implied
warranty.  In no event shall the copyright holder, author(s) or provider(s)
of this software be held liable for any damages arising from its use.

        FFTLength ................................ 1024
        WindowType ............................... Kaiser
        WindowLoss (dB) .......................... 7.91
        WindowAlpha .............................. 3
        Language ................................. Standard ANSI C
        Algorithm ................................ Integer (16 Bit)
        Use Shift Division ....................... False

******************************************************************************/

#include "duFFT_Include.h"

// export lookup tables
extern const duFFT_Workspace_t duFFT_win[];
extern const duFFT_Workspace_t duFFT_twr[];
extern const duFFT_Workspace_t duFFT_twi[];