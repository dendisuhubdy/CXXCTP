﻿#pragma once

#include "CXTPL_STD.hpp"
#include "CXXCTP_STD.hpp"

#include "../../resources/cxtpl/typeclass_instance_gen_hpp.cxtpl.hpp"
#include "../../resources/cxtpl/typeclass_instance_gen_cpp.cxtpl.hpp"

const char* typeclass_instance(
    const clang::ast_matchers::MatchFinder::MatchResult& matchResult,
    clang::Rewriter& rewriter,
    const clang::Decl* decl,
    const std::vector<cxxctp::parsed_func>& args);