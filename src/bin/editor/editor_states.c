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
#include "change.h"
#include "diff.h"

extern int _editor_signals_blocked;
EDITOR_STATE_DOUBLE(rel1_relative_x, ATTRIBUTE_STATE_REL1_RELATIVE_X)
EDITOR_STATE_DOUBLE(rel1_relative_y, ATTRIBUTE_STATE_REL1_RELATIVE_Y)
EDITOR_STATE_DOUBLE(rel2_relative_x, ATTRIBUTE_STATE_REL2_RELATIVE_X)
EDITOR_STATE_DOUBLE(rel2_relative_y, ATTRIBUTE_STATE_REL2_RELATIVE_Y)

EDITOR_STATE_INT(rel1_offset_x, ATTRIBUTE_STATE_REL1_OFFSET_X)
EDITOR_STATE_INT(rel1_offset_y, ATTRIBUTE_STATE_REL1_OFFSET_Y)
EDITOR_STATE_INT(rel2_offset_x, ATTRIBUTE_STATE_REL2_OFFSET_X)
EDITOR_STATE_INT(rel2_offset_y, ATTRIBUTE_STATE_REL2_OFFSET_Y)

EDITOR_STATE_DOUBLE(aspect_min, ATTRIBUTE_STATE_ASPECT_MIN)
EDITOR_STATE_DOUBLE(aspect_max, ATTRIBUTE_STATE_ASPECT_MAX)

EDITOR_STATE_DOUBLE(fill_origin_relative_x, ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X)
EDITOR_STATE_DOUBLE(fill_origin_relative_y, ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y)
EDITOR_STATE_INT(fill_origin_offset_x, ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X)
EDITOR_STATE_INT(fill_origin_offset_y, ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y)

EDITOR_STATE_DOUBLE(fill_size_relative_x, ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X)
EDITOR_STATE_DOUBLE(fill_size_relative_y, ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y)
EDITOR_STATE_INT(fill_size_offset_x, ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X)
EDITOR_STATE_INT(fill_size_offset_y, ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y)

EDITOR_STATE_DOUBLE_SAVE(container_align_x, ATTRIBUTE_STATE_CONTAINER_ALIGN_X)
EDITOR_STATE_DOUBLE_SAVE(container_align_y, ATTRIBUTE_STATE_CONTAINER_ALIGN_Y)

EDITOR_STATE_DOUBLE(minmul_h, ATTRIBUTE_STATE_MINMUL_H)
EDITOR_STATE_DOUBLE(minmul_w, ATTRIBUTE_STATE_MINMUL_W)

#define MAX_SET(VAL, VAL_CAPS) \
Eina_Bool \
editor_state_max_## VAL ##_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply, \
                           const char *part_name, const char *state_name, double state_val, int new_value) \
{ \
   int old_value; \
   int min_value; \
   Diff *diff; \
   Attribute attribute = ATTRIBUTE_STATE_MAX_##VAL_CAPS; \
 \
   assert(edit_object != NULL); \
   assert(new_value >= -1); \
 \
   old_value = edje_edit_state_max_## VAL ##_get(edit_object, part_name, state_name, state_val); \
   min_value = edje_edit_state_min_## VAL ##_get(edit_object, part_name, state_name, state_val); \
 \
   if ((min_value > new_value) && (new_value != -1)) \
     new_value = min_value; \
 \
   if (change) \
     { \
        diff = mem_calloc(1, sizeof(Diff)); \
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT; \
        diff->redo.function = editor_state_max_## VAL ##_set; \
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name); \
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name); \
        diff->redo.args.type_ssdi.d3 = state_val; \
        diff->redo.args.type_ssdi.i4 = new_value; \
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT; \
        diff->undo.function = editor_state_max_## VAL ##_set; \
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name); \
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name); \
        diff->undo.args.type_ssdi.d3 = state_val; \
        diff->undo.args.type_ssdi.i4 = old_value; \
        if (merge) \
          change_diff_merge_add(change, diff); \
        else \
          change_diff_add(change, diff); \
     } \
   if (apply) \
     { \
        CRIT_ON_FAIL(edje_edit_state_max_## VAL ##_set(edit_object, part_name, state_name, state_val, new_value)); \
        _editor_project_changed(); \
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute); \
     } \
   return true; \
}

