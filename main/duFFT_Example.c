

/******************************************************************************


        !  DO NOT EDIT THIS FILE AS IT MAY BE OVERWRITTEN BY
        THE DESIGNER RESULTING IN YOUR CHANGES BEING LOST  !

DISTRIBUTION.  This software may NOT be distributed to a third party without
prior written approval of the copyright holder. Please see the FFT Designer
End User Licence Agreement for details about distribution in object-code form.

DISCLAIMER.  This software is provided "AS IS", without express or implied
warranty.  In no event shall the copyright holder, author(s) or provider(s)
of this software be held liable for any damages arising from its use.

******************************************************************************/

#include "duFFT.h"
#include "duFFT_TestHarness.h"

#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"


/* Note. This code is for ANSI C compilers compliant with the C99 standard.
Non-compliant compilers should not be used as compilation may fail or the
performance simulated by the designer may not be achieved. */

// Define the length of duFFT_TestBuffer. Must be >= duFFT_INPUT_LENGTH
#define duFFT_TEST_BUFFER_LENGTH		duFFT_INPUT_LENGTH

/* Declare array duFFT_TestBuffer[] as a fixed-width 16 bit signed type.  Note.
duFFT.h will include the C99 header <stdint.h> where these types are defined. */

duFFT_Workspace_t duFFT_TestBuffer[duFFT_TEST_BUFFER_LENGTH];

