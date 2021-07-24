#include "factory.h"

std::map<std::string, std::function<void*()>> s_Factories;

std::function<void*()> GetFactory(const std::string& name)
{
	return s_Factories[name];
}