#include "CodeParse.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <regex>
#include <iterator>

map<string, pair<Type, TypeLow>> CodeParse::typeMap = {
    { "nop",{ R, RRDRSRT } },{ "NOP",{ R, RRDRSRT } },
    { "add",{ R, RRDRSRT } },{ "ADD",{ R, RRDRSRT } },
    { "addu",{ R, RRDRSRT } },{ "ADDU",{ R, RRDRSRT } },
    { "and",{ R, RRDRSRT } },{ "AND",{ R, RRDRSRT } },
    { "nor",{ R, RRDRSRT } },{ "NOR",{ R, RRDRSRT } },
    { "or",{ R, RRDRSRT } },{ "OR",{ R, RRDRSRT } },
    { "sub",{ R, RRDRSRT } },{ "SUB",{ R, RRDRSRT } },
    { "subu",{ R, RRDRSRT } },{ "SUBU",{ R, RRDRSRT } },
    { "xor",{ R, RRDRSRT } },{ "XOR",{ R, RRDRSRT } },
    { "slt",{ R, RRDRSRT } },{ "SLT",{ R, RRDRSRT } },
    { "sltu",{ R, RRDRSRT } },{ "SLTU",{ R, RRDRSRT } },
    { "mul",{ R, RRDRSRT } },{ "MUL",{ R, RRDRSRT } },
    { "sll",{ R, RRDRTSH } },{ "SLL",{ R, RRDRTSH } },
    { "sra",{ R, RRDRTSH } },{ "SRA",{ R, RRDRTSH } },
    { "srl",{ R, RRDRTSH } },{ "SRL",{ R, RRDRTSH } },
    { "sllv",{ R, RRDRTRS } },{ "SLLV",{ R, RRDRTRS } },
    { "srav",{ R, RRDRTRS } },{ "SRAV",{ R, RRDRTRS } },
    { "srlv",{ R, RRDRTRS } },{ "SRLV",{ R, RRDRTRS } },
    { "clo",{ R, RRDRS } },{ "CLO",{ R, RRDRS } },
    { "clz",{ R, RRDRS } },{ "CLZ",{ R, RRDRS } },
    { "mtc1",{ R, RRDRS } },{ "MTC1",{ R, RRDRS } },
    { "div",{ R, RRSRT } },{ "DIV",{ R, RRSRT } },
    { "divu",{ R, RRSRT } },{ "DIVU",{ R, RRSRT } },
    { "mult",{ R, RRSRT } },{ "MULT",{ R, RRSRT } },
    { "multu",{ R, RRSRT } },{ "MULTU",{ R, RRSRT } },
    { "madd",{ R, RRSRT } },{ "MADD",{ R, RRSRT } },
    { "maddu",{ R, RRSRT } },{ "MADDU",{ R, RRSRT } },
    { "msub",{ R, RRSRT } },{ "MSUB",{ R, RRSRT } },
    { "mfc0",{ R, RRTRD } },{ "MFC0",{ R, RRTRD } },
    { "mfc1",{ R, RRTRS } },{ "MFC1",{ R, RRTRS } },
    { "mtc0",{ R, RRDRT } },{ "MTC0",{ R, RRDRT } },
    { "jalr",{ R, RRSRD } },{ "JALR",{ R, RRSRD } },
    { "mfhi",{ R, RRD } },{ "MFHI",{ R, RRD } },
    { "mflo",{ R, RRD } },{ "MFLO",{ R, RRD } },
    { "mthi",{ R, RRS } },{ "MTHI",{ R, RRS } },
    { "mtlo",{ R, RRS } },{ "MTLO",{ R, RRS } },
    { "jr",{ R, RRS } },{ "JR",{ R, RRS } },
    { "addi",{ I, IRTRSIM } },{ "ADDI",{ I, IRTRSIM } },
    { "addiu",{ I, IRTRSIM } },{ "ADDIU",{ I, IRTRSIM } },
    { "andi",{ I, IRTRSIM } },{ "ANDI",{ I, IRTRSIM } },
    { "ori",{ I, IRTRSIM } },{ "ORI",{ I, IRTRSIM } },

    { "lui",{ I, IRTIM } },{ "LUI",{ I, IRTIM } },

    { "xori",{ I, IRTRSIM } },{ "XORI",{ I, IRTRSIM } },
    { "slti",{ I, IRTRSIM } },{ "SLTI",{ I, IRTRSIM } },
    { "sltiu",{ I, IRTRSIM } },{ "SLTIU",{ I, IRTRSIM } },
    { "beq",{ I, IRSRTLA } },{ "BEQ",{ I, IRSRTLA } },
    { "bne",{ I, IRSRTLA } },{ "BNE",{ I, IRSRTLA } },
    { "bgez",{ I, IRSLA } },{ "BGEZ",{ I, IRSLA } },
    { "bgezal",{ I, IRSLA } },{ "BGEZAL",{ I, IRSLA } },
    { "bgtz",{ I, IRSLA } },{ "BGTZ",{ I, IRSLA } },
    { "blez",{ I, IRSLA } },{ "BLEZ",{ I, IRSLA } },
    { "bltzal",{ I, IRSLA } },{ "BLTZAL",{ I, IRSLA } },
    { "bltz",{ I, IRSLA } },{ "BLTZ",{ I, IRSLA } },
    { "lb",{ I, IRTIMRS } },{ "LB",{ I, IRTIMRS } },
    { "lbu",{ I, IRTIMRS } },{ "LBU",{ I, IRTIMRS } },
    { "lh",{ I, IRTIMRS } },{ "LH",{ I, IRTIMRS } },
    { "lhu",{ I, IRTIMRS } },{ "LHU",{ I, IRTIMRS } },
    { "lw",{ I, IRTIMRS } },{ "LW",{ I, IRTIMRS } },
    { "sb",{ I, IRTIMRS } },{ "SB",{ I, IRTIMRS } },
    { "sh",{ I, IRTIMRS } },{ "SH",{ I, IRTIMRS } },
    { "sw",{ I, IRTIMRS } },{ "SW",{ I, IRTIMRS } },
    { "j",{ J, JNULL } },{ "J",{ J, JNULL } },
    { "jal",{ J, JNULL } },{ "JAL",{ J, JNULL } },

    { ".data", {P, PD} },{ ".DATA",{ P, PD } },
    { ".word", {P, PD} },{ ".WORD",{ P, PD } },
    { ".byte", {P, PD} },{ ".BYTE",{ P, PD } },
    { ".half",{ P, PD } },{ ".HALF",{ P, PD } },
    { "la",	   {P, PI} },{ "LA",{ P, PI } },
};   // inst_name -> type
map<string, unsigned int> CodeParse::opMap = {
	{ "nop", 0x0 },{ "NOP", 0x0 },
	{ "add", 0x0 },{ "ADD", 0x0 },
	{ "addu", 0x0 },{ "ADDU", 0x0 },
	{ "and", 0x0 },{ "AND", 0x0 },
	{ "clo", 0x1c },{ "CLO", 0x1c },
	{ "clz", 0x1c },{ "CLZ", 0x1c },
	{ "div", 0x0 },{ "DIV", 0x0 },
	{ "divu", 0x0 },{ "DIVU", 0x0 },
	{ "mul", 0x1c },{ "MUL", 0x1c },
	{ "mult", 0x0 },{ "MULT", 0x0 },
	{ "multu", 0x0 },{ "MULTU", 0x0 },
	{ "madd", 0x1c },{ "MADD", 0x1c },
	{ "maddu", 0x1c },{ "MADDU", 0x1c },
	{ "msub", 0x1c },{ "MSUB", 0x1c },
	{ "nor", 0x0 },{ "NOR", 0x0 },
	{ "or", 0x0 },{ "OR", 0x0 },
	{ "sll", 0x0 },{ "SLL", 0x0 },
	{ "sllv", 0x0 },{ "SLLV", 0x0 },
	{ "sra", 0x0 },{ "SRA", 0x0 },
	{ "srav", 0x0 },{ "SRAV", 0x0 },
	{ "srl", 0x0 },{ "SRL", 0x0 },
	{ "srlv", 0x0 },{ "SRLV", 0x0 },
	{ "sub", 0x0 },{ "SUB", 0x0 },
	{ "subu", 0x0 },{ "SUBU", 0x0 },
	{ "xor", 0x0 },{ "XOR", 0x0 },
	{ "slt", 0x0 },{ "SLT", 0x0 },
	{ "sltu", 0x0 },{ "SLTU", 0x0 },
	{ "mfhi", 0x0 },{ "MFHI", 0x0 },
	{ "mflo", 0x0 },{ "MFLO", 0x0 },
	{ "mthi", 0x0 },{ "MTHI", 0x0 },
	{ "mtlo", 0x0 },{ "MTLO", 0x0 },
	{ "mfc0", 0x10 },{ "MFC0", 0x10 },
	{ "mfc1", 0x11 },{ "MFC1", 0x11 },
	{ "mtc0", 0x10 },{ "MTC0", 0x10 },
	{ "mtc1", 0x11 },{ "MTC1", 0x11 },
	{ "jalr", 0x0 },{ "JALR", 0x0 },
	{ "jr", 0x0 },{ "JR", 0x0 },
	{ "addi", 0x8 },{ "ADDI", 0x8 },
	{ "addiu", 0x9 },{ "ADDIU", 0x9 },
	{ "andi", 0xc },{ "ANDI", 0xc },
	{ "ori", 0xd },{ "ORI", 0xd },
	{ "lui", 0xf },{ "LUI", 0xf },
	{ "xori", 0xe },{ "XORI", 0xe },
	{ "slti", 0xa },{ "SLTI", 0xa },
	{ "sltiu", 0xb },{ "SLTIU", 0xb },
	{ "beq", 0x4 },{ "BEQ", 0x4 },
	{ "bgez", 0x1 },{ "BGEZ", 0x1 },
	{ "bgezal", 0x1 },{ "BGEZAL", 0x1 },
	{ "bgtz", 0x7 },{ "BGTZ", 0x7 },
	{ "blez", 0x6 },{ "BLEZ", 0x6 },
	{ "bltzal", 0x1 },{ "BLTZAL", 0x1 },
	{ "bltz", 0x1 },{ "BLTZ", 0x1 },
	{ "bne", 0x5 },{ "BNE", 0x5 },
	{ "lb", 0x20 },{ "LB", 0x20 },
	{ "lbu", 0x24 },{ "LBU", 0x24 },
	{ "lh", 0x21 },{ "LH", 0x21 },
	{ "lhu", 0x25 },{ "LHU", 0x25 },
	{ "lw", 0x23 },{ "LW", 0x23 },
	{ "sb", 0x28 },{ "SB", 0x28 },
	{ "sh", 0x29 },{ "SH", 0x29 },
	{ "sw", 0x2b },{ "SW", 0x2b },
	{ "j", 0x2 },{ "J", 0x2 },
	{ "jal", 0x3 },{ "JAL", 0x3 },
};   // inst_name -> opcode
map<string, unsigned int> CodeParse::regMap = {
	{ "$zero",0 },{ "$ZERO",0 },
	{ "$at", 1 },{ "$AT", 1 },
	{ "$v0", 2 },{ "$V0", 2 },
	{ "$v1", 3 },{ "$V1", 3 },
	{ "$a0", 4 },{ "$A0", 4 },
	{ "$a1", 5 },{ "$A1", 5 },
	{ "$a2", 6 },{ "$A2", 6 },
	{ "$a3", 7 },{ "$A3", 7 },
	{ "$t0", 8 },{ "$T0", 8 },
	{ "$t1", 9 },{ "$T1", 9 },
	{ "$t2", 10 },{ "$T2", 10 },
	{ "$t3", 11 },{ "$T3", 11 },
	{ "$t4", 12 },{ "$T4", 12 },
	{ "$t5", 13 },{ "$T5", 13 },
	{ "$t6", 14 },{ "$T6", 14 },
	{ "$t7", 15 },{ "$T7", 15 },
	{ "$s0", 16 },{ "$S0", 16 },
	{ "$s1", 17 },{ "$S1", 17 },
	{ "$s2", 18 },{ "$S2", 18 },
	{ "$s3", 19 },{ "$S3", 19 },
	{ "$s4", 20 },{ "$S4", 20 },
	{ "$s5", 21 },{ "$S5", 21 },
	{ "$s6", 22 },{ "$S6", 22 },
	{ "$s7", 23 },{ "$S7", 23 },
	{ "$t8", 24 },{ "$T8", 24 },
	{ "$t9", 25 },{ "$T9", 25 },
	{ "$k0", 26 },{ "$K0", 26 },
	{ "$k1", 27 },{ "$K1", 27 },
	{ "$gp", 28 },{ "$GP", 28 },
	{ "$sp", 29 },{ "$SP", 29 },
	{ "$fp", 30 },{ "$FP", 30 },
	{ "$ra", 31 },{ "$RA", 31 },
};     // reg_name  -> address of regs
map<string, unsigned int> CodeParse::funcMap = {
	{ "nop", 0x0 },{ "NOP", 0x0 },
	{ "add", 0x20 },{ "ADD", 0x20 },
	{ "addu", 0x21 },{ "ADDU", 0x21 },
	{ "and", 0x24 },{ "AND", 0x24 },
	{ "clo", 0x21 },{ "CLO", 0x21 },
	{ "clz", 0x20 },{ "CLZ", 0x20 },
	{ "div", 0x1a },{ "DIV", 0x1a },
	{ "divu", 0x1b },{ "DIVU", 0x1b },
	{ "mul", 0x2 },{ "MUL", 0x2 },
	{ "mult", 0x18 },{ "MULT", 0x18 },
	{ "multu", 0x19 },{ "MULTU", 0x19 },
	{ "madd", 0x0 },{ "MADD", 0x0 },
	{ "maddu", 0x1 },{ "MADDU", 0x1 },
	{ "msub", 0x4 },{ "MSUB", 0x4 },
	{ "nor", 0x27 },{ "NOR", 0x27 },
	{ "or", 0x25 },{ "OR", 0x25 },
	{ "sll", 0x0 },{ "SLL", 0x0 },
	{ "sllv", 0x4 },{ "SLLV", 0x4 },
	{ "sra", 0x3 },{ "SRA", 0x3 },
	{ "srav", 0x7 },{ "SRAV", 0x7 },
	{ "srl", 0x2 },{ "SRL", 0x2 },
	{ "srlv", 0x6 },{ "SRLV", 0x6 },
	{ "sub", 0x22 },{ "SUB", 0x22 },
	{ "subu", 0x23 },{ "SUBU", 0x23 },
	{ "xor", 0x26 },{ "XOR", 0x26 },
	{ "slt", 0x2a },{ "SLT", 0x2a },
	{ "sltu", 0x2b },{ "SLTU", 0x2b },
	{ "mfhi", 0x10 },{ "MFHI", 0x10 },
	{ "mflo", 0x12 },{ "MFLO", 0x12 },
	{ "mthi", 0x11 },{ "MTHI", 0x11 },
	{ "mtlo", 0x13 },{ "MTLO", 0x13 },
	{ "mfc0", 0x0 },{ "MFC0", 0x0 },
	{ "mfc1", 0x0 },{ "MFC1", 0x0 },
	{ "mtc0", 0x0 },{ "MTC0", 0x0 },
	{ "mtc1", 0x0 },{ "MTC1", 0x0 },
	{ "jalr", 0x9 },{ "JALR", 0x9 },
	{ "jr", 0x8 },{ "JR", 0x8 },
}; // r_type_inst_name -> funcCode