MAX_SET(w, W)
MAX_SET(h, H)

#define MIN_SET(VAL, VAL_CAPS) \
Eina_Bool \
editor_state_min_## VAL ##_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply, \
                           const char *part_name, const char *state_name, double state_val, int new_value) \
{ \
   int old_value; \
   int max_value; \
   Diff *diff; \
   Attribute attribute = ATTRIBUTE_STATE_MIN_##VAL_CAPS; \
 \
   assert(edit_object != NULL); \
   assert(new_value >= 0); \
 \
   old_value = edje_edit_state_min_## VAL ##_get(edit_object, part_name, state_name, state_val); \
   max_value = edje_edit_state_max_## VAL ##_get(edit_object, part_name, state_name, state_val); \
 \
   if ((max_value < new_value) && (max_value != -1)) \
     new_value = max_value; \
 \
   if (change) \
     { \
        diff = mem_calloc(1, sizeof(Diff)); \
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT; \
        diff->redo.function = editor_state_min_## VAL ##_set; \
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name); \
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name); \
        diff->redo.args.type_ssdi.d3 = state_val; \
        diff->redo.args.type_ssdi.i4 = new_value; \
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT; \
        diff->undo.function = editor_state_min_## VAL ##_set; \
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name); \
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name); \
        diff->undo.args.type_ssdi.d3 = state_val; \
        diff->undo.args.type_ssdi.i4 = old_value; \
        if (merge) \
          change_diff_merge_add(change, diff); \
        else \
          change_diff_add(change, diff); \
     } \
   if (apply) \
     { \
        CRIT_ON_FAIL(edje_edit_state_min_## VAL ##_set(edit_object, part_name, state_name, state_val, new_value)); \
        _editor_project_changed(); \
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute); \
     } \
   return true; \
}

MIN_SET(w, W)
MIN_SET(h, H)

#define ALIGN_SET(FUNC, ATTRIBUTE) \
Eina_Bool \
editor_state_## FUNC ##_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply, \
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
        diff->redo.args.type_ssdd.d3 = state_val; \
        diff->redo.args.type_ssdd.d4 = new_val; \
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE; \
        diff->undo.function = editor_state_## FUNC ##_set; \
        diff->undo.args.type_ssdd.s1 = eina_stringshare_add(part_name); \
        diff->undo.args.type_ssdd.s2 = eina_stringshare_add(state_name); \
        diff->undo.args.type_ssdd.d3 = state_val; \
        diff->undo.args.type_ssdd.d4 = old_value; \
        if (merge) \
          change_diff_merge_add(change, diff); \
        else \
          change_diff_add(change, diff); \
     } \
   if (apply) \
     { \
       CRIT_ON_FAIL(edje_edit_state_## FUNC ##_set(edit_object, part_name, state_name, state_val, new_val)); \
       _editor_project_changed(); \
       if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute); \
     } \
   return true; \
}

ALIGN_SET(align_x, ATTRIBUTE_STATE_ALIGN_X)
ALIGN_SET(align_y, ATTRIBUTE_STATE_ALIGN_Y)

EDITOR_STATE_BOOL(fixed_h, ATTRIBUTE_STATE_FIXED_H)
EDITOR_STATE_BOOL(fixed_w, ATTRIBUTE_STATE_FIXED_W)
EDITOR_STATE_BOOL(fill_smooth, ATTRIBUTE_STATE_FILL_SMOOTH)
EDITOR_STATE_BOOL(visible, ATTRIBUTE_STATE_VISIBLE)
EDITOR_STATE_BOOL(container_min_h, ATTRIBUTE_STATE_CONTAINER_MIN_H)
EDITOR_STATE_BOOL(container_min_v, ATTRIBUTE_STATE_CONTAINER_MIN_V)

