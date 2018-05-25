#include "interpreter.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>

Interpreter::Interpreter(tokenizerBase& tok_, bool terminator_, obj_ptr main_):
	tok{tok_}, endAtTerminator{terminator_}, main{main_}{
	// TODO
	main->addIntoSlot("method", std::make_shared<Function<func_ptr>>(builtins::createMethod));
	curr_scope=main;
	tok.prepare();

	token currToken;
	do {
		currToken=tok.nextToken();
		terminator=false;
		if(curr_scope->callable) {

			if(currToken==token::openArguments) {
				tok.flush();
				Arguments args=Arguments(tok);
				curr_scope=(*curr_scope)(function_scope, args);
				currToken=tok.nextToken();
			}else{
				Arguments args;
				if(currToken==token::symbol || currToken==token::openArguments) {
					std::string symbol=tok.flush();
					while(symbolPriority(symbol)>curr_scope_priority) {
						if(currToken==token::symbol)
							args.addToken(currToken, symbol);
						else
							args.addTilClose(tok);

						currToken=tok.nextToken();
						if(currToken!=token::symbol && currToken!=token::openArguments) break;
						symbol=tok.flush();
					}
				}
				args.addToken(token::terminator, "");
				args.restart();

				curr_scope=(*curr_scope)(function_scope, args);
			}
		}


		switch(currToken) {
			case token::symbol:
				processSymbol();
				break;

			case token::terminator:
			case token::endOfBlock:
				resetScope();
				break;

			case token::openArguments:
				std::cout<<">>"<<std::endl;
				runBlock();
				std::cout<<"<<"<<std::endl;
				break;

			default:
				std::cerr<<"Unknown token."<<(int)currToken<<std::endl;
		}
	} while(!tok.eof() && currToken!=token::endOfBlock &&
			!(endAtTerminator && terminator));
}

void Interpreter::processSymbol() {
	std::string s=tok.flush();
	// std::cout<<"PROCESSING "<<s<<std::endl;
	if(s.empty())
		return;

	if(s=="True" || s=="False") {
		auto new_obj=builtins::new_bool(s[0]=='T', curr_scope);
		curr_scope->addIntoSlot(s, new_obj);
		curr_scope=curr_scope->getSlot(s);
		return;
	}

	bool no=true;
	for(auto&& c:s) {
		if(!('0'<=c && c<='9')) {
			no=false;
			break;
		}
	}

	if(no) {
		// TODO bug TMP
		auto new_no=builtins::new_number(stoi(s), curr_scope);
		curr_scope->addIntoSlot(s, new_no);
		curr_scope=curr_scope->getSlot(s);
		return;
	}
	
	obj_ptr next_slot=curr_scope->getSlot(s);

	// the following must be := or die
	if(next_slot==nullptr) {
		token nextTok=tok.nextToken();
		std::string op=tok.flush();

		if(nextTok==token::symbol && op==":=") {
			Interpreter expr(tok, true, curr_scope);
			obj_ptr copy=(expr.lastScope())->clone();
			curr_scope->addIntoSlot(s, copy);
			resetScope();
		}else{
			throw std::logic_error("Slot "+s+" not defined.");
		}
	}else{
		if(next_slot->callable) {
			function_scope=curr_scope;
			curr_scope_priority=symbolPriority(s);
		}
		curr_scope=next_slot;
	}
}

void Interpreter::runBlock() {
	Arguments block(tok);
	curr_scope=block.execute(curr_scope)->clone();
}

void Interpreter::resetScope() {
	old_scope=curr_scope;
	curr_scope=main;
	terminator=true;
}

using namespace std;
int main(int argc, char * * argv) {
	// std::ifstream iff { "tests/testfile_correct.io" };
	// std::ifstream iff { "/home/knezi/Dokumenty/mff/cpp/zapoctak/tests/testfile.io" };
	std::ifstream iff { "tests/testfile.io" };
	processStream in(iff);
	tokenizer tok(in);
	cout<<"####"<<endl;
	Interpreter run(tok, false);


	return 0;
}
