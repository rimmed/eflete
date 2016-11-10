/*
 * Edje Theme Editor
 * Copyright (C) 2013-2016 Samsung Electronics.
 *
 * This file is part of Edje Theme Editor.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; If not, see www.gnu.org/licenses/lgpl.html.
 */

#include <Eina.h>
#include <assert.h>
#include "common_macro.h"

Eina_File *
file_virtualize_open(const char *file)
{
   Eina_File *tmp_file, *mmap_file;

   tmp_file = eina_file_open(file, false);
   void *file_data = eina_file_map_all(tmp_file, EINA_FILE_SEQUENTIAL);
   size_t file_size = eina_file_size_get(tmp_file);
   mmap_file = eina_file_virtualize_writable("virtual_file", file_data, file_size);
   eina_file_map_free(tmp_file, file_data);
   eina_file_close(tmp_file);

   return mmap_file;
}

void
file_virtualize_save(Eina_File *f, const char *path)
{
   assert(f != NULL);
   assert(path != NULL);

   void * file_data = eina_file_map_all(f, EINA_FILE_SEQUENTIAL);
   size_t file_size = eina_file_size_get(f);

   FILE *fileptr = fopen(path, "wb");
   fwrite(file_data, file_size, 1, fileptr);
   fclose(fileptr);

   eina_file_map_free(f, file_data);
}
