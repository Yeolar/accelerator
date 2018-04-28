/*
 * Copyright 2017 Yeolar
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

/**
 * Stringize.
 */
#define ACC_STRINGIZE(x) #x
#define ACC_STRINGIZE2(x) ACC_STRINGIZE(x)

/**
 * Concatenate.
 */
#define ACC_CONCATENATE_IMPL(s1, s2) s1##s2
#define ACC_CONCATENATE(s1, s2) ACC_CONCATENATE_IMPL(s1, s2)

/**
 * Array size.
 */
#ifndef NELEMS
#define NELEMS(v) (sizeof(v) / sizeof((v)[0]))
#endif

/**
 * Likely.
 */
#undef LIKELY
#undef UNLIKELY

#if defined(__GNUC__) && __GNUC__ >= 4
#define LIKELY(x)   (__builtin_expect((x), 1))
#define UNLIKELY(x) (__builtin_expect((x), 0))
#else
#define LIKELY(x)   (x)
#define UNLIKELY(x) (x)
#endif

/**
 * Attribute.
 */
#define ACC_ALIGNED(size) __attribute__((__aligned__(size)))

/**
 * Warning.
 */
#define ACC_PUSH_WARNING _Pragma("GCC diagnostic push")
#define ACC_POP_WARNING _Pragma("GCC diagnostic pop")
#define ACC_DISABLE_WARNING(warningName)  \
  _Pragma(ACC_STRINGIZE(GCC diagnostic ignored warningName))

/**
 * Conditional arg.
 *
 * ACC_ARG_1_OR_NONE(a)    => NONE
 * ACC_ARG_1_OR_NONE(a, b) => a
 * ACC_ARG_2_OR_1(a)       => a
 * ACC_ARG_2_OR_1(a, b)    => b
 */
#define ACC_ARG_1_OR_NONE(a, ...) ACC_THIRD(a, ## __VA_ARGS__, a)
#define ACC_THIRD(a, b, ...) __VA_ARGS__

#define ACC_ARG_2_OR_1(...) ACC_ARG_2_OR_1_IMPL(__VA_ARGS__, __VA_ARGS__)
#define ACC_ARG_2_OR_1_IMPL(a, b, ...) b

/**
 * Anonymous variable. Introduces an identifier starting with str
 * and ending with a number that varies with the line.
 */
#ifdef __COUNTER__
#define ACC_ANONYMOUS_VARIABLE(str) ACC_CONCATENATE(str, __COUNTER__)
#else
#define ACC_ANONYMOUS_VARIABLE(str) ACC_CONCATENATE(str, __LINE__)
#endif

/**
 * The ACC_NARG macro evaluates to the number of arguments that have been
 * passed to it.
 *
 * Laurent Deniau, "__VA_NARG__," 17 January 2006, <comp.std.c> (29 November 2007).
 */
#define ACC_NARG(...)  ACC_NARG_(__VA_ARGS__, ACC_RSEQ_N())
#define ACC_NARG_(...) ACC_ARG_N(__VA_ARGS__)

#define ACC_ARG_N( \
   _1, _2, _3, _4, _5, _6, _7, _8, _9,_10,_11,_12,_13,_14,_15,_16, \
  _17,_18,_19,_20,_21,_22,_23,_24,_25,_26,_27,_28,_29,_30,_31,_32, \
  _33,_34,_35,_36,_37,_38,_39,_40,_41,_42,_43,_44,_45,_46,_47,_48, \
  _49,_50,_51,_52,_53,_54,_55,_56,_57,_58,_59,_60,_61,_62,_63,N,...) N

#define ACC_RSEQ_N() \
  63,62,61,60,59,58,57,56,55,54,53,52,51,50,49,48, \
  47,46,45,44,43,42,41,40,39,38,37,36,35,34,33,32, \
  31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16, \
  15,14,13,12,11,10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

/**
 * APPLYXn variadic X-Macro by M Joshua Ryan
 * Free for all uses. Don't be a jerk.
 */
#define ACC_APPLYX1(X,a) \
  X(a)
#define ACC_APPLYX2(X,a,b) \
  X(a) X(b)
#define ACC_APPLYX3(X,a,b,c) \
  X(a) X(b) X(c)
#define ACC_APPLYX4(X,a,b,c,d) \
  X(a) X(b) X(c) X(d)
#define ACC_APPLYX5(X,a,b,c,d,e) \
  X(a) X(b) X(c) X(d) X(e)
#define ACC_APPLYX6(X,a,b,c,d,e,f) \
  X(a) X(b) X(c) X(d) X(e) X(f)
