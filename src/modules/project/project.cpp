/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include "project.hpp"
#include <iostream>
#include "project.pb.h"

namespace eflete
{
  namespace project
  {
    struct Project::Impl
    {
      static const int kCurrentProjectVersion;
      internal::data::Project project;
    };

    /* change this constant after major changes to project format */
    const int Project::Impl::kCurrentProjectVersion = 1;

    Project::Project()
      : pimpl{ new Impl{} }
    {
      pimpl->project.set_version(Impl::kCurrentProjectVersion);
    };

    Project::~Project() = default;

    void Project::Test()
    {
      std::cout << "Hello\n" << std::endl;
      exit(0);
    }
  }
}
