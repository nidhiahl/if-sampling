#include "itree.h"
#include "treenode.cpp"
#include <math.h>
#include <queue>
#include <stack>


itree::itree(const data & dataObject): _dataObject(dataObject){}

itree::itree(const data & dataObject, int sampleSize, int maxTreeHeight, int maxNumOfNodes): _dataObject(dataObject), _sampleSize(sampleSize), _maxTreeHeight(maxTreeHeight), _maxNumOfNodes(maxNumOfNodes){}

itree::~itree(){}
//*************************************************STATIC iTree creation*******************************************************************//
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

/*void itree::constructiTree(){
	treeNodes.resize(_maxNumOfNodes);
    rootNode = new treenode(0);
	treeNodes[0] = rootNode;
    rootNode->dataPointIndices = _dataObject.getSample(_sampleSize);
    queue<treenode*> BFTforNodes;
    BFTforNodes.push(rootNode);
    while(!BFTforNodes.empty()){
    	treenode *currNode = BFTforNodes.front();
		BFTforNodes.pop();
		if(currNode){
			currNode->nodeSize = currNode->dataPointIndices.size();
			if(currNode->nodeSize <=1 || currNode->nodeHeight ==_maxTreeHeight){
    			currNode->isLeaf = bool(1);
        		currNode->dataPointIndices.clear();
        		currNode->dataPointIndices.resize(0);
    		}
    		else{
    			currNode->splitValue = currNode->PGIFsplitInfoSelection(_dataObject);
    			//currNode->splitValue = currNode->splitInfoSelection(_dataObject);
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
        		
				treeNodes[currNode->lChildAdd->nodeId] = currNode->lChildAdd;
				treeNodes[currNode->rChildAdd->nodeId] = currNode->rChildAdd;
				
        		currNode->dataPointIndices.clear();
        		currNode->dataPointIndices.resize(0);
        		
        		BFTforNodes.push(currNode->lChildAdd);
        		BFTforNodes.push(currNode->rChildAdd);
    		}
    	}
    }
}*/

//***********************************************************************************************************************************/

/*void itree::computeNodeMassforTree(){
	int numOfPointsPresent = _dataObject.getnumInstances();
	_pointToNode.resize(numOfPointsPresent,-1);
	_nodeMass.resize(_maxNumOfNodes, -1);
	rootNode->dataPointIndices.resize(0);
	smallest_leaf = numOfPointsPresent;
	largest_leaf = 0;
	//cout<<"nodemass computation"<<endl;
	treeNodes[0]->dataPointIndices = _dataObject.getSample(_sampleSize);
	//cout<<"treeNodes[0]->dataPointIndices.size()="<<treeNodes[0]->dataPointIndices.size()<<endl;
	treeNodes[0]->nodeMass = numOfPointsPresent;
    treenode *currNode;
	//cout<<"before for loop"<<endl;
    for(int nodeId = 0; nodeId < _maxNumOfNodes; nodeId++){
    	if(treeNodes[nodeId] == nullptr){continue;}
		currNode = treeNodes[nodeId];
		//cout<<"currNode="<<currNode<<endl;
		currNode->nodeMass = currNode->dataPointIndices.size();
		//cout<<"nodeId="<<currNode->nodeId<<" is marked node with mass="<<currNode->nodeMass/numOfPointsPresent<<endl;
			//cout<<"nodeId="<<currNode->nodeId<<" is marked node with mass="<<currNode->nodeMass<<endl;
			//cout<<"nodeId="<<currNode->nodeId<<" is marked node with mass="<<currNode->dataPointIndices.size()<<endl;
			
		_nodeMass[currNode->nodeId]=currNode->nodeMass;
		//cout<<"before if liaf nodeisLeaf="<<currNode->isLeaf<<endl;
		if(currNode->isLeaf){
			//cout<<"nodeis leaf, nodeId="<<currNode->nodeId<<endl;
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
                _pointToNode[currNode->dataPointIndices[in]] = nodeId;
            }
			//cout<<"done with leaf id="<<currNode->nodeId<<endl;
            continue;
        }
        //currNode->lChildAdd->dataPointIndices.resize(0);
		//currNode->rChildAdd->dataPointIndices.resize(0);
		//cout<<"currNode->dataPointIndices.size()="<<currNode->dataPointIndices.size()<<endl;
		for(int i=0; i<currNode->dataPointIndices.size(); i++){
			//cout<<"_dataObject.dataVector[currNode->dataPointIndices[i]]->attributes[currNode->splitAttribute] < currNode->splitValue="<<(_dataObject.dataVector[currNode->dataPointIndices[i]]->attributes[currNode->splitAttribute] < currNode->splitValue) << endl;
			if(_dataObject.dataVector[currNode->dataPointIndices[i]]->attributes[currNode->splitAttribute] < currNode->splitValue){
				
            	 if(currNode->lChildAdd == nullptr){
            	 	currNode->createLeftChild();
					treeNodes[2*nodeId+1] = currNode->lChildAdd;
            	 	currNode->lChildAdd->isLeaf = bool(1);
            	 }
                currNode->lChildAdd->dataPointIndices.push_back(currNode->dataPointIndices[i]);
            }
            else{
                if(currNode->rChildAdd == nullptr){
                	currNode->createRightChild();
					treeNodes[2*nodeId+2] = currNode->rChildAdd;
                	currNode->rChildAdd->isLeaf = bool(1);
                }
                currNode->rChildAdd->dataPointIndices.push_back(currNode->dataPointIndices[i]);
            }
        }
        currNode->dataPointIndices.clear();
		currNode->dataPointIndices.resize(0);

}

}*/



