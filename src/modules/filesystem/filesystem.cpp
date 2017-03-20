/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include "filesystem.hpp"
#include <fstream>

namespace
{
  class DefaultFS : public eflete::filesystem::Filesystem
  {
    bool Exists(const std::string &file)
    {
      return (bool)std::ifstream(file);
    };

    void RemoveFile(const std::string &file)
    {
      remove(file.c_str());
    }

    std::shared_ptr<std::ostream> GetOutputFileStream(const std::string &file)
    {
      return std::make_shared<std::ofstream>(file);
    };

    std::shared_ptr<std::istream> GetInputFileStream(const std::string &file)
    {
      return std::make_shared<std::ifstream>(file);
    };
  };
};

namespace eflete
{
  namespace filesystem
  {
    static std::shared_ptr<Filesystem> _fs;
    std::shared_ptr<Filesystem> Get()
    {
      if (!_fs)
        _fs = std::make_shared<::DefaultFS>();
      return _fs;
    }
    void Set(std::shared_ptr<Filesystem> fs)
    {
      _fs = fs;
    }
  };
};
