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

#include <string>
#include <vector>

#include "accelerator/Random.h"
#include "accelerator/String.h"
#include "accelerator/io/File.h"
#include "accelerator/io/Path.h"

namespace acc {

Path currentPath();

Path absolute(const Path& path, const Path& base = currentPath());

Path canonical(const Path& path, const Path& base = currentPath());

std::vector<Path> ls(const Path& path);

std::vector<Path> ls(const File& dir);

void createDirectory(const Path& path);

void createDirectory(const File& dir, const Path& child);

File openDirectory(const Path& path);

File openDirectory(const File& dir, const Path& child);

File openFile(const File& dir, const Path& child, int flags);

File tryOpenFile(const File& dir, const Path& child, int flags);

void remove(const Path& path);

void removeFile(const File& dir, const Path& path);

void rename(const Path& oldPath, const Path& newPath);

void rename(const File& oldDir,
            const Path& oldChild,
            const File& newDir,
            const Path& newChild);

void syncDirectory(const Path& path);

Path tempDirectoryPath();

Path uniquePath(const std::string& model = "%%%%%%%%%%%%%%%%");

Path generateUniquePath(Path path, StringPiece namePrefix);

} // namespace acc
