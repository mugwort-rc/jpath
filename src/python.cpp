#include "jpath.h"

#include <boost/algorithm/string.hpp>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

struct cond_node_def_visitor : boost::python::def_visitor<cond_node_def_visitor>
{
  friend class def_visitor_access;

  template <class T>
  void visit(T &class_) const
  {
    class_
      .def("__getattr__", &getattr)
      ;
  }

  static boost::python::object
    getattr(client::cond_node &self, const std::string &key)
  {
    std::string name;
    auto *ptr = &self;
    if ( self.type == client::cond_node::VARIABLE )
    {
      client::cond_variable_node *vptr
        = static_cast<client::cond_variable_node*>(ptr);
      if ( key == "value" )
      {
        return boost::python::object(vptr->value);
      }
      name = "cond_variable_node";
    }
    else if ( self.type == client::cond_node::OPERATOR )
    {
      client::cond_operator_node *optr
        = static_cast<client::cond_operator_node*>(ptr);
      if ( key == "operator" )
      {
        return boost::python::object(optr->node_operator);
      }
      else if ( key == "left" )
      {
        return boost::python::object(optr->left_node);
      }
      else if ( key == "right" )
      {
        return boost::python::object(optr->right_node);
      }
      name = "cond_operator_node";
    }
    std::string msg = "'" + name + "' object has no attribute '" + key + "'";
    PyErr_SetString(PyExc_AttributeError, msg.c_str());
    throw boost::python::error_already_set();
  }

};

struct path_index_to_python_converter
{
  static PyObject *convert(const client::path_index &index)
  {
    boost::python::object result;
    if ( index.type() == typeid(int) )
    {
      // int
      result = boost::python::object(boost::get<int>(index));
    }
    else if ( index.type() == typeid(std::string) )
    {
      // std::string
      result = boost::python::object(boost::get<std::string>(index));
    }
    return boost::python::incref(result.ptr());
  }
};

struct optional_cond_node_to_python_converter
{
  static PyObject *convert(const boost::optional<client::cond_node::node> &cond_node)
  {
    boost::python::object result;
    if ( cond_node )
    {
      result = boost::python::object(*cond_node);
    }
    return boost::python::incref(result.ptr());
  }
};

struct cond_expr_to_python_converter
{
  static PyObject *convert(const client::cond_expr &cond_expr)
  {
    boost::python::object result;
    auto path  = cond_expr.first;
    auto value = cond_expr.second;
    if ( value )
    {
      result = boost::python::make_tuple(path, *value);
    }
    else
    {
      result = boost::python::make_tuple(path);
    }
    return boost::python::incref(result.ptr());
  }
};

struct json_value_to_python_converter
{
  static PyObject *convert(const client::json_value &value)
  {
    boost::python::object result;
    if ( value.type() == typeid(int) )
    {
      // int
      result = boost::python::object(boost::get<int>(value));
    }
    else if ( value.type() == typeid(std::string) )
    {
      // std::string
      result = boost::python::object(boost::get<std::string>(value));
    }
    else if ( value.type() == typeid(bool) )
    {
      // bool
      result = boost::python::object(boost::get<bool>(value));
    }
    return boost::python::incref(result.ptr());
  }
};

template <class T>
std::string to_string(T &variant)
{
  if ( variant.type() == typeid(int) )
  {
    return boost::lexical_cast<std::string>(boost::get<int>(variant));
  }
  else if ( variant.type() == typeid(bool) )
  {
    return boost::get<bool>(variant) ? "true" : "false";
  }
  else if ( variant.type() == typeid(client::json_null) )
  {
    return "null";
  }
  else
  {
    return "\"" + boost::algorithm::replace_all_copy(
                    boost::get<std::string>(variant), "\"", "\\\"") + "\"";
  }
}

