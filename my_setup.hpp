#pragma once

#include <vector>
#include <string>

#include "type_name.hpp"

#ifndef __PRETTY_FUNCTION__
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif // __PRETTY_FUNCTION_

#ifdef __TRACCE_COLLECTOR__
#ifndef __COUT_FUNC_TRACE__
#define __COUT_FUNC_TRACE__(x) std::cout << __PRETTY_FUNCTION__ << ": " << (x) << std::endl; __TRACCE_COLLECTOR__.push_back( std::move(x) );
#endif // __COUT_FUNC_TRACE__
#else
#ifndef __COUT_FUNC_TRACE__
#define __COUT_FUNC_TRACE__(x) std::cout << __PRETTY_FUNCTION__ << ": " << (x) << std::endl; 
#endif // __COUT_FUNC_TRACE__
#endif //__TRACCE_COLLECTOR__