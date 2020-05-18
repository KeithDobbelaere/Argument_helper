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
 Argument Helper

 Refactored/Updated by
 Keith Dobbelaere  keith77mn77@gmail.com
 5/17/2020
 *******************************************************************/
#include <cassert>

#include "Argument_helper.h"

namespace dsr {

	Argument_helper::~Argument_helper() {
		for (auto it = all_arguments_.begin(); it != all_arguments_.end(); ++it) {
			delete* it;
		}
	}


	void Argument_helper::Argument_target::write_name(std::ostream& out) const {
		bool has_key = !key.empty();
		bool has_arg_desc = !arg_description.empty();
		if (has_key && has_arg_desc) {
			out << "[/" << key << ' ' << arg_description << "] ";
		}
		else {
			if (has_key) out << "[/" << key << "] ";
			else if (is_optional)
				out << '[' << arg_description << "] ";
			else
				out << arg_description << ' ';
		}
	}


	void Argument_helper::Argument_target::write_usage(std::ostream& out) const {
		out << "\t";
		write_name(out);
		out << '\n';
		Argument_helper::text_wrap(description.c_str(), out, 50, "\t   ");
	}

	class Argument_helper::FlagTarget : public Argument_helper::Argument_target {
	public:
		bool& val;
		FlagTarget(const char* k, const char* descr, bool& b) :
			Argument_target(k, std::string(descr), std::string()), val(b) {
			val = false;
		}
		virtual bool process(int&, const char**&) {
			val ^= true;
			return true;
		}
		virtual void write_value(std::ostream& out) const { out << std::boolalpha << val; }
	};


	class Argument_helper::StringVectorTarget : public Argument_target {
	public:
		std::vector<std::string>& val;

		StringVectorTarget(const char* k, const char* arg_descr, const char* descr, std::vector<std::string>& b) :
			Argument_target(k, descr, arg_descr), val(b) {}

		virtual bool process(int& argc, const char**& argv) {
			while (argc > 0 && argv[0][0] != '/') {
				val.push_back(argv[0]);
				--argc;
				++argv;
			}
			return true;
		}
		virtual void write_value(std::ostream& out) const {
			for (size_t i = 0; i < val.size(); ++i) {
				out << val[i];
				if (i != val.size() - 1)
					out << ", ";
			}
		}
	};

	void Argument_helper::set_string_vector(const char* arg_description,
		const char* description, std::vector<std::string>& dest) {
		assert(extra_arguments_ == nullptr, "Error: More than one vector specified to store extra arguments.");
		extra_arguments_descr_ = description;
		extra_arguments_arg_descr_ = arg_description;
		extra_arguments_ = &dest;
	}

	void Argument_helper::set_author(const char* author) {
		author_ = author;
	}

	void Argument_helper::set_description(const char* descr) {
		description_ = descr;
	}

	void Argument_helper::set_name(const char* name) {
		name_ = name;
		for (auto p = name_.begin(); p != name_.end(); ++p)
			*p = toupper(*p);
	}

	void Argument_helper::set_name_long_form(const char* name) {
		name_long_form_ = name;
	}

	void Argument_helper::set_company_name(const char* company) {
		company_name_ = company;
	}

	void Argument_helper::set_version(const char* s) {
		if (sscanf_s(s, "%u.%u.%u.%u", &version_.major, &version_.minor, &version_.revision, &version_.build) != 4) {
			std::cerr << "\nError: Version string improperly formatted. Should be four numbers,\n";
			std::cerr << "separated by '.' Example: \"0.1.023.1020\"\n";
		}
	}

	void  Argument_helper::set_build_date(const char* date) {
		date_ = date;
	}

	void Argument_helper::set_example_text(const char* example_text) {
		example_text_ = example_text;
	}

	void Argument_helper::new_argument_target(Argument_target* t) {
		if (t && !t->key.empty()) {
			if (keys_.find(t->key) != keys_.end()) {
				std::cerr << "\nError: Two arguments are defined with the same character key, namely\n";
				keys_[t->key]->write_usage(std::cerr);
				std::cerr << " and \n";
				t->write_usage(std::cerr);
				std::cerr << '\n';
			}
			keys_[t->key] = t;
		}
		all_arguments_.push_back(t);
	}

	void Argument_helper::new_flag(const char* key, const char* description, bool& dest) {
		Argument_target* t = new FlagTarget(key, description, dest);
		t->is_optional = true;
		new_argument_target(t);
	};

	void Argument_helper::new_named_string_vector(const char* key, const char* arg_description, const char* description,
		std::vector<std::string>& dest) {
		Argument_target* t = new StringVectorTarget(key, arg_description, description, dest);
		t->is_optional = true;
		new_argument_target(t);
	};

