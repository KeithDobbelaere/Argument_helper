/*
* Argument Helper
*
* Daniel Russel drussel@alumni.princeton.edu
* Stanford University
*
*
* This software is not subject to copyright protection and is in the
* public domain. Neither Stanford nor the author assume any
* responsibility whatsoever for its use by other parties, and makes no
* guarantees, expressed or implied, about its quality, reliability, or
* any other characteristic.
*
*/

/********************************************************************
 Refactored/Updated by
 Keith Dobbelaere  keith77mn77@gmail.com
 5/17/2020
 *******************************************************************/
#ifndef _DSR_ARGS_H_
#define _DSR_ARGS_H_
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>


namespace dsr {

	class Argument_helper {
	private:
		class Argument_target;
		template<typename T>
		class Specialized_target;
		class FlagTarget;
		class StringVectorTarget;

	public:
		Argument_helper() = default;

		Argument_helper(const Argument_helper&) = delete;
		Argument_helper& operator=(Argument_helper const&) = delete;

		~Argument_helper();

		void new_flag(const char* key, const char* description, bool& dest);
		template<typename T>
		void new_param(const char* arg_description, const char* description, T& dest);
		template<typename T>
		void new_named_param(const char* key, const char* value_name, const char* description, T& dest);
		template <typename T>
		void new_optional_param(const char* value_name, const char* description, T& dest);

		void new_named_args_vector(const char* key, const char* value_name, const char* description,
			std::vector<std::string>& dest);

		void set_extra_args_vector(const char* arg_description, const char* description, std::vector<std::string>& dest);
		void set_author(const char* author);
		void set_description(const char* descr);
		void set_version(const char* str);
		void set_name(const char* name);
		void set_name_long_form(const char* name);
		void set_company_name(const char* company);
		void set_build_date(const char* date);
		void set_example_text(const char* example_text);

		void process(int argc, const char** argv);
		void process(int argc, char** argv) {
			process(argc, const_cast<const char**>(argv));
		}
		void write_usage(std::ostream& out) const;
		void write_values(std::ostream& out) const;

	protected:
		std::map<std::string, Argument_target*> keys_;
		std::string author_;
		std::string name_;
		std::string name_long_form_;
		std::string company_name_;
		std::string description_;
		std::string date_;
		std::string example_text_;
		struct version {
			unsigned int major;
			unsigned int minor;
			unsigned int revision;
			unsigned int build;
		} version_ = { 0 };

		typedef std::vector<Argument_target*> UVect;
		UVect unnamed_arguments_;
		UVect optional_unnamed_arguments_;
		UVect all_arguments_;
		std::string extra_arguments_descr_;
		std::string extra_arguments_arg_descr_;
		std::vector<std::string>* extra_arguments_ = nullptr;

		void new_argument_target(Argument_target*);
		void handle_error() const;
		static void text_wrap(const char* input_string, std::ostream& out, size_t length, const char* indent_string = "");
	};

	class Argument_helper::Argument_target {
	public:
		std::string key;
		std::string description;
		std::string arg_description;
		bool is_optional;

		Argument_target(const std::string& k, const std::string& descr, const std::string& arg_descr) :
			key(k), description(descr), arg_description(arg_descr), is_optional(false) {}

		Argument_target(const std::string& descr, const std::string& arg_descr) :
			key(""), description(descr), arg_description(arg_descr), is_optional(false) {}

		virtual bool process(int&, const char**&) = 0;
		virtual void write_name(std::ostream& out) const;
		virtual void write_value(std::ostream& out) const = 0;
		virtual void write_usage(std::ostream& out) const;
		virtual ~Argument_target() {}
	};

	template <typename T>
	class Argument_helper::Specialized_target : public Argument_helper::Argument_target {
	public:
		T& val;
		Specialized_target(const char* arg_descr, const char* descr, T& b) :
			Argument_target("", descr, arg_descr), val(b) {
			val = { 0 };
		}
		Specialized_target(const char* k, const char* arg_descr, const char* descr, T& b) :
			Argument_target(k, descr, arg_descr), val(b) {
			val = { 0 };
		}

		virtual bool process(int& argc, const char**& argv) {
			if (argc == 0) {
				std::cerr << "Missing value for argument.\n";
				return false;
			}
			std::istringstream iss(argv[0]);
			if (!(iss >> val)) {
				std::cerr << "Invalid argument: " << argv[0] << '\n';
				return false;
			}
			else if (!iss.eof()) {
				std::cerr << "Trailing characters after argument: " << argv[0] << '\n';
				return false;
			}
			--argc;
			++argv;
			return true;
		}

		virtual void write_value(std::ostream& out) const { out << val; }
	};

	template <>
	class Argument_helper::Specialized_target<std::string> : public Argument_helper::Argument_target {
	public:
		std::string& val;
		Specialized_target(const char* arg_descr, const char* descr, std::string& b) :
			Argument_target("", descr, arg_descr), val(b) {}

		Specialized_target(const char* k, const char* arg_descr, const char* descr, std::string& b) :
			Argument_target(k, descr, arg_descr), val(b) {}

		virtual bool process(int& argc, const char**& argv) {
			if (argc == 0) {
				std::cerr << "Missing value for argument.\n";
				return false;
			}
			val = argv[0];
			--argc;
			++argv;
			return true;
		}

		virtual void write_value(std::ostream& out) const { out << val; }
	};

	template<typename T>
	inline void Argument_helper::new_param(const char* arg_description, const char* description, T& dest) {
		Argument_target* t = new Specialized_target<T>(arg_description, description, dest);
		unnamed_arguments_.push_back(t);
		all_arguments_.push_back(t);
	}

	template<typename T>
	inline void Argument_helper::new_named_param(const char* key, const char* value_name, const char* description, T& dest) {
		Argument_target* t = new Specialized_target<T>(key, value_name, description, dest);
		t->is_optional = true;
		new_argument_target(t);
	}

	template<typename T>
	inline void Argument_helper::new_optional_param(const char* value_name, const char* description, T& dest) {
		Argument_target* t = new Specialized_target<T>(value_name, description, dest);
		t->is_optional = true;
		optional_unnamed_arguments_.push_back(t);
	}
}
#endif
