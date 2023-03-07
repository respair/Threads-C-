#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <device_functions.h>
#include <cuda.h>
#include "device_launch_parameters.h"
#include "cuda_runtime.h"
#include "bmp.cuh"

__global__ void cudaFunc(unsigned char* data, int size, int Nt, int n, int* count) {
	for (int i = size - 1 - 3 * threadIdx.x; i >= 0; i -= 3 * Nt) {
		
		unsigned char red = data[i];
		unsigned char green = data[i - 1];
		unsigned char blue = data[i - 2];
		if (red * green * blue < n) {
			count[threadIdx.x] +=1;
		}
	}
}

int main(int argc, char *argv[]) {

	const char* name = "img01.bmp";
	BMP_File* bmp = loadBMP(name);
	const int NT = 7; //���������� �����
	int N = 1000;

	//�������������� ������� �� ������� ������
	int *count = (int*)malloc(sizeof(int) * NT);
	for (int i = 0; i < NT; i++) {
		count[i] = 0;
	}

	//�������� ������ �� cpu ��� ��������
	unsigned char *imgData;
	imgData = (unsigned char*)malloc(sizeof(unsigned char) * bmp->dh.data_size);
	imgData = bmp->data;

	//�������� ������ �� gpu
	int *CountCuda;
	unsigned char*DataCuda;
	cudaMalloc(&CountCuda, sizeof(int)*NT);
	cudaMalloc(&DataCuda, sizeof(unsigned char) * bmp->dh.data_size);
	
	//�������� � host'� �� device
	cudaMemcpy(CountCuda, count, (sizeof(int) * NT), cudaMemcpyHostToDevice);
	cudaMemcpy(DataCuda, imgData, sizeof(unsigned char) * bmp->dh.data_size, cudaMemcpyHostToDevice);

	//����� ���� � 7 ������
	cudaFunc<<<1, 7>>>(DataCuda, bmp->dh.data_size, NT, N, CountCuda);

	//�������� � device �� host
	cudaMemcpy(count, CountCuda, sizeof(int) * NT, cudaMemcpyDeviceToHost);

	int sum = 0;
	for (int i = 0; i < NT; i++) {
		sum += count[i];
	}
	printf("CUDA: %d\n", sum);

	//����������� ������
	free(imgData);
	free(count);
	freeBMP(bmp);
	cudaFree(CountCuda);
	cudaFree(DataCuda);
	return 0;
}
