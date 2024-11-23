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

// VerilogParser class implements Parser interface.
// Its implementation is based on verilog_parse() function
// that is generated by Bison.
//
// See verilog.y for parser specification.

#ifndef VERIBLE_VERILOG_PARSER_VERILOG_PARSER_H_
#define VERIBLE_VERILOG_PARSER_VERILOG_PARSER_H_

#include <cstddef>

#include "verible/common/parser/bison-parser-adapter.h"
#include "verible/common/parser/parser-param.h"

namespace verilog {

// parser wrapper to enable debug traces
int verilog_parse_wrapper(::verible::ParserParam *);

// TODO(fangism): Use a pseudo-preprocessor between the lexer and parser.
using VerilogParser = verible::BisonParserAdapter<verilog_parse_wrapper>;

// Lookup token/symbol name by token enum index. These names are suitable for
// use in human facing text (like error messages).
// Defined in verilog.y, emitted in verilog.tab.cc.
//
// See also: TokenTypeToString() in verilog_token.h
const char *verilog_symbol_name(size_t symbol_enum);

}  // namespace verilog

#endif  // VERIBLE_VERILOG_PARSER_VERILOG_PARSER_H_