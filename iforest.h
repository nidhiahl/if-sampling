#ifndef IFOREST_H
#define IFOREST_H
#include <map>
#include <unordered_map>
#include "data.h"
#include "itree.h"
#include <unordered_set>
#include <queue>
#include <math.h>

class iforest
{

public:
//iforest(){}
//iforest(const data & dataObject, int numiTrees, int sampleSize): _dataObject(dataObject), _numiTrees(numiTrees), _sampleSize(sampleSize){}
iforest(const data & dataObject, int numiTrees, int sampleSize): _dataObject(dataObject), _numiTrees(numiTrees), _sampleSize(sampleSize){
	int totalPoints = _dataObject.getnumInstances();	
	if(_sampleSize < 1){
		_sampleSize = totalPoints * _sampleSize < 256 ? 256 : _sampleSize;
    }
	_sampleSize = totalPoints < _sampleSize ? totalPoints : _sampleSize;	
	_maxTreeHeight = (int)log2(_sampleSize);
	_maxNumOfNodes = (int)pow(2.0,_maxTreeHeight+1)-1;
	_iTrees.resize(_numiTrees);		
}

virtual ~iforest(){}

void fit(){
	constructiForest();	
	insertAllPoints();
}



void constructiForest(){
    for(int treeId = 0; treeId < _numiTrees; treeId++){
		//cout<<"treeId="<<treeId<<endl;
		_iTrees[treeId] = new itree(_dataObject, _sampleSize, _maxTreeHeight, _maxNumOfNodes);
		_iTrees[treeId]->constructiTree();
	}
}

void insertAllPoints(){
	for(int treeId = 0; treeId < this->_numiTrees; treeId++){
		//cout<<"treeId="<<treeId<<endl;
       _iTrees[treeId]->insertAllPoints();
    }   
}
void computeAnomalyScore(){
	int totalPoints = _dataObject.getnumInstances();
	long double avgPLEstimation = avgPathLengthEstimationOfBST();
	long double avgPathLength;
	long double AS;
	for(int point=0; point<totalPoints; point++){
		avgPathLength = computeAvgPathLength(point);
		AS = pow(2,-(avgPathLength/avgPLEstimation));
		cout<<point<<" "<<AS<<" "<<_dataObject.dataVector[point]->label<<endl;
		anomalyScore.push_back({AS,point});
	}
}

long double avgPathLengthEstimationOfBST(){
	long double avgPathLengthEstimationOfBST = 0;
	if(_sampleSize == 2){avgPathLengthEstimationOfBST=1;}
	else if(_sampleSize > 2){avgPathLengthEstimationOfBST = (2 * (log(_sampleSize-1) + 0.5772156649)) - (2 * (_sampleSize - 1) / _sampleSize);}	
	return (avgPathLengthEstimationOfBST);
}

long double computeAvgPathLength(int point){
	long double avgPathLength = 0;
	for(int treeId = 0; treeId < _numiTrees; treeId++){
		avgPathLength += _iTrees[treeId]->treeNodes[_iTrees[treeId]->_pointToNode[point]]->averagePLOfNode;
	}
	avgPathLength /=_numiTrees;
	return avgPathLength;
}
	public:
    
    //private:
    int _numiTrees;										//number of iTrees in the iForest.
	int _sampleSize;									//sample size representing the iForest.
    int _maxTreeHeight;									//max Height of each iTree in iForest.
  	int _maxNumOfNodes;									//max number of node possible in each iTree.
	const data & _dataObject;
	vector<itree*> _iTrees;								//list of pointers to the iTrees in the forest.
	vector<pair<double, int>> anomalyScore;



};
#endif // IFOREST_H
