/*
 * procs.cpp
 *
 *  Created on: Jun 6, 2013
 *      Author: geogunow
 */

#include "procs.h"


void renormalize(options &opts, output &result){
	double sum1=0, sum2=0;
	int len = opts.mapi.size();
	for(int k = 0; k < len; k++){
		if(opts.mapData[k] != -1){
			sum1+=opts.mapData[k]; // sum input data
			sum2+=result.outData[k]; // sum data at input measurement locations
		}
	}
	double ratio=sum2/sum1;
	// display measurement, simulation sums & ratio
	cout<<"Measurement Sum = "<<sum1<<endl;
	cout<<"Simulation Sum = "<<sum2<<endl;
	cout<<"Ratio = "<<ratio<<endl;
	for(int k=0; k<len; k++){
		if(opts.mapData.at(k) != -1)
			opts.mapData.at(k) *= ratio;
	}
}

void write_data(options &opts, output result){

	int len = opts.mapi.size();
	double RMS = 0;
	int nonzero = 0;

	cout<<"Adjusted Reference Data:"<< endl;

	// Start writing to output file
	ofstream out(opts.outfile.c_str());
	out << "EXPOSURE = " << opts.EXP << endl;
	out << "POWER = " << opts.POW << endl;

	out<<"Adjusted Reference Data:"<<endl;

	for(int k = 0; k < len; k++){
		if(opts.mapData[k] == -1){
			out << "-----";
			cout << "-----";
		}
		else{
			out<<opts.mapData[k];
			printf("%4.3f",opts.mapData[k]);
		}
		if(opts.mapi[k]==opts.mapi[k+1]){
			out << ' ';
			cout << '\t';
		}
		else{
			out << endl;
			cout << endl;
		}
	}
	out<< endl <<"Error Magnitude (Absolute):" << endl;
	cout<< "Error Plot (Absolute):"<<endl;
	for(int k = 0; k < len; k++){
		if(opts.mapData[k] == -1){
			out << "-----";
			cout << "-----";
		}
		else{
			out << result.outData[k]-opts.mapData[k];
			RMS += pow(result.outData[k]-opts.mapData[k],2);
			printf("%4.3f",(result.outData[k]-opts.mapData[k]));
			nonzero++;
		}
		if(opts.mapi[k]==opts.mapi[k+1]){
			out << ' ';
			cout << '\t';
		}
		else{
			out << endl;
			cout << endl;
		}
	}
	RMS = sqrt(RMS/nonzero);

	out << "RMS = " << RMS << endl;
	cout << endl << "RMS = " << RMS << endl << endl;
	if(opts.SEQ) opts.RMS_vals.push_back(RMS);
		//opts.RMS_output(RMS);

	RMS = 0;
	nonzero = 0;
	out<< endl <<"Error Magnitude (Relative %):" << endl;
	cout<< "Error Plot (Relative %):"<<endl;
	for(int k = 0; k < len; k++){
		if(opts.mapData[k] == -1){
			out << "-----";
			cout << "-----";
		}
		else{
			out << 100*(result.outData[k]-opts.mapData[k])/(opts.mapData[k]);
			RMS += pow(100*(result.outData[k]-opts.mapData[k])/(opts.mapData[k]),2);
			printf("%4.3f",100*(result.outData[k]-opts.mapData[k])/(opts.mapData[k]));
			nonzero++;
		}
		if(opts.mapi[k]==opts.mapi[k+1]){
			out << ' ';
			cout << '\t';
		}
		else{
			out << endl;
			cout << endl;
		}
	}
	RMS = sqrt(RMS/nonzero);
	out << "Rel Error RMS = " << RMS << endl;
	char A = 'A';
	char col;
	out<< endl << "plot_core.py format:"<<endl;
	if(opts.sym == "QTR"){
		for(int k = 0; k < len; k++){
			if(opts.mapData[k] != -1){
				out << "coredata ";
				col = A + 7 - opts.mapj[k];
				out << col << opts.mapi[k] + 8 << " ";
				out << 100*(result.outData[k]-opts.mapData[k])/(opts.mapData[k]) << endl;
			}
		}
	}
	else{
		int ctr=1,start=0;
		for(int ii=0;ii<len;ii++){

			// count elements and correct index j
			if(opts.mapi[ii] == opts.mapi[ii+1]) ctr++;
			else{
				for(int jj = start; jj <= ii; jj++){
					opts.mapj[jj] += (15 - ctr)/2;
				}
				start = ii+1;
				ctr = 1;
			}
		}
		int add, row;
		for(int k = 0; k < len; k++){
			if(opts.mapData[k] != -1){
				row = opts.mapi.at(k);

				out << "coredata ";
				add = 14 - opts.mapj[k];

				// correct for awkward lettering in pdf
				if(add > 7) add++;
				if(add > 13) add++;
				if(add > 15) add++;

				col = A + add;
				out << col << opts.mapi[k] + 1 << " ";
				out << 100*(result.outData[k]-opts.mapData[k])/(opts.mapData[k]) << endl;
			}
		}
	}
	out.close();
}

////////////////////////////
// Read Simulate Ref Input
////////////////////////////

bool ref_simulate(options &opts){
	// declare more variables
	double POW, EXP, num1, num2;
	bool found = false;
	int i=0, j=0;

	// clear vectors
	opts.mapData.clear();
	opts.mapi.clear();
	opts.mapj.clear();

	// create input file stream
	ifstream in(opts.ref_file.c_str());
	if(!in.fail()) opts.ref_present = true;

	// look for the startIdent string given below
	string str, line, startIdent = "  ITE.BOR - QPANDA Flux solution with boron search";

	stringstream ss;

	// read output file data
	getline(in,line);
	cout << "Reference File Data:" << endl;
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
							opts.POW2 = POW;
						}
						if(abs(opts.POW2-POW) < pow(10.0,-3) && abs(opts.EXP2-EXP) < pow(10.0,-3)){
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
									opts.mapData.push_back(num1); // save value
									opts.mapi.push_back(i); // save row
									opts.mapj.push_back(j); // save col
									ss >> num1;
									if(!ss.fail()){
										//end of line not reached
										opts.mapData.push_back(num2); // save value
										opts.mapi.push_back(i); // save row
										opts.mapj.push_back(j+1); // save col
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
	return found;
}

/////////////////////
//Read Casmo Input //
/////////////////////

bool ref_casmo(options &opts){
	// declare more variables
	double POW, EXP, num1, num2;
	bool found = false;
	int i=0, j=0;

	stringstream ss;

	// clear vectors
	opts.mapData.clear();
	opts.mapi.clear();
	opts.mapj.clear();

	// create input file stream
	ifstream in(opts.ref_file.c_str());
	if(!in.fail()) opts.ref_present = true;

	// look for the startIdent string given below
	string str, line, startIdent = " 2RR1 Neutron Detector (Relative)";

	// read output file data
	getline(in,line);
	cout << "Reference File Data:" << endl;
	while(!in.fail()){
		// read until startIndent string found
		if(line == startIdent){
			// startIdent string found
			getline(in,line);
			found = 1;
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
						opts.mapData.push_back(num1); // save value
						opts.mapi.push_back(i); // save row
						opts.mapj.push_back(j); // save col
					}
					ss >> num1;
					if(!ss.fail()){
						//end of line not reached
						if(num2 != 0){
							opts.mapData.push_back(num2); // save value
							opts.mapi.push_back(i);  //save row
							opts.mapj.push_back(j+1);  //save col
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
	return found;
}
