#include "jpath.h"

#include <boost/fusion/adapted.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace client
{

  // struct json_null
  bool json_null::operator ==(const json_null &rhs) const
  {
    return true;
  }

  // struct json_path_node
  bool json_path_node::operator ==(const json_path_node &rhs) const
  {
    bool cond_eq = (bool(this->cond) == bool(rhs.cond));
    if ( cond_eq )
    {
      cond_eq = *this->cond == *rhs.cond;
    }
    return this->index == rhs.index && cond_eq;
  }

} // ::client

BOOST_FUSION_ADAPT_STRUCT(
  client::json_path_node,
  (client::path_index, index)
  (boost::optional<client::cond_expr>, cond)
)

template <typename Iterator>
struct json_path_grammar :
  boost::spirit::qi::grammar<Iterator, client::json_path()>
{
  json_path_grammar() :
    json_path_grammar::base_type(path)
  {

    quoted_char  = (boost::spirit::qi::char_('\\') >> '"') [boost::spirit::_val+='"']
                 | (boost::spirit::qi::char_ - '"') [boost::spirit::_val+=boost::spirit::qi::_1]
                 ;

    string_value = '"' >> *quoted_char >> '"';

    integer_value = boost::spirit::qi::int_;

    boolean_value = boost::spirit::qi::lit("true") [boost::spirit::_val=true]
                  | boost::spirit::qi::lit("false") [boost::spirit::_val=false]
                  ;

    null_value = boost::spirit::qi::lit("null") [boost::spirit::_val=client::json_null()];

    node = *(integer_value | string_value);

    cond_value = integer_value
               | boolean_value
               | null_value
               | string_value
               ;

    cond_rhs = '=' >> cond_value;

    cond_path = node % '/';

    expr = cond_path >> -cond_rhs;

    cond = '[' >> expr >> ']';

    path_node = node >> -cond;

    path = path_node % '/';

  }

private:
  boost::spirit::qi::rule<Iterator, std::string()>
    string_value, quoted_char;

  boost::spirit::qi::rule<Iterator, int()>
    integer_value;

  boost::spirit::qi::rule<Iterator, bool()>
    boolean_value;

  boost::spirit::qi::rule<Iterator, client::json_null()>
    null_value;

  boost::spirit::qi::rule<Iterator, client::path_index()>
    node;

  boost::spirit::qi::rule<Iterator, client::json_value()>
    cond_rhs, cond_value;

  boost::spirit::qi::rule<Iterator, client::cond_expr_path()>
    cond_path;

  boost::spirit::qi::rule<Iterator, client::cond_expr()>
    cond, expr;

  boost::spirit::qi::rule<Iterator, client::json_path_node()>
    path_node;

  boost::spirit::qi::rule<Iterator, client::json_path()>
    path;

};

client::json_path parse(const std::string &s)
{
  typedef std::string::const_iterator iterator_type;
  typedef json_path_grammar<iterator_type> Grammar;
  Grammar path;

  std::string::const_iterator s_begin = s.begin();
  std::string::const_iterator s_end = s.end();

  client::json_path result;
  bool r = boost::spirit::qi::parse(s_begin, s_end, path, result);
  if ( ! r )
  {
    return client::json_path();
  }
  else
  {
    return result;
  }
}

