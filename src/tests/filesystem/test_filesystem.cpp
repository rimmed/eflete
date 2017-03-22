/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <fstream>
#include "mocks/filesystem.hpp"
#include "modules/filesystem/filesystem.hpp"

namespace
{
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

  TEST(FilesystemDefaultImplementation, MakePath)
  {
    const std::string kTestPath = "some/strange/file/path/for foleders";
    EXPECT_EQ(eflete::filesystem::Get()->MakePath(kTestPath), true);

    // if path was created we can create file there
    auto test_file_path = kTestPath + "/" + kTestFile;
    std::ofstream test_file(test_file_path);
    EXPECT_EQ(test_file.good(), true);
    test_file.close();

    // if name is used by a file we can't create path
    EXPECT_EQ(eflete::filesystem::Get()->MakePath(test_file_path), false);
    // if one of parent folder is actually a file
    EXPECT_EQ(
      eflete::filesystem::Get()->MakePath(test_file_path + "/sub_folder"),
      false);

    // cleaninng
    remove(test_file_path.c_str());
    rmdir("some/strange/file/path/for foleders");
    rmdir("some/strange/file/path");
    rmdir("some/strange/file");
    rmdir("some/strange");
    rmdir("some");
  }

  TEST(Filesystem, InjectionWorks)
  {
    auto mock = std::make_shared<mocks::MockFilesystem>();
    eflete::filesystem::Set(mock);
    // check that Get returns setted implementation instead of default one
    ASSERT_EQ(eflete::filesystem::Get(), mock);
  }
}
