#pragma once

#include <unordered_map>
#include <sstream>

#ifdef __GNUC__
	#define UNUSED __attribute__((unused))
#else
	#define UNUSED
#endif

namespace cli {

	using namespace ::std;

	class ArgumentInterface {
		public:
			virtual void read_value_from(const char* arg) = 0;
			virtual bool is_required(void) const = 0;
			virtual bool requires_value(void) const = 0;
			virtual bool is_present(void) const = 0;
			virtual void set_as_present(void) = 0;
	};

	extern unordered_map<string, ArgumentInterface*> argument_map;
	extern unordered_map<string, string> argument_help_map;

	class Argument : public ArgumentInterface {
		protected:
			bool present;
		public:
			Argument(const string &argument_label, const string &description);

			virtual bool is_present (void) const;
			virtual void set_as_present(void);
	};

	template<typename T>
	class ValuedArgument : public Argument {
		protected:
			T value;
		public:
			ValuedArgument(const string &argument_label, const string &description)
				:	Argument(argument_label, description)
			{}

			virtual T& operator* (void) {
				return this->value;
			}
			virtual const T& operator* (void) const {
				return this->value;
			}
			virtual T& operator-> (void) {
				return this->value;
			}
			virtual const T& operator-> (void) const {
				return this->value;
			}

			virtual void read_value_from(const char* arg) {
				std::istringstream stream(arg);
				stream >> this->value;
				if (stream.fail()) {
					throw invalid_argument("Could not parse '"+string(arg)+"' to the correct type");
				}
				this->present = true;
			}

			bool requires_value(void) const {
				return true;
			}
	};

	class UnvaluedArgument : public Argument {
		public:
			UnvaluedArgument(const string &argument_label, const string &description);
		
			void read_value_from(UNUSED const char *arg);
			bool requires_value(void) const;
	};
}