#define ACC_APPLYX7(X,a,b,c,d,e,f,g) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g)
#define ACC_APPLYX8(X,a,b,c,d,e,f,g,h) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h)
#define ACC_APPLYX9(X,a,b,c,d,e,f,g,h,i) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i)
#define ACC_APPLYX10(X,a,b,c,d,e,f,g,h,i,j) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j)
#define ACC_APPLYX11(X,a,b,c,d,e,f,g,h,i,j,k) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k)
#define ACC_APPLYX12(X,a,b,c,d,e,f,g,h,i,j,k,l) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l)
#define ACC_APPLYX13(X,a,b,c,d,e,f,g,h,i,j,k,l,m) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m)
#define ACC_APPLYX14(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n)
#define ACC_APPLYX15(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o)
#define ACC_APPLYX16(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p)
#define ACC_APPLYX17(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q)
#define ACC_APPLYX18(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r)
#define ACC_APPLYX19(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r) X(s)
#define ACC_APPLYX20(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r) X(s) X(t)
#define ACC_APPLYX21(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r) X(s) X(t) X(u)
#define ACC_APPLYX22(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r) X(s) X(t) X(u) X(v)
#define ACC_APPLYX23(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r) X(s) X(t) X(u) X(v) X(w)
#define ACC_APPLYX24(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r) X(s) X(t) X(u) X(v) X(w) X(x)
#define ACC_APPLYX25(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r) X(s) X(t) X(u) X(v) X(w) X(x) X(y)
#define ACC_APPLYX26(X,a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z) \
  X(a) X(b) X(c) X(d) X(e) X(f) X(g) X(h) X(i) X(j) X(k) X(l) X(m) X(n) X(o) \
  X(p) X(q) X(r) X(s) X(t) X(u) X(v) X(w) X(x) X(y) X(z)
#define ACC_APPLYX_(M, ...) M(__VA_ARGS__)
#define ACC_APPLYXn(X, ...) \
  ACC_APPLYX_(ACC_CONCATENATE(ACC_APPLYX, ACC_NARG(__VA_ARGS__)), \
              X, __VA_ARGS__)

//////////////////////////////////////////////////////////////////////

/**
 * GCC compatible wrappers around clang attributes.
 *
 * @author Dominik Gabi
 */

#ifndef __has_attribute
#define ACC_HAS_ATTRIBUTE(x) 0
#else
#define ACC_HAS_ATTRIBUTE(x) __has_attribute(x)
#endif

#ifndef __has_cpp_attribute
#define ACC_HAS_CPP_ATTRIBUTE(x) 0
#else
#define ACC_HAS_CPP_ATTRIBUTE(x) __has_cpp_attribute(x)
#endif

#ifndef __has_extension
#define ACC_HAS_EXTENSION(x) 0
#else
#define ACC_HAS_EXTENSION(x) __has_extension(x)
#endif

/**
 * Fallthrough to indicate that `break` was left out on purpose in a switch
 * statement, e.g.
 *
 * switch (n) {
 *   case 22:
 *   case 33:  // no warning: no statements between case labels
 *     f();
 *   case 44:  // warning: unannotated fall-through
 *     g();
 *     ACC_FALLTHROUGH; // no warning: annotated fall-through
 * }
 */
#if ACC_HAS_CPP_ATTRIBUTE(fallthrough)
#define ACC_FALLTHROUGH [[fallthrough]]
#elif ACC_HAS_CPP_ATTRIBUTE(clang::fallthrough)
#define ACC_FALLTHROUGH [[clang::fallthrough]]
#elif ACC_HAS_CPP_ATTRIBUTE(gnu::fallthrough)
#define ACC_FALLTHROUGH [[gnu::fallthrough]]
#else
#define ACC_FALLTHROUGH
#endif

/**
 *  Maybe_unused indicates that a function, variable or parameter might or
 *  might not be used, e.g.
 *
 *  int foo(ACC_MAYBE_UNUSED int x) {
 *    #ifdef USE_X
 *      return x;
 *    #else
 *      return 0;
 *    #endif
 *  }
 */
#if ACC_HAS_CPP_ATTRIBUTE(maybe_unused)
#define ACC_MAYBE_UNUSED [[maybe_unused]]
#elif ACC_HAS_ATTRIBUTE(__unused__) || __GNUC__
#define ACC_MAYBE_UNUSED __attribute__((__unused__))
#else
#define ACC_MAYBE_UNUSED
#endif

/**
 * Nullable indicates that a return value or a parameter may be a `nullptr`,
 * e.g.
 *
 * int* ACC_NULLABLE foo(int* a, int* ACC_NULLABLE b) {
 *   if (*a > 0) {  // safe dereference
 *     return nullptr;
 *   }
 *   if (*b < 0) {  // unsafe dereference
 *     return *a;
 *   }
 *   if (b != nullptr && *b == 1) {  // safe checked dereference
 *     return new int(1);
 *   }
 *   return nullptr;
 * }
 */
#if ACC_HAS_EXTENSION(nullability)
#define ACC_NULLABLE _Nullable
#define ACC_NONNULL _Nonnull
#else
#define ACC_NULLABLE
#define ACC_NONNULL
#endif

/**
 * "Cold" indicates to the compiler that a function is only expected to be
 * called from unlikely code paths. It can affect decisions made by the
 * optimizer both when processing the function body and when analyzing
 * call-sites.
 */
#if __GNUC__
#define ACC_COLD __attribute__((__cold__))
#else
#define ACC_COLD
#endif

