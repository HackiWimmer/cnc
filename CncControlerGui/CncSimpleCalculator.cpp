#include <iomanip>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/karma.hpp>
#include <boost/phoenix.hpp>
#include "CncCommon.h"
#include "CncSimpleCalculator.h"

namespace 
{
	namespace qi    = boost::spirit::qi;
	namespace karma = boost::spirit::karma;
	namespace phx   = boost::phoenix;

	typedef std::function<double()> Value;
	
	#define BINARY_FUNCTOR(name, op)                        \
	struct name                                             \
	{                                                       \
	  name(Value x, Value y): x_(x), y_(y) {}               \
	  double operator()() { return x_() op y_(); }          \
	  Value x_, y_;                                         \
	}; 
	
	BINARY_FUNCTOR(ADD, +)
	BINARY_FUNCTOR(SUB, -)
	BINARY_FUNCTOR(MUL, *)
	BINARY_FUNCTOR(DIV, /)
	
	struct LIT
	{
	  LIT(double x): x_(x) {}
	  double operator()() { return x_; }
	  double x_;
	}; 
	
	struct NEG
	{
	  NEG(Value x): x_(x) {}
	  double operator()() { return -x_(); }
	  Value x_;
	}; 
	
	/////////////////////////////////////////////////////////////////////////////////
	template <typename It, typename Skipper = qi::space_type>
	struct parser 
	: qi::grammar<It, Value(), Skipper>
	{
		parser() : parser::base_type(expression)
		{
			using namespace qi;
			expression =
				term                  [_val = _1]
				>> *( ('+' >> term    [_val = phx::construct<ADD>(_val, _1)])
					| ('-' >> term    [_val = phx::construct<SUB>(_val, _1)])
					);

			term =
				factor                [_val = _1]
				>> *( ('*' >> factor  [_val = phx::construct<MUL>(_val, _1)])
					| ('/' >> factor  [_val = phx::construct<DIV>(_val, _1)])
					);

			factor =
				double_               [_val = phx::construct<LIT>(_1)]
				|   '(' >> expression [_val = _1] >> ')'
				|   ('-' >> factor    [_val = phx::construct<NEG>(_1)])
				|   ('+' >> factor    [_val = _1]);


			BOOST_SPIRIT_DEBUG_NODE(expression);
			BOOST_SPIRIT_DEBUG_NODE(term);
			BOOST_SPIRIT_DEBUG_NODE(factor);
		}

	  private:
		qi::rule<It, Value(), Skipper> expression, term, factor;
	};
};

////////////////////////////////////////////////////////////////////////////
CncSimpleCalculator::CncSimpleCalculator() 
////////////////////////////////////////////////////////////////////////////
{
}
////////////////////////////////////////////////////////////////////////////
bool CncSimpleCalculator::calculate(const wxString& expression, double& result) {
////////////////////////////////////////////////////////////////////////////
	std::string expr(expression);
	return calculate(expr, result);
}
////////////////////////////////////////////////////////////////////////////
bool CncSimpleCalculator::calculate(const std::string& expression, double& result) {
////////////////////////////////////////////////////////////////////////////
	bool ret = true;
	
	try
	{
		typedef std::string::const_iterator It;
		parser<It, qi::space_type> p;
		Value eval;
		
		auto f(begin(expression)), l(end(expression));
		
		if ( !qi::phrase_parse(f, l, p, qi::space, eval))
		{
			CNC_CERR_FUNCT_A(" parse failed: '%s'", std::string(f, l))
			ret = false;
		}
		
		if ( f != l ) 
		{
			CNC_CERR_FUNCT_A(" trailing unparsed: '%s'", std::string(f, l))
			ret = false;
		}
		
		result = eval();
	}
	catch (...)
	{
		CNC_CERR_FUNCT_A(" Unknown exception while parsing: %s ", expression)
		ret = false;
	}
	
	return ret;
}
