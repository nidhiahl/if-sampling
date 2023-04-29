#include <iostream>
#include "./data.h"
#include "./iforest.h"
#include "./ifSampling.h"


double computeaScoreTh(iforest & refiForestObject, int outlierCount){
	vector<double> as = refiForestObject.anomalyScore;
	sort(as.begin(),as.end(),greater<double>());
	return(as[outlierCount-1]);
}

int main(int argc, char* argv[])      //(argv[1] = inputdataFile.csv 
{
	srand(time(0));

	const string dataset = argv[1];
	const int numOfOutlier = atoi(argv[2]);
	//cout<<"start of data preparation"<<endl;
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
	
	//const double aScoreTh = 0.5;	
	const double aScoreTh = computeaScoreTh(refiForestObject,numOfOutlier);
	cout<<aScoreTh<<endl;
	//aScoreTh = anomalyScoreThreshold(refiForestObject);

	//exit(0);

	/*string tc[] = {"random","max_aggrement"};
	string lc[] = {"random","hard_discard","soft_discard"};
	string pc[] = {"random","core_points","border_points"};
	int ss[] = {1,2,3,4,5};
	*/
	string tc[] = {"random"};
	string lc[] = {"random","soft_discard","hard_discard"};
	string pc[] = {"random","core_points","border_points"};	
	int ss[] = {5};
	int numRuns = 5;
	for(int run = 1; run<= numRuns; run++){
		for(auto sample_size:ss){
			for(auto tree_criterion:tc){
				for(auto leaf_criterion:lc){
					for(auto point_criterion:pc){
						ifSampling *ifSamplingObject = new ifSampling(refiForestObject,aScoreTh,tree_criterion,leaf_criterion,point_criterion,double(sample_size)/100);
						ifSampling &refifSamplingObject = *ifSamplingObject;
						refifSamplingObject.sample();
						clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_sampling);
    					double samplingTime =  (((end_sampling.tv_sec - start_sampling.tv_sec) * 1e9)+(end_sampling.tv_nsec - start_sampling.tv_nsec))*1e-9;
						
						ofstream write_sampleSet(dataset+"/sampleSets/"+to_string(run)+"_Run"+to_string(sample_size)+"%_"+tree_criterion+"_"+leaf_criterion+"_"+point_criterion+"_sampleSet.csv",ios::out|ios::binary);
						if(!write_sampleSet){
							cout<<"Can not open input data file:/"+dataset+"/sampleSets/"<<sample_size<<"%_"<<tree_criterion<<"_"<<leaf_criterion<<"_"<<point_criterion<<"_sampleSet.csv"<<endl;
							exit(0);
						}
						for(auto i:refifSamplingObject.sampleSet){
							write_sampleSet<<i<<" "<<refDataObject.dataVector[i]->label<<endl;
						}
						write_sampleSet.close();
						delete ifSamplingObject;
					}
				}
			}	
		}
	}

	
	
	
return 0;
}

















































class StdDevCalc{
    private:

        double count;
        double meanPrev, meanCurr;
        double sPrev, sCurr;
        double varianceCurr;

    public:

        StdDevCalc() {
            count = 0;
        }

        void append(double d) {
            count++;

            if (count == 1) {
                // Set the very first values.
                meanCurr     = d;
                sCurr        = 0;
                varianceCurr = sCurr;
            }
            else {
                // Save the previous values.
                meanPrev     = meanCurr;
                sPrev        = sCurr;

                // Update the current values.
                meanCurr     = meanPrev + (d - meanPrev) / count;
                sCurr        = sPrev    + (d - meanPrev) * (d - meanCurr);
                varianceCurr = sCurr / (count - 1);
            }
        }

    double get_std_dev() {
        return sqrt(varianceCurr);
    }
};



double variance(vector<double> a, int n)
{
    //cout<<" Compute mean (average of elements)"<<endl;
    double sum = 0;
	//cout<<"a.siz()="<<a.size()<<endl;
    for (int i = 0; i < n; i++)
        sum += a[i];
    double mean = sum /
                  (double)n;
 
    //cout<<" Compute sum squared"<<endl;
    // differences with mean.
    double sqDiff = 0;
    for (int i = 0; i < n; i++)
        sqDiff += (a[i] - mean) *
                  (a[i] - mean);
	//cout<<"before return"<<endl;
    return sqDiff / n;
}
 
