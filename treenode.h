#ifndef TREENODE_H
#define TREENODE_H
# include "data.h"
#include <vector>
#include <math.h>
#include <random>

class treenode
{
    public:

        int nodeId;
        int parentId;
        int lChildId;
        int rChildId;
        treenode *parentAdd;
        treenode *lChildAdd;
        treenode *rChildAdd;
        vector<int> dataPointIndices;
        int splitAttribute;
        double splitValue;
        int nodeSize;   
        int nodeHeight;
        bool isLeaf;
        int nodeMass;
		long double averagePLOfNode;
		 

treenode()
{
    splitAttribute = -1;
    isLeaf = bool(0);
    nodeId = -1;
    parentId = -1;
    lChildId = -1;
    rChildId = -1;
    nodeMass = 0;
    nodeSize = 0;
	averagePLOfNode = 0;
    dataPointIndices.resize(0);
    parentAdd = nullptr;
    lChildAdd = nullptr;
    rChildAdd = nullptr;

}

treenode(int nId): nodeId(nId)
{
    splitAttribute = -1;
    isLeaf = bool(0);
    parentId = nodeId == 0 ? 0 : (nodeId-1)/2;
    lChildId = -1;
    rChildId = -1;
	nodeHeight = (int)log2(nodeId+1)+1;
    nodeMass = 0;
    nodeSize = 0;
	averagePLOfNode = 0;
    dataPointIndices.resize(0);
    parentAdd = nullptr;
    lChildAdd = nullptr;
    rChildAdd = nullptr;

	
}

virtual ~treenode(){}


///for PGIF
double invertedCumulativeProbabilityFunction(double target)
{
    double x = 0.0;
    double y = 0.5 - target;

    while(abs(y) > 0.000001)
    {

    }

    return x;
}

double PGIFsplitInfoSelection(const data &dataObject){
	
	int powerForSegment=1;
    std::random_device random_seed_generator;
    std::mt19937_64 RandomEngine(random_seed_generator());

    splitAttribute = std::uniform_int_distribution<>(0, dataObject.getnumAttributes()-1)(RandomEngine);

    vector<double> values;
    for(int i=0; i<dataPointIndices.size(); i++)
    {
        values.push_back(dataObject.dataVector[(dataPointIndices[i])]->attributes[splitAttribute]);
    }

    sort(values.begin(), values.end());

    vector<double> diff;
    double sum=0.0;

    for(int i=1; i<dataPointIndices.size(); i++)
    {
        diff.push_back(values[i]-values[i-1]);
        diff[i-1]=pow(diff[i-1], powerForSegment+1);

        sum+=diff[i-1];
    }

    vector<double> cumulativeProbability;
    for(int i=0; i<diff.size(); i++)
    {
        cumulativeProbability.push_back(diff[i]/sum);
    }

    double c = std::uniform_real_distribution<double> (0.0, 1.0)(RandomEngine);

    int i=0;
    while(cumulativeProbability[i]<c)
    {
        c-=cumulativeProbability[i];
        i++;
    }

    double y=(2*c)/cumulativeProbability[i]-1;

    return y*(values[i+1]-values[i])/2+(values[i]+values[i+1])/2;
}





double splitInfoSelection(const data &dataObject){
	std::random_device random_seed_generator;
    std::mt19937_64 RandomEngine(random_seed_generator());


	double maxVal = -999999;
	double minVal = 999999;
	int attempts = 0;
	while(attempts < 10){
		splitAttribute = std::uniform_int_distribution<>(0, dataObject.getnumAttributes()-1)(RandomEngine);
		for( int i = 0; i < dataPointIndices.size(); i++){
        		if(maxVal < dataObject.dataVector[(dataPointIndices[i])]->attributes[splitAttribute]){
				maxVal = dataObject.dataVector[(dataPointIndices[i])]->attributes[splitAttribute];
			}
        		if(minVal > dataObject.dataVector[(dataPointIndices[i])]->attributes[splitAttribute]){
        			minVal = dataObject.dataVector[(dataPointIndices[i])]->attributes[splitAttribute];
        		}
        }
        attempts = attempts + 1;
        double dataDiff = maxVal - minVal;
		if(dataDiff >= 0.0000000000000001){
			attempts = 10;
		}
	}
	 return std::uniform_real_distribution<double> (minVal, maxVal)(RandomEngine);
}


void createLeftChild(){
	lChildAdd = new treenode(2*nodeId+1);
	lChildAdd->parentAdd = this;
	lChildId = lChildAdd->nodeId;
}

void createRightChild(){
	rChildAdd = new treenode(2*nodeId+2);
	rChildAdd->parentAdd = this;
	rChildId = rChildAdd->nodeId;
}

void computeAveragePLOfNode(){
	averagePLOfNode = nodeHeight-1+avgPathLengthEstimationOfUnbuiltBST();
}

long double avgPathLengthEstimationOfUnbuiltBST(){
	long double avgPathLengthEstimationOfBST = 0;
	if(nodeSize == 2){avgPathLengthEstimationOfBST=1;}
	else if(nodeSize > 2){avgPathLengthEstimationOfBST = (2 * (log(nodeSize-1) + 0.5772156649)) - (2 * (nodeSize - 1) / nodeSize);}	
	return (avgPathLengthEstimationOfBST);
}

};





#endif // TREENODE_H
