/* Edje Theme Editor
* Copyright (C) 2013 Samsung Electronics.
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
* along with this program; If not, see http://www.gnu.org/licenses/lgpl.html.
*/

#include "open_file_dialog.h"

static void
_on_edj_done(void *data, Evas_Object *obj, void *event_info)
{
   Evas_Object *wd_list;
   const char *selected = (const char *)event_info;
   App_Data *ap = (App_Data *)data;

   if (selected)
     {
        if (eina_str_has_suffix(selected, ".edj"))
          {
             wd_list = ui_edj_load_done(ap, selected);
             if (!wd_list) return;
             add_callbacks_wd(wd_list, ap);
          }
        else
          {
             Evas_Object *win = elm_object_parent_widget_get(obj);
             WIN_NOTIFY_ERROR(win, _("The file must have a extension '.edj'"));
             return;
          }
     }

   ecore_main_loop_quit();
}

Eina_Bool
open_edj_file(App_Data *ap)
{
   Evas_Object *win, *bg, *fs;

   if ((!ap) || (!ap->win)) return EINA_FALSE;

   MODAL_WINDOW_ADD(win, ap->win, _("Open EDJ file dialog"), _on_edj_done, ap);
   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(bg);
   elm_win_resize_object_add(win, bg);
   FILESELECTOR_ADD(fs, win, _on_edj_done, ap);
   elm_win_resize_object_add(win, fs);

   ecore_main_loop_begin();

   evas_object_del(win);

   return true;
}
