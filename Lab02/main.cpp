#include <omp.h>
#include "common.h"
#include "DS_timer.h"

#define DECRYPT_ALGORITHM(a,b,c) (c += a*50 + b)
 
uchar* readData(char* _fileName, int _dataSize);
bool writeData(char* _fileName, int _dataSize, uchar* _data);

// Lab 1-2
int main(int argc, char** argv)
{
	DS_timer timer(2, 1);

	timer.setTimerName(0, (char*)"Serial Algorithm");
	timer.setTimerName(1, (char*)"Parallel Algorithm");

	if (argc < 6) {
		printf("It requires five arguments\n");
		printf("Usage: Extuction_file inputImgA inputImgB Width Height OutputFileName\n");
		return -1;
	}

	int width = atoi(argv[3]);
	int height = atoi(argv[4]);
	int dataSize = width * height * 3;

	// Read input data
	uchar* A = readData(argv[1], dataSize);
	uchar* B = readData(argv[2], dataSize);

	uchar* serialC = new uchar[dataSize];
	uchar* parallelC = new uchar[dataSize]; 
	memset(serialC, 0, sizeof(uchar) * dataSize);
	memset(parallelC, 0, sizeof(uchar) * dataSize);

	// Decrypt the image
	// The algorith is defined as DECRYPT_ALGORITHM(a,b,c)
	// See the definition at the top of this source code

	// 1. Serial algorithm
	timer.onTimer(0);
	for (int i = 0; i < dataSize; i++) {
		DECRYPT_ALGORITHM(A[i], B[i], serialC[i]);
	}
	timer.offTimer(0);

	timer.onTimer(1);
	// ***************************************************************
	// Write the decyprt result to parallelC array
	// Hint: DECRYPT_ALGORITHM(A[i], B[i], parallelC[i])

	// 2. Parallel algorithm

	#pragma omp parallel num_threads(6)
	{
		int current_thread_id = omp_get_thread_num();
		int max_thread_id = omp_get_num_threads();
		int chunk_size = dataSize / max_thread_id;
		int start_index = chunk_size * current_thread_id;
		int end_index = chunk_size * (current_thread_id + 1);
		for (int i = start_index; i < end_index; i++) {
			DECRYPT_ALGORITHM(A[i], B[i], parallelC[i]);
		}
	}
	



	// **************************************************************
	timer.offTimer(1);

	// Check the results
	bool isCorrect = true;
	for (int i = 0; i < dataSize; i++) {
		if (serialC[i] != parallelC[i]) {
			isCorrect = false;
			break;
		}
	}

	if (isCorrect)
		printf("The results is correct - Good job!\n");
	else
		printf("The result is not correct! :(\n");

	printf("Your computer has %d logical cores\n", omp_get_num_procs());
	timer.printTimer();


	if (!writeData(argv[5], dataSize, parallelC))
		printf("Fail to write the data\n");
	else
		printf("The decrption result was written to %s\n", argv[5]);
}

uchar* readData(char* _fileName, int _dataSize)
{
	uchar* data;
	data = new uchar[_dataSize];
	memset(data, 0, sizeof(uchar) * _dataSize);

	FILE* fp = NULL;
	fopen_s(&fp, _fileName, "rb");
	if (!fp) {
		printf("Fail to open %s\n", _fileName);
		return NULL;
	}

	fread(data, sizeof(uchar), _dataSize, fp);
	fclose(fp);

	return data;
}

bool writeData(char* _fileName, int _dataSize, uchar* _data)
{
	FILE* fp = NULL;
	fopen_s(&fp, _fileName, "wb");
	if (!fp) {
		printf("Fail to open %s\n", _fileName);
		return false;
	}

	fwrite(_data, sizeof(uchar), _dataSize, fp);
	fclose(fp);

	return true;
}
