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
	std::ifstream iff { "/home/knezi/Dokumenty/mff/cpp/zapoctak/tests/testfile.io" };
	processStream in(iff);
	tokenizer tok(in);
	cout<<"####"<<endl;
	Interpreter run(tok, false);
	
	// tok.prepare();
	// token a=tok.nextToken();
	// while(a!=token::endOfBlock) {
		// cout<<(int)a<<" "<<tok.flush()<<endl;
		// a=tok.nextToken();
	// }


	return 0;
}
