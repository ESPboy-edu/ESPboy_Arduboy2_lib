#pragma once

#include <stddef.h>
#include <Print.h>

// ----------------------------------------------------------------------------
//  Swap two elements .. 
//
template <typename T> void swap(T& x,T& y) {
	T temp;
	temp = x;
	x = y;
	y = temp;
}


// ----------------------------------------------------------------------------
//  A better absolute as it uses less memory than the standard one .. 
//
template<typename T> T absT(const T & v) {
  
  return (v < 0) ? -v : v;

}


// ----------------------------------------------------------------------------
//  Clamp the value within the range .. 
//
template <typename T> inline T clamp(const T& value, const T& low, const T& high) {

  return (value < low) ? low : (value > high) ? high : value; 

}

/*
// ----------------------------------------------------------------------------
//  Create a new instance of the specified object at the mem address ..
//
inline void * operator new (size_t size, void * pointer) {
	(void)size;
	return pointer;
}


constexpr inline const __FlashStringHelper * FlashString(const char * flashString)
{
	return reinterpret_cast<const __FlashStringHelper*>(flashString);
}
*/