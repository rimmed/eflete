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

#include "resource_manager2.h"
#include "resource_manager_private.h"
#include "project_manager.h"
#include "tabs.h"
#include "string_common.h"

/* TEMPORARY FUNCTION WHICH SHOULD BE DELETED AFTER RESOURCE_MANAGER2 IMPLEMENTED */
Group2 *
_get_current_group2(Project *pro)
{
   /*******************************************************/
   /******** THIS BEHAVIOUR SHOULD BE CHANGED *************/
   /*******************************************************/
   /******* BECAUSE GROUP2 DIFFERENT FROM GROUP ***********/
   /******* AND EFLETE DOESN'T WORK WITH IT YET ***********/
   /*******************************************************/
   Group *group = tabs_current_group_get();
   return (Group2 *)resource_manager_find(pro->groups2, group->name);
   /*******************************************************/
   /*******************************************************/
   /*******************************************************/
   /*******************************************************/
   /*******************************************************/
   /*******************************************************/
}

/* CALLBACK FUNCTIONS */

static void
_property_attribute_changed(void *data __UNUSED__,
                            Evas_Object *obj __UNUSED__,
                            void *event_info)
{
   TODO("Update editor and resource manager (read comment below)")
   /***********************************************************************
    ***********************************************************************
    **                          IMPORTANT NOTE:                          **
    ***********************************************************************
    ** This function is still WIP. That means that it will be            **
    ** fully usable when edje-edit will be fully moved to Model.         **
    ** So every edje_edit changes after editor will come in here         **
    ** to update part, state and all other structures.                   **
    **                                                                   **
    ** Right now only few attributes could be usable,                    **
    ** for example those who changes resource usage (color_class,        **
    ** sound, etc).                                                      **
    **                                                                   **
    ** TODO:                                                             **
    ** > edje-edit to model, expand structures to save entire data       **
    ** > expand editor (top blocks like image, sound, etc) not supported **
    ***********************************************************************
    ***********************************************************************/

   Attribute *attr = event_info;
   printf("Some attribute was changed [%d] \n", (int)*attr);
   switch ((int)*attr)
     {
      case ATTRIBUTE_GROUP_MIN_W:
      case ATTRIBUTE_GROUP_MIN_H:
      case ATTRIBUTE_GROUP_MAX_W:
      case ATTRIBUTE_GROUP_MAX_H:
      case ATTRIBUTE_STATE_MIN_W:
      case ATTRIBUTE_STATE_MIN_H:
      case ATTRIBUTE_STATE_MAX_W:
      case ATTRIBUTE_STATE_MAX_H:
      case ATTRIBUTE_STATE_ALIGN_X:
      case ATTRIBUTE_STATE_ALIGN_Y:
      case ATTRIBUTE_STATE_REL1_RELATIVE_X:
      case ATTRIBUTE_STATE_REL1_RELATIVE_Y:
      case ATTRIBUTE_STATE_REL2_RELATIVE_X:
      case ATTRIBUTE_STATE_REL2_RELATIVE_Y:
      case ATTRIBUTE_STATE_REL1_OFFSET_X:
      case ATTRIBUTE_STATE_REL1_OFFSET_Y:
      case ATTRIBUTE_STATE_REL2_OFFSET_X:
      case ATTRIBUTE_STATE_REL2_OFFSET_Y:
      case ATTRIBUTE_STATE_ASPECT_MIN:
      case ATTRIBUTE_STATE_ASPECT_MAX:
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X:
      case ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y:
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X:
      case ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y:
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X:
      case ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y:
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X:
      case ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y:
      case ATTRIBUTE_STATE_TEXT_ALIGN_X:
      case ATTRIBUTE_STATE_TEXT_ALIGN_Y:
      case ATTRIBUTE_STATE_TEXT_ELIPSIS:
      case ATTRIBUTE_STATE_TEXT_SIZE:
      case ATTRIBUTE_STATE_TEXT_FIT_X:
      case ATTRIBUTE_STATE_TEXT_FIT_Y:
      case ATTRIBUTE_STATE_TEXT_MAX_X:
      case ATTRIBUTE_STATE_TEXT_MAX_Y:
      case ATTRIBUTE_STATE_TEXT_MIN_X:
      case ATTRIBUTE_STATE_TEXT_MIN_Y:
      case ATTRIBUTE_STATE_FIXED_H:
      case ATTRIBUTE_STATE_FIXED_W:
      case ATTRIBUTE_STATE_IMAGE:
      case ATTRIBUTE_STATE_IMAGE_TWEEN:
      case ATTRIBUTE_STATE_REL1_TO_X:
      case ATTRIBUTE_STATE_REL1_TO_Y:
      case ATTRIBUTE_STATE_REL2_TO_X:
      case ATTRIBUTE_STATE_REL2_TO_Y:
      case ATTRIBUTE_STATE_TEXT_SOURCE:
      case ATTRIBUTE_STATE_TEXT_TEXT_SOURCE:
      case ATTRIBUTE_STATE_TEXT:
      case ATTRIBUTE_STATE_FONT:
      case ATTRIBUTE_STATE_TEXT_STYLE:
      case ATTRIBUTE_STATE_ASPECT_PREF:
      case ATTRIBUTE_PART_TEXT_EFFECT:
      case ATTRIBUTE_PART_TEXT_SHADOW_DIRECTION:
      case ATTRIBUTE_PART_CLIP_TO:
      case ATTRIBUTE_PART_DRAG_CONFINE:
      case ATTRIBUTE_PART_GROUP_SOURCE:
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_SELECTION_OVER:
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_CURSOR_OVER:
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_UNDER:
      case ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_OVER:
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_X:
      case ATTRIBUTE_STATE_CONTAINER_ALIGN_Y:
      case ATTRIBUTE_STATE_CONTAINER_MIN_H:
      case ATTRIBUTE_STATE_CONTAINER_MIN_V:
      case ATTRIBUTE_STATE_TABLE_HOMOGENEOUS:
      case ATTRIBUTE_STATE_CONTAINER_PADING_X:
      case ATTRIBUTE_STATE_CONTAINER_PADING_Y:
      case ATTRIBUTE_STATE_MINMUL_H:
      case ATTRIBUTE_STATE_MINMUL_W:
      case ATTRIBUTE_PART_MULTILINE:
      case ATTRIBUTE_PART_ENTRY_MODE:
      case ATTRIBUTE_STATE_FILL_TYPE:
      case ATTRIBUTE_STATE_COLOR:
      case ATTRIBUTE_STATE_OUTLINE_COLOR:
      case ATTRIBUTE_STATE_SHADOW_COLOR:
      case ATTRIBUTE_STATE_MAP_ROTATION_CENTER:
      case ATTRIBUTE_STATE_MAP_ROTATION_X:
      case ATTRIBUTE_STATE_MAP_ROTATION_Y:
      case ATTRIBUTE_STATE_MAP_ROTATION_Z:
      case ATTRIBUTE_STATE_MAP_POINT_COLOR_1:
      case ATTRIBUTE_STATE_MAP_POINT_COLOR_2:
      case ATTRIBUTE_STATE_MAP_POINT_COLOR_3:
      case ATTRIBUTE_STATE_MAP_POINT_COLOR_4:
      case ATTRIBUTE_STATE_FILL_SMOOTH:
      case ATTRIBUTE_STATE_VISIBLE:
      case ATTRIBUTE_STATE_IMAGE_BORDER_TOP:
      case ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM:
      case ATTRIBUTE_STATE_IMAGE_BORDER_LEFT:
      case ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT:
      case ATTRIBUTE_STATE_IMAGE_BORDER_FILL:
      case ATTRIBUTE_STATE_COLOR_CLASS:
      case ATTRIBUTE_STATE_MAP_ON:
      case ATTRIBUTE_STATE_MAP_PERSPECTIVE_ON:
      case ATTRIBUTE_STATE_MAP_PERSPECTIVE:
      case ATTRIBUTE_STATE_MAP_LIGHT:
      case ATTRIBUTE_STATE_MAP_SMOOTH:
      case ATTRIBUTE_STATE_MAP_ALPHA:
      case ATTRIBUTE_STATE_MAP_BACKFACE_CULL:
      case ATTRIBUTE_STATE_MAP_PERSPECTIVE_FOCAL:
      case ATTRIBUTE_STATE_MAP_PERSPECTIVE_ZPLANE:
      case ATTRIBUTE_PART_ITEM_ASPECT_MODE:
      case ATTRIBUTE_PART_ITEM_ALIGN_X:
      case ATTRIBUTE_PART_ITEM_ALIGN_Y:
      case ATTRIBUTE_PART_ITEM_WEIGHT_X:
      case ATTRIBUTE_PART_ITEM_WEIGHT_Y:
      case ATTRIBUTE_PART_ITEM_ASPECT_H:
      case ATTRIBUTE_PART_ITEM_ASPECT_W:
      case ATTRIBUTE_PART_ITEM_MAX_H:
      case ATTRIBUTE_PART_ITEM_MAX_W:
      case ATTRIBUTE_PART_ITEM_MIN_H:
      case ATTRIBUTE_PART_ITEM_MIN_W:
      case ATTRIBUTE_PART_ITEM_PREFER_H:
      case ATTRIBUTE_PART_ITEM_PREFER_W:
      case ATTRIBUTE_PART_ITEM_SPREAD_H:
      case ATTRIBUTE_PART_ITEM_SPREAD_W:
      case ATTRIBUTE_PART_ITEM_SPAN_COL:
      case ATTRIBUTE_PART_ITEM_SPAN_ROW:
      case ATTRIBUTE_PART_ITEM_POSITION_COL:
      case ATTRIBUTE_PART_ITEM_POSITION_ROW:
      case ATTRIBUTE_PART_ITEM_SOURCE:
      case ATTRIBUTE_PART_ITEM_PADDING_RIGHT:
      case ATTRIBUTE_PART_ITEM_PADDING_LEFT:
      case ATTRIBUTE_PART_ITEM_PADDING_TOP:
      case ATTRIBUTE_PART_ITEM_PADDING_BOTTOM:
      default:
         break;
     }
}

