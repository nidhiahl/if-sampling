#pragma once
#include "data.h"
#include "data.cpp"
#include "ifSampling.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

ifSampling :: ifSampling(iforest &iForestObject):iForestObject(iForestObject){
	_totalPoints = iForestObject._dataObject.getnumInstances();
	_maxDistance = 1;
}

/*ifSampling :: ifSampling(iforest &iForestObject, const double & Dc):iForestObject(iForestObject), _Dc(Dc){
	_totalPoints = iForestObject._dataObject.getnumInstances();
	_maxDistance = 1;
}
*/
ifSampling::~ifSampling(){}



void ifSampling :: computeLocalMass(){
	_rho.assign(_totalPoints,0);
	for(int i = 0; i < _totalPoints-1; i++){
		for(int j = 0; j <= i; j++){
			if (_massMatrix[i][j] < _Dc){
				_rho[i]++;
				_rho[j]++;
			}
		}
	}
}



// function to compute delta and nearest neighbour with higher density for every point 
void ifSampling:: computeRelativeDistance(){
	for(int i = 0; i < _totalPoints; i++){
		_rho[i]=_rho[i]*1.0/_totalPoints;
        _ordrho.push_back({_rho[i],i});
    }
	sort(_ordrho.rbegin(),_ordrho.rend());
	
	_delta.resize(_totalPoints);
    _nneigh.resize(_totalPoints);
	
	_delta[_ordrho[0].second] = 1;
	_nneigh[_ordrho[0].second] = -1;
    _rhodelta.push_back({_ordrho[0].first * _delta[_ordrho[0].second],_ordrho[0].second});

	_delta[_ordrho[1].second] = _ordrho[0].second > _ordrho[1].second ? _massMatrix[_ordrho[0].second][_ordrho[1].second] : _massMatrix[_ordrho[1].second][_ordrho[0].second];
	_nneigh[_ordrho[1].second] = _ordrho[0].second;
    _rhodelta.push_back({_ordrho[1].first * _delta[_ordrho[1].second],_ordrho[1].second});
	
	double secondMax_rhodelta = 0;
    for(int i = 2; i < _totalPoints; i++){
		int pointi = _ordrho[i].second;
		 _delta[pointi] =_maxDistance;
        _nneigh[pointi] = i;
		double min = 1;
		int minNN=-2;
		for(int j = 0;j < i; j++){
			int pointj =_ordrho[j].second;
			double dis = pointi > pointj ? _massMatrix[pointi][pointj] : _massMatrix[pointj][pointi];
			//cout<<"dis="<<dis<<" min="<<min<<endl;			
			if(dis <= min){
				min = dis;
				minNN = pointj;
            }
        }
		_delta[pointi] = min;
		_nneigh[pointi] = minNN;
	
		_rhodelta.push_back({_ordrho[i].first * _delta[pointi],pointi});    
    }
	
}



