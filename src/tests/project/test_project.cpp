/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include <gtest/gtest.h>
#include "project.hpp"

TEST(Project, Test1)
{
  ASSERT_EXIT(
    eflete::project::Project::Test(), ::testing::ExitedWithCode(0), "");
}
