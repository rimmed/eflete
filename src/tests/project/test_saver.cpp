/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>
#include "mock_filesystem.hpp"
#include "modules/project/saver.hpp"
#include "test_saver.pb.h"

namespace
{
  const std::string kTestFile = "some_strange_file_for_test_only";
  const std::string kTestData = "some_test_data";
  std::shared_ptr<eflete::filesystem::Filesystem> null_fs;

  TEST(ProjectSaver, CreateNewFile)
  {
    // prepare mock for filesystem
    std::shared_ptr<mocks::MockFilesystem> fs =
      std::make_shared<mocks::MockFilesystem>();

    {
      ::testing::InSequence seq;
      // saver should perfome following actions in sequence:
      // 1. check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(false));
      // 2. if not, check if file exists. if not saver must be marked as changed
      // because file needs to be saved
      EXPECT_CALL(*fs, Exists(kTestFile)).WillOnce(testing::Return(false));
    }
    eflete::filesystem::Set(fs);

    // starting test
    eflete::project::internal::Saver<tests::TestMesage> saver(kTestFile);

    // check that saver knows that file is new
    EXPECT_EQ(saver.isChanged(), true);

    // set null_fs to remove fs mock
    eflete::filesystem::Set(null_fs);
  }

  TEST(ProjectSaver, OpenFileWithoutAutosave)
  {
    // prepare mock for filesystem
    std::shared_ptr<mocks::MockFilesystem> fs =
      std::make_shared<mocks::MockFilesystem>();

    // stringstream that will be returned instead of real file stream
    auto iss = std::make_shared<std::istringstream>();
    // file "content"
    iss->str("i2: 42\n");
    {
      ::testing::InSequence seq;
      // saver should perfome following actions in sequence:
      // 1. check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(false));
      // 2. if not, check if file exists. if it exists saver must be marked as
      // not changed because file was opened
      EXPECT_CALL(*fs, Exists(kTestFile)).WillOnce(testing::Return(true));
      // 3. open input file stream
      EXPECT_CALL(*fs, GetInputFileStream(kTestFile))
        .WillOnce(testing::Return(iss));
    }
    eflete::filesystem::Set(fs);

    // starting test
    eflete::project::internal::Saver<tests::TestMesage> saver(kTestFile);

    // check that saver knows that file is not changed
    EXPECT_EQ(saver.isChanged(), false);

    // check that data was parsed
    EXPECT_EQ(saver.data.has_i1(), false);
    EXPECT_EQ(saver.data.i2(), 42);

    // set null_fs to remove fs mock
    eflete::filesystem::Set(null_fs);
  }

  TEST(ProjectSaver, OpenFileWithAutosave)
  {
    // prepare mock for filesystem
    std::shared_ptr<mocks::MockFilesystem> fs =
      std::make_shared<mocks::MockFilesystem>();

    // stringstream that will be returned instead of real file stream
    auto iss = std::make_shared<std::istringstream>();
    iss->str("i2: 42\n");
    {
      ::testing::InSequence seq;
      // saver should perfome following actions in sequence:
      // 1. check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(true));
      // 2. if autosave exists open it instead of main file
      EXPECT_CALL(*fs, GetInputFileStream(kTestFile + ".autosave"))
        .WillOnce(testing::Return(iss));
    }
    eflete::filesystem::Set(fs);

    // starting test
    eflete::project::internal::Saver<tests::TestMesage> saver(kTestFile);

    // check that saver knows that autosave file was open.
    EXPECT_EQ(saver.isChanged(), true);

    // check that data was parsed
    EXPECT_EQ(saver.data.has_i1(), false);
    EXPECT_EQ(saver.data.i2(), 42);

    // set null_fs to remove fs mock
    eflete::filesystem::Set(null_fs);
  }

  TEST(ProjectSaver, AutoSave)
  {
    // prepare mock for filesystem
    std::shared_ptr<mocks::MockFilesystem> fs =
      std::make_shared<mocks::MockFilesystem>();

    auto oss = std::make_shared<std::ostringstream>();
    {
      ::testing::InSequence seq;
      // saver should perfome following actions in sequence:
      // 1. check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(false));
      // 2. if not, check if file exists.
      EXPECT_CALL(*fs, Exists(kTestFile)).WillOnce(testing::Return(false));
      // 3. open output file stream
      EXPECT_CALL(*fs, GetOutputFileStream(kTestFile + ".autosave"))
        .WillOnce(testing::Return(oss));
    }
    eflete::filesystem::Set(fs);

    // starting test
    eflete::project::internal::Saver<tests::TestMesage> saver(kTestFile);

    // new file should be marked as changed
    EXPECT_EQ(saver.isChanged(), true);

    saver.data.set_i1(10);
    saver.data.set_i2(42);

    saver.AutoSave();

    EXPECT_EQ(oss->str(),
              "i1: 10\n"
              "i2: 42\n");

    // after autosave file should still be marked as chnaged
    EXPECT_EQ(saver.isChanged(), true);

    // set null_fs to remove fs mock
    eflete::filesystem::Set(null_fs);
  }