CodeParse::CodeParse(const string& fileName):
	fileName(fileName)
{
	
}

CodeParse::~CodeParse()
{

}

void CodeParse::strip(string& line) const
{
	if(!line.empty()) {
		if(isspace(line.front())) {
			line.erase(line.begin(), find_if(line.begin(), line.end(),
				[](char ch) {return !isspace(ch); }));
		}
	}
	if(!line.empty()) {
		if(isspace(line.back())) {
			line.erase(find_if(line.rbegin(), line.rend(),
				[](char ch) {return !isspace(ch); }).base() + 1, line.end());
		}
	}
}

void CodeParse::stripComments(string& line) const
{
	
	string::size_type comments = line.find('#');
	if(comments != string::npos) {
		line.erase(comments, line.size() - comments);
	}
	string::size_type comments2 = line.find("//");
	if(comments2 != string::npos)
		line.erase(comments2, line.size() - comments2);
}

string CodeParse::stripCommentsSegment(const string& segment) const
{
	string res, line;
	istringstream is(segment);
	while(is.good()) {
		getline(is, line);
		stripComments(line); strip(line);
		res += line + "\n";
	}
	return res;     // assignment operation
}

// exclude op
void CodeParse::parseComma(const string& inst, string& f, string& s, string& t) const
{
	string::size_type comma1 = inst.find(',');
	f = inst.substr(0, comma1);
	strip(f);
	string::size_type comma2 = inst.find(',', comma1 + 1);
	s = inst.substr(comma1 + 1, comma2 - comma1 - 1);
	strip(s);
	t = inst.substr(comma2 + 1, inst.size() - comma2 - 1);
	strip(t);
}

