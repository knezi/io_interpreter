#ifndef scope_hpp_
#define scope_hpp_

#include <map>
#include <iostream>
#include <memory>
#include <vector>

#include "tokenizer.hpp"

template<typename func>
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
		virtual ~Object() {};

		obj_ptr getSlot(const std::string& ObjectName);

		// TODO add to cpp
		template<typename str>
		void addIntoSlot(str&& ObjectName, obj_ptr obj) {
			Objects.insert({std::forward<str>(ObjectName),
					std::move(obj)});
		}

		virtual obj_ptr operator()(obj_ptr function_scope, Arguments& args);

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


template<typename func>
class Function: public Object {
	public:
		Function(func f_): f{f_}, Object{true} {}

		Function(Function&& f) = default;
		Function(const Function& f) = default;
		Function& operator=(Function&& f) = default;
		Function& operator=(const Function& f) = default;
		virtual ~Function() {};

		virtual obj_ptr operator()(obj_ptr function_scope, Arguments& args) override {
			return f(function_scope, args);
		}

	private:
		func f;
};



class Arguments {
	public:
		Arguments();
		Arguments(tokenizer& tok);

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
