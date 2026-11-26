#pragma once

#include "Engine/Core/Config.h"

#include <cstdint>
#include <utility>

#define ENGINE_EXPAND_MACRO(x) x
#define ENGINE_STRINGIFY_MACRO(x) #x

#define BIT(index) (std::uint32_t{1} << (index))
#define BIND_EVENT_FN(fn) [this](auto &&... args) ->decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

#include "Engine/Core/Memory.h"
#include "Engine/Core/Log.h"
#include "Engine/Core/Assert.h"