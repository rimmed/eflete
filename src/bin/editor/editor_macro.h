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

#define EDITOR_STATE_DOUBLE(FUNC, ATTRIBUTE) \
Eina_Bool \
editor_state_## FUNC ##_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, \
                            const char *part_name, const char *state_name, double state_val, double new_val) \
{ \
   Diff *diff; \
   Attribute attribute = ATTRIBUTE; \
   assert(edit_object != NULL); \
   assert(part_name != NULL); \
   assert(state_name != NULL); \
   if (change) \
     { \
        double old_value = edje_edit_state_## FUNC ##_get(edit_object, part_name, state_name, state_val); \
        diff = mem_calloc(1, sizeof(Diff)); \
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE; \
        diff->redo.function = editor_state_## FUNC ##_set; \
        diff->redo.args.type_ssdd.s1 = eina_stringshare_add(part_name); \
        diff->redo.args.type_ssdd.s2 = eina_stringshare_add(state_name); \
        diff->redo.args.type_ssdd.d1 = state_val; \
        diff->redo.args.type_ssdd.d2 = new_val; \
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE; \
        diff->undo.function = editor_state_## FUNC ##_set; \
        diff->undo.args.type_ssdd.s1 = eina_stringshare_add(part_name); \
        diff->undo.args.type_ssdd.s2 = eina_stringshare_add(state_name); \
        diff->undo.args.type_ssdd.d1 = state_val; \
        diff->undo.args.type_ssdd.d2 = old_value; \
        if (merge) \
          change_diff_merge_add(change, diff); \
        else \
          change_diff_add(change, diff); \
     } \
   if (!edje_edit_state_## FUNC ##_set(edit_object, part_name, state_name, state_val, new_val)) \
     return false; \
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute); \
   return true; \
}
