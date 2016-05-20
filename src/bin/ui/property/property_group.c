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

#include "property.h"
#include "property_private.h"
#include "group_manager.h"
#include "history.h"
#include "change.h"
#include "project_manager.h"

#define PART_MASK(TYPE) (1u << TYPE)
#define PART_RECTANGLE PART_MASK(EDJE_PART_TYPE_RECTANGLE)
#define PART_TEXT PART_MASK(EDJE_PART_TYPE_TEXT)
#define PART_IMAGE PART_MASK(EDJE_PART_TYPE_IMAGE)
#define PART_SWALLOW PART_MASK(EDJE_PART_TYPE_SWALLOW)
#define PART_TEXTBLOCK PART_MASK(EDJE_PART_TYPE_TEXTBLOCK)
#define PART_GROUP PART_MASK(EDJE_PART_TYPE_GROUP)
#define PART_BOX PART_MASK(EDJE_PART_TYPE_BOX)
#define PART_TABLE PART_MASK(EDJE_PART_TYPE_TABLE)
#define PART_PROXY PART_MASK(EDJE_PART_TYPE_PROXY)
#define PART_SPACER PART_MASK(EDJE_PART_TYPE_SPACER)

/* +1 is needed because valid type can be 0 */
#define ACTION_MASK(TYPE) (1u << (TYPE+1))
#define ACTION_NONE ACTION_MASK(EDJE_ACTION_TYPE_NONE)
#define ACTION_STATE_SET ACTION_MASK(EDJE_ACTION_TYPE_STATE_SET)
#define ACTION_ACTION_STOP ACTION_MASK(EDJE_ACTION_TYPE_ACTION_STOP)
#define ACTION_SIGNAL_EMIT ACTION_MASK(EDJE_ACTION_TYPE_SIGNAL_EMIT)
#define ACTION_DRAG_VAL_SET ACTION_MASK(EDJE_ACTION_TYPE_DRAG_VAL_SET)
#define ACTION_DRAG_VAL_STEP ACTION_MASK(EDJE_ACTION_TYPE_DRAG_VAL_STEP)
#define ACTION_DRAG_VAL_PAGE ACTION_MASK(EDJE_ACTION_TYPE_DRAG_VAL_PAGE)
#define ACTION_SCRIPT ACTION_MASK(EDJE_ACTION_TYPE_SCRIPT)
#define ACTION_FOCUS_SET ACTION_MASK(EDJE_ACTION_TYPE_FOCUS_SET)
#define ACTION_RESERVED00 ACTION_MASK(EDJE_ACTION_TYPE_RESERVED00)
#define ACTION_FOCUS_OBJECT ACTION_MASK(EDJE_ACTION_TYPE_FOCUS_OBJECT)
#define ACTION_PARAM_COPY ACTION_MASK(EDJE_ACTION_TYPE_PARAM_COPY)
#define ACTION_PARAM_SET ACTION_MASK(EDJE_ACTION_TYPE_PARAM_SET)
#define ACTION_SOUND_SAMPLE ACTION_MASK(EDJE_ACTION_TYPE_SOUND_SAMPLE)
#define ACTION_SOUND_TONE ACTION_MASK(EDJE_ACTION_TYPE_SOUND_TONE)

struct _Property_Group_Data {
   Group *group;
   Part *part;
   Eina_Stringshare *program;
   Edje_Action_Type action_type;

   /* data needed to correctly handle changes that will be passed to history module */
   struct {
      const char *format;
      Change *change;
      struct {
         int int_val1, int_val2, int_val3, int_val4;
         double double_val1;
         Eina_Stringshare *str_val1;
         Eina_Bool bool_val1;
         unsigned short ushort_val1;
      } old, new;
   } history;
   Property_Attribute items[PROPERTY_GROUP_ITEM_LAST];
};
typedef struct _Property_Group_Data Property_Group_Data;
static Property_Group_Data group_pd;

struct _Property_Group_Update_Info {
   Property_Attribute *pa;
   Property_Action *action;
};
typedef struct _Property_Group_Update_Info Property_Group_Update_Info;

/* array to find item by Attribute */
static Property_Group_Update_Info attribute_map[ATTRIBUTE_LAST];

/* mapping enums to strings */
static const char *ignore_flags_strings[] = { STR_NONE,
                                              "On hold",
                                              NULL};
static const char *pointer_mode_strings[] = { "AUTOGRAB",
                                              "NOGRAB",
                                              NULL};
static const char *select_mode_strings[] = { "DEFAULT",
                                             "EXPLICIT",
                                             NULL};
static const char *entry_mode_strings[] = { STR_NONE,
                                            "PLAIN",
                                            "EDITABLE",
                                            "PASSWORD",
                                            NULL};
static const char *cursor_mode_strings[] = { "UNDER",
                                             "BEFORE",
                                             NULL};
static const char *fill_type_strings[] = { "SCALE",
                                           "TILE",
                                           NULL};

static const char *text_effect_strings[] = { STR_NONE,
                                             "plain",
                                             "outline",
                                             "soft outline",
                                             "shadow",
                                             "soft shadow",
                                             "outline shadow",
                                             "outline soft shadow",
                                             "far shadow",
                                             "far soft shadow",
                                             "glow",
                                             NULL};
static const char *text_shadow_direction_strings[] = { "bottom right",
                                                       "bottom",
                                                       "bottom left",
                                                       "left",
                                                       "top left",
                                                       "top",
                                                       "top right",
                                                       "right",
                                                       NULL};
static const char *aspect_preference_strings[] = { STR_NONE,
                                                   "Vertical",
                                                   "Horizontal",
                                                   "Both",
                                                   "Source",
                                                   NULL};
static const char *action_type[] = {"NONE",
                                    "STATE_SET",
                                    "ACTION_STOP",
                                    "SIGNAL_EMIT",
                                    "DRAG_VAL_SET",
                                    "DRAG_VAL_STEP",
                                    "DRAG_VAL_PAGE",
                                    "SCRIPT",
                                    "FOCUS_SET",
                                    "[NOT SUPPORTED] RESERVED00",
                                    "FOCUS_OBJECT",
                                    "[NOT SUPPORTED] PARAM_COPY",
                                    "[NOT SUPPORTED] PARAM_SET",
                                    "SOUND_SAMPLE",
                                    "SOUND_TONE",
                                    "[NOT SUPPORTED] PHYSICS_IMPULSE",
                                    "[NOT SUPPORTED] PHYSICS_TORQUE_IMPULSE",
                                    "[NOT SUPPORTED] PHYSICS_FORCE",
                                    "[NOT SUPPORTED] PHYSICS_TORQUE",
                                    "[NOT SUPPORTED] PHYSICS_FORCES_CLEAR",
                                    "[NOT SUPPORTED] PHYSICS_VEL_SET",
                                    "[NOT SUPPORTED] PHYSICS_ANG_VEL_SET",
                                    "[NOT SUPPORTED] PHYSICS_STOP",
                                    "[NOT SUPPORTED] PHYSICS_ROT_SET",
                                    "[NOT SUPPORTED] VIBRATION_SAMPLE"};

static const char *item_aspect_mode_strings[] = { STR_NONE,
                                                  "Neither",
                                                  "Horizontal",
                                                  "Vertical",
                                                  "Both",
                                                  NULL};
static const char *table_homogeneous_strings[] = { STR_NONE,
                                                   "Table",
                                                   "Item",
                                                   NULL};
static const char *
transition_type_strings[] = { STR_NONE,
                              "linear",
                              "sinusoidal",
                              "accelerate",
                              "decelerate",
                              "accelerate factor",
                              "decelerate factor",
                              "sinusoidal factor",
                              "devisior interpretation",
                              "bounce",
                              "spring",
                              "cubic bezier",
                              NULL};

static const char *image_border_fill_strings[] = { STR_NONE,
                                                   "Default",
                                                   "Solid",
                                                   NULL};
/* defines for args */
#define EDIT_OBJ group_pd.group->edit_object
#define PART_ARGS group_pd.part->name
#define ITEM_ARGS group_pd.part->name, group_pd.part->current_item_name
#define STATE_ARGS PART_ARGS, group_pd.part->current_state->parsed_name, group_pd.part->current_state->parsed_val
#define PROGRAM_ARGS group_pd.program
#define CHANGE_MERGE group_pd.history.change, true
#define CHANGE_NO_MERGE group_pd.history.change, false

/* global callbacks */
static void
_on_part_selected(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info)
{
   Property_Data *pd = data;
   group_pd.part = event_info;

   assert(pd != NULL);
   assert(group_pd.part != NULL);

   DBG("selected part \"%s\"", PART_ARGS);
   group_pd.program = NULL;

   GENLIST_FILTER_APPLY(pd->genlist);
   property_item_update_recursively(&group_pd.items[PROPERTY_GROUP_ITEM_PART_TITLE]);
   property_item_update_recursively(&group_pd.items[PROPERTY_GROUP_ITEM_STATE_TITLE]);
}

static void
_on_group_navigator_unselected(void *data,
                               Evas_Object *obj __UNUSED__,
                               void *event_info __UNUSED__)
{
   Property_Data *pd = data;

   assert(pd != NULL);

   DBG("unselected_cb\n");
   group_pd.part = NULL;
   group_pd.program = NULL;
   GENLIST_FILTER_APPLY(pd->genlist);
}

static void
_on_part_state_selected(void *data __UNUSED__,
                        Evas_Object *obj __UNUSED__,
                        void *event_info)
{
   Part *part = event_info;

   assert(group_pd.part == part);

   DBG("selected state \"%s\"", group_pd.part->current_state->name);
   property_item_update_recursively(&group_pd.items[PROPERTY_GROUP_ITEM_STATE_TITLE]);
}

static void
_on_program_selected(void *data,
                     Evas_Object *obj __UNUSED__,
                     void *event_info)
{
   Property_Data *pd = data;
   group_pd.program = event_info;

   assert(pd != NULL);
   assert(group_pd.program != NULL);

   group_pd.action_type = edje_edit_program_action_get(EDIT_OBJ, PROGRAM_ARGS);
   DBG("selected program \"%s\"", group_pd.program);
   group_pd.part = NULL;

   GENLIST_FILTER_APPLY(pd->genlist);
   property_item_update_recursively(&group_pd.items[PROPERTY_GROUP_ITEM_PROGRAM_TITLE]);
}

static void
_on_group_changed(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info)
{
   Property_Data *pd = data;
   group_pd.group = event_info;

   assert(pd != NULL);

   DBG("group changed to \"%s\"", group_pd.group ? group_pd.group->name : NULL);
   group_pd.part = group_pd.group ? group_pd.group->current_part : NULL;
   group_pd.program = group_pd.group ? group_pd.group->current_program : NULL;

   GENLIST_FILTER_APPLY(pd->genlist);

   if (group_pd.group)
     property_item_update_recursively(&group_pd.items[PROPERTY_GROUP_ITEM_GROUP_TITLE]);

   if (group_pd.part)
     {
        property_item_update_recursively(&group_pd.items[PROPERTY_GROUP_ITEM_PART_TITLE]);
        property_item_update_recursively(&group_pd.items[PROPERTY_GROUP_ITEM_STATE_TITLE]);
     }
}

static void
_on_editor_attribute_changed(void *data __UNUSED__,
                             Evas_Object *obj __UNUSED__,
                             void *event_info)
{
   Attribute *attribute = event_info;
   Property_Attribute *pa = attribute_map[*attribute].pa;
   Property_Action *action = attribute_map[*attribute].action;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->update_cb != NULL);

   if (pa->realized)
     action->update_cb(pa, action);
}

static Eina_Bool
_filter_cb(Property_Attribute *pa)
{
   assert(pa != NULL);

   switch (pa->type.group_item)
     {
      case PROPERTY_GROUP_ITEM_GROUP_TITLE:
         return group_pd.group != NULL;
      case PROPERTY_GROUP_ITEM_PART_TITLE:
      case PROPERTY_GROUP_ITEM_STATE_TITLE:
         return group_pd.part != NULL;

      case PROPERTY_GROUP_ITEM_PART_ITEM_TITLE:
         return group_pd.part &&
            group_pd.part->current_item_name != NULL;

      case PROPERTY_GROUP_ITEM_PROGRAM_TITLE:
         return group_pd.program != NULL;

      default:
         if (group_pd.part)
           return !!(pa->filter_data.part_types & PART_MASK(group_pd.part->type));
         else if (group_pd.program)
           return !!(pa->filter_data.action_types & ACTION_MASK(group_pd.action_type));
         else
           return true;
     }
}

static Eina_Bool
_transition_filter_cb(Property_Attribute *pa)
{
   if (!group_pd.program) return false;
   if (group_pd.action_type != EDJE_ACTION_TYPE_STATE_SET) return false;

   Edje_Tween_Mode type = editor_program_transition_type_get(EDIT_OBJ, group_pd.program);

   assert(pa != NULL);

   switch (type)
     {
      case EDJE_TWEEN_MODE_ACCELERATE_FACTOR:
      case EDJE_TWEEN_MODE_DECELERATE_FACTOR:
      case EDJE_TWEEN_MODE_SINUSOIDAL_FACTOR:
         return (pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_FACTOR);
      case EDJE_TWEEN_MODE_DIVISOR_INTERP:
         return ((pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_FACTOR) ||
                 (pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_GRADIENT));
      case EDJE_TWEEN_MODE_BOUNCE:
         return ((pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_DECAY) ||
                 (pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BOUNCES));
      case EDJE_TWEEN_MODE_SPRING:
         return ((pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_DECAY) ||
                 (pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_SWINGS));
      case EDJE_TWEEN_MODE_CUBIC_BEZIER:
         return ((pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BEZIER_POINT1) ||
                 (pa->type.group_item == PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BEZIER_POINT2));
      default:
      case EDJE_TWEEN_MODE_NONE:
      case EDJE_TWEEN_MODE_LINEAR:
      case EDJE_TWEEN_MODE_SINUSOIDAL:
      case EDJE_TWEEN_MODE_ACCELERATE:
      case EDJE_TWEEN_MODE_DECELERATE:
         return false;
     }
}

