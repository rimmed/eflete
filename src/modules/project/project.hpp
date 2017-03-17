/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#ifndef EFLETE_MODULES_PROJECT_HPP_
#define EFLETE_MODULES_PROJECT_HPP_
#include <memory>

namespace eflete
{
  namespace project
  {
    class Project
    {
    private:
      struct Impl;
      std::unique_ptr<Impl> pimpl;

    public:
      Project();
      ~Project();
      static void Test();
    };
  };
};
#endif // EFLETE_MODULES_PROJECT_HPP_
