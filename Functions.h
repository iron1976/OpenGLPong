#pragma once
#include <iostream>
#include <typeinfo>
#include <string>
#include <array>

#define CheckErrorGL(x) x;\
{auto error = glGetError(); while (error != 0){print("|"<< "error: " << error << "|" << __FILE__ << ":"<<__LINE__); DebugBreak();} }


#define print(x) std::cout << x << std::endl

