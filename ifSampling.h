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
	
public:
	ifSampling( iforest & iforestObject): _iforestObject(iforestObject){
		tree_picked_first_time.resize(_iforestObject._numiTrees,bool(1));
	}

	virtual ~ifSampling(){}
	
	unordered_set<int> & sample(string tree_criterion, string leaf_criterion,string point_criterion, double sample_size){
		int sampleSize=_iforestObject._dataObject.getnumInstances() * 0.15;	
		//cout<<"numInstances="<<_iforestObject._dataObject.getnumInstances()<<endl;
		while(sampleSet.size() < sampleSize){
			//cout<<"\nsample number="<<sam<<"-------------"<<endl;
			itree * picked_tree = pickTree(tree_criterion);
			treenode * picked_leaf = pickLeaf(leaf_criterion, picked_tree);
			pickPoint(point_criterion,picked_leaf);
		}

	}
	
	itree * pickTree(string tree_criterion){
		if(tree_criterion=="random"){
			return pickRandomTree();
		}
	}
	
	itree * pickRandomTree(){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		int picked_treeId = std::uniform_int_distribution<>(0, _iforestObject._numiTrees-1)(RandomEngine);
		//cout<<"picked_treeId="<<picked_treeId<<"----";
		return _iforestObject._iTrees[picked_treeId];
	}
	
	treenode * pickLeaf(string leaf_criterion,itree * picked_tree){
		if(leaf_criterion=="random"){
			return pickRandomLeaf(picked_tree);
		}
		else if(leaf_criterion=="hard_discard" || "soft_discard"){
			discarding_threshold_for_leaf.resize(_iforestObject._numiTrees);
			if(tree_picked_first_time[picked_tree->_treeId]){
				tree_picked_first_time[picked_tree->_treeId]=bool(0);
				discarding_threshold_for_leaf[picked_tree->_treeId].resize(picked_tree->_leafNodes.size(), 0.0);
				if(leaf_criterion == "hard_discard"){
					hard_discard_noisy_leaf(picked_tree);
				}else if(leaf_criterion=="soft_discard"){
					soft_discard_noisy_leaf(picked_tree);
				}
			}
			//treenode * lessNoisyLeaf = pickLessNoisyLeaf(leaf_criterion,picked_tree);
			return pickLessNoisyLeaf(leaf_criterion,picked_tree);
		}
	}

	treenode * pickRandomLeaf(itree * picked_tree){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		int picked_leafId = picked_tree->_leafNodes[std::uniform_int_distribution<>(0, picked_tree->_leafNodes.size()-1)(RandomEngine)];
		//cout<<"picked_leafId="<<picked_leafId<<"----isLeaf="<<picked_tree->treeNodes[picked_leafId]->isLeaf<<"----";
		
		return picked_tree->treeNodes[picked_leafId];
	}

	/**Pick No Noise Leaf**/
	treenode * pickLessNoisyLeaf(string leaf_criterion, itree * picked_tree){
		
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		for(auto l:picked_tree->_leafNodes){
			int rnd_leaf = std::uniform_int_distribution<>(0, picked_tree->_leafNodes.size()-1)(RandomEngine);
			int picked_leafId = picked_tree->_leafNodes[rnd_leaf];
			if(std::uniform_int_distribution<>(0, 1)(RandomEngine) < discarding_threshold_for_leaf[picked_tree->_treeId][rnd_leaf]){
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

	void pickPoint(string point_criterion, treenode * picked_leaf){
		if(point_criterion=="random"){
			pickRandomPoint(picked_leaf);
		}
	}
	void pickRandomPoint(treenode * picked_leaf){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		
		for(int i=0; i<picked_leaf->dataPointIndices.size();i++){
			int picked_point = picked_leaf->dataPointIndices[std::uniform_int_distribution<>(0, picked_leaf->dataPointIndices.size()-1)(RandomEngine)];
			//cout<<"picked_pointId="<<picked_point<<"----";
			cout<<"_iforestObject.anomalyScore["<<picked_point<<"]="<<_iforestObject.anomalyScore[picked_point]<<endl;
			if(_iforestObject.anomalyScore[picked_point] <= 0.5 && sampleSet.insert(picked_point).second){
				i=picked_leaf->dataPointIndices.size();
			}
		}
	}
	
};

#endif 
