void app_main(void)
{
	volatile static int ErrorCount=0; // Note. static used for easier debugging

	// Example 1.  Calculate a DFT using input data from duFFT_TestBuffer[]

	/* First, array duFFT_TestBuffer[] is filled with test data. In a typical
	application, this might be done using an interrupt service routine, for
	example, with ADC values being read at regular intervals using a timer.
	
	However, in this example, duFFT_TEST_BUFFER_LENGTH = duFFT_INPUT_LENGTH, so
	the buffer can be filled directly by copying data from the tables provided
	by duFFT_TestHarness.c. The advantage of using the test tables is that once
	the DFT has been calculated, it can be checked against a known result. */

	int i;
	for(i=0; i<duFFT_TEST_BUFFER_LENGTH; i++)
	{
		duFFT_TestBuffer[i]=duFFT_SampledR[i]; // fill with known data
	}
	
	/* Next, duFFT_Init() is called with a pointer to duFFT_TestBuffer[]
	along with the array length. This allows the FFT methods to detect when
	pointers increment past the end of duFFT_TestBuffer[] and wrap them back to
	the start.
	
	The duFFT_Init() method only needs to be called once after a reset, or when
	the address range of the input buffer supplying data to the FFT changes.
	
	In effect, this allows continuous writing to array duFFT_TestBuffer[] in a
	circular fashion. At the same time it allows the FFT to be calculated at
	an arbitrary offset.
	
	Note. FFT sampling (copying data from the duFFT_TestBuffer[] to the FFT
	workspace duFFT_ResultR[] and duFFT_ResultI[]) only takes around 3% of the
	overall FFT calculation time and is carried out in a forward direction
	beginning at the specified offset. Therefore, for moderate acquisition
	rates, a duFFT_TestBuffer[] of equal length to the FFT input length may be
	sufficient to allow continuous acquisition. If higher data acquisition
	rates are required, the length of the duFFT_TestBuffer[] can be extended
	as required until there are enough CPU cycles in-hand to overcome any
	latency in the FFT sampling process. */

	duFFT_Init(duFFT_TestBuffer, duFFT_TEST_BUFFER_LENGTH); // setup FFT input range
	
	/* Only after duFFT_Init() has been called may duFFT_CalculateDft() be called.
	As mentioned, duFFT_CalculateDft() can perform a DFT from an arbitrary point
	within duFFT_TestBuffer[] by passing it the required start address.
	
	Data from the duFFT_TestBuffer[] is then sampled, beginning at the specified
	start address and wrapping back through the buffer start as required.
	
	E.g.  If the length of duFFT_TestBuffer[] is 128 samples and the FFT input
	length is 32 samples. Calling duFFT_CalculateDft(duFFT_TestBuffer+124) would
	cause the first 4 samples to be read from offsets 124, 125, 126 and 127.
	The duFFT_Sample() method would then detect that it's internal copy pointer
	had reached the address pointed by duFFT_input_buffer_end. The copy pointer
	would then be wrapped back to the address pointed by duFFT_input_buffer_start
	and the remaining input samples would be read from offsets 0,1,2,3... etc
	with the last sample being read from offset 27.
	
	This system allows the data acquisition process to be asynchronous with
	respect to DFT calculation.  A DFT result can thus be obtained whenever
	and as often as required, allowing for efficient overlapping and averaging
	algorithms as well as improved compatibility with multi-task environments.
	
	In this example, a single DFT will be calculated at zero offset within the
	array duFFT_TestBuffer[]. Calling duFFT_CalculateDft() initiates all required
	processes according to the specified FFT design. */

	duFFT_CalculateDft(duFFT_TestBuffer+0); // Calculate DFT at zero buffer offset
	
	/* Alternatively, if an FFT is required from say half way along the buffer,
	the call to duFFT_CalculateDft() would be as follows.

		duFFT_CalculateDft(duFFT_TestBuffer+duFFT_TEST_BUFFER_LENGTH/2);
	
	When duFFT_CalculateDft() has finished, the DFT output will be available in
	duFFT_ResultR[] and duFFT_ResultI[] corresponding to the Real and Imaginary
	parts of each complex result.
	
	In most cases the DFT result will need converting into a different format
	before use. E.g. Magnitude. This might also be done in-place, overwriting
	the complex DFT result, so no extra storage would be needed ....
	
		duFFT_ResultR(k) = Sqrt( duFFT_ResultR(k)^2 + duFFT_ResultI(k)^2 )
		where k = 0, 1, 2, 3, ..... duFFT_INPUT_LENGTH-1
	
		Note. If creating an audio spectrum-analyser type display, for example
		to be shown on a graphical LCD, only the first duFFT_INPUT_LENGTH/2+1
		results need be calculated. These correspond to DC and all frequencies
		up to and including Nyquist, spaced SampleRate/duFFT_INPUT_LENGTH Hz apart.
	
	On an integer processor, the final precision and dynamic range required by
	the application can drastically influence how result conversion can best
	be accomplished.  For example, in the above example it may not be necessary
	to calculate the square root if the intention is to later convert the result
	to a dB value using a lookup-table or when all that is required is a value
	proportional to signal power rather than an absolute result.
	
	To finish this example, the DFT result is checked against pre-calculated
	complex values by calling duFFT_TestHarnessCompareTransformed(). This method
	returns the number of errors found. There should be none! */

	ErrorCount=duFFT_TestHarnessCompareTransformed();	
	printf("ErroCount=%d", ErrorCount);
	if(ErrorCount)
	{
		ErrorCount=ErrorCount; // set a break point here to check for errors
		printf("ErroCount=%d", ErrorCount);
	}

	// Example 2.  Calling individual methods to calculate the DFT step-by-step.
	duFFT_TestHarnessLoadSampled(); // load FFT workspace with test data.
	duFFT_Window();						// apply specified windowing function.
	duFFT_Reorder();						// arrange data in bit-reverse format.
	duFFT_Transform();					// calculate an in-place DFT of data.
	// check the complex FFT result against the expected values.
	ErrorCount=duFFT_TestHarnessCompareTransformed();
	printf("ErroCount=%d", ErrorCount);
	if(ErrorCount)
	{
		ErrorCount=ErrorCount; // set a break point here to check for errors
		printf("ErroCount=%d", ErrorCount);
	}
    for (int i = 10; i >= 0; i--) {
        printf("Restarting in %d seconds...\n", i);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    printf("Restarting now.\n");
    fflush(stdout);
    esp_restart();
}
