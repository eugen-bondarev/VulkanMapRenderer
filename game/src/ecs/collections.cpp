#include "collections.h"

#define ADD_TO_COLLECTION(X)\
	if (X* element_of_type = dynamic_cast<X*>(element))\
		X::collection.push_back(element_of_type)

#define REMOVE_FROM_COLLECTION(X)\
	if (X* element_of_type = dynamic_cast<X*>(element))\
		for (int i = 0; i < X::collection.size(); i++)\
			if (X::collection[i] == element_of_type)\
				Engine::Util::Vector::RemoveAt(X::collection, i)

namespace Collections
{
	void AddToCollection(Component* element)
	{
		ADD_TO_COLLECTION(IRenderable);
		ADD_TO_COLLECTION(IOnUpdate);
	}

	void RemoveFromCollection(Component* element)
	{
		REMOVE_FROM_COLLECTION(IRenderable);
		REMOVE_FROM_COLLECTION(IOnUpdate);
	}
}