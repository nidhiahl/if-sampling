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
		//_iTrees[treeId] = new itree(_dataObject, _sampleSize, _maxTreeHeight, _maxNumOfNodes);
		//_iTrees[treeId]->constructiTree();
	}
}

void insertAllPoints(){
	for(int treeId = 0; treeId < this->_numiTrees; treeId++){
		//cout<<"treeId="<<treeId<<endl;
       // _iTrees[treeId]->computeNodeMassforTree();
    }   
}

	public:
    
    //private:
    int _numiTrees;										//number of iTrees in the iForest.
	int _sampleSize;									//sample size representing the iForest.
    int _maxTreeHeight;									//max Height of each iTree in iForest.
  	int _maxNumOfNodes;									//max number of node possible in each iTree.
	const data & _dataObject;
	vector<itree*> _iTrees;								//list of pointers to the iTrees in the forest.




};
#endif // IFOREST_H
