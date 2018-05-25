#include "scope.hpp"
#include "interpreter.hpp"

// OBJECT CLASS DEFINITION
obj_ptr Object::getSlot(const std::string& ObjectName) {
	auto slot=Objects.find(ObjectName);
	if(slot==Objects.end()) {
		if(UpperScope.get()!=nullptr)
			return UpperScope->getSlot(ObjectName);
		return nullptr;
	}
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
	addTilClose(tok);
}

void Arguments::addTilClose(tokenizerBase& tok) {
	// TODO rewrite
	size_t closing=1;
	token currToken=tok.nextToken();
	if(currToken==token::closeArguments) --closing;
	if(currToken==token::openArguments) ++closing;

	while(closing>0) {
		// std::cout<<"ARG "<<(int)currToken<<" "<<std::endl;
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

obj_ptr Arguments::execute(obj_ptr& scope, Arguments& args_values) {
	tokenizerBuilder exec;
	auto it=tokens.begin();
	while(it!=tokens.end() && it->first!=token::nextArgument) {
		exec.addToken(it->first, it->second);
		++it;
	}

	while(it!=tokens.end()) {
        ++it;
		if(it!=tokens.end()) {
			exec.addToken(token::symbol, ":=");
			while(!args_values.eof() && args_values.currToken()!=token::terminator
					&& args_values.currToken()!=token::endOfBlock
					&& args_values.currToken()!=token::nextArgument) {
				exec.addToken(args_values.currToken(), args_values.flush());
				args_values.move();
			}
			args_values.move();
			exec.addToken(token::terminator, "");
		}

		while(it!=tokens.end() && it->first!=token::nextArgument) {
			exec.addToken(it->first, it->second);
			++it;
		}
	}

	exec.restart();
	Interpreter interp(exec, false, scope);
	return interp.lastScope();
}

