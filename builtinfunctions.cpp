#include "builtinfunctions.hpp"
#include "interpreter.hpp"

namespace builtins {

obj_ptr Method::operator()(obj_ptr scope, Arguments& args_values) {
	// TODO args_values prolozit
	return args.execute(scope);
}

};
