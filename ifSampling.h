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
	unordered_set<int> sampleSet;
	vector<vector<float>> discarding_threshold_for_leaf;
	vector<bool> tree_picked_first_time;
	vector<vector<int>> count_already_picked_points;
	int _picked_treeId;
	int _picked_leaf_index;
	int _picked_pointId;
	
public:
	ifSampling( iforest & iforestObject): _iforestObject(iforestObject){
		tree_picked_first_time.resize(_iforestObject._numiTrees,bool(1));
	}

	virtual ~ifSampling(){}
	
	unordered_set<int> & sample(string tree_criterion, string leaf_criterion,string point_criterion, double sample_size){
		int sampleSize=_iforestObject._dataObject.getnumInstances() * sample_size;	
	
		if(point_criterion == "core_points"||"border_points"){
			count_already_picked_points.resize(_iforestObject._numiTrees);
		}
		if(leaf_criterion == "hard_discard"||"soft_discard"){
			discarding_threshold_for_leaf.resize(_iforestObject._numiTrees);
		}
			
		//cout<<"numInstances="<<_iforestObject._dataObject.getnumInstances()<<endl;
		while(sampleSet.size() < sampleSize){
			//cout<<"\nsample number="<<sam<<"-------------"<<endl;
			itree * picked_tree = pickTree(tree_criterion);
			if(tree_picked_first_time[picked_tree->_treeId]){
				tree_picked_first_time[picked_tree->_treeId]=bool(0);
				discarding_threshold_for_leaf[picked_tree->_treeId].resize(picked_tree->_leafNodes.size(), 0.0);
				if(leaf_criterion == "hard_discard"){
					hard_discard_noisy_leaf(picked_tree);
				}else if(leaf_criterion=="soft_discard"){
					soft_discard_noisy_leaf(picked_tree);
				}else{}
					if(point_criterion == "core_points"||"border_points"){
					count_already_picked_points[picked_tree->_treeId].resize(picked_tree->_leafNodes.size(),0);
				}else{}
		}
		
			treenode * picked_leaf;
			for(auto l:picked_tree->_leafNodes){
				picked_leaf = pickLeaf(leaf_criterion, picked_tree);
				if(picked_leaf){
					if(pickPoint(point_criterion,picked_leaf)){break;}
				}
			}
		}
	}
	
	
	itree * pickTree(string tree_criterion){
		if(tree_criterion=="random"){
			return pickRandomTree();
		}else if(tree_criterion == "max_aggrement"){
		}else{
			cout<<tree_criterion<<" is not an appropriate criterion for picking up a iTree, Input any of the two strings: \ni.random\nii.max_agreemnent";
			exit(0);
		}
	}	


	itree * pickRandomTree(){
		std::random_device random_seed_generator;
	   	std::mt19937_64 RandomEngine(random_seed_generator());
		_picked_treeId = std::uniform_int_distribution<>(0, _iforestObject._numiTrees-1)(RandomEngine);
		//cout<<"picked_treeId="<<picked_treeId<<"----";
		return _iforestObject._iTrees[_picked_treeId];
	}


	
	treenode * pickLeaf(string leaf_criterion,itree * picked_tree){
		if(leaf_criterion=="random"){
			return pickRandomLeaf(picked_tree);
		}
		else if(leaf_criterion=="hard_discard" || "soft_discard"){
			
			//treenode * lessNoisyLeaf = pickLessNoisyLeaf(leaf_criterion,picked_tree);
			return pickLessNoisyLeaf(leaf_criterion,picked_tree);
		}else{
			cout<<leaf_criterion<<" is not an appropriate criterion for picking up a node, Input any of the three strings: \ni.random\nii.hard_discard\niii.soft_discard";
			exit(0);
		}
	}



	treenode * pickRandomLeaf(itree * picked_tree){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		_picked_leaf_index = std::uniform_int_distribution<>(0, picked_tree->_leafNodes.size()-1)(RandomEngine);
		//cout<<"picked_leafId="<<picked_leafId<<"----isLeaf="<<picked_tree->treeNodes[picked_leafId]->isLeaf<<"----";
		return picked_tree->treeNodes[picked_tree->_leafNodes[_picked_leaf_index]];
	}

	/**Pick No Noise Leaf**/
	treenode * pickLessNoisyLeaf(string leaf_criterion, itree * picked_tree){
		
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
				if(_iforestObject.anomalyScore[point] > 0.5){
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
				if(_iforestObject.anomalyScore[point] > 0.5){
					countNoise++;
				}
			}
			discarding_threshold_for_leaf[picked_tree->_treeId][l] = countNoise/picked_tree->treeNodes[leafid]->dataPointIndices.size();
		}
	}




	bool pickPoint(string point_criterion, treenode * picked_leaf){
		bool point_picked;
		if(point_criterion=="random"){
			point_picked = pickRandomPoint(picked_leaf);
		}else if(point_criterion == "core_points"){
			point_picked = pickCorePoint(picked_leaf);
		}else if(point_criterion == "border_points"){
			point_picked = pickBorderPoint(picked_leaf);
		}else{
			cout<<point_criterion<<" is not an appropriate criterion for picking up a point, Input any of the three strings: \ni.random\nii.core_points\niii.border_points";
			exit(0);
		}
		
		return point_picked;
	}



	bool pickRandomPoint(treenode * picked_leaf){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		
		for(int i=0; i<picked_leaf->dataPointIndices.size();i++){
			int picked_point = picked_leaf->dataPointIndices[std::uniform_int_distribution<>(0, picked_leaf->dataPointIndices.size()-1)(RandomEngine)];
			//cout<<"picked_pointId="<<picked_point<<"----";
			//cout<<"_iforestObject.anomalyScore["<<picked_point<<"]="<<_iforestObject.anomalyScore[picked_point]<<endl;
			if(_iforestObject.anomalyScore[picked_point] <= 0.6 && sampleSet.insert(picked_point).second){
				//i=picked_leaf->dataPointIndices.size();
				_picked_pointId = picked_point;
				return bool(1);
			}
		}
		return bool(0);
	}



	bool pickCorePoint(treenode * picked_leaf){
		//count_already_picked_points[_picked_treeId][_picked_leaf_index] = 0;
		int numPoints = picked_leaf->dataPointIndices.size();
		
		for(int i = 0; i<numPoints;i++){
			int min = count_already_picked_points[_picked_treeId][_picked_leaf_index];
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
			if(_iforestObject.anomalyScore[picked_point] <= 0.5 && sampleSet.insert(picked_point).second){
				//i=picked_leaf->dataPointIndices.size();
				_picked_pointId = picked_point;
				return bool(1);
			}
		}
		return bool(0);
	}



	bool pickBorderPoint(treenode * picked_leaf){
	//count_already_picked_points[_picked_treeId][_picked_leaf_index] = 0;
		int numPoints = picked_leaf->dataPointIndices.size();
		
		for(int i = 0; i<numPoints;i++){
			int max = count_already_picked_points[_picked_treeId][_picked_leaf_index];
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
			if(_iforestObject.anomalyScore[picked_point] <= 0.5 && sampleSet.insert(picked_point).second){
				//i=picked_leaf->dataPointIndices.size();
				_picked_pointId = picked_point;
				return bool(1);
			}
		}
		return bool(0);
	}
};



#endif 
