  TEST(ProjectSaver, Save)
  {
    // prepare mock for filesystem
    std::shared_ptr<mocks::MockFilesystem> fs =
      std::make_shared<mocks::MockFilesystem>();

    auto oss = std::make_shared<std::ostringstream>();
    {
      ::testing::InSequence seq;
      // saver should perfome following actions in sequence:
      // 1. check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(false));
      // 2. if not, check if file exists. if not saver must be marked as changed
      // because file needs to be saved
      EXPECT_CALL(*fs, Exists(kTestFile)).WillOnce(testing::Return(false));
      // 3. open output file stream
      EXPECT_CALL(*fs, GetOutputFileStream(kTestFile))
        .WillOnce(testing::Return(oss));
      // 4. after saving saver should check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(true));
      // 5. and if it is - remove it
      EXPECT_CALL(*fs, RemoveFile(kTestFile + ".autosave"));
    }
    eflete::filesystem::Set(fs);

    // starting test
    eflete::project::internal::Saver<tests::TestMesage> saver(kTestFile);

    // new file is should be marked as changed
    EXPECT_EQ(saver.isChanged(), true);

    saver.data.set_i1(10);
    saver.data.set_i2(42);

    saver.Save();

    EXPECT_EQ(oss->str(),
              "i1: 10\n"
              "i2: 42\n");

    // after save file should not be marked as chnaged anymore
    EXPECT_EQ(saver.isChanged(), false);

    // set null_fs to remove fs mock
    eflete::filesystem::Set(null_fs);
  }

  TEST(ProjectSaver, Changed)
  {
    // prepare mock for filesystem
    std::shared_ptr<mocks::MockFilesystem> fs =
      std::make_shared<mocks::MockFilesystem>();

    // stringstream that will be returned instead of real file stream
    auto iss = std::make_shared<std::istringstream>();
    {
      ::testing::InSequence seq;
      // saver should perfome following actions in sequence:
      // 1. check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(false));
      // 2. if not, check if file exists.
      EXPECT_CALL(*fs, Exists(kTestFile)).WillOnce(testing::Return(true));
      // 3. open input file stream
      EXPECT_CALL(*fs, GetInputFileStream(kTestFile))
        .WillOnce(testing::Return(iss));
    }
    eflete::filesystem::Set(fs);

    // starting test
    eflete::project::internal::Saver<tests::TestMesage> saver(kTestFile);

    // check that saver knows that file is not changed
    EXPECT_EQ(saver.isChanged(), false);

    // tell that file is changed
    saver.Changed();

    // check that saver now knows that file is changed
    EXPECT_EQ(saver.isChanged(), true);

    // set null_fs to remove fs mock
    eflete::filesystem::Set(null_fs);
  }

  TEST(ProjectSaver, DiscardAutoSave)
  {
    // prepare mock for filesystem
    std::shared_ptr<mocks::MockFilesystem> fs =
      std::make_shared<mocks::MockFilesystem>();

    auto oss = std::make_shared<std::ostringstream>();
    {
      ::testing::InSequence seq;
      // saver should perfome following actions in sequence:
      // 1. check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(false));
      // 2. if not, check if file exists.
      EXPECT_CALL(*fs, Exists(kTestFile)).WillOnce(testing::Return(false));
      // 3. discardautosave should check if .autosave file exists
      EXPECT_CALL(*fs, Exists(kTestFile + ".autosave"))
        .WillOnce(testing::Return(true));
      // 4. and if it is - remove it
      EXPECT_CALL(*fs, RemoveFile(kTestFile + ".autosave"));
    }
    eflete::filesystem::Set(fs);

    // starting test
    eflete::project::internal::Saver<tests::TestMesage> saver(kTestFile);

    saver.DiscardAutoSave();

    // set null_fs to remove fs mock
    eflete::filesystem::Set(null_fs);
  }
}