EDITOR_STATE_STRING_WITH_RESET(rel1_to_x, ATTRIBUTE_STATE_REL1_TO_X, true, rel1_relative_x)
EDITOR_STATE_STRING_WITH_RESET(rel1_to_y, ATTRIBUTE_STATE_REL1_TO_Y, true, rel1_relative_y)
EDITOR_STATE_STRING_WITH_RESET(rel2_to_x, ATTRIBUTE_STATE_REL2_TO_X, true, rel2_relative_x)
EDITOR_STATE_STRING_WITH_RESET(rel2_to_y, ATTRIBUTE_STATE_REL2_TO_Y, true, rel2_relative_y)
EDITOR_STATE_STRING(proxy_source, ATTRIBUTE_STATE_PROXY_SOURCE, true)

EDITOR_STATE_BOOL(map_on, ATTRIBUTE_STATE_MAP_ON)
EDITOR_STATE_BOOL(map_perspective_on, ATTRIBUTE_STATE_MAP_PERSPECTIVE_ON)
EDITOR_STATE_STRING(map_perspective, ATTRIBUTE_STATE_MAP_PERSPECTIVE, true)
EDITOR_STATE_STRING(map_light, ATTRIBUTE_STATE_MAP_LIGHT, true)
EDITOR_STATE_BOOL(map_smooth, ATTRIBUTE_STATE_MAP_SMOOTH)
EDITOR_STATE_BOOL(map_alpha, ATTRIBUTE_STATE_MAP_ALPHA)
EDITOR_STATE_BOOL(map_backface_cull, ATTRIBUTE_STATE_MAP_BACKFACE_CULL)
EDITOR_STATE_INT(map_perspective_focal, ATTRIBUTE_STATE_MAP_PERSPECTIVE_FOCAL)
EDITOR_STATE_INT(map_perspective_zplane, ATTRIBUTE_STATE_MAP_PERSPECTIVE_ZPLANE)
EDITOR_STATE_STRING(map_rotation_center, ATTRIBUTE_STATE_MAP_ROTATION_CENTER, true)

//map_rotation (3 values)
Eina_Bool
editor_state_map_rotation_x_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                                const char *part_name, const char *state_name, double state_val, double n4)
{
   Diff *diff;
   double o4, o5, o6;
   Attribute attribute = ATTRIBUTE_STATE_MAP_ROTATION_X;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   edje_edit_state_map_rotation_get(edit_object, part_name, state_name, state_val, &o4, &o5, &o6);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE;
        diff->redo.function = editor_state_map_rotation_x_set;
        diff->redo.args.type_ssdd.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdd.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdd.d3 = state_val;
        diff->redo.args.type_ssdd.d4 = n4;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE;
        diff->undo.function = editor_state_map_rotation_x_set;
        diff->undo.args.type_ssdd.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssdd.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssdd.d3 = state_val;
        diff->undo.args.type_ssdd.d4 = o4;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_map_rotation_set(edit_object, part_name, state_name, state_val, n4, o5, o6));
        _editor_project_changed();
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}

Eina_Bool
editor_state_map_rotation_y_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                                const char *part_name, const char *state_name, double state_val, double n5)
{
   Diff *diff;
   double o4, o5, o6;
   Attribute attribute = ATTRIBUTE_STATE_MAP_ROTATION_Y;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   edje_edit_state_map_rotation_get(edit_object, part_name, state_name, state_val, &o4, &o5, &o6);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE;
        diff->redo.function = editor_state_map_rotation_y_set;
        diff->redo.args.type_ssdd.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdd.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdd.d3 = state_val;
        diff->redo.args.type_ssdd.d4 = n5;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE;
        diff->undo.function = editor_state_map_rotation_y_set;
        diff->undo.args.type_ssdd.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssdd.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssdd.d3 = state_val;
        diff->undo.args.type_ssdd.d4 = o5;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_map_rotation_set(edit_object, part_name, state_name, state_val, o4, n5, o6));
        _editor_project_changed();
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}

