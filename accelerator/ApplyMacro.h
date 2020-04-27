/*
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
 * The ACC_NARG macro evaluates to the number of arguments that have been
 * passed to it.
 *
 * Laurent Deniau, "__VA_NARG__," 17 January 2006, <comp.std.c> (29 November 2007).
 */
#define ACC_NARG_(dummy,                  \
  _99,_98,_97,_96,_95,_94,_93,_92,_91,_90,\
  _89,_88,_87,_86,_85,_84,_83,_82,_81,_80,\
  _79,_78,_77,_76,_75,_74,_73,_72,_71,_70,\
  _69,_68,_67,_66,_65,_64,_63,_62,_61,_60,\
  _59,_58,_57,_56,_55,_54,_53,_52,_51,_50,\
  _49,_48,_47,_46,_45,_44,_43,_42,_41,_40,\
  _39,_38,_37,_36,_35,_34,_33,_32,_31,_30,\
  _29,_28,_27,_26,_25,_24,_23,_22,_21,_20,\
  _19,_18,_17,_16,_15,_14,_13,_12,_11,_10,\
   _9, _8, _7, _6, _5, _4, _3, _2, _1, _0,...) _0

#define ACC_NARG(...)                     \
  ACC_NARG_(dummy, ##__VA_ARGS__,         \
  99, 98, 97, 96, 95, 94, 93, 92, 91, 90, \
  89, 88, 87, 86, 85, 84, 83, 82, 81, 80, \
  79, 78, 77, 76, 75, 74, 73, 72, 71, 70, \
  69, 68, 67, 66, 65, 64, 63, 62, 61, 60, \
  59, 58, 57, 56, 55, 54, 53, 52, 51, 50, \
  49, 48, 47, 46, 45, 44, 43, 42, 41, 40, \
  39, 38, 37, 36, 35, 34, 33, 32, 31, 30, \
  29, 28, 27, 26, 25, 24, 23, 22, 21, 20, \
  19, 18, 17, 16, 15, 14, 13, 12, 11, 10, \
   9,  8,  7,  6,  5,  4,  3,  2,  1,  0)

/**
 * APPLYXn variadic X-Macro.
 */
#define ACC_APPLYX0 (X, ...)
#define ACC_APPLYX1 (X, a, ...) X(a) ACC_APPLYX0 (X, __VA_ARGS__)
#define ACC_APPLYX2 (X, a, ...) X(a) ACC_APPLYX1 (X, __VA_ARGS__)
#define ACC_APPLYX3 (X, a, ...) X(a) ACC_APPLYX2 (X, __VA_ARGS__)
#define ACC_APPLYX4 (X, a, ...) X(a) ACC_APPLYX3 (X, __VA_ARGS__)
#define ACC_APPLYX5 (X, a, ...) X(a) ACC_APPLYX4 (X, __VA_ARGS__)
#define ACC_APPLYX6 (X, a, ...) X(a) ACC_APPLYX5 (X, __VA_ARGS__)
#define ACC_APPLYX7 (X, a, ...) X(a) ACC_APPLYX6 (X, __VA_ARGS__)
#define ACC_APPLYX8 (X, a, ...) X(a) ACC_APPLYX7 (X, __VA_ARGS__)
#define ACC_APPLYX9 (X, a, ...) X(a) ACC_APPLYX8 (X, __VA_ARGS__)

