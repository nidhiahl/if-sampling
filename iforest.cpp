#include "iforest.h"
#include "itree.cpp"
#include <unordered_set>
#include <queue>
#include <math.h>
using namespace std;



iforest::iforest(const data & dataObject, int numiTrees, int sampleSize): _dataObject(dataObject), _numiTrees(numiTrees), _sampleSize(sampleSize){
	int totalPoints = _dataObject.getnumInstances();	
	if(_sampleSize < 1){
		_sampleSize = totalPoints * _sampleSize < 256 ? 256 : _sampleSize;
    }
	_sampleSize = totalPoints < _sampleSize ? totalPoints : _sampleSize;	

	
	_maxTreeHeight = (int)log2(_sampleSize);
	_maxNumOfNodes = (int)pow(2.0,_maxTreeHeight+1)-1;
	_iTrees.resize(_numiTrees);	
	
}


iforest::~iforest(){}


void iforest::fit(){

	constructiForest();
	
	computeNodeMass();
	
}




void iforest::constructiForest(){
    for(int treeId = 0; treeId < _numiTrees; treeId++){
		_iTrees[treeId] = new itree(_dataObject, _sampleSize, _maxTreeHeight, _maxNumOfNodes);
		_iTrees[treeId]->constructiTree();
	}
}



void iforest::computeNodeMass(){
	for(int treeId = 0; treeId < this->_numiTrees; treeId++){
		//cout<<"treeId="<<treeId<<endl;
        _iTrees[treeId]->computeNodeMassforTree();
    }   
}

void iforest::write_smallest_largest_leaf(const string & dataset){
	ofstream write_leaf_size(dataset+"/intermediatefiles/leaf_size.csv",ios::out|ios::binary);
	write_leaf_size<<"treeId "<<"smallest_leaf "<<"largest_leaf"<<endl;
	float totalPoints = _dataObject.getnumInstances();
	for(int treeId = 0; treeId < this->_numiTrees; treeId++){
        write_leaf_size<<treeId<<" "<<_iTrees[treeId]->smallest_leaf/totalPoints<<" "<<_iTrees[treeId]->largest_leaf/totalPoints<<endl;
    } 
	write_leaf_size.close();
}


int iforest::computeLCA(int node1,int node2){
	 while(node1!=node2){
        if(node1>node2){node1 = node1%2==0 ? (node1/2)-1 : (node1-1)/2;}
        else{node2 = node2%2==0 ? (node2/2)-1 : (node2-1)/2;}
    }
    return node1;
}

void iforest::computeLCA_lookup(){
	//cout<<"LCA lookup start"<<endl;
	LCA_lookup.resize(_maxNumOfNodes);
	for(int node1=0;node1<_maxNumOfNodes-1;node1++){
		for(int node2=0;node2<=node1;node2++){
			int nodea = node1;
			int nodeb = node2;
			while(nodea!=nodeb){
        		if(nodea>nodeb){nodea = nodea%2==0 ? (nodea/2)-1 : (nodea-1)/2;}
        		else{nodeb = nodeb%2==0 ? (nodeb/2)-1 : (nodeb-1)/2;}
    		}
			//cout<<"node1="<<node1<<" node2="<<node2<<"LCA="<<nodea<<endl;
			LCA_lookup[node1].push_back(nodea);
		}	
	}
	//cout<<"LCA lookup done"<<endl;
}

double iforest::dissScoreComputation(int point1,int point2){
	//cout<<"dissSCore computation"<<endl;
	double tempMass = 0;
	int totalPoints = _dataObject.getnumInstances();
	//cout<<"totlaPoints="<<totalPoints<<endl;
	//cout<<"_iTrees.size()="<<_iTrees.size()<<endl;
	for(int treeId = 0; treeId < _iTrees.size(); treeId++){
				//cout<<"
				//cout<<"leafNodeforPoint1="<<_iTrees[treeId]->_pointToNode[point1]<<endl;
				int leafNodeforPoint1 = _iTrees[treeId]->_pointToNode[point1];
                //cout<<"leafNodeforPoint2="<<_iTrees[treeId]->_pointToNode[point2]<<endl;
				int leafNodeforPoint2 = _iTrees[treeId]->_pointToNode[point2];
                //int LCAnodeforPoint1_Point2 = computeLCA(leafNodeforPoint1,leafNodeforPoint2);
                int LCAnodeforPoint1_Point2 = leafNodeforPoint1>leafNodeforPoint2?LCA_lookup[leafNodeforPoint1][leafNodeforPoint2]:LCA_lookup[leafNodeforPoint2][leafNodeforPoint1];
				//cout<<"LCAnodeforPoint1_Point2="<<LCAnodeforPoint1_Point2<<" "<<_iTrees[treeId]->_pointToNode[point1]<<" "<<_iTrees[treeId]->_pointToNode[point2]<<endl;
				tempMass += _iTrees[treeId]->_nodeMass[LCAnodeforPoint1_Point2];
                
                
	}
	tempMass = tempMass/_numiTrees;
	tempMass = tempMass/totalPoints;
	return tempMass;
}


void iforest::computeMassMatrix(vector<vector<double>> &massMatrix){   
	computeLCA_lookup();     
    int totalPoints = _dataObject.getnumInstances();
	//cout<<"mass matrix computationstart"<<endl;
	massMatrix.resize(totalPoints);
    for(int point1 = 0; point1 < totalPoints; point1++){
        for(int point2 = 0; point2 <= point1; point2++)
        {
	    	massMatrix[point1].push_back(dissScoreComputation(point1,point2));
        }
	}
	//cout<<"mass matrix computation done"<<endl;
}











