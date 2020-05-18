/********************************************************************
 Argument Helper

 Refactored/Updated by
 Keith Dobbelaere  keith77mn77@gmail.com
 5/17/2020
 *******************************************************************/
#include "Argument_helper.h"


int main(int argc, const char* argv[]) {
	// Variables are passed by reference, and initialized after they're passed into Argument_helper.
	unsigned int threadId;
	int priority;
	int delay;
	double d;
	float f;
	std::string file_name;
	std::string another_file_name;
	bool b;
	bool B;
	std::vector<std::string> strings;
	{
		dsr::Argument_helper ah;
		ah.set_name_long_form("ExampleSoft (R) Example");
		ah.set_version("1.0.0.0");
		ah.set_author("Keith Dobbelaere");
		ah.set_description("This is an example application to demonstrate how to integrate the Argument_helper class "
		"into a program in order to read command arguments and process them into variables for use by the program.");
		ah.set_build_date(__DATE__);

		// These are non-optional values which will be read in the order they're defined.
		ah.new_param("threadId", "Thread ID of some target process to control.", threadId);
		ah.new_param("priority", "Sets new priority value of the targeted process.", priority);
		// These are optional parameters.
		ah.new_optional_param("file_name", "This particular argument is optional, even without a flag associated with it. "
			"The parser won't trigger an error if this third value is disregarded. What follows is just filler to show how "
			"lengthy descriptions will wrap around in the usage screen, which is accessible via the /? flag, as usual. "
			"The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog. "
			"The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog. "
			"The quick brown fox jumped over the lazy dog. The quick brown fox jumped over the lazy dog. ", file_name);
		ah.new_named_param("t", "timer", "Value for an imaginary timer to delay in milliseconds.", delay);
		ah.new_named_param("d", "double_value", "This number is a double.", d);
		ah.new_named_param("f", "value", "This number is a float.", f);
		ah.new_named_param("s", "string", "This is a string to demonstrate that strings containing spaces can be entered if "
			"they're \"surrounded by quotes.\"", another_file_name);
		ah.new_flag("B", "This flag sets a bool for use later in the program.", b);
		ah.new_flag("b", "And here's another one.", B);
		ah.new_named_string_vector("vector", "strings ...", "This is a switch which accepts a list of arguments, separated "
			"by white-space. Not sure what you'd need this for, but here it is.", strings);
		// This is text to be displayed in the usage screen, accessible with the /? switch.
		ah.set_example_text("\n>ARGUMENT_HELPER  "
			"543 21 /t 5000 /d 1.79769e+308 /f 3.14159 /s \"one string\" /B\n/vector 1 2 3 four five\n\n"
			"    Results:\n"
			"\tthreadId : 543\n"
			"\tpriority : 21\n"
			"\t[file_name] :\n"
			"\t[/B] : true\n"
			"\t[/b] : false\n"
			"\t[/d double_value] : 1.79769e+308\n"
			"\t[/f value] : 3.14159\n"
			"\t[/s string] : one string\n"
			"\t[/t timer] : 5000\n"
			"\t[/vector strings ...] : 1 2 3 four five");

		ah.process(argc, argv);

		ah.write_values(std::cout);
	}
	return 0;
}