std::string cond_node_repr(client::cond_node &self)
{
  std::string result;
  if ( self.type == client::cond_node::VARIABLE )
  {
    auto *vptr = static_cast<client::cond_variable_node*>(&self);
    auto value = vptr->value;
    for ( auto p : value.first )
    {
      if ( ! result.empty() )
      {
        result += "/";
      }
      result += to_string(p);
    }
    if ( value.second )
    {
      result += "=" + to_string(*value.second);
    }
  }
  else if ( self.type == client::cond_node::OPERATOR )
  {
    auto *optr = static_cast<client::cond_operator_node*>(&self);
    std::string left, right, oper;
    left = cond_node_repr(*optr->left_node.get());
    right = cond_node_repr(*optr->right_node.get());
    if ( optr->node_operator == client::cond_operator_node::AND )
    {
      oper = "AND";
    }
    else if ( optr->node_operator == client::cond_operator_node::OR )
    {
      oper = "OR";
    }
    result = oper + "(" + left + ", " + right + ")";
  }
  return result;
}

std::string jpath_node_repr(client::json_path_node &self)
{
  std::string path, cond;
  path = to_string(self.index);
  if ( self.cond )
  {
    cond = cond_node_repr(*(*self.cond).get());
  }
  if ( cond.empty() )
  {
    return path;
  }
  else
  {
    return path + "[" + cond + "]";
  }
}

BOOST_PYTHON_MODULE(__jpath)
{
  boost::python::def("parse", parse);

  // client::json_path
  boost::python::class_<client::json_path>("jpath")
    .def(boost::python::vector_indexing_suite<client::json_path>())
    ;

  // client::json_path_node
  boost::python::class_<client::json_path_node>("jpath_node")
    .def("__repr__", &jpath_node_repr)
    .add_property("path_index",
      boost::python::make_getter(&client::json_path_node::index,
        boost::python::return_value_policy<boost::python::return_by_value>()))
    .add_property("cond",
      boost::python::make_getter(&client::json_path_node::cond,
        boost::python::return_value_policy<boost::python::return_by_value>()))
    ;

  // client::path_index
  boost::python::to_python_converter<client::path_index,
    path_index_to_python_converter>();

  // boost::optional
  //   <client::cond_node::node>
  boost::python::to_python_converter<boost::optional<client::cond_node::node>,
    optional_cond_node_to_python_converter>();

  // client::cond_node
  auto cond_node = boost::python::class_<client::cond_node,
      client::cond_node::node
    >("cond_node", boost::python::no_init)
    .def(cond_node_def_visitor())
    .def("__repr__", &cond_node_repr)
    .add_property("type", boost::python::make_getter(&client::cond_node::type))
    ;
  {
    boost::python::scope cond_node_scope = cond_node;
    boost::python::enum_<client::cond_node::node_type>("node_type")
      .value("VARIABLE", client::cond_node::VARIABLE)
      .value("OPERATOR", client::cond_node::OPERATOR)
      .export_values()
      ;
  }

  // client::cond_variable_node
  boost::python::class_<client::cond_variable_node,
      boost::noncopyable,
      boost::python::bases<client::cond_node>
    >("cond_variable_node", boost::python::no_init)
    .add_property("value",
      boost::python::make_getter(&client::cond_variable_node::value,
        boost::python::return_value_policy<boost::python::return_by_value>()))
    ;

  // client::cond_operator_node
  auto cond_operator_node = boost::python::class_<client::cond_operator_node,
      boost::noncopyable,
      boost::python::bases<client::cond_node>
    >("cond_operator_node", boost::python::no_init)
    .add_property("operator",
      boost::python::make_getter(&client::cond_operator_node::node_operator))
    .add_property("left",
      boost::python::make_getter(&client::cond_operator_node::left_node,
        boost::python::return_value_policy<boost::python::return_by_value>()))
    .add_property("right",
      boost::python::make_getter(&client::cond_operator_node::right_node,
        boost::python::return_value_policy<boost::python::return_by_value>()))
    ;
  {
    boost::python::scope cond_operator_node_scope = cond_operator_node;
    boost::python::enum_<
        client::cond_operator_node::cond_operator
      >("cond_operator")
      .value("AND", client::cond_operator_node::AND)
      .value("OR", client::cond_operator_node::OR)
      .export_values()
      ;
  }

  // client::cond_expr
  boost::python::to_python_converter<client::cond_expr,
    cond_expr_to_python_converter>();

  // client::cond_expr_path
  boost::python::class_<client::cond_expr_path>("expr_path")
    .def(boost::python::vector_indexing_suite<client::cond_expr_path, true>())
    ;

  // client::json_value
  boost::python::to_python_converter<client::json_value,
    json_value_to_python_converter>();
}