static void
_colorclass_added(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *ei __UNUSED__)
{
}

static void
_colorclass_deleted(void *data __UNUSED__,
                    Evas_Object *obj __UNUSED__,
                    void *ei __UNUSED__)
{
}

static void
_colorclass_changed(void *data __UNUSED__,
                    Evas_Object *obj __UNUSED__,
                    void *ei __UNUSED__)
{
}

static void
_sound_added(void *data __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void *ei __UNUSED__)
{
}

static void
_sound_deleted(void *data __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *ei __UNUSED__)
{
}

static void
_image_added(void *data __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void *ei __UNUSED__)
{
}

static void
image_deleted(void *data __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void *ei __UNUSED__)
{
}

static void
_style_added(void *data __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void *ei __UNUSED__)
{
}

static void
_style_deleted(void *data __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *ei __UNUSED__)
{
}

static void
_part_renamed(void *data,
              Evas_Object *obj __UNUSED__,
              void *ei)
{
   Rename *ren = ei;
   Part2 *current_part;
   Project *pro = (Project *)data;

   Group2 *group = _get_current_group2(pro);

   current_part = (Part2 *)resource_manager_find(group->parts, ren->old_name);
   eina_stringshare_del(current_part->common.name);
   current_part->common.name = eina_stringshare_add(ren->new_name);
}

