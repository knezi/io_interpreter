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

// ARGUMENTS CLASS DEFINITION
Arguments::Arguments() {
	tokens.emplace_back(token::openArguments, "(");
	tokens.emplace_back(token::closeArguments, ")");
}

Arguments::Arguments(tokenizer& tok) {
	token currToken;
	do {
		currToken=tok.nextToken();
		tokens.emplace_back(currToken, tok.flush());
	} while(currToken!=token::closeArguments);
}
