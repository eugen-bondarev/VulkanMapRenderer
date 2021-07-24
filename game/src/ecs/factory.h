#pragma once

#include <functional>
#include <string>
#include <map>

template<typename T>
struct TypeParseTraits;

#define REGISTER_PARSE_TYPE(X) template <> struct TypeParseTraits<X> \
    { static const char* name; }; const char* TypeParseTraits<X>::name = #X

extern std::map<std::string, std::function<void*()>> s_Factories;

std::function<void*()> GetFactory(const std::string& name);

#define REGISTER(type)\
class Add##type\
{\
public:\
	Add##type()\
	{\
		if (s_Factories.find(#type) == s_Factories.end())\
		{\
			s_Factories[#type] = [&] { return new type(); };\
		}\
	}\
};\
\
inline static Add##type _Add##type