#include <gtest/gtest.h>

#include <fstream>

#include "paraminf/yaml_io_handler.h"
#include "paraminf/parameter_interface.h"
namespace paraminf
{
namespace test
{
template <typename T>
void testGetSingleParameter(const std::string& parameter_name, T expected_result, T init_value, ParameterInterface& param_inf, const std::string& error_message = "")
{
  T read_value = init_value;
  bool was_found = param_inf.getParam(parameter_name, read_value);
  EXPECT_TRUE(was_found) << "Parameter \"" << parameter_name << "\" was not found";
  EXPECT_EQ(read_value, expected_result) << error_message;
}

template <typename T>
void testGetParameterVector(const std::string& parameter_name, std::vector<T> expected_vector, ParameterInterface& param_inf, const std::string& error_message = "")
{
  std::vector<T> read_vector;
  bool was_found = param_inf.getParam(parameter_name, read_vector);
  EXPECT_TRUE(was_found) << "Parameter \"" << parameter_name << "\" was not found";

  ASSERT_EQ(read_vector.size(), expected_vector.size());
  for (uint i = 0; i < expected_vector.size(); i++)
  {
    EXPECT_EQ(read_vector[i], expected_vector[i]) << error_message << " at position: " << i;
  }
}

TEST(YamlIOTest, ReadFileAndTestSingleParameters)
{
  ParameterInterface param_inf;

  EXPECT_FALSE(param_inf.hasBeenUpdated());

  YamlIOHandler::readAndAddParametersFromFile(SOURCE_DIR "/test/test_yaml_files/random_order.yaml", param_inf);

  EXPECT_TRUE(param_inf.hasBeenUpdated());

  testGetSingleParameter("category/category2/paremeter124_bool", true, false, param_inf, "Bool was read incorrectly");
  testGetSingleParameter("0therC4tegory/subcategory/parameter_is_there", true, false, param_inf, "Bool was read incorrectly");
  testGetSingleParameter("category/parameter123_int", int(-7), 0, param_inf, "Int was read incorrectly");
  testGetSingleParameter("category/parameter111_double", -0.123456789, 0.0, param_inf, "Double was read incorrectly");
  testGetSingleParameter("category/xyz/parameter112_string", std::string("test_string"), std::string(""), param_inf, "String was read incorrectly");
  testGetSingleParameter("0therC4tegory/subcategory/subsubcategory/string_parameter", std::string("Also there"), std::string(""), param_inf, "String was read incorrectly");
}

TEST(YamlIOTest, ReadNonExistingParameter)
{
  ParameterInterface param_inf;
  int ret = 0;
  EXPECT_FALSE(param_inf.getParam("not_there", ret));
}

TEST(YamlIOTest, ReadFileAndTestParameterVectors)
{
  ParameterInterface param_inf;

  YamlIOHandler::readAndAddParametersFromFile(SOURCE_DIR "/test/test_yaml_files/random_order.yaml", param_inf);

  std::vector<bool> expected_bool = { false, false, false, false, true, true, false, true };
  testGetParameterVector("category/paremeter225_bool", expected_bool, param_inf, "Bool was read incorrectly");

  std::vector<double> expected_double = { -0.12, 0.4, 123456789.123456789, 4.0 };
  testGetParameterVector("category/parameter222_double_vector", expected_double, param_inf, "Bool was read incorrectly");

  std::vector<int> expected_int = { -7, 0, 2, -5, 45 };
  testGetParameterVector("0therC4tegory/subcategory/subsubcategory/parameter223_int_vector", expected_int, param_inf, "Bool was read incorrectly");

  std::vector<std::string> expected_string = { "A", "B", "C", "D", "e", "f", "g", "h", "123x" };
  testGetParameterVector("category/parameter211_string_vector", expected_string, param_inf, "Bool was read incorrectly");
}

TEST(YamlIOTest, WriteFile)
{
  ParameterInterface parameter_interface;

  ASSERT_TRUE(YamlIOHandler::readAndAddParametersFromFile(SOURCE_DIR "/test/test_yaml_files/random_order.yaml", parameter_interface));
  // read parameters of original file and write them again to new file
  ASSERT_TRUE(YamlIOHandler::writeParametersToFile("WriteFileTestOut.yaml", parameter_interface));

  std::ifstream ifs_expected(SOURCE_DIR "/test/test_yaml_files/expected_result_ordered.yaml");
  std::string content_expected((std::istreambuf_iterator<char>(ifs_expected)), (std::istreambuf_iterator<char>()));

  std::ifstream ifs_first_write("WriteFileTestOut.yaml");
  ASSERT_TRUE(ifs_first_write.good());

  std::string content_first_write((std::istreambuf_iterator<char>(ifs_first_write)), (std::istreambuf_iterator<char>()));
  EXPECT_TRUE(content_first_write == content_expected);

  // reread parameters of the new file and make sure they are all parsed correctly by writing them again
  ParameterInterface reread;
  ASSERT_TRUE(YamlIOHandler::readAndAddParametersFromFile("WriteFileTestOut.yaml", reread));
  ASSERT_TRUE(YamlIOHandler::writeParametersToFile("WriteFileTestOut2.yaml", reread));

  std::ifstream ifs_second_write("WriteFileTestOut2.yaml");
  ASSERT_TRUE(ifs_first_write.good());
  std::string content_second_write((std::istreambuf_iterator<char>(ifs_second_write)), (std::istreambuf_iterator<char>()));

  EXPECT_TRUE(content_second_write == content_expected);
}

TEST(YamlIOTest, ReadStringAndTestParameters)
{
  ParameterInterface param_inf;

  YamlIOHandler::readAndAddParametersFromString("{ test_int: 42, test_double: 1.0, test_bool: false, test_string: "
                                                "'apple', "
                                                "test_int_vec: [3, 2, 1], test_double_vec: [1.3, 4.2, 0.6, 9.3], "
                                                "test_bool_vec: [true, true,false], test_string_vec: ['apple', "
                                                "'banana']}",
                                                param_inf);

  testGetSingleParameter("test_int", 42, int(-32), param_inf, "Int was read incorrectly");
  testGetSingleParameter("test_double", 1.0, 0.0, param_inf, "Double was read incorrectly");
  testGetSingleParameter("test_bool", false, true, param_inf, "Bool was read incorrectly");
  testGetSingleParameter("test_string", std::string("apple"), std::string(""), param_inf, "String was read incorrectly");

  std::vector<int> expected_int = { 3, 2, 1 };
  std::vector<double> expected_double = { 1.3, 4.2, 0.6, 9.3 };
  std::vector<bool> expected_bool = { true, true, false };
  std::vector<std::string> expected_string = { "apple", "banana" };

  testGetParameterVector("test_int_vec", expected_int, param_inf, "Int vector was read incorrectly");
  testGetParameterVector("test_double_vec", expected_double, param_inf, "Double vector was read incorrectly");
  testGetParameterVector("test_bool_vec", expected_bool, param_inf, "Bool vector was read incorrectly");
  testGetParameterVector("test_string_vec", expected_string, param_inf, "String vector was read incorrectly");
}

TEST(YamlIOTest, ReadStringAndTestParametersFromFile)
{
  ParameterInterface param_inf;

  std::ifstream yaml_file_string(SOURCE_DIR "/test/test_yaml_files/random_order.yaml");
  std::string yaml_string((std::istreambuf_iterator<char>(yaml_file_string)), std::istreambuf_iterator<char>());

  YamlIOHandler::readAndAddParametersFromString(yaml_string, param_inf);

  testGetSingleParameter("category/category2/paremeter124_bool", true, false, param_inf, "Bool was read incorrectly");
  testGetSingleParameter("0therC4tegory/subcategory/parameter_is_there", true, false, param_inf, "Bool was read incorrectly");
  testGetSingleParameter("category/parameter123_int", int(-7), 0, param_inf, "Int was read incorrectly");
  testGetSingleParameter("category/parameter111_double", -0.123456789, 0.0, param_inf, "Double was read incorrectly");
  testGetSingleParameter("category/xyz/parameter112_string", std::string("test_string"), std::string(""), param_inf, "String was read incorrectly");
  testGetSingleParameter("0therC4tegory/subcategory/subsubcategory/string_parameter", std::string("Also there"), std::string(""), param_inf, "String was read incorrectly");
}

TEST(YamlIOTest, ReadRosparamStringAndTestParameters)
{
  ParameterInterface param_inf;

  std::ifstream yaml_file_string(SOURCE_DIR "/test/test_yaml_files/rosparam_test_file.yaml");
  std::string yaml_string((std::istreambuf_iterator<char>(yaml_file_string)), std::istreambuf_iterator<char>());

  YamlIOHandler::readAndAddParametersFromString(yaml_string, param_inf);

  testGetSingleParameter("test/test_int", 42, int(-32), param_inf, "Int was read incorrectly");
  testGetSingleParameter("test/test_double", 1.0, 0.0, param_inf, "Double was read incorrectly");
  testGetSingleParameter("test/test_bool", false, true, param_inf, "Bool was read incorrectly");
  testGetSingleParameter("test/test_string", std::string("apple"), std::string(""), param_inf, "String was read incorrectly");

  std::vector<int> expected_int = { 3, 2, 1 };
  std::vector<double> expected_double = { 1.3, 4.2, 0.6, 9.3 };
  std::vector<bool> expected_bool = { true, true, false };
  std::vector<std::string> expected_string = { "apple", "banana" };

  testGetParameterVector("test/test_int_vec", expected_int, param_inf, "Int vector was read incorrectly");
  testGetParameterVector("test/test_double_vec", expected_double, param_inf, "Double vector was read incorrectly");
  testGetParameterVector("test/test_bool_vec", expected_bool, param_inf, "Bool vector was read incorrectly");
  testGetParameterVector("test/test_string_vec", expected_string, param_inf, "String vector was read incorrectly");
}

TEST(YamlIOTest, ReadNodeAndTestParameters)
{
  ParameterInterface param_inf;

  // Test single parameters
  YAML::Node test_int;
  test_int["test_int"] = 42;

  YAML::Node test_double;
  test_int["test_double"] = 1.0;

  YAML::Node test_bool;
  test_int["test_bool"] = false;

  YAML::Node test_string;
  test_int["test_string"] = "apple";

  YamlIOHandler::readAndAddParametersFromNode(test_int, param_inf);
  YamlIOHandler::readAndAddParametersFromNode(test_double, param_inf);
  YamlIOHandler::readAndAddParametersFromNode(test_bool, param_inf);
  YamlIOHandler::readAndAddParametersFromNode(test_string, param_inf);

  testGetSingleParameter("test_int", 42, int(-32), param_inf, "Int was read incorrectly");
  testGetSingleParameter("test_double", 1.0, 0.0, param_inf, "Double was read incorrectly");
  testGetSingleParameter("test_bool", false, true, param_inf, "Bool was read incorrectly");
  testGetSingleParameter("test_string", std::string("apple"), std::string(""), param_inf, "String was read incorrectly");

  // Test vector parameters
  YAML::Node test_int_vec = YAML::Load("test_int_vec: [ 3, 2, 1 ]");
  std::vector<int> expected_int = { 3, 2, 1 };

  YAML::Node test_double_vec = YAML::Load("test_double_vec: [ 1.3, 4.2, 0.6, 9.3 ]");
  std::vector<double> expected_double = { 1.3, 4.2, 0.6, 9.3 };

  YAML::Node test_bool_vec = YAML::Load("test_bool_vec: [ true, true, false ]");
  std::vector<bool> expected_bool = { true, true, false };

  YAML::Node test_string_vec = YAML::Load("test_string_vec: [ 'apple', 'banana' ]");
  std::vector<std::string> expected_string = { "apple", "banana" };

  YamlIOHandler::readAndAddParametersFromNode(test_int_vec, param_inf);
  YamlIOHandler::readAndAddParametersFromNode(test_double_vec, param_inf);
  YamlIOHandler::readAndAddParametersFromNode(test_bool_vec, param_inf);
  YamlIOHandler::readAndAddParametersFromNode(test_string_vec, param_inf);

  testGetParameterVector("test_int_vec", expected_int, param_inf, "Int vector was read incorrectly");
  testGetParameterVector("test_double_vec", expected_double, param_inf, "Double vector was read incorrectly");
  testGetParameterVector("test_bool_vec", expected_bool, param_inf, "Bool vector was read incorrectly");
  testGetParameterVector("test_string_vec", expected_string, param_inf, "String vector was read incorrectly");
}

}  // namespace test
}  // namespace paraminf
