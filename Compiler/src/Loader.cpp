#include "../includes/Loader.hpp"

// uint16_t Loader::load(const std::string& path, Memory& mem, std::vector<int32_t>& constPool)
// {
// 	std::ifstream f(path);
// 	if (!f.is_open())
// 		throw std::runtime_error("Loader: cannot open '" + path + "'");

// 	uint16_t codeAddr = 0;
// 	std::string line;

// 	while (std::getline(f, line)) {
// 		if (line.empty()) continue;
// 		Instruction inst = parseLine(line, constPool);
// 		if (codeAddr > CODE_LIMIT - CODE_BASE)
// 			throw std::runtime_error("Loader: code section overflow");
// 		mem.writeCode(codeAddr++, inst);
// 	}

// 	mem.writeCode(codeAddr++, halt);

// 	return codeAddr;
// }


// Instruction Loader::parseLine(const std::string& raw, std::vector<int32_t>& constPool)
// {
// 	std::istringstream ss(raw);
// 	std::string cmd;
// 	ss >> cmd;


// 	if (cmd == "MOV") {
// 		std::string dst, src;
// 		ss >> dst >> src;
// 		if (!dst.empty() && dst.back() == ',')
// 			dst.pop_back();

// 		if (dst[0] == 'r') {
// 			uint8_t D = static_cast<uint8_t>(std::stoi(dst.substr(1)));
// 			if (src[0] == '#')
// 			{
// 				int32_t val = std::stoi(src.substr(1));
// 				uint8_t idx = static_cast<uint8_t>(constPool.size());
// 				constPool.push_back(val);
// 				inst = { static_cast<uint8_t>(OpCode::LOAD_NUM), D, idx, 0 };
// 			}
// 			else if (src[0] == '[')
// 			{
// 				uint8_t addr = static_cast<uint8_t>(std::stoi(src.substr(1, src.size() - 2)));
// 				inst = { static_cast<uint8_t>(OpCode::LOAD_VAR), D, addr, 0 };
// 			}
// 		} else if (dst[0] == '[') {
// 			std::string addrStr = dst.substr(1);
// 			if (!addrStr.empty() && addrStr.back() == ']') addrStr.pop_back();
// 			uint8_t addr = static_cast<uint8_t>(std::stoi(addrStr));
// 			uint8_t S    = static_cast<uint8_t>(std::stoi(src.substr(1)));
// 			inst = { static_cast<uint8_t>(OpCode::STORE_VAR), S, addr, 0 };
// 		}
// 		return inst;
// 	}

// 	OpCode arith = OpCode::HALT;
// 	if      (cmd == "ADD") arith = OpCode::ADD;
// 	else if (cmd == "SUB") arith = OpCode::SUB;
// 	else if (cmd == "MUL") arith = OpCode::MUL;
// 	else if (cmd == "DIV") arith = OpCode::DIV;

// 	if (arith != OpCode::HALT) {
// 		std::string rD, rL, rR;
// 		ss >> rD >> rL >> rR;
// 		if (!rD.empty() && rD.back() == ',') rD.pop_back();
// 		if (!rL.empty() && rL.back() == ',') rL.pop_back();
// 		uint8_t D = static_cast<uint8_t>(std::stoi(rD.substr(1)));
// 		uint8_t L = static_cast<uint8_t>(std::stoi(rL.substr(1)));
// 		uint8_t R = static_cast<uint8_t>(std::stoi(rR.substr(1)));
// 		return { static_cast<uint8_t>(arith), D, L, R };
// 	}

// 	if (cmd == "CMP") {
// 		std::string rL, rR;
// 		ss >> rL >> rR;
// 		if (!rL.empty() && rL.back() == ',') rL.pop_back();
// 		uint8_t L = static_cast<uint8_t>(std::stoi(rL.substr(1)));
// 		uint8_t R = static_cast<uint8_t>(std::stoi(rR.substr(1)));
// 		return { static_cast<uint8_t>(OpCode::CMP), 0, L, R };
// 	}

// 	OpCode jop = OpCode::HALT;
// 	if      (cmd == "JMP") jop = OpCode::JMP;
// 	else if (cmd == "JE")  jop = OpCode::JE;
// 	else if (cmd == "JNE") jop = OpCode::JNE;
// 	else if (cmd == "JG")  jop = OpCode::JG;
// 	else if (cmd == "JL")  jop = OpCode::JL;
// 	else if (cmd == "JGE") jop = OpCode::JGE;
// 	else if (cmd == "JLE") jop = OpCode::JLE;

// 	if (jop != OpCode::HALT) {
// 		std::string addr;
// 		ss >> addr;
// 		uint8_t target = static_cast<uint8_t>(std::stoi(addr));
// 		return { static_cast<uint8_t>(jop), target, 0, 0 };
// 	}

// 	if (cmd == "CALL") {
// 		std::string addr;
// 		ss >> addr;
// 		uint8_t target = static_cast<uint8_t>(std::stoi(addr));
// 		return { static_cast<uint8_t>(OpCode::CALL), target, 0, 0 };
// 	}

// 	if (cmd == "RET")
// 		return { static_cast<uint8_t>(OpCode::RET), 0, 0, 0 };

// 	return { static_cast<uint8_t>(OpCode::HALT), 0, 0, 0 };
// }
