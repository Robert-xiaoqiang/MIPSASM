#include "CoeBinParse.h"
#include <iterator>
#include <iomanip>
#include <regex>

using namespace std;

map<unsigned, string> CoeBinParse::regMap = {
	{ 0, "$zero" },
	{ 1, "$at" },
	{ 2, "$v0" },
	{ 3, "$v1" },
	{ 4, "$a0" },
	{ 5, "$a1" },
	{ 6, "$a2" },
	{ 7, "$a3" },
	{ 8, "$t0" },
	{ 9, "$t1" },
	{ 10, "$t2" },
	{ 11, "$t3" },
	{ 12, "$t4" },
	{ 13, "$t5" },
	{ 14, "$t6" },
	{ 15, "$t7" },
	{ 16, "$s0" },
	{ 17, "$s1" },
	{ 18, "$s2" },
	{ 19, "$s3" },
	{ 20, "$s4" },
	{ 21, "$s5" },
	{ 22, "$s6" },
	{ 23, "$s7" },
	{ 24, "$t8" },
	{ 25, "$t9" },
	{ 26, "$k0" },
	{ 27, "$k1" },
	{ 28, "$gp" },
	{ 29, "$sp" },
	{ 30, "$fp" },
	{ 31, "$ra" },
};
map<pair<unsigned, unsigned>, string> CoeBinParse::instMap {
	{ { 0x0, 0x0 }, "nop" },
	{ { 0x0, 0x20 }, "add" },
	{ { 0x0, 0x21 }, "addu" },
	{ { 0x0, 0x24 }, "and" },
	{ { 0x1c, 0x21 }, "clo" },
	{ { 0x1c, 0x20 }, "clz" },
	{ { 0x0, 0x1a }, "div" },
	{ { 0x0, 0x1b }, "divu" },
	{ { 0x1c, 0x2 }, "mul" },
	{ { 0x0, 0x18 }, "mult" },
	{ { 0x0, 0x19 }, "multu" },
	{ { 0x1c, 0x0 }, "madd" },
	{ { 0x1c, 0x1 }, "maddu" },
	{ { 0x1c, 0x4 }, "msub" },
	{ { 0x0, 0x27 }, "nor" },
	{ { 0x0, 0x25 }, "or" },
	{ { 0x0, 0x0 }, "sll" },
	{ { 0x0, 0x4 }, "sllv" },
	{ { 0x0, 0x3 }, "sra" },
	{ { 0x0, 0x7 }, "srav" },
	{ { 0x0, 0x2 }, "srl" },
	{ { 0x0, 0x6 }, "srlv" },
	{ { 0x0, 0x22 }, "sub" },
	{ { 0x0, 0x23 }, "subu" },
	{ { 0x0, 0x26 }, "xor" },
	{ { 0x0, 0x2a }, "slt" },
	{ { 0x0, 0x2b }, "sltu" },
	{ { 0x0, 0x10 }, "mfhi" },
	{ { 0x0, 0x12 }, "mflo" },
	{ { 0x0, 0x11 }, "mthi" },
	{ { 0x0, 0x13 }, "mtlo" },
	{ { 0x10, 0x0 }, "mfc0" },
	{ { 0x11, 0x0 }, "mfc1" },
	{ { 0x10, 0x0 }, "mtc0" },
	{ { 0x11, 0x0 }, "mtc1" },
	{ { 0x0, 0x9 }, "jalr" },
	{ { 0x0, 0x8 }, "jr" },
	{ { 0x8, 0x0 }, "addi" },
	{ { 0x9, 0x0 }, "addiu" },
	{ { 0xc, 0x0 }, "andi" },
	{ { 0xd, 0x0 }, "ori" },
	{ { 0xf, 0x0 }, "lui" },
	{ { 0xe, 0x0 }, "xori" },
	{ { 0xa, 0x0 }, "slti" },
	{ { 0xb, 0x0 }, "sltiu" },
	{ { 0x4, 0x0 }, "beq" },
	{ { 0x1, 0x0 }, "bgez" },
	{ { 0x1, 0x0 }, "bgezal" },
	{ { 0x7, 0x0 }, "bgtz" },
	{ { 0x6, 0x0 }, "blez" },
	{ { 0x1, 0x0 }, "bltzal" },
	{ { 0x1, 0x0 }, "bltz" },
	{ { 0x5, 0x0 }, "bne" },
	{ { 0x20, 0x0 }, "lb" },
	{ { 0x24, 0x0 }, "lbu" },
	{ { 0x21, 0x0 }, "lh" },
	{ { 0x25, 0x0 }, "lhu" },
	{ { 0x23, 0x0 }, "lw" },
	{ { 0x28, 0x0 }, "sb" },
	{ { 0x29, 0x0 }, "sh" },
	{ { 0x2b, 0x0 }, "sw" },
	{ { 0x2, 0x0 }, "j" },
	{ { 0x3, 0x0 }, "jal" },
};
map<string, pair<Type, TypeLow>> CoeBinParse::typeMap = {
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
	{ "lui",{ I, IRTRSIM } },{ "LUI",{ I, IRTRSIM } },
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
};
// generate by bne/beq or j

