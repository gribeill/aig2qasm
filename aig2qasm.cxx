#include <iostream>
#include <string>
#include <filesystem>

//#include "caterpillar.hpp"
//#include "mockturtle/mockturtle.hpp"
//#include "lorina/lorina.hpp"
//#include "tweedledum/tweedledum.hpp"
#include "cxxopts.hpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
	cxxopts::Options options("aig2qasm", "Convert AIG to QASM circuit.");

	options.add_options()
		("h,help", "Help me!")
		("file", "The aig file to convert.", cxxopts::value<std::string>());
	options.parse_positional({"file"});

	auto result = options.parse(argc, argv);

	if (result.count("help")) {
		std::cout << options.help() << std::endl;
		exit(0);
	}		

	std::string file;
	if (result.count("file")) {
		file = result["file"].as<std::string>();
	} else {
		std::cout << "Must supply an input file!" << std::endl;
		exit(1);
	}

	if (fs::extension(file) != fs::path(".aig")) {
		std::cout << "Must supply an AIG file as input!\nGot: " << file << std::endl;
		exit(1);
	}

	std::cout << "File is: " << file << std::endl;
	std::cout << "Well, that's a start... " << std::endl;
	return 0;
}