#ifndef scope_hpp_
#define scope_hpp_

#include <map>
#include <iostream>
#include <memory>

class Function;
class Object;
class Arguments;

class Object {
	public:
		Object(): callable{false} {}
		Object(bool c): callable{c} {}

		Object(Object&& f) = default;
		Object(const Object& f) = default;
		Object& operator=(Object&& f) = default;
		Object& operator=(const Object& f) = default;

		Object* getSlot(const std::string& ObjectName) {
			// TODO create a NULL Object
			return (Objects.find(ObjectName)->second).get();
		}

		template<typename str>
		void addIntoSlot(str&& ObjectName, std::unique_ptr<Object> obj) {
			Objects.insert({std::forward<str>(ObjectName),
					std::move(obj)});
		}


		virtual Object* operator()(Arguments& args) {
			std::cerr<<"Not callable"<<std::endl;
			return this;
		}

		const bool callable;

		// Object& getNextObject() {
			// // TODO zinvalidovat??
			// if(tmp_copy_valid)
				// return tmp_copy;
			// return *this;
		// }

		// void setNextObject(Object && obj) {
			// tmp_copy_valid=true;
			// tmp_copy=obj;
		// }

		// void setNextObject(const Object & obj) {
			// tmp_copy_valid=true;
			// tmp_copy=obj;
		// }

	private:
		std::map<std::string, std::unique_ptr<Object>> Objects;
		// Object tmp_copy;
		// bool tmp_copy_valid;

};


class Function: public Object {
	public:
		Function(Object& scope_) : scope{scope_}, Object{true} {}

		Function(Function&& f) = default;
		Function(const Function& f) = default;
		Function& operator=(Function&& f) = default;
		Function& operator=(const Function& f) = default;

		virtual Object* operator()(Arguments& args) override {
			std::cout<<"FUNCTION"<<callable;
			return this;
		}

	protected:
		Object& scope;

};

template<typename t>
class Print: public Function {
	public:
		Print(Object& scope): Function(scope) {}

		virtual Object* operator()(Arguments& args) override {
			//TODO
			std::cout<<"PRINTING VALUE ";
			std::cout<<((t&)scope).value;
			return &scope;
		}

};

// TODO tohle predelej do standardni metody pres konstruktor funtion
template<typename t>
class Increment: public Function {
	public:
		Increment(Object& scope): Function(scope) {}
		virtual Object* operator()(Arguments& args) override {
			++((t&)scope).value;
			return &scope;
		}

};

template<typename t>
class PrimitiveType: public Object {
	public:
		PrimitiveType(): value{} { addBuiltIns(); }; 
		PrimitiveType(t&& v): value{std::move(v)} { addBuiltIns(); }; 
		PrimitiveType(const t& v): value{v} { addBuiltIns(); }; 

		PrimitiveType(PrimitiveType&& f) = default;
		PrimitiveType(const PrimitiveType& f) = default;
		PrimitiveType& operator=(PrimitiveType&& f) = default;
		PrimitiveType& operator=(const PrimitiveType& f) = default;

		t value;

	private:
		void addBuiltIns() {
			addIntoSlot("print", std::make_unique<Print<PrimitiveType<t>>>(*this));
			addIntoSlot("++", std::make_unique<Increment<PrimitiveType<t>>>(*this));
		}
};

typedef PrimitiveType<uint_least64_t> Number;



class Arguments {
	public:
		Arguments() {
			tokens.emplace_back(token::openArguments, "(");
			tokens.emplace_back(token::closeArguments, ")");
		}

		Arguments(tokenizer& tok) {
			token currToken;
			do {
				currToken=tok.nextToken();
				tokens.emplace_back(currToken, tok.flush());
			} while(currToken!=token::closeArguments);
		}

		token nextToken() {
			return tokens[i++].first;
		}

		std::string flush() {
			// TODO dirty
			return tokens[i-1].second;
		}

		bool eof() {
			return tokens.size()==i;
		}

	private:
		std::vector<std::pair<token, std::string>> tokens;
		size_t i;
};
#endif
