#ifndef LAMBDA_H
#define LAMBDA_H

#include <iostream>

// For ostream manip funcs
typedef std::ostream&(*iomanipFunc)(std::ostream&);

#define MAKE_TRAITS_1(TYPE1, TYPE2, RESULT_TYPE) \
template<> \
struct LambdaTrait<TYPE1, TYPE2> \
{ \
	typedef RESULT_TYPE Type; \
};

#define MAKE_TRAITS_2(TYPE1, TYPE2, RESULT_TYPE) \
template<> \
struct LambdaTrait<TYPE1, TYPE2> \
{ \
	typedef RESULT_TYPE Type; \
};

// Traits - Default, use LHS (T1)
template <class T1, class T2>
struct LambdaTrait
{
	typedef T1 Type;	// Use T1 as default
};

// Make additional Traits
MAKE_TRAITS_1(int, int, int)			// int + int = int
MAKE_TRAITS_2(int, float, float)		// int + float = float && float + int = float
MAKE_TRAITS_2(int, double, double)		// etc..
MAKE_TRAITS_2(float, double, double)	// etc..

// Special cases for ostreams
template <class T2>
struct LambdaTrait<std::ostream, T2>
{
	typedef std::ostream& Type;
};

template <class T2>
struct LambdaTrait<T2, std::ostream>
{
	typedef std::ostream& Type;
};

template <>
struct LambdaTrait<std::ostream, std::ostream>
{
	typedef std::ostream& Type;
};

// The operator we call for an expression
// operator() can't be static inline, so use DoOP
template <class T1, class T2>
class LambdaAssignOP
{
public:
	typedef typename LambdaTrait<T1, T2>::Type Type;
	static inline Type DoOP(T1& a, T2& b) { a = (T1)b; return a; }
	static inline Type DoOP(T1& a, T2 b) { a = (T1)b; return a; }
};

template <class T1, class T2>
class LambdaSumOP
{
public:
	typedef typename LambdaTrait<T1, T2>::Type Type;
	static inline Type DoOP(T1 a, T2 b) { return a + b; }
};

template <class T1, class T2>
class LambdaMinusOP
{
public:
	typedef typename LambdaTrait<T1, T2>::Type Type;
	static inline Type DoOP(T1 a, T2 b) { return a - b; }
};

template <class T1, class T2>
class LambdaMultOP
{
public:
	typedef typename LambdaTrait<T1, T2>::Type Type;
	static inline Type DoOP(T1 a, T2 b) { return a * b; }
};

template <class T1, class T2>
class LambdaDivOP
{
public:
	typedef typename LambdaTrait<T1, T2>::Type Type;
	static inline Type DoOP(T1 a, T2 b) { return a / b; }
};

template <class T2>
class LambdaOStreamOP
{
public:
	typedef typename LambdaTrait<std::ostream, T2>::Type Type;
	static inline Type DoOP(std::ostream& os, T2 b)
	{
		os << b;
		return os;
	}
};

// Holds a the _1 variable
class LambdaVariable
{
public:
	typedef int Type;
	Type& operator()(Type& x)
	{
		return x;
	}
};

// Class remembers a literal in the expression, i.e the 1 in _1 + 1
template <class T1>
class LambdaLiteral
{
public:
	typedef T1 Type;
	LambdaLiteral(const T1 x) :
		_l(x)
	{

	}
	T1 operator()(int)
	{
		return _l;
	}

private:
	T1 _l;
};

// Holds ostream
class LambdaStream
{
public:
	typedef std::ostream& Type;
	LambdaStream(std::ostream& o) :
		_o(o)
	{
		
	}
	std::ostream& operator()(int)
	{
		return _o;
	}

private:
	std::ostream& _o;
};

// Binary expression
template <class L, class R, class OP>
class LambdaBinaryExpression
{
public:
	typedef typename LambdaTrait<typename L::Type, typename R::Type>::Type Type;
	LambdaBinaryExpression(const L& l, const R& r) :
		_l(l), _r(r)
	{
		
	}
	Type operator()(int& x)
	{
		return OP::DoOP(_l(x), _r(x));
	}

private:
	L _l;
	R _r;
};

// Contains an expression (wrapped)
template <class E>
class LambdaExpression
{
public:
	typedef typename E::Type Type;
	LambdaExpression(const E& e) :
		_expr(e)
	{
		
	}
	Type operator()(int& x)
	{
		return _expr(x);
	}

private:
	E _expr;
};

// Special case for LambdaVariable where operator() wants to return a reference
// Also LambdaVariable can have an assignment operator on the LHS
// i.e 
// _1 = 2
// _1 = <expr>
template <>
class LambdaExpression<LambdaVariable>
{
public:
	typedef LambdaVariable::Type Type;
	LambdaExpression(const LambdaVariable& e) :
		_expr(e)
	{
		
	}
	Type& operator()(int& x)
	{
		return _expr(x);
	}

