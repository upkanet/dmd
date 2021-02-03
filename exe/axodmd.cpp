/* Axorus DMD Player for Vialux DMD */
#include <windows.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "alp.h"
#include <iostream>
#include <string>
#include <csignal>

void signalHandler( int signum ) {
   std::cout << "Interrupt signal (" << signum << ") received.\n";

   // cleanup and close up stuff here  
   // terminate program  

   exit(signum);  
}

int main(int argc, const char **argv)
{
	//Register Signal handler
	signal(SIGINT, signalHandler);

	ALP_ID nDevId, nSeqId1, nSeqId2;
	long nDmdType, nSizeX, nSizeY;
	long nReturn;
	long nRepeat = 1; // repetiton of each seq
	//printf(AlpProjInquire(ALP_ID DeviceId, long InquireType, long *UserVarPtr));
	long nOffset = 1024;//1024;	// divide the frame LUT in 2
	//	const long nPictureTime = 0.016600*1000000;	// in �s, prompted from now on
	long nBit = 1;

	FILE *file;
	const char *filename, *filevec;
	UCHAR *block1;
	tFlutWrite LutArray;
	tAlpProjProgress QueueInfo;

	using namespace std;

	cout << "=== Axorus DMD Player ===\n\n";

	cout << "\nALP initialization\n";
	
	// Allocate the ALP high-speed device
	if (ALP_OK != AlpDevAlloc(ALP_DEFAULT, ALP_DEFAULT, &nDevId)) {
		cout << "ALP - AlpDevAlloc impossible - ERROR : "+to_string(AlpDevAlloc(ALP_DEFAULT, ALP_DEFAULT, &nDevId))+"\n";
		//return 1;
	}

	//SD : Reverses black into white. Caution : takes two frames to be complete
	//if (ALP_OK != AlpProjControl(nDevId, ALP_PROJ_INVERSION, 1)) printf("errorProjInv\n");

	// Inquire DMD type
	if (ALP_OK != AlpDevInquire(nDevId, ALP_DEV_DMDTYPE, &nDmdType)){
		cout << "ALP - AlpDevInquire impossible - ERROR : "+to_string(AlpDevInquire(nDevId, ALP_DEV_DMDTYPE, &nDmdType))+"\n";
		//return 1;
	}	
	switch (nDmdType) {
	case ALP_DMDTYPE_XGA_055A:
	case ALP_DMDTYPE_XGA_055X:
	case ALP_DMDTYPE_XGA_07A:
		nSizeX = 1024; nSizeY = 768;
		break;
	case ALP_DMDTYPE_DISCONNECT:
	case ALP_DMDTYPE_1080P_095A:
		nSizeX = 1920; nSizeY = 1080;
		break;
	case ALP_DMDTYPE_WUXGA_096A:
		nSizeX = 1920; nSizeY = 1200;
		break;
	default:
		// unsupported DMD type
		cout << "ALP - Unknown device - DMD TYPE : "+to_string(nDmdType)+"\n";
		//return 1;
	}

	cout << "\n";

	///////////////////////////////////////////////////////////////////////////////////////////////////

	//Prompt .bin and .vec filenames

	cout << "Loading parameters\n";

	string bin_name;
	string vec_name;
	if(argc > 1){
		string argpath;
		argpath += argv[1];
		bin_name = argpath + "/flash.bin";
		vec_name = argpath + "/flash.vec";
	}
	else{
		bin_name = "bin/flash.bin";
		vec_name = "bin/flash.vec";
	}

	//prompt Frequency of refresh
	char *ptrToDouble;
	double rate;
	if(argc > 2){
		rate = strtod(argv[2],&ptrToDouble);
	}
	else{
		rate = 1000;
	}

	double gray_rate;
	if(argc > 3){
		gray_rate = strtod(argv[3],&ptrToDouble);
	}
	else{
		gray_rate = 1000;
	}

	//SD : the number of repeats of each frame is calculated to assure a gray level refreshed at least at gray_rate Hz.
	long nbRepeats = pow(2, ceil(log((double)(gray_rate / rate)) / log((double)2)));
	const long nPictureTime = 1.0 / (rate*nbRepeats) * 1000000;

	double bitDepth;
	if(argc > 4){
		bitDepth = strtod(argv[4],&ptrToDouble);
		if(bitDepth > 8) bitDepth = 8;
	}
	else{
		bitDepth = 8;
	}
	nBit = bitDepth;

	cout << ".bin file path : " + bin_name + "\n";
	cout << ".vec file path : " + vec_name + "\n";
	printf("Rate : %.0f Hz\n",rate);
	printf("Gray Rate : %.0f Hz\n", gray_rate);
	printf("Bit depth : %.0f\n", bitDepth);

	cout << "\n";

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Load frames

	// filename
	filename = bin_name.c_str();

	// check valid file
	file = fopen(filename, "r");
	if (file == NULL)
	{
		printf("Invalid file name");
	}

	// read header
	short header[4];
	fread(header, sizeof(short), 4, file);
	printf("Header SizeX = %d, SizeY =  %d, NbFrames = %d\n", header[0], header[1], header[2]);
	fclose(file);

	printf("\nThe BIN buffer allocation\n");
	// SD: the display window will be placed at the middle of the DMD screen
	//nBit = 8;
	int nFrames = header[2];
	int width = header[0];
	int height = header[1];

	// allocate buffer
	block1 = NULL;
	block1 = (UCHAR *)calloc(nFrames*nSizeX*nSizeY, 1);
	if (block1 == NULL)
	{
		printf("Failed to allocate buffer. \n");
	}

	printf("[OK] BIN buffer allocated\n");
	//////////

	// SD: coordinates of the display window
	int beg_w = (nSizeX - width) / 2;
	int beg_h = (nSizeY - height) / 2;


	// Read file
	file = fopen(filename, "r");
	printf("[OK] BIN file opened\n");

	fseek(file, 8, 0);
	printf("[OK] BIN file reading\n");


	//SD: Centering frames in the middle of the screen
	// buffer
	int header_size = sizeof(short)* 4;
	// read file
	for (int i = 0; i<nFrames; i++)
	{
		fseek(file, i * width*height + header_size, SEEK_SET);
		// SD: the display window is located at the center of the screen of the DMD
		for (int line = 0; line < height; line++){
			fread(block1 + i*(nSizeX*nSizeY) + (beg_h + line)*nSizeX + beg_w, 1, width, file);
		}
	}

	fclose(file);
	printf("[OK] BIN file closed\n");


	////////////////////////////////////////////////////////////////////////////////////////////
	//Read Vec file
	printf("\nVEC file analysis\n");

	long int temp1, temp2, temp3, temp4, temp5;

	filevec = vec_name.c_str();

	long int numberOfVecFrames = 0;

	file = fopen(filevec, "r");
	if (file == NULL) {
		printf("Invalid file name. \n");
	}
	printf("[OK] VEC file opened\n");

	fscanf(file, "%ld %ld %ld %ld %ld", &temp1, &temp2, &temp3, &temp4, &temp5);
	while (fscanf(file, "%ld %ld %ld %ld %ld", &temp1, &temp2, &temp3, &temp4, &temp5) != EOF)
	{
		numberOfVecFrames++;
	}
	fclose(file);

	printf("# of frames to be played : %d\n", numberOfVecFrames);

	//SD: Modify slightly nOffset to take into account the gray level repetitions 
	//nOffset = nOffset - (nOffset % nbRepeats);
	long Nloop, Nremain;
	//SD: Nloop takes into account the gray level repetitions
	Nloop = (numberOfVecFrames * nbRepeats) / (nOffset * 2);
	Nremain = (numberOfVecFrames * nbRepeats) % (nOffset * 2);

	printf("Nloop = %d, Nremain = %d \n", Nloop, Nremain);

	///////////////////////////////////////////////////////////////////////
	// Fill the LUT

	LutArray.nSize = nOffset;
	LutArray.nOffset = 0;
	long i;


	file = fopen(filevec, "r");

	fscanf(file, "%ld %ld %ld %ld %ld", &temp1, &temp2, &temp3, &temp4, &temp5);

	for (i = 0; i < nOffset; i = i + nbRepeats)
	{
		fscanf(file, "%ld %ld %ld %ld %ld", &temp1, &temp2, &temp3, &temp4, &temp5);
		// SD : the gray level refresh rate is guaranteed manually by introducing repetitions of the same frame.
		for (int j = 0; j < nbRepeats; j++)
		{
			LutArray.FrameNumbers[i + j] = temp2;
		}
		//printf("%ld ", temp2);
	}

	AlpProjControlEx(nDevId, ALP_FLUT_WRITE_18BIT, &LutArray);

	// Pour passer l'adresse de la structure � la fonction
	// cela �vite de copier la structure (qui est grosse) quand on
	// appelle la fonction */

	printf("[OK] Lut transfer\n");

	///////////////////////////////////////////////////////////////////////////
	// SEQ PARAMETERS

	long nframesavail;

	if (ALP_OK != AlpDevInquire(nDevId, ALP_AVAIL_MEMORY, &nframesavail)) return 1;
	printf("Memory 1 = %d\n", nframesavail);

	// Allocate two sequences of nFrames
	if (ALP_OK != AlpSeqAlloc(nDevId, nBit, nFrames, &nSeqId1)) printf("errorSeqAlloc\n");
	if (ALP_OK != AlpSeqAlloc(nDevId, nBit, nFrames, &nSeqId2)) printf("errorSeqAlloc\n");
	printf("alloc OK nFrames = %ld \n", nFrames);

	if (ALP_OK != AlpDevInquire(nDevId, ALP_AVAIL_MEMORY, &nframesavail)) return 1;
	printf("Memory 2 = %d\n", nframesavail);


	//SD We reduce the bitdepth of the display and we suppress the Dark Phase
	ALP_OK != AlpSeqControl(nDevId, nSeqId1, ALP_BITNUM, 1);
	ALP_OK != AlpSeqControl(nDevId, nSeqId1, ALP_BIN_MODE, ALP_BIN_UNINTERRUPTED);
	ALP_OK != AlpSeqControl(nDevId, nSeqId2, ALP_BITNUM, 1);
	ALP_OK != AlpSeqControl(nDevId, nSeqId2, ALP_BIN_MODE, ALP_BIN_UNINTERRUPTED);

	// Set timing
	if (ALP_OK != AlpSeqTiming(nDevId, nSeqId1, ALP_DEFAULT, nPictureTime,
		ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT)) return 1;

	printf(" nPictureTime = %ld \n", nPictureTime);
	printf("One sequence loaded\n");

	if (ALP_OK != AlpSeqTiming(nDevId, nSeqId2, ALP_DEFAULT, nPictureTime,
		ALP_DEFAULT, ALP_DEFAULT, ALP_DEFAULT)) return 1;
	printf("timing OK \n");

	// Set repeat
	if (nRepeat>0 && ALP_OK != AlpSeqControl(nDevId, nSeqId1, ALP_SEQ_REPEAT, nRepeat))
	{
		printf("error (ALP_SEQ_REPEAT).\r\n");
		return 1;
	}
	if (nRepeat>0 && ALP_OK != AlpSeqControl(nDevId, nSeqId2, ALP_SEQ_REPEAT, nRepeat))
	{
		printf("error (ALP_SEQ_REPEAT).\r\n");
		return 1;
	}
	printf("repeat OK \n");

	// Set FLUT parameters
	//Seq1
	if (ALP_OK != AlpSeqControl(nDevId, nSeqId1, ALP_FLUT_MODE, ALP_FLUT_18BIT))
	{
		printf("error (ALP_FLUT_MODE).\r\n");
		return 1;
	}

	if (ALP_OK != AlpSeqControl(nDevId, nSeqId1, ALP_FLUT_ENTRIES9, nOffset * 2))
	{
		printf("error (ALP_FLUT_ENTRIES).\r\n");
		return 1;
	}

	//Seq2
	if (ALP_OK != AlpSeqControl(nDevId, nSeqId2, ALP_FLUT_MODE, ALP_FLUT_18BIT))
	{
		printf("error (ALP_FLUT_MODE).\r\n");
		return 1;
	}

	if (ALP_OK != AlpSeqControl(nDevId, nSeqId2, ALP_FLUT_ENTRIES9, nOffset * 2))
	{
		printf("error (ALP_FLUT_ENTRIES).\r\n");
		return 1;
	}//

	if (ALP_OK != AlpSeqControl(nDevId, nSeqId2, ALP_FLUT_OFFSET9, nOffset * 2))
	{
		printf("error (ALP_FLUT_OFFSET).\r\n");
		return 1;
	}
	printf("offset ok\n");



	///////////////////////////////////////////////////////////////////////////////////
	//Set up queue mode
	if (ALP_OK != AlpProjControl(nDevId, ALP_PROJ_QUEUE_MODE, ALP_PROJ_SEQUENCE_QUEUE)) return 1;



	// Transmit images into ALP memory
	nReturn = AlpSeqPut(nDevId, nSeqId1, ALP_DEFAULT, ALP_DEFAULT, block1);
	printf("transmit 1 ok \n");
	printf(" nreturn = %d\n", nReturn);
	printf(" bloc 1 = %d\n", block1);
	nReturn = AlpSeqPut(nDevId, nSeqId2, ALP_DEFAULT, ALP_DEFAULT, block1);
	printf(" nreturn = %d\n", nReturn);
	printf(" bloc 1 = %d\n", block1);
	printf(" invalid = %d\n", ALP_ADDR_INVALID);
	printf(" invalid = %d\n", ALP_SEQ_IN_USE);
	printf(" invalid = %d\n", ALP_PARM_INVALID);
	free(block1);
	if (ALP_OK != nReturn) return 1;
	printf("Transmit OK. \n");

	long j;
	int ts = 30;
	for (j = 1; j <= Nloop; j++)
	{
		printf("j = %d. \n", j);

		// Start Sequence 1 in queue mode
		if (ALP_OK != AlpProjStart(nDevId, nSeqId1)) return 1;
		printf("Seq1 \n");

		// Wait until Seq 1 is started to rewrite the LUT and reset the queue
		AlpProjInquireEx(nDevId, ALP_PROJ_PROGRESS, &QueueInfo);

		while (QueueInfo.SequenceId == 2)
		{
			AlpProjInquireEx(nDevId, ALP_PROJ_PROGRESS, &QueueInfo);
			Sleep(ts);
		}

		AlpProjControl(nDevId, ALP_PROJ_RESET_QUEUE, ALP_DEFAULT);
		printf("Seq 2 Finished . \n");

		// Rewrite the LUT part 2
		LutArray.nSize = nOffset;
		LutArray.nOffset = nOffset;

		for (i = 0; i < nOffset; i = i + nbRepeats)
		{
			fscanf(file, "%ld %ld %ld %ld %ld", &temp1, &temp2, &temp3, &temp4, &temp5);
			// SD : the gray level refresh rate is guaranteed manually by introducing repetitions of the same frame.
			for (int j = 0; j < nbRepeats; j++)
			{
				LutArray.FrameNumbers[i + j] = temp2;
			}
		}


		AlpProjControlEx(nDevId, ALP_FLUT_WRITE_18BIT, &LutArray);
		printf(" Transfer Lut part 2 OK .\n");

		// Start Sequence 2 in queue mode
		if (ALP_OK != AlpProjStart(nDevId, nSeqId2)) return 1;
		printf("Seq2 \n");

		// Wait until Seq 2 is started to rewrite the LUT and reset the queue
		AlpProjInquireEx(nDevId, ALP_PROJ_PROGRESS, &QueueInfo);
		printf("SeqID = %d , Queue NWaiting = %d, Nframes = %d . \n", QueueInfo.SequenceId, QueueInfo.nWaitingSequences, QueueInfo.nFramesPerSubSequence);

		while (QueueInfo.SequenceId == 1)
		{
			AlpProjInquireEx(nDevId, ALP_PROJ_PROGRESS, &QueueInfo);
			Sleep(ts);
		}

		AlpProjControl(nDevId, ALP_PROJ_RESET_QUEUE, ALP_DEFAULT);
		printf("Seq 1 Finished . \n");

		// Rewrite the LUT part 1
		LutArray.nSize = nOffset;
		LutArray.nOffset = 0;

		for (i = 0; i < nOffset; i = i + nbRepeats)
		{
			fscanf(file, "%ld %ld %ld %ld %ld", &temp1, &temp2, &temp3, &temp4, &temp5);
			// SD : the gray level refresh rate is guaranteed manually by introducing repetitions of the same frame.
			for (int j = 0; j < nbRepeats; j++)
			{
				LutArray.FrameNumbers[i + j] = temp2;
			}
		}

		AlpProjControlEx(nDevId, ALP_FLUT_WRITE_18BIT, &LutArray);
		printf(" Transfer Lut part 1 OK.\n");

		AlpProjInquireEx(nDevId, ALP_PROJ_PROGRESS, &QueueInfo);
		printf("SeqID = %d , Queue NWaiting = %d, Nframes = %d . \n", QueueInfo.SequenceId, QueueInfo.nWaitingSequences, QueueInfo.nFramesPerSubSequence);

		// loop
	}

	if (Nremain > nOffset)
	{
		printf("Nremain = %d.", Nremain);

		// Start Sequence 1 in queue mode
		if (ALP_OK != AlpProjStart(nDevId, nSeqId1)) return 1;
		printf("Seq1 \n");

		// Wait until Seq 1 is started to rewrite the LUT and reset the queue
		AlpProjInquireEx(nDevId, ALP_PROJ_PROGRESS, &QueueInfo);

		while (QueueInfo.SequenceId == 2)
		{
			AlpProjInquireEx(nDevId, ALP_PROJ_PROGRESS, &QueueInfo);
			Sleep(ts);
		}

		AlpProjControl(nDevId, ALP_PROJ_RESET_QUEUE, ALP_DEFAULT);
		printf("Seq 2 Finished . \n");

		// Rewrite the LUT part 2
		LutArray.nSize = nOffset;
		LutArray.nOffset = nOffset;

		for (i = 0; i < Nremain - nOffset; i = i + nbRepeats)
		{
			fscanf(file, "%ld %ld %ld %ld %ld", &temp1, &temp2, &temp3, &temp4, &temp5);
			for (int j = 0; j < nbRepeats; j++)
			{
				LutArray.FrameNumbers[i + j] = temp2;
			}
		}

		AlpProjControlEx(nDevId, ALP_FLUT_WRITE_18BIT, &LutArray);
		printf(" Transfer Lut part 2 OK .\n");

		if (ALP_OK != AlpSeqControl(nDevId, nSeqId2, ALP_FLUT_ENTRIES9, (Nremain - nOffset) * 2))
		{
			printf("error (ALP_FLUT_ENTRIES).\r\n");
			return 1;
		}

		// Start Sequence 2 in queue mode
		if (ALP_OK != AlpProjStart(nDevId, nSeqId2)) return 1;
		printf("Seq2 \n");
	}

	else
	{
		printf("Nremain = %d.", Nremain);

		if (ALP_OK != AlpSeqControl(nDevId, nSeqId1, ALP_FLUT_ENTRIES9, Nremain * 2))
		{
			printf("error (ALP_FLUT_ENTRIES).\r\n");
			return 1;
		}

		// Start Sequence 1 in queue mode
		if (ALP_OK != AlpProjStart(nDevId, nSeqId1)) return 1;
		printf("Seq1 \n");

	}


	// Wait for key stroke
	printf("Press any key to stop ALP projection");
	do { _getch(); } while (_kbhit());

	// Done
	fclose(file);
	AlpDevHalt(nDevId);
	AlpDevFree(nDevId);

	return 0;
}
/*///Magnification

if( nSizeX != header[0] || nSizeY != header[1] )
{ printf("size error. \n");
long noffX, noffY, frameNb, i;

noffX = (nSizeX-header[0])/2;
noffY = (nSizeY-header[1])/2;
FrameNb = 0;

while (++ FrameNb < header[2])
{ i = 0;
while (++i<header[0])
{
fseek(file,8+header[0]*header[1]*FrameNb+i*header[0],0);
fread(block1+(noffX+i)*nSizeX+noffY+FrameNb*nSizeX*nSizeY , 1 , header[0] , file );
}
}
}
else
{ */