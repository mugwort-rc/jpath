#ifndef __JPATH_20140531__
#define __JPATH_20140531__

#include <map>
#include <vector>

#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace client
{

  struct json_null
  {
    bool operator ==(const json_null &rhs) const;
  };

  typedef boost::variant<std::string, int, bool, json_null> json_value;
  typedef boost::variant<std::string, int> path_index;

  typedef std::vector<path_index> cond_expr_path;
  typedef std::pair<cond_expr_path, boost::optional<json_value>> cond_expr;

  struct json_path_node;
  typedef std::vector<json_path_node> json_path;

  struct json_path_node
  {
    path_index index;
    boost::optional<cond_expr> cond;

    bool operator ==(const json_path_node &rhs) const;
  };

} // ::client

client::json_path parse(const std::string &s);

#endif // __JPATH_20140531__
