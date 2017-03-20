/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <fstream>
#include "modules/filesystem/filesystem.hpp"

namespace
{
  class MockFilesystem : public eflete::filesystem::Filesystem
  {
  public:
    MOCK_METHOD1(Exists, bool(const std::string &file));
    MOCK_METHOD1(RemoveFile, void(const std::string &file));
    MOCK_METHOD1(GetOutputFileStream,
                 std::shared_ptr<std::ostream>(const std::string &file));
    MOCK_METHOD1(GetInputFileStream,
                 std::shared_ptr<std::istream>(const std::string &file));
  };

  const std::string kTestFile = "some_strange_file_for_test_only";
  const std::string kTestData = "some_test_data";

  TEST(FilesystemDefaultImplementation, ExistsWorks)
  {
    remove(kTestFile.c_str());
    // file doesn't exists
    EXPECT_EQ(eflete::filesystem::Get()->Exists(kTestFile), false);
    // creating file
    std::ofstream of(kTestFile);
    of.close();
    EXPECT_EQ(eflete::filesystem::Get()->Exists(kTestFile), true);
    // check after remove
    remove(kTestFile.c_str());
    EXPECT_EQ(eflete::filesystem::Get()->Exists(kTestFile), false);
  }

  TEST(FilesystemDefaultImplementation, GetOutputFileStreamWorks)
  {
    {
      // writting to file
      auto ofs = eflete::filesystem::Get()->GetOutputFileStream(kTestFile);
      *ofs << kTestData;
      // closing file
    }
    // checking written data
    std::ifstream ifs(kTestFile);
    std::string written_data;
    ifs >> written_data;
    EXPECT_EQ(written_data, kTestData);

    // cleaninng
    remove(kTestFile.c_str());
  }

  TEST(FilesystemDefaultImplementation, GetInputFileStreamWorks)
  {
    // preparing test file
    std::ofstream ofs(kTestFile);
    ofs << kTestData;
    ofs.close();
    // reading file
    auto ifs = eflete::filesystem::Get()->GetInputFileStream(kTestFile);
    std::string read_data;
    *ifs >> read_data;
    // checking read data
    EXPECT_EQ(read_data, kTestData);

    // cleaninng
    remove(kTestFile.c_str());
  }

  TEST(FilesystemDefaultImplementation, RemoveFile)
  {
    // preparing test file
    std::ofstream ofs(kTestFile);
    ofs << kTestData;
    ofs.close();

    // remove file
    eflete::filesystem::Get()->RemoveFile(kTestFile);
    // check that file is deleted
    EXPECT_EQ(eflete::filesystem::Get()->Exists(kTestFile), false);

    // cleaninng
    remove(kTestFile.c_str());
  }

  TEST(Filesystem, InjectionWorks)
  {
    std::shared_ptr<MockFilesystem> mock = std::make_shared<MockFilesystem>();
    eflete::filesystem::Set(mock);
    // check that Get returns setted implementation instead of default one
    ASSERT_EQ(eflete::filesystem::Get(), mock);
  }
}
