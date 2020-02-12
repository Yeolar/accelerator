/*
 * Copyright 2012-present Facebook, Inc.
 * Copyright 2017-present Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/**
 * Necessarily evil preprocessor-related amenities.
 */

/**
 * ACC_ONE_OR_NONE(hello, world) expands to hello and
 * ACC_ONE_OR_NONE(hello) expands to nothing. This macro is used to
 * insert or eliminate text based on the presence of another argument.
 */
#define ACC_ONE_OR_NONE(a, ...) ACC_THIRD(a, ## __VA_ARGS__, a)
#define ACC_THIRD(a, b, ...) __VA_ARGS__

/**
 * Helper macro that extracts the first argument out of a list of any
 * number of arguments.
 */
#define ACC_ARG_1(a, ...) a

/**
 * Helper macro that extracts the second argument out of a list of any
 * number of arguments. If only one argument is given, it returns
 * that.
 */
#define ACC_ARG_2_OR_1(...) ACC_ARG_2_OR_1_IMPL(__VA_ARGS__, __VA_ARGS__)
#define ACC_ARG_2_OR_1_IMPL(a, b, ...) b

/**
 * Helper macro that provides a way to pass argument with commas in it to
 * some other macro whose syntax doesn't allow using extra parentheses.
 * Example:
 *
 *   #define MACRO(type, name) type name
 *   MACRO(ACC_SINGLE_ARG(std::pair<size_t, size_t>), x);
 *
 */
#define ACC_SINGLE_ARG(...) __VA_ARGS__

/**
 * ACC_ANONYMOUS_VARIABLE(str) introduces an identifier starting with
 * str and ending with a number that varies with the line.
 */
#ifndef ACC_ANONYMOUS_VARIABLE
#define ACC_CONCATENATE_IMPL(s1, s2) s1##s2
#define ACC_CONCATENATE(s1, s2) ACC_CONCATENATE_IMPL(s1, s2)
#ifdef __COUNTER__
#define ACC_ANONYMOUS_VARIABLE(str) ACC_CONCATENATE(str, __COUNTER__)
#else
#define ACC_ANONYMOUS_VARIABLE(str) ACC_CONCATENATE(str, __LINE__)
#endif
#endif

/**
 * Use ACC_STRINGIZE(x) when you'd want to do what #x does inside
 * another macro expansion.
 */
#define ACC_STRINGIZE(x) #x
#define ACC_STRINGIZE2(x) ACC_STRINGIZE(x)

/**
 * The ACC_NARG macro evaluates to the number of arguments that have been
 * passed to it.
 *
 * Laurent Deniau, "__VA_NARG__," 17 January 2006, <comp.std.c> (29 November 2007).
 */
#define ACC_NARG_(dummy, _9, _8, _7, _6, _5, _4, _3, _2, _1, _0, ...) _0
#define ACC_NARG(...) \
        ACC_NARG_(dummy, ##__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

/**
 * APPLYXn variadic X-Macro.
 */
#define ACC_APPLYX0(X, ...)
#define ACC_APPLYX1(X, a, ...) X(a) ACC_APPLYX0(X, __VA_ARGS__)
#define ACC_APPLYX2(X, a, ...) X(a) ACC_APPLYX1(X, __VA_ARGS__)
#define ACC_APPLYX3(X, a, ...) X(a) ACC_APPLYX2(X, __VA_ARGS__)
#define ACC_APPLYX4(X, a, ...) X(a) ACC_APPLYX3(X, __VA_ARGS__)
#define ACC_APPLYX5(X, a, ...) X(a) ACC_APPLYX4(X, __VA_ARGS__)
#define ACC_APPLYX6(X, a, ...) X(a) ACC_APPLYX5(X, __VA_ARGS__)
#define ACC_APPLYX7(X, a, ...) X(a) ACC_APPLYX6(X, __VA_ARGS__)
#define ACC_APPLYX8(X, a, ...) X(a) ACC_APPLYX7(X, __VA_ARGS__)
#define ACC_APPLYX9(X, a, ...) X(a) ACC_APPLYX8(X, __VA_ARGS__)
#define ACC_APPLYXi(X, n, ...) ACC_APPLYX##n(X, __VA_ARGS__)
#define ACC_APPLYX_(X, n, ...) ACC_APPLYXi(X, n, __VA_ARGS__)

#define ACC_APPLYXn(X, ...) ACC_APPLYX_(X, ACC_NARG(__VA_ARGS__), __VA_ARGS__)

/**
 * APPLYFn variadic F-Macro.
 */
#define ACC_APPLYF0(x, ...)
#define ACC_APPLYF1(x, A, ...) A(x) ACC_APPLYF0(x, __VA_ARGS__)
#define ACC_APPLYF2(x, A, ...) A(x) ACC_APPLYF1(x, __VA_ARGS__)
#define ACC_APPLYF3(x, A, ...) A(x) ACC_APPLYF2(x, __VA_ARGS__)
#define ACC_APPLYF4(x, A, ...) A(x) ACC_APPLYF3(x, __VA_ARGS__)
#define ACC_APPLYF5(x, A, ...) A(x) ACC_APPLYF4(x, __VA_ARGS__)
#define ACC_APPLYF6(x, A, ...) A(x) ACC_APPLYF5(x, __VA_ARGS__)
#define ACC_APPLYF7(x, A, ...) A(x) ACC_APPLYF6(x, __VA_ARGS__)
#define ACC_APPLYF8(x, A, ...) A(x) ACC_APPLYF7(x, __VA_ARGS__)
#define ACC_APPLYF9(x, A, ...) A(x) ACC_APPLYF8(x, __VA_ARGS__)
#define ACC_APPLYFi(x, n, ...) ACC_APPLYF##n(x, __VA_ARGS__)
#define ACC_APPLYF_(x, n, ...) ACC_APPLYFi(x, n, __VA_ARGS__)

#define ACC_APPLYFn(x, ...) ACC_APPLYF_(x, ACC_NARG(__VA_ARGS__), __VA_ARGS__)

////////////////////////////////////////////////////////////

// Generalize warning push/pop.
#define ACC_PUSH_WARNING _Pragma("GCC diagnostic push")
#define ACC_POP_WARNING _Pragma("GCC diagnostic pop")
#define ACC_DISABLE_WARNING_INTERNAL2(warningName) #warningName
#define ACC_DISABLE_WARNING(warningName) \
  _Pragma(ACC_DISABLE_WARNING_INTERNAL2(GCC diagnostic ignored warningName))

////////////////////////////////////////////////////////////

#if __GNUC__
#define ACC_DETAIL_BUILTIN_EXPECT(b, t) (__builtin_expect(b, t))
#else
#define ACC_DETAIL_BUILTIN_EXPECT(b, t) b
#endif

//  Likeliness annotations
//
//  Useful when the author has better knowledge than the compiler of whether
//  the branch condition is overwhelmingly likely to take a specific value.
//
//  Useful when the author has better knowledge than the compiler of which code
//  paths are designed as the fast path and which are designed as the slow path,
//  and to force the compiler to optimize for the fast path, even when it is not
//  overwhelmingly likely.

#define ACC_LIKELY(x) ACC_DETAIL_BUILTIN_EXPECT((x), 1)
#define ACC_UNLIKELY(x) ACC_DETAIL_BUILTIN_EXPECT((x), 0)

