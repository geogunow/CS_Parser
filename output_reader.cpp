/*
 * output_reader.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: geogunow
 */


#include "output_reader.h"
using namespace std;

///////////////////////////////////
// Reading data in Simulate form //
///////////////////////////////////

bool output::simulate(options opts){

	// declare more variables
	double POW, EXP, num1, num2;
	bool found = false;
	int i=0, j=0;

	// create input file stream
	ifstream in(opts.input.c_str());

	// look for the startIdent string given below
	string str, line, startIdent = "  ITE.BOR - QPANDA Flux solution with boron search";

	stringstream ss;

	// read output file data
	getline(in,line);
	cout << "Output File Data:" << endl;
	while(!in.fail()){
		// read until startIndent string found
		if(line == startIdent){
			// startIdent string found
			getline(in,line);
			while(!in.fail()){
				// continue reading, look for power & exposure data
				if(line[1]=='%'){
					ss.clear();
					ss << line;
					ss >> str;
					ss >> str;
					ss >> POW;  // saving power point
					for(int kk = 0; kk < 9; kk++) ss >> str; // read unused strings
					ss >> EXP;  // saving exposure point
					ss.str("");  // clear ss stream
					if(opts.SEQ){
						opts.POW = POW;
					}
					if(abs(opts.POW-POW) < pow(10.0,-3) && abs(opts.EXP-EXP) < pow(10.0,-3)){
						found = true;  // power & exposure match input
					}
					break;
				}
				getline(in,line);
			}
		}
		if(found == true){
			// Found matching power, exposure
			// continue reading
			getline(in,line);
			while(!in.fail()){
				// check for next identifier - "Renorm"
				if(line.length() > 6){
					if(line.substr(1,6) == "Renorm"){
						// read two lines, start looking at data from the second
						getline(in,line);
						getline(in,line);

						// ** indicates end of data
						while(line.substr(1,2)!="**"){
							cout<<line<<endl; // output line to screen

							// clear ss stream
							ss.str("");
							ss.clear();

							// read in unused first string (number), read the next two numbers
							ss << line;
							ss >> str;
							ss >> num1;
							ss >> num2;

							// NOTE: last number in line is unused
							j=0;
							while(!ss.fail()){
								outData.push_back(num1); // save value
								outi.push_back(i); // save row
								outj.push_back(j); // save col
								ss >> num1;
								if(!ss.fail()){
									//end of line not reached
									outData.push_back(num2); // save value
									outi.push_back(i); // save row
									outj.push_back(j+1); // save col
								}
								j = j+2;
								ss >> num2;
							}
							i++;
							getline(in,line);
						}
						break;
					}
				}
				getline(in,line);
			}
			break;
		}
		getline(in,line);
	}
	in.close();
	return found;
}


////////////////////////////////////
// Reading data in Casmo MxN form //
////////////////////////////////////

bool output::casmo(options opts){

	// declare more variables
	double POW, EXP, num1, num2;
	bool found = false;
	int i=0, j=0;

	stringstream ss;

	// create input file stream
	ifstream in(opts.input.c_str());

	// look for the startIdent string given below
	string startIdent = " C H A R A C T E R I S T I C S   S O L U T I O N";
	string tmp, tmp2;
	string str, line, ident2 = " 2RR1 Neutron Detector (Relative)";

	// read output file data
	getline(in,line);
	cout << "Output File Data:" << endl;
	while(!in.fail()){
		// read until startIndent string found
		if(line.substr(0,48) == startIdent){
			ss.str("");
			ss.clear();
			ss << line;
			ss >> tmp2;
			while(!ss.fail()){
				tmp = tmp2;
				ss >> tmp2;
			}
			ss.str("");
			ss.clear();
			ss << tmp;
			ss >> EXP;
			if(abs(EXP - opts.EXP) < pow(10.0,-3)){
				getline(in,line);
				while(!in.fail()){
					getline(in,line);
					if(line == ident2){
						found = 1;
						getline(in,line);
						break;
					}
				}
			}
		}
		if(found == 1){
			// Found matching power, exposure
			// continue reading
			getline(in,line);
			for(int i=0; i<8; i++){

				// output line to screen
				cout<<line<<endl;

				// clear ss stream
				ss.str("");
				ss.clear();

				ss << line;
				ss >> num1;  // discard first value
				ss >> num1;
				ss >> num2;

				// last number and first number unused
				j=0;
				while(!ss.fail()){
					if(num1 != 0){
						outData.push_back(num1); // save value
						outi.push_back(i); // save row
						outj.push_back(j); // save col
					}
					ss >> num1;
					if(!ss.fail()){
						//end of line not reached
						if(num2 != 0){
							outData.push_back(num2); // save value
							outi.push_back(i);  //save row
							outj.push_back(j+1);  //save col
						}
					}
					j = j+2;
					ss >> num2;
				}
				getline(in,line);
			}
			break;
		}
		getline(in,line);
	}
	in.close();
	return found;
}

//////////////////////////////////////
// Folding flux map to quarter core //
//////////////////////////////////////

void output::folding(void){
	// alert user of folding
	cout<<"Folding into quarter core..." << endl;

	// declare new pointers and variables
	int len = outi.size();
	int L1 = max_elem(outi);
	int L2 = max_elem(outj);
	int mid_i, mid_j, k, pts, stop = 0, ctr = 1;
	vector<double> out2;

	// find midpoint of map
	mid_i = (L1+1)/2;
	mid_j = (L2+1)/2;


	// establish initial k as midpoint and count elements in rows
	int start = 0; // marks points corrected in outj
	for(int ii=0;ii<len;ii++){

		// count elements and correct index j
		if(outi[ii] == outi[ii+1]) ctr++;
		else{
			for(int jj = start; jj <= ii; jj++){
				outj[jj] += ((L1+1) - ctr)/2;
			}
			start = ii+1;
			ctr = 1;
		}

		//establish midpoint
		if(outi[ii] == mid_i && outj[ii] == mid_j){
			k = ii;
		}
	}
	while(stop == 0){
		// start new point
		pts=0;
		out2.push_back(0);
		for(int kk=0;kk<len;kk++){
			if( abs(outi[k] - mid_i) == abs(outi[kk] - mid_i) ){
				if( abs(outj[k] - mid_j) == abs(outj[kk] - mid_j) ){
					out2.back() += outData[kk];	// add symmetric points
					pts++;
				}
			}
		}
		if(out2.back()!=0) out2.back() = out2.back()/pts; // create average

		if(k == len - 1){
			stop = 1;//end reached
			cout << out2.back()<<endl;
		}
		else{
			// pick new k and output folded data
			if(outj[k+1] == outj[k] + 1){
				cout << out2.back() << '\t';
				k++; // next location is in the same row -> simply increment k
			}
			else{
				cout << out2.back() << endl;
				while(outj[k] != mid_j) k++; // find next value for k in the lower right quadrant
			}
		}
	}
	outData = out2;
	outi.resize(outData.size());
	outj.resize(outData.size());
}

void output::clear(void){
	outData.clear();
	outi.clear();
	outj.clear();
}
