//============================================================================
// Name        : flux_parser.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Flux map parser
//============================================================================

#include <cstdlib>
#include "options.h"
#include "output_reader.h"
#include "koroush_reader.h"
#include "procs.h"

using namespace std;


// NOTE :: Line 208

int main(){

	//Declare Variables
	string fname;
	options opts;
	output result;
	kclass kman;

	bool found;

	//Ask user for input file name
	cout << "Input File Name: ";
	cin >> fname;

	int exit_code = opts.read_input(fname);
	if(exit_code != 0) return 1;

	bool success, last;
	if(opts.ref_mode !=0){
		switch(opts.ref_mode){
		case 1:
			READ_SIMULATE:
			if(opts.SEQ) last = opts.get_exp();
			success = ref_simulate(opts);
			break;
		case 2:
			READ_CASMO:
			if(opts.SEQ) last = opts.get_exp();
			success = ref_casmo(opts);
			break;
		case 3:
			success = kman.read_input(opts.ref_file);
			Reading:
			last = kman.get_statepoint(opts);
			if(success) opts.ref_present = true;
			break;
		default:
			return -1;
		}
		if(!success){
			cout << "ERROR: Reference input found but statepoint not found!" << endl;
			return -1;
		}
	}
	if(opts.fold_ref){
		// DO FOLDING on reference data!
		opts.fold_input();
	}
	if(opts.ref_present){
		// map included in input
		if(opts.typ == 1){
			// simulate file
			found = result.simulate(opts);
		}
		else if(opts.typ == 2){
			// casmo file
			found = result.casmo(opts);
		}
		else{
			cout<<"ERROR: Output type unknown!"<<endl;
			return -1;
		}

		if(found == false){
			cout<<"ERROR: Failed to find matching exposure and power!!!"<<endl;
			return -1;
		}
		if(opts.fold_out){
			// DO FOLDING on output data!
			result.folding();
			if(opts.mapData.size() != result.outData.size())
				cout << "ERROR Folding index mismatch!!!" << endl;
		}
		if(opts.mapi.size() != result.outi.size()){
			// Number of points in measurement, output do not match!
			cout << "********************************"<<endl;
			cout << "* ERROR: Map Index Mismatch!!! *"<<endl;
			cout << "********************************"<<endl;
		}

		// Renormalize Data
		renormalize(opts,result);
		write_data(opts,result);


		if(opts.SEQ){
			if(!last){
				// use for the conditional a flag from koruoush_reader
				// to identify whether the end is reached
				// change name of output file as well here
				// (as not to overwrite data)
				result.clear();
				if(opts.ref_mode == 3) goto Reading;
				else{
					if(opts.ref_mode == 1){
						goto READ_SIMULATE;
					}
					if(opts.ref_mode == 2){
						goto READ_CASMO;
					}
				}
			}
		}
	}
	else cout << "Measurement Map Not Included!" << endl;
	if(opts.plot == true && opts.SEQ == false){
		if(opts.sym == "QTR"){
			system("./Python/plot_core.py");
		}
		else{
			system("./Python/plot_full.py");
		}
	}
	if(opts.SEQ){
		opts.RMS_make_output_file();
	}
	return 0;
}