Eina_Bool
editor_state_map_rotation_z_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                                const char *part_name, const char *state_name, double state_val, double n6)
{
   Diff *diff;
   double o4, o5, o6;
   Attribute attribute = ATTRIBUTE_STATE_MAP_ROTATION_Z;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   edje_edit_state_map_rotation_get(edit_object, part_name, state_name, state_val, &o4, &o5, &o6);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE;
        diff->redo.function = editor_state_map_rotation_z_set;
        diff->redo.args.type_ssdd.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdd.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdd.d3 = state_val;
        diff->redo.args.type_ssdd.d4 = n6;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_DOUBLE;
        diff->undo.function = editor_state_map_rotation_z_set;
        diff->undo.args.type_ssdd.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssdd.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssdd.d3 = state_val;
        diff->undo.args.type_ssdd.d4 = o6;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_map_rotation_set(edit_object, part_name, state_name, state_val, o4, o5, n6));
        _editor_project_changed();
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}

EDITOR_STATE_INT_INT_INT_INT_INT(map_point_color, map_point_color, ATTRIBUTE_STATE_MAP_POINT_COLOR, 1)
EDITOR_STATE_INT_INT_INT_INT_INT(map_point_color, map_point_color, ATTRIBUTE_STATE_MAP_POINT_COLOR, 2)
EDITOR_STATE_INT_INT_INT_INT_INT(map_point_color, map_point_color, ATTRIBUTE_STATE_MAP_POINT_COLOR, 3)
EDITOR_STATE_INT_INT_INT_INT_INT(map_point_color, map_point_color, ATTRIBUTE_STATE_MAP_POINT_COLOR, 4)

TODO("Fix edje_edit API")
//EDITOR_STATE_STRING(box_layout, ATTRIBUTE_STATE_BOX_LAYOUT)
//EDITOR_STATE_STRING(box_alt_layout, ATTRIBUTE_STATE_BOX_ALT_LAYOUT)

EDITOR_STATE_STRING(color_class, ATTRIBUTE_STATE_COLOR_CLASS, true)

TODO("Replace with image stub")
EDITOR_STATE_STRING_WITH_FALLBACK(image, ATTRIBUTE_STATE_IMAGE, NULL, false)

EDITOR_STATE_INT_INT_INT_INT(color, color, ATTRIBUTE_STATE_COLOR)
EDITOR_STATE_INT_INT_INT_INT(outline_color, color2, ATTRIBUTE_STATE_OUTLINE_COLOR)
EDITOR_STATE_INT_INT_INT_INT(shadow_color, color3, ATTRIBUTE_STATE_SHADOW_COLOR)

/* IMAGE BORDER */

Eina_Bool
editor_state_image_border_left_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                                   const char *part_name, const char *state_name, double state_val, int n4)
{
   Diff *diff;
   int o4, o5, o6, o7;
   Attribute attribute = ATTRIBUTE_STATE_IMAGE_BORDER_LEFT;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   edje_edit_state_image_border_get(edit_object, part_name, state_name, state_val, &o4, &o5, &o6, &o7);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->redo.function = editor_state_image_border_left_set;
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdi.d3 = state_val;
        diff->redo.args.type_ssdi.i4 = n4;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->undo.function = editor_state_image_border_left_set;
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssdi.d3 = state_val;
        diff->undo.args.type_ssdi.i4 = o4;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_image_border_set(edit_object, part_name, state_name, state_val, n4, o5, o6, o7));
        _editor_project_changed();
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}

Eina_Bool
editor_state_image_border_right_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                                    const char *part_name, const char *state_name, double state_val, int n5)
{
   Diff *diff;
   int o4, o5, o6, o7;
   Attribute attribute = ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   edje_edit_state_image_border_get(edit_object, part_name, state_name, state_val, &o4, &o5, &o6, &o7);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->redo.function = editor_state_image_border_right_set;
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdi.d3 = state_val;
        diff->redo.args.type_ssdi.i4 = n5;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->undo.function = editor_state_image_border_right_set;
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssdi.d3 = state_val;
        diff->undo.args.type_ssdi.i4 = o5;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_image_border_set(edit_object, part_name, state_name, state_val, o4, n5, o6, o7));
        _editor_project_changed();
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}

