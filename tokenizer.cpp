// #define DEBUG
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <memory>
#include "tokenizer.hpp"

charGroup processStream::getCurrent() {
	if(eof_) return charGroup::eof;

	switch(curr_) {
		case ';':
			return charGroup::terminator;

		case '\n':
		case '\r':
			return charGroup::newline;

		case ' ':
		case '\t':
		case '\v':
		case '\f':
			return charGroup::whitespace;

		case '/':
			return charGroup::slash;

		case '\\':
			return charGroup::backslash;

		case '"':
			return charGroup::quote;
			
		case ',':
			return charGroup::comma;

		case '(':
			return charGroup::open;

		case ')':
			return charGroup::close;
	}


	if(('a'<=curr_ && curr_<='z') ||
		('A'<=curr_ && curr_<='Z') ||
		('0'<=curr_ && curr_<='9'))
		return charGroup::alphanum;

	// for(char && c: ops_)  ??
	for(char c: ops_) {
		if(c==curr_)
			return charGroup::op;
	}

#ifdef DEBUG
	std::cout<<"INVALID "<<curr_<<std::endl;
#endif
	return charGroup::invalid;
};


std::string processStream::flush() {
	std::string tmp=std::move(buffer_);
	return tmp;
};

void processStream::move() {
	in_>>curr_;
	buffer_.push_back(curr_);
	if(in_.eof())
		eof_=true;

#ifdef DEBUG
	std::cout<<in_.eof()<<" "<<curr_<<std::endl;
#endif
};


void processStream::moveBack() {
	if(buffer_.size()!=0)
		buffer_.pop_back();

	if(in_.tellg()>0)
		in_.unget();
};


int main(int argc, char * * argv) {
	std::ifstream iff { "tests/testfile.io" };
	processStream in(iff);

	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	std::cout<<in.flush()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	std::cout<<in.flush()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;
	in.move(); std::cout<<in.getCurrent()<<std::endl;

	std::cout<<in.flush()<<std::endl;
	return 0;
}
