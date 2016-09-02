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
#include "string_common.h"

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
_part_renamed(void *data __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void *ei)
{
   Rename *ren = ei;
   printf("Rename part from %s to %s \n", ren->old_name, ren->new_name);
}

static void
_group_data_renamed(void *data __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void *ei)
{
   Rename *ren = ei;
   printf("Rename group data from %s to %s \n", ren->old_name, ren->new_name);
}
static void
_editor_part_added_cb(void *data __UNUSED__,
                      Evas_Object *obj __UNUSED__,
                      void *event_info)
{
   Eina_Stringshare *part_name = event_info;
   printf("Added new part %s \n", part_name);
}

static void
_editor_part_deleted_cb(void *data __UNUSED__,
                        Evas_Object *obj __UNUSED__,
                        void *event_info)
{
   Eina_Stringshare *part_name = event_info;
   printf("Deleted part %s \n", part_name);
}

static void
_editor_program_added_cb(void *data __UNUSED__,
                         Evas_Object *obj __UNUSED__,
                         void *event_info)
{
   Eina_Stringshare *program_name = event_info;
   printf("Added new proram %s \n", program_name);
}

static void
_editor_program_deleted_cb(void *data __UNUSED__,
                           Evas_Object *obj __UNUSED__,
                           void *event_info)
{
   Eina_Stringshare *program_name = event_info;
   printf("Deleted program %s \n", program_name);
}

static void
_editor_group_data_added_cb(void *data __UNUSED__,
                            Evas_Object *obj __UNUSED__,
                            void *event_info)
{
   Eina_Stringshare *group_data_name = event_info;
   printf("Added new group data %s \n", group_data_name);
}

static void
_editor_group_data_deleted_cb(void *data __UNUSED__,
                              Evas_Object *obj __UNUSED__,
                              void *event_info)
{
   Eina_Stringshare *group_data_name = event_info;
   printf("Deleted group data %s \n", group_data_name);
}

static void
_editor_part_item_added_cb(void *data __UNUSED__,
                           Evas_Object *obj __UNUSED__,
                           void *event_info)
{
   const Editor_Item *editor_item = event_info;
   printf("Added to part %s new item %s \n", editor_item->part_name, editor_item->item_name);
}

static void
_editor_part_item_deleted_cb(void *data __UNUSED__,
                             Evas_Object *obj __UNUSED__,
                             void *event_info)
{
   const Editor_Item *editor_item = event_info;
   printf("Deleted from part %s new item %s \n", editor_item->part_name, editor_item->item_name);
}

static void
_editor_state_added_cb(void *data __UNUSED__,
                       Evas_Object *obj __UNUSED__,
                       void *event_info)
{
   const Editor_State *editor_state = event_info;
   printf("Added to part %s new state %s %f \n", editor_state->part_name,
          editor_state->state_name, editor_state->state_value);
}

static void
_editor_state_deleted_cb(void *data __UNUSED__,
                         Evas_Object *obj __UNUSED__,
                         void *event_info)
{
   const Editor_State *editor_state = event_info;
   printf("Deleted from part %s new state %s %f \n", editor_state->part_name,
          editor_state->state_name, editor_state->state_value);
}

static void
_editor_part_restacked_cb(void *data __UNUSED__,
                          Evas_Object *obj __UNUSED__,
                          void *event_info)
{
   const Editor_Part_Restack *editor_part_restack = event_info;
   printf("Restack part %s related to %s \n",
          editor_part_restack->part_name,
          editor_part_restack->relative_part_name);
}

static void
_editor_part_item_restacked_cb(void *data __UNUSED__,
                               Evas_Object *obj __UNUSED__,
                               void *event_info)
{
   const Editor_Part_Item_Restack *editor_part_item_restack = event_info;
   printf("Restacking from part %s part item %s related %s",
          editor_part_item_restack->part_name,
          editor_part_item_restack->part_item,
          editor_part_item_restack->relative_part_item);
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
_resource_callbacks_register()
{
   evas_object_smart_callback_add(ap.win, SIGNAL_PART_RENAMED, _part_renamed, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_DATA_RENAMED, _group_data_renamed, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_ADDED, _editor_part_added_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_DELETED, _editor_part_deleted_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_RESTACKED, _editor_part_restacked_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_ITEM_ADDED, _editor_part_item_added_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_ITEM_DELETED, _editor_part_item_deleted_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PART_ITEM_RESTACKED, _editor_part_item_restacked_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_STATE_ADDED, _editor_state_added_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_STATE_DELETED, _editor_state_deleted_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PROGRAM_ADDED, _editor_program_added_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_PROGRAM_DELETED, _editor_program_deleted_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_GROUP_DATA_ADDED, _editor_group_data_added_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_GROUP_DATA_DELETED, _editor_group_data_deleted_cb, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, _property_attribute_changed, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_ADDED, _group_add, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_DELETED, _group_del, NULL);

   TODO("Need signals to add/etc top level blocks (groups, images, sounds, color_classes etc)")
}

void
_resource_callbacks_unregister()
{
   evas_object_smart_callback_del_full(ap.win, SIGNAL_PART_RENAMED, _part_renamed, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_GROUP_DATA_RENAMED, _group_data_renamed, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_ADDED, _editor_part_added_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_DELETED, _editor_part_deleted_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_RESTACKED, _editor_part_restacked_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_ITEM_ADDED, _editor_part_item_added_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_ITEM_DELETED, _editor_part_item_deleted_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PART_ITEM_RESTACKED, _editor_part_item_restacked_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_STATE_ADDED, _editor_state_added_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_STATE_DELETED, _editor_state_deleted_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PROGRAM_ADDED, _editor_program_added_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_PROGRAM_DELETED, _editor_program_deleted_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_GROUP_DATA_ADDED, _editor_group_data_added_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_GROUP_DATA_DELETED, _editor_group_data_deleted_cb, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, _property_attribute_changed, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_GROUP_ADDED, _group_add, NULL);
   evas_object_smart_callback_del_full(ap.win, SIGNAL_GROUP_DELETED, _group_del, NULL);
}
