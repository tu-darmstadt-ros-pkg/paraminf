#pragma once

#include <memory>
#include <vector>
#include <any>
#include <map>
#include <algorithm>
#include <type_traits>
#include <stdexcept>

namespace paraminf
{
/**
 * @brief The ParameterInterface class can be used for handling and passing parameters of arbitrary types.
 */
class ParameterInterface
{
public:
  /**
   * @brief Alias for std::shared_ptr
   */
  using Ptr = std::shared_ptr<ParameterInterface>;

  /**
   * @brief Alias for read only std::shared_ptr
   */
  using ConstPtr = std::shared_ptr<const ParameterInterface>;

  virtual ~ParameterInterface() = default;

  /**
   * @brief Tries to retrieve the value for the given parameter name and if succesful writes it to the given reference.
   * @details If the parameter is of type int and int is convertible to the type of the given reference, the int value is casted and written to the given reference.
   * @param parameter_name the name of the parameter that should be looked up
   * @param parameter_value the reference to the value that should be overwritten, if the value for the parameter name could be retrived
   * @return true if the parameter was found and could successfully be retrieved and written to the given reference
   */
  template <class ValueType>
  bool getParam(const std::string& parameter_name, ValueType& parameter_value) const
  {
    return getParamImpl(parameter_name, parameter_value);
  }

  /**
   * @brief Tries to retrieve the value for the given parameter name and if succesful returns it.
   * @details If the parameter is of type int and int is convertible to the given ValueType, the int value is casted and returned.
   * If no parameter with the given name and type  is found, an exeption is thrown.
   * @param parameter_name the name of the parameter that should be looked up
   * @return retrieved parameter value with the given parameter_name
   */
  template <class ValueType>
  ValueType getParam(const std::string& parameter_name) const
  {
    ValueType parameter_value;
    bool param_found = getParamImpl(parameter_name, parameter_value);

    if (!param_found)
    {
      throw std::invalid_argument("Parameter \"" + parameter_name + " was not found");
    }
    return parameter_value;
  }

  /**
   * @brief Creates an parameter entry for the of the given name with the given value.
   * @param parameter_name the name of the parameter entry that should be created
   * @param parameter_value the value of the paramter
   */
  template <class ValueType>
  void setParam(const std::string& parameter_name, ValueType parameter_value)
  {
    parameter_set_[parameter_name] = parameter_value;
    has_been_updated_ = true;
  }

  /**
   * @brief Querries whether a parameter is available in the parameter interface.
   * @param parameter_name the name of the parameter that should be checked
   * @return true, if the parameter is available
   */
  bool hasParam(const std::string& parameter_name) const;

  /**
   * @brief Querries whether a parameter with the given name and type is available in the parameter interface.
   * @param parameter_name the name of the parameter that should be checked
   * @return true, if the parameter with the given type is available
   */
  template <class ValueType>
  bool hasParamOfType(const std::string& parameter_name) const
  {
    std::map<std::string, std::any>::const_iterator itr = parameter_set_.find(parameter_name);

    return itr != parameter_set_.end() && (isType<ValueType>(itr->second) || (std::is_convertible_v<int, ValueType> && isType<int>(itr->second)));
  }

  /**
   * @brief Returns a vector with all parameter names available.
   * @return vector with all parameter names
   */
  std::vector<std::string> getAllParameterNames() const;

  /**
   * @brief Returns true if any parameter has been added or updated since the instantiation of the parameter interface or the last call of resetUpdateFlag().
   * @details The update flag is set every time setParam() is called
   * @return if any parameter has been added or updated
   */
  bool hasBeenUpdated() const;

  /**
   * @brief Resets the update flag s.t. hasBeenUpdated() returns false until setParam() is called again.
   */
  void resetUpdateFlag();

private:
  bool has_been_updated_ = false;

  std::map<std::string, std::any> parameter_set_;

  template <class ValueType>
  bool getParamImpl(const std::string& parameter_name, ValueType& parameter_value) const
  {
    std::map<std::string, std::any>::const_iterator itr = parameter_set_.find(parameter_name);
    // if the parameter is not found return false
    if (itr == parameter_set_.end())
      return false;
    // if the parameter is found and has the querried type set it and return true
    else if (isType<ValueType>(itr->second))
    {
      parameter_value = std::any_cast<ValueType>(itr->second);
      return true;
    }
    else
    {
      // if the parameter has been added as int and int can be converted to the queried type set it and return true
      if constexpr (std::is_convertible_v<int, ValueType>)
      {
        if (isType<int>(itr->second))
          parameter_value = static_cast<ValueType>(std::any_cast<int>(itr->second));
        return true;
      }
    }
    return false;
  }

  template <typename ValueType>
  static bool isType(const std::any& to_check)
  {
    return to_check.type() == typeid(ValueType);
  }
};
}  // namespace paraminf
