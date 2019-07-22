#include "RandomInput.h"

std::string RandomInput::fileAddress_ = "";

int RandomInput::clearInput(std::string& input)
{
	bool onlyspaces = true;

	size_t commentpos = input.find("//"); // // represent comment

	// find and erase comments from any line
	if (commentpos != std::string::npos) {
		input.erase(commentpos, input.length());
	}

	// ignores empty lines
	for (std::string::const_iterator i = input.begin(); i != input.end(); ++i)
	{
		if (*i != ' ' && *i != '\t')
		{
			onlyspaces = false;
			break;
		}
	}

	if (input != "" && !onlyspaces) {
		return 1; // ok
	}
	else if (input != "" || !onlyspaces) {
		return 0; // error
	}
	else {
		return 2; // comment or empty line
	}
}

bool RandomInput::readList()
{
	std::ifstream in;

	std::string input;
	// file addresses kept until one is randomized
	std::vector<std::string> addresses;
	
	in.open("randlist.txt");
	unsigned nFiles = 0;
	int iterator = 1;
	if (!in) // if reading fails
	{
		in.close();
		
		std::ofstream out("randlist.txt");

		out << "// This file contains the addresses the randomizer should pick from."
			 "\n// Path should look like: input/2019.07.17/"
			 "\n// Place each address in a new line!"
			 "\n// WARNING: The position of \"/\" characters is very important!"
		     "\n\n// Program and filesystem made by Szoke Andras-Lorand ;)";
		
		return false;
	}
	else while (std::getline(in, input))
	{
		// clear input and check against ignore conditions
		int respMessage = clearInput(input);

		if (respMessage == 1) // if response == good proceed
		{
			addresses.push_back(input);
			nFiles++;
		}
		else if (respMessage == 0) { // if response == error, warn
			NGin::Logger::log("randlist.txt line: " + std::to_string(iterator) + " could not be read.",
							  NGin::Logger::Severity::Warning);
		}

		iterator++;
	}
	in.close();
	
	if (nFiles != 0) {
		srand(static_cast<unsigned>(time(NULL))); // reseed
		fileAddress_ = addresses[unsigned(rand() % nFiles)];
	}
	else {
		return false;
	}
	
	return true;
}