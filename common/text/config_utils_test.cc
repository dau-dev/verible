// Copyright 2017-2020 The Verible Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "common/text/config_utils.h"

#include "gtest/gtest.h"
#include "absl/strings/match.h"

namespace verible {
namespace config {

TEST(ConfigUtilsTest, ComplainInvalidParameter) {
  absl::Status s;
  // singular ...
  s = ParseNameValues("baz:123", {{"foo", nullptr}});
  EXPECT_FALSE(s.ok());
  EXPECT_EQ(s.message(),
            "baz: unknown parameter; supported "
            "parameter is 'foo'");

  // plural.
  s = ParseNameValues("baz:123", {{"foo", nullptr}, {"bar", nullptr}});
  EXPECT_FALSE(s.ok());
  EXPECT_EQ(s.message(),
            "baz: unknown parameter; supported "
            "parameters are 'foo', 'bar'");

  s = ParseNameValues("foo:123", {{"foo", nullptr}, {"bar", nullptr}});
  EXPECT_TRUE(s.ok());
}

TEST(ConfigUtilsTest, ParseInteger) {
  absl::Status s;
  int value = -1;
  s = ParseNameValues("baz:42", {{"baz", SetInt(&value, 0, 100)}});
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(value, 42);

  s = ParseNameValues("baz:fourtytwo", {{"baz", SetInt(&value, 0, 100)}});
  EXPECT_FALSE(s.ok());
  // would be cool though :)
  EXPECT_EQ(s.message(), "baz: 'fourtytwo': Cannot parse integer");

  s = ParseNameValues("baz:142", {{"baz", SetInt(&value, 0, 100)}});
  EXPECT_FALSE(s.ok());
  EXPECT_EQ(s.message(), "baz: 142 out of range [0...100]");

  s = ParseNameValues("baz:-1", {{"baz", SetInt(&value, 0, 100)}});
  EXPECT_FALSE(s.ok());
  EXPECT_EQ(s.message(), "baz: -1 out of range [0...100]");

  s = ParseNameValues("baz:-12345", {{"baz", SetInt(&value)}});
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(value, -12345);
}

TEST(ConfigUtilsTest, ParseBool) {
  absl::Status s;
  bool value = false;
  for (auto config : {"baz", "baz:TrUe", "baz:on", "baz:1"}) {
    s = ParseNameValues(config, {{"baz", SetBool(&value)}});
    EXPECT_TRUE(s.ok());
    EXPECT_TRUE(value);
  }

  for (auto config : {"baz:fAlse", "baz:off", "baz:0"}) {
    s = ParseNameValues(config, {{"baz", SetBool(&value)}});
    EXPECT_TRUE(s.ok());
    EXPECT_FALSE(value);
  }

  s = ParseNameValues("baz:foobar", {{"baz", SetBool(&value)}});
  EXPECT_FALSE(s.ok());
  EXPECT_TRUE(
      absl::StartsWith(s.message(), "baz: Boolean value should be one of"));
}

TEST(ConfigUtilsTest, ParseString) {
  absl::Status s;
  std::string str;
  s = ParseNameValues("baz:hello", {{"baz", SetString(&str)}});
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(str, "hello");

  s = ParseNameValues("baz:hello",
                      {{"baz", SetStringOneOf(&str, {"hello", "world"})}});
  EXPECT_TRUE(s.ok());
  EXPECT_EQ(str, "hello");

  // Selection from multiple strings
  s = ParseNameValues("baz:greetings",
                      {{"baz", SetStringOneOf(&str, {"hello", "world"})}});
  EXPECT_FALSE(s.ok());
  EXPECT_EQ(s.message(),
            "baz: Value can only be one of ['hello', 'world']; "
            "got 'greetings'");

  // Selection from one string
  s = ParseNameValues("baz:greetings",
                      {{"baz", SetStringOneOf(&str, {"hello"})}});
  EXPECT_FALSE(s.ok());
  EXPECT_EQ(s.message(), "baz: Value can only be 'hello'; got 'greetings'");
}

TEST(ConfigUtilsTest, ParseMultipleParameters) {
  absl::Status s;
  int answer;
  bool panic;
  s = ParseNameValues("answer:42;panic:off", {{"answer", SetInt(&answer)},
                                              {"panic", SetBool(&panic)}});
  EXPECT_TRUE(s.ok());
  EXPECT_FALSE(panic);
  EXPECT_EQ(answer, 42);

  s = ParseNameValues("answer:43;panic:on", {{"answer", SetInt(&answer)},
                                             {"panic", SetBool(&panic)}});
  EXPECT_TRUE(s.ok()) << s.message();
  EXPECT_TRUE(panic);
  EXPECT_EQ(answer, 43);
}
}  // namespace config
}  // namespace verible