#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include "bmp_reader.h"
//#include <windows.h>
#include <unistd.h>
using namespace std;

int count_pth = 0;

const char* name = "img01.bmp";
BMP_File* bmp = loadBMP(name);


// for Thread
struct Arg
{
	int n = -1; // ¹ Th
	int size = -1;
	int NT = 7;
	int N = 1000;
};

pthread_mutex_t mutex;

/*pthread*/
void* Count(void* arg) // func for thread
{
	Arg* a = (Arg*)arg;
	int count = 0;

	for (int i = a->size - 1 - 3 * a->n; i >= 0; i -= 3 * a->NT) {
		unsigned char red = bmp->data[i];
		unsigned char green = bmp->data[i - 1];
		unsigned char blue = bmp->data[i - 2];
		if (red * green * blue < a->N) {
			count++;
		}
	}

	pthread_mutex_lock(&mutex);
	cout << "pTh #" << a->n + 1 << " Count: " << count << "\n";
	count_pth += count;
	pthread_mutex_unlock(&mutex);

	return 0;
}

void pThread(BMP_File* bmp, int NT = 7)
{
	int pth[7];
	pthread_t pth_id[7];
	pthread_mutex_init(&mutex, NULL);

	Arg arg[7];
	for (int i = 0; i < NT; i++)
	{
		arg[i].size = bmp->dh.data_size;
		arg[i].n = i;
		pth[i] = pthread_create(&pth_id[i], NULL, Count, &arg[i]);
		//th[i] = CreateThread(NULL, 0, Count, &arg[i], 0, &th_id[i]);
	}

	for (int i = 0; i < NT; i++)
		pthread_join(pth_id[i], NULL);

	pthread_mutex_destroy(&mutex);
}

int main()
{
	pThread(bmp);
	cout << "CountPThread: " << count_pth << endl;
	freeBMP(bmp);
}
