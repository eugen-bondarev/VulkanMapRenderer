#pragma once

namespace Engine
{
	namespace Util
	{
		namespace Mem
		{
			namespace Aligned
			{
				extern uint32_t minUniformBufferOffsetAlignment;

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

	template <typename T>
	struct Aligned
	{
		T* data;

		inline static uint32_t dynamicAlignment;

		template <typename T1>
		size_t CalculateDynamicAlignment()
		{
			if (Util::Mem::Aligned::minUniformBufferOffsetAlignment > 0) 
			{
				return (sizeof(T1) + Util::Mem::Aligned::minUniformBufferOffsetAlignment - 1) & ~(Util::Mem::Aligned::minUniformBufferOffsetAlignment - 1);
			}

			return sizeof(T1);
		}

		Aligned(uint32_t amount_of_instances)
		{
			dynamicAlignment = CalculateDynamicAlignment<T>();
			data = Util::Mem::Aligned::Alloc<T>(amount_of_instances * dynamicAlignment, dynamicAlignment);
		}

		~Aligned()
		{
			Util::Mem::Aligned::Free(data);
		}

		T& operator[](int i)
		{		
			T& item = *(T*)(((uint64_t)data + (i * dynamicAlignment)));

			return item;
		}

		const T& operator[](int i) const
		{		
			T& item = *(T*)(((uint64_t)data + (i * dynamicAlignment)));

			return item;
		}
	};
}