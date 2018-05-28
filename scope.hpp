#ifndef scope_hpp_
#define scope_hpp_

#include <iostream>
#include <map>
#include <memory>
#include <vector>


template<typename func>
class Function;
class Object;
class Arguments;


using obj_ptr=std::shared_ptr<Object>;
using func_ptr=obj_ptr (*) (obj_ptr, Arguments&);

class Object {
	public:
		Object(): callable{false} {}
		Object(bool c): callable{c} {}

	 	Object(Object&& f) = default;
		Object(const Object& f) = delete;
		Object& operator=(Object&& f) = default;
		Object& operator=(const Object& f) = delete;
		virtual ~Object() = default;

		obj_ptr getSlot(const std::string& ObjectName) const;

		template<typename str>
		void addIntoSlot(str&& ObjectName, obj_ptr obj) {
			Objects.insert_or_assign(std::forward<str>(ObjectName),
					std::move(obj));
		}

		virtual obj_ptr operator()(obj_ptr function_scope, Arguments& args);

		const bool callable;
		virtual obj_ptr clone() const;
		void addUpperScope(obj_ptr upper) {
			UpperScope=upper;
		}

		obj_ptr getUpperScope() const {
			return UpperScope;
		}

	private:
		std::map<std::string, obj_ptr> Objects;
		obj_ptr UpperScope;

	protected:
		void cloneScope(const obj_ptr& new_obj) const;
};


template<typename func>
class Function: public Object {
	public:
		Function(func f_): f{f_}, Object{true} {}

		Function(Function&& f) = default;
		Function(const Function& f) = delete;
		Function& operator=(Function&& f) = default;
		Function& operator=(const Function& f) = default;
		virtual ~Function() {};

		obj_ptr operator()(obj_ptr function_scope, Arguments& args) override {
			return f(function_scope, args);
		}

		obj_ptr clone() const override {
			obj_ptr new_obj=std::make_shared<Function<func>>(f);
			cloneScope(new_obj);
			return new_obj;
		}

	private:
		func f;
};


#include "tokenizer.hpp"

class Arguments {
	public:
		Arguments();
		Arguments(tokenizerBase& tok);
		void addTilClose(tokenizerBase &tok);

		Arguments(Arguments && a) = default;
		Arguments(const Arguments & a) = default;
		Arguments& operator=(Arguments && a) = default;
		Arguments& operator=(const Arguments & a) = default;

		obj_ptr execute(obj_ptr& scope);
		obj_ptr execute(obj_ptr& scope, Arguments& args_values);
		obj_ptr exec_curr_part(obj_ptr& scope);
		void next_argument();

		void addToken(token t, const std::string& s) {
			tokens.emplace_back(t, s);
		}

		void move() {
			++it;
		}

		token currToken() const {
			return it->first;
		}

		std::string flush() const {
			return it->second;
		}

		bool eof() const {
			return tokens.end()==it;
		}

		void restart() {
			it=tokens.begin();
		}

	private:
		std::vector<std::pair<token, std::string>> tokens;
		std::vector<std::pair<token, std::string>>::iterator it;
};

inline size_t symbolPriority(const std::string& sym) {
	if(sym=="==" || sym=="!=" || sym=="<" || sym=="<=" || sym==">" || sym==">=")
		return 1;

	if(sym=="+" || sym=="-")
		return 2;

	if(sym=="*" || sym=="/")
		return 3;

	
	// functions
	return 4;
}
#endif
