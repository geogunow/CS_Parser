/*
 * options.cpp
 *
 *  Created on: Jun 5, 2013
 *      Author: geogunow
 */
#include "options.h"

using namespace std;

options::options(void){
	input = "input.inp"; // Initialize input file string to input.inp
	outfile = "mod_map.out"; // Initialize output file to mod_map.out

	ref_mode = 0;
	typ = 0;

	// Initialize to HZP step
	POW = 0;
	EXP = 0;

	POW2 = 0;
	EXP2 = 0;

	ref_present = false;
	plot = false;
	fold_out = false;
	fold_ref = false;
	SEQ = false;
	pt = 0;

	sym = "FULL";
}

int options::read_input(std::string fname){

	//Declare varaibles
	double tmp;
	string ID,in_str,in_arg,sym_ref = "QTR",sym_out = "FULL";
	stringstream ss;

	bool map_included = false; // bool if MAP card should exist

	//Open the file mentioned by user
	ifstream in_file(fname.c_str());
	if(in_file.fail()){
		cout << "ERROR! Failed to open input file" << endl;
		return -1;
	}

	//Start reading input file
	getline(in_file,in_str);

	while(!in_file.fail()){
		//Look for expected cards in format 'XXX'
		ID = in_str.substr(1,3);

		//Check if card is comment
		if("COM"!=ID){
		    //Not a comment, check card ID
			if("INP"==ID){
			    //Card is input file name
				ss.clear();  //Clear ss stream

				//Record file name (expecting no spaces)
				ss << in_str;
				ss >> in_arg;
				ss >> in_arg;
				input = in_arg;
			}
			else if("MAP"==ID){
				//Card is measurement map
				ref_present = true; //Map is included

				//Clear ss stream, output line to ss stream
				ss.clear();
				ss << in_str;
				ss >> in_arg;  //Read in card (not used)

				//Declare variables
				string str;
				int end = 0, i = -1, j = 0;
				double num;

				//Read line (string)
				while(1){
					ss >> str;
					if(ss.fail()) break; //Break loop when the end of line is reached
					else{
						num = stod(str,end); //Convert strings to doubles
						i=0;
						mapi.push_back(i); //track row
						mapj.push_back(j); //track column
						mapData.push_back(num); //track value
						j++;
					}
				}
				//Repeat for further lines
				while(end == 0){
					//get another line
					getline(in_file,in_str);
					j = 0;            // set column back to 0
					if(in_file.fail()){
					    //Reached end of file before ending block (error)
						end = 1;
						cout<<"ERROR in termination"<< endl;
						ref_present = false;
					}
					i++;
					ss.clear();
					ss << in_str;
					//Read line, as before
					while(1){
						ss >> str;
						if(ss.fail()) break;
						else{
							num = stod(str,end); //convert to double
							mapi.push_back(i); //track row
							mapj.push_back(j); //track column
							mapData.push_back(num); //track value
							j++;
						}
					}
				}
			}
			else if("TYP"==ID){
				// output file type -- 1 = simulate, 2 = casmo
				ss.clear();
				ss << in_str;
				ss >> in_arg;
				ss >> in_arg;
				if(in_arg == "simulate"){
					typ = 1;
				}
				else if(in_arg == "casmo"){
					typ = 2;
				}
				else{
					cout << "ERROR on TYP card: file type unknown" << endl;
				}
				ss >> in_arg;
				if(!ss.fail()){
					if(in_arg == "FULL"){
						sym_out = "FULL";
					}
					else if(in_arg == "QTR"){
						sym_out = "QTR";
					}
					else{
						cout << "ERROR on TYP card: unknown symmetry" << endl;
					}
				}
			}
			else if("REF"==ID){
				ss.clear();
				ss << in_str;
				ss >> in_arg;
				ss >> in_arg;
				if(in_arg == "MAP"){
					map_included = true;
				}
				else if(in_arg == "simulate"){
					ref_mode = 1;
				}
				else if(in_arg == "casmo"){
					ref_mode = 2;
				}
				else if(in_arg == "K"){
					ref_mode = 3;
					SEQ = true;
				}
				else{
					cout << "ERROR on REF card: input method unknown" << endl;
				}
				ss >> in_arg;
				if(!ss.fail()){
					if(in_arg == "FULL"){
						sym_ref = "FULL";
					}
					else if(in_arg == "QTR"){
						sym_ref = "QTR";
					}
					else{
						cout << "ERROR on REF card: unknown symmetry" << endl;
					}
				}
			}
			else if("FIL"==ID){
				// reference file card (use for K type in particular)
				ss.clear();
				ss << in_str;
				ss >> in_arg;
				ss >> in_arg;
				ref_file = in_arg;
			}
			else if("POW"==ID){
				//Power card
				ss.clear();  // clear ss stream
				ss << in_str;
				ss >> in_arg;
				ss >> POW; // get power value
			}
			else if("EXP"==ID){
			    //Exposure card
				ss.clear();  // clear ss stream
				ss << in_str;
				ss >> in_arg;
				ss >> EXP; // get exposure value
			}
			else if("EPR"==ID){
				// Exposure and Power of Reference
				// First list exposure, then power
				// only for single point
				ss.clear();  // clear ss stream
				ss << in_str;
				ss >> in_arg;
				ss >> EXP2; // get reference exposure
				ss >> tmp;
				// if available, input reference power
				if(!ss.fail()) POW2 = tmp;
			}
			else if("SEQ"==ID){
				// sequence of exposure values
				SEQ = true;
				ss.str("");
				ss.clear();
				ss << in_str;
				ss >> in_arg;
				ss >> tmp;
				while(!ss.fail()){
					EXP_list.push_back(tmp);
					ss >> tmp;
				}
			}
			else if("SYM"==ID){
				// reference file (or map) symmetry
				ss.clear();  // clear ss stream
				ss << in_str;
				ss >> in_arg;
				ss >> in_arg; // read second string (separated by spaces)

				if(in_arg == "QTR"){
					// Quarter core folding will be performed
					sym = "QTR";
				}
				else if(in_arg == "FULL"){
					// No folding
					cout << "Full core output" << endl;
					sym = "FULL";
				}
				else{
					cout << "ERROR in SYM card" << endl << "Default: Full Core Output" << endl;
					sym = "FULL";
				}
			}
			else if("PLT"==ID) plot = true; //Turn on plotting

			else cout << "ERROR reading input - unknown card" << endl; // Card not specified
		}
		getline(in_file,in_str);
	}
	if(map_included == true && ref_present == false){
		cout << "ERROR MAP card needed for MAP reference type" << endl;
	}
	in_file.close();

	// Folding options for output file (testing file)
	if(sym_out == "FULL" && sym == "QTR"){
		fold_out = true;
	}
	else if(sym_out == "QTR" && sym == "FULL"){
		cout << "ERROR: Not possible to create full core from quarter core input" << endl;
		ref_present = false;
	}

	// Folding options for reference data
	if(sym_ref == "FULL" && sym == "QTR"){
		fold_ref = true;
	}
	else if(sym_out == "QTR" && sym == "FULL"){
		cout << "ERROR: Not possible to create full core from quarter core input" << endl;
		ref_present = false;
	}

	// load reference file if applicable

	return 0;
}

