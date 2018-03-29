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
	endOfArgument,		// 4
	openArgument,		// 5
	closeArgument,		// 6
	invalidCharacter	// 7
};

using parseTokenSignature = token (*) (processStream &);

class box {
	public:
		box(processStream& in): in_(in) {};
		virtual ~box() noexcept {};


		void addNextBox(box* box) {
			nextBoxes_.push_back(box);
		};

		virtual box* getNextBox() = 0;
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
			// if(indexNextBox // TODO check boundaries
			return nextBoxes_[indexNextBox_++];
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


// TODO change order??
// TODO EVERYWHERE inline functions, move code to CPP
class argumentBox: public box {
	public:
		argumentBox(processStream& in):
			box(in),
			argument_(in) {};

		virtual box* getNextBox() { // TODO move code to base class
			// if(indexNextBox // TODO check boundaries
			if(endOfArgument_) {
				argument_.reset(); // prepare for the next argument
				return nextBoxes_[indexNextBox_++];
			}

			return this;
		};

		virtual token parseToken() { // TODO move code to BASE class
			if(!argument_.ready())
				argument_.prepare();

			indexNextBox_=0; // offer nextBoxes from start again
			endOfArgument_=false;
			token nextToken=argument_.nextToken();

			if(nextToken==token::endOfArgument) {
				endOfArgument_=true;
				// EndOfArgument is for this layer to know it should continue
				return token::skip;
			}

			return nextToken;
		};

	private:
		bool endOfArgument_=false;
		tokenizer argument_;
};
#endif