// exclude op
void CodeParse::parseComma(const string& inst, string& f, string& s) const
{
	string::size_type comma1 = inst.find(',');
	f = inst.substr(0, comma1);
	strip(f);
	s = inst.substr(comma1 + 1, inst.size() - comma1 - 1);
	strip(s);
}

void CodeParse::parseComma(const string& inst, string& f) const
{
	f = inst;
	strip(f);
}

// address is chanaged; // low address is invarient!!!
// offset is newAdd instruction or datas
void CodeParse::updateLabelMap(unsigned address, int offset)
{
	for(auto ite = labelMap.begin(); ite != labelMap.end(); ite++) {
		if(ite->second > address) {
			if(offset > 0) {
				ite->second += offset;  // unsigned implicit conversion
			} else if(offset < 0) {
				ite->second -= -offset;
			}
			//cout << ite->first << "changed!" << endl;
		}
	}
}

void CodeParse::formatInst(istringstream& is)
{
	while(is) {
		string line;
		getline(is, line); // may be empty line!!!
		stripComments(line);
		strip(line);
		if(!line.empty()) {
			string::size_type label = line.find(':');
			if(label != string::npos) {
				string temp(line.substr(0, label + 1));
				temp.pop_back();
				strip(temp);
				labelMap.insert(make_pair(temp, (baseAddr >> 2) + instructions.size())); // word address;
				line.erase(0, label + 1);
			}
			string::size_type semicolon = line.rfind(';');
			if(semicolon != string::npos) {
				line.erase(semicolon, line.size() - semicolon);
			}
			strip(line);
			if(!line.empty() && (line.find(".text") != string::npos || 
								 line.find(".TEXT") != string::npos)) {
				;
			} else if(!line.empty()) {
				instructions.push_back({ INST, line });
			}
		}
	}
}