/* local callbacks */
static Eina_List *
_subitems_get(Property_Attribute *pa)
{
   Eina_List *items = NULL;

   assert(pa != NULL);
#define APPEND(TYPE) items = eina_list_append(items, &group_pd.items[TYPE]);
   switch (pa->type.group_item)
     {
      case PROPERTY_GROUP_ITEM_GROUP_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_GROUP_NAME);
         APPEND(PROPERTY_GROUP_ITEM_GROUP_MIN);
         APPEND(PROPERTY_GROUP_ITEM_GROUP_MAX);
         break;
      case PROPERTY_GROUP_ITEM_PART_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_PART_NAME);
         APPEND(PROPERTY_GROUP_ITEM_PART_TYPE);
         APPEND(PROPERTY_GROUP_ITEM_PART_SCALE);
         APPEND(PROPERTY_GROUP_ITEM_PART_MOUSE_EVENTS);
         APPEND(PROPERTY_GROUP_ITEM_PART_REPEAT_EVENTS);
         APPEND(PROPERTY_GROUP_ITEM_PART_CLIP_TO);
         APPEND(PROPERTY_GROUP_ITEM_PART_IGNORE_FLAGS);
         APPEND(PROPERTY_GROUP_ITEM_PART_POINTER_MODE);
         APPEND(PROPERTY_GROUP_ITEM_PART_GROUP_SOURCE);
         APPEND(PROPERTY_GROUP_ITEM_PART_TEXT_EFFECT);
         APPEND(PROPERTY_GROUP_ITEM_PART_TEXT_SHADOW_DIRECTION);
         APPEND(PROPERTY_GROUP_ITEM_PART_SELECT_MODE);
         APPEND(PROPERTY_GROUP_ITEM_PART_ENTRY_MODE);
         APPEND(PROPERTY_GROUP_ITEM_PART_CURSOR_MODE);
         APPEND(PROPERTY_GROUP_ITEM_PART_MULTILINE);
         APPEND(PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_SELECTION_UNDER);
         APPEND(PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_SELECTION_OVER);
         APPEND(PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_CURSOR_UNDER);
         APPEND(PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_CURSOR_OVER);
         APPEND(PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_ANCHORS_UNDER);
         APPEND(PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_ANCHORS_OVER);
         APPEND(PROPERTY_GROUP_ITEM_PART_DRAGABLE_TITLE);
         break;
      case PROPERTY_GROUP_ITEM_PART_DRAGABLE_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_PART_DRAGABLE_ENABLE);
         APPEND(PROPERTY_GROUP_ITEM_PART_DRAGABLE_STEP);
         APPEND(PROPERTY_GROUP_ITEM_PART_DRAGABLE_COUNT);
         APPEND(PROPERTY_GROUP_ITEM_PART_DRAGABLE_CONFINE);
         APPEND(PROPERTY_GROUP_ITEM_PART_DRAGABLE_THRESHOLD);
         APPEND(PROPERTY_GROUP_ITEM_PART_DRAGABLE_EVENTS);
         break;
      case PROPERTY_GROUP_ITEM_STATE_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_NAME);
         APPEND(PROPERTY_GROUP_ITEM_STATE_VISIBLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_PROXY_SOURCE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_COLORS_TITLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_SIZE_TITLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_TITLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_IMAGE_TITLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_TITLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_TITLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_CONTAINER_TITLE);
         break;
      case PROPERTY_GROUP_ITEM_STATE_SIZE_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_SIZE_MIN);
         APPEND(PROPERTY_GROUP_ITEM_STATE_SIZE_MAX);
         APPEND(PROPERTY_GROUP_ITEM_STATE_SIZE_MINMUL);
         APPEND(PROPERTY_GROUP_ITEM_STATE_SIZE_FIXED);
         APPEND(PROPERTY_GROUP_ITEM_STATE_SIZE_ASPECT_PREF);
         APPEND(PROPERTY_GROUP_ITEM_STATE_SIZE_ASPECT);
         break;
      case PROPERTY_GROUP_ITEM_STATE_POSITION_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_ALIGN);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_TITLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_TITLE);
         break;
      case PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_TO_X);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_TO_Y);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_RELATIVE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_OFFSET);
         break;
      case PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_TO_X);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_TO_Y);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_RELATIVE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_OFFSET);
         break;
      case PROPERTY_GROUP_ITEM_STATE_IMAGE_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_IMAGE_NORMAL);
         APPEND(PROPERTY_GROUP_ITEM_STATE_IMAGE_TWEEN);
         APPEND(PROPERTY_GROUP_ITEM_STATE_IMAGE_MIDDLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_IMAGE_BORDER_H);
         APPEND(PROPERTY_GROUP_ITEM_STATE_IMAGE_BORDER_V);
         break;
      case PROPERTY_GROUP_ITEM_STATE_FILL_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_TYPE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_SMOOTH);
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_ORIGIN_TITLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_SIZE_TITLE);
         break;
      case PROPERTY_GROUP_ITEM_STATE_FILL_ORIGIN_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_ORIGIN_RELATIVE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_ORIGIN_OFFSET);
         break;
      case PROPERTY_GROUP_ITEM_STATE_FILL_SIZE_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_SIZE_RELATIVE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_FILL_SIZE_OFFSET);
         break;
      case PROPERTY_GROUP_ITEM_STATE_COLORS_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_COLORS_COLOR_CLASS);
         APPEND(PROPERTY_GROUP_ITEM_STATE_COLORS_COLOR);
         APPEND(PROPERTY_GROUP_ITEM_STATE_COLORS_OUTLINE_COLOR);
         APPEND(PROPERTY_GROUP_ITEM_STATE_COLORS_SHADOWCOLOR);
         break;
      case PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_TEXT);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_ALIGN);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_MIN);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_MAX);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_FONT);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_SIZE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_STYLE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_FIT);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_SOURCE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_TEXT_SOURCE);
         APPEND(PROPERTY_GROUP_ITEM_STATE_TEXT_ELIPSIS);
         break;
      case PROPERTY_GROUP_ITEM_PROGRAM_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_NAME);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_SIGNAL);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_SOURCE);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_IN);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_FILTER_PART);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_FILTER_STATE);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TITLE);
         break;
      case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_STATE);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_VALUE);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_EMIT_SIGNAL);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_EMIT_SOURCE);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_DRAG_VALUE);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_TITLE);
         break;
      case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_TYPE);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_TIME);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_FACTOR);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_GRADIENT);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_DECAY);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BOUNCES);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_SWINGS);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BEZIER_POINT1);
         APPEND(PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BEZIER_POINT2);
         break;
      case PROPERTY_GROUP_ITEM_STATE_CONTAINER_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_STATE_CONTAINER_ALIGN);
         APPEND(PROPERTY_GROUP_ITEM_STATE_CONTAINER_PADDING);
         APPEND(PROPERTY_GROUP_ITEM_STATE_CONTAINER_MIN);
         APPEND(PROPERTY_GROUP_ITEM_STATE_CONTAINER_HOMOGENEOUS);
         break;
      case PROPERTY_GROUP_ITEM_PART_ITEM_TITLE:
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_NAME);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_SOURCE);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_MIN);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_MAX);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_PREFER);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_ALIGN);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_WEIGHT);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_ASPECT_MODE);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_ASPECT);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_SPREAD);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_SPAN);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_POSITION);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_PADDING_H);
         APPEND(PROPERTY_GROUP_ITEM_PART_ITEM_PADDING_V);
         break;
      default:
         CRIT("items callback not found for %s", pa->name);
         abort();
     }

   return items;
#undef APPEND
}

static void
_fill_combobox_with_enum(Evas_Object *control, const char **array)
{
   int i = 0;

   assert(control != NULL);
   assert(array != NULL);

   while (array[i] != NULL)
     {
        ewe_combobox_item_add(control, array[i]);
        ++i;
     }
}

static void
_color_class_colors_fill(void *data __UNUSED__,
                         Evas_Object *combo,
                         void *event_info __UNUSED__)
{
   int cc_val[12];
   Evas_Object *color;
   const Eina_List *items, *l;
   Ewe_Combobox_Item *item;
   Evas *canvas;

   items = ewe_combobox_items_list_get(combo);
   if (!items) return;

   item = eina_list_data_get(items);
   canvas = evas_object_evas_get(item->content);
   items = eina_list_next(items);
   EINA_LIST_FOREACH(items, l, item)
     {
        edje_edit_color_class_colors_get(EDIT_OBJ, item->title,
                                         &cc_val[0], &cc_val[1], &cc_val[2], &cc_val[3],
                                         &cc_val[4], &cc_val[5], &cc_val[6], &cc_val[7],
                                         &cc_val[8], &cc_val[9], &cc_val[10], &cc_val[11]);
        evas_color_argb_premul(cc_val[3], &cc_val[0], &cc_val[1], &cc_val[2]);
        evas_color_argb_premul(cc_val[7], &cc_val[4], &cc_val[5], &cc_val[6]);
        evas_color_argb_premul(cc_val[11], &cc_val[8], &cc_val[9], &cc_val[10]);
        /* FIXME: this is bad solition, user shoud not use edje object for add contnent to a
         * combobox item. Need to move combobox from edje ocject to layout. */
        color = edje_object_add(canvas);
        edje_object_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
        evas_object_color_set(color, cc_val[0], cc_val[1], cc_val[2], cc_val[3]);
        edje_object_part_swallow(item->content, "swallow.color1", color);
        color = edje_object_add(canvas);
        edje_object_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
        evas_object_color_set(color, cc_val[4], cc_val[5], cc_val[6], cc_val[7]);
        edje_object_part_swallow(item->content, "swallow.color2", color);
        color = edje_object_add(canvas);
        edje_object_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
        evas_object_color_set(color, cc_val[8], cc_val[9], cc_val[10], cc_val[11]);
        edje_object_part_swallow(item->content, "swallow.color3", color);
     }
}

