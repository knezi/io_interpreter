#include "scope.hpp"


// OBJECT CLASS DEFINITION
obj_ptr Object::getSlot(const std::string& ObjectName) {
	auto slot=Objects.find(ObjectName);
	if(slot==Objects.end())
		return nullptr;
	return slot->second;
}

obj_ptr Object::operator()(obj_ptr function_scope, Arguments& args) {
	std::cerr<<"Not callable"<<std::endl;
	// TODO RETURN
	return std::shared_ptr<Object>(this);
}

Object::Object(const Object& o): callable{o.callable} {
	std::cout<<"A"<<std::endl;
	copy_object(o);
}

Object& Object::operator=(const Object& o) {
	std::cout<<"A"<<std::endl;
	// TODO callable
	copy_object(o);
	return *this;
}

void Object::copy_object(const Object& src) {
	for(auto &&o:src.Objects) {
		std::cout<<"copying"<<o.first<<std::endl;
		Objects.insert({o.first, std::make_shared<Object>(o.second.get())});
	}
}


// FUNCTION CLASS DEFINITON
template<typename func>
Function<func>::Function(const Function& fo): Object{fo}, f{fo.f} {};

// ARGUMENTS CLASS DEFINITION
Arguments::Arguments(): tokens{} { it=tokens.begin(); };

Arguments::Arguments(tokenizerBase& tok): tokens{} {
	token currToken=tok.nextToken();
	while(currToken!=token::closeArguments) {
		std::cout<<"ARG "<<(int)currToken<<std::endl;
		tokens.emplace_back(currToken, tok.flush());
		currToken=tok.nextToken();
	}

	it=tokens.begin();
	tok.flush(); // flush the trailing )
}
