
#include "koroush_reader.h"

using namespace std;

kclass::kclass(void){
	statepoints = 0;
	current_pt = 1;
}

bool kclass::read_input(string fname){
	bool exit_success = true;
	vector<vector<double> > mapData;

	vector<int> inst_i,inst_j;

	int tmp1[] = {10,8,12,2,5,7,4,6,7,9,11,13,2,7,10,4,8,0,13,2,6,7,11,4,8,10,12,14,
			1,2,3,5,0,9,13,6,7,11,1,5,9,10,12,14,4,7,6,12,13,1,3,5,7,3,8,10,5,7};
	vector<int> mapi (tmp1, tmp1 + sizeof(tmp1) / sizeof(tmp1[0]) );
	int tmp2[] = {14,14,13,13,13,13,12,12,12,11,11,11,11,11,10,10,10,9,9,9,9,9,8,8,8,
			7,7,7,7,7,7,7,6,6,6,6,6,5,5,5,4,4,4,4,4,4,3,2,2,2,2,2,2,1,1,0,0,0};
	vector<int> mapj (tmp2, tmp2 + sizeof(tmp2) / sizeof(tmp2[0]) );
	int tmp3[15] = {7,11,13,13,15,15,15,15,15,15,15,13,13,11,7};
	vector<int> widths (tmp3, tmp3 + sizeof(tmp3) / sizeof(tmp3[0]));

	// convert to normal read-in form
	int row;
	for(int k=0; k<mapi.size(); k++){
		row = mapi.at(k);
		if(row == 0 or row == 14) mapj.at(k) -= 4;
		else if(row == 1 or row == 13) mapj.at(k) -= 2;
		else if(row == 2 or row == 3 or row == 12 or row == 11) mapj.at(k) -= 1;
	}
	if(mapi.size() != mapj.size()) cout << "ERROR: mapi/mapj mismatch" << endl;

	// start reading file

	ifstream in;
	stringstream ss;
	string str,line;
	int count;
	double val;
	int pts = 0;

	in.open(fname.c_str());

	getline(in,line);
	while(!in.fail()){

		pts++;
		str = "";
		vector <double> data;
		count = 0;

		for(int i=0; i<line.size(); i++){
			if(line.at(i) != '\t'){
				// reading in numbers
				str.push_back(line.at(i));
			}
			if(line.at(i) == '\t' or i == line.size()-1){
				ss.str("");
				ss.clear();

				// reached tab -> save value
				ss << str;
				ss >> val;

				if(count == 0){
					exp_vals.push_back(val);
				}
				else{
					if(str == ""){
						data.push_back(-1);
					}
					else data.push_back(val);
				}

				str = "";
				count++;
			}
		}
		if(line.at(line.size()-1) == '\t'){
			data.push_back(-1);
		}

		//error checking
		if(mapi.size() != data.size()){
			cout << "ERROR: mapi/ koroush data file mismatch at line " << pts << endl;
			cout << "mapi size = " << mapi.size() << endl;
			cout << "data size = " << data.size() << endl;
			cout << "Last data = " << data.at(data.size()-1) << endl;
		}

		mapData.push_back(data);
		getline(in,line);
	}

	// create maps

	int pos;
	korData.resize(pts);
	kori.resize(pts);
	korj.resize(pts);
	for(int k=0; k<pts; k++){
		for(int i=0; i<15; i++){
			for(int j=0; j<widths.at(i); j++){
				korData.at(k).push_back(-1);
				kori.at(k).push_back(i);
				korj.at(k).push_back(j);
			}
		}
		for(int ii=0; ii<mapi.size(); ii++){
			pos = 0;
			for(int kk=0; kk<mapi.at(ii); kk++){
				pos += widths.at(kk);
			}
			pos += mapj.at(ii);
			korData.at(k).at(pos) = mapData.at(k).at(ii);
		}
	}

	statepoints = pts;
	return exit_success;
}

bool kclass::get_statepoint(options & opts){
	bool last;
	string pt;

	opts.EXP = exp_vals.at(current_pt-1);
	opts.POW = 100;

	// transfer data to opts object
	opts.mapi = kori.at(current_pt-1);
	opts.mapj = korj.at(current_pt-1);
	opts.mapData = korData.at(current_pt-1);

	// create new file name
	stringstream ss;
	ss << current_pt;
	ss >> pt;
	opts.outfile = "mod_map";
	opts.outfile.append(pt);
	opts.outfile.append(".out");

	if(current_pt == statepoints){
		last = true;
	}
	else last = false;

	current_pt++;
	return last;
}
