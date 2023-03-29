#include <iostream>
#include "./data.h"
#include "./iforest.h"
#include "./ifSampling.h"


int main(int argc, char* argv[])      //(argv[1] = inputdataFile.csv 
{
	srand(time(0));

	const string dataset = argv[1];
	
	struct timespec start_dP,end_dP;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_dP);

	data *dataObj = new data();
    dataObj->createDataVector(dataset+"/"+dataset+".csv");
    const data &refDataObject = *dataObj;
	
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_dP);
    double dPTime =  (((end_dP.tv_sec - start_dP.tv_sec) * 1e9)+(end_dP.tv_nsec - start_dP.tv_nsec))*1e-9;

	//cout<<"dPTime="<<dPTime<<endl;
	
	//cout<<"data read done"<<endl;


//Create iforest

	struct timespec start_matrix,end_matrix;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_matrix);

	iforest *iForestObject = new iforest(refDataObject, 100, 256);
	iforest &refiForestObject = *iForestObject;
	
	
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_matrix);
    double matrixTime =  (((end_matrix.tv_sec - start_matrix.tv_sec) * 1e9)+(end_matrix.tv_nsec - start_matrix.tv_nsec))*1e-9;
	

return 0;
}