#define ACC_APPLYX10(X, a, ...) X(a) ACC_APPLYX9 (X, __VA_ARGS__)
#define ACC_APPLYX11(X, a, ...) X(a) ACC_APPLYX10(X, __VA_ARGS__)
#define ACC_APPLYX12(X, a, ...) X(a) ACC_APPLYX11(X, __VA_ARGS__)
#define ACC_APPLYX13(X, a, ...) X(a) ACC_APPLYX12(X, __VA_ARGS__)
#define ACC_APPLYX14(X, a, ...) X(a) ACC_APPLYX13(X, __VA_ARGS__)
#define ACC_APPLYX15(X, a, ...) X(a) ACC_APPLYX14(X, __VA_ARGS__)
#define ACC_APPLYX16(X, a, ...) X(a) ACC_APPLYX15(X, __VA_ARGS__)
#define ACC_APPLYX17(X, a, ...) X(a) ACC_APPLYX16(X, __VA_ARGS__)
#define ACC_APPLYX18(X, a, ...) X(a) ACC_APPLYX17(X, __VA_ARGS__)
#define ACC_APPLYX19(X, a, ...) X(a) ACC_APPLYX18(X, __VA_ARGS__)

#define ACC_APPLYX20(X, a, ...) X(a) ACC_APPLYX19(X, __VA_ARGS__)
#define ACC_APPLYX21(X, a, ...) X(a) ACC_APPLYX20(X, __VA_ARGS__)
#define ACC_APPLYX22(X, a, ...) X(a) ACC_APPLYX21(X, __VA_ARGS__)
#define ACC_APPLYX23(X, a, ...) X(a) ACC_APPLYX22(X, __VA_ARGS__)
#define ACC_APPLYX24(X, a, ...) X(a) ACC_APPLYX23(X, __VA_ARGS__)
#define ACC_APPLYX25(X, a, ...) X(a) ACC_APPLYX24(X, __VA_ARGS__)
#define ACC_APPLYX26(X, a, ...) X(a) ACC_APPLYX25(X, __VA_ARGS__)
#define ACC_APPLYX27(X, a, ...) X(a) ACC_APPLYX26(X, __VA_ARGS__)
#define ACC_APPLYX28(X, a, ...) X(a) ACC_APPLYX27(X, __VA_ARGS__)
#define ACC_APPLYX29(X, a, ...) X(a) ACC_APPLYX28(X, __VA_ARGS__)

#define ACC_APPLYX30(X, a, ...) X(a) ACC_APPLYX29(X, __VA_ARGS__)
#define ACC_APPLYX31(X, a, ...) X(a) ACC_APPLYX30(X, __VA_ARGS__)
#define ACC_APPLYX32(X, a, ...) X(a) ACC_APPLYX31(X, __VA_ARGS__)
#define ACC_APPLYX33(X, a, ...) X(a) ACC_APPLYX32(X, __VA_ARGS__)
#define ACC_APPLYX34(X, a, ...) X(a) ACC_APPLYX33(X, __VA_ARGS__)
#define ACC_APPLYX35(X, a, ...) X(a) ACC_APPLYX34(X, __VA_ARGS__)
#define ACC_APPLYX36(X, a, ...) X(a) ACC_APPLYX35(X, __VA_ARGS__)
#define ACC_APPLYX37(X, a, ...) X(a) ACC_APPLYX36(X, __VA_ARGS__)
#define ACC_APPLYX38(X, a, ...) X(a) ACC_APPLYX37(X, __VA_ARGS__)
#define ACC_APPLYX39(X, a, ...) X(a) ACC_APPLYX38(X, __VA_ARGS__)

#define ACC_APPLYX40(X, a, ...) X(a) ACC_APPLYX39(X, __VA_ARGS__)
#define ACC_APPLYX41(X, a, ...) X(a) ACC_APPLYX40(X, __VA_ARGS__)
#define ACC_APPLYX42(X, a, ...) X(a) ACC_APPLYX41(X, __VA_ARGS__)
#define ACC_APPLYX43(X, a, ...) X(a) ACC_APPLYX42(X, __VA_ARGS__)
#define ACC_APPLYX44(X, a, ...) X(a) ACC_APPLYX43(X, __VA_ARGS__)
#define ACC_APPLYX45(X, a, ...) X(a) ACC_APPLYX44(X, __VA_ARGS__)
#define ACC_APPLYX46(X, a, ...) X(a) ACC_APPLYX45(X, __VA_ARGS__)
#define ACC_APPLYX47(X, a, ...) X(a) ACC_APPLYX46(X, __VA_ARGS__)
#define ACC_APPLYX48(X, a, ...) X(a) ACC_APPLYX47(X, __VA_ARGS__)
#define ACC_APPLYX49(X, a, ...) X(a) ACC_APPLYX48(X, __VA_ARGS__)

