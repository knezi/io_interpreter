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
struct traits {
	static obj_ptr print(obj_ptr scope, Arguments& args) {
		//TODO
		std::cout<<"PRINTING VALUE ";
		std::cout<<((t*)scope.get())->value;
		std::cout<<std::endl;
		return scope;
	}
};

template<>
struct traits<Bool> {
	static obj_ptr print(obj_ptr scope, Arguments& args);
};


obj_ptr cond(obj_ptr scope, Arguments& args);
obj_ptr while_(obj_ptr scope, Arguments& args);
obj_ptr for_(obj_ptr scope, Arguments& args);

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
};

inline obj_ptr createMethod(obj_ptr scope, Arguments& args) {
	auto func=std::make_shared<Function<Method>>(Method(args));
	return func;
}

template<typename t>
class PrimitiveType: public Object {
	public:
		PrimitiveType(): value{} {}; 
		PrimitiveType(t&& v): value{v} {}; 
		PrimitiveType(const t& v): value{v} {}; 

		PrimitiveType(PrimitiveType&& f) noexcept = default;
		PrimitiveType(const PrimitiveType& f) = delete;
		PrimitiveType& operator=(PrimitiveType&& f) noexcept = default;
		PrimitiveType& operator=(const PrimitiveType& f) = delete;
		virtual ~PrimitiveType() {};

		t value;

		obj_ptr clone() const override {
			obj_ptr new_obj=std::make_shared<PrimitiveType<t>>(value);
			cloneScope(new_obj);
			return new_obj;
		}
};

obj_ptr new_bool(bool val, obj_ptr upper);

template<typename t>
inline obj_ptr equality(obj_ptr scope, Arguments& args) {
	obj_ptr val=args.execute(scope);
	if(((t*)scope.get())->value==((t*)val.get())->value)
		return new_bool(true, scope->getUpperScope());
	return new_bool(false, scope->getUpperScope());
}

template<typename t, typename f>
class Operator {
	public:
		Operator(f comp_): comp{comp_} {};
		Operator(const Operator& o) = default;
		Operator(Operator&& o) = default;
		Operator& operator=(const Operator& o) = default;
		Operator& operator=(Operator&& o) = default;

		obj_ptr operator()(obj_ptr scope, Arguments& args) {
			obj_ptr val=args.execute(scope);
			if(comp(((t*)scope.get())->value, ((t*)val.get())->value))
				return new_bool(true, scope->getUpperScope());
			return new_bool(false, scope->getUpperScope());
		}

	private:
		f comp;
};

template<typename t>
using operator_t = builtins::Operator<Number, bool (*) (t, t)>;

template<typename t>
inline bool eq(t a, t b) {
	return a==b;
}

template<typename t>
inline bool neq(t a, t b) {
	return a!=b;
}

template<typename t>
inline bool gt(t a, t b) {
	return a>b;
}

template<typename t>
inline bool ge(t a, t b) {
	return a>=b;
}

template<typename t>
inline bool lt(t a, t b) {
	return a<b;
}

template<typename t>
inline bool le(t a, t b) {
	return a<=b;
}

template<typename t>
void add_operators(obj_ptr new_no) {
	std::vector<std::pair<std::string, bool (*) (t,t)>> ops =
				{{"==", eq<t>},
				{"!=", neq<t>},
				{"<", lt<t>},
				{"<=", le<t>},
				{">", gt<t>},
				{">=", ge<t>}};

	for(auto && op: ops) {
		new_no->addIntoSlot(op.first,
				std::make_shared<Function<operator_t<t>>>(operator_t<t>(op.second))); 
	}
}

inline obj_ptr new_number(int val, obj_ptr upper) {
	auto new_no=std::make_shared<builtins::Number>(val);
	new_no->addIntoSlot("print",
			std::make_shared<Function<func_ptr>>(traits<Number>::print));

	new_no->addIntoSlot("++",
			std::make_shared<Function<func_ptr>>(builtins::increment<Number>));

	new_no->addIntoSlot("+",
			std::make_shared<Function<func_ptr>>(builtins::plus<Number>));

	new_no->addIntoSlot("*",
			std::make_shared<Function<func_ptr>>(builtins::times<Number>));

	add_operators<int>(new_no);

	new_no->addUpperScope(upper);

	return new_no;
}

};
#endif
