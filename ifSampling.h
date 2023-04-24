#ifndef ifSampling_H
#define ifSampling_H
#pragma once
#include "data.h"
#include <sstream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <fstream>






class ifSampling
{
public:
	const iforest & _iforestObject;  
	const double _aScoreTh;   
	unordered_set<int> sampleSet;
	vector<vector<float>> discarding_threshold_for_leaf;
	vector<bool> tree_picked_first_time;
	vector<vector<int>> count_already_picked_points;
	const string _tree_criterion;
	const string _leaf_criterion;
	const string _point_criterion;
	double _sample_size;
	int _picked_treeId;
	int _picked_leaf_index;
	int _picked_pointId;
	
public:
	ifSampling( iforest & iforestObject, double anomalyScoreThreshold, string tree_criterion, string leaf_criterion,string point_criterion, double sample_size): _iforestObject(iforestObject), _aScoreTh(anomalyScoreThreshold), _tree_criterion(tree_criterion), _leaf_criterion(leaf_criterion), _point_criterion(point_criterion), _sample_size(sample_size){
		tree_picked_first_time.resize(_iforestObject._numiTrees,bool(1));
		//cout<<_sample_size<<endl;
	}

	virtual ~ifSampling(){}
	
	unordered_set<int> & sample(){
		int sampleSize=_iforestObject._dataObject.getnumInstances() * _sample_size;	
	
		if(_point_criterion == "core_points"||"border_points"){
			count_already_picked_points.resize(_iforestObject._numiTrees);
		}
		if(_leaf_criterion == "hard_discard"||"soft_discard"){
			discarding_threshold_for_leaf.resize(_iforestObject._numiTrees);
		}
			
		while(sampleSet.size() < sampleSize){
			//cout<<"\nsample number="<<sampleSet.size()<<"-------------";
			itree * picked_tree = pickTree();
			if(tree_picked_first_time[picked_tree->_treeId]){
				tree_picked_first_time[picked_tree->_treeId]=bool(0);
				discarding_threshold_for_leaf[picked_tree->_treeId].resize(picked_tree->_leafNodes.size(), 0.0);
				if(_leaf_criterion == "hard_discard"){
					hard_discard_noisy_leaf(picked_tree);
				}else if(_leaf_criterion=="soft_discard"){
					soft_discard_noisy_leaf(picked_tree);
				}else{}
					if(_point_criterion == "core_points"||"border_points"){
					count_already_picked_points[picked_tree->_treeId].resize(picked_tree->_leafNodes.size(),0);
				}else{}
		}
		
			treenode * picked_leaf;
			for(auto l:picked_tree->_leafNodes){
				picked_leaf = pickLeaf(picked_tree);
				if(picked_leaf){
					if(pickPoint(picked_leaf)){break;}
				}
			}
		}
	}
	
	
	itree * pickTree(){
		if(_tree_criterion=="random"){
			return pickRandomTree();
		}else if(_tree_criterion == "max_aggrement"){
		}else{
			cout<<_tree_criterion<<" is not an appropriate criterion for picking up a iTree, Input any of the two strings: \ni.random\nii.max_agreemnent";
			exit(0);
		}
	}	


	itree * pickRandomTree(){
		std::random_device random_seed_generator;
	   	std::mt19937_64 RandomEngine(random_seed_generator());
		_picked_treeId = std::uniform_int_distribution<>(0, _iforestObject._numiTrees-1)(RandomEngine);
		//cout<<"picked_treeId="<<_picked_treeId<<"----";
		return _iforestObject._iTrees[_picked_treeId];
	}


	
	treenode * pickLeaf(itree * picked_tree){
		if(_leaf_criterion=="random"){
			return pickRandomLeaf(picked_tree);
		}
		else if(_leaf_criterion=="hard_discard" || "soft_discard"){
			
			//treenode * lessNoisyLeaf = pickLessNoisyLeaf(picked_tree);
			return pickLessNoisyLeaf(picked_tree);
		}else{
			cout<<_leaf_criterion<<" is not an appropriate criterion for picking up a node, Input any of the three strings: \ni.random\nii.hard_discard\niii.soft_discard";
			exit(0);
		}
	}



	treenode * pickRandomLeaf(itree * picked_tree){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		_picked_leaf_index = std::uniform_int_distribution<>(0, picked_tree->_leafNodes.size()-1)(RandomEngine);
		//cout<<"picked_leafId="<<_picked_leaf_index<<"--------";
		//cout<<"picked_leafId="<<picked_leafId<<"----isLeaf="<<picked_tree->treeNodes[picked_leafId]->isLeaf<<"----";
		return picked_tree->treeNodes[picked_tree->_leafNodes[_picked_leaf_index]];
	}

	/**Pick No Noise Leaf**/
	treenode * pickLessNoisyLeaf(itree * picked_tree){
		
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		for(auto l:picked_tree->_leafNodes){
			_picked_leaf_index = std::uniform_int_distribution<>(0, picked_tree->_leafNodes.size()-1)(RandomEngine);
			int picked_leafId = picked_tree->_leafNodes[_picked_leaf_index];
			if(std::uniform_int_distribution<>(0, 1)(RandomEngine) < discarding_threshold_for_leaf[picked_tree->_treeId][_picked_leaf_index]){
				return picked_tree->treeNodes[picked_leafId];
			}
		}
		return nullptr;
	}
	
	
	