	void Argument_helper::write_usage(std::ostream& out) const {
		if (!company_name_.empty())
			out << company_name_ << ' ';
		if (!name_long_form_.empty()) {
			out << name_long_form_ << " Version ";
			out << version_.major << '.' << version_.minor << '.' << version_.revision << '.' << version_.build << ' ';
		}
		out << name_ << '\n';
		if (!author_.empty()) {
			out << "Copyright (c) " << author_;
			if (!date_.empty())
				out << ", " << date_;
			out << ". All rights reserved.\n";
		}
		out << '\n';
		std::ostringstream oss;
		oss << "Usage: " << name_ << " ";
		for (auto it = unnamed_arguments_.begin(); it != unnamed_arguments_.end(); ++it) {
			(*it)->write_name(oss);
		}
		for (auto it = optional_unnamed_arguments_.begin();
			it != optional_unnamed_arguments_.end(); ++it) {
			(*it)->write_name(oss);
		}
		for (auto it = keys_.begin(); it != keys_.end(); ++it) {
			(it->second)->write_name(oss);
		}
		text_wrap(oss.str().c_str(), out, 70);
		if (extra_arguments_ != nullptr)
			text_wrap(extra_arguments_arg_descr_.c_str(), out, 70);
		if (!description_.empty()) {
			out << "\n\nDescription:\n";
			text_wrap(description_.c_str(), out, 60, "\t");
		}
		out << "\n\nParameter list:\n";
		for (auto it = unnamed_arguments_.begin(); it != unnamed_arguments_.end(); ++it) {
			(*it)->write_usage(out);
			out << '\n';
		}
		for (auto it = optional_unnamed_arguments_.begin();
			it != optional_unnamed_arguments_.end(); ++it) {
			(*it)->write_usage(out);
			out << '\n';
		}
		for (auto it = keys_.begin(); it != keys_.end(); ++it) {
			(it->second)->write_usage(out);
			out << '\n';
		}
		if (!example_text_.empty()) {
			out << "\nExample:\n";
			out << example_text_ << '\n';
		}
	}

	void Argument_helper::write_values(std::ostream& out) const {
		for (auto it = unnamed_arguments_.begin(); it != unnamed_arguments_.end(); ++it) {
			(*it)->write_name(out);
			out << ": ";
			(*it)->write_value(out);
			out << '\n';
		}
		for (auto it = optional_unnamed_arguments_.begin(); it != optional_unnamed_arguments_.end(); ++it) {
			(*it)->write_name(out);
			out << ": ";
			(*it)->write_value(out);
			out << '\n';
		}
		if (extra_arguments_ != nullptr) {
			for (auto it = extra_arguments_->begin(); it != extra_arguments_->end(); ++it) {
				out << *it << " ";
			}
		}
		for (auto it = keys_.begin(); it != keys_.end(); ++it) {
			it->second->write_name(out);
			out << ": ";
			it->second->write_value(out);
			out << '\n';
		}
	}

	void Argument_helper::text_wrap(const char* input_string, std::ostream& out, size_t max_length, const char* indent_string) {
		std::istringstream iss(input_string);
		size_t line_len = 0;
		out << indent_string;
		char word[100];
		do {
			word[0] = '\0';
			iss >> word;
			line_len += strlen(word) + 1;
			if (line_len > max_length) {
				out << '\n' << indent_string;
				line_len = 0;
			}
			out << word << ' ';
		} while (iss);
		out << '\n';
	}

	void Argument_helper::process(int argc, const char** argv) {
		const char* basename = strrchr(argv[0], '\\');
		if (basename)
			set_name(basename + 1);
		else
			set_name(argv[0]);
		++argv;
		--argc;

		auto current_unnamed_ = unnamed_arguments_.begin();
		auto current_optional_unnamed_ = optional_unnamed_arguments_.begin();
		for (int i = 0; i < argc; ++i) {
			if (strcmp(argv[i], "/?") == 0) {
				write_usage(std::cout);
				exit(EXIT_SUCCESS);
			}
		}
		while (argc != 0) {
			const char* cur_arg = argv[0];
			if (cur_arg[0] == '/') {
				--argc; ++argv;

				if (cur_arg[1] == '\0') {
					std::cerr << cur_arg << " is an invalid parameter.\n";
					handle_error();
				}
				auto f = keys_.find(cur_arg + 1);
				if (f != keys_.end()) {
					if (!f->second->process(argc, argv))
						handle_error();
				}
				else {
					std::cerr << cur_arg << " is an invalid parameter.\n";
					handle_error();
				}
			}
			else {
				if (current_unnamed_ != unnamed_arguments_.end()) {
					Argument_target* t = *current_unnamed_;
					if (!t->process(argc, argv))
						handle_error();
					++current_unnamed_;
				}
				else if (current_optional_unnamed_ != optional_unnamed_arguments_.end()) {
					Argument_target* t = *current_optional_unnamed_;
					t->process(argc, argv);
					++current_optional_unnamed_;
				}
				else if (extra_arguments_ != nullptr) {
					extra_arguments_->push_back(cur_arg);
					--argc;
					++argv;
				}
				else {
					std::cerr << "Invalid extra argument " << argv[0] << '\n';
					handle_error();
				}
			}
		}
		if (current_unnamed_ != unnamed_arguments_.end()) {
			std::cerr << "Missing required arguments:\n";
			for (; current_unnamed_ != unnamed_arguments_.end(); ++current_unnamed_) {
				(*current_unnamed_)->write_name(std::cerr);
				std::cerr << '\n';
			}
			std::cerr << '\n';
			handle_error();
		}
	}

	void Argument_helper::handle_error() const {
		write_usage(std::cerr);
		exit(EXIT_FAILURE);
	}
}