void CodeParse::formatPseudo()
{
    for(unsigned i = 0; i < instructions.size(); i++) {
        string s(instructions[i].second);
        string::size_type opIndex = s.find(' ');
        string op = s.substr(0, opIndex);
        if(instructions[i].first == INST) {
            if(typeMap[op].second == PI) {
                if(op == "la" || op == "LA") {
                    s.erase(0, opIndex);
                    strip(s);
                    string rs("$zero"), rt, label;
                    parseComma(s, rt, label);
                    updateLabelMap(i, 1);
                    instructions.erase(instructions.begin() + i);
                    unsigned high = ((labelMap[label] << 2) & (0xffffffffu - 0xffffu)) >> 16;
                    unsigned low = ((labelMap[label] << 2) & 0xffffu);
                    instructions.insert(instructions.begin() + i, make_pair(INST,
                        "lui " + rt + ", " + to_string(high) + ";"));
                    instructions.insert(instructions.begin() + i + 1, make_pair(INST,
                        "ori " + rt + ", " + rt + ", " + to_string(low) + ";"));
                }
            } else if(typeMap[op].second == PD) {
                string dataPattern = "\\s*(,|;)\\s*";
                regex data_re(dataPattern);
                vector<pair<Code, string>> tempData; // word count!!!
                if(op == ".data" || op == ".DATA") {
                    s.erase(0, opIndex);
                    strip(s);
                    unsigned address = static_cast<unsigned>(stoul(s, 0, 0)); // byte address; 4 alignment!
                    for(unsigned k = (baseAddr >> 2) + i; k < (address >> 2); k++) {
                        tempData.push_back({ DATA, "00000000" });
                    }
                    // error_no !!!

                } else if(op == ".word" || op == ".WORD") {
                    s.erase(0, opIndex);
                    strip(s);
                    if(s.back() != ',') s += ";";
                    sregex_iterator site(s.begin(), s.end(), data_re), site_end;
                    for(; site != site_end; site++) {
                        ostringstream os;
                        os << hex << uppercase << internal << setw(8) << setfill('0')
                           << static_cast<unsigned>(stoul(site->prefix(), 0, 0));
                        tempData.push_back({ DATA,  os.str()});
                    }

                } else if(op == ".byte" || op == ".BYTE") {
                    s.erase(0, opIndex);
                    strip(s);
                    if(s.back() != ',') s += ";";
                    sregex_iterator site(s.begin(), s.end(), data_re), site_end;
                    unsigned curByte = 0u;
                    unsigned wordValue = 0u;
                    for(; site != site_end; site++) {
                        unsigned byteValue = static_cast<unsigned>(stoul(site->prefix(), 0, 0)) & 0xffu;
                        wordValue += byteValue << curByte * 8;
                        curByte++;
                        if(curByte == 4) {
                            ostringstream os;
                            os << hex << uppercase << internal << setw(8) << setfill('0') << wordValue;
                            tempData.push_back({ DATA, os.str() });
                            curByte = 0u;
                            wordValue = 0u;
                        }
                    }
                    if(curByte) {
                        ostringstream os;
                        os << hex << uppercase << internal << setw(8) << setfill('0') << wordValue;
                        tempData.push_back({ DATA, os.str() });
                    }
                } else if(op == ".half" || op == ".HALF") {
                    s.erase(0, opIndex);
                    strip(s);
                    if(s.back() != ',') s += ";";
                    sregex_iterator site(s.begin(), s.end(), data_re), site_end;
                    unsigned curByte = 0u;
                    unsigned wordValue = 0u;
                    for(; site != site_end; site++) {
                        unsigned halfValue = static_cast<unsigned>(stoul(site->prefix(), 0, 0)) & 0xffffu;
                        wordValue += halfValue << curByte * 8;
                        curByte += 2;
                        if(curByte == 4) {
                            ostringstream os;
                            os << hex << uppercase << internal << setw(8) << setfill('0') << wordValue;
                            tempData.push_back({ DATA, os.str() });
                            curByte = 0u;
                            wordValue = 0u;
                        }
                    }
                    if(curByte) {
                        ostringstream os;
                        os << hex << uppercase << internal << setw(8) << setfill('0') << wordValue;
                        tempData.push_back({ DATA, os.str() });
                    }
                }
                instructions.erase(instructions.begin() + i);
                instructions.insert(instructions.begin() + i, tempData.begin(), tempData.end());
                updateLabelMap((baseAddr >> 2) + i, static_cast<int>(tempData.size()) - 1);
            }
        }
    }
}

