/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#include "filesystem.hpp"
#include <sys/stat.h>
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

    bool MakePath(const std::string &path)
    {
      bool result = true;
      // 0777 will inherit mode from process
      auto mkdir_res = mkdir(path.c_str(), 0777);
      if (mkdir_res != 0)
        {
          switch (errno)
            {
              case EEXIST:
                // nothing to do
                break;
              case ENOENT:
                // parent doesn't exists
                // create it
                result = MakePath(path.substr(0, path.find_last_of('/')));
                // and try once again
                result = result && (mkdir(path.c_str(), 0777) == 0);
                break;
              default:
                result = false;
                break;
            }
        }

      // check that path is available
      struct stat info;
      result = result && (stat(path.c_str(), &info) == 0);
      // and it is a directory
      result = result && ((info.st_mode & S_IFDIR) == S_IFDIR);

      return result;
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