void options::fold_input(void){
	// alert user of folding
	cout<<"Folding reference input into quarter core..." << endl;

	// declare new pointers and variables
	int len = mapi.size();
	int L1 = max_elem(mapi);
	int L2 = max_elem(mapj);
	int mid_i, mid_j, k, pts, stop = 0, ctr = 1;
	vector<double> map2;

	// find midpoint of map
	mid_i = (L1+1)/2;
	mid_j = (L2+1)/2;


	// establish initial k as midpoint and count elements in rows
	int start = 0; // marks points corrected in mapj
	for(int ii=0;ii<len;ii++){

		// count elements and correct index j
		if(mapi[ii] == mapi[ii+1]) ctr++;
		else{
			for(int jj = start; jj <= ii; jj++){
				mapj[jj] += ((L1+1) - ctr)/2;
			}
			start = ii+1;
			ctr = 1;
		}

		//establish midpoint
		if(mapi[ii] == mid_i && mapj[ii] == mid_j){
			k = ii;
		}
	}
	int i=0, j=0;
	vector<int> mapi2, mapj2;
	while(stop == 0){
		// start new point
		pts=0;
		map2.push_back(0);
		for(int kk=0;kk<len;kk++){
			if( abs(mapi[k] - mid_i) == abs(mapi[kk] - mid_i) ){
				if( abs(mapj[k] - mid_j) == abs(mapj[kk] - mid_j) ){
					map2.back() += mapData[kk];	// add symmetric points
					pts++;
				}
			}
		}
		if(map2.back()!=0) map2.back() = map2.back()/pts; // create average

		mapi2.push_back(i);
		mapj2.push_back(j);
		if(k == len - 1){
			stop = 1;//end reached
			cout << map2.back()<<endl;
		}
		else{
			// pick new k and input folded data
			if(mapj[k+1] == mapj[k] + 1){
				cout << map2.back() << '\t';
				j++;
				k++; // next location is in the same row -> simply increment k
			}
			else{
				cout << map2.back() << endl;
				i++;
				j = 0;
				while(mapj[k] != mid_j) k++; // find next value for k in the lower right quadrant
			}
		}
	}
	mapData = map2;
	mapi = mapi2;
	mapj = mapj2;
}

void options::RMS_make_output_file(void){
	ofstream out("RMS_error.txt");
	out << "Exposure" << '\t' << "RMS" << endl;
	for(int i=0; i<RMS_vals.size(); i++){
		out << EXP_list.at(i) << '\t' << RMS_vals.at(i) << endl;
	}
	out.close();
}

void options::RMS_output(double RMS){
	RMS_vals.push_back(RMS);
}

bool options::get_exp(void){
	bool last = false;
	EXP = EXP_list.at(pt);
	EXP2 = EXP_list.at(pt);
	if(pt == EXP_list.size()-1) last = true;

	string point;
	stringstream ss;
	ss << pt;
	ss >> point;
	outfile = "mod_map";
	outfile.append(point);
	outfile.append(".out");

	pt++;
	return last;
}
