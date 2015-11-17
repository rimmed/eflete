/*
 * Edje Theme Editor
 * Copyright (C) 2013-2014 Samsung Electronics.
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

#ifndef UI_WORKSPACE_H
#define UI_WORKSPACE_H

/**
 * @defgroup Workspace Workspace
 * @ingroup EFLETE
 *
 * A Workspace module create and control workspace area. It's contain control
 * for zoom operation with workspace, button to switch separate mode view.
 * Workspace provide highlight element for show and edit current part.
 * Also Workspace contain two rulers vertical and horizontal with functional
 * like most graphics editors. A popup menu can show by mouse click right
 * button on workspace object.
 */

#include <Evas.h>
#include <Elementary.h>
#include <Ewe.h>
#include "group_manager.h"

/**
 * Create a new Workspace object to the parent.
 *
 * @param parent The parent object.
 * @param group The group to be editing.
 *
 * @return The new object or NULL if it cannot be created.
 *
 * @ingroup Workspace
 */
Evas_Object *
workspace_add(Evas_Object *parent, Group *group);

/**
 * Get the groupedit object, which loaded into workspace.
 *
 * @param obj The workspace object.
 *
 * @return The groupedit object or NULL if it didn't exist'.
 *
 * @ingroup Workspace
 */
Evas_Object *
ws_groupedit_get(Evas_Object *obj);

/**
 * Update part info in part list
 *
 * @param obj The workspace object.
 * @param part Part to be updated.
 *
 * @ingroup Workspace
 */
void
workspace_group_navigator_update_part(Evas_Object *obj, Part_ *part);

/**
 * Force recal for editable object from the workspace.
 *
 * @param obj The workspace object.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_recalc(Evas_Object *obj);

/**
 * Add new part into style, which currently loaded into workspace.
 *
 * @param obj The workspace object.
 * @param part Pointer to Part_ structure.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_part_add(Evas_Object *obj, Part_ *part);
/**
 * Delete part from style, which currently loaded into workspace.
 *
 * @param obj The workspace object.
 * @param part Pointer to deleting part.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_part_del(Evas_Object *obj, Part_ *part);

/**
 * Stack part above above in workspace module. This function is a wrapper
 * on _edit_object_part_restack_above();
 *
 * @param obj The workspace object.
 * @param part The name of part, which need above above.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_part_above(Evas_Object *obj, const char *part);

/**
 * Stack part below below in workspace module. This function is a wrapper
 * on _edit_object_part_restack_below();
 *
 * @param obj The workspace object.
 * @param part The name of part, which need below below.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_part_below(Evas_Object *obj, const char *part);

/** Move the given part relative the next one.
 *
 * @param obj The workspace object.
 * @param part Name of part, which will reordered.
 * @param rel_part Name of part, which will be relative to reordering 'part'.
 * @param direct If EINA_TRUE, part will restack below, otherwise part will
 *   restack above rel_part.
 *
 * @return @c EINA_TRUE in case of success, @c EINA_FALSE otherwise.
 */

Eina_Bool
workspace_edit_object_part_restack(Evas_Object *obj,
                                   const char *part,
                                   const char *rel_part,
                                   Eina_Bool direct);

/**
 *
 */
Eina_Bool
workspace_edit_object_part_state_set(Evas_Object *obj, Part_ *part);

Eina_Bool
workspace_edit_object_part_state_add(Evas_Object *obj, const char *part,
                                     const char *state, double value);
