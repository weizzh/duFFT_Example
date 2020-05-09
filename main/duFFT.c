

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

#include "duFFT.h"

duFFT_Workspace_t duFFT_ResultR[duFFT_INPUT_LENGTH];
duFFT_Workspace_t duFFT_ResultI[duFFT_INPUT_LENGTH];

duFFT_Workspace_t * duFFT_input_buffer_start;
duFFT_Workspace_t * duFFT_input_buffer_end;

void duFFT_Sample(duFFT_Workspace_t * input_buffer)
{
	/* This method copies duFFT_INPUT_LENGTH samples beginning at the address
	pointed by input_buffer to the duFFT_ResultR[] array and also zeros array
	duFFT_ResultI[].  The input data is assumed to be in Q15 format having an
	allowed physical range of +32767 to -32768 corresponding to an arithmetic
	range of +0.999969482421875 and -1 respectively.

	Do not call this method before calling duFFT_Init().

	Provided duFFT_Init() has been called correctly, this method is compatible
	with input data stored in a circular buffer. In this case, if the copying
	process reaches the address pointed by duFFT_input_buffer_end, copying will
	continue from the address pointed by duFFT_input_buffer_start. */

	duFFT_PRIM00S(input_buffer); // Sample from address pointed by input_buffer
}

void duFFT_Window(void)
{
	/* This method applies the mirrored windowing function held in duFFT_win[]
	to the contents of duFFT_ResultR[], saving the result back to duFFT_ResultR[].

	duFFT_ResultR[k] = duFFT_ResultR[k] * duFFT_win[k] when k<duFFT_INPUT_LENGTH/2
	otherwise, duFFT_ResultR[k] = duFFT_ResultR[k] * duFFT_win[duFFT_INPUT_LENGTH-k-1]

	Note. Both operands are interpreted as Q15 format signed integers having an
	allowed physical range of +32767 to -32768 that corresponds to an arithmetic
	range of +0.999969482421875 and -1 respectively.
	
	Therefore, the multiplication result is not subject to overflow, even when
	a rectangular window function is applied to a full range input. In practice,
	the result will be subject to attenuation due to the inherent properties of
	any non-rectangular window function. */

	duFFT_PRIM00W; // Apply pre-calculated windowing function to the data.
}

void duFFT_Reorder(void)
{
	/* This method performs an in-place reordering of the FFT workspace from
	Natural-Order into Bit-Reversed order using the Gold-Rader algorithm. This
	method must be called before calling duFFT_Transform() which requires input
	data to be in bit-reversed order. */

	duFFT_PRIM00R; // Reorder data using the Gold-Rader Bit Reverse Algorithm.
}

void duFFT_Transform(void)
{
	/* This method calculates an in-place Discrete Fourier Transform using the
	Cooley-Tukey Decimation-In-Time (DIT) Radix-2 algorithm. Input data must be
	in bit-reversed order and may be Real or Complex. Output values are complex
	and are in Natural order. I.e.

	Frequency Bins 0(DC), 1, 2 ... etc = Result[0], Result[1], Result[2] ... etc

	In-Place computation means the input (stored in duFFT_ResultR, duFFT_ResultI)
	is overwritten during the calculation process.

	Note. Input values are interpreted as Q15 format signed integers having an
	allowed physical range of +32767 to -32768 corresponding to an arithmetic
	range of +0.999969482421875 and -1 respectively.  The DFT result is scaled
	by 1/duFFT_INPUT_LENGTH during the calculation process to prevent saturation
	and to ensure the output fits within the original storage type.

	No post-processing other than the above scaling is applied by this method.
	The result is left in complex form and may subsequently be converted to
	Magnitude or Phase etc by application software. */

	duFFT_PRIM00T; // Cooley-Tukey DIT Radix-2 Discrete Fourier Transform.
}

void duFFT_CalculateDft(duFFT_Workspace_t * input_buffer)
{
	/* This method performs all the required steps to calculate a Discrete
	Fourier Transform (DFT) including ...

	1.	Sampling data from the address specified by input_buffer to the FFT
		workspace, with buffer wrapping as required.
	2.	Applying the specified windowing function prior to DFT calculation.
	3.	Arranging the input samples into the required bit-reverse order.
	4.	Calculation of the DFT using a Cooley-Tukey DIT Radix-2 Algorithm. */

	duFFT_Sample(input_buffer);
	duFFT_Window();
	duFFT_Reorder();
	duFFT_Transform();
}

void duFFT_Init(duFFT_Workspace_t * input_buffer_start, int input_buffer_length)
{
	/* This method must be called (once) before duFFT_Sample() or duFFT_Calculate()
	so that duFFT_input_buffer_start and duFFT_input_buffer_end can be initialised.
	
	These pointers define the allowed range of addresses from which data can be
	copied into the FFT workspace. Once they have been initialised, duFFT_Sample()
	can check that data is being copied from the allowed input address range and
	wrap the copy pointer back to the start as required.  This allows the use of
	a circular input buffer from which the input data for the FFT can be sampled
	at an arbitrary offset. */

	duFFT_input_buffer_start = input_buffer_start;
	duFFT_input_buffer_end = input_buffer_start + input_buffer_length;
}