Eina_Bool
editor_state_image_border_top_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                                  const char *part_name, const char *state_name, double state_val, int n6)
{
   Diff *diff;
   int o4, o5, o6, o7;
   Attribute attribute = ATTRIBUTE_STATE_IMAGE_BORDER_TOP;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   edje_edit_state_image_border_get(edit_object, part_name, state_name, state_val, &o4, &o5, &o6, &o7);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->redo.function = editor_state_image_border_top_set;
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdi.d3 = state_val;
        diff->redo.args.type_ssdi.i4 = n6;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->undo.function = editor_state_image_border_top_set;
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssdi.d3 = state_val;
        diff->undo.args.type_ssdi.i4 = o6;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_image_border_set(edit_object, part_name, state_name, state_val, o4, o5, n6, o7));
        _editor_project_changed();
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}

Eina_Bool
editor_state_image_border_bottom_set(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                                     const char *part_name, const char *state_name, double state_val, int n7)
{
   Diff *diff;
   int o4, o5, o6, o7;
   Attribute attribute = ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   edje_edit_state_image_border_get(edit_object, part_name, state_name, state_val, &o4, &o5, &o6, &o7);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->redo.function = editor_state_image_border_bottom_set;
        diff->redo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdi.d3 = state_val;
        diff->redo.args.type_ssdi.i4 = n7;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_INT;
        diff->undo.function = editor_state_image_border_bottom_set;
        diff->undo.args.type_ssdi.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssdi.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssdi.d3 = state_val;
        diff->undo.args.type_ssdi.i4 = o7;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_image_border_set(edit_object, part_name, state_name, state_val, o4, o5, o6, n7));
        _editor_project_changed();
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}

/**********************/

EDITOR_STATE_UCHAR(image_border_fill, ATTRIBUTE_STATE_IMAGE_BORDER_FILL)
EDITOR_STATE_UCHAR(fill_type, ATTRIBUTE_STATE_FILL_TYPE)
EDITOR_STATE_UCHAR(aspect_pref, ATTRIBUTE_STATE_ASPECT_PREF)
EDITOR_STATE_UCHAR(table_homogeneous, ATTRIBUTE_STATE_TABLE_HOMOGENEOUS)

EDITOR_STATE_INT_SAVE(container_padding_x, ATTRIBUTE_STATE_CONTAINER_PADING_X)
EDITOR_STATE_INT_SAVE(container_padding_y, ATTRIBUTE_STATE_CONTAINER_PADING_Y)

Eina_Bool
editor_state_tween_add(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                       const char *part_name, const char *state_name, double state_val,
                       Eina_Stringshare *name)
{
   Diff *diff;
   Attribute attribute = ATTRIBUTE_STATE_IMAGE_TWEEN;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_STRING;
        diff->redo.function = editor_state_tween_add;
        diff->redo.args.type_ssds.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssds.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssds.d3 = state_val;
        diff->redo.args.type_ssds.s4 = eina_stringshare_add(name);
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_STRING;
        diff->undo.function = editor_state_tween_del;
        diff->undo.args.type_ssds.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssds.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssds.d3 = state_val;
        diff->undo.args.type_ssds.s4 = eina_stringshare_add(name);
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_tween_add(edit_object, part_name, state_name, state_val, name));
        _editor_project_changed();
        if (!_editor_signals_blocked)
          evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}
Eina_Bool
editor_state_tween_del(Evas_Object *edit_object, Change *change, Eina_Bool merge, Eina_Bool apply,
                       const char *part_name, const char *state_name, double state_val,
                       Eina_Stringshare *name)
{
   Diff *diff;
   Attribute attribute = ATTRIBUTE_STATE_IMAGE_TWEEN;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_STRING;
        diff->redo.function = editor_state_tween_del;
        diff->redo.args.type_ssds.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssds.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssds.d3 = state_val;
        diff->redo.args.type_ssds.s4 = eina_stringshare_add(name);
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_STRING;
        diff->undo.function = editor_state_tween_add;
        diff->undo.args.type_ssds.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssds.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssds.d3 = state_val;
        diff->undo.args.type_ssds.s4 = eina_stringshare_add(name);
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_tween_del(edit_object, part_name, state_name, state_val, name));
        _editor_project_changed();
        if (!_editor_signals_blocked)
          evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
     }
   return true;
}

