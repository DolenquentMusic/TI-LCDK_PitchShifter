// Dolengewicz, Okoro
// EE443 HW2


#include "DSP_Config.h" 
  
// Data is received as 2 16-bit words (left/right) packed into one
// 32-bit word.  The union allows the data to be accessed as a single 
// entity when transferring to and from the serial port, but still be 
// able to manipulate the left and right channels independently.

#define LEFT  0
#define RIGHT 1

volatile union {
	Uint32 UINT;
	Int16 Channel[2];
} CodecDataIn, CodecDataOut;


/*Global Vars*/
extern double outGainA;
extern double outGainB;
extern double gainSlope;
extern double decay;
extern double inputGain;
extern double outputGain;

extern int ii;

extern short delayStep;
extern short delayValA;
extern short delayValB;
extern int GUI_Out;
extern short GUI_Out_Input;
extern int bufferSize;
extern int audioBuffer[];
extern short output;

extern double gainVector[];

extern short switchState;

/*Locol Vars*/

int delayIdxA = 0;
int delayIdxB = 0;
short downCount = 0;
short gainIdx = 0;

interrupt void Codec_ISR()
{                    

 	if(CheckForOverrun())					// overrun error occurred (i.e. halted DSP)
		return;								// so serial port is reset to recover

   	CodecDataIn.UINT = ReadCodecData(); // get input data samples
   	//CodecDataIn.Channel[LEFT] = CodecDataIn.Channel[LEFT] >> 1;
    //CodecDataIn.Channel[RIGHT] = CodecDataIn.Channel[RIGHT] >> 1;

   	GUI_Out_Input = CodecDataIn.Channel[LEFT];

   	if(switchState & 1) { //no shift
   		CodecDataOut.UINT = CodecDataIn.UINT;
   		audioBuffer[ii] = 0;

   		downCount = -1;

   	} else{ //if shift

   		if(switchState & (1 << 1)) { //shift down if SW1-6 is down

   			downCount++;
   			if(downCount == 0)
   			{
   				outGainA = 1;
   				outGainB = 0;
   				delayValA = 0;
   				delayValB = 0;
   				gainIdx = 0;
   				delayStep = 0;
   			}

			delayValA = delayValA + delayStep;
			if(delayValA > 719) delayValA = 0;

			if(downCount > 359) {

				downCount = 720;
				delayValB = delayValA-360;
				if(delayValB < 0) delayValB += 720;

				outGainA = gainVector[gainIdx++];
				if(gainIdx >= 719) gainIdx = 0;

				outGainB = 1 - outGainA;

			}

			if(delayStep == 1) delayStep = 0;
			else delayStep = 1;

			delayIdxA = ii - delayValA;
			delayIdxB = ii - delayValB;

			if (delayIdxA < 0)  delayIdxA = delayIdxA + bufferSize;
			if (delayIdxB < 0)  delayIdxB = delayIdxB + bufferSize;


		  	audioBuffer[ii] = CodecDataIn.Channel[LEFT];


		  	CodecDataOut.Channel[RIGHT] = 0;
		  	output = outGainA*audioBuffer[delayIdxA] + outGainB*audioBuffer[delayIdxB];
		  	CodecDataOut.Channel[LEFT]  = output;

		  	audioBuffer[ii] = CodecDataIn.Channel[LEFT];

   		}
   		else { //else shift up

   			if(downCount != -1) {
   				downCount = -1;
   				gainIdx = 0;
   				delayValA = 720;
   				delayValB = 360;
   				delayStep = 0;
   			}

			delayValA = delayValA - delayStep;
			if(delayValA < 0) delayValA += 720;

			delayValB = delayValA - 360;
			if(delayValB < 0) delayValB += 720;

			if(delayStep == 1) delayStep = 0;
			else delayStep = 1;

			outGainA = gainVector[gainIdx++];
			if(gainIdx >= 720) gainIdx = 0;

			outGainB = 1 - outGainA;

			delayIdxA = ii - delayValA;
			delayIdxB = ii - delayValB;

			if (delayIdxA < 0)  delayIdxA += bufferSize;
			if (delayIdxB < 0)  delayIdxB += bufferSize;

			CodecDataOut.Channel[RIGHT] = 0; //CodecDataIn.Channel[RIGHT];
			output = outGainA*audioBuffer[delayIdxA] + outGainB*audioBuffer[delayIdxB];
			CodecDataOut.Channel[LEFT]  = output;


		  	audioBuffer[ii] = CodecDataIn.Channel[LEFT];

   		}

 		ii++;

 		if(ii > bufferSize)	ii = 0;
   	}

  	GUI_Out = CodecDataOut.Channel[LEFT];
  	WriteCodecData(CodecDataOut.UINT);
}