#define ACC_APPLYX50(X, a, ...) X(a) ACC_APPLYX49(X, __VA_ARGS__)
#define ACC_APPLYX51(X, a, ...) X(a) ACC_APPLYX50(X, __VA_ARGS__)
#define ACC_APPLYX52(X, a, ...) X(a) ACC_APPLYX51(X, __VA_ARGS__)
#define ACC_APPLYX53(X, a, ...) X(a) ACC_APPLYX52(X, __VA_ARGS__)
#define ACC_APPLYX54(X, a, ...) X(a) ACC_APPLYX53(X, __VA_ARGS__)
#define ACC_APPLYX55(X, a, ...) X(a) ACC_APPLYX54(X, __VA_ARGS__)
#define ACC_APPLYX56(X, a, ...) X(a) ACC_APPLYX55(X, __VA_ARGS__)
#define ACC_APPLYX57(X, a, ...) X(a) ACC_APPLYX56(X, __VA_ARGS__)
#define ACC_APPLYX58(X, a, ...) X(a) ACC_APPLYX57(X, __VA_ARGS__)
#define ACC_APPLYX59(X, a, ...) X(a) ACC_APPLYX58(X, __VA_ARGS__)

#define ACC_APPLYX60(X, a, ...) X(a) ACC_APPLYX59(X, __VA_ARGS__)
#define ACC_APPLYX61(X, a, ...) X(a) ACC_APPLYX60(X, __VA_ARGS__)
#define ACC_APPLYX62(X, a, ...) X(a) ACC_APPLYX61(X, __VA_ARGS__)
#define ACC_APPLYX63(X, a, ...) X(a) ACC_APPLYX62(X, __VA_ARGS__)
#define ACC_APPLYX64(X, a, ...) X(a) ACC_APPLYX63(X, __VA_ARGS__)
#define ACC_APPLYX65(X, a, ...) X(a) ACC_APPLYX64(X, __VA_ARGS__)
#define ACC_APPLYX66(X, a, ...) X(a) ACC_APPLYX65(X, __VA_ARGS__)
#define ACC_APPLYX67(X, a, ...) X(a) ACC_APPLYX66(X, __VA_ARGS__)
#define ACC_APPLYX68(X, a, ...) X(a) ACC_APPLYX67(X, __VA_ARGS__)
#define ACC_APPLYX69(X, a, ...) X(a) ACC_APPLYX68(X, __VA_ARGS__)

#define ACC_APPLYX70(X, a, ...) X(a) ACC_APPLYX69(X, __VA_ARGS__)
#define ACC_APPLYX71(X, a, ...) X(a) ACC_APPLYX70(X, __VA_ARGS__)
#define ACC_APPLYX72(X, a, ...) X(a) ACC_APPLYX71(X, __VA_ARGS__)
#define ACC_APPLYX73(X, a, ...) X(a) ACC_APPLYX72(X, __VA_ARGS__)
#define ACC_APPLYX74(X, a, ...) X(a) ACC_APPLYX73(X, __VA_ARGS__)
#define ACC_APPLYX75(X, a, ...) X(a) ACC_APPLYX74(X, __VA_ARGS__)
#define ACC_APPLYX76(X, a, ...) X(a) ACC_APPLYX75(X, __VA_ARGS__)
#define ACC_APPLYX77(X, a, ...) X(a) ACC_APPLYX76(X, __VA_ARGS__)
#define ACC_APPLYX78(X, a, ...) X(a) ACC_APPLYX77(X, __VA_ARGS__)
#define ACC_APPLYX79(X, a, ...) X(a) ACC_APPLYX78(X, __VA_ARGS__)