static void
_init_cb(Property_Attribute *pa, Property_Action *action)
{
   Evas_Object *color;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
      case ATTRIBUTE_STATE_NAME:
      case ATTRIBUTE_PROGRAM_NAME:
      case ATTRIBUTE_PART_ITEM_NAME:
      case ATTRIBUTE_STATE_IMAGE:
         elm_object_disabled_set(action->control, true);
         break;
      case ATTRIBUTE_STATE_IMAGE_TWEEN:
      case ATTRIBUTE_GROUP_MIN_W:
      case ATTRIBUTE_GROUP_MIN_H:
      case ATTRIBUTE_GROUP_MAX_W:
      case ATTRIBUTE_GROUP_MAX_H:
      case ATTRIBUTE_PART_NAME:
      case ATTRIBUTE_PART_TYPE:
      case ATTRIBUTE_PROGRAM_ACTION:
      case ATTRIBUTE_PART_SCALE:
      case ATTRIBUTE_PART_MOUSE_EVENTS:
      case ATTRIBUTE_PART_REPEAT_EVENTS:
      case ATTRIBUTE_PART_CLIP_TO:
      case ATTRIBUTE_PART_DRAG_STEP_X:
      case ATTRIBUTE_PART_DRAG_STEP_Y:
      case ATTRIBUTE_PART_DRAG_COUNT_X:
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
      case ATTRIBUTE_PART_DRAG_CONFINE:
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
      case ATTRIBUTE_PART_DRAG_EVENT:
      case ATTRIBUTE_PART_GROUP_SOURCE:
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_OVER:
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_OVER:
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_OVER:
      case ATTRIBUTE_PART_MULTILINE:
      case ATTRIBUTE_PART_ITEM_SOURCE:
      case ATTRIBUTE_PART_ITEM_MIN_W:
      case ATTRIBUTE_PART_ITEM_MIN_H:
      case ATTRIBUTE_PART_ITEM_PREFER_W:
      case ATTRIBUTE_PART_ITEM_PREFER_H:
      case ATTRIBUTE_PART_ITEM_ASPECT_W:
      case ATTRIBUTE_PART_ITEM_ASPECT_H:
      case ATTRIBUTE_PART_ITEM_PADDING_TOP:
      case ATTRIBUTE_PART_ITEM_PADDING_BOTTOM:
      case ATTRIBUTE_PART_ITEM_PADDING_LEFT:
      case ATTRIBUTE_PART_ITEM_PADDING_RIGHT:
      case ATTRIBUTE_STATE_IMAGE_BORDER_TOP:
      case ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM:
      case ATTRIBUTE_STATE_IMAGE_BORDER_LEFT:
      case ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT:
      case ATTRIBUTE_STATE_PROXY_SOURCE:
      case ATTRIBUTE_STATE_VISIBLE:
      case ATTRIBUTE_STATE_FILL_SMOOTH:
      case ATTRIBUTE_STATE_MIN_W:
      case ATTRIBUTE_STATE_MIN_H:
      case ATTRIBUTE_STATE_MINMUL_W:
      case ATTRIBUTE_STATE_MINMUL_H:
      case ATTRIBUTE_STATE_FIXED_W:
      case ATTRIBUTE_STATE_FIXED_H:
      case ATTRIBUTE_STATE_REL1_TO_X:
      case ATTRIBUTE_STATE_REL1_TO_Y:
      case ATTRIBUTE_STATE_REL2_TO_X:
      case ATTRIBUTE_STATE_REL2_TO_Y:
      case ATTRIBUTE_STATE_COLOR:
      case ATTRIBUTE_STATE_OUTLINE_COLOR:
      case ATTRIBUTE_STATE_SHADOW_COLOR:
      case ATTRIBUTE_STATE_TEXT:
      case ATTRIBUTE_STATE_FONT:
      case ATTRIBUTE_STATE_TEXT_MIN_X:
      case ATTRIBUTE_STATE_TEXT_MIN_Y:
      case ATTRIBUTE_STATE_TEXT_MAX_X:
      case ATTRIBUTE_STATE_TEXT_MAX_Y:
      case ATTRIBUTE_STATE_TEXT_FIT_X:
      case ATTRIBUTE_STATE_TEXT_FIT_Y:
      case ATTRIBUTE_STATE_TEXT_SIZE:
      case ATTRIBUTE_STATE_TEXT_ELIPSIS_ENABLE:
      case ATTRIBUTE_STATE_TEXT_SOURCE:
      case ATTRIBUTE_STATE_TEXT_TEXT_SOURCE:
      case ATTRIBUTE_STATE_TEXT_STYLE:
      case ATTRIBUTE_PROGRAM_SIGNAL:
      case ATTRIBUTE_PROGRAM_SOURCE:
      case ATTRIBUTE_STATE_CONTAINER_PADING_X:
      case ATTRIBUTE_STATE_CONTAINER_PADING_Y:
      case ATTRIBUTE_STATE_CONTAINER_MIN_V:
      case ATTRIBUTE_STATE_CONTAINER_MIN_H:
      case ATTRIBUTE_PROGRAM_FILTER_PART:
      case ATTRIBUTE_PROGRAM_FILTER_STATE:
      case ATTRIBUTE_PROGRAM_EMIT_SIGNAL:
      case ATTRIBUTE_PROGRAM_EMIT_SOURCE:
      case ATTRIBUTE_PROGRAM_STATE:
         break;
      case ATTRIBUTE_STATE_MAX_W:
      case ATTRIBUTE_STATE_MAX_H:
      case ATTRIBUTE_PART_ITEM_MAX_W:
      case ATTRIBUTE_PART_ITEM_MAX_H:
         elm_spinner_min_max_set(action->control, -1, 9999);
         break;
      case ATTRIBUTE_PART_ITEM_SPREAD_W:
      case ATTRIBUTE_PART_ITEM_SPREAD_H:
      case ATTRIBUTE_PART_ITEM_SPAN_COL:
      case ATTRIBUTE_PART_ITEM_SPAN_ROW:
         elm_spinner_min_max_set(action->control, 1, 100);
         break;
      case ATTRIBUTE_PART_ITEM_POSITION_COL:
      case ATTRIBUTE_PART_ITEM_POSITION_ROW:
         elm_spinner_min_max_set(action->control, 0, 100);
         break;
      case ATTRIBUTE_PART_DRAG_X:
      case ATTRIBUTE_PART_DRAG_Y:
         elm_spinner_min_max_set(action->control, -1, 1);
         break;
      case ATTRIBUTE_PART_ITEM_WEIGHT_X:
      case ATTRIBUTE_PART_ITEM_WEIGHT_Y:
         elm_spinner_min_max_set(action->control, -1, 1);
         elm_spinner_step_set(action->control, 0.1);
         elm_spinner_label_format_set(action->control, "%.2f");
         break;
         break;
      case ATTRIBUTE_STATE_ASPECT_MIN:
      case ATTRIBUTE_STATE_ASPECT_MAX:
      case ATTRIBUTE_PROGRAM_IN_FROM:
      case ATTRIBUTE_PROGRAM_IN_RANGE:
      case ATTRIBUTE_PROGRAM_TRANSITION_TIME:
      case ATTRIBUTE_PROGRAM_TRANSITION_BOUNCES:
      case ATTRIBUTE_PROGRAM_TRANSITION_SWINGS:
         elm_spinner_step_set(action->control, 0.1);
         elm_spinner_label_format_set(action->control, "%.2f");
         break;
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_X:
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_Y:
      case ATTRIBUTE_PROGRAM_TRANSITION_FACTOR:
      case ATTRIBUTE_PROGRAM_TRANSITION_GRADIENT:
      case ATTRIBUTE_PROGRAM_TRANSITION_DECAY:
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X1:
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y1:
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X2:
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y2:
         elm_spinner_min_max_set(action->control, -9999, 9999);
         elm_spinner_step_set(action->control, 0.1);
         elm_spinner_label_format_set(action->control, "%.2f");
         break;
      case ATTRIBUTE_STATE_TEXT_ELIPSIS:
      case ATTRIBUTE_STATE_ALIGN_X:
      case ATTRIBUTE_STATE_ALIGN_Y:
      case ATTRIBUTE_STATE_TEXT_ALIGN_X:
      case ATTRIBUTE_STATE_TEXT_ALIGN_Y:
      case ATTRIBUTE_PART_ITEM_ALIGN_X:
      case ATTRIBUTE_PART_ITEM_ALIGN_Y:
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_X:
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_Y:
      case ATTRIBUTE_PROGRAM_VALUE:
         elm_spinner_min_max_set(action->control, 0, 1);
         elm_spinner_step_set(action->control, 0.1);
         elm_spinner_label_format_set(action->control, "%.2f");
         break;
      case ATTRIBUTE_STATE_REL1_RELATIVE_X:
      case ATTRIBUTE_STATE_REL1_RELATIVE_Y:
      case ATTRIBUTE_STATE_REL2_RELATIVE_X:
      case ATTRIBUTE_STATE_REL2_RELATIVE_Y:
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X:
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y:
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X:
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y:
         elm_spinner_min_max_set(action->control, -10, 10);
         elm_spinner_step_set(action->control, 0.1);
         elm_spinner_label_format_set(action->control, "%.2f");
         break;
      case ATTRIBUTE_STATE_REL1_OFFSET_X:
      case ATTRIBUTE_STATE_REL1_OFFSET_Y:
      case ATTRIBUTE_STATE_REL2_OFFSET_X:
      case ATTRIBUTE_STATE_REL2_OFFSET_Y:
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X:
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y:
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X:
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y:
         elm_spinner_min_max_set(action->control, -9999, 9999);
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_MODE:
         _fill_combobox_with_enum(action->control, item_aspect_mode_strings);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_TYPE:
         _fill_combobox_with_enum(action->control, transition_type_strings);
         break;
      case ATTRIBUTE_STATE_ASPECT_PREF:
         _fill_combobox_with_enum(action->control, aspect_preference_strings);
         break;
      case ATTRIBUTE_PART_IGNORE_FLAGS:
         _fill_combobox_with_enum(action->control, ignore_flags_strings);
         break;
      case ATTRIBUTE_PART_POINTER_MODE:
         _fill_combobox_with_enum(action->control, pointer_mode_strings);
         break;
      case ATTRIBUTE_PART_SELECT_MODE:
         _fill_combobox_with_enum(action->control, select_mode_strings);
         break;
      case ATTRIBUTE_PART_ENTRY_MODE:
         _fill_combobox_with_enum(action->control, entry_mode_strings);
         break;
      case ATTRIBUTE_PART_CURSOR_MODE:
         _fill_combobox_with_enum(action->control, cursor_mode_strings);
         break;
      case ATTRIBUTE_STATE_FILL_TYPE:
         _fill_combobox_with_enum(action->control, fill_type_strings);
         break;
      case ATTRIBUTE_PART_TEXT_EFFECT:
         _fill_combobox_with_enum(action->control, text_effect_strings);
         break;
      case ATTRIBUTE_PART_TEXT_SHADOW_DIRECTION:
         _fill_combobox_with_enum(action->control, text_shadow_direction_strings);
         break;
      case ATTRIBUTE_STATE_TABLE_HOMOGENEOUS:
         _fill_combobox_with_enum(action->control, table_homogeneous_strings);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_FILL:
         _fill_combobox_with_enum(action->control, image_border_fill_strings);
         break;
      case ATTRIBUTE_STATE_COLOR_CLASS:
         ewe_combobox_style_set(action->control, "color_class");

         color = elm_image_add(action->control);
         elm_image_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
         ewe_combobox_content_set(action->control, "swallow.color1", color);
         evas_object_data_set(action->control, "color1", color);

         color = elm_image_add(action->control);
         elm_image_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
         ewe_combobox_content_set(action->control, "swallow.color2", color);
         evas_object_data_set(action->control, "color2", color);

         color = elm_image_add(action->control);
         elm_image_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
         ewe_combobox_content_set(action->control, "swallow.color3", color);
         evas_object_data_set(action->control, "color3", color);

         evas_object_smart_callback_add(action->control, "expanded", _color_class_colors_fill, NULL);
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("init callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
}

static void
_styles_combobox_fill(Evas_Object *combo, const char *selected)
{
   Eina_List *l;
   Resource *style;

   assert(combo != NULL);

   if (selected)
     ewe_combobox_text_set(combo, selected);
   else
     ewe_combobox_text_set(combo, STR_NONE);
   ewe_combobox_item_add(combo, STR_NONE);
   EINA_LIST_FOREACH(ap.project->styles, l, style)
     {
        ewe_combobox_item_add(combo, style->name);
     }
}

static void
_groups_combobox_fill(Evas_Object *combo, const char *selected, Eina_Bool with_none)
{
   Eina_List *l;
   Group *group;

   assert(with_none || selected != NULL);

   if (selected)
     ewe_combobox_text_set(combo, selected);
   else
     ewe_combobox_text_set(combo, STR_NONE);

   if (with_none)
     ewe_combobox_item_add(combo, STR_NONE);

   EINA_LIST_FOREACH(ap.project->groups, l, group)
     {
        if (group != group_pd.group)
          ewe_combobox_item_add(combo, group->name);
     }
}

static void
_parts_combobox_fill(Evas_Object *combo, const char *selected, int allowed_types_mask)
{
   Eina_List *l;
   Part *part;

   assert(combo != NULL);

   if (selected)
     ewe_combobox_text_set(combo, selected);
   else
     ewe_combobox_text_set(combo, STR_NONE);
   ewe_combobox_item_add(combo, STR_NONE);

   if (allowed_types_mask)
     {
        EINA_LIST_FOREACH(group_pd.group->parts, l, part)
          {
             if ((PART_MASK(part->type) & allowed_types_mask) && (part != group_pd.part))
               ewe_combobox_item_add(combo, part->name);
          }
     }
   else
     {
        EINA_LIST_FOREACH(group_pd.group->parts, l, part)
          {
             if (part != group_pd.part)
               ewe_combobox_item_add(combo, part->name);
          }
     }
}

static void
_part_states_combobox_fill(Evas_Object *combo, const char *part_name, const char *selected, Eina_Bool ignore_value)
{
   Eina_List *l;
   Part *part;
   State *state;
   const char *state_name = NULL;

   assert(combo != NULL);

   if (selected)
     ewe_combobox_text_set(combo, selected);
   else
     ewe_combobox_text_set(combo, STR_NONE);

   if (!part_name)
     {
        elm_object_disabled_set(combo, true);
        return;
     }

   elm_object_disabled_set(combo, false);

   part = pm_resource_unsorted_get(group_pd.group->parts, part_name);

   if (ignore_value)
     {
        /* skipping states with same name but different value */
        EINA_LIST_FOREACH(part->states, l, state)
          {
             if (state_name && !strcmp(state_name, state->parsed_name))
               continue;

             ewe_combobox_item_add(combo, state->parsed_name);
             state_name = state->parsed_name;
          }
     }
   else
     {
        EINA_LIST_FOREACH(part->states, l, state)
           ewe_combobox_item_add(combo, state->name);
     }
}

static void
_color_class_select(Evas_Object *combo, const char *selected)
{
   int cc_val[12];
   Evas_Object *color;

   assert(combo != NULL);

   if (selected)
     {
        ewe_combobox_text_set(combo, selected);
        edje_edit_color_class_colors_get(EDIT_OBJ, selected,
                                         &cc_val[0], &cc_val[1], &cc_val[2], &cc_val[3],
                                         &cc_val[4], &cc_val[5], &cc_val[6], &cc_val[7],
                                         &cc_val[8], &cc_val[9], &cc_val[10], &cc_val[11]);
        evas_color_argb_premul(cc_val[3], &cc_val[0], &cc_val[1], &cc_val[2]);
        evas_color_argb_premul(cc_val[7], &cc_val[4], &cc_val[5], &cc_val[6]);
        evas_color_argb_premul(cc_val[11], &cc_val[8], &cc_val[9], &cc_val[10]);

        color = evas_object_data_get(combo, "color1");
        evas_object_color_set(color, cc_val[0], cc_val[1], cc_val[2], cc_val[3]);

        color = evas_object_data_get(combo, "color2");
        evas_object_color_set(color, cc_val[4], cc_val[5], cc_val[6], cc_val[7]);

        color = evas_object_data_get(combo, "color3");
        evas_object_color_set(color, cc_val[8], cc_val[9], cc_val[10], cc_val[11]);
     }
   else
     {
        ewe_combobox_text_set(combo, STR_NONE);

        color = evas_object_data_get(combo, "color1");
        evas_object_color_set(color, 0, 0, 0, 0);

        color = evas_object_data_get(combo, "color2");
        evas_object_color_set(color, 0, 0, 0, 0);

        color = evas_object_data_get(combo, "color3");
        evas_object_color_set(color, 0, 0, 0, 0);
     }
}

static void
_color_classes_combobox_fill(Evas_Object *combo, const char *selected)
{
   Eina_List *cclist, *l;
   Ewe_Combobox_Item *it;
   Eina_Stringshare *color_class;

   assert(combo != NULL);

   _color_class_select(combo, selected);

   cclist = edje_edit_color_classes_list_get(EDIT_OBJ);
   ewe_combobox_item_add(combo, STR_NONE);
   EINA_LIST_FOREACH(cclist, l, color_class)
     {
        it = ewe_combobox_item_add(combo, color_class);
        ewe_combobox_item_style_set(combo, it, "color_class");
     }

   edje_edit_string_list_free(cclist);
}

static void
_update_cb(Property_Attribute *pa, Property_Action *action)
{
   int int_val1, int_val2, int_val3, int_val4;
   double double_val1;
   unsigned short ushort_val1;
   Eina_Bool bool_val1;
   Eina_Stringshare *str_val1, *str_val2;
   Eina_List *images_list, *l;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);

   PROPERTY_DATA_GET(action->control);

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
         property_entry_set(action->control, group_pd.group->name);
         break;
      case ATTRIBUTE_STATE_NAME:
         property_entry_set(action->control, group_pd.part->current_state->name);
         break;
      case ATTRIBUTE_PROGRAM_NAME:
         property_entry_set(action->control, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_NAME:
         property_entry_set(action->control, group_pd.part->current_item_name);
         break;
      case ATTRIBUTE_STATE_IMAGE:
         str_val1 = edje_edit_state_image_get(EDIT_OBJ, STATE_ARGS);
         if (!strcmp(str_val1, EFLETE_DUMMY_IMAGE_NAME))
           {
              edje_edit_string_free(str_val1);
              str_val1 = eina_stringshare_add(_("None"));
           }
         property_entry_set(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_TWEEN:
         images_list = edje_edit_state_tweens_list_get(EDIT_OBJ, STATE_ARGS);
         property_image_tween_cleanup(action->control);
         EINA_LIST_FOREACH(images_list, l, str_val1)
           {
              property_image_tween_append(action->control, eina_stringshare_add(str_val1));
           }
         edje_edit_string_list_free(images_list);
         break;
      case ATTRIBUTE_GROUP_MIN_W:
         int_val1 = edje_edit_group_min_w_get(EDIT_OBJ);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_GROUP_MIN_H:
         int_val1 = edje_edit_group_min_h_get(EDIT_OBJ);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_GROUP_MAX_W:
         int_val1 = edje_edit_group_max_w_get(EDIT_OBJ);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_GROUP_MAX_H:
         int_val1 = edje_edit_group_max_h_get(EDIT_OBJ);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_NAME:
         property_entry_set(action->control, PART_ARGS);
         break;
      case ATTRIBUTE_PART_TYPE:
         elm_layout_text_set(action->control, NULL, gm_part_type_text_get(group_pd.part->type));
         break;
      case ATTRIBUTE_PROGRAM_ACTION:
         elm_layout_text_set(action->control, NULL, action_type[group_pd.action_type]);
         break;
      case ATTRIBUTE_PART_SCALE:
         bool_val1 = edje_edit_part_scale_get(EDIT_OBJ, PART_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_PART_MOUSE_EVENTS:
         bool_val1 = edje_edit_part_mouse_events_get(EDIT_OBJ, PART_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_PART_REPEAT_EVENTS:
         bool_val1 = edje_edit_part_repeat_events_get(EDIT_OBJ, PART_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_PART_CLIP_TO:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_clip_to_get(EDIT_OBJ, PART_ARGS);
         _parts_combobox_fill(action->control, str_val1, PART_RECTANGLE | PART_IMAGE);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_PROXY_SOURCE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_proxy_source_get(EDIT_OBJ, STATE_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_IGNORE_FLAGS:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_ignore_flags_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_POINTER_MODE:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_pointer_mode_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_SELECT_MODE:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_select_mode_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_ENTRY_MODE:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_entry_mode_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_CURSOR_MODE:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_cursor_mode_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_STATE_FILL_TYPE:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_state_fill_type_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_PART_TEXT_EFFECT:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_text_effect_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_TEXT_SHADOW_DIRECTION:
         /* shodow directions are shifted by 4 in enum */
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_text_shadow_direction_get(EDIT_OBJ, PART_ARGS)>>4);
         break;
      case ATTRIBUTE_PART_DRAG_X:
         int_val1 = edje_edit_part_drag_x_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_Y:
         int_val1 = edje_edit_part_drag_y_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_X:
         int_val1 = edje_edit_part_drag_step_x_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_Y:
         int_val1 = edje_edit_part_drag_step_y_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_X:
         int_val1 = edje_edit_part_drag_count_x_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
         int_val1 = edje_edit_part_drag_count_y_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_CONFINE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_drag_confine_get(EDIT_OBJ, PART_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_drag_threshold_get(EDIT_OBJ, PART_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_DRAG_EVENT:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_drag_event_get(EDIT_OBJ, PART_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_GROUP_SOURCE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_source_get(EDIT_OBJ, PART_ARGS);
         _groups_combobox_fill(action->control, str_val1, true);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_UNDER:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_textblock_selection_under_get(EDIT_OBJ, PART_ARGS);
         _groups_combobox_fill(action->control, str_val1, true);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_UNDER:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_textblock_cursor_under_get(EDIT_OBJ, PART_ARGS);
         _groups_combobox_fill(action->control, str_val1, true);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_UNDER:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_textblock_anchors_under_get(EDIT_OBJ, PART_ARGS);
         _groups_combobox_fill(action->control, str_val1, true);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_OVER:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_textblock_selection_over_get(EDIT_OBJ, PART_ARGS);
         _groups_combobox_fill(action->control, str_val1, true);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_OVER:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_textblock_cursor_over_get(EDIT_OBJ, PART_ARGS);
         _groups_combobox_fill(action->control, str_val1, true);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_OVER:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_textblock_anchors_over_get(EDIT_OBJ, PART_ARGS);
         _groups_combobox_fill(action->control, str_val1, true);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_MULTILINE:
         bool_val1 = edje_edit_part_multiline_get(EDIT_OBJ, PART_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;

      case ATTRIBUTE_STATE_CONTAINER_ALIGN_X:
         double_val1 = edje_edit_state_container_align_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_Y:
         double_val1 = edje_edit_state_container_align_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_CONTAINER_PADING_X:
         int_val1 = edje_edit_state_container_padding_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_CONTAINER_PADING_Y:
         int_val1 = edje_edit_state_container_padding_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_CONTAINER_MIN_V:
         bool_val1 = edje_edit_state_container_min_v_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_CONTAINER_MIN_H:
         bool_val1 = edje_edit_state_container_min_h_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_TABLE_HOMOGENEOUS:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_state_table_homogeneous_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_PART_ITEM_SOURCE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_item_source_get(EDIT_OBJ, ITEM_ARGS);
         _groups_combobox_fill(action->control, str_val1, false);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_ITEM_MIN_W:
         int_val1 = edje_edit_part_item_min_w_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_MIN_H:
         int_val1 = edje_edit_part_item_min_h_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_MAX_W:
         int_val1 = edje_edit_part_item_max_w_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_MAX_H:
         int_val1 = edje_edit_part_item_max_h_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_PREFER_W:
         int_val1 = edje_edit_part_item_prefer_w_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_PREFER_H:
         int_val1 = edje_edit_part_item_prefer_h_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_WEIGHT_X:
         double_val1 = edje_edit_part_item_weight_x_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PART_ITEM_WEIGHT_Y:
         double_val1 = edje_edit_part_item_weight_y_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PART_ITEM_ALIGN_X:
         double_val1 = edje_edit_part_item_align_x_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PART_ITEM_ALIGN_Y:
         double_val1 = edje_edit_part_item_align_y_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_MODE:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_item_aspect_mode_get(EDIT_OBJ, ITEM_ARGS));
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_TYPE:
         ewe_combobox_select_item_set(action->control,
           (int) editor_program_transition_type_get(EDIT_OBJ, PROGRAM_ARGS));
         GENLIST_FILTER_APPLY(pd->genlist);
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_W:
         int_val1 = edje_edit_part_item_aspect_w_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_H:
         int_val1 = edje_edit_part_item_aspect_h_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_SPREAD_W:
         int_val1 = edje_edit_part_item_spread_w_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_SPREAD_H:
         int_val1 = edje_edit_part_item_spread_h_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_SPAN_COL:
         ushort_val1 = edje_edit_part_item_span_col_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, ushort_val1);
         break;
      case ATTRIBUTE_PART_ITEM_SPAN_ROW:
         ushort_val1 = edje_edit_part_item_span_row_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, ushort_val1);
         break;
      case ATTRIBUTE_PART_ITEM_POSITION_COL:
         ushort_val1 = edje_edit_part_item_position_col_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, ushort_val1);
         break;
      case ATTRIBUTE_PART_ITEM_POSITION_ROW:
         ushort_val1 = edje_edit_part_item_position_row_get(EDIT_OBJ, ITEM_ARGS);
         elm_spinner_value_set(action->control, ushort_val1);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_LEFT:
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, &int_val1, NULL, NULL, NULL);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_RIGHT:
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, &int_val1, NULL, NULL);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_TOP:
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, NULL, &int_val1, NULL);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_BOTTOM:
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, NULL, NULL, &int_val1);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_SOURCE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_text_source_get(EDIT_OBJ, STATE_ARGS);
         _parts_combobox_fill(action->control, str_val1, PART_TEXT);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_TEXT_SOURCE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_text_text_source_get(EDIT_OBJ, STATE_ARGS);
         _parts_combobox_fill(action->control, str_val1, PART_TEXT);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_STYLE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_text_style_get(EDIT_OBJ, STATE_ARGS);
         _styles_combobox_fill(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_VISIBLE:
         bool_val1 = edje_edit_state_visible_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_FILL_SMOOTH:
         bool_val1 = edje_edit_state_fill_smooth_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_MIN_W:
         int_val1 = edje_edit_state_min_w_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_MIN_H:
         int_val1 = edje_edit_state_min_h_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_MINMUL_W:
         int_val1 = edje_edit_state_minmul_w_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_MINMUL_H:
         int_val1 = edje_edit_state_minmul_h_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_FIXED_W:
         bool_val1 = edje_edit_state_fixed_w_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_FIXED_H:
         bool_val1 = edje_edit_state_fixed_h_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_MAX_W:
         int_val1 = edje_edit_state_max_w_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_MAX_H:
         int_val1 = edje_edit_state_max_h_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_ASPECT_MIN:
         double_val1 = edje_edit_state_aspect_min_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_ASPECT_MAX:
         double_val1 = edje_edit_state_aspect_max_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_ASPECT_PREF:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_state_aspect_pref_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_ALIGN_X:
         double_val1 = edje_edit_state_align_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_ALIGN_Y:
         double_val1 = edje_edit_state_align_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_REL1_TO_X:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_rel1_to_x_get(EDIT_OBJ, STATE_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_REL1_TO_Y:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_rel1_to_y_get(EDIT_OBJ, STATE_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_REL1_RELATIVE_X:
         double_val1 = edje_edit_state_rel1_relative_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_REL1_RELATIVE_Y:
         double_val1 = edje_edit_state_rel1_relative_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_REL1_OFFSET_X:
         double_val1 = edje_edit_state_rel1_offset_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_REL1_OFFSET_Y:
         double_val1 = edje_edit_state_rel1_offset_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_REL2_TO_X:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_rel2_to_x_get(EDIT_OBJ, STATE_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_REL2_TO_Y:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_rel2_to_y_get(EDIT_OBJ, STATE_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_REL2_RELATIVE_X:
         double_val1 = edje_edit_state_rel2_relative_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_REL2_RELATIVE_Y:
         double_val1 = edje_edit_state_rel2_relative_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_REL2_OFFSET_X:
         double_val1 = edje_edit_state_rel2_offset_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_REL2_OFFSET_Y:
         double_val1 = edje_edit_state_rel2_offset_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_LEFT:
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, &int_val1, NULL, NULL, NULL);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT:
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, &int_val1, NULL, NULL);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_TOP:
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, NULL, &int_val1, NULL);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM:
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, NULL, NULL, &int_val1);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_FILL:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_state_image_border_fill_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X:
         double_val1 = edje_edit_state_fill_origin_relative_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y:
         double_val1 = edje_edit_state_fill_origin_relative_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X:
         double_val1 = edje_edit_state_fill_size_relative_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y:
         double_val1 = edje_edit_state_fill_size_relative_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X:
         double_val1 = edje_edit_state_fill_origin_offset_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y:
         double_val1 = edje_edit_state_fill_origin_offset_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X:
         double_val1 = edje_edit_state_fill_size_offset_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y:
         double_val1 = edje_edit_state_fill_size_offset_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_COLOR:
         edje_edit_state_color_get(EDIT_OBJ, STATE_ARGS, &int_val1, &int_val2, &int_val3, &int_val4);
         property_color_control_color_set(action->control, int_val1, int_val2, int_val3, int_val4);
         break;
      case ATTRIBUTE_STATE_OUTLINE_COLOR:
         edje_edit_state_outline_color_get(EDIT_OBJ, STATE_ARGS, &int_val1, &int_val2, &int_val3, &int_val4);
         property_color_control_color_set(action->control, int_val1, int_val2, int_val3, int_val4);
         break;
      case ATTRIBUTE_STATE_SHADOW_COLOR:
         edje_edit_state_shadow_color_get(EDIT_OBJ, STATE_ARGS, &int_val1, &int_val2, &int_val3, &int_val4);
         property_color_control_color_set(action->control, int_val1, int_val2, int_val3, int_val4);
         break;
      case ATTRIBUTE_STATE_COLOR_CLASS:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_state_color_class_get(EDIT_OBJ, STATE_ARGS);
         _color_classes_combobox_fill(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_TEXT:
         str_val1 = edje_edit_state_text_get(EDIT_OBJ, STATE_ARGS);
         property_entry_set(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_FONT:
         str_val1 = edje_edit_state_font_get(EDIT_OBJ, STATE_ARGS);
         property_entry_set(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_MIN_X:
         bool_val1 = edje_edit_state_text_min_x_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_MIN_Y:
         bool_val1 = edje_edit_state_text_min_y_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_MAX_X:
         bool_val1 = edje_edit_state_text_max_x_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_MAX_Y:
         bool_val1 = edje_edit_state_text_max_y_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_ALIGN_X:
         double_val1 = edje_edit_state_text_align_x_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_ALIGN_Y:
         double_val1 = edje_edit_state_text_align_y_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_FIT_X:
         bool_val1 = edje_edit_state_text_fit_x_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_FIT_Y:
         bool_val1 = edje_edit_state_text_fit_y_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_SIZE:
         int_val1 = edje_edit_state_text_size_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_STATE_TEXT_ELIPSIS:
         double_val1 = edje_edit_state_text_elipsis_get(EDIT_OBJ, STATE_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         elm_object_disabled_set(action->control, double_val1 < 0);
         elm_check_state_set(pa->action1.control, double_val1 >= 0);
         break;
      case ATTRIBUTE_STATE_TEXT_ELIPSIS_ENABLE:
         break;
      case ATTRIBUTE_PROGRAM_SIGNAL:
         str_val1 = edje_edit_program_signal_get(EDIT_OBJ, PROGRAM_ARGS);
         property_entry_set(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PROGRAM_EMIT_SIGNAL:
         str_val1 = edje_edit_program_emit_signal_get(EDIT_OBJ, PROGRAM_ARGS);
         property_entry_set(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PROGRAM_EMIT_SOURCE:
         str_val1 = edje_edit_program_emit_source_get(EDIT_OBJ, PROGRAM_ARGS);
         property_entry_set(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PROGRAM_STATE:
         str_val1 = edje_edit_program_state_get(EDIT_OBJ, PROGRAM_ARGS);
         property_entry_set(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PROGRAM_SOURCE:
         str_val1 = edje_edit_program_source_get(EDIT_OBJ, PROGRAM_ARGS);
         property_entry_set(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PROGRAM_IN_FROM:
         double_val1 = edje_edit_program_in_from_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_IN_RANGE:
         double_val1 = edje_edit_program_in_range_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_TIME:
         double_val1 = edje_edit_program_transition_time_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_FACTOR:
         double_val1 = edje_edit_program_transition_factor_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_GRADIENT:
         double_val1 = edje_edit_program_transition_gradient_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_DECAY:
         double_val1 = edje_edit_program_transition_decay_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BOUNCES:
         double_val1 = edje_edit_program_transition_bounces_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_SWINGS:
         double_val1 = edje_edit_program_transition_swings_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X1:
         double_val1 = edje_edit_program_transition_bezier_x1_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y1:
         double_val1 = edje_edit_program_transition_bezier_y1_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X2:
         double_val1 = edje_edit_program_transition_bezier_x2_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y2:
         double_val1 = edje_edit_program_transition_bezier_y2_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_X:
         double_val1 = edje_edit_program_drag_value_x_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_Y:
         double_val1 = edje_edit_program_drag_value_y_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_VALUE:
         double_val1 = edje_edit_program_value_get(EDIT_OBJ, PROGRAM_ARGS);
         elm_spinner_value_set(action->control, double_val1);
         break;
      case ATTRIBUTE_PROGRAM_FILTER_PART:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_program_filter_part_get(EDIT_OBJ, PROGRAM_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         property_item_update(&group_pd.items[PROPERTY_GROUP_ITEM_PROGRAM_FILTER_STATE]);
         break;
      case ATTRIBUTE_PROGRAM_FILTER_STATE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_program_filter_state_get(EDIT_OBJ, PROGRAM_ARGS);
         str_val2 = edje_edit_program_filter_part_get(EDIT_OBJ, PROGRAM_ARGS);
         _part_states_combobox_fill(action->control, str_val2, str_val1, true);
         edje_edit_string_free(str_val1);
         edje_edit_string_free(str_val2);
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("update callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
}

static void
_start_cb(Property_Attribute *pa, Property_Action *action)
{
   int r, g, b, a, int_val1;
   Eina_Stringshare *tmp_str_val1;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);
   assert(group_pd.history.change == NULL);

   group_pd.history.change = change_add(NULL);

   /* setting new and old to current value. this will be checked on stop to ignore
      empty changes (i.e. from 0 to 0) */
#define VAL(NAME) \
 group_pd.history.new.NAME = group_pd.history.old.NAME
#define STR_VAL(NAME, VAL) \
 group_pd.history.new.NAME = VAL; \
 group_pd.history.old.NAME = eina_stringshare_add(group_pd.history.new.NAME);

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
         group_pd.history.format = _("group name changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(group_pd.group->name));
         break;
      case ATTRIBUTE_STATE_NAME:
         group_pd.history.format = _("state name changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(group_pd.part->current_state->name));
         break;
      case ATTRIBUTE_PROGRAM_NAME:
         group_pd.history.format = _("program name changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(PROGRAM_ARGS));
         break;
      case ATTRIBUTE_PART_ITEM_NAME:
         group_pd.history.format = _("part item's name changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(PROGRAM_ARGS));
         break;
      case ATTRIBUTE_STATE_IMAGE:
         group_pd.history.format = _("image changed from \"%s\" to \"%s\"");
         tmp_str_val1 = edje_edit_state_image_get(EDIT_OBJ, STATE_ARGS);
         if (!strcmp(tmp_str_val1, EFLETE_DUMMY_IMAGE_NAME))
           {
              edje_edit_string_free(tmp_str_val1);
              tmp_str_val1 = eina_stringshare_add(_("None"));
           }
         STR_VAL(str_val1, tmp_str_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_TWEEN:
         group_pd.history.format = _("changed \"%d\" tween images");
         break;
      case ATTRIBUTE_GROUP_MIN_W:
         group_pd.history.format = _("group.min_w changed from %d to %d");
         VAL(int_val1) = edje_edit_group_min_w_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MIN_H:
         group_pd.history.format = _("group.min_h changed from %d to %d");
         VAL(int_val1) = edje_edit_group_min_h_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MAX_W:
         group_pd.history.format = _("group.max_w changed from %d to %d");
         VAL(int_val1) = edje_edit_group_max_w_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MAX_H:
         group_pd.history.format = _("group.max_h changed from %d to %d");
         VAL(int_val1) = edje_edit_group_max_h_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_PART_NAME:
         group_pd.history.format = _("part name changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(PART_ARGS));
         break;
      case ATTRIBUTE_PART_TYPE:
         /* part type can't be changed */
         break;
      case ATTRIBUTE_PROGRAM_ACTION:
         /* program action can't be changed */
         break;
      case ATTRIBUTE_PART_SCALE:
         group_pd.history.format = _("part scale %s");
         break;
      case ATTRIBUTE_PART_MOUSE_EVENTS:
         group_pd.history.format = _("mouse events %s");
         break;
      case ATTRIBUTE_PART_REPEAT_EVENTS:
         group_pd.history.format = _("repeat events %s");
         break;
      case ATTRIBUTE_PART_CLIP_TO:
         group_pd.history.format = _("clip to changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_clip_to_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_STATE_PROXY_SOURCE:
         group_pd.history.format = _("proxy source changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_proxy_source_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_PART_IGNORE_FLAGS:
         group_pd.history.format = _("ignore_flags changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            ignore_flags_strings[edje_edit_part_ignore_flags_get(EDIT_OBJ, PART_ARGS)]));
         break;
      case ATTRIBUTE_PART_POINTER_MODE:
         group_pd.history.format = _("pointer mode changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            pointer_mode_strings[edje_edit_part_pointer_mode_get(EDIT_OBJ, PART_ARGS)]));
         break;
      case ATTRIBUTE_PART_SELECT_MODE:
         group_pd.history.format = _("select mode changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            select_mode_strings[edje_edit_part_select_mode_get(EDIT_OBJ, PART_ARGS)]));
         break;
      case ATTRIBUTE_PART_ENTRY_MODE:
         group_pd.history.format = _("entry mode changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            entry_mode_strings[edje_edit_part_entry_mode_get(EDIT_OBJ, PART_ARGS)]));
         break;
      case ATTRIBUTE_PART_CURSOR_MODE:
         group_pd.history.format = _("cursor mode changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            cursor_mode_strings[edje_edit_part_cursor_mode_get(EDIT_OBJ, PART_ARGS)]));
         break;
      case ATTRIBUTE_STATE_FILL_TYPE:
         group_pd.history.format = _("fill type changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            fill_type_strings[edje_edit_state_fill_type_get(EDIT_OBJ, STATE_ARGS)]));
         break;
      case ATTRIBUTE_PART_TEXT_EFFECT:
         group_pd.history.format = _("text effect changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            text_effect_strings[edje_edit_part_text_effect_get(EDIT_OBJ, PART_ARGS)]));
         break;
      case ATTRIBUTE_PART_TEXT_SHADOW_DIRECTION:
         group_pd.history.format = _("shadow direction changed from \"%s\" to \"%s\"");
         /* shodow directions are shifted by 4 in enum */
         STR_VAL(str_val1, eina_stringshare_add(
            text_shadow_direction_strings[edje_edit_part_text_shadow_direction_get(EDIT_OBJ, PART_ARGS)>>4]));
         break;
      case ATTRIBUTE_PART_DRAG_X:
         group_pd.history.format = _("dragable enable x changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_Y:
         group_pd.history.format = _("dragable enable y changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_X:
         group_pd.history.format = _("dragable step x changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_step_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_Y:
         group_pd.history.format = _("dragable step y changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_step_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_X:
         group_pd.history.format = _("dragable count x changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_count_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
         group_pd.history.format = _("dragable count y changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_count_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_CONFINE:
         group_pd.history.format = _("dragable confine changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_drag_confine_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
         group_pd.history.format = _("dragable threshold changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_drag_threshold_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_DRAG_EVENT:
         group_pd.history.format = _("dragable event changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_drag_event_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_GROUP_SOURCE:
         group_pd.history.format = _("source changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_source_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_UNDER:
         group_pd.history.format = _("selection under changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_textblock_selection_under_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_UNDER:
         group_pd.history.format = _("cursor under changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_textblock_cursor_under_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_UNDER:
         group_pd.history.format = _("anchors under changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_textblock_anchors_under_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_OVER:
         group_pd.history.format = _("selection over changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_textblock_selection_over_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_OVER:
         group_pd.history.format = _("cursor over changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_textblock_cursor_over_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_OVER:
         group_pd.history.format = _("anchors over changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_textblock_anchors_over_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_STATE_TEXT_SOURCE:
         group_pd.history.format = _("text source changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_text_source_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_TEXT_TEXT_SOURCE:
         group_pd.history.format = _("text text_source changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_text_text_source_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_TEXT_STYLE:
         group_pd.history.format = _("text style changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_text_style_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_PART_MULTILINE:
         group_pd.history.format = _("multiline %s");
         break;
      case ATTRIBUTE_STATE_VISIBLE:
         group_pd.history.format = _("state visible %s");
         break;
      case ATTRIBUTE_STATE_FILL_SMOOTH:
         group_pd.history.format = _("fill smooth %s");
         break;
      case ATTRIBUTE_STATE_MIN_W:
         group_pd.history.format = _("state min_w changed from %d to %d");
         VAL(int_val1) = edje_edit_state_min_w_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_MIN_H:
         group_pd.history.format = _("state min_h changed from %d to %d");
         VAL(int_val1) = edje_edit_state_min_h_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_MINMUL_W:
         group_pd.history.format = _("state minmul_w changed from %d to %d");
         VAL(int_val1) = edje_edit_state_minmul_w_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_MINMUL_H:
         group_pd.history.format = _("state minmul_h changed from %d to %d");
         VAL(int_val1) = edje_edit_state_minmul_h_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FIXED_W:
         group_pd.history.format = _("state fixed_w %s");
         break;
      case ATTRIBUTE_STATE_FIXED_H:
         group_pd.history.format = _("state fixed_h %s");
         break;
      case ATTRIBUTE_STATE_MAX_W:
         group_pd.history.format = _("state max_w changed from %d to %d");
         VAL(int_val1) = edje_edit_state_max_w_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_MAX_H:
         group_pd.history.format = _("state max_h changed from %d to %d");
         VAL(int_val1) = edje_edit_state_max_h_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_ASPECT_MIN:
         group_pd.history.format = _("state aspect_min changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_aspect_min_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_ASPECT_MAX:
         group_pd.history.format = _("state aspect_max changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_aspect_max_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_ASPECT_PREF:
         group_pd.history.format = _("aspect preference changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            aspect_preference_strings[edje_edit_state_aspect_pref_get(EDIT_OBJ, STATE_ARGS)]));
         break;
      case ATTRIBUTE_STATE_ALIGN_X:
         group_pd.history.format = _("align x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_align_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_ALIGN_Y:
         group_pd.history.format = _("align y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_align_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL1_TO_X:
         group_pd.history.format = _("rel1 to_x changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_rel1_to_x_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_REL1_TO_Y:
         group_pd.history.format = _("rel1 to_y changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_rel1_to_y_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_REL1_RELATIVE_X:
         group_pd.history.format = _("rel1 relative x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_rel1_relative_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL1_RELATIVE_Y:
         group_pd.history.format = _("rel1 relative y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_rel1_relative_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL1_OFFSET_X:
         group_pd.history.format = _("rel1 offset x changed from %d to %d");
         VAL(int_val1) = edje_edit_state_rel1_offset_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL1_OFFSET_Y:
         group_pd.history.format = _("rel1 offset y changed from %d to %d");
         VAL(int_val1) = edje_edit_state_rel1_offset_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL2_TO_X:
         group_pd.history.format = _("rel2 to_x changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_rel2_to_x_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_REL2_TO_Y:
         group_pd.history.format = _("rel2 to_y changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_rel2_to_y_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_REL2_RELATIVE_X:
         group_pd.history.format = _("rel2 relative x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_rel2_relative_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL2_RELATIVE_Y:
         group_pd.history.format = _("rel2 relative y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_rel2_relative_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL2_OFFSET_X:
         group_pd.history.format = _("rel2 offset x changed from %d to %d");
         VAL(int_val1) = edje_edit_state_rel2_offset_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL2_OFFSET_Y:
         group_pd.history.format = _("rel2 offset y changed from %d to %d");
         VAL(int_val1) = edje_edit_state_rel2_offset_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_LEFT:
         group_pd.history.format = _("image left border changed from %d to %d");
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, &int_val1, NULL, NULL, NULL);
         VAL(int_val1) = int_val1;
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT:
         group_pd.history.format = _("image right border changed from %d to %d");
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, &int_val1, NULL, NULL);
         VAL(int_val1) = int_val1;
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_TOP:
         group_pd.history.format = _("image top border changed from %d to %d");
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, NULL, &int_val1, NULL);
         VAL(int_val1) = int_val1;
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM:
         group_pd.history.format = _("image bottom border changed from %d to %d");
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, NULL, NULL, &int_val1);
         VAL(int_val1) = int_val1;
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_FILL:
         group_pd.history.format = _("border fill changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            image_border_fill_strings[edje_edit_state_image_border_fill_get(EDIT_OBJ, STATE_ARGS)]));
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X:
         group_pd.history.format = _("fill origin relative x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_fill_origin_relative_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y:
         group_pd.history.format = _("fill origin relative y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_fill_origin_relative_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X:
         group_pd.history.format = _("fill size relative x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_fill_size_relative_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y:
         group_pd.history.format = _("fill size relative y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_fill_size_relative_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X:
         group_pd.history.format = _("fill origin offset x changed from %.2f to %.2f");
         VAL(int_val1) = edje_edit_state_fill_origin_offset_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y:
         group_pd.history.format = _("fill origin offset y changed from %.2f to %.2f");
         VAL(int_val1) = edje_edit_state_fill_origin_offset_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X:
         group_pd.history.format = _("fill size offset x changed from %.2f to %.2f");
         VAL(int_val1) = edje_edit_state_fill_size_offset_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y:
         group_pd.history.format = _("fill size offset y changed from %.2f to %.2f");
         VAL(int_val1) = edje_edit_state_fill_size_offset_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_COLOR_CLASS:
         group_pd.history.format = _("color class changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_color_class_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_COLOR:
         group_pd.history.format = _("color changed from [%d,%d,%d,%d] to [%d,%d,%d,%d]");
         edje_edit_state_color_get(EDIT_OBJ, STATE_ARGS, &r, &g, &b, &a);
         VAL(int_val1) = r;
         VAL(int_val2) = g;
         VAL(int_val3) = b;
         VAL(int_val4) = a;
         break;
      case ATTRIBUTE_STATE_OUTLINE_COLOR:
         group_pd.history.format = _("outline color changed from [%d,%d,%d,%d] to [%d,%d,%d,%d]");
         edje_edit_state_outline_color_get(EDIT_OBJ, STATE_ARGS, &r, &g, &b, &a);
         VAL(int_val1) = r;
         VAL(int_val2) = g;
         VAL(int_val3) = b;
         VAL(int_val4) = a;
         break;
      case ATTRIBUTE_STATE_SHADOW_COLOR:
         group_pd.history.format = _("shadow color changed from [%d,%d,%d,%d] to [%d,%d,%d,%d]");
         edje_edit_state_shadow_color_get(EDIT_OBJ, STATE_ARGS, &r, &g, &b, &a);
         VAL(int_val1) = r;
         VAL(int_val2) = g;
         VAL(int_val3) = b;
         VAL(int_val4) = a;
         break;
      case ATTRIBUTE_STATE_TEXT:
         group_pd.history.format = _("text changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_text_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_FONT:
         group_pd.history.format = _("font changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_state_font_get(EDIT_OBJ, STATE_ARGS));
         break;
      case ATTRIBUTE_STATE_TEXT_MIN_X:
         group_pd.history.format = _("text min_x %s");
         break;
      case ATTRIBUTE_STATE_TEXT_MIN_Y:
         group_pd.history.format = _("text min_y %s");
         break;
      case ATTRIBUTE_STATE_TEXT_MAX_X:
         group_pd.history.format = _("text max_x %s");
         break;
      case ATTRIBUTE_STATE_TEXT_MAX_Y:
         group_pd.history.format = _("text max_y %s");
         break;
      case ATTRIBUTE_STATE_TEXT_FIT_X:
         group_pd.history.format = _("text fit_x %s");
         break;
      case ATTRIBUTE_STATE_TEXT_FIT_Y:
         group_pd.history.format = _("text fit_y %s");
         break;
      case ATTRIBUTE_STATE_TEXT_ALIGN_X:
         group_pd.history.format = _("text align x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_text_align_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_TEXT_ALIGN_Y:
         group_pd.history.format = _("text align y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_text_align_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_TEXT_SIZE:
         group_pd.history.format = _("text size changed from %d to %d");
         VAL(int_val1) = edje_edit_state_text_size_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_TEXT_ELIPSIS:
         group_pd.history.format = _("text elipsis changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_text_elipsis_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_TEXT_ELIPSIS_ENABLE:
         group_pd.history.format = _("text elipsis %s");
         break;
      case ATTRIBUTE_PROGRAM_SIGNAL:
         group_pd.history.format = _("program signal changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_program_signal_get(EDIT_OBJ, PROGRAM_ARGS));
         break;
      case ATTRIBUTE_PROGRAM_EMIT_SIGNAL:
         group_pd.history.format = _("program emit signal changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_program_emit_signal_get(EDIT_OBJ, PROGRAM_ARGS));
         break;
      case ATTRIBUTE_PROGRAM_EMIT_SOURCE:
         group_pd.history.format = _("program emit source changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_program_emit_source_get(EDIT_OBJ, PROGRAM_ARGS));
         break;
      case ATTRIBUTE_PROGRAM_STATE:
         group_pd.history.format = _("program state changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_program_state_get(EDIT_OBJ, PROGRAM_ARGS));
         break;
      case ATTRIBUTE_PROGRAM_SOURCE:
         group_pd.history.format = _("program source changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_program_source_get(EDIT_OBJ, PROGRAM_ARGS));
         break;
      case ATTRIBUTE_PROGRAM_IN_FROM:
         group_pd.history.format = _("program in.from changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_in_from_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_IN_RANGE:
         group_pd.history.format = _("program in.range changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_in_range_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_TIME:
         group_pd.history.format = _("program's transition time changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_time_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_FACTOR:
         group_pd.history.format = _("program's transition factor changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_factor_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_GRADIENT:
         group_pd.history.format = _("program's transition gradient changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_gradient_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_DECAY:
         group_pd.history.format = _("program's transition decay changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_decay_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BOUNCES:
         group_pd.history.format = _("program's transition bounces changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_bounces_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_SWINGS:
         group_pd.history.format = _("program's transition swings changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_swings_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X1:
         group_pd.history.format = _("program's transition bezier_x1 changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_bezier_x1_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y1:
         group_pd.history.format = _("program's transition bezier_y1 changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_bezier_y1_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X2:
         group_pd.history.format = _("program's transition bezier_x2 changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_bezier_x2_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y2:
         group_pd.history.format = _("program's transition bezier_y2 changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_transition_bezier_y2_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_X:
         group_pd.history.format = _("program drag value x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_drag_value_x_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_Y:
         group_pd.history.format = _("program drag value y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_drag_value_y_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_VALUE:
         group_pd.history.format = _("program value changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_program_value_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_FILTER_PART:
         group_pd.history.format = _("filter part changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_program_filter_part_get(EDIT_OBJ, PROGRAM_ARGS));
         break;
      case ATTRIBUTE_PROGRAM_FILTER_STATE:
         group_pd.history.format = _("filter state changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_program_filter_state_get(EDIT_OBJ, PROGRAM_ARGS));
         break;
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_X:
         group_pd.history.format = _("container's align_x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_container_align_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_Y:
         group_pd.history.format = _("container's align_y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_state_container_align_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_CONTAINER_PADING_X:
         group_pd.history.format = _("container's padding_x changed from %d to %d");
         VAL(int_val1) = edje_edit_state_container_padding_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_CONTAINER_PADING_Y:
         group_pd.history.format = _("container's padding_y changed from %d to %d");
         VAL(int_val1) = edje_edit_state_container_padding_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_CONTAINER_MIN_V:
         group_pd.history.format = _("container's min_x %s");
         break;
      case ATTRIBUTE_STATE_CONTAINER_MIN_H:
         group_pd.history.format = _("container's min_y %s");
         break;
      case ATTRIBUTE_STATE_TABLE_HOMOGENEOUS:
         group_pd.history.format = _("table homogeneous changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            table_homogeneous_strings[edje_edit_state_table_homogeneous_get(EDIT_OBJ, STATE_ARGS)]));
         break;

      case ATTRIBUTE_PART_ITEM_SOURCE:
         group_pd.history.format = _("item's source changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_item_source_get(EDIT_OBJ, ITEM_ARGS));
         break;
      case ATTRIBUTE_PART_ITEM_MIN_W:
         group_pd.history.format = _("part item's min_w changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_min_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_MIN_H:
         group_pd.history.format = _("part item's min_h changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_min_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_MAX_W:
         group_pd.history.format = _("part item's max_w changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_max_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_MAX_H:
         group_pd.history.format = _("part item's max_h changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_max_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_PREFER_W:
         group_pd.history.format = _("part item's prefer_w changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_prefer_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_PREFER_H:
         group_pd.history.format = _("part item's prefer_h changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_prefer_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_WEIGHT_X:
         group_pd.history.format = _("part item's weight_x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_part_item_weight_x_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_WEIGHT_Y:
         group_pd.history.format = _("part item's weight_y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_part_item_weight_y_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_ALIGN_X:
         group_pd.history.format = _("part item's align_x changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_part_item_align_x_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_ALIGN_Y:
         group_pd.history.format = _("part item's align_y changed from %.2f to %.2f");
         VAL(double_val1) = edje_edit_part_item_align_y_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_MODE:
         group_pd.history.format = _("part item's aspect mode changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            item_aspect_mode_strings[edje_edit_part_item_aspect_mode_get(EDIT_OBJ, ITEM_ARGS)]));
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_TYPE:
         group_pd.history.format = _("program's transition type changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            transition_type_strings[editor_program_transition_type_get(EDIT_OBJ, PROGRAM_ARGS)]));
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_W:
         group_pd.history.format = _("part item's aspect_w changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_aspect_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_H:
         group_pd.history.format = _("part item's aspect_h changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_aspect_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_SPREAD_W:
         group_pd.history.format = _("part item's spread by rows changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_spread_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_SPREAD_H:
         group_pd.history.format = _("part item's spread by columns changed from %d to %d");
         VAL(int_val1) = edje_edit_part_item_spread_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_SPAN_COL:
         group_pd.history.format = _("part item's span by columns changed from %hu to %hu");
         VAL(ushort_val1) = edje_edit_part_item_span_col_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_SPAN_ROW:
         group_pd.history.format = _("part item's span by rows changed from %hu to %hu");
         VAL(ushort_val1) = edje_edit_part_item_span_row_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_POSITION_COL:
         group_pd.history.format = _("part item's column position changed from %hu to %hu");
         VAL(ushort_val1) = edje_edit_part_item_position_col_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_POSITION_ROW:
         group_pd.history.format = _("part item's row position changed from %hu to %hu");
         VAL(ushort_val1) = edje_edit_part_item_position_row_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_LEFT:
         group_pd.history.format = _("part item's left padding changed from %d to %d");
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, &int_val1, NULL, NULL, NULL);
         VAL(int_val1) = int_val1;
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_RIGHT:
         group_pd.history.format = _("part item's right padding changed from %d to %d");
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, &int_val1, NULL, NULL);
         VAL(int_val1) = int_val1;
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_TOP:
         group_pd.history.format = _("part item's top padding changed from %d to %d");
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, NULL, &int_val1, NULL);
         VAL(int_val1) = int_val1;
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_BOTTOM:
         group_pd.history.format = _("part item's bottom padding changed from %d to %d");
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, NULL, NULL, &int_val1);
         VAL(int_val1) = int_val1;
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("start callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
#undef VAL
}

static void
_change_cb(Property_Attribute *pa, Property_Action *action)
{
   double double_val1 = 0.0;
   int r, g, b, a;
   Eina_Stringshare *str_val1 = NULL;
   Eina_Bool bool_val1 = false;;
   Ewe_Combobox_Item *cb_item = NULL;

   Eina_List *deleted_tweens = NULL, *l;
   Eina_List *added_tweens = NULL;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);
   assert(group_pd.history.change != NULL);

   switch (action->control_type)
     {
      case PROPERTY_CONTROL_SPINNER:
         double_val1 = elm_spinner_value_get(action->control);
         break;
      case PROPERTY_CONTROL_ENTRY:
      case PROPERTY_CONTROL_IMAGE_NORMAL:
         str_val1 = property_entry_get(action->control);
         break;
      case PROPERTY_CONTROL_CHECK:
         bool_val1 = elm_check_state_get(action->control);
         break;
      case PROPERTY_CONTROL_COMBOBOX:
         TODO("change this after migrating to elm_combobox");
         cb_item = ewe_combobox_select_item_get(action->control);
         break;
      case PROPERTY_CONTROL_COLOR:
         property_color_control_color_get(action->control, &r, &g, &b, &a);
         break;
      case PROPERTY_CONTROL_IMAGE_TWEEN:
         deleted_tweens = property_image_tween_deleted_list_get();
         added_tweens = property_image_tween_added_list_get();
         break;
      default:
         break;
     }

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
         TODO("implement group rename");
         break;
      case ATTRIBUTE_STATE_NAME:
         TODO("implement state rename");
         break;
      case ATTRIBUTE_PROGRAM_NAME:
         TODO("implement program rename");
         break;
      case ATTRIBUTE_PART_ITEM_NAME:
         TODO("implement part's item rename");
         break;
      case ATTRIBUTE_STATE_IMAGE:
         CRIT_ON_FAIL(editor_state_image_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_IMAGE_TWEEN:
         EINA_LIST_FOREACH(added_tweens, l, str_val1)
           {
              CRIT_ON_FAIL(editor_state_tween_add(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
           }
         EINA_LIST_FOREACH(deleted_tweens, l, str_val1)
           {
              CRIT_ON_FAIL(editor_state_tween_del(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
           }
         group_pd.history.new.int_val1 = eina_list_count(added_tweens) +
            eina_list_count(deleted_tweens);
         property_image_tween_lists_free();
         break;
      case ATTRIBUTE_GROUP_MIN_W:
         CRIT_ON_FAIL(editor_group_min_w_set(EDIT_OBJ, CHANGE_MERGE, double_val1));
         group_pd.history.new.int_val1 = edje_edit_group_min_w_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MIN_H:
         CRIT_ON_FAIL(editor_group_min_h_set(EDIT_OBJ, CHANGE_MERGE, double_val1));
         group_pd.history.new.int_val1 = edje_edit_group_min_h_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MAX_W:
         CRIT_ON_FAIL(editor_group_max_w_set(EDIT_OBJ, CHANGE_MERGE, double_val1));
         group_pd.history.new.int_val1 = edje_edit_group_max_w_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MAX_H:
         CRIT_ON_FAIL(editor_group_max_h_set(EDIT_OBJ, CHANGE_MERGE, double_val1));
         group_pd.history.new.int_val1 = edje_edit_group_max_h_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_PART_NAME:
         CRIT_ON_FAIL(editor_part_name_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TYPE:
         /* part type can't be changed */
         break;
      case ATTRIBUTE_PROGRAM_ACTION:
         /* program action can't be changed */
         break;
      case ATTRIBUTE_PART_SCALE:
         CRIT_ON_FAIL(editor_part_scale_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_PART_MOUSE_EVENTS:
         CRIT_ON_FAIL(editor_part_mouse_events_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_PART_REPEAT_EVENTS:
         CRIT_ON_FAIL(editor_part_repeat_events_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_PART_CLIP_TO:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_clip_to_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_PROXY_SOURCE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_state_proxy_source_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_IGNORE_FLAGS:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_part_ignore_flags_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_POINTER_MODE:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_part_pointer_mode_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_SELECT_MODE:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_part_select_mode_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_ENTRY_MODE:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_part_entry_mode_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_CURSOR_MODE:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_part_cursor_mode_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_FILL_TYPE:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_state_fill_type_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TEXT_EFFECT:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_part_text_effect_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TEXT_SHADOW_DIRECTION:
         str_val1 = eina_stringshare_add(cb_item->title);
         /* shodow directions are shifted by 4 in enum */
         CRIT_ON_FAIL(editor_part_text_shadow_direction_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, cb_item->index << 4));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_DRAG_X:
         CRIT_ON_FAIL(editor_part_drag_x_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_drag_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_Y:
         CRIT_ON_FAIL(editor_part_drag_y_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_drag_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_X:
         CRIT_ON_FAIL(editor_part_drag_step_x_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_drag_step_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_Y:
         CRIT_ON_FAIL(editor_part_drag_step_y_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_drag_step_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_X:
         CRIT_ON_FAIL(editor_part_drag_count_x_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_drag_count_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
         CRIT_ON_FAIL(editor_part_drag_count_y_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_drag_count_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_CONFINE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_drag_confine_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_drag_threshold_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_DRAG_EVENT:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_drag_event_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_GROUP_SOURCE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_group_source_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_UNDER:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_textblock_selection_under_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_UNDER:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_textblock_cursor_under_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_UNDER:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_textblock_anchors_under_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_OVER:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_textblock_selection_over_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_OVER:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_textblock_cursor_over_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_OVER:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_textblock_anchors_over_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_SOURCE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_state_text_source_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_TEXT_SOURCE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_state_text_text_source_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_STYLE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_state_text_style_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_MULTILINE:
         CRIT_ON_FAIL(editor_part_multiline_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_VISIBLE:
         CRIT_ON_FAIL(editor_state_visible_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_FILL_SMOOTH:
         CRIT_ON_FAIL(editor_state_fill_smooth_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_MIN_W:
         CRIT_ON_FAIL(editor_state_min_w_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_min_w_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_MIN_H:
         CRIT_ON_FAIL(editor_state_min_h_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_min_h_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_MINMUL_W:
         CRIT_ON_FAIL(editor_state_minmul_w_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_minmul_w_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_MINMUL_H:
         CRIT_ON_FAIL(editor_state_minmul_h_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_minmul_h_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FIXED_W:
         CRIT_ON_FAIL(editor_state_fixed_w_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_FIXED_H:
         CRIT_ON_FAIL(editor_state_fixed_h_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_MAX_W:
         CRIT_ON_FAIL(editor_state_max_w_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_max_w_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_MAX_H:
         CRIT_ON_FAIL(editor_state_max_h_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_max_h_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_ASPECT_MIN:
         CRIT_ON_FAIL(editor_state_aspect_min_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_aspect_min_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_ASPECT_MAX:
         CRIT_ON_FAIL(editor_state_aspect_max_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_aspect_max_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_ASPECT_PREF:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_state_aspect_pref_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_ALIGN_X:
         CRIT_ON_FAIL(editor_state_align_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_align_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_ALIGN_Y:
         CRIT_ON_FAIL(editor_state_align_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_align_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL1_TO_X:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_state_rel1_to_x_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_REL1_TO_Y:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_state_rel1_to_y_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_REL1_RELATIVE_X:
         CRIT_ON_FAIL(editor_state_rel1_relative_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_rel1_relative_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL1_RELATIVE_Y:
         CRIT_ON_FAIL(editor_state_rel1_relative_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_rel1_relative_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL1_OFFSET_X:
         CRIT_ON_FAIL(editor_state_rel1_offset_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_rel1_offset_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL1_OFFSET_Y:
         CRIT_ON_FAIL(editor_state_rel1_offset_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_rel1_offset_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL2_TO_X:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_state_rel2_to_x_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_REL2_TO_Y:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_state_rel2_to_y_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_REL2_RELATIVE_X:
         CRIT_ON_FAIL(editor_state_rel2_relative_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_rel2_relative_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL2_RELATIVE_Y:
         CRIT_ON_FAIL(editor_state_rel2_relative_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_rel2_relative_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL2_OFFSET_X:
         CRIT_ON_FAIL(editor_state_rel2_offset_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_rel2_offset_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_REL2_OFFSET_Y:
         CRIT_ON_FAIL(editor_state_rel2_offset_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_rel2_offset_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_LEFT:
         CRIT_ON_FAIL(editor_state_image_border_left_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, &group_pd.history.new.int_val1, NULL, NULL, NULL);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT:
         CRIT_ON_FAIL(editor_state_image_border_right_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, &group_pd.history.new.int_val1, NULL, NULL);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_TOP:
         CRIT_ON_FAIL(editor_state_image_border_top_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, NULL, &group_pd.history.new.int_val1, NULL);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM:
         CRIT_ON_FAIL(editor_state_image_border_bottom_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         edje_edit_state_image_border_get(EDIT_OBJ, STATE_ARGS, NULL, NULL, NULL, &group_pd.history.new.int_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_BORDER_FILL:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_state_image_border_fill_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;

      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X:
         CRIT_ON_FAIL(editor_state_fill_origin_relative_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_fill_origin_relative_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y:
         CRIT_ON_FAIL(editor_state_fill_origin_relative_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_fill_origin_relative_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X:
         CRIT_ON_FAIL(editor_state_fill_size_relative_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_fill_size_relative_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y:
         CRIT_ON_FAIL(editor_state_fill_size_relative_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_fill_size_relative_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X:
         CRIT_ON_FAIL(editor_state_fill_origin_offset_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_fill_origin_offset_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y:
         CRIT_ON_FAIL(editor_state_fill_origin_offset_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_fill_origin_offset_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X:
         CRIT_ON_FAIL(editor_state_fill_size_offset_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_fill_size_offset_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y:
         CRIT_ON_FAIL(editor_state_fill_size_offset_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_fill_size_offset_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_COLOR_CLASS:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         _color_class_select(action->control, str_val1);
         CRIT_ON_FAIL(editor_state_color_class_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_COLOR:
         CRIT_ON_FAIL(editor_state_color_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, r, g, b, a));
         edje_edit_state_color_get(EDIT_OBJ, STATE_ARGS, &r, &g, &b, &a);
         group_pd.history.new.int_val1 = r;
         group_pd.history.new.int_val2 = g;
         group_pd.history.new.int_val3 = b;
         group_pd.history.new.int_val4 = a;
         break;
      case ATTRIBUTE_STATE_OUTLINE_COLOR:
         CRIT_ON_FAIL(editor_state_outline_color_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, r, g, b, a));
         edje_edit_state_outline_color_get(EDIT_OBJ, STATE_ARGS, &r, &g, &b, &a);
         group_pd.history.new.int_val1 = r;
         group_pd.history.new.int_val2 = g;
         group_pd.history.new.int_val3 = b;
         group_pd.history.new.int_val4 = a;
         break;
      case ATTRIBUTE_STATE_SHADOW_COLOR:
         CRIT_ON_FAIL(editor_state_shadow_color_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, r, g, b, a));
         edje_edit_state_shadow_color_get(EDIT_OBJ, STATE_ARGS, &r, &g, &b, &a);
         group_pd.history.new.int_val1 = r;
         group_pd.history.new.int_val2 = g;
         group_pd.history.new.int_val3 = b;
         group_pd.history.new.int_val4 = a;
         break;
      case ATTRIBUTE_STATE_TEXT:
         CRIT_ON_FAIL(editor_state_text_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_FONT:
         CRIT_ON_FAIL(editor_state_font_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_MIN_X:
         CRIT_ON_FAIL(editor_state_text_min_x_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_MIN_Y:
         CRIT_ON_FAIL(editor_state_text_min_y_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_MAX_X:
         CRIT_ON_FAIL(editor_state_text_max_x_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_MAX_Y:
         CRIT_ON_FAIL(editor_state_text_max_y_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_FIT_X:
         CRIT_ON_FAIL(editor_state_text_fit_x_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_FIT_Y:
         CRIT_ON_FAIL(editor_state_text_fit_y_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_TEXT_ALIGN_X:
         CRIT_ON_FAIL(editor_state_text_align_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_text_align_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_TEXT_ALIGN_Y:
         CRIT_ON_FAIL(editor_state_text_align_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_text_align_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_TEXT_SIZE:
         CRIT_ON_FAIL(editor_state_text_size_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_text_size_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_TEXT_ELIPSIS:
         CRIT_ON_FAIL(editor_state_text_elipsis_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_text_elipsis_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_TEXT_ELIPSIS_ENABLE:
         CRIT_ON_FAIL(editor_state_text_elipsis_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, bool_val1 ? 0.0 : -1.0));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_PROGRAM_SIGNAL:
         CRIT_ON_FAIL(editor_program_signal_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PROGRAM_EMIT_SIGNAL:
         CRIT_ON_FAIL(editor_program_emit_signal_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PROGRAM_EMIT_SOURCE:
         CRIT_ON_FAIL(editor_program_emit_source_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PROGRAM_STATE:
         CRIT_ON_FAIL(editor_program_state_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PROGRAM_SOURCE:
         CRIT_ON_FAIL(editor_program_source_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PROGRAM_IN_FROM:
         CRIT_ON_FAIL(editor_program_in_from_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_in_from_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_IN_RANGE:
         CRIT_ON_FAIL(editor_program_in_range_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_in_range_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_TIME:
         CRIT_ON_FAIL(editor_program_transition_time_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_time_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_FACTOR:
         CRIT_ON_FAIL(editor_program_transition_factor_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_factor_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_GRADIENT:
         CRIT_ON_FAIL(editor_program_transition_gradient_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_gradient_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_DECAY:
         CRIT_ON_FAIL(editor_program_transition_decay_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_decay_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BOUNCES:
         CRIT_ON_FAIL(editor_program_transition_bounces_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_bounces_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_SWINGS:
         CRIT_ON_FAIL(editor_program_transition_swings_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_swings_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X1:
         CRIT_ON_FAIL(editor_program_transition_bezier_x1_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_bezier_x1_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y1:
         CRIT_ON_FAIL(editor_program_transition_bezier_y1_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_bezier_y1_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X2:
         CRIT_ON_FAIL(editor_program_transition_bezier_x2_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_bezier_x2_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y2:
         CRIT_ON_FAIL(editor_program_transition_bezier_y2_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_transition_bezier_y2_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_X:
         CRIT_ON_FAIL(editor_program_drag_value_x_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_drag_value_x_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_Y:
         CRIT_ON_FAIL(editor_program_drag_value_y_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_drag_value_y_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_VALUE:
         CRIT_ON_FAIL(editor_program_value_set(EDIT_OBJ, CHANGE_MERGE, PROGRAM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_program_value_get(EDIT_OBJ, PROGRAM_ARGS);
         break;
      case ATTRIBUTE_PROGRAM_FILTER_PART:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_program_filter_part_set(EDIT_OBJ, CHANGE_NO_MERGE, PROGRAM_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         property_item_update(&group_pd.items[PROPERTY_GROUP_ITEM_PROGRAM_FILTER_STATE]);
         break;
      case ATTRIBUTE_PROGRAM_FILTER_STATE:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_program_filter_state_set(EDIT_OBJ, CHANGE_NO_MERGE, PROGRAM_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_X:
         CRIT_ON_FAIL(editor_state_container_align_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_container_align_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_Y:
         CRIT_ON_FAIL(editor_state_container_align_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_state_container_align_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_CONTAINER_PADING_X:
         CRIT_ON_FAIL(editor_state_container_padding_x_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_container_padding_x_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_CONTAINER_PADING_Y:
         CRIT_ON_FAIL(editor_state_container_padding_y_set(EDIT_OBJ, CHANGE_MERGE, STATE_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_state_container_padding_y_get(EDIT_OBJ, STATE_ARGS);
         break;
      case ATTRIBUTE_STATE_CONTAINER_MIN_V:
         CRIT_ON_FAIL(editor_state_container_min_v_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_CONTAINER_MIN_H:
         CRIT_ON_FAIL(editor_state_container_min_h_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1));
         group_pd.history.new.bool_val1 = bool_val1;
         break;
      case ATTRIBUTE_STATE_TABLE_HOMOGENEOUS:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_state_table_homogeneous_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;

      case ATTRIBUTE_PART_ITEM_SOURCE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         CRIT_ON_FAIL(editor_part_item_source_set(EDIT_OBJ, CHANGE_NO_MERGE, ITEM_ARGS, str_val1));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_ITEM_MIN_W:
         CRIT_ON_FAIL(editor_part_item_min_w_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_min_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_MIN_H:
         CRIT_ON_FAIL(editor_part_item_min_h_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_min_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_MAX_W:
         CRIT_ON_FAIL(editor_part_item_max_w_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_max_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_MAX_H:
         CRIT_ON_FAIL(editor_part_item_max_h_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_max_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_PREFER_W:
         CRIT_ON_FAIL(editor_part_item_prefer_w_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_prefer_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_PREFER_H:
         CRIT_ON_FAIL(editor_part_item_prefer_h_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_prefer_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_WEIGHT_X:
         CRIT_ON_FAIL(editor_part_item_weight_x_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_part_item_weight_x_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_WEIGHT_Y:
         CRIT_ON_FAIL(editor_part_item_weight_y_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_part_item_weight_y_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_ALIGN_X:
         CRIT_ON_FAIL(editor_part_item_align_x_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_part_item_align_x_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_ALIGN_Y:
         CRIT_ON_FAIL(editor_part_item_align_y_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.double_val1 = edje_edit_part_item_align_y_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_MODE:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_part_item_aspect_mode_set(EDIT_OBJ, CHANGE_NO_MERGE, ITEM_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PROGRAM_TRANSITION_TYPE:
         str_val1 = eina_stringshare_add(cb_item->title);
         CRIT_ON_FAIL(editor_program_transition_type_set(EDIT_OBJ, CHANGE_NO_MERGE, PROGRAM_ARGS, cb_item->index));
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_W:
         CRIT_ON_FAIL(editor_part_item_aspect_w_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_aspect_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_ASPECT_H:
         CRIT_ON_FAIL(editor_part_item_aspect_h_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_aspect_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_SPREAD_W:
         CRIT_ON_FAIL(editor_part_item_spread_w_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_spread_w_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_SPREAD_H:
         CRIT_ON_FAIL(editor_part_item_spread_h_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         group_pd.history.new.int_val1 = edje_edit_part_item_spread_h_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_SPAN_COL:
         CRIT_ON_FAIL(editor_part_item_span_col_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, (int)double_val1));
         group_pd.history.new.ushort_val1 = edje_edit_part_item_span_col_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_SPAN_ROW:
         CRIT_ON_FAIL(editor_part_item_span_row_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, (int)double_val1));
         group_pd.history.new.ushort_val1 = edje_edit_part_item_span_row_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_POSITION_COL:
         CRIT_ON_FAIL(editor_part_item_position_col_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, (int)double_val1));
         group_pd.history.new.ushort_val1 = edje_edit_part_item_position_col_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_POSITION_ROW:
         CRIT_ON_FAIL(editor_part_item_position_row_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, (int)double_val1));
         group_pd.history.new.ushort_val1 = edje_edit_part_item_position_row_get(EDIT_OBJ, ITEM_ARGS);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_TOP:
         CRIT_ON_FAIL(editor_part_item_padding_top_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, NULL, &group_pd.history.new.int_val1, NULL);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_BOTTOM:
         CRIT_ON_FAIL(editor_part_item_padding_bottom_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, NULL, NULL, &group_pd.history.new.int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_LEFT:
         CRIT_ON_FAIL(editor_part_item_padding_left_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, &group_pd.history.new.int_val1, NULL, NULL, NULL);
         break;
      case ATTRIBUTE_PART_ITEM_PADDING_RIGHT:
         CRIT_ON_FAIL(editor_part_item_padding_right_set(EDIT_OBJ, CHANGE_MERGE, ITEM_ARGS, double_val1));
         edje_edit_part_item_padding_get(EDIT_OBJ, ITEM_ARGS, NULL, &group_pd.history.new.int_val1, NULL, NULL);
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("change callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
}

static void
_stop_cb(Property_Attribute *pa, Property_Action *action)
{
   Eina_Stringshare *msg = NULL;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);
   assert(group_pd.history.change != NULL);

#define CHECK_VAL(NAME) \
 if (group_pd.history.new.NAME == group_pd.history.old.NAME) \
   { \
     change_free(group_pd.history.change); \
     goto clean; \
   }

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
      case ATTRIBUTE_PART_NAME:
      case ATTRIBUTE_STATE_NAME:
      case ATTRIBUTE_PROGRAM_NAME:
      case ATTRIBUTE_PART_ITEM_NAME:
      case ATTRIBUTE_STATE_IMAGE:
         CHECK_VAL(str_val1);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       group_pd.history.old.str_val1,
                                       group_pd.history.new.str_val1);
         break;
      case ATTRIBUTE_STATE_IMAGE_TWEEN:
         msg = eina_stringshare_printf(group_pd.history.format,
                                       group_pd.history.new.int_val1);
         break;
      case ATTRIBUTE_PART_CLIP_TO:
      case ATTRIBUTE_STATE_PROXY_SOURCE:
      case ATTRIBUTE_PART_IGNORE_FLAGS:
      case ATTRIBUTE_PART_POINTER_MODE:
      case ATTRIBUTE_PART_SELECT_MODE:
      case ATTRIBUTE_PART_ENTRY_MODE:
      case ATTRIBUTE_PART_CURSOR_MODE:
      case ATTRIBUTE_STATE_FILL_TYPE:
      case ATTRIBUTE_PART_TEXT_EFFECT:
      case ATTRIBUTE_PART_TEXT_SHADOW_DIRECTION:
      case ATTRIBUTE_PART_DRAG_CONFINE:
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
      case ATTRIBUTE_PART_DRAG_EVENT:
      case ATTRIBUTE_PART_GROUP_SOURCE:
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_OVER:
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_OVER:
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_OVER:
      case ATTRIBUTE_PART_ITEM_SOURCE:
      case ATTRIBUTE_PART_ITEM_ASPECT_MODE:
      case ATTRIBUTE_STATE_TEXT_SOURCE:
      case ATTRIBUTE_STATE_TEXT_TEXT_SOURCE:
      case ATTRIBUTE_STATE_TEXT_STYLE:
      case ATTRIBUTE_STATE_ASPECT_PREF:
      case ATTRIBUTE_STATE_REL1_TO_X:
      case ATTRIBUTE_STATE_REL1_TO_Y:
      case ATTRIBUTE_STATE_REL2_TO_X:
      case ATTRIBUTE_STATE_REL2_TO_Y:
      case ATTRIBUTE_STATE_IMAGE_BORDER_FILL:
      case ATTRIBUTE_STATE_COLOR_CLASS:
      case ATTRIBUTE_STATE_TEXT:
      case ATTRIBUTE_STATE_FONT:
      case ATTRIBUTE_PROGRAM_SIGNAL:
      case ATTRIBUTE_PROGRAM_EMIT_SIGNAL:
      case ATTRIBUTE_PROGRAM_EMIT_SOURCE:
      case ATTRIBUTE_PROGRAM_STATE:
      case ATTRIBUTE_PROGRAM_SOURCE:
      case ATTRIBUTE_STATE_TABLE_HOMOGENEOUS:
      case ATTRIBUTE_PROGRAM_FILTER_PART:
      case ATTRIBUTE_PROGRAM_FILTER_STATE:
      case ATTRIBUTE_PROGRAM_TRANSITION_TYPE:
         CHECK_VAL(str_val1);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       (group_pd.history.old.str_val1) ? group_pd.history.old.str_val1 : STR_NONE,
                                       (group_pd.history.new.str_val1) ? group_pd.history.new.str_val1 : STR_NONE);
         break;
      case ATTRIBUTE_GROUP_MIN_W:
      case ATTRIBUTE_GROUP_MIN_H:
      case ATTRIBUTE_GROUP_MAX_W:
      case ATTRIBUTE_GROUP_MAX_H:
      case ATTRIBUTE_PART_DRAG_X:
      case ATTRIBUTE_PART_DRAG_Y:
      case ATTRIBUTE_PART_DRAG_STEP_X:
      case ATTRIBUTE_PART_DRAG_STEP_Y:
      case ATTRIBUTE_PART_DRAG_COUNT_X:
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
      case ATTRIBUTE_STATE_MIN_W:
      case ATTRIBUTE_STATE_MIN_H:
      case ATTRIBUTE_STATE_MINMUL_W:
      case ATTRIBUTE_STATE_MINMUL_H:
      case ATTRIBUTE_STATE_MAX_W:
      case ATTRIBUTE_STATE_MAX_H:
      case ATTRIBUTE_STATE_REL1_OFFSET_X:
      case ATTRIBUTE_STATE_REL1_OFFSET_Y:
      case ATTRIBUTE_STATE_REL2_OFFSET_X:
      case ATTRIBUTE_STATE_REL2_OFFSET_Y:
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X:
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y:
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X:
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y:
      case ATTRIBUTE_STATE_TEXT_SIZE:
      case ATTRIBUTE_STATE_CONTAINER_PADING_X:
      case ATTRIBUTE_STATE_CONTAINER_PADING_Y:
      case ATTRIBUTE_PART_ITEM_MIN_W:
      case ATTRIBUTE_PART_ITEM_MIN_H:
      case ATTRIBUTE_PART_ITEM_MAX_W:
      case ATTRIBUTE_PART_ITEM_MAX_H:
      case ATTRIBUTE_PART_ITEM_PREFER_W:
      case ATTRIBUTE_PART_ITEM_PREFER_H:
      case ATTRIBUTE_PART_ITEM_ASPECT_W:
      case ATTRIBUTE_PART_ITEM_ASPECT_H:
      case ATTRIBUTE_PART_ITEM_SPREAD_W:
      case ATTRIBUTE_PART_ITEM_SPREAD_H:
      case ATTRIBUTE_PART_ITEM_PADDING_LEFT:
      case ATTRIBUTE_PART_ITEM_PADDING_RIGHT:
      case ATTRIBUTE_PART_ITEM_PADDING_TOP:
      case ATTRIBUTE_PART_ITEM_PADDING_BOTTOM:
      case ATTRIBUTE_STATE_IMAGE_BORDER_LEFT:
      case ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT:
      case ATTRIBUTE_STATE_IMAGE_BORDER_TOP:
      case ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM:
         CHECK_VAL(int_val1);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       group_pd.history.old.int_val1,
                                       group_pd.history.new.int_val1);
         break;
      case ATTRIBUTE_PART_ITEM_SPAN_COL:
      case ATTRIBUTE_PART_ITEM_SPAN_ROW:
      case ATTRIBUTE_PART_ITEM_POSITION_COL:
      case ATTRIBUTE_PART_ITEM_POSITION_ROW:
         CHECK_VAL(ushort_val1);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       group_pd.history.old.ushort_val1,
                                       group_pd.history.new.ushort_val1);
         break;
      case ATTRIBUTE_STATE_ASPECT_MIN:
      case ATTRIBUTE_STATE_ASPECT_MAX:
      case ATTRIBUTE_STATE_ALIGN_X:
      case ATTRIBUTE_STATE_ALIGN_Y:
      case ATTRIBUTE_STATE_TEXT_ALIGN_X:
      case ATTRIBUTE_STATE_TEXT_ALIGN_Y:
      case ATTRIBUTE_STATE_REL1_RELATIVE_X:
      case ATTRIBUTE_STATE_REL1_RELATIVE_Y:
      case ATTRIBUTE_STATE_REL2_RELATIVE_X:
      case ATTRIBUTE_STATE_REL2_RELATIVE_Y:
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X:
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y:
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X:
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y:
      case ATTRIBUTE_STATE_TEXT_ELIPSIS:
      case ATTRIBUTE_PROGRAM_IN_FROM:
      case ATTRIBUTE_PROGRAM_IN_RANGE:
      case ATTRIBUTE_PROGRAM_TRANSITION_TIME:
      case ATTRIBUTE_PROGRAM_TRANSITION_FACTOR:
      case ATTRIBUTE_PROGRAM_TRANSITION_GRADIENT:
      case ATTRIBUTE_PROGRAM_TRANSITION_DECAY:
      case ATTRIBUTE_PROGRAM_TRANSITION_BOUNCES:
      case ATTRIBUTE_PROGRAM_TRANSITION_SWINGS:
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X1:
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y1:
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X2:
      case ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y2:
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_X:
      case ATTRIBUTE_PROGRAM_DRAG_VALUE_Y:
      case ATTRIBUTE_PROGRAM_VALUE:
      case ATTRIBUTE_PART_ITEM_ALIGN_X:
      case ATTRIBUTE_PART_ITEM_ALIGN_Y:
      case ATTRIBUTE_PART_ITEM_WEIGHT_X:
      case ATTRIBUTE_PART_ITEM_WEIGHT_Y:
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_X:
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_Y:
         if (fabs(group_pd.history.new.double_val1 - group_pd.history.old.double_val1) < DBL_EPSILON)
           {
              change_free(group_pd.history.change);
              goto clean;
           }
         msg = eina_stringshare_printf(group_pd.history.format,
                                       group_pd.history.old.double_val1,
                                       group_pd.history.new.double_val1);
         break;
      case ATTRIBUTE_STATE_COLOR:
      case ATTRIBUTE_STATE_OUTLINE_COLOR:
      case ATTRIBUTE_STATE_SHADOW_COLOR:
         if ((group_pd.history.new.int_val1 == group_pd.history.old.int_val1) &&
             (group_pd.history.new.int_val2 == group_pd.history.old.int_val2) &&
             (group_pd.history.new.int_val3 == group_pd.history.old.int_val3) &&
             (group_pd.history.new.int_val4 == group_pd.history.old.int_val4))
           {
              change_free(group_pd.history.change);
              goto clean;
           }
         msg = eina_stringshare_printf(group_pd.history.format,
                                       group_pd.history.old.int_val1,
                                       group_pd.history.old.int_val2,
                                       group_pd.history.old.int_val3,
                                       group_pd.history.old.int_val4,
                                       group_pd.history.new.int_val1,
                                       group_pd.history.new.int_val2,
                                       group_pd.history.new.int_val3,
                                       group_pd.history.new.int_val4);
         break;
      case ATTRIBUTE_PART_TYPE:
         /* part type can't be changed */
         break;
      case ATTRIBUTE_PROGRAM_ACTION:
         /* program action can't be changed */
         break;
      case ATTRIBUTE_PART_SCALE:
      case ATTRIBUTE_PART_MOUSE_EVENTS:
      case ATTRIBUTE_PART_REPEAT_EVENTS:
      case ATTRIBUTE_PART_MULTILINE:
      case ATTRIBUTE_STATE_VISIBLE:
      case ATTRIBUTE_STATE_FILL_SMOOTH:
      case ATTRIBUTE_STATE_FIXED_W:
      case ATTRIBUTE_STATE_FIXED_H:
      case ATTRIBUTE_STATE_TEXT_MIN_X:
      case ATTRIBUTE_STATE_TEXT_MIN_Y:
      case ATTRIBUTE_STATE_TEXT_MAX_X:
      case ATTRIBUTE_STATE_TEXT_MAX_Y:
      case ATTRIBUTE_STATE_TEXT_FIT_X:
      case ATTRIBUTE_STATE_TEXT_FIT_Y:
      case ATTRIBUTE_STATE_TEXT_ELIPSIS_ENABLE:
      case ATTRIBUTE_STATE_CONTAINER_MIN_V:
      case ATTRIBUTE_STATE_CONTAINER_MIN_H:
         msg = eina_stringshare_printf(group_pd.history.format,
                                       (group_pd.history.new.bool_val1) ?
                                       _("turned on") : _("turned off"));
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("stop callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
   change_description_set(group_pd.history.change, msg);
   history_change_add(group_pd.group->history, group_pd.history.change);
   eina_stringshare_del(msg);
#undef CHECK_VAL
clean:
   eina_stringshare_del(group_pd.history.new.str_val1);
   group_pd.history.new.str_val1 = NULL;
   eina_stringshare_del(group_pd.history.old.str_val1);
   group_pd.history.old.str_val1 = NULL;
   group_pd.history.change = NULL;
}

/* blocks */
static inline void
_action_internal(Property_Action *action, const char *name, const char *units,
                 Property_Control control_type, Attribute attribute)
{
   /* check that we haven't inited this action already */
   assert(action->update_cb == NULL);

   action->name = name;
   action->units = units;
   action->control_type = control_type;
   action->type.attribute = attribute;
   action->update_cb = _update_cb;
   action->init_cb = _init_cb;
   action->start_cb = _start_cb;
   action->stop_cb = _stop_cb;
   action->change_cb = _change_cb;
}

static inline void
_action1(Property_Attribute *pa, const char *name, const char *units,
         Property_Control control_type, Attribute attribute)
{
   _action_internal(&pa->action1, name, units, control_type, attribute);

   assert(attribute_map[attribute].pa == NULL);
   assert(attribute_map[attribute].action == NULL);

   attribute_map[attribute].pa = pa;
   attribute_map[attribute].action = &pa->action1;
}

static inline void
_action2(Property_Attribute *pa, const char *name, const char *units,
         Property_Control control_type, Attribute attribute)
{
   _action_internal(&pa->action2, name, units, control_type, attribute);

   assert(attribute_map[attribute].pa == NULL);
   assert(attribute_map[attribute].action == NULL);

   attribute_map[attribute].pa = pa;
   attribute_map[attribute].action = &pa->action2;
}

static void
_init_items()
{
   Property_Group_Item it;
#define IT group_pd.items[it]
   for (it = 0 /* first element of enum */; it < PROPERTY_GROUP_ITEM_LAST; it++)
     {
        IT.type.group_item = it;
        IT.filter_cb = _filter_cb;

        /* default value: show for all types */
        IT.filter_data.part_types = ~0u;

        switch(it)
          {
             /* group block */
           case PROPERTY_GROUP_ITEM_GROUP_TITLE:
              IT.name = "group";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_GROUP_NAME:
              IT.name = "name";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_GROUP_NAME);
              break;
           case PROPERTY_GROUP_ITEM_GROUP_MIN:
              IT.name = "min";
              _action1(&IT, "w", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_GROUP_MIN_W);
              _action2(&IT, "h", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_GROUP_MIN_H);
              break;
           case PROPERTY_GROUP_ITEM_GROUP_MAX:
              IT.name = "max";
              _action1(&IT, "w", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_GROUP_MAX_W);
              _action2(&IT, "h", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_GROUP_MAX_H);
              break;

              /* part block */
           case PROPERTY_GROUP_ITEM_PART_TITLE:
              IT.name = "part";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_PART_NAME:
              IT.name = "name";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PART_NAME);
              break;
           case PROPERTY_GROUP_ITEM_PART_TYPE:
              IT.name = "type";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_LABEL, ATTRIBUTE_PART_TYPE);
              break;
           case PROPERTY_GROUP_ITEM_PART_SCALE:
              IT.name = "scale";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_PART_SCALE);
              break;
           case PROPERTY_GROUP_ITEM_PART_MOUSE_EVENTS:
              IT.name = "mouse events";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_PART_MOUSE_EVENTS);
              break;
           case PROPERTY_GROUP_ITEM_PART_REPEAT_EVENTS:
              IT.name = "repeat events";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_PART_REPEAT_EVENTS);
              break;
           case PROPERTY_GROUP_ITEM_PART_CLIP_TO:
              IT.name = "clip to";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_CLIP_TO);
              break;
           case PROPERTY_GROUP_ITEM_PART_IGNORE_FLAGS:
              IT.name = "ignore flags";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_IGNORE_FLAGS);
              break;
           case PROPERTY_GROUP_ITEM_PART_POINTER_MODE:
              IT.name = "pointer mode";
              IT.filter_data.part_types &= ~PART_SPACER;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_POINTER_MODE);
              break;
           case PROPERTY_GROUP_ITEM_PART_GROUP_SOURCE:
              IT.name = "source";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_GROUP_SOURCE);
              IT.filter_data.part_types = PART_GROUP;
              break;

              /* part.draggable block */
           case PROPERTY_GROUP_ITEM_PART_DRAGABLE_TITLE:
              IT.name = "dragable";
              IT.expandable = true;
              IT.expanded = false;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_PART_DRAGABLE_ENABLE:
              IT.name = "enable";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_Y);
              break;
           case PROPERTY_GROUP_ITEM_PART_DRAGABLE_STEP:
              IT.name = "step";
              _action1(&IT, "x", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_STEP_X);
              _action2(&IT, "y", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_STEP_Y);
              break;
           case PROPERTY_GROUP_ITEM_PART_DRAGABLE_COUNT:
              IT.name = "count";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_COUNT_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_COUNT_Y);
              break;
           case PROPERTY_GROUP_ITEM_PART_DRAGABLE_CONFINE:
              IT.name = "confine";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_DRAG_CONFINE);
              break;
           case PROPERTY_GROUP_ITEM_PART_DRAGABLE_THRESHOLD:
              IT.name = "threshold";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_DRAG_THRESHOLD);
              break;
           case PROPERTY_GROUP_ITEM_PART_DRAGABLE_EVENTS:
              IT.name = "events";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_DRAG_EVENT);
              break;

              /* state block */
           case PROPERTY_GROUP_ITEM_STATE_TITLE:
              IT.name = "state";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_STATE_NAME:
              IT.name = "name";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_STATE_NAME);
              break;
           case PROPERTY_GROUP_ITEM_STATE_VISIBLE:
              IT.name = "visible";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_VISIBLE);
              break;
           case PROPERTY_GROUP_ITEM_STATE_PROXY_SOURCE:
              IT.name = "proxy source";
              IT.filter_data.part_types = PART_PROXY;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_PROXY_SOURCE);
              break;

              /* state size block */
           case PROPERTY_GROUP_ITEM_STATE_SIZE_TITLE:
              IT.name = "size";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_STATE_SIZE_MIN:
              IT.name = "min";
              _action1(&IT, "w", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_MIN_W);
              _action2(&IT, "h", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_MIN_H);
              break;
           case PROPERTY_GROUP_ITEM_STATE_SIZE_MAX:
              IT.name = "max";
              _action1(&IT, "w", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_MAX_W);
              _action2(&IT, "h", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_MAX_H);
              break;
           case PROPERTY_GROUP_ITEM_STATE_SIZE_MINMUL:
              IT.name = "minmul";
              _action1(&IT, "w", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_MINMUL_W);
              _action2(&IT, "h", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_MINMUL_H);
              break;
           case PROPERTY_GROUP_ITEM_STATE_SIZE_FIXED:
              IT.name = "fixed";
              _action1(&IT, "w", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_FIXED_W);
              _action2(&IT, "h", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_FIXED_H);
              break;
           case PROPERTY_GROUP_ITEM_STATE_SIZE_ASPECT_PREF:
              IT.name = "aspect preference";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_ASPECT_PREF);
              break;
           case PROPERTY_GROUP_ITEM_STATE_SIZE_ASPECT:
              IT.name = "aspect";
              _action1(&IT, "min", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_ASPECT_MIN);
              _action2(&IT, "max", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_ASPECT_MAX);
              break;

              /* part text */
           case PROPERTY_GROUP_ITEM_PART_TEXT_EFFECT:
              IT.name = "effect";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_TEXT_EFFECT);
              break;
           case PROPERTY_GROUP_ITEM_PART_TEXT_SHADOW_DIRECTION:
              IT.name = "shadow direction";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_TEXT_SHADOW_DIRECTION);
              break;

              /* part textblock */
           case PROPERTY_GROUP_ITEM_PART_SELECT_MODE:
              IT.name = "select mode";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_SELECT_MODE);
              break;
           case PROPERTY_GROUP_ITEM_PART_ENTRY_MODE:
              IT.name = "entry mode";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_ENTRY_MODE);
              break;
           case PROPERTY_GROUP_ITEM_PART_CURSOR_MODE:
              IT.name = "cursor mode";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_CURSOR_MODE);
              break;
           case PROPERTY_GROUP_ITEM_PART_MULTILINE:
              IT.name = "multiline";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_PART_MULTILINE);
              break;
           case PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_SELECTION_UNDER:
              IT.name = "selection under";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_TEXTBLOCK_SELECTION_UNDER);
              break;
           case PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_SELECTION_OVER:
              IT.name = "selection over";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_TEXTBLOCK_SELECTION_OVER);
              break;
           case PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_CURSOR_UNDER:
              IT.name = "cursor under";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_TEXTBLOCK_CURSOR_UNDER);
              break;
           case PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_CURSOR_OVER:
              IT.name = "cursor over";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_TEXTBLOCK_CURSOR_OVER);
              break;
           case PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_ANCHORS_UNDER:
              IT.name = "anchors under";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_UNDER);
              break;
           case PROPERTY_GROUP_ITEM_PART_TEXTBLOCK_ANCHORS_OVER:
              IT.name = "anchors over";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_OVER);
              break;

              /* state position block */
           case PROPERTY_GROUP_ITEM_STATE_POSITION_TITLE:
              IT.name = "position";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_ALIGN:
              IT.name = "align";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_ALIGN_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_ALIGN_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_TITLE:
              IT.name = "rel1 (start point)";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/start-point"));
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_TO_X:
              IT.name = "relative to";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_REL1_TO_X);
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_TO_Y:
              IT.name = "";
              _action1(&IT, "y", NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_REL1_TO_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_RELATIVE:
              IT.name = "relative";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/align"));
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_REL1_RELATIVE_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_REL1_RELATIVE_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL1_OFFSET:
              IT.name = "offset";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/offset"));
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_REL1_OFFSET_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_REL1_OFFSET_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_TITLE:
              IT.name = "rel2 (end point)";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/end-point"));
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_TO_X:
              IT.name = "relative to";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_REL2_TO_X);
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_TO_Y:
              IT.name = "";
              _action1(&IT, "y", NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_REL2_TO_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_RELATIVE:
              IT.name = "relative";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/align"));
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_REL2_RELATIVE_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_REL2_RELATIVE_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_POSITION_REL2_OFFSET:
              IT.name = "offset";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/offset"));
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_REL2_OFFSET_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_REL2_OFFSET_Y);
              break;

              /* part image */
           case PROPERTY_GROUP_ITEM_STATE_IMAGE_TITLE:
              IT.name = "image";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              IT.filter_data.part_types = PART_IMAGE;
              break;
           case PROPERTY_GROUP_ITEM_STATE_IMAGE_NORMAL:
              IT.name = "image";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_IMAGE_NORMAL, ATTRIBUTE_STATE_IMAGE);
              break;
           case PROPERTY_GROUP_ITEM_STATE_IMAGE_TWEEN:
              IT.name = "tween";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_IMAGE_TWEEN, ATTRIBUTE_STATE_IMAGE_TWEEN);
              break;
           case PROPERTY_GROUP_ITEM_STATE_IMAGE_BORDER_H:
              IT.name = "border";
              _action1(&IT, "left", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_IMAGE_BORDER_LEFT);
              _action2(&IT, "right", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT);
              break;
           case PROPERTY_GROUP_ITEM_STATE_IMAGE_BORDER_V:
              IT.name = "";
              _action1(&IT, "top", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_IMAGE_BORDER_TOP);
              _action2(&IT, "bottom", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM);
              break;
           case PROPERTY_GROUP_ITEM_STATE_IMAGE_MIDDLE:
              IT.name = "border fill";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_IMAGE_BORDER_FILL);
              break;

              /* state fill block */
           case PROPERTY_GROUP_ITEM_STATE_FILL_TITLE:
              IT.name = "fill";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              IT.filter_data.part_types = PART_IMAGE | PART_PROXY;
              break;
           case PROPERTY_GROUP_ITEM_STATE_FILL_TYPE:
              IT.name = "type";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_FILL_TYPE);
              break;
           case PROPERTY_GROUP_ITEM_STATE_FILL_SMOOTH:
              IT.name = "smooth";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_FILL_SMOOTH);
              break;
           case PROPERTY_GROUP_ITEM_STATE_FILL_ORIGIN_TITLE:
              IT.name = "origin";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/start-point"));
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_STATE_FILL_ORIGIN_RELATIVE:
              IT.name = "relative";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/align"));
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_FILL_ORIGIN_OFFSET:
              IT.name = "offset";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/offset"));
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_FILL_SIZE_TITLE:
              IT.name = "size";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/end-point"));
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_STATE_FILL_SIZE_RELATIVE:
              IT.name = "relative";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/align"));
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_FILL_SIZE_OFFSET:
              IT.name = "offset";
              IT.icon_name = eina_stringshare_add(_("elm/image/icon/offset"));
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y);
              break;

              /* state colors block */
           case PROPERTY_GROUP_ITEM_STATE_COLORS_TITLE:
              IT.name = "colors";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              IT.filter_data.part_types &= ~PART_SPACER & ~PART_SWALLOW;
              break;
           case PROPERTY_GROUP_ITEM_STATE_COLORS_COLOR_CLASS:
              IT.name = "color class";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_COLOR_CLASS);
              break;
           case PROPERTY_GROUP_ITEM_STATE_COLORS_COLOR:
              IT.name = "color";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COLOR, ATTRIBUTE_STATE_COLOR);
              break;
           case PROPERTY_GROUP_ITEM_STATE_COLORS_OUTLINE_COLOR:
              IT.name = "outline color";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COLOR, ATTRIBUTE_STATE_OUTLINE_COLOR);
              break;
           case PROPERTY_GROUP_ITEM_STATE_COLORS_SHADOWCOLOR:
              IT.name = "shadow color";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COLOR, ATTRIBUTE_STATE_SHADOW_COLOR);
              break;

              /* state text common block */
           case PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_TITLE:
              IT.name = "text";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              IT.filter_data.part_types = PART_TEXT | PART_TEXTBLOCK;
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_TEXT:
              IT.name = "text";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_STATE_TEXT);
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_ALIGN:
              IT.name = "align";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_TEXT_ALIGN_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_TEXT_ALIGN_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_MIN:
              IT.name = "min";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_TEXT_MIN_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_TEXT_MIN_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_COMMON_MAX:
              IT.name = "max";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_TEXT_MAX_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_TEXT_MAX_Y);
              break;

              /* state text block */
           case PROPERTY_GROUP_ITEM_STATE_TEXT_FONT:
              IT.name = "font";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_STATE_FONT);
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_SIZE:
              IT.name = "size";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_TEXT_SIZE);
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_FIT:
              IT.name = "fit";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_TEXT_FIT_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_TEXT_FIT_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_SOURCE:
              IT.name = "source";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_TEXT_SOURCE);
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_TEXT_SOURCE:
              IT.name = "text source";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_TEXT_TEXT_SOURCE);
              break;
           case PROPERTY_GROUP_ITEM_STATE_TEXT_ELIPSIS:
              IT.name = "elipsis";
              IT.filter_data.part_types = PART_TEXT;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_TEXT_ELIPSIS_ENABLE);
              _action2(&IT, NULL, NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_TEXT_ELIPSIS);
              break;

              /* state textblock block */
           case PROPERTY_GROUP_ITEM_STATE_TEXT_STYLE:
              IT.name = "style";
              IT.filter_data.part_types = PART_TEXTBLOCK;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_TEXT_STYLE);
              break;

              /* part item block */
           case PROPERTY_GROUP_ITEM_STATE_CONTAINER_TITLE:
              IT.name = "container";
              IT.expanded = true;
              IT.expandable = true;
              IT.expand_cb = _subitems_get;
              IT.filter_data.part_types = PART_BOX | PART_TABLE;
              break;
           case PROPERTY_GROUP_ITEM_STATE_CONTAINER_ALIGN:
              IT.name = "align";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_CONTAINER_ALIGN_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_CONTAINER_ALIGN_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_CONTAINER_PADDING:
              IT.name = "padding";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_CONTAINER_PADING_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_STATE_CONTAINER_PADING_Y);
              break;
           case PROPERTY_GROUP_ITEM_STATE_CONTAINER_MIN:
              IT.name = "min";
              _action1(&IT, "h", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_CONTAINER_MIN_H);
              _action2(&IT, "v", NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_CONTAINER_MIN_V);
              break;
           case PROPERTY_GROUP_ITEM_STATE_CONTAINER_HOMOGENEOUS:
              IT.name = "homogeneous mode";
              IT.filter_data.part_types = PART_TABLE;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_STATE_TABLE_HOMOGENEOUS);
              break;

              /* part item block */
           case PROPERTY_GROUP_ITEM_PART_ITEM_TITLE:
              IT.name = "item";
              IT.expanded = true;
              IT.expandable = true;
              IT.expand_cb = _subitems_get;
              IT.filter_data.part_types = PART_BOX | PART_TABLE;
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_NAME:
              IT.name = "name";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PART_ITEM_NAME);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_SOURCE:
              IT.name = "item source";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_ITEM_SOURCE);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_MIN:
              IT.name = "min";
              _action1(&IT, "w", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_MIN_W);
              _action2(&IT, "h", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_MIN_H);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_MAX:
              IT.name = "max";
              _action1(&IT, "w", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_MAX_W);
              _action2(&IT, "h", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_MAX_H);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_PREFER:
              IT.name = "prefer";
              _action1(&IT, "w", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_PREFER_W);
              _action2(&IT, "h", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_PREFER_H);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_ALIGN:
              IT.name = "align";
              _action1(&IT, "x", "%", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_ALIGN_X);
              _action2(&IT, "y", "%", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_ALIGN_Y);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_WEIGHT:
              IT.name = "weight";
              _action1(&IT, "x", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_WEIGHT_X);
              _action2(&IT, "y", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_WEIGHT_Y);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_ASPECT_MODE:
              IT.name = "aspect mode";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_ITEM_ASPECT_MODE);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_ASPECT:
              IT.name = "aspect";
              _action1(&IT, "w", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_ASPECT_W);
              _action2(&IT, "h", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_ASPECT_H);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_SPREAD:
              IT.name = "spread";
              _action1(&IT, "row", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_SPREAD_W);
              _action2(&IT, "column", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_SPREAD_H);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_PADDING_H:
              IT.name = "paddings";
              _action1(&IT, "left", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_PADDING_LEFT);
              _action2(&IT, "right", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_PADDING_RIGHT);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_PADDING_V:
              _action1(&IT, "top", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_PADDING_TOP);
              _action2(&IT, "bottom", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_PADDING_BOTTOM);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_SPAN:
              IT.name = "span";
              IT.filter_data.part_types = PART_TABLE;
              _action1(&IT, "row", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_SPAN_COL);
              _action2(&IT, "column", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_SPAN_ROW);
              break;
           case PROPERTY_GROUP_ITEM_PART_ITEM_POSITION:
              IT.name = "position";
              IT.filter_data.part_types = PART_TABLE;
              _action1(&IT, "row", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_POSITION_COL);
              _action2(&IT, "column", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_ITEM_POSITION_ROW);
              break;

              /* program block */
           case PROPERTY_GROUP_ITEM_PROGRAM_TITLE:
              IT.name = "program";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_NAME:
              IT.name = "name";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PROGRAM_NAME);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION:
              IT.name = "action";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_LABEL, ATTRIBUTE_PROGRAM_ACTION);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_SIGNAL:
              IT.name = "signal";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PROGRAM_SIGNAL);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_SOURCE:
              IT.name = "source";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PROGRAM_SOURCE);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_IN:
              IT.name = "in";
              _action1(&IT, "from", "sec", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_IN_FROM);
              _action2(&IT, "range", "sec", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_IN_RANGE);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_FILTER_PART:
              IT.name = "filter";
              _action1(&IT, "part", NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PROGRAM_FILTER_PART);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_FILTER_STATE:
              IT.name = "";
              _action1(&IT, "state", NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PROGRAM_FILTER_STATE);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TITLE:
              IT.name = "action params";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              IT.filter_data.action_types &= ~ACTION_NONE;
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_STATE:
              IT.name = "state";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PROGRAM_STATE);
              IT.filter_data.action_types = ACTION_STATE_SET;
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_VALUE:
              IT.name = "value";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_VALUE);
              IT.filter_data.action_types = ACTION_STATE_SET;
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_EMIT_SIGNAL:
              IT.name = "signal";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PROGRAM_EMIT_SIGNAL);
              IT.filter_data.action_types = ACTION_SIGNAL_EMIT;
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_EMIT_SOURCE:
              IT.name = "source";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PROGRAM_EMIT_SOURCE);
              IT.filter_data.action_types = ACTION_SIGNAL_EMIT;
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_DRAG_VALUE:
              IT.name = "drag value";
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_DRAG_VALUE_X);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_DRAG_VALUE_Y);
              IT.filter_data.action_types = ACTION_DRAG_VAL_SET | ACTION_DRAG_VAL_STEP | ACTION_DRAG_VAL_PAGE;
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_TITLE:
              IT.name = "transition";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              IT.filter_data.action_types = ACTION_STATE_SET;
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_TYPE:
              IT.name = "type";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PROGRAM_TRANSITION_TYPE);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_TIME:
              IT.name = "time";
              _action1(&IT, NULL, "sec", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_TIME);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_FACTOR:
              IT.name = "factor";
              IT.filter_cb = _transition_filter_cb;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_FACTOR);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_GRADIENT:
              IT.name = "gradient";
              IT.filter_cb = _transition_filter_cb;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_GRADIENT);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_DECAY:
              IT.name = "decay";
              IT.filter_cb = _transition_filter_cb;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_DECAY);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BOUNCES:
              IT.name = "bounces";
              IT.filter_cb = _transition_filter_cb;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_BOUNCES);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_SWINGS:
              IT.name = "swings";
              IT.filter_cb = _transition_filter_cb;
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_SWINGS);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BEZIER_POINT1:
              IT.name = "point 1";
              IT.filter_cb = _transition_filter_cb;
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X1);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y1);
              break;
           case PROPERTY_GROUP_ITEM_PROGRAM_ACTION_TRANSITION_BEZIER_POINT2:
              IT.name = "point 2";
              IT.filter_cb = _transition_filter_cb;
              _action1(&IT, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X2);
              _action2(&IT, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y2);
              break;

           case PROPERTY_GROUP_ITEM_LAST:
              break;
          }
     }
#undef IT
}

/* public */
void
property_group_init(Property_Data *pd)
{
   assert(pd != NULL);

   _init_items();

   /* register global callbacks */
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_CHANGED, _on_group_changed, pd);
   evas_object_smart_callback_add(ap.win, SIGNAL_PART_SELECTED, _on_part_selected, pd);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_NAVIGATOR_UNSELECTED, _on_group_navigator_unselected, pd);
   evas_object_smart_callback_add(ap.win, SIGNAL_PART_STATE_SELECTED, _on_part_state_selected, pd);
   evas_object_smart_callback_add(ap.win, SIGNAL_PROGRAM_SELECTED, _on_program_selected, pd);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, _on_editor_attribute_changed, pd);
}

Eina_List *
property_group_items_get()
{
   Eina_List *items = NULL;

   items = eina_list_append(items, &group_pd.items[PROPERTY_GROUP_ITEM_GROUP_TITLE]);
   items = eina_list_append(items, &group_pd.items[PROPERTY_GROUP_ITEM_PART_TITLE]);
   items = eina_list_append(items, &group_pd.items[PROPERTY_GROUP_ITEM_STATE_TITLE]);
   items = eina_list_append(items, &group_pd.items[PROPERTY_GROUP_ITEM_PART_ITEM_TITLE]);
   items = eina_list_append(items, &group_pd.items[PROPERTY_GROUP_ITEM_PROGRAM_TITLE]);

   return items;
}