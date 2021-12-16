#pragma once

namespace Engine
{
	template<typename Container>
	inline bool isValidIndex(size_t index, const Container& container)
	{
		return index >= 0 && index < container.size();
	}

	//template<typename Container>
	//inline const sp<typename Container::value_type>& getAtIndexSafe(size_t index, const Container& container)
	//{
	//	return isValidIndex(index, container) ? container[index] : nullptr;
	//}


}