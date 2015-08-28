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

#ifndef UI_PROPERTY_1_H
#define UI_PROPERTY_1_H

/**
 * @defgroup Property Property
 * @ingroup Block
 *
 * This widget designed for show group and part details.
 */

#ifndef HAVE_ENVENTOR
#include "syntax_color.h"
#endif
#include "widget_manager.h"
#include "ui_workspace.h"

/**
 * Add a new Property widget.
 *
 * @param parent The parent widget.
 * @return A new 'property' widget handler or NULL, on errors
 *
 * @ingroup Property
 */
Evas_Object *
ui_property_add(Evas_Object *parent);

/**
 * Set to current 'property' widget a part state view which state details.
 * If current property has a part view, then old view will be removed.
 *
 * @param property The 'property' widget.
 * @param part The Part data which will be displayed.
 *
 * @ingroup Property
 */
Eina_Bool
ui_property_state_set(Evas_Object *property, Part_ *part);

/**
 * Unset the part state view content on a given property.
 *
 * @param property the 'property' widget.
 *
 * @ingroup Property
 */
void
ui_property_state_unset(Evas_Object *property);

/**
 * Set to current 'property' widget a part item view.
 * If current property has a part item view, then old view will be removed.
 *
 * @param property The 'property' widget.
 * @param item The Item name data which will be displayed.
 *
 * @ingroup Property
 */
Eina_Bool
ui_property_item_set(Evas_Object *property, Eina_Stringshare *item);

/**
 * Unset the part item view content on a given property.
 *
 * @param property the 'property' widget.
 *
 * @ingroup Property
 */
void
ui_property_item_unset(Evas_Object *property);

#endif /* UI_PROPERTY_1_H */
