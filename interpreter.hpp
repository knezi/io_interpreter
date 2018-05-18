#ifndef interpreter_hpp_
#define interpreter_hpp_
#include <fstream>

class Interpreter {
	public:
		Interpreter(tokenizer& tok_): tok(tok_) {
			curr_scope=&main;
			tok.prepare(); // ????? TODO

			token currToken;
			do {
				currToken=tok.nextToken();
				if(curr_scope->callable) {
					Arguments args;
					if(currToken==token::openArguments) {
						args=Arguments(tok);
					}

					curr_scope=(*curr_scope)(args);
				}

				std::cout<<"PROCESSING "<<(int)currToken<<std::endl;

				switch(currToken) {
					case token::symbol:
						processSymbol();
						break;

					case token::terminator:
						curr_scope=&main;
						break;
						
					default:
						std::cerr<<"Unknown token."<<(int)currToken<<std::endl;
				}
			} while(currToken!=token::endOfBlock);

			if(!tok.eof())
				std::cerr<<"File hasn't been properly ended."<<std::endl;
		}

	private:
		tokenizer& tok;
		Object* curr_scope;
		Object main;

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
				curr_scope->addIntoSlot(s, std::make_unique<Number>(stoi(s)));
				curr_scope=curr_scope->getSlot(s);
				return;
			}
			
			
			curr_scope=curr_scope->getSlot(s);
		}

};
#endif
