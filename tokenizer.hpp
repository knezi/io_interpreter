#ifndef tokenizer_hpp_
#define tokenizer_hpp_

#include <istream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#ifdef DEBUG
#include <iostream>
#endif


// STREAM

enum class charGroup {
	// value is ASCII of one of the representants
	alphanum,
	semicolon,
	newline,
	whitespace,
	op, // operator - does not include slash '/'
	slash,
	backslash,
	quote,
	comma,
	open,
	close,
	eof,
	invalid
};

// after construct - getCurrent gives the first char
class processStream {
	public:
		processStream(std::istream & in): in_(in) {
			in_>>std::noskipws>>curr_;

#ifdef DEBUG
			std::cout<<"DEBUG: PROCESS STREAM: EOF? "<<in_.eof()<<" "<<curr_<<std::endl;
#endif
		};

		charGroup getCurrent() const;
		void move();
		void moveBack();
		std::string flush();
		bool eof() {
			return eof_;
		}

	private:
		std::istream & in_;
		char curr_=0;
		bool eof_=false;

		std::vector<char> ops_={':', '.', '\'', '~', '!', '@', '$', // missing slash, backslash!!
			'%', '^', '&', '*', '-', '+', '=', '{', '}',  
			'[', ']', ',', '<', '>', '?'};

		std::string buffer_;
};


// BOXES
enum class token {
	symbol,				// 0
	skip, 				// 1
	argument,			// 2
	terminator,			// 3
	eof,				// 4
	endOfArgument,		// 5
	invalidCharacter	// 6
};

using parseTokenSignature = token (*) (processStream &);

class tokenBox {
	public:
		tokenBox(processStream& in, parseTokenSignature parseTokenFunc):
			in_(in),
			parseToken_(parseTokenFunc) {
			// nextBoxes_=std::vector<tokenBox>();
		};

		void addNextBox(tokenBox* box) {
			nextBoxes_.push_back(box);
		};

		tokenBox* getNextBox() {
			// if(indexNextBox // TODO check boundaries
			return nextBoxes_[indexNextBox_++];
		};

		token parseToken() {
			indexNextBox_=0; // offer nextBoxes from start again
			return parseToken_(in_);
		}

	private:
		std::vector<tokenBox* > nextBoxes_;
		processStream& in_;
		std::size_t indexNextBox_=0;
		parseTokenSignature parseToken_;
};


// tokenizer - returns token by token
// Boxflow
//        |---> EOF
//        |
// WC1 -->+---> Terminator --> back to WC1
//        |
//        |---> Symbol -> WC2 -+-> Open -- NWC1 -> Argument -> NWC2 --+-> Close --+
//                             |         ^                            |           |
//                     WC1 <---+         +-<---------- Comma <--------+    WC1 <--+
// Next BOX is always unambiguously defined - not true!! TODO

class tokenizer {
	public:
		tokenizer(processStream& in): in_(in) {
			prepareBoxes();
		};

		void prepareBoxes();

		token nextToken();

	private:
		tokenBox* curr_; // there is last used box
		std::map<std::string, std::unique_ptr<tokenBox> > boxes_;
		processStream& in_;

};

#endif