//function to compute the potential mass based neighborhood(PMBN) given the dc/epsilon(cutoff distance)
void ifSampling::find_potential_dcNN_list(){

	_rho.resize(_totalPoints);
	_potential_dcNN_list.resize(_totalPoints);
	double treeThold = 0.01;
	long int count_distComputation=0;
	long int count_qualifyingPairs=0;
	int maxNodesinTree = this->iForestObject._iTrees[0]->_maxNumOfNodes;
	vector<vector<char>> markedNodes;
	int DcPoints = this->_Dc * this->_totalPoints;
	int numiTree = this->iForestObject._numiTrees;
	//cout<<"treeThold*numiTree="<<treeThold*numiTree<<endl;
	int neighborinAtleast = treeThold*numiTree;
	//cout<<"neighborinAtleast="<<neighborinAtleast<<endl;
	for(int treeId = 0; treeId < numiTree; treeId++)
	{	
		markedNodes.push_back(vector<char>(maxNodesinTree,'0'));
		find_markedNodes(this->iForestObject._iTrees[treeId]->treeNodes[0],markedNodes, DcPoints ,treeId);
	}
	vector<int> neighbors_of_Pointi;
	for(int pointi = 0; pointi < this->_totalPoints; pointi++)
	{	
		neighbors_of_Pointi = vector<int>(this->_totalPoints, 0);
		for(int tree = 0; tree < numiTree; tree++)
		{	
			int markednode_for_Pointi = this->iForestObject._iTrees[tree]->_pointToNode[pointi];     //initialize with the leaf node of the point and find marked node
			//cout<<"before"<<markedNodes[tree][markednodeIdforPoint]<<endl;
						
			if(markedNodes[tree][markednode_for_Pointi] == '2'){continue;}
			//cout<<"after"<<markedNodes[tree][markednode_for_Pointi]<<endl;
			while(markedNodes[tree][markednode_for_Pointi] != '1')
			{	
				markednode_for_Pointi = (int)(markednode_for_Pointi-1)/2;
			
			}
			//cout<<"pointi"<<pointi<<endl;
			//cout<<"Marked node for pointi="<<markednode_for_Pointi<<endl;
			//cout<<"[markednode_for_Pointi]->dataPointIndices.size()="<<this->iForestObject._iTrees[tree]->treeNodes[markednode_for_Pointi]->dataPointIndices.size()<<endl;
			for(auto neighborId : this->iForestObject._iTrees[tree]->treeNodes[markednode_for_Pointi]->dataPointIndices)
			{	//cout<<"neighborId"<<neighborId<<endl;
				neighbors_of_Pointi[neighborId]++;	
				//cout<<"	neighbors_of_Pointi[neighborId]="<<neighbors_of_Pointi[neighborId]<<" neighborinAtleast="<<neighborinAtleast<<endl;
				if(neighbors_of_Pointi[neighborId] == neighborinAtleast)
				{	//cout<<"first appearance"<<neighborId<<endl;
				//cout<<this->_iforest->dissScoreComputationUsingLCAlookup(point,neighborId,this->_datalen,this->_mu);
					if(pointi < neighborId){	
						count_distComputation++;
						double dissScore = iForestObject.dissScoreComputation(pointi,neighborId);
				
						if(dissScore <= _Dc){	
							count_qualifyingPairs++;
							_rho[pointi]++;
							_rho[neighborId]++;								
						}
						_potential_dcNN_list[pointi].push_back({dissScore, neighborId});
						_potential_dcNN_list[neighborId].push_back({dissScore, pointi});
					}
				}
			}
		}
		sort(_potential_dcNN_list.rbegin(),_potential_dcNN_list.rend());
	}
	cout<<"#Potential pairs"<<count_distComputation<<endl;
	cout<<"#Qualifying pairs"<<count_qualifyingPairs<<endl;
}

//function to find the special nodes that contains the potential neighborhood points
void ifSampling::find_markedNodes(treenode *node, vector<vector<char>> &mNodes , int DcPoints, int treeId)
{
	if(node->nodeMass <= DcPoints )
	{
		mNodes[treeId][node->nodeId] = '1';
	}
	else if(!node->isLeaf)
	{
		find_markedNodes(node->lChildAdd, mNodes,DcPoints, treeId);
		find_markedNodes(node->rChildAdd, mNodes,DcPoints, treeId);
	}
	else
	{
		mNodes[treeId][node->nodeId] = '2';
	}
}

void ifSampling::sRelativeDistance(){
	_delta.resize(_totalPoints,1);
    _nneigh.resize(_totalPoints,-1);
	for(int i = 0; i < _totalPoints; i++){
		//_rho[i] *=1.0/_totalPoitns;
        _ordrho.push_back({_rho[i],i});
    }
	sort(_ordrho.rbegin(),_ordrho.rend());

	_delta[_ordrho[0].second] = 1;
	_nneigh[_ordrho[0].second] = _ordrho[0].second;
	_rhodelta.push_back({_ordrho[0].first * 1,_ordrho[0].second});
	_delta[_ordrho[1].second] = iForestObject.dissScoreComputation(_ordrho[0].second,_ordrho[1].second);
	_nneigh[_ordrho[1].second] = _ordrho[0].second;
	_rhodelta.push_back({_ordrho[1].first * _delta[_ordrho[1].second],_ordrho[1].second});

	for(int point=1;point<_totalPoints; point++){
		for(int neighbor=0;neighbor<_potential_dcNN_list[point].size();neighbor++){
			auto neighborPoint = _potential_dcNN_list[point][neighbor];
			if(_rho[point]<_rho[neighborPoint.second]){
				_delta[point]=neighborPoint.first;
				_nneigh[point]=neighborPoint.second;
				neighbor=_potential_dcNN_list[point].size();
			}
		}
	}
}


void ifSampling :: assignClusterId(int k){
	_cId.resize(_totalPoints,0);
	//_clusterCenters.resize(k);
	int currentCid = 0;
	for(int i=0;i<_totalPoints;i++){
		int pointi=_ordrho[i].second;
		if(_cId[pointi]==0 && currentCid <=k){
			currentCid++;
			_clusterCenters.push_back(pointi);
			_cId[pointi]=currentCid;
			developCluster(currentCid, pointi);
		}
	}
}

