/*
 * procs.h
 *
 *  Created on: Jun 6, 2013
 *      Author: geogunow
 */

#ifndef PROCS_H_
#define PROCS_H_

#include "options.h"
#include "output_reader.h"

void renormalize(options &opts, output &result);
void write_data(options &opts, output result);
bool ref_simulate(options &opts);
bool ref_casmo(options &opts);

#endif /* PROCS_H_ */