void CodeParse::format()
{
	ifstream inFile;
	string baseAddr_pattern = "(#[ \t\r\f\v]*baseAddr[ \t]*)([^;]+)";
	string dataAddr_pattern = "#[ \t]*DataAddre[ \t]*:";
	string text_pattern = "(((.text)|(.TEXT))\\s*)([^;\\s]+)";

	regex baseAddr_re(baseAddr_pattern);
	regex dataAddr_re(dataAddr_pattern);
	regex text_re(text_pattern);
	
	if(!fileName.empty()) {
		inFile.open(fileName.c_str());
		inFile >> noskipws;
		istream_iterator<char> is(inFile), is_end;
		string text(is, is_end);
		smatch base;
		if(regex_search(text, base, baseAddr_re)) {
			baseAddr = static_cast<unsigned int>(stoi(base[2].str(), 0, 16));
		}
		if(regex_search(text, base, text_re)) {
			baseAddr = static_cast<unsigned int>(stoi(base[5].str(), 0, 0));
		}
		sregex_iterator site(text.begin(), text.end(), dataAddr_re), site_end;
		if(site == site_end) {
			istringstream is(text);
			formatInst(is);
		} else {
			string inst = site->prefix();
			istringstream is(inst);
			formatInst(is);
			//sregex_iterator next(site), temp(next);
			auto next = site, temp = site;
			next++;
			for(; next != site_end; next++) {
				DataField d(stripCommentsSegment(string(next->prefix())));
				d.parse();
				datas.push_back(d);
				temp = next;
			}
			DataField d(stripCommentsSegment(string(temp->suffix())));
			d.parse();
			datas.push_back(d);
            labelMap.insert(d.labelMap.begin(), d.labelMap.end());
		}
		inFile.close();
        formatPseudo();
	}
}

