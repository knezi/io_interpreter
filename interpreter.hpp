#ifndef interpreter_hpp_
#define interpreter_hpp_
#include <fstream>
#include <exception>

class Interpreter {
	public:
		Interpreter(tokenizer& tok_, bool terminator_):
			Interpreter(tok_, terminator_, std::make_shared<Object>()) {};

		Interpreter(tokenizer& tok_, bool terminator_, obj_ptr main_):
			tok{tok_}, endAtTerminator{terminator_}, main{main_}{
			// TODO
			curr_scope=main;
			tok.prepare();

			token currToken;
			do {
				currToken=tok.nextToken();
				terminator=false;
				if(curr_scope->callable) {
					Arguments args;
					if(currToken==token::openArguments) {
						args=Arguments(tok);
					}

					// std::cout<<"CALL METHOD"<<std::endl;
					curr_scope=(*curr_scope)(function_scope, args);
				}

				// std::cout<<"PROCESSING "<<(int)currToken<<std::endl;

				switch(currToken) {
					case token::symbol:
						processSymbol();
						break;

					case token::terminator:
						resetScope();
						break;
						
					default:
						std::cerr<<"Unknown token."<<(int)currToken<<std::endl;
				}
			} while(!tok.eof() && currToken!=token::endOfBlock &&
					!(endAtTerminator && terminator));

			// TODO
			// if(!tok.eof())
				// std::cerr<<"File hasn't been properly ended."<<std::endl;
		}


		std::shared_ptr<Object> lastScope() {
			return old_scope;
		}

	private:
		tokenizer& tok;
		obj_ptr curr_scope;
		obj_ptr function_scope;
		obj_ptr main;
		obj_ptr old_scope;
		bool endAtTerminator;
		bool terminator;

		void processSymbol() {
			std::string s=tok.flush();
			std::cout<<"SYMBOL "<<s<<std::endl;
			bool no=true;
			// std::cout<<"Calling print"<<std::endl;
			for(auto&& c:s) {
			// std::cout<<"Calling print"<<std::endl;
				if(!('0'<=c && c<='9')) {
					no=false;
					break;
				}
			}
			// std::cout<<"Calling print"<<std::endl;

			if(no) {
				// TODO TMP
				std::shared_ptr<Number> new_no=std::make_shared<Number>(stoi(s));
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
					std::cout<<">>"<<std::endl;
					Interpreter expr(tok, true, curr_scope);
					std::cout<<"<<"<<std::endl;
					// TODO prasarna
					// std::cout<<"ADDING "<<s<<std::endl;
					curr_scope->addIntoSlot(s, expr.lastScope());
					resetScope();
				}else{
					// TODO throw exception
					throw std::exception();
				}
			}else{
				if(next_slot->callable)
					function_scope=curr_scope;
				curr_scope=next_slot;
			}
		}

		void resetScope() {
			// std::cout<<"RESETING"<<std::endl;
			old_scope=curr_scope;
			curr_scope=main;
			terminator=true;
		}
};
#endif
