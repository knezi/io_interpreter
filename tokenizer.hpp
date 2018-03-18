#ifndef tokenizer_hpp_
#define tokenizer_hpp_

#include <istream>

#ifdef DEBUG
#include <iostream>
#endif

enum charGroup {
	// value is ASCII of one of the representants
	alphanum,
	terminator,
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

class processStream {
	public:
		processStream(std::istream & in): in_(in) {
			in_>>std::noskipws;
		};

		charGroup getCurrent();
		void move();

	private:
		std::istream & in_;
		char curr_=0;
		bool eof_=false;

		std::vector<char> ops_={':', '.', '\'', '~', '!', '@', '$', // missing slash, backslash!!
			'%', '^', '&', '*', '-', '+', '=', '{', '}',  
			'[', ']', ',', '<', '>', '?'};
};

#endif