void CodeParse::typeR(ofstream& out, const string& inst, OpenMethod method) const
{
	string rs = "$zero", rt = "$zero", rd = "$zero", shamt = "0", op;
	istringstream is(inst);
	unsigned res = 0;
	is >> op;
	string temp(inst);
	auto firstWs = find_if(temp.begin(), temp.end(),
		[](char ch) { return isspace(ch); });
	string::size_type opSpace = firstWs - temp.begin();
	temp.erase(0, opSpace);
	strip(temp);
	switch(typeMap[op].second) {
	case RRD:
		parseComma(temp, rd);
		break;
	case RRS:
		parseComma(temp, rs);
		break;
	case RRDRS:
		parseComma(temp, rd, rs);
		break;
	case RRDRT:
		parseComma(temp, rd, rt);
		break;
	case RRTRD:
		parseComma(temp, rt, rd);
		break;
	case RRTRS:
		parseComma(temp, rt, rs);
		break;
	case RRSRT:
		parseComma(temp, rs, rt);
		break;
	case RRSRD:
		parseComma(temp, rs, rd);
		break;
	case RRDRSRT:
	{
		if(op != "nop" && op != "NOP") {
			parseComma(temp, rd, rs, rt);
		}
		break;
	}
	case RRDRTRS:
		parseComma(temp, rd, rt, rs);
		break;
	case RRDRTSH:
		parseComma(temp, rd, rt, shamt);
		break;
	default:
		break;
	}
	res += opMap[op];              // may be 0b000000 
	res = (res << 5) + regMap[rs];
	res = (res << 5) + regMap[rt];
	res = (res << 5) + regMap[rd];
	res = (res << 5) + static_cast<unsigned>(stoi(shamt, 0, 0));
	res = (res << 6) + funcMap[op];
	if(method == TEXT)
		out << hex << uppercase << internal << setw(8) << setfill('0') << res;
	else
		out.write(reinterpret_cast<char *>(&res), 4); // 32bits
}

