#pragma once

//
//  Copyright (C) 2020 Pharap (@Pharap)
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//       http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//

#include <stddef.h>
#include <stdint.h>

//
// Warning:
// This class doesn't support move semantics or types that don't have default constructors,
// so be sure to only use it with trivial types that have default constructors.
//

template<typename Type, size_t Capacity>
class List
{
public:
	static_assert((Capacity < 128), "Capacity must be less than 128");

	using ItemType = Type;
	using IndexType = uint8_t;
	using IndexOfType = int8_t;

	using ValueType = Type;
	using Reference = ValueType&;
	using ConstReference = const ValueType&;

	static constexpr size_t capacity = Capacity;
	static constexpr IndexType firstIndex = 0;
	static constexpr IndexType lastIndex = Capacity - 1;

private:
	ItemType items[Capacity];
	IndexType next;

public:
	constexpr List() :
		items {}, next { 0 }
	{
	}
	
	// O(1)
	constexpr IndexType getCapacity() const
	{
		return static_cast<IndexType>(capacity);
	}

	// O(1)
	constexpr IndexType getCount() const
	{
		return this->next;
	}

	// O(1)
	constexpr bool isEmpty() const
	{
		return (this->next == firstIndex);
	}

	// O(1)
	constexpr bool isFull() const
	{
		return (this->next == this->getCapacity());
	}

	// O(1)
	ItemType & operator [] (IndexType index)
	{
		// assert(index < this->getCapacity());
		// assert(index < this->next);

		return this->items[index];
	}
	
	// O(1)
	const ItemType & operator [] (IndexType index) const
	{
		// assert(index < this->getCapacity());
		// assert(index < this->next);

		return this->items[index];
	}

	// O(1)
	bool add(const ItemType & item)
	{
		if (this->isFull())
			return false;

		this->items[this->next] = item;
		++this->next;
		return true;
	}
	
	// O(n)
	bool insert(IndexType index, const ItemType & item)
	{
		// assert(index < this->getCapacity());
		// assert(index < this->next);

		if (this->isFull())
			return false;

		// Increase capacity
		++this->next;
		
		// Move items up to make room
		for (IndexType i = index + 1; i < this->next; ++i)
			this->items[i] = this->items[i - 1];
		
		// Copy the new item into place
		this->items[index] = item;
		return true;
	}
	
	// O(n)
	bool remove(const ItemType & item)
	{
		for(IndexType index = 0; index < this->next; ++index)
		{
			if (this->items[index] != item)
				continue;

			// Decrease capacity
			--this->next;
			
			// Move everything down
			while (index < this->next)
			{
				this->items[index] = this->items[index + 1];
				++index;
			}
			
			// Destroy the last item
			this->items[this->next].~ItemType();
			return true;
		}

		return false;
	}
	
	// O(n)
	void removeAt(IndexType index)
	{
		// assert(index < this->getCapacity());
		// assert(index < this->next);

		// Decrease capacity
		--this->next;

		// Move all items down
		for (IndexType i = index; i < this->next; ++i)
				this->items[i] = this->items[i + 1];

		// Destroy the last item
		this->items[this->next].~ItemType();
	}
	
	// O(n)
	void clear()
	{
		// Destroy all items
		for (IndexType i = 0; i < this->next; ++i)
			this->items[i].~ItemType();

		// Reset capacity
		this->next = 0;
	}
	
	// O(n)
	bool contains(const ItemType & item) const
	{
		return this->getIndexOf(item) != -1;
	}

	// O(n)
	// Returns -1 if item not found
	IndexOfType getIndexOf(const ItemType & item) const
	{
		// Do a manual search
		for (IndexType index = 0; index < this->next; ++index)
			if (this->items[index] == item)
				return static_cast<IndexOfType>(index);

		// If no match was found, return -1
		return -1;
	}
};