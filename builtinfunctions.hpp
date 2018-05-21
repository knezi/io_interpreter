#ifndef builtinfunctions_hpp_
#define builtinfunctions_hpp_

#include "scope.hpp"
#include "tokenizer.hpp"
#include "interpreter.hpp"

#include <memory>


namespace builtins {
template<typename t>
class PrimitiveType;
using Number=PrimitiveType<uint_least64_t>;
using Bool=PrimitiveType<bool>;

template<typename t>
// struct traits {
	inline obj_ptr print(obj_ptr scope, Arguments& args) {
		//TODO
		std::cout<<"PRINTING VALUE ";
		std::cout<<((t*)scope.get())->value;
		std::cout<<std::endl;
		return scope;
	}
// };

// template<Bool>
// struct traits {
	// inline obj_ptr print(obj_ptr scope, Arguments& args) {
		// //TODO
		// std::cout<<"VALUE ";
		// std::cout<<((Bool*)scope.get())->value;
		// std::cout<<std::endl;
		// return scope;
	// }
// };

template<typename t>
inline obj_ptr increment(obj_ptr scope, Arguments& args) {
	// TODO pretypovani
	++((t*)scope.get())->value;
	return scope;
}

template<typename t>
inline obj_ptr plus(obj_ptr scope, Arguments& args) {
	obj_ptr val=args.execute(scope);
	((t*)scope.get())->value+=((t*)val.get())->value;
	return scope;
}

template<typename t>
inline obj_ptr times(obj_ptr scope, Arguments& args) {
	obj_ptr val=args.execute(scope);
	((t*)scope.get())->value*=((t*)val.get())->value;
	return scope;
}

inline obj_ptr hello(obj_ptr scope, Arguments& args) {
	std::cout<<"HELLO WORLD"<<std::endl;
	return scope;
}

// creates method callable object
class Method {
	public:
		Method(Arguments&& args_): args{std::move(args_)} {};
		Method(const Arguments& args_): args{args_} {};

		obj_ptr operator()(obj_ptr scope, Arguments& args);

	private:
		Arguments args;
		std::vector<tokenlist> tokens;
};

inline obj_ptr createMethod(obj_ptr scope, Arguments& args) {
	auto func=std::make_shared<Function<Method>>(Method(args));
	return func;
}

template<typename t>
class PrimitiveType: public Object {
	public:
		PrimitiveType(): value{} { addBuiltIns(); }; 
		PrimitiveType(t&& v): value{v} { addBuiltIns(); }; 
		PrimitiveType(const t& v): value{v} { addBuiltIns(); }; 

		PrimitiveType(PrimitiveType&& f) noexcept = default;
		PrimitiveType(const PrimitiveType& f) = delete;
		PrimitiveType& operator=(PrimitiveType&& f) noexcept = default;
		PrimitiveType& operator=(const PrimitiveType& f) = delete;
		virtual ~PrimitiveType() {};

		t value;

		void addBuiltIns() {
			addIntoSlot("print", std::make_shared<Function<obj_ptr (*) (obj_ptr, Arguments&)>>(print<PrimitiveType<t>>));
			addIntoSlot("++", std::make_shared<Function<obj_ptr (*) (obj_ptr, Arguments&)>>(builtins::increment<PrimitiveType<t>>));
			addIntoSlot("+", std::make_shared<Function<obj_ptr (*) (obj_ptr, Arguments&)>>(builtins::plus<PrimitiveType<t>>));
			addIntoSlot("*", std::make_shared<Function<obj_ptr (*) (obj_ptr, Arguments&)>>(builtins::times<PrimitiveType<t>>));
		};

		obj_ptr clone() override {
			obj_ptr new_obj=std::make_shared<PrimitiveType<t>>(value);
			cloneScope(new_obj);
			return new_obj;
		}
};

};
#endif