CoeBinParse::CoeBinParse(const string& fileName):
	fileName(fileName)
{

}

CoeBinParse::~CoeBinParse()
{

}

void CoeBinParse::strip(string& line) const
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

// just 16base;
// only pattial inst
// no data definition / data segment
void CoeBinParse::format(Source s)
{
	ifstream in;
    if(s == COE) {
		in.open(fileName.c_str()); // text method
		string coePattern = "\\s*(,|;)\\s*";
		regex coe_re(coePattern);

		in >> noskipws;
		istream_iterator<char> isite(in), isite_end;
		string text(isite, isite_end);
		auto semicolon = text.find(';');
		text.erase(0, semicolon + 1);
		strip(text);
		auto equal = text.find('=');
		text.erase(0, equal + 1);
		strip(text);
		sregex_iterator site(text.begin(), text.end(), coe_re), site_end;
		for(; site != site_end; site++) {
			insts.push_back(static_cast<unsigned>(stoul(site->prefix(), 0, 16)));
		}
	} else {
		in.open(fileName.c_str(), ios::binary);
		while(in) {
			unsigned inst; 
			in.read(reinterpret_cast<char *>(&inst), 4); // 32bits;
			if(!in) break;
			insts.push_back(inst);
		}
	}
	
	in.close();
}

void CoeBinParse::convertToAsm(const string& outFile)
{
	for(unsigned i = 0; i < insts.size(); i++) {
		unsigned op, funct = 0u;
        op = insts[i] & (0xffffffffu - 0x3ffffffu);
		op >>= 26;
        if(op == 0u || op == 0x1cu || op == 0x10u || op == 0x11u) {
			funct = insts[i] & 0x3fu;
        }
        string inst = instMap[make_pair(op, funct)];
		switch(typeMap[inst].first) {
		case R:
			typeR(insts[i]);
			break;
		case I:
			typeI(insts[i], i); // wordaddress
			break;
		case J:
			typeJ(insts[i], i); // pc4[31:28]; is necessary! little program???
			break;
		default:
			break;
		}
	}
    output(outFile);
}

void CoeBinParse::typeR(unsigned inst)
{
	unsigned op, rs, rt, rd, shamt, funct;
    op = (inst & (0xffffffffu - 0x3ffffffu)) >> 26;
    rs = (inst & (0x3ffffffu - 0x1fffffu)) >> 21;
    rt = (inst & (0x1fffffu - 0xffffu)) >> 16;
    rd = (inst & (0xffffu - 0x7ffu)) >> 11;
    shamt = (inst & (0x7ffu - 0x3fu)) >> 6;
	funct = inst & 0x3fu;

    string code = instMap[make_pair(op, funct)];
	switch(typeMap[code].second) {
	case RRD:
		codes.push_back(code + "\t" + regMap[rd] + ";");
		break;
	case RRS:
		codes.push_back(code + "\t" + regMap[rs] + ";");
		break;
	case RRDRS:
		codes.push_back(code + "\t" + regMap[rd] + ", " + regMap[rs] + ";");
		break;
	case RRDRT:
		codes.push_back(code + "\t" + regMap[rd] + ", " + regMap[rt] + ";");
		break;
	case RRTRD:
		codes.push_back(code + "\t" + regMap[rt] + ", " + regMap[rd] + ";");
		break;
	case RRTRS:
		codes.push_back(code + "\t" + regMap[rt] + ", " + regMap[rs] + ";");
		break;
	case RRSRT:
		codes.push_back(code + "\t" + regMap[rs] + ", " + regMap[rt] + ";");
		break;
	case RRSRD:
		codes.push_back(code + "\t" + regMap[rs] + ", " + regMap[rd] + ";");
		break;
	case RRDRSRT:
	{
		if(code == "nop")
			codes.push_back(code + ";");
		else
			codes.push_back(code + "\t" + regMap[rd] + ", " + regMap[rs] + ", " +
				            regMap[rt] + ";");
		break;
	}
	case RRDRTRS:
		codes.push_back(code + "\t" + regMap[rd] + ", " + regMap[rt] + ", " +
			            regMap[rs] + ";");
		break;
	case RRDRTSH:
	{
		ostringstream os;
		os << code << "\t" << regMap[rd] << ", " << regMap[rt] << ", " << shamt << ";";
		codes.push_back(os.str());
		break;
	}
		
	default:
		break;
	}
}

