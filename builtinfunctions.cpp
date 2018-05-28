#include "builtinfunctions.hpp"
#include "interpreter.hpp"

namespace builtins {

obj_ptr Method::operator()(obj_ptr scope, Arguments& args_values) {
	return args.execute(scope, args_values);
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

// control flow
obj_ptr cond(obj_ptr scope, Arguments& args) {
	obj_ptr cond=args.exec_curr_part(scope);

	if(((Bool*)cond.get())->value) {
		return args.exec_curr_part(scope);
	}else{
		args.next_argument();
		if(!args.eof()) {
			return args.exec_curr_part(scope);
		}
	}

	return scope;
}

obj_ptr while_(obj_ptr scope, Arguments& args) {
	obj_ptr cond=args.exec_curr_part(scope);

	while(((Bool*)cond.get())->value) {
		args.exec_curr_part(scope);
		args.restart();
		cond=args.exec_curr_part(scope);
	}

	return scope;
}

obj_ptr for_(obj_ptr scope, Arguments& args) {
	tokenizerBuilder tok;
	std::string var=args.flush();
	args.move(); args.move();

	// counter
	tok.addToken(token::symbol, var);
	tok.addToken(token::symbol, ":=");
	obj_ptr start=args.exec_curr_part(scope)->clone();
	tok.addToken(token::symbol, std::to_string(((Number*)start.get())->value));
	tok.addToken(token::terminator, "");

	// while loop
	tok.addToken(token::symbol, "while");
	tok.addToken(token::openArguments, "(");

		// cond
		tok.addToken(token::symbol, var);
		tok.addToken(token::symbol, "!=");
		obj_ptr stop=args.exec_curr_part(scope)->clone();
		tok.addToken(token::symbol, std::to_string(((Number*)stop.get())->value));
		tok.addToken(token::nextArgument, ",");


		// body
		tokenlist step;
		while(args.currToken()!=token::nextArgument) {
			step.emplace_back(args.currToken(), args.flush());
			args.move();
		}
		args.move();


		// code
		while(!args.eof() && args.currToken()!=token::endOfBlock) {
			tok.addToken(args.currToken(), args.flush());
			args.move();
		}
		tok.addToken(token::terminator, ";");

		// step
		tok.addToken(token::symbol, var);
		tok.addToken(token::symbol, "=");
		tok.addToken(token::symbol, var);
		tok.addToken(token::symbol, "+");
		tok.addTokens(step);


	tok.addToken(token::closeArguments, ")");
	tok.addToken(token::terminator, "");
	tok.restart();

	Interpreter exec(tok, false, scope);

	return scope;
}

obj_ptr new_bool(bool val, obj_ptr upper) {
	auto new_no=std::make_shared<builtins::Bool>(val);
	new_no->addIntoSlot("print",
			std::make_shared<Function<func_ptr>>(traits<Bool>::print));
	new_no->addUpperScope(upper);

	add_operators<bool>(new_no);

	return new_no;
}

};