/**
 * Copy new state for the give part, from another state of this part.
 *
 * @param obj The groupedit object,
 * @param part Part that contain state.
 * @param state_from to copy from (not including state value).
 * @param value_from The value of the state to copy from.
 * @param state_to copy into (not including state value).
 * @param value_to The value of the state to copy into.
 *
 * @return EINA_TRUE on success or EINA_FALSE, on errors.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_part_state_copy(Evas_Object *obj, const char *part,
                                     const char *state_from, double value_from,
                                     const char *state_to, double value_to);

Eina_Bool
workspace_edit_object_part_state_del(Evas_Object *obj, const char *part,
                                     const char *state, double value);
/**
 * Set a visibility for part of current groupedit object in the workspace.
 *
 * @param obj The workspace object
 * @param part The name of groupedit part to be showed of hided
 * @param visible Use EINA_TRUE to show, EINA_FALSE to hide part.
 *
 * @return EINA_TRUE, if it's showed/hided, EINA_FALSE otherwise
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_visible_set(Evas_Object *obj,
                                  const char *part,
                                  Eina_Bool visible);

/**
 * Set zoom factor for view zoommed style in groupspace.
 *
 * @param obj The Workspace object.
 * @param factor A factor for scale. Where value 1.0 = 100% scale (not zoom
 * object). Minimum is 0.01, maximum is 20. (from 1% to 2000% zoom).
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_zoom_factor_set(Evas_Object *obj, double factor);

/**
 * Get zoom factor from workspace.
 *
 * @param The workspace object.
 *
 * @return double value of zoom factor, or '0' if fail.
 *
 * @ingroup Workspace
 */
double
workspace_zoom_factor_get(Evas_Object *obj);

/**
 * Switch beetwen show or hide mode of legend in workspace object.
 *
 * @param obj The workspace object.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_legend_visible_set(Evas_Object *obj);

/**
 * Set highlight to selected part.
 *
 * @param obj The workspace object.
 * @param part Evas_Object pointer to selected part object.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_highlight_set(Evas_Object *obj, Part *part);

/**
 * Unset highlight from previously selected part.
 *
 * @param obj The workspace object.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_highlight_unset(Evas_Object *obj);

/**
 * Turn on/off the separatr mode
 *
 * @param obj The workspace object,
 * @param separated ON/OFF (true/false) the separate mode,
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_separate_mode_set(Evas_Object *obj, Eina_Bool separate);

/**
 * Check if separate mode is activated.
 *
 * @param obj The workspace object.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_separate_mode_get(Evas_Object *obj);

/**
 * Turn on/off the middle border of highlight (align) visibility.
 *
 * @param obj The workspace object,
 * @param separated ON/OFF (true/false) the middle border (align) visibility,
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_highlight_align_visible_set(Evas_Object *obj, Eina_Bool flag);

/**
 * Returns if middle border of highlight (align) is visible
 *
 * @param obj The workspace object,
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_highlight_align_visible_get(Evas_Object *obj);

/**
 * Turn on/off the object area visibility.
 *
 * @param obj The workspace object,
 * @param separated ON/OFF (true/false) the object area visibility,
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_object_area_visible_set(Evas_Object *obj, Eina_Bool flag);

/**
 * Returns if the object area is visible
 *
 * @param obj The workspace object,
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_object_area_visible_get(Evas_Object *obj);

/**
 * Change selection state for given item on workspace. If item should be
 * selected if will be highlighted, or unhighlighted in otherwise.
 *
 * @param obj The workspace object.
 * @param item_name The name of item, which selection should be changed.
 * @param selected If EINA_TRUE - item will be selected, otherwise unselected.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_part_item_selected_set(Evas_Object *obj,
                                             Eina_Stringshare *item_name,
                                             Eina_Bool selected);

/**
 * Add new item into container part.
 *
 * @param obj The workspace object.
 * @param part The name of the part, that will contain new item.
 * @param item The name of new item.
 * @param source The name of the group, that will used as source for item.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_part_item_add(Evas_Object *obj, Eina_Stringshare *part,
                                    Eina_Stringshare *item,
                                    Eina_Stringshare *source);
/**
 * Delete item from container part.
 *
 * @param obj The workspace object.
 * @param part The name of the part, that contain item.
 * @param item The name of deleting item.
 *
 * @return EINA_FALSE on failure, EINA_TRUE on success.
 *
 * @ingroup Workspace
 */
Eina_Bool
workspace_edit_object_part_item_del(Evas_Object *obj, Eina_Stringshare *part,
                                    Eina_Stringshare *item);

#endif /* UI_WORKSPACE_H */
