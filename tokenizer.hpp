#ifndef tokenizer_hpp_
#define tokenizer_hpp_

#include <istream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <queue>


#ifdef DEBUG
#include <iostream>
#endif

class tokenizer;
class tokenizerBase;
class tokenizerBuilder;

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

class tokenizerBase {
	public:
		tokenizerBase() {};
		virtual ~tokenizerBase() {};
		virtual void prepare() = 0;
		virtual token nextToken() = 0;
		virtual std::string flush() = 0;
		virtual bool eof() = 0;
		virtual void reset() = 0;
		virtual bool ready() = 0;
};

class tokenizer: public tokenizerBase {
	public:
		tokenizer(processStream& in): in_{in}, tokenizerBase{} {};
		virtual ~tokenizer() = default;

		// this cannnot be called from the constructor to avoid indefinite
		// recursion tokenizer -> argumentBox -> tokenizer
		void prepare() override;

		token nextToken() override;

		std::string flush() override {
			return in_.flush();
		}

		bool eof() override {
			return in_.eof();
		}

		void reset() override {
			curr_=boxes_.find("WC1")->second.get();
		};

		bool ready() override {
			return ready_;
		}

	private:
		box* curr_; // last, already processed box
		std::map<std::string, std::unique_ptr<box> > boxes_;
		processStream& in_;
		bool ready_=false;
};

#include "scope.hpp"

using tokenlist=std::vector<std::pair<token, std::string>>;
class tokenizerBuilder: public tokenizerBase {
	public:
		tokenizerBuilder(tokenlist& tokens);
		tokenizerBuilder();

		tokenizerBuilder(const tokenizerBuilder& b) = default;
		tokenizerBuilder(tokenizerBuilder&& b) = default;
		tokenizerBuilder& operator=(const tokenizerBuilder& b) = default;
		tokenizerBuilder& operator=(tokenizerBuilder&& b) = default;

		~tokenizerBuilder() override = default;

		void prepare() override {};
		void reset() override {}; // in builder reset is trigged automatically
		bool ready() override { return true;} // no need for preparation

		token nextToken() override;
		std::string flush() override;
		bool eof() override;

		void addToken(token tok, const std::string& str);
		void addToken(token tok, std::string&& str);
		void addTokens(const tokenlist& q);
		void restart() {  it=tokens.begin(); };

	private:
		tokenlist tokens;
		tokenlist::iterator it;
		std::string currString;
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