void CodeParse::typeI(ofstream& out, const string& inst, OpenMethod method, unsigned curPos)
{
	string rs, rt, imm, op, label;
	unsigned res = 0;
	istringstream is(inst);
	is >> op;
	
	string temp(inst);
	auto firstWs = find_if(temp.begin(), temp.end(),
						  [](char ch) { return isspace(ch); });
	string::size_type opSpace = firstWs - temp.begin();
	temp.erase(0, opSpace);
	strip(temp);
	switch(typeMap[op].second) {
	case IRTRSIM:
		parseComma(temp, rt, rs, imm);
		break;
	case IRSRTLA:
		parseComma(temp, rs, rt, label);
		imm = to_string(static_cast<int>(labelMap[label] - (curPos + 1)));
		break;
	case IRTIMRS:
	{
		string immRs;
		parseComma(temp, rt, immRs);
		string::size_type leftBracket = immRs.find('(');
		string::size_type rightBracket = immRs.find(')');
		imm = immRs.substr(0, leftBracket);       // byte address
		rs = immRs.substr(leftBracket + 1, rightBracket - leftBracket - 1);
		strip(rs);
		break;
	}
	case IRSLA:
	{
		parseComma(temp, rs, label);
		imm = to_string((curPos + 1 - labelMap[label]));
		if(op == "bgez" || op == "BGEZ" ) {
			rt = "$at";
		} else if(op == "bgezal" || op == "BGEZAL"){
			rt = "$t3";
		} else if(op == "bgtz" || op == "blez" || op == "BGTZ" || op == "BLEZ") {
			rt = "$zero";
		} else if(op == "bltz"  || op == "BLTZ"){
			rt = "$zero";
		} else if(op == "bltzal" || op == "BLTZAL") {
			rt = "$t2";
		}
		break;
	}
    case IRTIM:
        parseComma(temp, rt, imm);
        break;
	default:
		break;
	}
	res += opMap[op];
	res = (res << 5) + regMap[rs];
	res = (res << 5) + regMap[rt];                         // imm is signed!!!
	res = (res << 16) + (((1 << 16) - 1) & stoi(imm, 0, 0)); // 10 8 16 base OK! == strtoi();
	if(method == TEXT)
		out << hex << uppercase << internal << setw(8) << setfill('0') << res;
	else
		out.write(reinterpret_cast<char *>(&res), 4); // 32bits
}

void CodeParse::typeJ(ofstream& out, const string& inst, OpenMethod method)
{
	// op imm26
	string label, op;
	unsigned res = 0;
	istringstream is(inst);
	is >> op >> label;

	res += opMap[op];
	res = (res << 26) + labelMap[label];
	if(method == TEXT)
		out << hex << uppercase << internal << setw(8) << setfill('0') << res;
	else
		out.write(reinterpret_cast<char *>(&res), 4); // 32bits
}

void CodeParse::convertToCoe(const string& outFile)
{
	ofstream out(outFile.c_str());
	out << "memory_initialization_radix=16;" << endl;
	out << "memory_initialization_vector=" << endl;
	unsigned i = 0;
    for(; i < (baseAddr >> 2); i++) {
		out << "00000000";
		out << ','
			<< (((i + 1) % 8 == 0) ? '\n' : ' '); // flush();
	}
	for(unsigned j = 0; j < instructions.size(); j++) {	// Code: DATA / INST
		if(instructions[j].first == INST) {
			string op;
			istringstream is(instructions[j].second);
			is >> op;
			try {
				if(typeMap[op].first == R) {
					typeR(out, instructions[j].second, TEXT);
				} else if(typeMap[op].first == I) {
					typeI(out, instructions[j].second, TEXT, i);
				} else if(typeMap[op].first == J) {
					typeJ(out, instructions[j].second, TEXT);
				}
				
			}
			catch(exception& e) {
				cout << "invalid format:\n" << instructions[j].second << endl;
				cout << "Please seperate by <SPACE> or <TAB>" << endl;
			}
		} else {
			out << instructions[j].second;
		}
		i++;
		//cout << "i = " << i << ": " << instructions[j].second << endl;
		out << ','
			<< (((i + 1 - 1) % 8 == 0) ? '\n' : ' '); // flush();
	}
	//unsigned byteAddress = baseAddr + instructions.size() * 4; // 4 multiple
	//unsigned wordAddress = byteAddress >> 2;
	unsigned wordAddress = i;
	for(unsigned k = 0; k < datas.size(); k++) {
		unsigned curField = datas[k].dataAddr;
        for(; wordAddress < (curField >> 2); wordAddress++) {
			out << "00000000";
			out << ','
				<< (((wordAddress + 1) % 8 == 0) ? '\n' : ' '); // flush();
		}
		//byteAddress = wordAddress << 2;
		unsigned byteCount = 0, wordValue = 0x00000000u;
		for(unsigned n = 0; n < datas[k].datas.size(); n++) {
			// datas[k].datas[n].first -> Bytes 1 2 4
			// datas[k].datas[n].second -> data
			// 8 && 16 && 32bits 1 2 4Bytes
			unsigned curByte = datas[k].datas[n].first;
			unsigned curData = datas[k].datas[n].second;
			if(byteCount + curByte <= 4) {
				wordValue = wordValue + (curData << byteCount * 8); // ignore signed negative number!
				byteCount += curByte;
				if(byteCount == 4) {
					out << hex << uppercase << internal << setw(8) << setfill('0') << wordValue;
					out << ','
						<< (((wordAddress + 1) % 8 == 0) ? '\n' : ' '); // flush();
					wordAddress++;
					byteCount = 0; wordValue = 0;
				}
			} else {
				unsigned low = curData & ((0xffffffffu >> (32 - (4 - byteCount) * 8)));
				wordValue = wordValue + (low << byteCount * 8);
				out << hex << uppercase << internal << setw(8) << setfill('0') << wordValue;
				out << ','
					<< (((wordAddress + 1) % 8 == 0) ? '\n' : ' '); // flush();
				wordAddress++;
				wordValue = curData >> (4 - byteCount) * 8;
				byteCount = curByte - (4 - byteCount);
			}
		}
		if(wordValue > 0u) {
			out << hex << uppercase << internal << setw(8) << setfill('0') << wordValue;
			out << ','
				<< (((wordAddress + 1) % 8 == 0) ? '\n' : ' '); // flush();
			wordAddress++;
		}
	}
	out.close();

    ifstream in(outFile);
    in >> noskipws;
    istream_iterator<char> isit(in), isit_end;
    string text(isit, isit_end);
    in.close();
    strip(text);
    text.erase(text.length() - 1);
    text.back() = ';';
    out.open(outFile);
    out << text;
    out.close();

}

