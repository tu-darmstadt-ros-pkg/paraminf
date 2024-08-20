#pragma once

#include <yaml-cpp/yaml.h>

#include "paraminf/parameter_interface.h"

namespace paraminf
{
/**
 * @brief The YamlIOHandler class can be used to read parameters from and write parameters to YAML files.
 */
class YamlIOHandler
{
public:
  /**
   * @brief Reads the parameters from a YAML file and adds them to the specified interface.
   * @param yaml_file_path path to the YAML file
   * @param parameter_interface parmeter interface where the parsed parameters should be added
   * @return true if parsing has been succesful
   */
  static bool readAndAddParametersFromFile(const std::string& yaml_file_path, ParameterInterface& parameter_interface);

  /**
   * @brief Reads the parameters from a YAML string and adds them to the specified interface.
   * @param yaml_input_string input YAML string
   * @param parameter_interface parmeter interface where the parsed parameters should be added
   * @return true if parsing has been succesful
   */
  static bool readAndAddParametersFromString(const std::string& yaml_input_string, ParameterInterface& parameter_interface);

  /**
   * @brief Reads the parameters from a yaml-cpp node and adds them to the specified interface.
   * @param node yaml-cpp node from which the parameters should be parsed
   * @param parameter_interface parmeter interface where the parsed parameters should be added
   * @return true if parsing has been succesful
   */
  static bool readAndAddParametersFromNode(const YAML::Node& node, ParameterInterface& parameter_interface);

  /**
   * @brief Writes the parameters of the given parameter interface to a YAML file.
   * @param yaml_file_path path of the file where the parameters should be written
   * @param parameter_interface the parameter interface of which the parameters should be written
   * @return true if wirting has been succesful
   */
  static bool writeParametersToFile(const std::string& yaml_file_path, const ParameterInterface& parameter_interface);

private:
  static void evaluateNode(const YAML::Node& node, const std::string& name_prefix, ParameterInterface& parameter_interface);

  static void readAndAddSingleParameter(const std::string& parameter_name, YAML::Node& value_node, ParameterInterface& parameter_interface);
  static void readAndAddParameterVector(const std::string& parameter_name, YAML::Node& vector_node, ParameterInterface& parameter_interface);

  template <typename T>
  static bool addSingleParameterIfTypeCorrect(const std::string& parameter_name, YAML::Node& value_node, ParameterInterface& parameter_interface);
  template <typename T>
  static bool addParameterVectorIfTypeCorrect(const std::string& parameter_name, YAML::Node& vector_node, ParameterInterface& parameter_interface);

  template <typename T>
  static bool tryParse(YAML::Node node, T& val);

  static void setEmitterOptions(YAML::Emitter& yaml_emitter);

  static void emitDoubleVec(YAML::Emitter& yaml_emitter, const std::vector<double>& double_vec);

  /**
   * @brief emitDouble enforces that a double gets written to the YAML file with a decimal even if the value has no
   * fraction, e.g. 1 gets written as 1.0. This ensures that the value is recogniced as double when read in
   * @param yaml_emitter the emmitter stream the double should be added to
   * @param d the value of the double
   */
  static void emitDouble(YAML::Emitter& yaml_emitter, double d);

  /**
   * @brief determines the type of the parameter, calls the  corresponding function to
   get its value and push it into the YAML emitter
   * @param yaml_emitter
   * @param parameter_name
   * @param parameter_interface
   */
  static void appendParameterToYaml(YAML::Emitter& yaml_emitter, const std::string& parameter_name, const ParameterInterface& parameter_interface);
};
}  // namespace paraminf