void CoeBinParse::typeI(unsigned inst, unsigned curPos)
{
    unsigned op, rs, rt;
    int imm;
    op = (inst & (0xffffffffu - 0x3ffffffu)) >> 26;
    rs = (inst & (0x3ffffffu - 0x1fffffu)) >> 21;
    rt = (inst & (0x1fffffu - 0xffffu)) >> 16;
    imm = static_cast<int>(inst & 0xffffu);

    string code(instMap[make_pair(op, 0u)]);
    switch(typeMap[code].second) {
    case IRTRSIM:
        codes.push_back(code + "\t" + regMap[rt] + ", " + regMap[rs] + ", " +
                        to_string(imm) + ";");
        break;
    case IRSRTLA:
    {
        unsigned address = static_cast<unsigned>((int)curPos + 1 + imm);
                string curLabel;
        auto ite = labelMap.find(address);
        if(ite != labelMap.end()) {
            curLabel = ite->second;
        } else {
            curLabel = "label_" + to_string(labelMap.size());
            labelMap.insert(make_pair(static_cast<unsigned>((int)curPos + 1 + imm),
                curLabel));
        }
        codes.push_back(code + "\t" + regMap[rs] + ", " + regMap[rt] + ", " +
                        curLabel + ";");
        break;
    }
    case IRTIMRS:
    {
        ostringstream os;
        os << code << "\t" << regMap[rt] << ", " << imm << "(" << regMap[rs] << ");";
        codes.push_back(os.str());
        break;
    }
    case IRSLA:
    {
        /*if(code == "bgez" || code == "BGEZ") {
            rt = 1u;
        } else if(code == "bgezal" || code == "BGEZAL") {
            rt = 11u;
        } else if(code == "bgtz" || code == "blez" || code == "BGTZ" || code == "BLEZ") {
            rt = 0u;
        } else if(code == "bltz" || code == "BLTZ") {
            rt = 0u;
        } else if(code == "bltzal" || code == "BLTZAL") {
            rt = 10u;
        }*/
        unsigned address = static_cast<unsigned>((int)curPos + 1 + imm);
        string curLabel;
        auto ite = labelMap.find(address);
        if(ite != labelMap.end()) {
            curLabel = ite->second;
        } else {
            curLabel = "label_" + to_string(labelMap.size());
            labelMap.insert(make_pair(address, curLabel));
        }
        codes.push_back(code + "\t" + regMap[rs] + ", " + curLabel + ";");
        break;
    }
    default:
        break;
    }
}

void CoeBinParse::typeJ(unsigned inst, unsigned curPos)
{
    unsigned op, pseudoDirect;
    op = (inst & (0xffffffffu - 0x3ffffffu)) >> 26;
    pseudoDirect = inst & 0x3ffffffu;
    unsigned direct = pseudoDirect + ((0xfc000000u & curPos) << 26);
    string code = instMap[make_pair(op, 0u)];

    auto ite = labelMap.find(direct);
    string curLabel;
    if(ite != labelMap.end()) {
        curLabel = ite->second;
    } else {
        curLabel = "label_" + to_string(labelMap.size());
        labelMap.insert(make_pair(direct, curLabel));
    }
    codes.push_back(code + "\t" + curLabel + ";");

}

void CoeBinParse::output(const string& outFile) const
{
    //string::size_type dot = fileName.rfind('.');
    //string outFile(fileName);
    //outFile.replace(dot + 1, fileName.size() - 1 - dot, "asm");

    ofstream out(outFile.c_str());
	for(unsigned i = 0; i < codes.size(); i++) {
		auto label = labelMap.find(i);
		if(label != labelMap.end()) {
			out << label->second << ":" << endl;
		}
		out << codes[i] << endl;
	}
	out.close();
}
