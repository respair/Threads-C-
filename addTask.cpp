#define WINDOWS_IGNORE_PACKING_MISMATCH
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "bmp_reader.h"

#include <windows.h>
using namespace std;

int count_ev = 0;
int count_s = 0;

HANDLE hEvent;
HANDLE Semaphore;

const char* name = "img01.bmp";
BMP_File* bmp = loadBMP(name);

// for CreateThread
struct Arg
{
	int n = -1; // ¹ Th
	int size = -1;
	int NT = 7;
	int N = 1000;
};

//CRITICAL_SECTION cs;

/*event*/
DWORD __stdcall eCount(void* arg) // func for thread
{
	Arg* a = (Arg*)arg;
	int count = 0;
	WaitForSingleObject(hEvent, INFINITE);
	for (int i = a->size - 1 - 3 * a->n; i >= 0; i -= 3 * a->NT) {
		unsigned char red = bmp->data[i];
		unsigned char green = bmp->data[i - 1];
		unsigned char blue = bmp->data[i - 2];
		if (red * green * blue < a->N) {
			count++;
		}
	}

	//EnterCriticalSection(&cs);
	cout << "Th #" << a->n + 1 << " Count: " << count << "\n";
	count_ev += count;
	//LeaveCriticalSection(&cs);
	SetEvent(hEvent);

	return 0;
}

void Event(BMP_File* bmp, int NT = 7)
{
	HANDLE th[7];
	hEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	DWORD th_id[7];
	//InitializeCriticalSection(&cs);

	Arg arg[7];
	for (int i = 0; i < NT; i++)
	{
		arg[i].size = bmp->dh.data_size;
		arg[i].n = i;
		th[i] = CreateThread(NULL, 0, eCount, &arg[i], 0, &th_id[i]);
	}
	WaitForMultipleObjects(NT, th, true, INFINITE);
	for (int i = 0; i < NT; i++) {
		CloseHandle(th[i]);
	}
	CloseHandle(hEvent);
	//DeleteCriticalSection(&cs);
}

/*semaphore*/
DWORD __stdcall sCount(void* arg) // func for thread
{
	Arg* a = (Arg*)arg;
	int count = 0;
	WaitForSingleObject(Semaphore, INFINITE);
	for (int i = a->size - 1 - 3 * a->n; i >= 0; i -= 3 * a->NT) {
		unsigned char red = bmp->data[i];
		unsigned char green = bmp->data[i - 1];
		unsigned char blue = bmp->data[i - 2];
		if (red * green * blue < a->N) {
			count++;
		}
	}

	cout << "Th #" << a->n + 1 << " Count: " << count << "\n";
	count_s += count;
	ReleaseSemaphore(Semaphore, 1, 0);

	return 0;
}

void SemaphoreC(BMP_File* bmp, int NT = 7)
{
	HANDLE th[7];
	Semaphore = CreateSemaphore(NULL, 1, 1, NULL);
	DWORD th_id[7];

	Arg arg[7];
	for (int i = 0; i < NT; i++)
	{
		arg[i].size = bmp->dh.data_size;
		arg[i].n = i;
		th[i] = CreateThread(NULL, 0, sCount, &arg[i], 0, &th_id[i]);
	}
	WaitForMultipleObjects(NT, th, true, INFINITE);
	for (int i = 0; i < NT; i++) {
		CloseHandle(th[i]);
	}
	CloseHandle(Semaphore);
}

int main()
{
	Event(bmp);
	cout << "Event Count: " << count_ev << "\n";
	cout << "\n";

	SemaphoreC(bmp);
	cout << "Semaphore Count: " << count_s << "\n";
	cout << "\n";

	freeBMP(bmp);
	return 0;
}