double standardDeviation(vector<double> a, int n)
{
    return sqrt(variance(a, n));
}



double aanomalyScoreThreshold(iforest & refiForestObject){
	int _totalPoints = refiForestObject._dataObject.getnumInstances()-1;
	vector<double> as = refiForestObject.anomalyScore;
	sort(as.begin(),as.end());
	vector<double> diff;
	diff.resize(_totalPoints);
	for(int i=0;i<_totalPoints;i++){
		diff[i] = abs(as[i]-as[i+1])/max(as[i],as[i+1]);
	}
	int mini=0;
	for(int i=0;i<_totalPoints;i++){
		/*double mean = (accumulate(diff.begin(), diff.begin()+i,0))/(i+1);
		double noisesd = ((diff.begin(), diff.begin()+i) - mean)*((diff.begin(), diff.begin()+i)-mean)/(i+1);
		noisesd = noisesd/(i+1);
		
		mean = (accumulate(diff.begin()+i+1, diff.end(),0))/(_totalPoints-1-i);
		double normalsd = ((diff.begin(), diff.begin()+i)-mean)*((diff.begin(), diff.begin()+i)-mean)/(_totalPoints-1-i);
		normalsd = normalsd/(i+1);*/
		//cout<<"Inside for loop"<<endl;
		vector<double> a(diff.begin(), diff.begin()+i+1);
		vector<double> b(diff.begin()+i+1, diff.end());
		double noisesd = standardDeviation( a,i+1);
		//cout<<"noisesd="<<noisesd;
		double normalsd = standardDeviation(b,_totalPoints-1-i);
		double min = 1;
		if(min > abs(noisesd-normalsd)){
			min=abs(noisesd-normalsd);
			mini=i;
		}
	}
	cout<<"mini="<<mini<<" threshold="<<as[mini];
	return as[mini];
} 


double anomalyScoreThreshold(iforest & refiForestObject){

	double minStdDevDiff;
	StdDevCalc noisePoint;
	int i;
	int _totalPoints = refiForestObject._dataObject.getnumInstances();
	for(i = 0; i < _totalPoints; i++){
			StdDevCalc normalPoint;
			int max_id = i;
			int mxChngecount=0;
			int mxNOTChngecount=0;
			for(int j = i+1; j < _totalPoints; j++){
				
				if(refiForestObject.anomalyScore[j] > refiForestObject.anomalyScore[max_id]){
					mxChngecount++;
					normalPoint.append(refiForestObject.anomalyScore[max_id]);
					//normalPoint.append(log10(refiForestObject.anomalyScore[max_id]));
					max_id = j;
				
				}else{
					mxNOTChngecount++;
					normalPoint.append(refiForestObject.anomalyScore[j]);
					//normalPoint.append(log10(refiForestObject.anomalyScore[j]));
				}
		}
		noisePoint.append(refiForestObject.anomalyScore[max_id]);
		//noisePoint.append(log10(refiForestObject.anomalyScore[max_id]));
		if(max_id!=i){
			swap(refiForestObject.anomalyScore[max_id],refiForestObject.anomalyScore[i]);		
		}
		//cout<<"FOR i="<<i<<":  non-ceters : "<<normalPoint.get_std_dev()<<" centers : "<< noisePoint.get_std_dev()<<"  difference:"<<abs(noisePoint.get_std_dev() - normalPoint.get_std_dev())<<endl;
		if(i==0){
			minStdDevDiff = abs(noisePoint.get_std_dev() - normalPoint.get_std_dev());
			//NOISE.push_back(i);
			continue;		
		}
		else{
			if(minStdDevDiff > abs(noisePoint.get_std_dev() - normalPoint.get_std_dev())){
				minStdDevDiff = abs(noisePoint.get_std_dev() - normalPoint.get_std_dev());
				//NOISE.push_back(i);
			}else{
				cout<<"refiForestObject.anomalyScore[i]= "<<refiForestObject.anomalyScore[i]<<endl;
				return (refiForestObject.anomalyScore[i]);
			}
		}
	}
	cout<<"refiForestObject.anomalyScore[i]= "<<refiForestObject.anomalyScore[i]<<endl;
	return (refiForestObject.anomalyScore[i]);
} 








