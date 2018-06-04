#pragma once

#include <iostream>
#include <memory>
#include <vector>
#include <type_traits>

typedef char				s8;
typedef short				s16;
typedef int					s32;
typedef long long			s64;
typedef ptrdiff_t			ssize;

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef size_t				usize;

static_assert(sizeof(s8) == 1, "s8 wasn't right size");
static_assert(sizeof(s16) == 2, "s16 wasn't right size");
static_assert(sizeof(s32) == 4, "s32 wasn't right size");
static_assert(sizeof(s64) == 8, "s64 wasn't right size");

static_assert(sizeof(u8) == 1, "u8 wasn't right size");
static_assert(sizeof(u16) == 2, "u16 wasn't right size");
static_assert(sizeof(u32) == 4, "u32 wasn't right size");
static_assert(sizeof(u64) == 8, "u64 wasn't right size");

#ifdef _DEBUG 
#define Rk_Assert(cond) if (!(cond)) __debugbreak();
#else
#define Rk_Assert(cond)
#endif