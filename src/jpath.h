#ifndef __JPATH_20140531__
#define __JPATH_20140531__

#include <map>
#include <vector>

#include <memory>

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

  struct cond_node
  {
    typedef std::shared_ptr<cond_node> node;

    typedef enum {
      VARIABLE,
      OPERATOR,
    } node_type;

    cond_node(node_type type);

    node_type type;
  };
  struct cond_variable_node : cond_node
  {
    cond_variable_node(const cond_expr &value);

    cond_expr value;

    inline static node create(const cond_expr &value)
    {
      return node(new cond_variable_node(value));
    }
  };
  struct cond_operator_node : cond_node
  {
    typedef enum {
      OR,
      AND,
    } cond_operator;

    cond_operator_node(cond_operator op, const node &left, const node &right);

    node left_node, right_node;
    cond_operator node_operator;

    inline static node and_node(const node &left, const node &right)
    {
      return node(new cond_operator_node(AND, left, right));
    }
    static node or_node(const node &left, const node &right)
    {
      return node(new cond_operator_node(OR, left, right));
    }
  };

  struct json_path_node;
  typedef std::vector<json_path_node> json_path;

  struct json_path_node
  {
    path_index index;
    boost::optional<cond_node::node> cond;

    bool operator ==(const json_path_node &rhs) const;
  };

} // ::client

client::json_path parse(const std::string &s);

#endif // __JPATH_20140531__
