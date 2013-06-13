/*
 * output_reader.h
 *
 *  Created on: Jun 6, 2013
 *      Author: geogunow
 */

#ifndef OUTPUT_READER_H_
#define OUTPUT_READER_H_

#include "options.h"

struct output{

	std::vector<double> outData;
	std::vector<int> outi;
	std::vector<int> outj;

	bool simulate(options opts);
	bool casmo(options opts);
	void folding(void);
	void clear(void);
};


#endif /* OUTPUT_READER_H_ */
