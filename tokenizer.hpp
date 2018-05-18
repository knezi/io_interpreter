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
	hash,
	star,
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
			'%', '^', '&', '-', '+', '=', '{', '}',  
			'[', ']', ',', '<', '>', '?'};

		std::string buffer_;
};


// BOXES
enum class token {
	symbol,				// 0
	skip, 				// 1
	argument,			// 2
	terminator,			// 3
	nextArgument,		// 4
	endOfBlock,			// 5
	openArguments,		// 6
	closeArguments,		// 7
	invalidCharacter	// 8
};

using parseTokenSignature = token (*) (processStream &);

class box {
	public:
		box(processStream& in): in_(in) {};
		virtual ~box() noexcept {};


		void addNextBox(box* box) {
			nextBoxes_.push_back(box);
		};

		virtual box* getNextBox();

		virtual token parseToken() = 0;

	protected:
		std::vector<box* > nextBoxes_;
		processStream& in_;
		std::size_t indexNextBox_=0;
};

class tokenBox: public box {
	public:
		tokenBox(processStream& in, parseTokenSignature parseTokenFunc):
			box(in),
			parseToken_(parseTokenFunc) {};

		virtual box* getNextBox() {
			return box::getNextBox();
		};

		virtual token parseToken() {
			indexNextBox_=0; // offer nextBoxes from start again
			return parseToken_(in_);
		};

	private:
		parseTokenSignature parseToken_;
};



// tokenizer - returns token by token
// Boxflow
// box is a group of states in DFA
//        |---> BlockEnd
//        |
// WC1 -->+---> Terminator --> back to WC1
//        |
//        |---> Symbol -> WC2 -+-> Open -- NWC1 -> Argument -> NWC2 --+-> Close --+
//                             |         ^                            |           |
//                     WC1 <---+         +-<---------- Comma <--------+    WC1 <--+
// Next BOX is always unambiguously defined - not true!! TODO

class tokenizer {
	public:
		tokenizer(processStream& in): in_(in) {};

		// this cannnot be called from the constructor to avoid indefinite
		// recursion tokenizer -> argumentBox -> tokenizer
		void prepare();

		token nextToken();
		token futureToken();

		std::string flush() {
			return in_.flush();
		}

		bool eof() {
			return in_.eof();
		}

		void reset() {
			curr_=boxes_.find("WC1")->second.get();
		};

		bool ready() {
			return ready_;
		}

	private:
		box* curr_; // last, already processed box
		std::map<std::string, std::unique_ptr<box> > boxes_;
		processStream& in_;
		bool ready_=false;
};


class argumentBox: public box {
	public:
		argumentBox(processStream& in):
			box(in),
			argument_(in) {};

		virtual box* getNextBox();
		virtual token parseToken();

	private:
		bool endOfArgument_=false;
		tokenizer argument_;
};
#endif
