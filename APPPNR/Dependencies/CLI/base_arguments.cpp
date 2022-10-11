#include "base_arguments.h"
#include <iostream>

using namespace cli;

Argument::Argument(const std::string &argument_label, const string &description)
	:	present(false)
{
	string prefixed_label = "--" + argument_label;

	if (cli::argument_map.find(prefixed_label) != cli::argument_map.end()) {
		throw invalid_argument("Redeclaration of argument '" + argument_label + "'");
	}
	cli::argument_map[prefixed_label] = this;

	cli::argument_help_map[prefixed_label] = prefixed_label + ": " + description;

}

bool Argument::is_present(void) const {
	return this->present;
}

void Argument::set_as_present(void) {
	this->present = true;
}

UnvaluedArgument::UnvaluedArgument(const std::string &argument_label, const string &description)
	:	Argument(argument_label, description)
{}

void UnvaluedArgument::read_value_from(UNUSED const char *arg) {
	// do nothing
}

bool UnvaluedArgument::requires_value(void) const {
	return false;
}
