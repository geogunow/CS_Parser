/*
 * koroush_reader.h
 *
 *  Created on: Jun 6, 2013
 *      Author: geogunow
 */

#ifndef KOROUSH_READER_H_
#define KOROUSH_READER_H_

#include "util.h"
#include "options.h"

using namespace std;

class kclass{
private:
	// vectors for use in multiple state points
	vector<double> pow_vals;
	vector<double> exp_vals;

	vector<vector<int> > kori;
	vector<vector<int> > korj;
	vector<vector<double> > korData;

	int statepoints;
	int current_pt;

public:
	kclass(void);
	bool read_input(string fname);
	bool get_statepoint(options &opts);

};


#endif /* KOROUSH_READER_H_ */