void ifSampling :: developCluster(int currentCid, int pointi){
	for(int neighbor=0; neighbor<_rho[pointi]; neighbor++){
		if(_cId[neighbor] != 0){
			
		}
		else{
			if(_nneigh[neighbor]==-1){
				
			}
			else {
				_cId[neighbor] == _cId[_nneigh[neighbor]];
				if(_cId[neighbor]==currentCid){
					developCluster(currentCid,neighbor);
				}
			}
		}
	}
}


//finds the nearesr neighbor with higer density for the scalable approach (sMPC)
/*void ifSampling::sRelativeDistance(){
	 _delta.resize(_totalPoints,0);
    _nneigh.resize(_totalPoints,-1);
	//for_each(_rho.begin(), _rho.end(), [_totalPoints](double &c){ c /= _totalPoints; });
	//_rhodelta.resize(_totalPoints);
		for(int i = 0; i < _totalPoints; i++){
        _ordrho.push_back({_rho[i]*1.0/_totalPoints,i});
    }
	sort(_ordrho.rbegin(),_ordrho.rend());
	//cout<<"sRelative Distacne"<<endl;
	//cout<<_totalPoints<<endl;
	_delta[_ordrho[0].second] = 1;
	_nneigh[_ordrho[0].second] = _ordrho[0].second;
	_rhodelta.push_back({_ordrho[0].first * 1,_ordrho[0].second});
	_delta[_ordrho[1].second] = iForestObject.dissScoreComputation(_ordrho[0].second,_ordrho[1].second);
	_nneigh[_ordrho[1].second] = _ordrho[0].second;
	_rhodelta.push_back({_ordrho[1].first * _delta[_ordrho[1].second],_ordrho[1].second});
	//cout<<"point0 and point1 initialization done"<<endl;
	for(int i=2; i<_ordrho.size();i++){
		double nndist = 1;
		int nn = -1;
		int pointi = _ordrho[i].second;
		//cout<<"pointi="<<pointi<<endl;
		for(int j = 0; j < i; j++){
			int pointj = _ordrho[j].second; 
			//cout<<"pointj="<<pointj<<endl;
		
			auto itr = _potential_dcNN_list[pointi].find(pointj);
			if(itr != _potential_dcNN_list[pointi].end()){
				//cout<<"pointj belongs to PMBN of pointi"<<endl;
				if(nndist > (*itr).second){
					nndist = (*itr).second;
					nn = pointj;
				} 
			}
		}
		//_delta[pointi] = nndist;
		//_nneigh[pointi] = nn;
		//cout<<"_nneigh[pointi]="<<_nneigh[pointi]<<endl;
		//cout<<"nn="<<nn<<endl;
		if(nn==-1){
		//cout<<"pointj does not belongs to PMBN of pointi"<<endl;
				
			for(int j = 0; j < i; j++){
				int pointj = _ordrho[j].second;
				//auto itr = iForestObject._potential_dcNN_list[iti]).find(itj);
				double disScore = iForestObject.dissScoreComputation(pointi,pointj);
				if(nndist > disScore){
					nndist = disScore;
					nn = pointj;
				} 
			}
		}
		_delta[pointi] = nndist;
		_nneigh[pointi] = nn;
		_rhodelta.push_back({_ordrho[pointi].first*_delta[pointi],pointi});
		
	}
}*/







void ifSampling :: find_k_clustercenters(int k){
	sort(_rhodelta.rbegin(),_rhodelta.rend());
	_clusterCenters.resize(0); 
	//cout<<"cluster centre identification"<<endl;
	for(int i = 0; i < k; i++){
		//cout<<i<<"th cluster center="<<endl;
		//cout<<_rhodelta[i].second<<endl;
		_clusterCenters.push_back(_rhodelta[i].second);
	}
}

