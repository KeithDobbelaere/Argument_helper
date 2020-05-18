#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "Argument_helper.h"

#include <iostream>

std::string input_filename, output_filename;
int iv, oiv, niv;

int main(int argc, const char* argv[]) {
	dsr::Argument_helper ah;
	ah.new_string("input_filename.type", "The name of the input file",
		input_filename);
	ah.new_string("output_filename.type", "The name of the output file",
		output_filename);
	ah.new_int("count", "Some integer", iv);
	ah.new_optional_int("opt_count", "Some optional integer", oiv);
	ah.new_named_int('i', "integer", "named_int", "Some named integer", niv);
	ARGUMENT_HELPER_BASICS(ah);
	ah.set_description("A program");
	ah.set_author("Daniel Russel, drussel@graphics.stanford.edu");

	ah.process(argc, argv);

	//test(argv);
	ah.write_values(std::cout);

	return 0;
}