/*
 * util.h
 *
 *  Created on: Jun 4, 2013
 *      Author: geogunow
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

//Function for converting string to double in expected format
double stod(string str, int &end);

int max_elem(vector<int> v);

#endif /* UTIL_H_ */
