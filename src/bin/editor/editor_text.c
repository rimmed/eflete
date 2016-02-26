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

#define ALLOW_DIRECT_EDJE_EDIT_CALLS
#include "editor.h"
#include "editor_macro.h"
#include "diff.h"
#include "change.h"

extern int _editor_signals_blocked;
EDITOR_STATE_DOUBLE(text_align_x, ATTRIBUTE_STATE_TEXT_ALIGN_X)
EDITOR_STATE_DOUBLE(text_align_y, ATTRIBUTE_STATE_TEXT_ALIGN_Y)

EDITOR_STATE_DOUBLE(text_elipsis, ATTRIBUTE_STATE_TEXT_ELIPSIS)

Eina_Bool
editor_state_text_size_set(Evas_Object *edit_object, Change *change, Eina_Bool merge,
                           const char *part_name, const char *state_name, double state_val, int new_val)
{
   Diff *diff;
   Attribute attribute = ATTRIBUTE_STATE_TEXT_SIZE;

   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   assert(new_val >= 0);

   if (change)
     {
        double old_value = edje_edit_state_text_size_get(edit_object, part_name, state_name, state_val);
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->redo.function = editor_state_text_size_set;
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdi.d3 = state_val;
        diff->redo.args.type_ssdi.i4 = new_val;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->undo.function = editor_state_text_size_set;
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssdi.d3 = state_val;
        diff->undo.args.type_ssdi.i4 = old_value;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (!edje_edit_state_text_size_set(edit_object, part_name, state_name, state_val, new_val))
     return false;
   _editor_project_changed();
   if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

EDITOR_STATE_BOOL(text_fit_x, ATTRIBUTE_STATE_TEXT_FIT_X)
EDITOR_STATE_BOOL(text_fit_y, ATTRIBUTE_STATE_TEXT_FIT_Y)
EDITOR_STATE_BOOL(text_max_x, ATTRIBUTE_STATE_TEXT_MAX_X)
EDITOR_STATE_BOOL(text_max_y, ATTRIBUTE_STATE_TEXT_MAX_Y)
EDITOR_STATE_BOOL(text_min_x, ATTRIBUTE_STATE_TEXT_MIN_X)
EDITOR_STATE_BOOL(text_min_y, ATTRIBUTE_STATE_TEXT_MIN_Y)

EDITOR_STATE_STRING(text_source, ATTRIBUTE_STATE_TEXT_SOURCE, true)
EDITOR_STATE_STRING(text_text_source, ATTRIBUTE_STATE_TEXT_TEXT_SOURCE, true)
EDITOR_STATE_STRING(text, ATTRIBUTE_STATE_TEXT, true)

EDITOR_STATE_STRING_WITH_FALLBACK(font, ATTRIBUTE_STATE_FONT, NULL)
TODO("Add style fallback here")
EDITOR_STATE_STRING_WITH_FALLBACK(text_style, ATTRIBUTE_STATE_TEXT_STYLE, NULL)
