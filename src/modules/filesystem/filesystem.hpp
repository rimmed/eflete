/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#ifndef EFLETE_MODULES_FILESYSTEM_HPP_
#define EFLETE_MODULES_FILESYSTEM_HPP_
#include <iosfwd>
#include <memory>
#include <string>

namespace eflete
{
  namespace filesystem
  {
    class Filesystem
    {
    public:
      virtual bool Exists(const std::string &file) = 0;
      virtual std::shared_ptr<std::ostream> GetOutputFileStream(
        const std::string &file) = 0;
      virtual std::shared_ptr<std::istream> GetInputFileStream(
        const std::string &file) = 0;
    };
    std::shared_ptr<Filesystem> Get();
    void Set(std::shared_ptr<Filesystem>);
  };
};
#endif // EFLETE_MODULES_FILESYSTEM_HPP_
