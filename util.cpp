/*
 * util.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: geogunow
 */


#include "util.h"

using namespace std;

//Function for converting string to double in expected format
double stod(string str, int &end){
	stringstream stream;
	double num;

	//Check for reading another card (input not terminated by /)
	if(str[0] == '\''){
		cout << "Termination Error" << endl;
		end = 1;
	}
	//Check to see if string being read is last number
	if(str[str.length()-1]=='/'){
		end = 1;
		str = str.substr(0,str.length()-1);
	}
	//Check for empty measurement points (given by a string begining with -)
	//NOTE: we do not expect negative measurement data
	if(str[0] == '-') num = -1;  //Using -1 as placeholder for point not measured
	else{
	    //If it is a measured point, convert string to double
		stream << str;
		stream >> num;
	}
	return num;
}

int max_elem(vector<int> v){
	if(v.size() == 0){
		cout << "ERROR index vector of size zero!!!!";
		return 0;
	}
	else{
		int maxim = v.at(0);
		for(int i=0; i<v.size(); i++){
			if(v.at(i) > maxim){
				maxim = v.at(i);
			}
		}
		return maxim;
	}
}

template <class ForwardIterator>
	ForwardIterator Max ( ForwardIterator first, int iters ){
  	ForwardIterator largest = first;
  	if (iters==1) return first;
  		for(int i=0;i<iters;i++){
    		if (*largest<*first) largest=first;
    		first++;
    	}
  	return largest;
}