Eina_Bool
editor_state_reset(Evas_Object *edit_object, Change *change, Eina_Bool apply,
                   const char *part_name, const char *state_name, double state_val)
{
   Eina_Bool res = true;
   Eina_List *tweens, *l;
   Eina_Stringshare *tween;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(state_name != NULL);

   you_shall_not_pass_editor_signals(change);

   Edje_Part_Type type = edje_edit_part_type_get(edit_object, part_name);

   switch (type)
     {
      case EDJE_PART_TYPE_TEXTBLOCK:
         res = res && editor_state_text_style_reset(edit_object, change, apply, part_name, state_name, state_val);
      case EDJE_PART_TYPE_TEXT:
         res = res && editor_state_text_align_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_align_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_elipsis_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_size_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_fit_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_fit_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_max_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_max_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_min_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_min_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_source_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_text_source_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_text_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_font_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_outline_color_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_shadow_color_reset(edit_object, change, apply, part_name, state_name, state_val);
         break;
      case EDJE_PART_TYPE_IMAGE:
         res = res && editor_state_fill_origin_relative_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_origin_relative_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_size_relative_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_size_relative_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_origin_offset_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_origin_offset_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_size_offset_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_size_offset_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_smooth_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_type_reset(edit_object, change, apply, part_name, state_name, state_val);

         res = res && editor_state_image_border_top_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_image_border_bottom_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_image_border_left_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_image_border_right_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_image_border_fill_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_image_reset(edit_object, change, apply, part_name, state_name, state_val);

         tweens = edje_edit_state_tweens_list_get(edit_object, part_name, state_name, state_val);
         EINA_LIST_FOREACH(tweens, l, tween)
            res = res && editor_state_tween_del(edit_object, change, apply, false, part_name, state_name, state_val, tween);
         edje_edit_string_list_free(tweens);
         break;
      case EDJE_PART_TYPE_PROXY:
         res = res && editor_state_fill_origin_relative_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_origin_relative_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_size_relative_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_size_relative_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_origin_offset_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_origin_offset_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_size_offset_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_size_offset_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_smooth_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_fill_type_reset(edit_object, change, apply, part_name, state_name, state_val);

         res = res && editor_state_proxy_source_reset(edit_object, change, apply, part_name, state_name, state_val);
         break;
      case EDJE_PART_TYPE_TABLE:
         res = res && editor_state_table_homogeneous_reset(edit_object, change, apply, part_name, state_name, state_val);
      case EDJE_PART_TYPE_BOX:
         res = res && editor_state_container_align_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_container_align_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_container_padding_x_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_container_padding_y_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_container_min_h_reset(edit_object, change, apply, part_name, state_name, state_val);
         res = res && editor_state_container_min_v_reset(edit_object, change, apply, part_name, state_name, state_val);
      default:
         break;
     }
   res = res && editor_state_align_x_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_align_y_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_aspect_max_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_aspect_min_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_aspect_pref_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_color_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_color_class_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_fixed_h_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_fixed_w_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_max_h_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_max_w_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_min_h_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_min_w_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_minmul_h_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_minmul_w_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel1_offset_x_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel1_offset_y_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel1_relative_x_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel1_relative_y_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel1_to_x_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel1_to_y_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel2_offset_x_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel2_offset_y_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel2_relative_x_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel2_relative_y_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel2_to_x_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_rel2_to_y_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_visible_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_on_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_perspective_on_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_smooth_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_alpha_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_backface_cull_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_perspective_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_light_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_rotation_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_point_color_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_rotation_center_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_perspective_focal_reset(edit_object, change, apply, part_name, state_name, state_val);
   res = res && editor_state_map_perspective_zplane_reset(edit_object, change, apply, part_name, state_name, state_val);

   you_shall_pass_editor_signals(change);

   return res;
}