static void
_group_data_renamed(void *data,
              Evas_Object *obj __UNUSED__,
              void *ei)
{
   Rename *ren = ei;
   Group_Data2 *group_data;
   Project *pro = (Project *)data;

   Group2 *group = _get_current_group2(pro);
   group_data = (Group_Data2 *)resource_manager_find(group->data_items, ren->old_name);

   eina_stringshare_del(group_data->common.name);
   group_data->common.name = eina_stringshare_add(ren->new_name);
}
static void
_editor_part_added_cb(void *data __UNUSED__,
                      Evas_Object *obj __UNUSED__,
                      void *event_info)
{
   Eina_Stringshare *part_name = event_info;
   Project *pro = (Project *)data;

   Group2 *group = _get_current_group2(pro);
   Part2 *part = _gm_part_add(pro, group, part_name);
   _part_dependency_load(pro, group, part);
}

static void
_editor_part_deleted_cb(void *data,
                        Evas_Object *obj __UNUSED__,
                        void *event_info)
{
   State2 *state;
   Part2 *part;
   Part_Item2 *item;
   Eina_Stringshare *part_name = event_info;
   Project *pro = (Project *)data;

   Group2 *group = _get_current_group2(pro);
   part = (Part2 *)resource_manager_find(group->parts, part_name);

   TODO("Apply more complex work (with warning and error maybe?) with parts which are used by other resources later")
   /* step by step */
   /* 1. remove part from all "used_in" and "uses___" and cleanup */
   _resource_usage_dependency_cleanup((Resource2 *)part);
   eina_stringshare_del(part->common.name);
   EINA_LIST_FREE(part->states, state)
     {
        /* 2.1. remove each state from all "used_in" and "uses___" and cleanup */
        _resource_usage_dependency_cleanup((Resource2 *)state);
        /* 2.2. cleanup list of tweens */
        eina_list_free(state->tweens);
        /* 2.3. free state */
        eina_stringshare_del(state->common.name);
        eina_stringshare_del(state->normal);
        free(state);
     }
   EINA_LIST_FREE(part->items, item)
     {
        /* 3.1. remove each item from all "used_in" and "uses___" and cleanup */
        _resource_usage_dependency_cleanup((Resource2 *)item);
        /* 3.2. free item */
        eina_stringshare_del(item->common.name);
        free(item);
     }
   /* 4. cleanup items and state list */
   eina_list_free(part->states);
   eina_list_free(part->items);
   /* 5. remove part from group->parts */
   group->parts = eina_list_remove(group->parts, part);
   /* 6. free part */
   free(part);
}

