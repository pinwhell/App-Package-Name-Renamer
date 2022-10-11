#include "command_line_interface.h"

#include <iostream>

using namespace std;
using namespace cli;

unordered_map<string, ArgumentInterface*> cli::argument_map;
unordered_map<string, string> cli::argument_help_map;

void cli::capture_all_arguments_from(int argc, char **argv) {
	int i = 1;
	decltype(cli::argument_map)::iterator argument_index;
	ArgumentInterface *argument;
	string arg_label;

	while (i < argc) {
		arg_label = argv[i];

		argument_index = cli::argument_map.find(arg_label);
		if (argument_index == cli::argument_map.end()) {
			throw std::invalid_argument("Invalid argument '"+arg_label+"'");
		}

		argument = argument_index->second;
		if (argument->requires_value()) {
			i++;
			if (i == argc) {
				throw std::domain_error("Argument '"+arg_label+"' requires a valid value");
			}

			try {
				argument->read_value_from(argv[i]);
			} catch (const std::invalid_argument &e) {
				throw std::invalid_argument("Could not parse value for argument '"+arg_label+"': "+e.what());
			}
		} else {
			argument->set_as_present();
		}

		i++;
	}

	for (auto possible_argument : cli::argument_map) {
		arg_label = possible_argument.first;
		argument = possible_argument.second;
		if (argument->is_required() && !argument->is_present()) {
			throw std::domain_error("Missing required argument '"+arg_label+"'");
		}
	}

}

void create_alias_helper(const string &existing_label, const string &prefixed_existing_label, const string &alias, const string &prefixed_alias) {

	auto existing_argument = cli::argument_map.find(prefixed_existing_label);
	if (existing_argument == cli::argument_map.end()) {
		throw invalid_argument("Argument " + existing_label + " not previously declared");
	}

	if (cli::argument_map.find(prefixed_alias) != cli::argument_map.end()) {
		throw invalid_argument("Redeclaration of argument '" + alias + "'");
	}
	cli::argument_map[prefixed_alias] = existing_argument->second;

	auto argument_help = cli::argument_help_map.find(prefixed_existing_label);
	if (argument_help == cli::argument_help_map.end()) {
		throw invalid_argument("Cannot create alias to another alias: '"+existing_label+"'");
	}
	argument_help->second = prefixed_alias + ", " + argument_help->second;
}

void cli::create_alias(const string &existing_label, const string &alias) {
	string prefixed_existing_label = "--"+existing_label;
	string prefixed_alias = "--"+alias;
	create_alias_helper(existing_label, prefixed_existing_label, alias, prefixed_alias);
}

void cli::create_alias(const string &existing_label, char alias) {
	string prefixed_existing_label = "--"+existing_label;
	string str_alias = string(1, alias);
	string prefixed_alias = "-" + str_alias;
	create_alias_helper(existing_label, prefixed_existing_label, str_alias, prefixed_alias);
}
