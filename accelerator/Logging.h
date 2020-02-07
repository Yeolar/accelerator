/*
 * Copyright 2017-present Yeolar
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

#include <string.h>

#include "accelerator/LogBase.h"
#include "accelerator/Singleton.h"
#include "accelerator/String.h"

namespace acc {
namespace logging {

class ACCLogger : public BaseLogger {
 public:
  ACCLogger() : BaseLogger("acc") {}
};

} // namespace logging
} // namespace acc

// printf interface macro helper; do not use directly

#ifndef ACCLOG_STREAM
#define ACCLOG_STREAM(severity)                                             \
  (::acc::Singleton< ::acc::logging::ACCLogger>::get()->level() > severity) \
    ? (void)0                                                               \
    : ::acc::logging::LogMessageVoidify() &                                 \
      ::acc::logging::LogMessage(                                           \
        ::acc::Singleton< ::acc::logging::ACCLogger>::get(),                \
        severity, __FILENAME__, __LINE__).stream()
#endif

#ifndef ACCLOG_STREAM_TRACE
#define ACCLOG_STREAM_TRACE(severity, traceid)                              \
  (::acc::Singleton< ::acc::logging::ACCLogger>::get()->level() > severity) \
    ? (void)0                                                               \
    : ::acc::logging::LogMessageVoidify() &                                 \
      ::acc::logging::LogMessage(                                           \
        ::acc::Singleton< ::acc::logging::ACCLogger>::get(),                \
        severity, __FILENAME__, __LINE__, traceid).stream()
#endif

#ifndef ACCLOG_STREAM_RAW
#define ACCLOG_STREAM_RAW(severity)                                         \
  (::acc::Singleton< ::acc::logging::ACCLogger>::get()->level() > severity) \
    ? (void)0                                                               \
    : ::acc::logging::LogMessageVoidify() &                                 \
      ::acc::logging::RawLogMessage(                                        \
        ::acc::Singleton< ::acc::logging::ACCLogger>::get()).stream()
#endif

#ifndef ACCLOG_COST_IMPL
#define ACCLOG_COST_IMPL(severity, threshold)                               \
  (::acc::Singleton< ::acc::logging::ACCLogger>::get()->level() > severity) \
    ? (bool)0                                                               \
    : ::acc::logging::detail::LogScopeParam{                                \
        ::acc::Singleton< ::acc::logging::ACCLogger>::get(),                \
        severity, __FILENAME__, __LINE__, threshold} + [&]() noexcept
#endif

///////////////////////////////////////////////////////////////////////////
// acc framework
//
#define ACCLOG(severity) \
  ACCLOG_STREAM(::acc::logging::LOG_##severity)

#define ACCTLOG(severity, traceid) \
  ACCLOG_STREAM_TRACE(::acc::logging::LOG_##severity, traceid)

#define ACCPLOG(severity) \
  ACCLOG(severity) << ::acc::errnoStr(errno) << ", "

#define ACCRLOG(severity) \
  ACCLOG_STREAM_RAW(::acc::logging::LOG_##severity)

#define ACCLOG_ON(severity) \
  if (::acc::Singleton< ::acc::logging::ACCLogger>::get()->level() \
    <= ::acc::logging::LOG_##severity)

#define ACCLOG_IF(severity, condition) \
  (!(condition)) ? (void)0 : ACCLOG(severity)
#define ACCPLOG_IF(severity, condition) \
  (!(condition)) ? (void)0 : ACCPLOG(severity)

#define ACCCHECK(condition) \
  (condition) ? (void)0 : ACCLOG(FATAL) << "Check " #condition " failed. "
#define ACCCHECK_LT(a, b) ACCCHECK((a) <  (b))
#define ACCCHECK_LE(a, b) ACCCHECK((a) <= (b))
#define ACCCHECK_GT(a, b) ACCCHECK((a) >  (b))
#define ACCCHECK_GE(a, b) ACCCHECK((a) >= (b))
#define ACCCHECK_EQ(a, b) ACCCHECK((a) == (b))
#define ACCCHECK_NE(a, b) ACCCHECK((a) != (b))

#define ACCPCHECK(condition) \
  ACCCHECK(condition) << ::acc::errnoStr(errno) << ", "

#ifndef NDEBUG
#define DCHECK(condition) ACCCHECK(condition)
#define DCHECK_LT(a, b)   ACCCHECK_LT(a, b)
#define DCHECK_LE(a, b)   ACCCHECK_LE(a, b)
#define DCHECK_GT(a, b)   ACCCHECK_GT(a, b)
#define DCHECK_GE(a, b)   ACCCHECK_GE(a, b)
#define DCHECK_EQ(a, b)   ACCCHECK_EQ(a, b)
#define DCHECK_NE(a, b)   ACCCHECK_NE(a, b)
#else
#define DCHECK(condition) while (false) ACCCHECK(condition)
#define DCHECK_LT(a, b)   while (false) ACCCHECK_LT(a, b)
#define DCHECK_LE(a, b)   while (false) ACCCHECK_LE(a, b)
#define DCHECK_GT(a, b)   while (false) ACCCHECK_GT(a, b)
#define DCHECK_GE(a, b)   while (false) ACCCHECK_GE(a, b)
#define DCHECK_EQ(a, b)   while (false) ACCCHECK_EQ(a, b)
#define DCHECK_NE(a, b)   while (false) ACCCHECK_NE(a, b)
#endif

#define ACCCOST_SCOPE(severity, threshold) \
  ACCLOG_COST_IMPL(::acc::logging::LOG_##severity, threshold)