#define ACC_APPLYX80(X, a, ...) X(a) ACC_APPLYX79(X, __VA_ARGS__)
#define ACC_APPLYX81(X, a, ...) X(a) ACC_APPLYX80(X, __VA_ARGS__)
#define ACC_APPLYX82(X, a, ...) X(a) ACC_APPLYX81(X, __VA_ARGS__)
#define ACC_APPLYX83(X, a, ...) X(a) ACC_APPLYX82(X, __VA_ARGS__)
#define ACC_APPLYX84(X, a, ...) X(a) ACC_APPLYX83(X, __VA_ARGS__)
#define ACC_APPLYX85(X, a, ...) X(a) ACC_APPLYX84(X, __VA_ARGS__)
#define ACC_APPLYX86(X, a, ...) X(a) ACC_APPLYX85(X, __VA_ARGS__)
#define ACC_APPLYX87(X, a, ...) X(a) ACC_APPLYX86(X, __VA_ARGS__)
#define ACC_APPLYX88(X, a, ...) X(a) ACC_APPLYX87(X, __VA_ARGS__)
#define ACC_APPLYX89(X, a, ...) X(a) ACC_APPLYX88(X, __VA_ARGS__)

#define ACC_APPLYX90(X, a, ...) X(a) ACC_APPLYX89(X, __VA_ARGS__)
#define ACC_APPLYX91(X, a, ...) X(a) ACC_APPLYX90(X, __VA_ARGS__)
#define ACC_APPLYX92(X, a, ...) X(a) ACC_APPLYX91(X, __VA_ARGS__)
#define ACC_APPLYX93(X, a, ...) X(a) ACC_APPLYX92(X, __VA_ARGS__)
#define ACC_APPLYX94(X, a, ...) X(a) ACC_APPLYX93(X, __VA_ARGS__)
#define ACC_APPLYX95(X, a, ...) X(a) ACC_APPLYX94(X, __VA_ARGS__)
#define ACC_APPLYX96(X, a, ...) X(a) ACC_APPLYX95(X, __VA_ARGS__)
#define ACC_APPLYX97(X, a, ...) X(a) ACC_APPLYX96(X, __VA_ARGS__)
#define ACC_APPLYX98(X, a, ...) X(a) ACC_APPLYX97(X, __VA_ARGS__)
#define ACC_APPLYX99(X, a, ...) X(a) ACC_APPLYX98(X, __VA_ARGS__)

#define ACC_APPLYXi(X, n, ...) ACC_APPLYX##n(X, __VA_ARGS__)
#define ACC_APPLYX_(X, n, ...) ACC_APPLYXi(X, n, __VA_ARGS__)

#define ACC_APPLYXn(X, ...) ACC_APPLYX_(X, ACC_NARG(__VA_ARGS__), __VA_ARGS__)

/**
 * APPLYFn variadic F-Macro.
 */
#define ACC_APPLYF0 (x, ...)
#define ACC_APPLYF1 (x, A, ...) A(x) ACC_APPLYF0 (x, __VA_ARGS__)
#define ACC_APPLYF2 (x, A, ...) A(x) ACC_APPLYF1 (x, __VA_ARGS__)
#define ACC_APPLYF3 (x, A, ...) A(x) ACC_APPLYF2 (x, __VA_ARGS__)
#define ACC_APPLYF4 (x, A, ...) A(x) ACC_APPLYF3 (x, __VA_ARGS__)
#define ACC_APPLYF5 (x, A, ...) A(x) ACC_APPLYF4 (x, __VA_ARGS__)
#define ACC_APPLYF6 (x, A, ...) A(x) ACC_APPLYF5 (x, __VA_ARGS__)
#define ACC_APPLYF7 (x, A, ...) A(x) ACC_APPLYF6 (x, __VA_ARGS__)
#define ACC_APPLYF8 (x, A, ...) A(x) ACC_APPLYF7 (x, __VA_ARGS__)
#define ACC_APPLYF9 (x, A, ...) A(x) ACC_APPLYF8 (x, __VA_ARGS__)