	void hard_discard_noisy_leaf(itree * picked_tree){
		for(int l =0; l < picked_tree->_leafNodes.size(); l++){
			int leafid = picked_tree->_leafNodes[l];
			for(auto point:picked_tree->treeNodes[leafid]->dataPointIndices){
				if(_iforestObject.anomalyScore[point] > _aScoreTh){
					discarding_threshold_for_leaf[picked_tree->_treeId][l] = 1;
					break;
				}
			}
		}
	}



	void soft_discard_noisy_leaf(itree * picked_tree){
		for(int l =0; l < picked_tree->_leafNodes.size(); l++){
			int leafid = picked_tree->_leafNodes[l];
			float countNoise = 0;
			for(auto point:picked_tree->treeNodes[leafid]->dataPointIndices){
				if(_iforestObject.anomalyScore[point] > _aScoreTh){
					countNoise++;
				}
			}
			discarding_threshold_for_leaf[picked_tree->_treeId][l] = countNoise/picked_tree->treeNodes[leafid]->dataPointIndices.size();
		}
	}




	bool pickPoint(treenode * picked_leaf){
		bool point_picked;
		if(_point_criterion=="random"){
			point_picked = pickRandomPoint(picked_leaf);
		}else if(_point_criterion == "core_points"){
			point_picked = pickCorePoint(picked_leaf);
		}else if(_point_criterion == "border_points"){
			point_picked = pickBorderPoint(picked_leaf);
		}else{
			cout<<_point_criterion<<" is not an appropriate criterion for picking up a point, Input any of the three strings: \ni.random\nii.core_points\niii.border_points";
			exit(0);
		}
		
		return point_picked;
	}



	bool pickRandomPoint(treenode * picked_leaf){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		
		for(int i=0; i<picked_leaf->dataPointIndices.size();i++){
			int picked_point = picked_leaf->dataPointIndices[std::uniform_int_distribution<>(0, picked_leaf->dataPointIndices.size()-1)(RandomEngine)];
			if(_iforestObject.anomalyScore[picked_point] <= _aScoreTh && sampleSet.insert(picked_point).second){
				_picked_pointId = picked_point;
				return bool(1);
			}
		}
		return bool(0);
	}



	bool pickCorePoint(treenode * picked_leaf){
		int numPoints = picked_leaf->dataPointIndices.size();
		int min = count_already_picked_points[_picked_treeId][_picked_leaf_index];
		for(int i = min; i<numPoints;i++){
			min = count_already_picked_points[_picked_treeId][_picked_leaf_index];
			for(int j = min+1; j < numPoints; j++){
				if(_iforestObject.anomalyScore[picked_leaf->dataPointIndices[min]] > _iforestObject.anomalyScore[picked_leaf->dataPointIndices[j]]){
					min = j;
				}
			}
			int temp = 	picked_leaf->dataPointIndices[min];
			picked_leaf->dataPointIndices[min] = picked_leaf->dataPointIndices[count_already_picked_points[_picked_treeId][_picked_leaf_index]];
			picked_leaf->dataPointIndices[count_already_picked_points[_picked_treeId][_picked_leaf_index]] = temp;
			int picked_point = picked_leaf->dataPointIndices[count_already_picked_points[_picked_treeId][_picked_leaf_index]];
			count_already_picked_points[_picked_treeId][_picked_leaf_index]++;
			if(_iforestObject.anomalyScore[picked_point] <= _aScoreTh && sampleSet.insert(picked_point).second){
				_picked_pointId = picked_point;
				return bool(1);
			}
		}
		return bool(0);
	}

	bool pickBorderPoint(treenode * picked_leaf){
		int numPoints = picked_leaf->dataPointIndices.size();
		int max = count_already_picked_points[_picked_treeId][_picked_leaf_index];
		for(int i = max; i<numPoints;i++){
			max = count_already_picked_points[_picked_treeId][_picked_leaf_index];
			for(int j = max+1; j < numPoints; j++){
				if(_iforestObject.anomalyScore[picked_leaf->dataPointIndices[max]] < _iforestObject.anomalyScore[picked_leaf->dataPointIndices[j]]){
					max = j;
				}
			}
			int temp = 	picked_leaf->dataPointIndices[max];
			picked_leaf->dataPointIndices[max] = picked_leaf->dataPointIndices[count_already_picked_points[_picked_treeId][_picked_leaf_index]];
			picked_leaf->dataPointIndices[count_already_picked_points[_picked_treeId][_picked_leaf_index]] = temp;
			int picked_point = picked_leaf->dataPointIndices[count_already_picked_points[_picked_treeId][_picked_leaf_index]];
			count_already_picked_points[_picked_treeId][_picked_leaf_index]++;
			if(_iforestObject.anomalyScore[picked_point] <= _aScoreTh && sampleSet.insert(picked_point).second){
				_picked_pointId = picked_point;
				return bool(1);
			}
		}
		return bool(0);
	}
};



#endif 
