static void
_editor_program_added_cb(void *data,
                         Evas_Object *obj __UNUSED__,
                         void *event_info)
{
   Eina_Stringshare *program_name = event_info;
   Project *pro = (Project *)data;
   Group2 *group = _get_current_group2(pro);

   Program2 *program = _program_load(group, program_name);
   _program_dependency_load(pro, group, program);
}

static void
_editor_program_deleted_cb(void *data,
                           Evas_Object *obj __UNUSED__,
                           void *event_info)
{
   Eina_Stringshare *program_name = event_info;
   Project *pro = (Project *)data;
   Group2 *group = _get_current_group2(pro);
   Program2 *program = (Program2 *)resource_manager_find(group->programs, program_name);

   /* 1.1. remove each program from all "used_in" and "uses___" and cleanup */
   _resource_usage_dependency_cleanup((Resource2 *)program);
   /* 1.2. cleanup list of tweens */
   eina_list_free(program->afters);
   eina_list_free(program->targets);
   /* 1.3. free state */
   free(program);

   group->programs = eina_list_remove(group->programs, program);
}

static void
_editor_group_data_added_cb(void *data,
                            Evas_Object *obj __UNUSED__,
                            void *event_info)
{
   Eina_Stringshare *group_data_name = event_info;
   Project *pro = (Project *)data;
   Group2 *group = _get_current_group2(pro);

   _gm_group_data_add(pro, group, group_data_name);
}

static void
_editor_group_data_deleted_cb(void *data,
                              Evas_Object *obj __UNUSED__,
                              void *event_info)
{
   Eina_Stringshare *group_data_name = event_info;
   Project *pro = (Project *)data;
   Group2 *group = _get_current_group2(pro);
   Group_Data2 *group_data = (Group_Data2 *)resource_manager_find(group->data_items, group_data_name);

   eina_stringshare_del(group_data->common.name);
   eina_stringshare_del(group_data->source);

   group->data_items = eina_list_remove(group->data_items, group_data);
   free(group_data);
}

static void
_editor_part_item_added_cb(void *data,
                           Evas_Object *obj __UNUSED__,
                           void *event_info)
{
   const Editor_Item *editor_item = event_info;
   Resource2 *used;
   Part_Item2 *item;
   Project *pro = (Project *)data;
   Group2 *group = _get_current_group2(pro);
   Part2 *part = (Part2 *)resource_manager_find(group->parts, editor_item->part_name);
   unsigned int count = eina_list_count(part->items);

   /* we can use _item_dependency_load here, but let's avoid using edje edit */
   item = _gm_part_item_add(part, editor_item->item_name, count);
   used = resource_manager_find(pro->groups2, editor_item->source);
   if (used)
     _resource_usage_resource_add((Resource2 *)item, used);
}

