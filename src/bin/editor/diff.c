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

#include "diff.h"
#include "change.h"

typedef Eina_Bool (* function_type_int) (Evas_Object *, Change*, int);
typedef Eina_Bool (* function_type_string_string_double_double) (Evas_Object *, Change*, const char *, const char *, double, double);

static Eina_Bool
_apply(Evas_Object *obj, Function_Info *fi)
{
   switch (fi->type)
     {
      case FUNCTION_TYPE_NONE:
         return true;
      case FUNCTION_TYPE_INT:
         return ((function_type_int)fi->function)(obj, NULL, fi->args.type_int.ival);
      case FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE:
         return ((function_type_string_string_double_double)fi->function)(obj, NULL, fi->args.type_ssdd.s1, fi->args.type_ssdd.s2, fi->args.type_ssdd.d1, fi->args.type_ssdd.d2);

         /* Don't add 'case default:'. Compiler should warn about new values in enum */
     }
   return false;
}

Eina_Bool
diff_undo(Evas_Object *obj, Diff *diff)
{
   assert(obj != NULL);
   assert(diff != NULL);

   return _apply(obj, &diff->undo);
}

Eina_Bool
diff_redo(Evas_Object *obj, Diff *diff)
{
   assert(obj != NULL);
   assert(diff != NULL);

   return _apply(obj, &diff->redo);
}

void
diff_update(Diff *diff, Diff *new_diff)
{
   assert(diff != NULL);
   assert(new_diff != NULL);
   assert(diff->undo.function == new_diff->undo.function);
   assert(diff->redo.function == new_diff->redo.function);

   switch (diff->redo.type)
     {
      case FUNCTION_TYPE_NONE:
      case FUNCTION_TYPE_INT:
         break;
      case FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE:
         eina_stringshare_del(diff->redo.args.type_ssdd.s1);
         eina_stringshare_del(diff->redo.args.type_ssdd.s2);
         eina_stringshare_ref(new_diff->redo.args.type_ssdd.s1);
         eina_stringshare_ref(new_diff->redo.args.type_ssdd.s2);
         break;
         /* Do not forget to replace previous stringshares in existing_diff.redo
            if needed. */
         /* Don't add 'case default:'. Compiler should warn about new values in enum */
     }

   diff->redo = new_diff->redo;
   diff_free(new_diff);
}

void
diff_free(Diff *diff)
{
   assert(diff != NULL);

   switch (diff->redo.type)
     {
      case FUNCTION_TYPE_NONE:
      case FUNCTION_TYPE_INT:
         break;
      case FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE:
         eina_stringshare_del(diff->redo.args.type_ssdd.s1);
         eina_stringshare_del(diff->redo.args.type_ssdd.s2);
         break;
         /* Do not forget to clean stringshares */
         /* Don't add 'case default:'. Compiler should warn about new values in enum */
     }
   free(diff);
}