Eina_Bool
editor_state_add(Evas_Object *edit_object, Change *change, Eina_Bool merge __UNUSED__, Eina_Bool apply,
                 const char *part_name, const char *state_name, double state_val)
{
   Diff *diff;
   Editor_State event_info;
   Edje_Part_Type type;

   assert(edit_object != NULL);

   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE;
        diff->redo.function = editor_state_add;
        diff->redo.args.type_ssd.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssd.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssd.d3 = state_val;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE;
        diff->undo.function = editor_state_del;
        diff->undo.args.type_ssd.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssd.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssd.d3 = state_val;

        change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_add(edit_object, part_name, state_name, state_val));

        /* fix incorrect default values */
        TODO("Fix edje_edit")
           type = edje_edit_part_type_get(edit_object, part_name);
        if (type == EDJE_PART_TYPE_BOX)
          CRIT_ON_FAIL(edje_edit_state_box_layout_set(edit_object, part_name, state_name, state_val, "horizontal"));

        /* apply our default values */
        if (!editor_state_reset(edit_object, NULL, apply, part_name, state_name, state_val))
          return false;

        _editor_project_changed();
        event_info.part_name = eina_stringshare_add(part_name);
        event_info.state_name = eina_stringshare_add(state_name);
        event_info.state_value = state_val;
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_STATE_ADDED, (void *)&event_info);
        eina_stringshare_del(event_info.part_name);
        eina_stringshare_del(event_info.state_name);
     }
   return true;
}

Eina_Bool
editor_state_copy(Evas_Object *edit_object, Change *change, Eina_Bool merge __UNUSED__, Eina_Bool apply,
                  const char *part_name, const char *from_name, double from_val,
                                         const char *state_name, double state_val)
{
   Diff *diff;
   Editor_State event_info;

   assert(edit_object != NULL);

   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE_STRING_DOUBLE;
        diff->redo.function = editor_state_copy;
        diff->redo.args.type_ssdsd.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssdsd.s2 = eina_stringshare_add(from_name);
        diff->redo.args.type_ssdsd.d3 = from_val;
        diff->redo.args.type_ssdsd.s4 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssdsd.d5 = state_val;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE;
        diff->undo.function = editor_state_del;
        diff->undo.args.type_ssd.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssd.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssd.d3 = state_val;

        change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_copy(edit_object, part_name, from_name, from_val, state_name, state_val));
        _editor_project_changed();
        event_info.part_name = eina_stringshare_add(part_name);
        event_info.state_name = eina_stringshare_add(state_name);
        event_info.state_value = state_val;
        if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_STATE_ADDED, (void *)&event_info);
        eina_stringshare_del(event_info.part_name);
        eina_stringshare_del(event_info.state_name);
     }
   return true;
}

Eina_Bool
editor_state_del(Evas_Object *edit_object, Change *change, Eina_Bool merge __UNUSED__, Eina_Bool apply,
                 const char *part_name, const char *state_name, double state_val)
{
   Diff *diff;
   Editor_State event_info;

   assert(edit_object != NULL);

   event_info.part_name = eina_stringshare_add(part_name);
   event_info.state_name = eina_stringshare_add(state_name);
   event_info.state_value = state_val;
   if (!_editor_signals_blocked) evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_STATE_DELETED, (void *)&event_info);
   if (change)
     {
        if (!editor_state_reset(edit_object, change, apply, part_name, state_name, state_val))
          return false;
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE;
        diff->redo.function = editor_state_del;
        diff->redo.args.type_ssd.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_ssd.s2 = eina_stringshare_add(state_name);
        diff->redo.args.type_ssd.d3 = state_val;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_DOUBLE;
        diff->undo.function = editor_state_add;
        diff->undo.args.type_ssd.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_ssd.s2 = eina_stringshare_add(state_name);
        diff->undo.args.type_ssd.d3 = state_val;

        change_diff_add(change, diff);
     }
   if (apply)
     {
        CRIT_ON_FAIL(edje_edit_state_del(edit_object, part_name, state_name, state_val));
        _editor_project_changed();
        eina_stringshare_del(event_info.part_name);
        eina_stringshare_del(event_info.state_name);
     }
   return true;
}