void itree::computeNodeMassforTree(){
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


void itree::find_markedNodes(int treeId,double dc, vector<vector<int>> &markedNodes){
	//cout<<"itree::find_markedNodes"<<endl;
	double totalPoints = _dataObject.getnumInstances();
	_pointTomarkedNode.resize(totalPoints,-1);
	stack<int> dfsNodes;
	rootNode->dataPointIndices.resize(0);
	//cout<<"rootNode->dataPointIndices.size()="<<rootNode->dataPointIndices.size()<<endl;
	for(int i = 0; i < totalPoints; i++){
			rootNode->dataPointIndices.push_back(i);
	}
	//cout<<"rootNode->dataPointIndices.size()="<<rootNode->dataPointIndices.size()<<endl;
	
	dfsNodes.push(0);
	while(!dfsNodes.empty()){
		int currNodeId = dfsNodes.top();
		dfsNodes.pop();
		//cout<<"nodeId="<<currNodeId<<endl;
			//cout<<"nodeId="<<currNodeId<<" is marked node with mass="<<treeNodes[currNodeId]->nodeMass/totalPoints<<endl;
			//cout<<"nodeId="<<currNodeId<<" is marked "<<treeNodes[currNodeId]->splitValue<<"node with mass="<<treeNodes[currNodeId]->splitAttribute<<endl;
			//cout<<"nodeId="<<currNodeId<<" is marked node with mass="<<treeNodes[currNodeId]->dataPointIndices.size()<<endl;
			
		if(treeNodes[currNodeId]->nodeMass/totalPoints <= dc || treeNodes[currNodeId]->isLeaf){
			markedNodes[treeId].push_back(currNodeId);
			//cout<<"nodeId="<<currNodeId<<" is marked node with mass="<<treeNodes[currNodeId]->nodeMass/totalPoints<<endl;
			//cout<<"nodeId="<<currNodeId<<" is marked "<<treeNodes[currNodeId]->splitValue<<"node with mass="<<treeNodes[currNodeId]->splitAttribute<<endl;
			//cout<<"nodeId="<<currNodeId<<" is marked node with mass="<<treeNodes[currNodeId]->dataPointIndices.size()<<endl;
			for(int in = 0; in < treeNodes[currNodeId]->dataPointIndices.size(); in++){
                _pointTomarkedNode[treeNodes[currNodeId]->dataPointIndices[in]] = currNodeId;
            }
			//cout<<"node is leaf="<<treeNodes[currNodeId]->isLeaf<<endl;
			continue;
		}
		
		

		treeNodes[currNodeId]->lChildAdd->dataPointIndices.clear();
		treeNodes[currNodeId]->rChildAdd->dataPointIndices.clear();
		treeNodes[currNodeId]->lChildAdd->dataPointIndices.resize(0);
		treeNodes[currNodeId]->rChildAdd->dataPointIndices.resize(0);
		//cout<<"currNode left child and right child->dataPointIndices.size()="<<treeNodes[2*currNodeId+1]->dataPointIndices.size()<<" "<<treeNodes[2*currNodeId+2]->dataPointIndices.size()<<endl;
		for(int i=0; i<treeNodes[currNodeId]->dataPointIndices.size(); i++){
			if(_dataObject.dataVector[treeNodes[currNodeId]->dataPointIndices[i]]->attributes[treeNodes[currNodeId]->splitAttribute] < treeNodes[currNodeId]->splitValue){
	
            	 
                treeNodes[currNodeId]->lChildAdd->dataPointIndices.push_back(treeNodes[currNodeId]->dataPointIndices[i]);
            }
            else{
                
                treeNodes[currNodeId]->rChildAdd->dataPointIndices.push_back(treeNodes[currNodeId]->dataPointIndices[i]);
            }
        }
		//cout<<"currNode left child and right child->dataPointIndices.size()="<<treeNodes[2*currNodeId+1]->dataPointIndices.size()<<" "<<treeNodes[2*currNodeId+2]->dataPointIndices.size()<<endl;
		
        treeNodes[currNodeId]->dataPointIndices.clear();
		treeNodes[currNodeId]->dataPointIndices.resize(0);
		
		dfsNodes.push(2*currNodeId+2);
		dfsNodes.push(2*currNodeId+1);
	}
	
}




















