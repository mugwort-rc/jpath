#include "jpath.h"

#include <boost/algorithm/string.hpp>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

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

struct optional_cond_expr_to_python_converter
{
  static PyObject *convert(const boost::optional<client::cond_expr> &cond_expr)
  {
    boost::python::object result;
    if ( cond_expr )
    {
      auto path  = (*cond_expr).first;
      auto value = (*cond_expr).second;
      if ( value )
      {
        result = boost::python::make_tuple(path, *value);
      }
      else
      {
        result = boost::python::make_tuple(path);
      }
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

std::string jpath_node_repr(client::json_path_node &self)
{
  auto to_string = [](const client::path_index &index) -> std::string
  {
    if ( index.type() == typeid(int) )
    {
      return boost::lexical_cast<std::string>(boost::get<int>(index));
    }
    else
    {
      return "\"" + boost::algorithm::replace_all_copy(
                      boost::get<std::string>(index), "\"", "\\\"") + "\"";
    }
  };

  std::string path, cond;
  path = to_string(self.index);
  if ( self.cond )
  {
    auto pair = *self.cond;
    for ( auto p : pair.first )
    {
      if ( ! cond.empty() )
      {
        cond += "/";
      }
      cond += to_string(p);
    }
    if ( pair.second )
    {
      auto value = *pair.second;
      cond += "=";
      if ( value.type() == typeid(int) )
      {
        cond += boost::lexical_cast<std::string>(boost::get<int>(value));
      }
      else if ( value.type() == typeid(std::string) )
      {
        cond += "\"" + boost::algorithm::replace_all_copy(
          boost::get<std::string>(value), "\"", "\\\"") + "\"";
      }
      else if ( value.type() == typeid(bool) )
      {
        cond += boost::get<bool>(value) ? "true" : "false";
      }
      else if ( value.type() == typeid(client::json_null) )
      {
        cond += "null";
      }
    }
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
  //   <client::cond_expr>
  // client::cond_expr
  boost::python::to_python_converter<boost::optional<client::cond_expr>,
    optional_cond_expr_to_python_converter>();

  // client::cond_expr_path
  boost::python::class_<client::cond_expr_path>("expr_path")
    .def(boost::python::vector_indexing_suite<client::cond_expr_path, true>())
    ;

  // client::json_value
  boost::python::to_python_converter<client::json_value,
    json_value_to_python_converter>();
}

