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

#ifdef HAVE_CONFIG_H
   #include "eflete_config.h"
#endif /* include eflete_config.h */

#include "main_window.h"
#include "history_ui.h"
#include "navigator.h"
#include "tabs.h"
#include "shortcuts.h"
#include "cursor.h"

static void
_navigator_group_open(void *data __UNUSED__,
                      Evas_Object *obj __UNUSED__,
                      void *event_info)
{
   tabs_tab_add((Group *)event_info);
}

static void
_on_done(void *data __UNUSED__,
         Evas_Object *obj __UNUSED__,
         void *event_info __UNUSED__)
{
   ui_main_window_del();
}

Eina_Bool
ui_main_window_del(void)
{
   if (ap.project) project_close();

   /*
   if (!history_term(ap.history))
     {
        ERR("Failed terminate history module");
        abort();
     }
   */
#ifdef HAVE_ENVENTOR
   code_edit_mode_switch(false);
#endif

   /* FIXME: remove it from here */
   //live_view_free(ap.live_view);

   free(ap.menu);
   ap.menu = NULL;
   cursor_main_free();

   config_save();
   INFO("%s %s - Finished...", PACKAGE_NAME, VERSION);
   /* FIXME: when be implemented multi workspace feature, remove this line */
   evas_object_del(ap.workspace);
   elm_exit();

   return true;
}

static void
_history_click(void *data __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   elm_layout_content_set(ap.block.right_top, NULL, ap.block.history);
}
static void
_signal_click(void *data __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void *event_info __UNUSED__)
{
   elm_layout_content_set(ap.block.right_top, NULL, ap.block.history);
}

Eina_Bool
ui_main_window_add(void)
{
   Config *config;
   Evas_Object *bg, *navigator, *tabs, *toolbar;

   config_load();
   config = config_get();

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   ap.win = elm_win_add(NULL, "eflete", ELM_WIN_BASIC);

   assert(ap.win != NULL);

   evas_object_resize(ap.win, config->window.w, config->window.h);
   evas_object_move(ap.win, config->window.x, config->window.y);

   elm_win_title_set(ap.win, "EFL Edje Theme Editor");

   evas_object_smart_callback_add(ap.win, "delete,request", _on_done, NULL);
   if (!cursor_main_set(ap.win, CURSOR_ARROW))
     {
        ERR("Main cursor not setted.");
        abort();
     }

   elm_object_theme_set(ap.win, ap.theme);

   bg = elm_bg_add(ap.win);
   elm_win_resize_object_add(ap.win, bg);
   evas_object_size_hint_min_set(bg, 1024, 600);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_focus_highlight_enabled_set(ap.win, false);
   evas_object_show(bg);

   ap.win_layout = elm_layout_add(ap.win);
   elm_layout_theme_set(ap.win_layout, "layout", "window", "main");
   evas_object_size_hint_weight_set(ap.win_layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(ap.win, ap.win_layout);
   elm_layout_text_set(ap.win_layout, "eflete.project.time", _("Last saved: none"));
   elm_layout_text_set(ap.win_layout, "eflete.project.part", _("Project path: none"));
   evas_object_show(ap.win_layout);

   /* add panes to main window */
   ap.panes.left = elm_panes_add(ap.win);
   evas_object_size_hint_weight_set(ap.panes.left, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ap.panes.left, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_content_set(ap.win_layout, "eflete.swallow.panes", ap.panes.left);

   ap.panes.right = elm_panes_add(ap.win);
   evas_object_size_hint_weight_set(ap.panes.right, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ap.panes.right, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_content_set(ap.panes.left, "right", ap.panes.right);

   ap.panes.right_hor = elm_panes_add(ap.win);
   elm_panes_horizontal_set(ap.panes.right_hor, true);
   evas_object_size_hint_weight_set(ap.panes.right_hor, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(ap.panes.right_hor, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_part_content_set(ap.panes.right, "right", ap.panes.right_hor);

   /* apply the panes size from config */
   elm_panes_content_left_size_set(ap.panes.left, config->panes.left);
   elm_panes_content_left_size_set(ap.panes.right, config->panes.right);
   elm_panes_content_left_size_set(ap.panes.right_hor, config->panes.right_hor);

   navigator = navigator_add();
   evas_object_smart_callback_add(navigator, "group,open", _navigator_group_open, NULL);
   elm_object_part_content_set(ap.panes.left, "left", navigator);

   tabs = tabs_add();
   elm_object_part_content_set(ap.panes.right, "left", tabs);

   /* add tabs with history and signals */
   ap.block.right_top = elm_layout_add(ap.win);
   elm_layout_theme_set(ap.block.right_top, "layout", "tabs", "default");

   toolbar = elm_toolbar_add(ap.block.right_top);
   elm_object_style_set(toolbar, "editor_tabs_horizontal");
   elm_layout_content_set(ap.block.right_top, "elm.swallow.toolbar", toolbar);
   elm_toolbar_shrink_mode_set(toolbar, ELM_TOOLBAR_SHRINK_SCROLL);
   elm_toolbar_select_mode_set(toolbar, ELM_OBJECT_SELECT_MODE_ALWAYS);
   evas_object_size_hint_weight_set(toolbar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_toolbar_align_set(toolbar, 0.0);

   elm_toolbar_item_append(toolbar, NULL, _("History"), _history_click, NULL);
   elm_toolbar_item_append(toolbar, NULL, _("Signals"), _signal_click, NULL);

   ap.block.history = history_ui_add();
   ap.block.signals = ui_signal_list_add(ap.win);
   elm_layout_content_set(ap.block.right_top, NULL, ap.block.history);
   elm_object_part_content_set(ap.panes.right_hor, "left", ap.block.right_top);

   ap.block.property = ui_property_add(ap.win);
   elm_object_part_content_set(ap.panes.right_hor, "right", ap.block.property);

   ap.menu = ui_menu_add();

   evas_object_smart_callback_add(navigator, "group,open", _navigator_group_open, NULL);
   //ui_panes_add();
   //ap.workspace = workspace_add(ap.block.canvas);
   //ui_block_ws_set(ap.workspace);
   //evas_object_show(ap.workspace);
   //ap.live_view = live_view_add(ap.block.bottom_right, false);
   //ui_block_live_view_set(ap.live_view->layout);
   //ap.colorsel = colorselector_add(ap.win);
   #ifdef HAVE_ENVENTOR
     ap.enventor= enventor_object_init(ap.win);
   #endif /* HAVE_ENVENTOR */
   //register_callbacks();

   return true;
}
