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

// Object::Object(const Object& o): callable{o.callable} {
	// std::cout<<"A"<<std::endl;
	// copy_object(o);
// }

// Object& Object::operator=(const Object& o) {
	// std::cout<<"A"<<std::endl;
	// // TODO callable
	// copy_object(o);
	// return *this;
// }

obj_ptr Object::clone() {
	obj_ptr new_obj=std::make_shared<Object>(callable);
	cloneScope(new_obj);
	return new_obj;
}

void Object::cloneScope(const obj_ptr& new_obj) {
	for(auto &&o:Objects) {
		std::cout<<"copying"<<o.first<<std::endl;
		new_obj->addIntoSlot(o.first, o.second->clone());
	}
}


// FUNCTION CLASS DEFINITON

// ARGUMENTS CLASS DEFINITION
Arguments::Arguments() { it=tokens.begin(); };

Arguments::Arguments(tokenizerBase& tok) {
	token currToken=tok.nextToken();
	while(currToken!=token::closeArguments) {
		std::cout<<"ARG "<<(int)currToken<<std::endl;
		tokens.emplace_back(currToken, tok.flush());
		currToken=tok.nextToken();
	}

	it=tokens.begin();
	tok.flush(); // flush the trailing )
}