#define ACC_APPLYF10(x, A, ...) A(x) ACC_APPLYF9 (x, __VA_ARGS__)
#define ACC_APPLYF11(x, A, ...) A(x) ACC_APPLYF10(x, __VA_ARGS__)
#define ACC_APPLYF12(x, A, ...) A(x) ACC_APPLYF11(x, __VA_ARGS__)
#define ACC_APPLYF13(x, A, ...) A(x) ACC_APPLYF12(x, __VA_ARGS__)
#define ACC_APPLYF14(x, A, ...) A(x) ACC_APPLYF13(x, __VA_ARGS__)
#define ACC_APPLYF15(x, A, ...) A(x) ACC_APPLYF14(x, __VA_ARGS__)
#define ACC_APPLYF16(x, A, ...) A(x) ACC_APPLYF15(x, __VA_ARGS__)
#define ACC_APPLYF17(x, A, ...) A(x) ACC_APPLYF16(x, __VA_ARGS__)
#define ACC_APPLYF18(x, A, ...) A(x) ACC_APPLYF17(x, __VA_ARGS__)
#define ACC_APPLYF19(x, A, ...) A(x) ACC_APPLYF18(x, __VA_ARGS__)

#define ACC_APPLYF20(x, A, ...) A(x) ACC_APPLYF19(x, __VA_ARGS__)
#define ACC_APPLYF21(x, A, ...) A(x) ACC_APPLYF20(x, __VA_ARGS__)
#define ACC_APPLYF22(x, A, ...) A(x) ACC_APPLYF21(x, __VA_ARGS__)
#define ACC_APPLYF23(x, A, ...) A(x) ACC_APPLYF22(x, __VA_ARGS__)
#define ACC_APPLYF24(x, A, ...) A(x) ACC_APPLYF23(x, __VA_ARGS__)
#define ACC_APPLYF25(x, A, ...) A(x) ACC_APPLYF24(x, __VA_ARGS__)
#define ACC_APPLYF26(x, A, ...) A(x) ACC_APPLYF25(x, __VA_ARGS__)
#define ACC_APPLYF27(x, A, ...) A(x) ACC_APPLYF26(x, __VA_ARGS__)
#define ACC_APPLYF28(x, A, ...) A(x) ACC_APPLYF27(x, __VA_ARGS__)
#define ACC_APPLYF29(x, A, ...) A(x) ACC_APPLYF28(x, __VA_ARGS__)

#define ACC_APPLYF30(x, A, ...) A(x) ACC_APPLYF29(x, __VA_ARGS__)
#define ACC_APPLYF31(x, A, ...) A(x) ACC_APPLYF30(x, __VA_ARGS__)
#define ACC_APPLYF32(x, A, ...) A(x) ACC_APPLYF31(x, __VA_ARGS__)
#define ACC_APPLYF33(x, A, ...) A(x) ACC_APPLYF32(x, __VA_ARGS__)
#define ACC_APPLYF34(x, A, ...) A(x) ACC_APPLYF33(x, __VA_ARGS__)
#define ACC_APPLYF35(x, A, ...) A(x) ACC_APPLYF34(x, __VA_ARGS__)
#define ACC_APPLYF36(x, A, ...) A(x) ACC_APPLYF35(x, __VA_ARGS__)
#define ACC_APPLYF37(x, A, ...) A(x) ACC_APPLYF36(x, __VA_ARGS__)
#define ACC_APPLYF38(x, A, ...) A(x) ACC_APPLYF37(x, __VA_ARGS__)
#define ACC_APPLYF39(x, A, ...) A(x) ACC_APPLYF38(x, __VA_ARGS__)

