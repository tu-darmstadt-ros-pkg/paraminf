#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>

#include <yaml-cpp/yaml.h>

#include <boost/algorithm/string.hpp>

#include "paraminf/yaml_io_handler.h"

namespace paraminf
{
bool YamlIOHandler::readAndAddParametersFromFile(const std::string& yaml_file_path, ParameterInterface& parameter_interface)
{
  try
  {
    std::vector<YAML::Node> parameter_nodes = YAML::LoadAllFromFile(yaml_file_path);

    for (auto& node : parameter_nodes)
    {
      evaluateNode(node, "", parameter_interface);
    }
    return true;
  }
  catch (...)
  {
    return false;
  }
}

bool YamlIOHandler::readAndAddParametersFromString(const std::string& yaml_input_string, ParameterInterface& parameter_interface)
{
  try
  {
    std::vector<YAML::Node> parameter_nodes = YAML::LoadAll(yaml_input_string);

    for (auto& node : parameter_nodes)
    {
      evaluateNode(node, "", parameter_interface);
    }
    return true;
  }
  catch (...)
  {
    return false;
  }
}

bool YamlIOHandler::readAndAddParametersFromNode(const YAML::Node& node, ParameterInterface& parameter_interface)
{
  try
  {
    evaluateNode(node, "", parameter_interface);
    return true;
  }
  catch (...)
  {
    return false;
  }
}

bool YamlIOHandler::writeParametersToFile(const std::string& yaml_file_path, const ParameterInterface& parameter_interface)
{
  try
  {
    YAML::Emitter yaml;

    setEmitterOptions(yaml);
    std::vector<std::string> parameter_names = parameter_interface.getAllParameterNames();
    std::string parameter_path;
    std::vector<std::string> open_tokens;
    yaml << YAML::BeginMap;
    for (const auto& param_name : parameter_names)
    {
      std::vector<std::string> new_tokens;
      // False positive by Clang Tidy: https://github.com/boostorg/algorithm/issues/63
      // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
      boost::split(new_tokens, param_name, boost::is_any_of("/"));

      // count the number of tokens that are consecutively equal in the prefixpath of the current structure allready
      // written to the yaml and the new parameter
      int nr_of_same_tokens = 0;
      for (int i = 0; i < open_tokens.size() && i < new_tokens.size() - 1 &&  // new_tokens.size() - 1 as the last token is the name of the key
                      new_tokens[i] == open_tokens[i];
           i++)
      {
        nr_of_same_tokens = i + 1;
      }
      // close all maps that are not part of the new parameter path
      for (int i = nr_of_same_tokens; i < open_tokens.size(); i++)
      {
        yaml << YAML::EndMap;
      }

      // open all new maps that are needed for the new parameter path
      // as invariant it is expected, that there is allready an open map to insert the new name as key and the new map
      // for the subdirectory as value
      for (int i = nr_of_same_tokens; i < new_tokens.size() - 1; i++)
      {
        yaml << YAML::Key << new_tokens[i] << YAML::Value << YAML::BeginMap;
      }
      // add the parmeter name as key and declare that it is followed by its value
      yaml << YAML::Key << new_tokens[new_tokens.size() - 1] << YAML::Value;
      // remove the parameter name from the token vector and replace open tokens with the remaining path
      new_tokens.pop_back();
      open_tokens = new_tokens;

      // figure out correct parameter type and apend it to the yaml stream
      appendParameterToYaml(yaml, param_name, parameter_interface);
    }

    // close all maps that are still open after the last parameter has been added
    for (int i = 0; i < open_tokens.size(); i++)
    {
      yaml << YAML::EndMap;
    }
    yaml << YAML::EndMap;

    // write the yaml stream to the file
    std::ofstream output_file;
    output_file.open(yaml_file_path);
    output_file << yaml.c_str();
    output_file.close();
    return true;
  }
  catch (...)
  {
    return false;
  }
}

void YamlIOHandler::evaluateNode(const YAML::Node& node, const std::string& name_prefix, ParameterInterface& parameter_interface)
{
  if (node.IsMap())
  {
    for (auto it = node.begin(); it != node.end(); it++)
    {
      auto node_pair = *it;

      if (node_pair.second.IsScalar())
      {
        readAndAddSingleParameter(name_prefix + node_pair.first.as<std::string>(), node_pair.second, parameter_interface);
      }
      else if (node_pair.second.IsSequence())
      {
        readAndAddParameterVector(name_prefix + node_pair.first.as<std::string>(), node_pair.second, parameter_interface);
      }
      else if (node_pair.second.IsMap())
      {
        evaluateNode(node_pair.second, name_prefix + node_pair.first.as<std::string>() + "/", parameter_interface);
      }
      else
      {
        throw std::invalid_argument("YAML node type is not supported. Name prefix: " + name_prefix);
      }
    }
  }
}

void YamlIOHandler::readAndAddSingleParameter(const std::string& parameter_name, YAML::Node& value_node, ParameterInterface& parameter_interface)
{
  if (addSingleParameterIfTypeCorrect<int>(parameter_name, value_node, parameter_interface))
  {
    return;
  }
  if (addSingleParameterIfTypeCorrect<double>(parameter_name, value_node, parameter_interface))
  {
    return;
  }
  if (addSingleParameterIfTypeCorrect<bool>(parameter_name, value_node, parameter_interface))
  {
    return;
  }
  if (addSingleParameterIfTypeCorrect<std::string>(parameter_name, value_node, parameter_interface))
  {
    return;
  }
  throw std::invalid_argument("Parameter type of " + parameter_name + " is not supported.");
}

void YamlIOHandler::readAndAddParameterVector(const std::string& parameter_name, YAML::Node& vector_node, ParameterInterface& parameter_interface)
{
  if (addParameterVectorIfTypeCorrect<int>(parameter_name, vector_node, parameter_interface))
  {
    return;
  }
  if (addParameterVectorIfTypeCorrect<double>(parameter_name, vector_node, parameter_interface))
  {
    return;
  }
  if (addParameterVectorIfTypeCorrect<bool>(parameter_name, vector_node, parameter_interface))
  {
    return;
  }
  if (addParameterVectorIfTypeCorrect<std::string>(parameter_name, vector_node, parameter_interface))
  {
    return;
  }
  throw std::invalid_argument("Parameter sequence type of " + parameter_name + " is not supported.");
}

void YamlIOHandler::setEmitterOptions(YAML::Emitter& yaml_emitter)
{
  yaml_emitter.SetIndent(4);
  yaml_emitter.SetBoolFormat(YAML::TrueFalseBool);
  yaml_emitter.SetBoolFormat(YAML::LowerCase);
  yaml_emitter.SetDoublePrecision(std::numeric_limits<double>::max_digits10);
  yaml_emitter.SetSeqFormat(YAML::Flow);
}

void YamlIOHandler::emitDoubleVec(YAML::Emitter& yaml_emitter, const std::vector<double>& double_vec)
{
  yaml_emitter << YAML::BeginSeq;
  for (double d : double_vec)
  {
    emitDouble(yaml_emitter, d);
  }
  yaml_emitter << YAML::EndSeq;
}

void YamlIOHandler::emitDouble(YAML::Emitter& yaml_emitter, double d)
{
  if (fmod(d, 1) == 0)
  {
    // enforce ".0" if the double has no decimal fraction in order to be parsed as double if read again
    std::stringstream s;
    s << std::fixed << std::setprecision(1) << d;
    yaml_emitter << s.str();
  }
  else
  {
    yaml_emitter << d;
  }
}

void YamlIOHandler::appendParameterToYaml(YAML::Emitter& yaml_emitter, const std::string& parameter_name, const ParameterInterface& parameter_interface)
{
  std::string value_as_string;
  parameter_interface.getParam(parameter_name, value_as_string);
  if (parameter_interface.hasParamOfType<int>(parameter_name))
  {
    yaml_emitter << parameter_interface.getParam<int>(parameter_name);
  }
  else if (parameter_interface.hasParamOfType<double>(parameter_name))
  {
    emitDouble(yaml_emitter, parameter_interface.getParam<double>(parameter_name));
  }
  else if (parameter_interface.hasParamOfType<std::string>(parameter_name))
  {
    yaml_emitter << parameter_interface.getParam<std::string>(parameter_name);
  }
  else if (parameter_interface.hasParamOfType<bool>(parameter_name))
  {
    yaml_emitter << parameter_interface.getParam<bool>(parameter_name);
  }
  else if (parameter_interface.hasParamOfType<std::vector<int>>(parameter_name))
  {
    yaml_emitter << parameter_interface.getParam<std::vector<int>>(parameter_name);
  }
  else if (parameter_interface.hasParamOfType<std::vector<double>>(parameter_name))
  {
    emitDoubleVec(yaml_emitter, parameter_interface.getParam<std::vector<double>>(parameter_name));
  }
  else if (parameter_interface.hasParamOfType<std::vector<bool>>(parameter_name))
  {
    yaml_emitter << parameter_interface.getParam<std::vector<bool>>(parameter_name);
  }
  else if (parameter_interface.hasParamOfType<std::vector<std::string>>(parameter_name))
  {
    yaml_emitter << parameter_interface.getParam<std::vector<std::string>>(parameter_name);
  }
  else
  {
    throw std::invalid_argument("Parameter \"" + parameter_name + " was not found");
  }
}

template <typename T>
bool YamlIOHandler::tryParse(YAML::Node node, T& val)
{
  try
  {
    val = node.as<T>();
    return true;
  }
  catch (...)
  {
    return false;
  }
}

template <typename T>
bool YamlIOHandler::addSingleParameterIfTypeCorrect(const std::string& parameter_name, YAML::Node& value_node, ParameterInterface& parameter_interface)
{
  T value_of_type_t;
  if (tryParse(value_node, value_of_type_t))
  {
    parameter_interface.setParam(parameter_name, value_of_type_t);
    return true;
  }
  return false;
}

template <typename T>
bool YamlIOHandler::addParameterVectorIfTypeCorrect(const std::string& parameter_name, YAML::Node& vector_node, ParameterInterface& parameter_interface)
{
  std::vector<T> new_parameter_vector(vector_node.size());

  bool parsing_succefull = true;
  uint element_count = 0;

  for (auto it = vector_node.begin(); it != vector_node.end(); it++)
  {
    T vector_element;
    parsing_succefull = tryParse(*it, vector_element);
    if (parsing_succefull)
    {
      new_parameter_vector[element_count] = vector_element;
      element_count++;
    }
    else
    {
      return false;
    }
  }

  parameter_interface.setParam(parameter_name, new_parameter_vector);
  return true;
}
}  // namespace paraminf
