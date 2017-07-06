// Dolengewicz, Okoro
// EE443 HW1

#include "DSP_Config.h"


#define LEFT 0
#define RIGHT 1

double outGainA = 0;
double outGainB = 1;
double gainSlope = (1.0/720.0);
double decay = .5;
double inputGain = .8;
double outputGain = .8;

int ii = 0;

short delayStep = 1;
short delayValA = 720;
short delayValB = 360;
int GUI_Out;
short GUI_Out_Input;
int bufferSize = 12000;
int audioBuffer[12000];
short output = 0;

double gainVector[1440];

short switchState = 0;


int main()

{
	// initialize DSP board
	DSP_Init();
	// call StartUp for application specific code
	// defined in each application directory
	StartUp();

	for(ii = 0;ii < bufferSize;ii++) {
		audioBuffer[ii] = 0;
	}

	for(ii = 0; ii < 720; ii++)
		gainVector[ii] = 1 - ii*gainSlope;
	for(ii = 720; ii < 1440; ii++)
		gainVector[ii] = (ii-720)*gainSlope;

	ii = 0;

	while(1) {

		switchState = ReadSwitches();

		WriteLEDs(switchState);

	}
}

