#pragma once
#include <fstream>
#include <random>

#include "NGin/Base.h"

class RandomInput {
public:
	static bool readList();

	static std::string getAddress() { return fileAddress_; }
private:

	static int clearInput(std::string& input);

	static std::string fileAddress_;
};