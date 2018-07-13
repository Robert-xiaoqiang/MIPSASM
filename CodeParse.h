#ifndef _CODE_PARSE_H
#define _CODE_PARSE_H

#include "DataField.h"
#include <string>
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;

enum OpenMethod { BINARY, TEXT };
enum Code {DATA, INST};
enum Type { R, I, J, P };
enum TypeLow {
	RRD, RRS, RRDRS, RRDRT, RRTRD, RRTRS, RRSRT, RRSRD, RRDRSRT, RRDRTRS, RRDRTSH,
    IRTRSIM, IRSRTLA, IRTIMRS, IRSLA, IRTIM, JNULL,
	PI, PD,
};
class CodeParse {
	const string fileName = "";       // {""}
	unsigned int baseAddr = 0;
	vector<DataField> datas;          // DataAddre & data
	vector<pair<Code, string>> instructions;      // , ,(margin has no whitespace)
                                      // instructions for word address 0, 1, 2, 3, ...,
    map<string, unsigned int> labelMap; // label -> word address

	static map<string, pair<Type, TypeLow>> typeMap;   // inst_name -> (type, oprands_count)
	static map<string, unsigned int> opMap;   // inst_name -> opcode
	static map<string, unsigned int> regMap;     // reg_name  -> address of regs
	static map<string, unsigned int> funcMap; // r_type_inst_name -> funcCode

	void typeR(ofstream& out, const string& inst, OpenMethod method) const;
    void typeI(ofstream& out, const string& inst, OpenMethod method, unsigned curPos);
    void typeJ(ofstream& out, const string& inst, OpenMethod method);
	void typeP(ofstream& out, const string& inst, OpenMethod method) const;
	void strip(string& line) const;
	void stripComments(string& line) const;
	string stripCommentsSegment(const string& segment) const;

	void parseComma(const string& inst, string& f, string& s, string& t) const;
	void parseComma(const string& inst, string& f, string& s) const;
	void parseComma(const string& inst, string& f) const;
    void updateLabelMap(unsigned address, int offset);
public:
	CodeParse(const string& fileName);
	~CodeParse();

	void format();
	void formatInst(istringstream& is);
	void formatPseudo();
    void convertToCoe(const string& outFile);
    void convertToBin(const string& outFile);
};

#endif