static void
_editor_part_item_deleted_cb(void *data,
                             Evas_Object *obj __UNUSED__,
                             void *event_info)
{
   const Editor_Item *editor_item = event_info;
   Project *pro = (Project *)data;
   Group2 *group = _get_current_group2(pro);
   Part2 *part = (Part2 *)resource_manager_find(group->parts, editor_item->part_name);
   Resource2 *part_item = resource_manager_find(part->items, editor_item->item_name);

   /* 3.1. remove each item from all "used_in" and "uses___" and cleanup */
   _resource_usage_dependency_cleanup(part_item);
   /* 3.2. free item */
   free(part_item);

   part->items = eina_list_remove(part->items, part_item);
}

static void
_editor_state_added_cb(void *data __UNUSED__,
                       Evas_Object *obj __UNUSED__,
                       void *event_info)
{
   const Editor_State *editor_state = event_info;
   Project *pro = (Project *)data;
   Part2 *part;
   State2 *state;
   Group2 *group = _get_current_group2(pro);

   part = (Part2 *)resource_manager_find(group->parts, editor_state->part_name);
   state = _gm_state_add(pro, group, part, editor_state->state_name, editor_state->state_value);
   _state_dependency_load(pro, group, part, state);
}

static void
_editor_state_deleted_cb(void *data,
                         Evas_Object *obj __UNUSED__,
                         void *event_info)
{
   const Editor_State *editor_state = event_info;
   Project *pro = (Project *)data;
   Part2 *part;
   State2 *state;
   Group2 *group = _get_current_group2(pro);

   part = (Part2 *)resource_manager_find(group->parts, editor_state->part_name);
   state = (State2 *)resource_manager_v_find(part->states, editor_state->state_name, editor_state->state_value);

   /* 2.1. remove each state from all "used_in" and "uses___" and cleanup */
   _resource_usage_dependency_cleanup((Resource2 *)state);
   /* 2.2. cleanup list of tweens */
   eina_list_free(state->tweens);
   /* 2.3. free state */
   free(state);

   part->states = eina_list_remove(part->states, state);
}

static void
_editor_part_restacked_cb(void *data,
                          Evas_Object *obj __UNUSED__,
                          void *event_info)
{
   const Editor_Part_Restack *editor_part_restack = event_info;
   Project *pro = (Project *)data;
   Group2 *group = _get_current_group2(pro);
   Part2 *part, *rel_part = NULL;
   Eina_List *rel_l;

   part = (Part2 *)resource_manager_find(group->parts,
                                         editor_part_restack->part_name);
   if (editor_part_restack->relative_part_name)
     rel_part = (Part2 *)resource_manager_find(group->parts,
                                               editor_part_restack->relative_part_name);
   part->group->parts = eina_list_remove(part->group->parts, part);
   if (rel_part)
     {
        rel_l = eina_list_data_find_list(part->group->parts, rel_part);
        assert (rel_l != NULL);
        part->group->parts = eina_list_prepend_relative_list(part->group->parts, part, rel_l);
     }
   else
     part->group->parts = eina_list_append(part->group->parts, part);
}

static void
_editor_part_item_restacked_cb(void *data,
                               Evas_Object *obj __UNUSED__,
                               void *event_info)
{
   const Editor_Part_Item_Restack *editor_part_item_restack = event_info;
   Project *pro = (Project *)data;
   Group2 *group = _get_current_group2(pro);
   Part_Item2 *part_item, *relative_part_item;
   Part2 *part = (Part2 *)resource_manager_find(group->parts,
                                                editor_part_item_restack->part_name);
   part_item = (Part_Item2 *)resource_manager_find(part->items,
                                                   editor_part_item_restack->part_item);
   relative_part_item = (Part_Item2 *)resource_manager_find(part->items,
                                                            editor_part_item_restack->relative_part_item);

   part->items = eina_list_remove(part->items, part_item);

   if (relative_part_item)
     part->items = eina_list_prepend_relative(part->items,
                                              part_item,
                                              relative_part_item);
   else
     part->items = eina_list_append(part->items, part_item);
}

