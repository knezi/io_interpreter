#include "tokenizer.hpp"
#include "scope.hpp"
#include "interpreter.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>


using namespace std;
int main(int argc, char * * argv) {
	// std::ifstream iff { "tests/testfile_correct.io" };
	std::ifstream iff { "tests/testfile.io" };
	processStream in(iff);
	tokenizer tok(in);
	Interpreter run(tok);
	
	return 0;
}
