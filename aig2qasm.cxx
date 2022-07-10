#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <filesystem>
#include <fmt/format.h>

#include "caterpillar.hpp"
#include "mockturtle/mockturtle.hpp"
#include "lorina/lorina.hpp"
#include "tweedledum/tweedledum.hpp"
#include "cxxopts.hpp"

namespace fs = std::filesystem;

//https://stackoverflow.com/questions/8581832/converting-a-vectorint-to-string
template<typename T>
std::string vec_to_str(std::vector<T> &vec){

	std::ostringstream oss;

	if (!vec.empty()) {
		std::copy(vec.begin(), vec.end()-1, 
			std::ostream_iterator<T>(oss, ", "));

		oss << vec.back();
	}

	return oss.str();
}

int main(int argc, char* argv[])
{

	using namespace caterpillar;
	using namespace mockturtle;
	using namespace tweedledum;

	// Input Options ///////////////////////////////////////////////////////////////////////////////

	cxxopts::Options options("aig2qasm", "Convert AIG to QASM circuit.");

	options.add_options()
		("h,help", "Help me!")
		("p,pebble", "Use pebbling strategy for network synthesis", 
			cxxopts::value<bool>()->default_value("true") )
		("b,bestfit", "Use best-fit strategy for network synthesis",
			cxxopts::value<bool>()->default_value("false") )
		("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
		("L,limit", "Pebble limit.", cxxopts::value<int>()->default_value("100"))
		("O,to-stdout", "Output to standard output, instead of file.",
			cxxopts::value<bool>()->default_value("false"))
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

	if (fs::path(file).extension() != fs::path(".aig")) {
		std::cout << "Must supply an AIG file as input!\nGot: " << file << std::endl;
		exit(1);
	}		

	bool pebble 		= result["pebble"].as<bool>();
	int pebble_limit 	= result["limit"].as<int>();

	bool bestfit 		= result["bestfit"].as<bool>();

	bool verbose 		= result["verbose"].as<bool>();
	bool stdout         = result["to-stdout"].as<bool>();

	if (bestfit){
		pebble = false; //dumb way to do this
	}


	//Network Synthesis ////////////////////////////////////////////////////////////////////////////

	
	//read in AIGER file
	aig_network aig;
	auto const retcode = lorina::read_aiger(file, aiger_reader(aig));
	if (retcode != lorina::return_code::success)
		return 0;

	netlist<stg_gate> circ;
	logic_network_synthesis_stats stats;

	if (pebble) {

		pebbling_mapping_strategy_params ps;
		ps.pebble_limit = pebble_limit;
	
	#ifdef USE_Z3	
		pebbling_mapping_strategy< aig_network, z3_pebble_solver<aig_network> > strategy( ps );
	#else
		pebbling_mapping_strategy< aig_network, bsat_pebble_solver<aig_network> > strategy( ps );
	#endif

		logic_network_synthesis(circ, aig, strategy, stg_from_pprm(), {}, &stats);

	} else if (bestfit) {

		best_fit_mapping_strategy<aig_network> strategy;
		logic_network_synthesis(circ, aig, strategy, stg_from_pprm(), {}, &stats);

	} else {

		std::cout << "Did nothing!" << std::endl;
		return 0;

	}
	
	std::string output_idx = vec_to_str(stats.o_indexes);
	std::string input_idx  = vec_to_str(stats.i_indexes);
	std::string elapsed_time = fmt::format( "//Total time = {:>5.4f} secs", to_seconds(stats.time_total));

	if (verbose){

		std::cout << "//Network synthesis statistics:\n" << std::endl;
		std::cout << elapsed_time << std::endl;
		std::cout << "//Required ancillae: " << stats.required_ancillae << std::endl;
		std::cout << "//Input indices: \n\t" << input_idx << std::endl;
		std::cout << "//Output indices: \n\t" << output_idx << std::endl;
		
	}	
	
	std::ostringstream qasm_oss;
	write_qasm(circ, qasm_oss);

	if (stdout) {

		std::cout << qasm_oss.str() << std::endl;
	} else {

		fs::path ofile = fs::path(file).replace_extension(fs::path(".qasm"));

		//add some info for downstream tools
		std::ofstream ofs (ofile);
		if (ofs.is_open())
		{
			ofs << "//Synthesized with aig2qasm from " << file << std::endl;
			ofs << elapsed_time << std::endl;
			ofs << "//Required ancillae: " << stats.required_ancillae << std::endl;
			ofs << "//Input indices: " << input_idx << std::endl;
			ofs << "//Output indices: " << output_idx << std::endl;
			ofs << qasm_oss.str();

		} else {
			std::cout << "Could not open output file!" << std::endl;
			exit(1);
		}

	}
	
	return 0;
}