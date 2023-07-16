#pragma once

//
// Copyright (C) 2019 Pharap (@Pharap)
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

// Include __FlashStringHelper
#include <WString.h>

//
// FlashStringHelper
//

using FlashStringHelper = const __FlashStringHelper *;

//
// asFlashStringHelper
//

inline FlashStringHelper asFlashStringHelper(const char * flashString) noexcept
{
	return static_cast<FlashStringHelper>(static_cast<const void *>(flashString));
}

inline FlashStringHelper asFlashStringHelper(const unsigned char * flashString) noexcept
{
	return static_cast<FlashStringHelper>(static_cast<const void *>(flashString));
}

inline FlashStringHelper asFlashStringHelper(const signed char * flashString) noexcept
{
	return static_cast<FlashStringHelper>(static_cast<const void *>(flashString));
}

// Include pgm_read_ptr
#include <avr/pgmspace.h>

//
// readFlashString
//

inline FlashStringHelper readFlashStringPointer(const char * const * flashString) noexcept
{
	return static_cast<FlashStringHelper>(pgm_read_ptr(flashString));
}

inline FlashStringHelper readFlashStringPointer(const unsigned char * const * flashString) noexcept
{
	return static_cast<FlashStringHelper>(pgm_read_ptr(flashString));
}

inline FlashStringHelper readFlashStringPointer(const signed char * const * flashString) noexcept
{
	return static_cast<FlashStringHelper>(pgm_read_ptr(flashString));
}
