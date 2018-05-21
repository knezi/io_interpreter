#ifndef interpreter_hpp_
#define interpreter_hpp_
#include <fstream>
#include <exception>

#include "tokenizer.hpp"
#include "scope.hpp"
#include "builtinfunctions.hpp"

class Interpreter {
	public:
		Interpreter(tokenizerBase& tok_, bool terminator_):
			Interpreter(tok_, terminator_, std::make_shared<Object>()) {};
		Interpreter(tokenizerBase& tok_, bool terminator_, obj_ptr main_);

		std::shared_ptr<Object> lastScope() {
			return old_scope;
		}

	private:
		tokenizerBase& tok;
		obj_ptr curr_scope;
		size_t curr_scope_priority;
		obj_ptr function_scope;
		obj_ptr main;
		obj_ptr old_scope;
		bool endAtTerminator;
		bool terminator;
		void processSymbol();
		void resetScope();
		void runBlock();
};
#endif
