#include "scope.hpp"
#include "interpreter.hpp"

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

obj_ptr Object::clone() {
	obj_ptr new_obj=std::make_shared<Object>(callable);
	cloneScope(new_obj);
	return new_obj;
}

void Object::cloneScope(const obj_ptr& new_obj) {
	for(auto &&o:Objects) {
		new_obj->addIntoSlot(o.first, o.second->clone());
	}
}

// FUNCTION CLASS DEFINITON

// ARGUMENTS CLASS DEFINITION
Arguments::Arguments() { it=tokens.begin(); };

Arguments::Arguments(tokenizerBase& tok) {
	// TODO rewrite
	size_t closing=1;
	token currToken=tok.nextToken();
	if(currToken==token::closeArguments) --closing;
	if(currToken==token::openArguments) ++closing;

	while(closing>0) {
		// std::cout<<"ARG "<<(int)currToken<<" "<<tok.flush()<<std::endl;
		tokens.emplace_back(currToken, tok.flush());
		currToken=tok.nextToken();
		if(currToken==token::closeArguments) --closing;
		if(currToken==token::openArguments) ++closing;
	}

	tokens.emplace_back(token::endOfBlock, "");

	it=tokens.begin();
	tok.flush(); // flush the trailing )
}

obj_ptr Arguments::execute(obj_ptr& scope) {
	tokenizerBuilder exec(tokens);
	Interpreter interp(exec, false, scope);
	return interp.lastScope();
}
