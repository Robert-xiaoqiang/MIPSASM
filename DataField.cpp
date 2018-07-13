#include "DataField.h"
#include <regex>
#include <cctype>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

map<string, int> DataField::bytesMap = {
	{"db", 1}, {"DB", 1},
	{"dw", 2}, {"DW", 2},
	{"dd", 4}, {"DD", 4},
	{"EQU", -1}, {"equ", -1},
	{"RSEB", -2}, {"rseb", -2},
	{"RSEW", -3}, {"rsew", -3},
	{"RSED", -4}, {"rsed", -4},
};

DataField::DataField(const string& text):
	text(text) // copy ctor
{

}

DataField::~DataField()
{

}

void DataField::strip(string& line) const
{
	if(!line.empty()) {
		if(isspace(line.front())) {
			line.erase(line.begin(), find_if(line.begin(), line.end(),
				[](char ch) {return !isspace(ch);}));
		}
	}
	if(!line.empty()) {
		if(isspace(line.back())) {
			line.erase(find_if(line.rbegin(), line.rend(),
				[](char ch) {return !isspace(ch); }).base() + 1, line.end());
		}
	}
}

void DataField::parseUnit(string& strData, unsigned curBytes)
{

	if(strData.front() == '\'' || strData.front() == '\"') {
		strData.pop_back();
		strData.erase(0, 1);
		for(auto i : strData) {
			datas.push_back(make_pair(1, static_cast<unsigned>(i) & 0x000000ffu)); // not necessary!
		}
		unsigned upper = !(strData.length() % curBytes) ? strData.length() / curBytes : strData.length() / curBytes + 1;
		for(unsigned i = 0; i < curBytes * upper - strData.length(); i++)
			datas.push_back(make_pair(1, 0u));
	} else {
		datas.push_back(make_pair(curBytes, 
					static_cast<unsigned>(stoul(strData, 0, 16)) & (0xffffffffu >> (32 - curBytes * 8))));
	}
}

// curPos Byte address
void DataField::parseDatas(const string& str)
{
	string code(str);
	strip(code);
	string data_pattern = "([ \t]*,[ \t\n]*)|([ \t]*;[ \t\n]*)";
	regex data_re(data_pattern);
	sregex_iterator site(code.begin(), code.end(), data_re), site_end;
	bool newWidth = true;
	int curBytes = 4;         // default defined double word 32 bits

	for(; site != site_end; site++) {
		string rawData = site->prefix();
		strip(rawData);
		istringstream is(rawData);
		string bytesCode;
		string strData;
		if(newWidth){
			curBytes = 4;
			newWidth = false;
		}
		if(!(*site)[1].length()) // not comma
			newWidth = true;
		is >> bytesCode;
		if(bytesMap.find(bytesCode) != bytesMap.end()) {
			curBytes = bytesMap[bytesCode];
			is >> strData;
			try {
				parseUnit(strData, curBytes);
			}
			catch(exception& e) {
				cout << strData << ": too large" << endl;
			}
		} else {
			istringstream isnew(rawData);
			isnew >> strData;
			try {
				parseUnit(strData, curBytes);
			}
			catch(exception& e) {
				cout << strData << ": too large" << endl;
			}
		}

	}
		
	return;
}

//
// DataAddre start text!!!
// 
// XXXXXXX;
// label: bytes datas;
//

unsigned int DataField::countBytes() const
{
	unsigned ret = 0u;
	for(const auto& i : datas)
		ret += i.first;

	return ret;
}

void DataField::parse()
{ 
	string label_pattern = "(;[\n\t ]*)([[:alnum:]]+)([ \t]*:[ \t\n]*)";
	regex label_re(label_pattern);
	string text(this->text);
	strip(text);
	// first find dataAddre
	string::size_type semicolon = text.find(';');
	dataAddr = static_cast<unsigned>(stoi(text.substr(0, semicolon), 0, 16));
	text.erase(0, semicolon); // not necessary

	sregex_iterator site(text.begin(), text.end(), label_re), site_end;
	sregex_iterator next(site), temp(site);
	if(site == site_end) {
		parseDatas(text.substr(1));
    } else {
        labelMap.insert(make_pair((*site)[2].str(), (dataAddr + countBytes()) >> 2));
        next++;
        for(; next != site_end; next++) {
            labelMap.insert(make_pair((*site)[2].str(), (dataAddr + countBytes()) >> 2)); // Byte addr + Byte addr
            parseDatas(string(next->prefix()) + ";");
            temp = next;
        }
        parseDatas(temp->suffix());
    }
}