	// The assign operator for LambdaExpression
	template <class R>
	LambdaExpression<LambdaBinaryExpression<LambdaExpression<LambdaVariable>, LambdaExpression<R>, LambdaAssignOP<Type, typename R::Type>>>
		operator=(const LambdaExpression<R>& r)
	{
		typedef LambdaBinaryExpression<LambdaExpression<LambdaVariable>, LambdaExpression<R>, LambdaAssignOP<Type, typename R::Type>> LambdaExpression_t;
		return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(*this, r));
	}

	// The assign operator for literal
	template <class R>
	LambdaExpression<LambdaBinaryExpression<LambdaExpression<LambdaVariable>, LambdaExpression<R>, LambdaAssignOP<Type, R>>>
		operator=(const R x)
	{
		typedef LambdaBinaryExpression<LambdaExpression<LambdaVariable>, LambdaExpression<R>, LambdaAssignOP<Type, R>> LambdaExpression_t;
		LambdaLiteral<R> r(x);
		return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(*this, r));
	}

private:
	LambdaVariable _expr;
};

// Special case for lambda stream
template <>
class LambdaExpression<LambdaStream>
{
public:
	typedef LambdaStream::Type Type;
	LambdaExpression(const LambdaStream& e) :
		_expr(e)
	{
		
	}
	std::ostream& operator()(int& x)
	{
		return _expr(x);
	}

private:
	LambdaStream _expr;
};

// cout << EXPR
template <class R>
LambdaExpression<LambdaBinaryExpression<LambdaExpression<LambdaStream>, LambdaExpression<R>, LambdaOStreamOP<typename R::Type>>>
operator<<(std::ostream& o, const LambdaExpression<R>& r)
{
	typedef LambdaBinaryExpression<LambdaExpression<LambdaStream>, LambdaExpression<R>, LambdaOStreamOP<typename R::Type>> LambdaExpression_t;
	LambdaStream l(o);
	return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r));
}

// EXPR << literal

template <class L, class R>
LambdaExpression<LambdaBinaryExpression<LambdaExpression<L>, LambdaLiteral<R>, LambdaOStreamOP<R>>>
operator<<(const LambdaExpression<L>& l, R x)
{
	typedef LambdaBinaryExpression<LambdaExpression<L>, LambdaLiteral<R>, LambdaOStreamOP<R>> LambdaExpression_t;
	LambdaLiteral<R> r(x);
	return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r));
}

// EXPR << manipFunc
template <class L>
LambdaExpression<LambdaBinaryExpression<LambdaExpression<L>, LambdaLiteral<iomanipFunc>, LambdaOStreamOP<iomanipFunc>>>
operator<<(const LambdaExpression<L>& l, iomanipFunc x) {
	typedef LambdaBinaryExpression<LambdaExpression<L>, LambdaLiteral<iomanipFunc>, LambdaOStreamOP<iomanipFunc>> LambdaExpression_t;
	LambdaLiteral<iomanipFunc> r(x);
	return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r));
};

// Creating an operation is standard bar which operator to overload
// and which evaluation object to use
#define CREATE_OP(LAMBDA_OP, OP) \
template <class L, class R> \
LambdaExpression<LambdaBinaryExpression<LambdaExpression<L>, LambdaExpression<R>, LAMBDA_OP<typename L::Type, typename R::Type>>> \
OP(const LambdaExpression<L>& l, const LambdaExpression<R>& r) \
{ \
	typedef LambdaBinaryExpression<LambdaExpression<L>, LambdaExpression<R>, LAMBDA_OP<typename L::Type, typename R::Type> > LambdaExpression_t; \
    return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); \
}; \
\
template <class L, class R> \
LambdaExpression<LambdaBinaryExpression<LambdaExpression<L>, LambdaExpression<LambdaLiteral<R> >, LAMBDA_OP<typename L::Type, R> > > \
OP(const LambdaExpression<L>& l, const R x) \
{ \
    typedef LambdaBinaryExpression<LambdaExpression<L>, LambdaExpression<LambdaLiteral<R> >, LAMBDA_OP<typename L::Type, R> > LambdaExpression_t; \
    LambdaLiteral<R> r(x); \
    return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); \
}; \
\
template <class L, class R> \
LambdaExpression<LambdaBinaryExpression<LambdaExpression<LambdaLiteral<L> >, LambdaExpression<R>, LAMBDA_OP<L, typename R::Type> > > \
OP(const L x, const LambdaExpression<R>& r) \
{ \
    typedef LambdaBinaryExpression<LambdaExpression<LambdaLiteral<L> >, LambdaExpression<R>, LAMBDA_OP<L, typename R::Type> > LambdaExpression_t; \
    LambdaLiteral<L> l(x); \
    return LambdaExpression<LambdaExpression_t>(LambdaExpression_t(l, r)); \
};


CREATE_OP(LambdaSumOP, operator+)
CREATE_OP(LambdaMinusOP, operator-)
CREATE_OP(LambdaMultOP, operator*)
CREATE_OP(LambdaDivOP, operator/)


// Initialize out _1 object, it should act like a variable
LambdaVariable __1;
LambdaExpression<LambdaVariable> _1(__1);

#endif