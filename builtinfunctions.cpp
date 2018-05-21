#include "builtinfunctions.hpp"
#include "interpreter.hpp"

namespace builtins {

obj_ptr Method::operator()(obj_ptr scope, Arguments& args_values) {
	// TODO args_values prolozit
	return args.execute(scope);
}


obj_ptr traits<Bool>::print(obj_ptr scope, Arguments& args) {
	//TODO
	std::cout<<"PRITING VALUE ";
	if(((Bool*)scope.get())->value)
		std::cout<<"TRUE";
	else
		std::cout<<"FALSE";
	std::cout<<std::endl;
	return scope;
}

};
