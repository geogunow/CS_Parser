/*
 * options.h
 *
 *  Created on: Jun 5, 2013
 *      Author: geogunow
 */

#ifndef OPTIONS_H_
#define OPTIONS_H_

#include "util.h"

struct options{
	std::string input;
	std::string outfile;
	std::string ref_file;
	std::string sym;
	double POW;
	double EXP;

	// reference power, exposure
	double POW2;
	double EXP2;
	std::vector<double> EXP_list;

	bool ref_present;
	bool plot;
	bool fold_out;
	bool fold_ref;
	bool SEQ;
	int pt;

	int ref_mode;
	int typ;

	std::vector<double> mapData;
	std::vector<int> mapi;
	std::vector<int> mapj;

	std::vector<double> RMS_vals;
	void RMS_output(double RMS);
	void RMS_make_output_file(void);

	options(void);
	int read_input(std::string fname);
	void fold_input(void);
	bool get_exp(void);
};


#endif /* OPTIONS_H_ */
