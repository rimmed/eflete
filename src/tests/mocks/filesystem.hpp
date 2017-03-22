/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include <gmock/gmock.h>
#include "modules/filesystem/filesystem.hpp"

namespace mocks
{
  class MockFilesystem : public eflete::filesystem::Filesystem
  {
  public:
    MOCK_METHOD1(Exists, bool(const std::string &file));
    MOCK_METHOD1(RemoveFile, void(const std::string &file));
    MOCK_METHOD1(MakePath, bool(const std::string &path));
    MOCK_METHOD1(GetOutputFileStream,
                 std::shared_ptr<std::ostream>(const std::string &file));
    MOCK_METHOD1(GetInputFileStream,
                 std::shared_ptr<std::istream>(const std::string &file));
  };
}
