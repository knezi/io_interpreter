#include "builtinfunctions.hpp"
#include "interpreter.hpp"

namespace builtins {

Method::Method(Arguments& args): tok{} {
		token currToken;

		while(!args.eof()) {
			tokens.emplace_back();

			do{
				currToken=args.currToken();
				if(currToken==token::nextArgument) break;

				tokens[tokens.size()-1].emplace_back(currToken, args.flush());
				args.move();
			} while(!args.eof());
		}

		tokens[tokens.size()-1].emplace_back(token::endOfBlock, "");
		
		tok.addTokens(tokens[tokens.size()-1]);
		for(auto&& a:tokens[0])
			std::cout<<(int)a.first<<" "<<a.second<<std::endl;
		tok.restart();
	token c=tok.nextToken();
	while(c!=token::endOfBlock){
		std::cout<<(int)c<<tok.flush()<<std::endl;
		c=tok.nextToken();
	}
}

obj_ptr Method::operator()(obj_ptr scope, Arguments& args) {
	std::cout<<"AHOJ"<<std::endl;
	tok.restart();
	token c=tok.nextToken();
	while(c!=token::endOfBlock){
		std::cout<<(int)c<<tok.flush()<<std::endl;
		c=tok.nextToken();
	}
	tok.restart();
	Interpreter execute(tok, false, scope);
	return execute.lastScope();
}

// PRIMITIVETYPE CLASS DEFINITION
// WTF??
// template<typename t>
// void PrimitiveType<t>::addBuiltIns() {
	// // addIntoSlot("print", std::make_shared<Function<obj_ptr (*) (obj_ptr, Arguments&)>>(print<PrimitiveType<t>>));
	// // addIntoSlot("++", std::make_shared<Function<obj_ptr (*) (obj_ptr, Arguments&)>>(builtins::increment<PrimitiveType<t>>));
	// // auto a=std::make_shared<Function<builtins::method>>(builtins::method(10));
	// // addIntoSlot("method", a);
// }


// template<typename t>
// obj_ptr PrimitiveType<t>::clone() {
	// std::cout<<"RPIM";
	// obj_ptr new_obj=std::make_shared<PrimitiveType<t>>(value);
	// cloneScope(new_obj);
	// return new_obj;
// }

template<typename t>
obj_ptr plus(obj_ptr scope, Arguments& args) {
	token currToken;
	std::vector<tokenlist> tokens;
	tokenizerBuilder tok;

	while(!args.eof()) {
		tokens.emplace_back();

		do{
			currToken=args.currToken();
			if(currToken==token::nextArgument) break;

			tokens[tokens.size()-1].emplace_back(currToken, args.flush());
			args.move();
		} while(!args.eof());
	}

	tokens[tokens.size()-1].emplace_back(token::endOfBlock, "");
	
	tok.addTokens(tokens[tokens.size()-1]);
	tok.restart();

	Interpreter execute(tok, false, scope);

	((t*)scope.get())->value+=((t*)execute.lastScope().get())->value;
	return scope;
}


template obj_ptr plus<Number>(obj_ptr scope, Arguments& args);

template<typename t>
obj_ptr times(obj_ptr scope, Arguments& args) {
	token currToken;
	std::vector<tokenlist> tokens;
	tokenizerBuilder tok;

	while(!args.eof()) {
		tokens.emplace_back();

		do{
			currToken=args.currToken();
			if(currToken==token::nextArgument) break;

			tokens[tokens.size()-1].emplace_back(currToken, args.flush());
			args.move();
		} while(!args.eof());
	}

	tokens[tokens.size()-1].emplace_back(token::endOfBlock, "");
	
	tok.addTokens(tokens[tokens.size()-1]);
	tok.restart();

	Interpreter execute(tok, false, scope);

	((t*)scope.get())->value*=((t*)execute.lastScope().get())->value;
	return scope;
}


template obj_ptr times<Number>(obj_ptr scope, Arguments& args);
};
