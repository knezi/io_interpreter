// #define DEBUG
#include <utility>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <map>
#include <memory>
#include "tokenizer.hpp"

// PROCESS STREAM

charGroup processStream::getCurrent() const {
	if(eof_) return charGroup::eof;

	switch(curr_) {
		case ';':
			return charGroup::semicolon;

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

		case '#':
			return charGroup::hash;

		case '*':
			return charGroup::star;
	}


	if(('a'<=curr_ && curr_<='z') ||
		('A'<=curr_ && curr_<='Z') ||
		('0'<=curr_ && curr_<='9') ||
		('_' == curr_))
		return charGroup::alphanum;

	// for(char && c: ops_)  ?? TODO
	for(char c: ops_) {
		if(c==curr_)
			return charGroup::op;
	}

#ifdef DEBUG
	std::cout<<"INVALID CHARACTER "<<curr_<<" "<<(int)curr_<<std::endl;
#endif
	return charGroup::invalid;
};


std::string processStream::flush() {
	std::string tmp=std::move(buffer_);
	return tmp;
};

void processStream::move() {
	// in this order, so curr_ is never in the buffer
	buffer_.push_back(curr_);
	in_>>curr_;
	if(in_.eof()) // TODO EOF _ is the last character added? No chance
		eof_=true;

#ifdef DEBUG
	std::cout<<"DEBUG: PROCESS STREAM: EOF? "<<in_.eof()<<" "<<curr_<<std::endl;
#endif
};


// needed for comments
void processStream::moveBack() {
	if(buffer_.size()!=0)
		buffer_.pop_back();

	if(in_.tellg()>0)
		in_.unget();
};


// TOKENIZER
template<charGroup group, token tok>
token parseOneChar(processStream& in) {
	if(in.getCurrent()==group) {
		in.move();
		return tok;
	}
	return token::invalidCharacter;
}

token parseSymbol(processStream& in) {
	charGroup group=in.getCurrent();
	if(group!=charGroup::alphanum &&
			group!=charGroup::quote &&
			group!=charGroup::op)
		return token::invalidCharacter;
	
	while(group==charGroup::alphanum ||
			group==charGroup::quote ||
			group==charGroup::op) {
		in.move();
		group=in.getCurrent();
	}

	return token::symbol;
}

// leaves current at the last character of the comment (\n or slash)
token parseComment(processStream& in) {
	charGroup group=in.getCurrent();
	bool inlineComment=false;

	if(group==charGroup::slash) {
		in.move();
		group=in.getCurrent();
		// multiline comment
		if(group==charGroup::star) {
			while(true) {
				in.move();
				if(in.getCurrent()==charGroup::star) {
					in.move();
					if(in.getCurrent()==charGroup::slash)
						return token::skip;
					// because of **/
					in.moveBack();
				}
			}
		}
		else if(group==charGroup::slash)
			inlineComment=true;
		else {
			in.moveBack();
			return token::invalidCharacter;
		}
	}

	if(group==charGroup::hash || inlineComment) {
		while(group!=charGroup::newline &&
				group!=charGroup::eof) {
			in.move();
			group=in.getCurrent();
		}
		return token::skip;
	}

	return token::invalidCharacter;
}

// Whitespace (' ', \f, \t, \v), [Newline (\r, \n)] + comments
// satisfies empty string
template<bool newLines>
token parseWC(processStream& in) {
	charGroup group=in.getCurrent();
	while(group==charGroup::whitespace ||
			(newLines && group==charGroup::newline) ||
			parseComment(in)==token::skip) {
		in.move();
		group=in.getCurrent();
	}
	return token::skip;
}

token parseTerminator(processStream& in) {
	charGroup group=in.getCurrent();
	if(group==charGroup::semicolon ||
		group==charGroup::newline) {
		in.move();
		return token::terminator;
	}
	return token::invalidCharacter;
}

// checks for ) , or EOF
token parseBlockEnd(processStream& in) {
	charGroup group=in.getCurrent();
	if(group==charGroup::eof ||
			group==charGroup::comma ||
			group==charGroup::close)
		// don't move here - comma or close will be used by the upper layer
		return token::endOfArgument;

	return token::invalidCharacter;
}

void tokenizer::prepare() {
	if(ready_) return;
	ready_=true;

	// prepare boxes
	boxes_.insert({"WC1", std::make_unique<tokenBox>(in_, parseWC<false>)});
	
	boxes_.insert({"BlockEnd",
			std::make_unique<tokenBox>(in_, parseBlockEnd)});

	boxes_.insert({"Terminator",
			std::make_unique<tokenBox>(in_, parseTerminator)});
	
	boxes_.insert({"Symbol",
			std::make_unique<tokenBox>(in_, parseSymbol)}); // todo quote
	boxes_.insert({"WC2",
			std::make_unique<tokenBox>(in_, parseWC<false>)});
	boxes_.insert({"Open",
			std::make_unique<tokenBox>(in_,
					parseOneChar<charGroup::open, token::openArgument>)});
	boxes_.insert({"NWC1",
			std::make_unique<tokenBox>(in_, parseWC<true>)});
	boxes_.insert({"Argument", std::make_unique<argumentBox>(in_)});
	boxes_.insert({"NWC2",
			std::make_unique<tokenBox>(in_, parseWC<true>)});
	boxes_.insert({"Comma",
			std::make_unique<tokenBox>(in_,
					parseOneChar<charGroup::comma, token::terminator>)}); // TODO skip??
	boxes_.insert({"Close",
			std::make_unique<tokenBox>(in_,
					parseOneChar<charGroup::close, token::closeArgument>)});
	
	// transition
	std::vector<std::pair<std::string, std::string> > transitions={
											{"WC1", "Terminator"}, 
											{"Terminator", "WC1"}, 

											{"WC1", "BlockEnd"}, 
	
											{"WC1", "Symbol"},
											{"Symbol", "WC2"},
											{"WC2", "Open"},
											{"Open", "NWC1"},
											{"NWC1", "Argument"},
											{"Argument", "NWC2"},
											{"NWC2", "Close"},
											{"Close", "WC1"},
	
											{"NWC2", "Comma"},
											{"Comma", "NWC1"},
											{"WC2", "WC1"}}; // this has lower priority DIRTY TRICK WARNING TODO


	for(auto && tr: transitions) {
		box* to=boxes_.find(tr.second)
								->second.get();
		boxes_.find(tr.first)
			->second
			->addNextBox(to);
	}

	curr_=boxes_.find("WC1")->second.get();
}

token tokenizer::nextToken() {
	box* newCurr=curr_->getNextBox();
	token currToken=newCurr->parseToken(); // TODO KDYŽ DOJDOU BOXY
	while(currToken==token::invalidCharacter) {
		newCurr=curr_->getNextBox();
		currToken=newCurr->parseToken();
	}

	curr_=newCurr;

	return currToken;
}

int main(int argc, char * * argv) {
	std::ifstream iff { "tests/testfile.io" };
	processStream in(iff); 

	tokenizer run(in);
	run.prepare();

	// std::cout<<(int)run.nextToken()<<' '<<in.flush()<<std::endl;

	token currToken;
	do {
		currToken=run.nextToken();
		if(currToken==token::skip) {
			in.flush();
			continue;
		}

		std::cout<<(int)currToken<<' '<<in.flush()<<std::endl;
	} while(currToken!=token::endOfArgument);

	return 0;
}
