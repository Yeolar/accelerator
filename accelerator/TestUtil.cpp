/*
 * Copyright 2017 Facebook, Inc.
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

#include "accelerator/TestUtil.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "accelerator/Conv.h"
#include "accelerator/Exception.h"
#include "accelerator/Logging.h"
#include "accelerator/String.h"
#include "accelerator/io/File.h"
#include "accelerator/io/FileUtil.h"

namespace acc {
namespace test {

TemporaryFile::TemporaryFile(StringPiece namePrefix,
                             Path dir,
                             Scope scope,
                             bool closeOnDestruction)
  : scope_(scope),
    closeOnDestruction_(closeOnDestruction),
    fd_(-1),
    path_(generateUniquePath(std::move(dir), namePrefix)) {
  fd_ = open(path_.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
  checkUnixError(fd_, "open failed");

  if (scope_ == Scope::UNLINK_IMMEDIATELY) {
    remove(path_);
    path_.clear();
  }
}

const Path& TemporaryFile::path() const {
  ACCCHECK_NE(scope_, Scope::UNLINK_IMMEDIATELY);
  DCHECK(!path_.empty());
  return path_;
}

TemporaryFile::~TemporaryFile() {
  if (fd_ != -1 && closeOnDestruction_) {
    if (close(fd_) == -1) {
      ACCPLOG(ERROR) << "close failed";
    }
  }

  // If we previously failed to unlink() (UNLINK_IMMEDIATELY), we'll
  // try again here.
  if (scope_ != Scope::PERMANENT && !path_.empty()) {
    remove(path_);
  }
}

TemporaryDirectory::TemporaryDirectory(StringPiece namePrefix,
                                       Path dir,
                                       Scope scope)
  : scope_(scope),
    path_(generateUniquePath(std::move(dir), namePrefix)) {
  createDirectory(path_);
}

TemporaryDirectory::~TemporaryDirectory() {
  if (scope_ == Scope::DELETE_ON_DESTRUCTION) {
    remove(path_);
  }
}

ChangeToTempDir::ChangeToTempDir() : initialPath_(currentPath()) {
  std::string p = dir_.path().str();
  checkUnixError(::chdir(p.c_str()), "failed chdir to ", p);
}

ChangeToTempDir::~ChangeToTempDir() {
  std::string p = initialPath_.str();
  checkUnixError(::chdir(p.c_str()), "failed chdir to ", p);
}

} // namespace test
} // namespace acc
