
#define LINUX

#include <stdio.h>
#include <CAENVMElib.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>

#include "utimer.h"
#define irint(A) int((A)+0.5)

int printres(int *v, int dimension){
	for (int i=0; i<dimension; i++){
		std::cout << *v++ << " ";
	}
	std::cout << std::endl;
	return 0;
}

// *** CAEN source: int printtofile(double *v, int dimension){
int printtofile(int *v, int dimension){                           // *** Roberto
	FILE *file;
	int i;
	// *** CAEN source: double max = 0.0;
	// *** CAEN source: double mean = 0.0;
	int max = 0;                                              // *** Roberto
	int mean = 0;                                             // *** Roberto
	int min = 0x7fffffff;                                     // *** Roberto
	//Open file
	file = fopen("./timeV513.dat", "w");
	if (file == NULL){
		printf("Error opening file\n");
		exit(1);
	}
	//loop on time
	fprintf(file, "Execution time\n");
	for(i = 0; i < dimension; i++){
		//calcolate minimum value                         // *** Roberto
		if (min > v[i]){                                  // *** Roberto
			min = v[i];                               // *** Roberto
		}                                                 // *** Roberto
		//calcolate maximum value
		if (max < v[i]){
			max = v[i];
		}
		//calcolate mean
		mean += v[i];
		//print the vector
		// *** CAEN source: fprintf(file, "%f\n", v[i]);
		fprintf(file, "%d\n", v[i]);                       // *** Roberto
	}
	// *** CAEN source: mean = mean/dimension;
	double fmean = double(mean)/dimension;
	mean = irint(fmean);                                       // *** Roberto
	// *** CAEN source: fprintf(file, "mean = %f \nmax=%f\n", mean, max);
	fprintf(file, "mean = %d\nmax = %d\nmin = %d\n", mean, max, min);              // *** Roberto
	fclose(file);
	return 0;
}

int main()
{
	uint32_t v513ioreg = 0xa00000 + 4;

	//varaibles
	int time_v[1000]= {0}; //in microseconds                              // *** Roberto
	uTimer ut;                                                            // *** Roberto
	int re, i;
	int32_t handle;
	int link = 1;
	unsigned int data;

        ut.setOffset();                                                       // *** Roberto

	printf("Let's start\n");
	
	re = CAENVME_Init(cvV1718, link, 0, &handle);                         // *** Roberto
	if (re != cvSuccess){
		printf("Error %d connecting the bridge\n", re);
		return re;
	}
	printf("Connection Done\n");
	
	re = CAENVME_ReadCycle(handle,v513ioreg,&data,cvA24_U_DATA,cvD16);
	if (re != cvSuccess){
		printf("Error %d ReadCycle\n", re);
		return re;
	}
	printf("Read ok\n");

	//Do something in a loop
	for(i=0; i<1000; i++){
		ut.start();                        // Roberto
		re = CAENVME_ReadCycle(handle,v513ioreg,&data,cvA24_U_DATA,cvD16);
		ut.stop();                                                    // *** Roberto
		time_v[i] = irint(ut.elapsedMicroseconds());                  // *** Roberto
	}
	printf("1000 read ok\n");

	int count_errors = 0;
	uint32_t dset = 0xff00;
	uint32_t dclr = 0;
	ut.start();
	for(i=0; i<5000; i++){
		re = CAENVME_WriteCycle(handle,v513ioreg,&dset,cvA24_U_DATA,cvD16);
		if (re) count_errors++;
		re = CAENVME_WriteCycle(handle,v513ioreg,&dclr,cvA24_U_DATA,cvD16);
		if (re) count_errors++;
	}
	ut.stop();
	int tottime = irint(ut.elapsedMicroseconds()/10000);
	printf(" Only write V513 i/o reg - Tot errors %d average time %d\n", count_errors, tottime);
	
	count_errors = 0;
	ut.start();
	for(i=0; i<5000; i++) {
		re = CAENVME_WriteCycle(handle,v513ioreg,&dset,cvA24_U_DATA,cvD16);
		if (re) count_errors++;
		re = CAENVME_ReadCycle(handle,v513ioreg,&data,cvA24_U_DATA,cvD16);
		if (re) count_errors++;
		re = CAENVME_WriteCycle(handle,v513ioreg,&dclr,cvA24_U_DATA,cvD16);
		if (re) count_errors++;
		re = CAENVME_ReadCycle(handle,v513ioreg,&data,cvA24_U_DATA,cvD16);
		if (re) count_errors++;
	}
	ut.stop();
	tottime = irint(ut.elapsedMicroseconds()/10000);
	printf(" Write out reg + read inp reg - Tot errors %d average time %d\n", count_errors, tottime);
	
	count_errors = 0;
	ut.start();
	for(i=0; i<10000; i++){
		re = CAENVME_ReadCycle(handle,v513ioreg,&data,cvA24_U_DATA,cvD16);
		if (re) count_errors++;
	}
	ut.stop();
	tottime = irint(ut.elapsedMicroseconds()/10000);
	printf(" Read reg 0x2 - Tot errors %d average time %d\n", count_errors, tottime);
	
	//free handle
	CAENVME_End(handle);
	
	//print time vector on std output
	re = printtofile(time_v, 1000);
	
	re = printres(time_v, 1000);
	
	return re;
}
