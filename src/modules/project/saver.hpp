/*
 * Copyright (C) 2017 Samsung Electronics. All rights reserved.
 * Licensed under https://www.gnu.org/licenses/lgpl.html <See COPYING file>
 */

#ifndef EFLETE_MODULES_PROJECT_SAVER_HPP_
#define EFLETE_MODULES_PROJECT_SAVER_HPP_
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/message.h>
#include <google/protobuf/text_format.h>
#include <string>
#include "filesystem.hpp"

namespace eflete
{
  namespace project
  {
    namespace internal
    {
      template <class T>
      class Saver
      {
        std::string file_name_;
        std::string autosave_file_name_;
        bool changed_;
        void ReadMessageFromFile(const std::string &file)
        {
          auto fs = eflete::filesystem::Get();

          auto input_stream = fs->GetInputFileStream(file);
          auto google_input_stream =
            std::make_shared<google::protobuf::io::IstreamInputStream>(
              &(*input_stream));
          google::protobuf::TextFormat::Parse(&(*google_input_stream), &data);
        };
        void WriteMessageToFile(const std::string &file)
        {
          auto fs = eflete::filesystem::Get();

          auto output_stream = fs->GetOutputFileStream(file);
          auto google_output_stream =
            std::make_shared<google::protobuf::io::OstreamOutputStream>(
              &(*output_stream));
          google::protobuf::TextFormat::Print(data, &(*google_output_stream));
        };

      public:
        T data;

        Saver(const std::string &file)
        {
          file_name_          = file;
          autosave_file_name_ = file + ".autosave";

          auto fs = eflete::filesystem::Get();
          if (fs->Exists(autosave_file_name_))
            {
              // load autosave
              ReadMessageFromFile(autosave_file_name_);
              changed_ = true;
            }
          else if (fs->Exists(file_name_))
            {
              // load file
              ReadMessageFromFile(file_name_);
              changed_ = false;
            }
          else
            {
              // create new
              changed_ = true;
            }
        };

        Saver(const std::string &file, bool create);

        void Save()
        {
          WriteMessageToFile(file_name_);
          DiscardAutoSave();

          changed_ = false;
        };

        void AutoSave()
        {
          WriteMessageToFile(autosave_file_name_);
        };

        void DiscardAutoSave()
        {
          auto fs = eflete::filesystem::Get();

          if (fs->Exists(autosave_file_name_))
            fs->RemoveFile(autosave_file_name_);
        };

        void Changed()
        {
          changed_ = true;
        };

        bool isChanged()
        {
          return changed_;
        }
      };
    };
  };
};

#endif // EFLETE_MODULES_PROJECT_SAVER_HPP_
