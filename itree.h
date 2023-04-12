#ifndef ITREE_H
#define ITREE_H
#include "data.h"
#include "treenode.h"
#include <vector>
#include <math.h>
#include <queue>
#include <stack>

class itree
{
public:
        
treenode * rootNode;
vector<int> _pointToNode;   //stores leaf node associated with each point in the dataset.
vector<treenode*> treeNodes;
vector<int> _leafNodes;
int _sampleSize;    
int _maxTreeHeight;
int _maxNumOfNodes;
const data & _dataObject;

itree(const data & dataObject): _dataObject(dataObject){}

itree(const data & dataObject, int sampleSize, int maxTreeHeight, int maxNumOfNodes): _dataObject(dataObject), _sampleSize(sampleSize), _maxTreeHeight(maxTreeHeight), _maxNumOfNodes(maxNumOfNodes){}

virtual ~itree(){}

void constructiTree(){
	//cout<<"constructiTree"<<endl;
	_leafNodes.resize(0);
	treeNodes.resize(_maxNumOfNodes,nullptr);
	//cout<<"treenodes resize"<<endl;
    rootNode = new treenode(0);
	//cout<<"root node created"<<endl;
	treeNodes[0] = rootNode;
	//cout<<"treenode[0] initialized with root node"<<endl;
    rootNode->dataPointIndices = _dataObject.getSample(_sampleSize);
	//cout<<"samples picked from data"<<endl;
	if(treeNodes[0]->dataPointIndices.size() == 0){
		treeNodes[0] = nullptr;
	}
	//cout<<"root node created and populated with the smaple"<<endl;
	treenode *currNode;
	for(int nodeId =0; nodeId < _maxNumOfNodes; nodeId++){
		 currNode = treeNodes[nodeId];
		if(currNode==nullptr){
			continue;
		}

		currNode->nodeSize = currNode->dataPointIndices.size();
		if(currNode->nodeSize <=1 || currNode->nodeHeight == _maxTreeHeight){
			currNode->isLeaf = bool(1);
			currNode->computeAveragePLOfNode();
        	currNode->dataPointIndices.clear();
        	currNode->dataPointIndices.resize(0);
			_leafNodes.push_back(currNode->nodeId);
		}
		else{
			//currNode->splitValue = currNode->PGIFsplitInfoSelection(_dataObject);
			currNode->splitValue = currNode->splitInfoSelection(_dataObject);
    		currNode->createLeftChild();
			currNode->createRightChild();
			for(int i=0; i<currNode->nodeSize; i++){     
            	if(_dataObject.dataVector[currNode->dataPointIndices[i]]->attributes[currNode->splitAttribute]<currNode->splitValue){
               		currNode->lChildAdd->dataPointIndices.push_back(currNode->dataPointIndices[i]);
            	}
            	else{
            		currNode->rChildAdd->dataPointIndices.push_back(currNode->dataPointIndices[i]);
            	}

        	}
			treeNodes[2*nodeId+1] = currNode->lChildAdd;
			treeNodes[2*nodeId+2] = currNode->rChildAdd;
			
       		currNode->dataPointIndices.clear();
       		currNode->dataPointIndices.resize(0);
    	}
	}	
}



void insertAllPoints(){
	int numOfPointsPresent = _dataObject.getnumInstances();
	_pointToNode.resize(numOfPointsPresent,-1);
	//_nodeMass.resize(_maxNumOfNodes, -1);
	rootNode->dataPointIndices.resize(0);
	//smallest_leaf = numOfPointsPresent;
	//largest_leaf = 0;
	for(int i = 0; i < numOfPointsPresent; i++){
			rootNode->dataPointIndices.push_back(i);
	}
	//rootNode->nodeMass = numOfPointsPresent;
	treenode *currNode;
	for(int nodeId = 0; nodeId < _maxNumOfNodes; nodeId++){
		currNode = treeNodes[nodeId];
		if(currNode==nullptr){
			continue;
		}
		currNode->nodeMass = currNode->dataPointIndices.size();
		
		if(currNode->isLeaf){
            for(int in = 0; in < currNode->dataPointIndices.size(); in++){
                _pointToNode[currNode->dataPointIndices[in]] = currNode->nodeId;
            }
            continue;
        }
        //currNode->lChildAdd->dataPointIndices.resize(0);
		//currNode->rChildAdd->dataPointIndices.resize(0);
		

		for(int i=0; i<currNode->dataPointIndices.size(); i++){
			if(_dataObject.dataVector[currNode->dataPointIndices[i]]->attributes[currNode->splitAttribute] < currNode->splitValue){
            	 if(currNode->lChildAdd == nullptr){
            	 	currNode->createLeftChild();
            	 	currNode->lChildAdd->isLeaf = bool(1);
            	 }
                currNode->lChildAdd->dataPointIndices.push_back(currNode->dataPointIndices[i]);
            }
            else{
                if(currNode->rChildAdd == nullptr){
                	currNode->createRightChild();
                	currNode->rChildAdd->isLeaf = bool(1);
                }
                currNode->rChildAdd->dataPointIndices.push_back(currNode->dataPointIndices[i]);
            }
        }
        //currNode->dataPointIndices.clear();
		//currNode->dataPointIndices.resize(0);
	}

}
		
};




#endif // ITREE_H
