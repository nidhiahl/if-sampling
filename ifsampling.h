#ifndef ifSampling_H
#define ifSampling_H
#pragma once
#include "data.h"
#include "distance.h"
#include "data.cpp"
#include "distance.cpp"
#include "localDensity.h"
#include "localDensity.cpp"
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

class ifSampling
{
    public:
        
    ifSampling( iforest &){
		
	}

	virtual ~ifSampling();
	
};

#endif 







