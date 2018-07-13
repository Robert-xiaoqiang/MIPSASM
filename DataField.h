#ifndef _DATAFIELD_H_
#define _DATAFIELD_H_

#include <vector>
#include <map>
#include <string>

using namespace std;

class DataField {
	const string text;
	map<string, string> macros;         // copy to CodeParse

	static map<string, int> bytesMap;

	void strip(string& line) const;
	void stripComments(string & line) const;
	unsigned int countBytes() const;
	void parseUnit(string& strData, unsigned curBytes);
public:
	unsigned int dataAddr;
    vector<pair<unsigned int, unsigned int>> datas;
    map<string, unsigned int> labelMap; // copy to CodeParse
			
	DataField(const string& text = "");
	~DataField();
	void parse();
	void parseDatas(const string& str);
};

#endif