#define ACC_APPLYF40(x, A, ...) A(x) ACC_APPLYF39(x, __VA_ARGS__)
#define ACC_APPLYF41(x, A, ...) A(x) ACC_APPLYF40(x, __VA_ARGS__)
#define ACC_APPLYF42(x, A, ...) A(x) ACC_APPLYF41(x, __VA_ARGS__)
#define ACC_APPLYF43(x, A, ...) A(x) ACC_APPLYF42(x, __VA_ARGS__)
#define ACC_APPLYF44(x, A, ...) A(x) ACC_APPLYF43(x, __VA_ARGS__)
#define ACC_APPLYF45(x, A, ...) A(x) ACC_APPLYF44(x, __VA_ARGS__)
#define ACC_APPLYF46(x, A, ...) A(x) ACC_APPLYF45(x, __VA_ARGS__)
#define ACC_APPLYF47(x, A, ...) A(x) ACC_APPLYF46(x, __VA_ARGS__)
#define ACC_APPLYF48(x, A, ...) A(x) ACC_APPLYF47(x, __VA_ARGS__)
#define ACC_APPLYF49(x, A, ...) A(x) ACC_APPLYF48(x, __VA_ARGS__)

#define ACC_APPLYF50(x, A, ...) A(x) ACC_APPLYF49(x, __VA_ARGS__)
#define ACC_APPLYF51(x, A, ...) A(x) ACC_APPLYF50(x, __VA_ARGS__)
#define ACC_APPLYF52(x, A, ...) A(x) ACC_APPLYF51(x, __VA_ARGS__)
#define ACC_APPLYF53(x, A, ...) A(x) ACC_APPLYF52(x, __VA_ARGS__)
#define ACC_APPLYF54(x, A, ...) A(x) ACC_APPLYF53(x, __VA_ARGS__)
#define ACC_APPLYF55(x, A, ...) A(x) ACC_APPLYF54(x, __VA_ARGS__)
#define ACC_APPLYF56(x, A, ...) A(x) ACC_APPLYF55(x, __VA_ARGS__)
#define ACC_APPLYF57(x, A, ...) A(x) ACC_APPLYF56(x, __VA_ARGS__)
#define ACC_APPLYF58(x, A, ...) A(x) ACC_APPLYF57(x, __VA_ARGS__)
#define ACC_APPLYF59(x, A, ...) A(x) ACC_APPLYF58(x, __VA_ARGS__)

#define ACC_APPLYF60(x, A, ...) A(x) ACC_APPLYF59(x, __VA_ARGS__)
#define ACC_APPLYF61(x, A, ...) A(x) ACC_APPLYF60(x, __VA_ARGS__)
#define ACC_APPLYF62(x, A, ...) A(x) ACC_APPLYF61(x, __VA_ARGS__)
#define ACC_APPLYF63(x, A, ...) A(x) ACC_APPLYF62(x, __VA_ARGS__)
#define ACC_APPLYF64(x, A, ...) A(x) ACC_APPLYF63(x, __VA_ARGS__)
#define ACC_APPLYF65(x, A, ...) A(x) ACC_APPLYF64(x, __VA_ARGS__)
#define ACC_APPLYF66(x, A, ...) A(x) ACC_APPLYF65(x, __VA_ARGS__)
#define ACC_APPLYF67(x, A, ...) A(x) ACC_APPLYF66(x, __VA_ARGS__)
#define ACC_APPLYF68(x, A, ...) A(x) ACC_APPLYF67(x, __VA_ARGS__)
#define ACC_APPLYF69(x, A, ...) A(x) ACC_APPLYF68(x, __VA_ARGS__)