void CodeParse::convertToBin(const string& outFile)
{
    //string::size_type dot = fileName.rfind('.');
    //string outFile(fileName);
    //outFile.replace(dot + 1, fileName.size() - 1 - dot, "bin");

	ofstream out(outFile.c_str(), ios::binary);
	unsigned i = 0;
	unsigned dummy = 0u;
    for(; i < (baseAddr >> 2); i++) {
		out.write(reinterpret_cast<char *>(&dummy), 4); // 32bits
	}

	for(unsigned j = 0; j < instructions.size(); j++) {
		if(instructions[j].first == INST) {
			string op;
			istringstream is(instructions[j].second);
			is >> op;
			if(typeMap[op].first == R) {
				typeR(out, instructions[j].second, BINARY);
			} else if(typeMap[op].first == I) {
				typeI(out, instructions[j].second, BINARY, i);
			} else if(typeMap[op].first == J) {
				typeJ(out, instructions[j].second, BINARY);
			}
		} else {
			unsigned data = static_cast<unsigned>(stoul(instructions[j].second, 0, 16));
			out.write(reinterpret_cast<char *>(&data), 4);
		}
		i++;
	}
	//unsigned byteAddress = baseAddr + instructions.size() * 4; // 4 multiple
	//unsigned wordAddress = byteAddress >> 2;
	unsigned wordAddress = i;
	for(unsigned k = 0; k < datas.size(); k++) {
		unsigned curField = datas[k].dataAddr;
        for(; wordAddress < (curField >> 2); wordAddress++) {
			out.write(reinterpret_cast<char *>(&dummy), 4);
		}
		//byteAddress = wordAddress << 2;
		unsigned byteCount = 0, wordValue = 0x00000000;
		for(unsigned n = 0; n < datas[k].datas.size(); n++) {
			// datas[k].datas[n].first -> Bytes 1 2 4
			// datas[k].datas[n].second -> data
			// 8 && 16 && 32bits 1 2 4Bytes
			unsigned curByte = datas[k].datas[n].first;
			unsigned curData = datas[k].datas[n].second;
			if(byteCount + curByte <= 4) {
				wordValue = wordValue + (curData << byteCount * 8); // ignore signed negative number!
				byteCount += curByte;
				if(byteCount == 4) {
					out.write(reinterpret_cast<char *>(&dummy), 4);
					wordAddress++;
					byteCount = 0; wordValue = 0;
				}
			} else {
				unsigned low = curData & ((0xffffffffu >> (32 - (4 - byteCount) * 8)));
				wordValue = wordValue + (low << byteCount * 8);
				out.write(reinterpret_cast<char *>(&dummy), 4);
				wordAddress++;
				wordValue = curData >> (4 - byteCount) * 8;
				byteCount = curByte - (4 - byteCount);
			}
		}
		if(wordValue > 0u) {
			out.write(reinterpret_cast<char *>(&dummy), 4);
			wordAddress++;
		}
	}
	out.close();
}