static void
_group_add(void *data __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void *event_info)
{
   Group *group = (Group *)event_info;
   printf("New Group added [%s]", group->name);
}

static void
_group_del(void *data __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void *event_info)
{
   Eina_Stringshare *group_name = (Eina_Stringshare *)event_info;
   printf("Group deleted [%s]", group_name);
}

/* INITIAL FUNCTIONS */

void
_resource_callbacks_register(Project *project)
{
   TODO("Those signals and their edje_edit API need to be implemented through editor")
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_COLORCLASS_ADDED, _colorclass_added, project);
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_COLORCLASS_DELETED, _colorclass_deleted, project);
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_COLORCLASS_CHANGED, _colorclass_changed, project);
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_SOUND_ADDED, _sound_added, project);
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_SOUND_DELETED, _sound_deleted, project);
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_IMAGE_ADDED, _image_added, project);
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_IMAGE_DELETED, image_deleted, project);
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_STYLE_ADDED, _style_added, project);
   evas_object_smart_callback_add(ap.win,  SIGNAL_EDITOR_STYLE_DELETED, _style_deleted, project);

   /* already implemented stack of editor changes */
   evas_object_smart_callback_add(ap.win, SIGNAL_PART_RENAMED, _part_renamed, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_DATA_RENAMED, _group_data_renamed, project);
   TODO("PART COPY - check if it is working after integration")
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_ADDED, _editor_part_added_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_DELETED, _editor_part_deleted_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_RESTACKED, _editor_part_restacked_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_ITEM_ADDED, _editor_part_item_added_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_ITEM_DELETED, _editor_part_item_deleted_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_ITEM_RESTACKED, _editor_part_item_restacked_cb, project);
   TODO("STATE COPY - check if it is working after integration")
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_STATE_ADDED, _editor_state_added_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_STATE_DELETED, _editor_state_deleted_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PROGRAM_ADDED, _editor_program_added_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PROGRAM_DELETED, _editor_program_deleted_cb, project);
   TODO("add afters and targets addition and changes")
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_GROUP_DATA_ADDED, _editor_group_data_added_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_GROUP_DATA_DELETED, _editor_group_data_deleted_cb, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, _property_attribute_changed, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_ADDED, _group_add, project);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_DELETED, _group_del, project);
}

void
_resource_callbacks_unregister(Project *project)
{
   TODO("Those signals and their edje_edit API need to be implemented through editor")
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_COLORCLASS_ADDED, _colorclass_added, project);
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_COLORCLASS_DELETED, _colorclass_deleted, project);
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_COLORCLASS_CHANGED, _colorclass_changed, project);
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_SOUND_ADDED, _sound_added, project);
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_SOUND_DELETED, _sound_deleted, project);
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_IMAGE_ADDED, _image_added, project);
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_IMAGE_DELETED, image_deleted, project);
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_STYLE_ADDED, _style_added, project);
   evas_object_smart_callback_del_full(ap.win,  SIGNAL_EDITOR_STYLE_DELETED, _style_deleted, project);

   /* already implemented stack of editor changes */
   evas_object_smart_callback_del_full(ap.win, SIGNAL_PART_RENAMED, _part_renamed, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_GROUP_DATA_RENAMED, _group_data_renamed, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_ADDED, _editor_part_added_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_DELETED, _editor_part_deleted_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_RESTACKED, _editor_part_restacked_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_ITEM_ADDED, _editor_part_item_added_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_ITEM_DELETED, _editor_part_item_deleted_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_ITEM_RESTACKED, _editor_part_item_restacked_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_STATE_ADDED, _editor_state_added_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_STATE_DELETED, _editor_state_deleted_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PROGRAM_ADDED, _editor_program_added_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PROGRAM_DELETED, _editor_program_deleted_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_GROUP_DATA_ADDED, _editor_group_data_added_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_GROUP_DATA_DELETED, _editor_group_data_deleted_cb, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, _property_attribute_changed, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_GROUP_ADDED, _group_add, project);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_GROUP_DELETED, _group_del, project);
}
