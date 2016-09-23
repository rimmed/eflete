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

#ifndef GROUP_NAVIGATOR_H
#define GROUP_NAVIGATOR_H

#include "eflete.h"
#include "resource_manager2.h"

/**
 * @defgroup Partlist Partlist
 *
 * @ingroup EFLETE
 */

/**
 * emited when part is selected in part list.
 * eventinfo - pointer to Part structure
 *
 * @ingroup Partlist
 */
#define SIGNAL_GROUP_NAVIGATOR_PART_SELECTED "SIGNAL_GROUP_NAVIGATOR_PART_SELECTED"

/**
 * emited when part visibility is changed.
 * eventinfo - pointer to Part structure
 *
 * @ingroup Partlist
 */
#define SIGNAL_GROUP_NAVIGATOR_PART_VISIBLE_CHANGED "SIGNAL_GROUP_NAVIGATOR_PART_VISIBLE_CHANGE"

/**
 * @ingroup Partlist
 */
Evas_Object *
group_navigator_add(Evas_Object *parent, Group2 *group);

Evas_Object *
group_navigator_dummy_get(void);

/**
 * @ingroup Partlist
 */
void
group_navigator_select(Evas_Object *obj, Resource2 *res);

/**
 * @ingroup Partlist
 */
void
group_navigator_part_update(Evas_Object *obj, Part2 *part);

void
group_navigator_group_data_update(Evas_Object *obj, Resource2 *group_data);

void
group_navigator_part_state_select(Evas_Object *obj, State2 *state);

void
group_navigator_part_add(Evas_Object *obj, Part2 *part);

void
group_navigator_part_del(Evas_Object *obj, Part2 *part);

void
group_navigator_part_item_add(Evas_Object *obj, Part2 *part, Eina_Stringshare * item_name);

void
group_navigator_part_item_del(Evas_Object *obj, Part_Item2 *item);

void
group_navigator_part_state_add(Evas_Object *obj, Part2 *part, State2 *state);

void
group_navigator_part_state_del(Evas_Object *obj, Part2 *part, State2 *state);

void
group_navigator_part_restack(Evas_Object *obj, Part2 *part, Part2 *rel_part);

void
group_navigator_part_item_restack(Evas_Object *obj,
                                  Part2 *part,
                                  Eina_Stringshare *part_item,
                                  Eina_Stringshare *relative_part_item);

void
group_navigator_program_add(Evas_Object *obj, Eina_Stringshare *program);

void
group_navigator_program_del(Evas_Object *obj, Program2 *program);

void
group_navigator_group_data_add(Evas_Object *obj, Eina_Stringshare *group_data);

void
group_navigator_group_data_del(Evas_Object *obj, Resource2 *group_data);

void
group_navigator_add_part_request(Evas_Object *obj);

void
group_navigator_add_part_item_request(Evas_Object *obj);

void
group_navigator_add_state_request(Evas_Object *obj);

void
group_navigator_add_program_request(Evas_Object *obj);

void
group_navigator_add_group_data_request(Evas_Object *obj);

void
group_navigator_delete_request(Evas_Object *obj);

void
group_navigator_state_next_request(Evas_Object *obj);

void
group_navigator_part_next_request(Evas_Object *obj);

void
group_navigator_part_prev_request(Evas_Object *obj);

void
group_navigator_part_showhide_request(Evas_Object *obj);

void
group_navigator_all_parts_showhide_request(Evas_Object *obj);
#endif /* GROUP_NAVIGATOR_H */
