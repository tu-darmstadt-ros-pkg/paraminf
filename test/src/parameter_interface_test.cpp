#include <gtest/gtest.h>

#include <fstream>
#include <vector>

#include <eigen3/Eigen/Core>

#include "paraminf/parameter_interface.h"

namespace paraminf
{
namespace test
{
template <typename T>
void testSingleParameter(const std::string& parameter_name, T expected_result, ParameterInterface& parameter_interface, const std::string& error_message = "")
{
  parameter_interface.setParam<T>(parameter_name, expected_result);

  T read_value;
  bool was_found = parameter_interface.getParam(parameter_name, read_value);
  ASSERT_TRUE(was_found) << "Parameter \"" << parameter_name << "\" was not found";
  EXPECT_EQ(read_value, expected_result) << error_message;
}

template <typename T>
void testParameterVector(const std::string& parameter_name, std::vector<T> expected_vector, ParameterInterface& parameter_interface, const std::string& error_message = "")
{
  parameter_interface.setParam(parameter_name, expected_vector);

  std::vector<T> read_vector;
  bool was_found = parameter_interface.getParam(parameter_name, read_vector);
  ASSERT_TRUE(was_found) << "Parameter \"" << parameter_name << "\" was not found";

  ASSERT_EQ(read_vector.size(), expected_vector.size());
  for (uint i = 0; i < expected_vector.size(); i++)
  {
    EXPECT_EQ(read_vector[i], expected_vector[i]) << error_message << " at position: " << i;
  }
}

template <typename T, int R, int C>
void testEigenMatrixParameter(const std::string& parameter_name, const Eigen::Matrix<T, R, C>& expected_matrix, ParameterInterface& parameter_interface,
                              const std::string& error_message = "")
{
  parameter_interface.setParam(parameter_name, expected_matrix);

  Eigen::Matrix<T, R, C> read_value = Eigen::Matrix<T, R, C>::Zero();
  bool was_found = parameter_interface.getParam(parameter_name, read_value);
  ASSERT_TRUE(was_found) << "Parameter \"" << parameter_name << "\" was not found";

  for (size_t row = 0; row < expected_matrix.rows(); row++)
  {
    for (size_t col = 0; col < expected_matrix.cols(); col++)
    {
      EXPECT_EQ(read_value(row, col), expected_matrix(row, col)) << error_message;
    }
  }
}

TEST(ParameterInterfaceTest, SetAndGetParameterTest)
{
  ParameterInterface parameter_interface;

  char ch(42);

  size_t sizet(1);

  Eigen::Vector3d eigen_vector(1, 2, 3);
  Eigen::Matrix2d eigen_matrix;
  eigen_matrix(0, 0) = 1;
  eigen_matrix(0, 1) = 2;
  eigen_matrix(1, 0) = 3;
  eigen_matrix(1, 1) = 4;

  std::vector int_vec = { 3, 2, 6 };
  std::vector double_vec = { 3.0, 1.1 };
  std::vector bool_vec = { false, false, true };
  std::vector string_vec = { "test1", "test2" };

  EXPECT_FALSE(parameter_interface.hasBeenUpdated());

  testSingleParameter("int_param", 3, parameter_interface, "Int was read incorrectly");

  EXPECT_TRUE(parameter_interface.hasBeenUpdated());
  parameter_interface.resetUpdateFlag();
  EXPECT_FALSE(parameter_interface.hasBeenUpdated());

  testSingleParameter("double_param", 4.2, parameter_interface, "Double was read incorrectly");

  EXPECT_TRUE(parameter_interface.hasBeenUpdated());
  parameter_interface.resetUpdateFlag();
  EXPECT_FALSE(parameter_interface.hasBeenUpdated());

  testSingleParameter("bool_param", false, parameter_interface, "Bool was read incorrectly");
  testSingleParameter<std::string>("string_param", "test", parameter_interface, "String was read incorrectly");
  testSingleParameter("char_param", ch, parameter_interface, "Char was read incorrectly");
  testSingleParameter("size_t_param", sizet, parameter_interface, "size_t was read incorrectly");

  EXPECT_TRUE(parameter_interface.hasBeenUpdated());

  testSingleParameter("intvec_param", int_vec, parameter_interface, "Int was read incorrectly");
  testSingleParameter("doublevec_param", double_vec, parameter_interface, "String was read incorrectly");
  testSingleParameter("boolvec_param", bool_vec, parameter_interface, "Double was read incorrectly");
  testSingleParameter("stringvec_param", string_vec, parameter_interface, "Bool was read incorrectly");

  EXPECT_TRUE(parameter_interface.hasBeenUpdated());

  testEigenMatrixParameter("Eigen::Vector3d", eigen_vector, parameter_interface, "eigen_vector was read incorrectly");
  testEigenMatrixParameter("Eigen::Matrix3d", eigen_matrix, parameter_interface, "eigen_matrix was read incorrectly");
}

TEST(ParameterInterfaceTest, IntParameterAutoConversionTest)
{
  ParameterInterface parameter_interface;

  parameter_interface.setParam("test_int", 42);

  int found_int = 0;
  bool was_found_int_as_int = parameter_interface.getParam("test_int", found_int);
  ASSERT_TRUE(was_found_int_as_int) << "Parameter \"test_int\" was not found";
  EXPECT_EQ(found_int, 42) << "Int parameter was read incorrectly";

  double found_double = 0.0;
  bool was_found_int_as_double = parameter_interface.getParam("test_int", found_double);
  ASSERT_TRUE(was_found_int_as_double) << "Parameter \"test_int\" as double was not found";
  EXPECT_EQ(found_double, 42.0) << "Int parameter as double was read incorrectly";

  size_t found_size_t = 0;
  bool was_found_int_as_size_t = parameter_interface.getParam("test_int", found_size_t);
  ASSERT_TRUE(was_found_int_as_size_t) << "Parameter \"test_int\" as size_t was not found";
  EXPECT_EQ(found_int, 42) << "Int parameter as size_t was read incorrectly";

  char found_char = 0;
  bool was_found_int_as_char = parameter_interface.getParam("test_int", found_char);
  ASSERT_TRUE(was_found_int_as_char) << "Parameter \"test_int\" as char was not found";
  EXPECT_EQ(found_int, 42) << "Int parameter as char was read incorrectly";

  std::string found_string;
  bool was_found_int_as_string = parameter_interface.getParam("test_int", found_string);
  EXPECT_TRUE(!was_found_int_as_string) << "Parameter \"test_int\" as string was found";
}

TEST(ParameterInterfaceTest, GetParameterOrErrorTest)
{
  ParameterInterface parameter_interface;

  parameter_interface.setParam("test_int", 42);

  int found_int = parameter_interface.getParam<int>("test_int");
  EXPECT_EQ(found_int, 42) << "Int parameter was read incorrectly";

  EXPECT_ANY_THROW(parameter_interface.getParam<bool>("test_bool"));
}

TEST(ParameterInterfaceTest, HasParamTest)
{
  ParameterInterface parameter_interface;

  parameter_interface.setParam("test_int", 42);
  parameter_interface.setParam("test_string", std::string("test"));

  std::vector<int> test_vector_int = { 0, 1, 2, 3 };
  parameter_interface.setParam("test_vector_int", test_vector_int);

  EXPECT_TRUE(parameter_interface.hasParam("test_int")) << "Int parameter was not found";
  EXPECT_TRUE(parameter_interface.hasParam("test_string")) << "String parameter was not found";
  EXPECT_TRUE(parameter_interface.hasParam("test_vector_int")) << "Int vector parameter was not found";

  EXPECT_TRUE(parameter_interface.hasParamOfType<int>("test_int")) << "Int parameter with the correct type was not "
                                                                      "found";
  EXPECT_TRUE(parameter_interface.hasParamOfType<std::string>("test_string")) << "String parameter with the correct "
                                                                                 "type was "
                                                                                 "not found";
  EXPECT_TRUE(parameter_interface.hasParamOfType<std::vector<int>>("test_vector_int")) << "Int vector parameter with "
                                                                                          "the "
                                                                                          "correct type was not "
                                                                                          "found";

  EXPECT_FALSE(parameter_interface.hasParamOfType<std::string>("test_int")) << "Int parameter with the incorrect "
                                                                               "string type "
                                                                               "was found";
  EXPECT_FALSE(parameter_interface.hasParamOfType<bool>("test_string")) << "String parameter with the incorrect bool "
                                                                           "type "
                                                                           "was found";
  EXPECT_FALSE(parameter_interface.hasParamOfType<int>("test_vector_int")) << "Int vector parameter with the incorrect "
                                                                              "int "
                                                                              "type was found";
  EXPECT_FALSE(parameter_interface.hasParamOfType<std::vector<bool>>("test_vector_int")) << "Int vector parameter with "
                                                                                            "the "
                                                                                            "incorrect"
                                                                                            "bool vector type was "
                                                                                            "found";
}

}  // namespace test
}  // namespace paraminf
