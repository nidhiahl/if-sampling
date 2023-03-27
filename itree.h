#ifndef ITREE_H
#define ITREE_H
#include "data.h"
#include "treenode.h"
#include <vector>


class itree
{
    public:
        itree(const data &);
        itree(const data &,int, int, int);
        void constructiTree();
        void insertAllPoints();
		treenode * rootNode;
		vector<int> _pointToNode;   //stores leaf node associated with each point in the dataset.
		vector<treenode*> treeNodes;
		virtual ~itree();
        
        
    //protected:

    //private:
	int _sampleSize;    
	int _maxTreeHeight;
    int _maxNumOfNodes;
	const data & _dataObject;
		
};

itree::itree(const data & dataObject): _dataObject(dataObject){}

itree::itree(const data & dataObject, int sampleSize, int maxTreeHeight, int maxNumOfNodes): _dataObject(dataObject), _sampleSize(sampleSize), _maxTreeHeight(maxTreeHeight), _maxNumOfNodes(maxNumOfNodes){}

itree::~itree(){}



void itree::constructiTree(){
	treeNodes.resize(_maxNumOfNodes,nullptr);
    rootNode = new treenode(0);
	treeNodes[0] = rootNode;
    rootNode->dataPointIndices = _dataObject.getSample(_sampleSize);
	if(treeNodes[0]->dataPointIndices.size() == 0){
		treeNodes[0] = nullptr;
	}
	treenode *currNode;
	for(int nodeId =0; nodeId < _maxNumOfNodes; nodeId++){
		 currNode = treeNodes[nodeId];
		if(currNode==nullptr){
			continue;
		}

		currNode->nodeSize = currNode->dataPointIndices.size();
		if(currNode->nodeSize <=1 || currNode->nodeHeight == _maxTreeHeight){
			currNode->isLeaf = bool(1);
        	currNode->dataPointIndices.clear();
        	currNode->dataPointIndices.resize(0);
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



void itree::insertAllPoints(){
	int numOfPointsPresent = _dataObject.getnumInstances();
	_pointToNode.resize(numOfPointsPresent,-1);
	_nodeMass.resize(_maxNumOfNodes, -1);
	rootNode->dataPointIndices.resize(0);
	smallest_leaf = numOfPointsPresent;
	largest_leaf = 0;
	for(int i = 0; i < numOfPointsPresent; i++){
			rootNode->dataPointIndices.push_back(i);
	}
	rootNode->nodeMass = numOfPointsPresent;
    queue<treenode*> BFTforNodes;
    BFTforNodes.push(rootNode);
    while(!BFTforNodes.empty()){
    	treenode *currNode = BFTforNodes.front();
		BFTforNodes.pop();
		currNode->nodeMass = currNode->dataPointIndices.size();
		//cout<<"nodeId="<<currNode->nodeId<<" is marked node with mass="<<currNode->nodeMass/numOfPointsPresent<<endl;
			//cout<<"nodeId="<<currNode->nodeId<<" is marked node with mass="<<currNode->nodeMass<<endl;
			//cout<<"nodeId="<<currNode->nodeId<<" is marked node with mass="<<currNode->dataPointIndices.size()<<endl;
			
		_nodeMass[currNode->nodeId]=currNode->nodeMass;
		if(currNode->isLeaf){
			//cout<<currNode->nodeId<<"->dataPointIndices.size();"<<currNode->dataPointIndices.size()<<endl;
			if(smallest_leaf>currNode->dataPointIndices.size()){
				//cout<<"smallestleaf="<<smallest_leaf<<endl;
				smallest_leaf = currNode->dataPointIndices.size();
				//cout<<"smallestleaf="<<smallest_leaf<<endl;
			}
            if(largest_leaf<currNode->dataPointIndices.size()){
				//cout<<"largestleaf="<<largest_leaf<<endl;
				largest_leaf = currNode->dataPointIndices.size();
				//cout<<"largestleaf="<<largest_leaf<<endl;
				
			}
            for(int in = 0; in < currNode->dataPointIndices.size(); in++){
                _pointToNode[currNode->dataPointIndices[in]] = currNode->nodeId;
            }
            continue;
        }
        currNode->lChildAdd->dataPointIndices.resize(0);
		currNode->rChildAdd->dataPointIndices.resize(0);
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
        		
   		BFTforNodes.push(currNode->lChildAdd);
   		BFTforNodes.push(currNode->rChildAdd);

}

}



#endif // ITREE_H
