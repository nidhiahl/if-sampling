#include <iostream>
#include "./data.h"
#include "./iforest.h"
#include "./ifSampling.h"


int main(int argc, char* argv[])      //(argv[1] = inputdataFile.csv 
{
	srand(time(0));

	const string dataset = argv[1];
	//cout<<"start of data preparation"<<endl;
	struct timespec start_dP,end_dP;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_dP);

	data *dataObj = new data();
    dataObj->createDataVector(dataset+"/"+dataset+".csv");
    const data &refDataObject = *dataObj;
	
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_dP);
    double dPTime =  (((end_dP.tv_sec - start_dP.tv_sec) * 1e9)+(end_dP.tv_nsec - start_dP.tv_nsec))*1e-9;

	cout<<"dPTime="<<dPTime<<endl;
	
	cout<<"data read done"<<endl;


//Create iforest

	struct timespec start_iforest,end_iforest;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_iforest);

	iforest *iForestObject = new iforest(refDataObject, 100, 256);
	iforest &refiForestObject = *iForestObject;
	refiForestObject.constructiForest();
	refiForestObject.insertAllPoints();
	refiForestObject.computeAnomalyScore(); 
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_iforest);
    double iforestTime =  (((end_iforest.tv_sec - start_iforest.tv_sec) * 1e9)+(end_iforest.tv_nsec - start_iforest.tv_nsec))*1e-9;
	
	struct timespec start_sampling,end_sampling;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_sampling);
		
	ofstream write_AS(dataset+"/sampleSets/AS.csv",ios::out|ios::binary);
	if(!write_AS){
		cout<<"Can not open input data file:/"+dataset+"/sampleSets/AS.csv"<<endl;
		exit(0);
	}
	int point =0;
	for(auto i:refiForestObject.anomalyScore){
		write_AS<<point<<" "<<i<<endl;
		point++;
	}
	write_AS.close();
	



	string tree_criterion = "random";
	string leaf_criterion = "random";
	string point_criterion = "random";		
	ifSampling *ifSamplingObject = new ifSampling(refiForestObject);
	ifSampling &refifSamplingObject = *ifSamplingObject;
	refifSamplingObject.sample(tree_criterion,leaf_criterion,point_criterion,0.15);
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_sampling);
    double samplingTime =  (((end_sampling.tv_sec - start_sampling.tv_sec) * 1e9)+(end_sampling.tv_nsec - start_sampling.tv_nsec))*1e-9;
	
	ofstream write_sampleSet(dataset+"/sampleSets/"+tree_criterion+"_"+leaf_criterion+"_"+point_criterion+"_sampleSet.csv",ios::out|ios::binary);
	if(!write_sampleSet){
		cout<<"Can not open input data file:/"+dataset+"/sampleSets/"<<tree_criterion<<"_"<<leaf_criterion<<"_"<<point_criterion<<"_sampleSet.csv"<<endl;
		exit(0);
	}
	for(auto i:refifSamplingObject.sampleSet){
		write_sampleSet<<i<<" "<<refDataObject.dataVector[i]->label<<endl;
	}
	write_sampleSet.close();
	
	
return 0;
}
















