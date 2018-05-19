#ifndef scope_hpp_
#define scope_hpp_

#include <map>
#include <iostream>
#include <memory>

class Function;
class Object;
class Arguments;

typedef std::shared_ptr<Object> obj_ptr;

class Object {
	public:
		Object(): callable{false} {}
		Object(bool c): callable{c} {}

		Object(Object&& f) = default;
		Object(const Object& f) = default;
		Object& operator=(Object&& f) = default;
		Object& operator=(const Object& f) = default;

		obj_ptr getSlot(const std::string& ObjectName) {
			auto slot=Objects.find(ObjectName);
			if(slot==Objects.end())
				return nullptr;
			return slot->second;
		}

		template<typename str>
		void addIntoSlot(str&& ObjectName, obj_ptr obj) {
			Objects.insert({std::forward<str>(ObjectName),
					std::move(obj)});
		}


		virtual obj_ptr operator()(obj_ptr function_scope, Arguments& args) {
			std::cerr<<"Not callable"<<std::endl;
			// TODO RETURN
			return std::shared_ptr<Object>(this);
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
		std::map<std::string, obj_ptr> Objects;
		// Object tmp_copy;
		// bool tmp_copy_valid;

};


class Function: public Object {
	public:
		Function(): Object{true} {}

		Function(Function&& f) = default;
		Function(const Function& f) = default;
		Function& operator=(Function&& f) = default;
		Function& operator=(const Function& f) = default;

		virtual obj_ptr operator()(obj_ptr function_scope, Arguments& args) override {
			std::cout<<"FUNCTION"<<callable;
			return function_scope;
		}

};

template<typename t>
class Print: public Function {
	public:

		virtual obj_ptr operator()(obj_ptr function_scope, Arguments& args) override {
			//TODO
			std::cout<<"PRINTING VALUE ";
			std::cout<<((t*)function_scope.get())->value;
			std::cout<<std::endl;
			return function_scope;
		}

};

// TODO tohle predelej do standardni metody pres konstruktor funtion
template<typename t>
class Increment: public Function {
	public:
		virtual obj_ptr operator()(obj_ptr function_scope, Arguments& args) override {
			// TODO pretypovani
			++((t*)function_scope.get())->value;
			return function_scope;
		}

};

template<typename t>
class PrimitiveType: public Object {
	public:
		PrimitiveType(): value{} { addBuiltIns(); }; 
		PrimitiveType(t&& v): value{v} { addBuiltIns(); }; 
		PrimitiveType(const t& v): value{v} { addBuiltIns(); }; 

		PrimitiveType(PrimitiveType&& f) = default;
		PrimitiveType(const PrimitiveType& f) = default;
		PrimitiveType& operator=(PrimitiveType&& f) = default;
		PrimitiveType& operator=(const PrimitiveType& f) = default;

		t value;

		void addBuiltIns() {
			addIntoSlot("print", std::make_shared<Print<PrimitiveType<t>>>());
			addIntoSlot("++", std::make_shared<Increment<PrimitiveType<t>>>());
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
