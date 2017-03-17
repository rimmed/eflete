/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include "project.hpp"
#include <iostream>

namespace eflete
{
  namespace project
  {
    void Project::Test()
    {
      std::cout << "Hello\n" << std::endl;
      exit(0);
    }
  }
}
