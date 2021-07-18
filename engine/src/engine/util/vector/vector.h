#pragma once

#include <vector>

namespace Engine
{
	namespace Util
	{
		namespace Vector
		{
			template <typename T>
			void Merge(std::vector<T>& result, const std::vector<T>& a, const std::vector<T>& b)
			{
				result.resize(a.size() + b.size());

				memcpy(&result[0], a.data(), a.size() * sizeof(T));
				memcpy(&result[a.size()], b.data(), b.size() * sizeof(T));
			}

			template <typename T>
			std::vector<T> Merge(const std::vector<T>& a, const std::vector<T>& b)
			{
				std::vector<T> result;
				result.resize(a.size() + b.size());
				memcpy(&result[0], a.data(), a.size() * sizeof(T));
				memcpy(&result[a.size()], b.data(), b.size() * sizeof(T));
				return result;
			}
		}
	}
}