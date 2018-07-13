#ifndef _COEPARSE_H_
#define _COEPARSE_H_

#include "CodeParse.h"
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

enum Source {BIN, COE};

class CoeBinParse {
	static map<unsigned, string> regMap;
	static map<pair<unsigned, unsigned>, string> instMap;
	static map<string, pair<Type, TypeLow>> typeMap;

	const string fileName;
	vector<unsigned> insts;
	vector<string> codes;
    map<unsigned, string> labelMap; // wordaddress: label_X
	void strip(string& line) const;
	void typeR(unsigned inst);
	void typeI(unsigned inst, unsigned curPos); // index of insts vector !
	void typeJ(unsigned inst, unsigned curPos);
    void output(const string& outFile) const;
public:
	CoeBinParse(const string& fileName = "");
	virtual ~CoeBinParse();
	void format(Source s);
    void convertToAsm(const string& outFile);
};

#endif
