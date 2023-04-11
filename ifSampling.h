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
	
public:
	ifSampling( iforest & iforestObject): _iforestObject(iforestObject){
		
	}

	virtual ~ifSampling(){}
	
	unordered_set<int> & sample(string tree_criterion, string leaf_criterion,string point_criterion, double sample_size){	
		//cout<<"numInstances="<<_iforestObject._dataObject.getnumInstances()<<endl;
		for(int sam =0; sam < _iforestObject._dataObject.getnumInstances(); sam++ ){
			cout<<"\nsample number="<<sam<<"-------------"<<endl;
			pickPoint(point_criterion,pickLeaf(leaf_criterion,pickTree(tree_criterion)));
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
		cout<<"picked_treeId="<<picked_treeId<<"----";
		return _iforestObject._iTrees[picked_treeId];
	}
	
	treenode * pickLeaf(string leaf_criterion,itree * picked_tree){
		if(leaf_criterion=="random"){
			return pickRandomLeaf(picked_tree);
		}
	}

	treenode * pickRandomLeaf(itree * picked_tree){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		int picked_leafId = picked_tree->_leafNodes[std::uniform_int_distribution<>(0, picked_tree->_leafNodes.size()-1)(RandomEngine)];
		cout<<"picked_leafId="<<picked_leafId<<"----isLeaf="<<picked_tree->treeNodes[picked_leafId]->isLeaf<<"----";
		
		return picked_tree->treeNodes[picked_leafId];
	}

	void pickPoint(string point_criterion, treenode * picked_leaf){
		if(point_criterion=="random"){
			pickRandomPoint(picked_leaf);
		}
	}
	void pickRandomPoint(treenode * picked_leaf){
		std::random_device random_seed_generator;
    	std::mt19937_64 RandomEngine(random_seed_generator());
		int picked_point = picked_leaf->dataPointIndices[std::uniform_int_distribution<>(0, picked_leaf->dataPointIndices.size()-1)(RandomEngine)];
		cout<<"picked_pointId="<<picked_point<<"----";
		
		if(sampleSet.insert(picked_point).second){
			pickRandomPoint(picked_leaf);
		}
	}
	
};

#endif 
























