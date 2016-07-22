/*
 * Edje Theme Editor
 * Copyright (C) 2013-2015 Samsung Electronics.
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

#include <string.h>
#include <Eina.h>
#include <assert.h>
#include "eflete.h"

int
sort_cb(const void *data1, const void *data2)
{
   if (!data1) return 1;
   if (!data2) return -1;

   if (data1 == data2)
     return 0;
   else
     return strcmp(data1, data2);
}

void
state_name_split(Eina_Stringshare *name, Eina_Stringshare **name_out, double *val_out)
{
   char **state_split;

   assert(name != NULL);
   assert(name_out != NULL || val_out != NULL);

   state_split = eina_str_split(name, " ", 2);
   if (name_out) *name_out = eina_stringshare_add(state_split[0]);
   if (val_out) *val_out = atof(state_split[1]);
   free(state_split[0]);
   free(state_split);
}

void
string_char_replace(char *str, char src, char rep)
{
   assert(str != NULL);

   while (*str != '\0')
     {
        if (*str == src) *str = rep;
        str++;
     }
}

char *
string_cat(const char *str1, const char *str2)
{
   char *string;
   string = mem_calloc(strlen(str1) + strlen(str2) + 1, sizeof(char));
   strcpy(string, str1);
   strcat(string, str2);

   return string;
}