#define ACC_APPLYF70(x, A, ...) A(x) ACC_APPLYF69(x, __VA_ARGS__)
#define ACC_APPLYF71(x, A, ...) A(x) ACC_APPLYF70(x, __VA_ARGS__)
#define ACC_APPLYF72(x, A, ...) A(x) ACC_APPLYF71(x, __VA_ARGS__)
#define ACC_APPLYF73(x, A, ...) A(x) ACC_APPLYF72(x, __VA_ARGS__)
#define ACC_APPLYF74(x, A, ...) A(x) ACC_APPLYF73(x, __VA_ARGS__)
#define ACC_APPLYF75(x, A, ...) A(x) ACC_APPLYF74(x, __VA_ARGS__)
#define ACC_APPLYF76(x, A, ...) A(x) ACC_APPLYF75(x, __VA_ARGS__)
#define ACC_APPLYF77(x, A, ...) A(x) ACC_APPLYF76(x, __VA_ARGS__)
#define ACC_APPLYF78(x, A, ...) A(x) ACC_APPLYF77(x, __VA_ARGS__)
#define ACC_APPLYF79(x, A, ...) A(x) ACC_APPLYF78(x, __VA_ARGS__)

#define ACC_APPLYF80(x, A, ...) A(x) ACC_APPLYF79(x, __VA_ARGS__)
#define ACC_APPLYF81(x, A, ...) A(x) ACC_APPLYF80(x, __VA_ARGS__)
#define ACC_APPLYF82(x, A, ...) A(x) ACC_APPLYF81(x, __VA_ARGS__)
#define ACC_APPLYF83(x, A, ...) A(x) ACC_APPLYF82(x, __VA_ARGS__)
#define ACC_APPLYF84(x, A, ...) A(x) ACC_APPLYF83(x, __VA_ARGS__)
#define ACC_APPLYF85(x, A, ...) A(x) ACC_APPLYF84(x, __VA_ARGS__)
#define ACC_APPLYF86(x, A, ...) A(x) ACC_APPLYF85(x, __VA_ARGS__)
#define ACC_APPLYF87(x, A, ...) A(x) ACC_APPLYF86(x, __VA_ARGS__)
#define ACC_APPLYF88(x, A, ...) A(x) ACC_APPLYF87(x, __VA_ARGS__)
#define ACC_APPLYF89(x, A, ...) A(x) ACC_APPLYF88(x, __VA_ARGS__)

#define ACC_APPLYF90(x, A, ...) A(x) ACC_APPLYF89(x, __VA_ARGS__)
#define ACC_APPLYF91(x, A, ...) A(x) ACC_APPLYF90(x, __VA_ARGS__)
#define ACC_APPLYF92(x, A, ...) A(x) ACC_APPLYF91(x, __VA_ARGS__)
#define ACC_APPLYF93(x, A, ...) A(x) ACC_APPLYF92(x, __VA_ARGS__)
#define ACC_APPLYF94(x, A, ...) A(x) ACC_APPLYF93(x, __VA_ARGS__)
#define ACC_APPLYF95(x, A, ...) A(x) ACC_APPLYF94(x, __VA_ARGS__)
#define ACC_APPLYF96(x, A, ...) A(x) ACC_APPLYF95(x, __VA_ARGS__)
#define ACC_APPLYF97(x, A, ...) A(x) ACC_APPLYF96(x, __VA_ARGS__)
#define ACC_APPLYF98(x, A, ...) A(x) ACC_APPLYF97(x, __VA_ARGS__)
#define ACC_APPLYF99(x, A, ...) A(x) ACC_APPLYF98(x, __VA_ARGS__)

#define ACC_APPLYFi(x, n, ...) ACC_APPLYF##n(x, __VA_ARGS__)
#define ACC_APPLYF_(x, n, ...) ACC_APPLYFi(x, n, __VA_ARGS__)

#define ACC_APPLYFn(x, ...) ACC_APPLYF_(x, ACC_NARG(__VA_ARGS__), __VA_ARGS__)

