#include "paraminf/parameter_interface.h"

namespace paraminf
{
bool ParameterInterface::hasParam(const std::string& parameter_name) const
{
  std::map<std::string, std::any>::const_iterator itr = parameter_set_.find(parameter_name);

  return itr != parameter_set_.end();
}

std::vector<std::string> ParameterInterface::getAllParameterNames() const
{
  size_t parameter_number = parameter_set_.size();

  std::vector<std::string> parameter_names(parameter_number);

  size_t counter = 0;
  for (auto const& element : parameter_set_)
  {
    parameter_names[counter] = element.first;
    counter++;
  }

  sort(parameter_names.begin(), parameter_names.end());
  return parameter_names;
}

bool ParameterInterface::hasBeenUpdated() const { return has_been_updated_; }

void ParameterInterface::resetUpdateFlag() { has_been_updated_ = false; }

}  // namespace paraminf
