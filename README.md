# Paraminf
This package provides a generic parameter interface that can hold arbitrary types that can be accessed by their names.
The parameters can also be loaded and stored to YAML files.

The functionality is similar to the ROS parameter server while being implemented without ROS dependencies.

## Usage
This example code would work with the file example in the next section. For more details refer to the Doxygen documentation.

```c++
  /* initialization and read in */
  ParameterInterface param_inf;
  YamlIOHandler::readAndAddParametersFromFile("input/file/path/input.yaml", param_inf);

  /* read single value parameter */

  /* option 1 */
  int int_value;
  bool was_found = param_inf.getParam("category1/int_parameters/int_parameter_name", int_value);

  /* option 2 */
  int other_int_value=param_inf.getParam<int>("category1/int_parameters/int_parameter_name");

  /* read vector parameter */
  std::vector<double> double_vec=param_inf.getParam<std::vector<double>>("category2/vectors/double_vectors/vec1");

  /* storing parameters */
  YamlIOHandler::writeParametersToFile("output/file/path/output.yaml", param_inf);
```

Example YAML file:

```yaml
category1:
  int_parameters:
    int_parameter_name: 42
category2:
  vectors:
    double_vectors:
      vec1: [-0.12, 0.4, 123456789.1234568]
```
## Documentation
When building the package you can use the flag '-DBUILD_DOC=TRUE' to build the documentation. You can access it in the doc folder afterwards.

## Installation
While the package is set up to be build using [ament](https://design.ros2.org/articles/ament.html), it has no ROS dependencies.
The [yaml-cpp](https://github.com/jbeder/yaml-cpp) package is required for using this package. It can be installed using:
```
sudo apt install libyaml-cpp-dev
```
After cloning the package into your catkin workspace use `colcon build`.
This package has been developed originally using catkin as build system. You can find this version on the "catkin_version" branch.

## Future Development & Contribution
The project during which the package was developed has been discontinued.
But in case you find bugs, typos or have suggestions for improvements feel free to open an issue.
We would especially appreciate Pull Requests fixing open issues.

## Authors
- Felix Biemüller
- Alexander Stumpf
- Khrystyna Rud
