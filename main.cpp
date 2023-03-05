#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "bmp_reader.h"
#include <iomanip>
#include <windows.h>
#include <thread>
#include <mutex>
#include <omp.h>
using namespace std;

//int G = 511, ID = 5;
//int X = G * 2 + ID;
int A = 3; // X % 4;			// вариант деления = 3
int B = 7; // 5 + X % 5;        // кол-во потоков/процессов = 7
int count_th = 0;
int count_stdth = 0;
int count_omp = 0;

const char* name = "img01.bmp";
BMP_File* bmp = loadBMP(name);

// for CreateThread
struct Arg
{
	int n = -1; // № Th
	int size = -1;
	int NT = 7;
	int N = 1000;
};

// just print
void GetPixels(BMP_File* bmp)
{
	for (unsigned int i = 0; i < bmp->dh.data_size; i++) //bmp->dh.data_size
	{
		if (i % 16 == 0)
		{
			printf("%04x ", i);
			cout << ": ";
		}

		printf("%02x", bmp->data[i]);
		cout << " ";
		if (i % 16 == 15 && i != 0)
			cout << "\n";
	}
}

// in cycle
int CountCycle(BMP_File* bmp, int N=1000)
{
	int count = 0;

	for (unsigned int i = 0; i < bmp->dh.data_size; i += 3) {
		unsigned char red = bmp->data[i + 2];
		unsigned char green = bmp->data[i + 1];
		unsigned char blue = bmp->data[i];
		if (red * green * blue < N) {
			count++;
		}
	}

	return count;
}

// my_cycle (v3)
int myCountCycle(BMP_File* bmp, int n, int NT = 7, int N = 1000)
{
	int count = 0;

	for (int i = bmp->dh.data_size - 1 - 3 * n; i >= 0; i -= 3 * NT) {
		unsigned char red = bmp->data[i];
		unsigned char green = bmp->data[i - 1];
		unsigned char blue = bmp->data[i - 2];
		if (red * green * blue < N) {
			count++;
		}
	}

	return count;
}

CRITICAL_SECTION cs;

/*CreateTread*/
DWORD __stdcall Count(void* arg) // func for thread
{
	Arg* a = (Arg*)arg;
	int count = 0;
	EnterCriticalSection(&cs);
	LeaveCriticalSection(&cs);

	for (int i = a->size - 1 - 3*a->n; i >= 0; i -= 3*a->NT) { 
		unsigned char red = bmp->data[i];
		unsigned char green = bmp->data[i - 1];
		unsigned char blue = bmp->data[i - 2];
		if (red * green * blue < a->N) {
			count++;
		}
	}

	EnterCriticalSection(&cs);
	cout << "Th #" << a->n + 1 << " Count: " << count << "\n";
	count_th += count;
	LeaveCriticalSection(&cs);

	return 0;
}

void Thread(BMP_File* bmp, int NT = 7) 
{
	HANDLE th[7];
	DWORD th_id[7];
	InitializeCriticalSection(&cs);
	
	Arg arg[7];
	for (int i = 0; i < NT; i++)
	{
		arg[i].size = bmp->dh.data_size;
		arg[i].n = i;
		th[i] = CreateThread(NULL, 0, Count, &arg[i], 0, &th_id[i]);
	}
	WaitForMultipleObjects(NT, th, true, INFINITE);
	DeleteCriticalSection(&cs);
}

mutex mtx;

/*std::thread*/
void stdCountCycle(BMP_File* bmp, int n, int NT = 7, int N = 1000)
{
	int count = 0;

	for (int i = bmp->dh.data_size - 1 - 3 * n; i >= 0; i -= 3 * NT) {
		unsigned char red = bmp->data[i];
		unsigned char green = bmp->data[i - 1];
		unsigned char blue = bmp->data[i - 2];
		if (red * green * blue < N) {
			count++;
		}
	}
	mtx.lock();
	cout << "stdTh #" << n + 1 << " Count: " << count << "\n";
	count_stdth += count;
	mtx.unlock();
}

void stdThread(BMP_File* bmp, int NT = 7)
{
	thread thd[7];
	for (int i = 0; i < NT; i++)
	{
		thd[i] = thread(stdCountCycle, bmp, i, 7, 1000);
	}
	for (int i = 0; i < NT; i++)
	{
		thd[i].join();
	}

}

/*omp*/
void ompCountCycle(BMP_File* bmp, int n, int NT = 7, int N = 1000)
{
	int count = 0;

	for (int i = bmp->dh.data_size - 1 - 3 * n; i >= 0; i -= 3 * NT) {
		unsigned char red = bmp->data[i];
		unsigned char green = bmp->data[i - 1];
		unsigned char blue = bmp->data[i - 2];
		if (red * green * blue < N) {
			count++;
		}
	}
	#pragma omp critical 
		{
			cout << "ompTh #" << n + 1 << " Count: " << count << "\n";
			count_omp += count;
		}
}

void OMP(BMP_File* bmp, int NT = 7)
{
	omp_set_num_threads(7);
	#pragma omp parallel 
		{
			ompCountCycle(bmp, omp_get_thread_num(), 7, 1000);
			#pragma omp barrier
		}
}


int main() {
	//GetPixels(bmp);

	int count = CountCycle(bmp);
	cout << "CountCycle: " << count << "\n";
	cout << "\n";

	Thread(bmp);
	cout << "CountThread: " << count_th << "\n";
	cout << "\n";

	stdThread(bmp);
	cout << "stdCountThread: " << count_stdth << "\n";
	cout << "\n";

	OMP(bmp);
	cout << "ompCountThread: " << count_omp << "\n";
	cout << "\n";


	freeBMP(bmp);
	return 0;
}