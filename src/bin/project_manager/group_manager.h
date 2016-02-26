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

#ifndef GROUP_MANAGER_H
#define GROUP_MANAGER_H

#include "eflete.h"

struct _State
{
   Eina_Stringshare *name;    /**< state name as returned from edje_edit */
   Eina_List *used_in;        /**< list of programs where state is used */

   Eina_Stringshare *parsed_name;  /**< parsed state name */
   double parsed_val;          /**< parsed state value */
   Part *part;                /**< pointer to part */
};

struct _Part
{
   Eina_Stringshare *name;    /**< part name */
   Eina_List *used_in;        /**< list of programs where part is used */

   Edje_Part_Type type;       /**< part type */
   Eina_List *states;         /**< list of states */
   State *current_state;      /**< pointer to selected state */
   Eina_List *items;          /**< list of item names. Used only for BOX and TABLE parts */
   Eina_Stringshare * current_item_name; /**< name of selected item */
   Group *group;              /**< pointer to group */
   Eina_Bool visible;         /**< is part visible on workspace*/
};

struct _Group
{
   Eina_Stringshare *name;    /**< group name */
   Eina_List *used_in;        /**< list of parts where group is used */

   Eina_Stringshare *widget;  /**< parsed widget name */
   Eina_Stringshare *class;   /**< parsed class name */
   Eina_Stringshare *style;   /**< parsed style name */
   Group *main_group;         /**< pointer to main group. NULL if group is not an alias */
   Eina_List *aliases;        /**< list of pointers to aliases. NULL if group is an alias */
   Eina_List *parts;          /**< list of parts */
   Eina_List *programs;       /**< list of programs */

   Evas_Object *edit_object;  /**< object needed to access group with edje_edit functions. Should be NULL if group is not open */
   Part *current_part;        /**< pointer to selected part */
   Eina_Stringshare *current_program; /**< pointer to selected program */

   History *history;          /**< history of changes in the group */
};

void
gm_groups_load(Project *pro);

void
gm_groups_free(Project *pro);

void
gm_group_edit_object_load(Project *pro, Group *group, Evas *e);

void
gm_group_edit_object_reload(Project *pro, Group *group);

void
gm_group_edit_object_unload(Group *group);

State *
gm_state_add(Project *pro, Part *part, const char *state_name);

Part *
gm_part_add(Project *pro, Group *group, Eina_Stringshare *part_name);

void
gm_part_item_add(Project *pro, Part *part, Eina_Stringshare *item_name);

Group *
gm_group_add(Project *pro, const char *group_name);

void
gm_group_del(Project *pro, Group *group);

void
gm_state_del(Project *pro, State *state);

void
gm_part_del(Project *pro, Part* part);

void
gm_part_item_del(Project *pro, Part *part, Eina_Stringshare *item_name);

void
gm_part_rename(Part* part, const char *new_part_name);

void
gm_part_restack(Part *part, Part *rel_part);

void
gm_part_item_restack(Part *part, Eina_Stringshare *part_item, Eina_Stringshare *relative_part_item);

void
gm_program_add(Project *pro, Group *group, Eina_Stringshare *program);

void
gm_program_del(Project *pro, Group *group, Eina_Stringshare *program);

const char *
gm_part_type_text_get(Edje_Part_Type part_type);
#endif /* GROUP_MANAGER_H */
