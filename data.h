#ifndef DATA_H
#define DATA_H
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

typedef struct point{
    double* attributes;
    bool isPresent;
    int label;
}point;

class data
{
    public:
        data();
        virtual ~data();
        void createDataVector(const string &);
		vector<int> & getSample(int) const ;
        vector<point*>  getDataVector() const ;
        int getnumInstances() const;
        void setnumInstances(int);
        int getnumAttributes() const;
        void setnumAttributes(int);
        
        vector<point*> dataVector;
        
    protected:

    private:
    int numInstances;
    int numAttributes;
};



data::data()
{
    //ctor
}

data::~data()
{
    //dtor
}



/**************Setter methods*****************/


void data::setnumInstances(int numInstances){
    this->numInstances = numInstances;
}

void data::setnumAttributes(int numAttributes){
    this->numAttributes = numAttributes;
}

/**************Getter methods*****************/


int data::getnumInstances()const{
    return this->numInstances;
}

int data::getnumAttributes() const {
    return this->numAttributes;
}

vector<point*> data::getDataVector() const {
    const vector<point*> & refDataVector = this->dataVector;
    return dataVector;
}


/*******takes complete dataset File as input************/

void data::createDataVector(const string & dataFile){
	ifstream inDataFile(dataFile.c_str(),ios::in|ios::binary) ;
	if(!inDataFile){
		cout<<"Can not open input data file: "<<dataFile<<endl;
		exit(0);
	}
	inDataFile>>numInstances>>numAttributes;
	dataVector.resize(numInstances);
	for(int instance = 0; instance < numInstances; instance++){
		point *newPoint = new point();
		newPoint->attributes = new double[numAttributes];
		int id;
		inDataFile>>id;
		for(int attribute = 0; attribute < numAttributes; attribute++){
			inDataFile>>newPoint->attributes[attribute];
		}
		
		inDataFile>>newPoint->label;
		newPoint->isPresent = bool(1);
		dataVector[id] = newPoint;
	}
	inDataFile.close();
}


//***************************************Select sample from the original dataset i.e for static run****************************************//
vector<int> & data::getSample(int sampleSize) const {
	vector<int>* sample = new vector<int>;            
	vector<int> &refSample = *sample;
	//cout<<"start of getsmaple"<<endl;
	int cnt = 0;
	//cout<<"num instance="<<numInstances<<endl;
	for(int i = 0; i < numInstances; i++){
		//cout<<"dataVector[i]->isPresent="<<dataVector[i]->isPresent<<endl;
		if(dataVector[i]->isPresent){
			(*sample).push_back(i);
			cnt++;
		}
	}
	std::random_shuffle((*sample).begin(),(*sample).end());
	(*sample).erase((*sample).begin()+sampleSize,(*sample).begin()+cnt);
	return refSample;
}
#endif
