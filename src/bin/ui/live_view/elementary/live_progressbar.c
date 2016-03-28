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

#include "live_elementary_widgets.h"

Evas_Object *
widget_progressbar_create(Evas_Object *parent, const Group *group)
{
   assert(parent != NULL);
   assert(group != NULL);
   assert(group->style != NULL);
   assert(group->class != NULL);

   Evas_Object *object = elm_progressbar_add(parent);
   if (!strcmp(group->class, "vertical"))
      elm_progressbar_horizontal_set(object, false);

   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_SWALLOW_SET, on_swallow_check, object);
   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_TEXT_SET, on_text_check, object);
   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_SIGNAL_SEND, send_signal, object);
   evas_object_event_callback_add(object, EVAS_CALLBACK_DEL, demo_object_del, NULL);

   elm_object_style_set(object, group->style);

   return object;
}
