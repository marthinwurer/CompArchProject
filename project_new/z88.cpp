#include <iostream>
#include <iomanip>

#include "connections.h"
#include "components.h"
#include "run_program.h"

int main(int argc, char *argv[]) {
	if(argc != 2) {
		std::cout << "Usage: " << argv[0] << " <path_to_object_file>" << std::endl;
		return 1;
	}

	try {
		connect_components();
		std::cout << std::hex;
		instruction_mem.load(argv[1]);
		data_mem.load(argv[1]);

		run_program();
//TODO: run program
	}
	catch(ArchLibError &ale) {
		std::cout << std::endl <<
			"Simulation Aborted - ArchLib Runtime Error Occured"
			<< std::endl << "Reason: " << ale.what() << std::endl;

		return 1;
	}

	return 0;
}
