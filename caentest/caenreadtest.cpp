
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
	file = fopen("./time.dat", "w");
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

int main(){
	//varaibles
	// *** CAEN source double time_v[1000]= {0}; //in ms
	int time_v[1000]= {0}; //in microseconds                              // *** Roberto
	clock_t begin, end;
	uTimer ut;                                                            // *** Roberto
	int re, i;
	int32_t handle;
	int link = 1;
	unsigned int data;

        ut.setOffset();                                                       // *** Roberto

	printf("Let's start\n");
	
	//Connect to V1718
	// *** CAEN source: re = CAENVME_Init2(cvV1718, &link, 0, &handle);
	re = CAENVME_Init(cvV1718, link, 0, &handle);                         // *** Roberto
	if (re != cvSuccess){
		printf("Error %d connecting the bridge\n", re);
		return re;
	}
	printf("Connection Done\n");
	
	//check if everything is working fine
	// *** CAEN source re = CAENVME_ReadRegister(handle, 0x02, &data);
	CVRegisters cvr = static_cast<CVRegisters>(0x02);                     // *** Roberto
	re = CAENVME_ReadRegister(handle, cvr, &data);                        // *** Roberto
	if (re != cvSuccess){
		printf("Error %d ReadCycle\n", re);
		return re;
	}
	printf("Read ok\n");

	//Do something in a loop
	for(i=0; i<1000; i++){
		//start time
		// *** CAEN source: begin = clock();
		ut.start();                        // Roberto
		//ReadCycle ROC FPGA of a V17XX
		// *** CAEN source: re = CAENVME_ReadRegister(handle, 0x02, &data);
		re = CAENVME_ReadRegister(handle, cvr, &data);                // *** Roberto
		//stop time
		// *** CAEN source: end = clock();
		//update time vector
		//we divided for CLOCKS_PER_SEC to have seconds and then multipled for 1000000 to have us
		// *** CAEN source: time_v[i]= ((double)(end - begin))/CLOCKS_PER_SEC * 1000000;
		ut.stop();                                                    // *** Roberto
		time_v[i] = irint(ut.elapsedMicroseconds());                  // *** Roberto
	}

	for(i=cvInput0; i<=cvInput1; i++){
		CVInputSelect is = static_cast<CVInputSelect>(i);
		re = CAENVME_SetInputConf(handle,is,cvDirect,cvActiveHigh);
		printf(" CAENVME_SetInputConf on chan %d gave %d\n", i, re);
	}
	for(i=cvOutput0; i<=cvOutput4; i++){
		CVOutputSelect os = static_cast<CVOutputSelect>(i);
		re = CAENVME_SetOutputConf(handle,os,cvDirect,cvActiveHigh,cvManualSW );
		printf(" CAENVME_SetOutputConf on chan %d gave %d\n", i, re);
	}

	int count_errors = 0;
	ut.start();
	for(i=0; i<5000; i++){
		re = CAENVME_WriteRegister(handle,cvOutRegSet,1<<6);
		if (re) count_errors++;
		re = CAENVME_WriteRegister(handle,cvOutRegClear,1<<6);
		if (re) count_errors++;
	}
	ut.stop();
	int tottime = irint(ut.elapsedMicroseconds()/10000);
	printf(" Only write out reg - Tot errors %d average time %d\n", count_errors, tottime);
	
	count_errors = 0;
	ut.start();
	for(i=0; i<5000; i++){
		re = CAENVME_WriteRegister(handle,cvOutRegSet,1<<6);
		if (re) count_errors++;
		re = CAENVME_ReadRegister(handle,cvInputReg,&data);
		if (re) count_errors++;
		re = CAENVME_WriteRegister(handle,cvOutRegClear,1<<6);
		if (re) count_errors++;
		re = CAENVME_ReadRegister(handle,cvInputReg,&data);
		if (re) count_errors++;
	}
	ut.stop();
	tottime = irint(ut.elapsedMicroseconds()/10000);
	printf(" Write out reg + read inp reg - Tot errors %d average time %d\n", count_errors, tottime);
	
	count_errors = 0;
	ut.start();
	for(i=0; i<5000; i++){
		re = CAENVME_WriteRegister(handle,cvOutRegSet,1<<6);
		if (re) count_errors++;
		re = CAENVME_ReadRegister(handle,cvr,&data);          // Roberto 04.09.2024
		if (re) count_errors++;
		re = CAENVME_WriteRegister(handle,cvOutRegClear,1<<6);
		if (re) count_errors++;
		re = CAENVME_ReadRegister(handle,cvr,&data);          // Roberto 04.09.2024
		if (re) count_errors++;
	}
	ut.stop();
	tottime = irint(ut.elapsedMicroseconds()/10000);
	printf(" Write out reg + read reg 0x2 - Tot errors %d average time %d\n", count_errors, tottime);
	
	count_errors = 0;
	ut.start();
	for(i=0; i<10000; i++){
		re = CAENVME_ReadRegister(handle,cvr,&data);          // Roberto 04.09.2024
		if (re) count_errors++;
	}
	ut.stop();
	tottime = irint(ut.elapsedMicroseconds()/10000);
	printf(" Read reg 0x2 - Tot errors %d average time %d\n", count_errors, tottime);
	
	count_errors = 0;
	ut.start();
	for(i=0; i<10000; i++){
		re = CAENVME_ReadRegister(handle,cvInputReg,&data);          // Roberto 04.09.2024
		if (re) count_errors++;
	}
	ut.stop();
	tottime = irint(ut.elapsedMicroseconds()/10000);
	printf(" Read inp reg - Tot errors %d average time %d\n", count_errors, tottime);
	
	//free handle
	CAENVME_End(handle);
	
	
	//print time vector on std output
	re = printtofile(time_v, 1000);
	
	re = printres(time_v, 1000);
	
	return re;
}
