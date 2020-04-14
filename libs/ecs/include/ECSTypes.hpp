#pragma once

#include <bitset>
#include <cinttypes>

typedef std::uint32_t Entity;
typedef std::uint8_t  ComponentType;

constexpr ComponentType MAX_COMPONENTS =   32;
constexpr Entity        MAX_ENTITIES   = 2000;

typedef std::bitset<MAX_COMPONENTS> Signature;
