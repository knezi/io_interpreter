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
	return std::move(buffer_);
};

void processStream::move() {
	// in this order, so curr_ is never in the buffer
	buffer_.push_back(curr_);
	in_>>curr_;

	// this is true when there are no more chars
	// -> previous op returns nonsense
	if(in_.eof())
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
token parseDummy(processStream& in) {
	return token::invalidCharacter;
}

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
	// star alone is a symbol
	if(group==charGroup::star) {
		in.move();
		return token::symbol;
	}
	
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
		return token::endOfBlock;

	return token::invalidCharacter;
}

void tokenizer::prepare() {
	if(ready_) return;
	ready_=true;

	// prepare boxes
	boxes_.insert({"Dummy", std::make_unique<tokenBox>(in_, parseDummy)});
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
					parseOneChar<charGroup::open, token::openArguments>)});
	boxes_.insert({"NWC1",
			std::make_unique<tokenBox>(in_, parseWC<true>)});
	boxes_.insert({"Argument", std::make_unique<argumentBox>(in_)});
	boxes_.insert({"NWC2",
			std::make_unique<tokenBox>(in_, parseWC<true>)});
	boxes_.insert({"Comma",
			std::make_unique<tokenBox>(in_,
					parseOneChar<charGroup::comma, token::nextArgument>)});
	boxes_.insert({"Close",
			std::make_unique<tokenBox>(in_,
					parseOneChar<charGroup::close, token::closeArguments>)});
	
	// transition
	std::vector<std::pair<std::string, std::string> > transitions={
											{"Dummy", "WC1"}, 
											{"WC1", "Terminator"}, 
											{"Terminator", "WC1"}, 

											{"WC1", "BlockEnd"}, 
	
						// lower priority than Terminator and BlokEnd
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

	auto dummyBox=boxes_.find("Dummy")->second.get();

	for(auto && box: boxes_) {
		box.second->addNextBox(dummyBox);
	}

	curr_=dummyBox;
}

token tokenizer::nextToken() {
	token currToken;

	// automatically skips tokens skip
	do {
		in_.flush();
		box* newCurr=curr_->getNextBox();
		currToken=newCurr->parseToken();
		while(currToken==token::invalidCharacter) {
			newCurr=curr_->getNextBox();
			currToken=newCurr->parseToken();
		}

		curr_=newCurr;
	} while(currToken==token::skip);

	// std::cout<<"TOKEN "<<(int)currToken<<std::endl;
	return currToken;
}


box* box::getNextBox() {
	if(indexNextBox_>nextBoxes_.size()-1)
		indexNextBox_=nextBoxes_.size()-1;

	return nextBoxes_[indexNextBox_++];
};

box* argumentBox::getNextBox() {
	if(endOfArgument_) {
		argument_.reset(); // prepare for the next argument
		return box::getNextBox();
	}

	return this;
};

token argumentBox::parseToken() { // TODO move code to BASE class
	if(!argument_.ready())
		argument_.prepare();

	indexNextBox_=0; // offer nextBoxes from start again
	endOfArgument_=false;
	token nextToken=argument_.nextToken();

	if(nextToken==token::endOfBlock) {
		endOfArgument_=true;
		// EndOfArgument is for this layer to know it should continue
		return token::skip;
	}

	return nextToken;
};


// TOKENIZERBUILDER CLASS DEFINITION
token tokenizerBuilder::nextToken() {
	currString=it->second;
	return (it++)->first;
}

std::string tokenizerBuilder::flush() {
	return currString;
}

bool tokenizerBuilder::eof() {
	return it==tokens.end();
}

void tokenizerBuilder::addToken(token tok, std::string str) {
	tokens.emplace_back(tok, str);
}

void tokenizerBuilder::addTokens(const tokenlist& q) {
	for(auto&& tok:q) {
		tokens.push_back(tok);
	}
}
