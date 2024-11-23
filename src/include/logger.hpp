#pragma once
#include "code_segment.hpp"

struct Logger {
    static void log(CodeSegment const& code_pair);
};