int ifSampling :: numberOfClusterCenters(){

	double minStdDevDiff;
	StdDevCalc clusCenters;
	int i;
	for(i = 0; i < _totalPoints; i++){
			StdDevCalc nonClusCenters;
			int max_id = i;
			int mxChngecount=0;
			int mxNOTChngecount=0;
			for(int j = i+1; j < _totalPoints; j++){
				//cout<<" _ordrho[i].second="<<_ordrho[0].second<<" _ordrho[j].second="<<_ordrho[j].second<<endl; 
				//cout<<"_rhodelta[j].first="<<_rhodelta[j].first<<endl;
    			//cout<<"_rhodelta[max_id].first="<<_rhodelta[max_id].first<<endl;
    
				if(_rhodelta[j].first > _rhodelta[max_id].first){
					mxChngecount++;
					//cout<<"Max changs="<<mxChngecount<<endl;_rhodelta[max_id].first==0?0:log10(_rhodelta[max_id].first)
					//nonClusCenters.append(log10(max(0.000000000000000001,_rhodelta[max_id].first)));
					nonClusCenters.append(_rhodelta[max_id].first==0?0:log10(_rhodelta[max_id].first));
					//cout<<"11111111nonClusCenters.get_std_dev()="<<nonClusCenters.get_std_dev()<<endl;
					max_id = j;
				
				}else{
					mxNOTChngecount++;
					//cout<<"Max no change changs="<<mxNOTChngecount<<endl;
					//nonClusCenters.append(log10(max(0.000000000000000001,_rhodelta[j].first)));
					nonClusCenters.append(_rhodelta[j].first==0?0:log10(_rhodelta[j].first));
					//cout<<"log10(_rhodelta[j].first)="<<log10(_rhodelta[j].first)<<endl;
					//cout<<"2222222222222nonClusCenters.get_std_dev()="<<nonClusCenters.get_std_dev()<<endl;	
				}
		}
		//clusCenters.append(log10(max(0.000000000001,_rhodelta[max_id].first)));
		clusCenters.append(_rhodelta[max_id].first==0?0:log10(_rhodelta[max_id].first));
					if(max_id!=i){
				//cout<<"_rhodelta[max_id].second="<<_rhodelta[max_id].second<<" _rhodelta[i].second="<<_rhodelta[i].second<<endl;
				swap(_rhodelta[max_id],_rhodelta[i]);
				//cout<<"_rhodelta[max_id].second="<<_rhodelta[max_id].second<<" _rhodelta[i].second="<<_rhodelta[i].second<<endl;
				
		}
		//cout<<"Max changs="<<mxChngecount<<endl;
	//cout<<"Max no change changs="<<mxNOTChngecount<<endl;
		//cout<<"clusCenters.get_std_dev()="<<clusCenters.get_std_dev()<<endl;
		//cout<<"33333333333nonClusCenters.get_std_dev()="<<nonClusCenters.get_std_dev()<<endl;
		//cout<<"FOR i="<<i<<":  non-ceters : "<<nonClusCenters.get_std_dev()<<" centers : "<< clusCenters.get_std_dev()<<"  difference:"<<abs(clusCenters.get_std_dev() - nonClusCenters.get_std_dev())<<endl;
		if(i==0){
			minStdDevDiff = abs(clusCenters.get_std_dev() - nonClusCenters.get_std_dev());
			_clusterCenters.push_back(_rhodelta[i].second);
			continue;		
		}
		else{
			if(minStdDevDiff > abs(clusCenters.get_std_dev() - nonClusCenters.get_std_dev())){
				minStdDevDiff = abs(clusCenters.get_std_dev() - nonClusCenters.get_std_dev());
				_clusterCenters.push_back(_rhodelta[i].second);
			}else{
				return (i);
			}
		}
	}
	return (i);
} 




void ifSampling :: clusterAssignment(){
	int numOfClusters = _clusterCenters.size();
	_clusters.resize(numOfClusters);
	_cId.resize(_totalPoints,-1);
	for(int cId = 0; cId < numOfClusters; cId++){
		int cc = _clusterCenters[cId];
		_cId[cc] = cId+1;
		_clusters[cId].push_back(cc);
	}
	for(int i = 0; i < _totalPoints; i++){
	
		
		int currPoint = _ordrho[i].second;
		//cout<<"cc assignment, point="<<currPoint<<endl;
		//cout<<"_cId[i]="<<_cId[currPoint]<<endl;
		//cout<<"_nneigh[i]="<<_nneigh[currPoint]<<endl;
		if(_cId[currPoint] < 0){ 
			_cId[currPoint] = _cId[_nneigh[currPoint]];
			_clusters[_cId[currPoint]-1].push_back(currPoint);
		}	
	}
	
}



vector<double> ifSampling :: getDeltaVector() const{
    return this->_delta;
}
vector<int> ifSampling :: getNeighbourVector() const{
    return this->_nneigh;
}

vector<pair<double,int>> ifSampling :: getOrdRho(){
    return this->_ordrho;
}


