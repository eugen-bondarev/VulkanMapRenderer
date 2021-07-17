#pragma once

namespace Engine
{
	namespace Util
	{
		namespace Mem
		{
			namespace Aligned
			{
				template <typename T>
				T* Alloc(size_t size, size_t alignment)
				{
					void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
					data = _aligned_malloc(size, alignment);
#else
					int res = posix_memalign(&data, alignment, size);
					if (res != 0)
						data = nullptr;
#endif
					return (T*) data;
				}

				void Free(void *data);
			}
		}
	}
}