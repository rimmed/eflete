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

#include "main_window.h"

static Popup_Button btn_pressed;
static Evas_Object *popup;
static Evas_Object *helper;
static Evas_Object *gengrid;
static Evas_Object *fs;
static Helper_Done_Cb dismiss_func;
static void* func_data;

static const Popup_Button _btn_ok         = BTN_OK;
static const Popup_Button _btn_save       = BTN_SAVE;
static const Popup_Button _btn_replace    = BTN_REPLACE;
static const Popup_Button _btn_dont_save  = BTN_DONT_SAVE;
static const Popup_Button _btn_cancel     = BTN_CANCEL;
static Popup_Validator_Func validator     = NULL;
static void *user_data                    = NULL;

/* this one is for gengrid items */
struct _Item
{
   const char* image_name;
   const char* source;
};
typedef struct _Item Item;
static Elm_Gengrid_Item_Class *gic = NULL;

static void
_btn_cb(void *data,
        Evas_Object *obj __UNUSED__,
        void *ei __UNUSED__)
{
   btn_pressed = *((Popup_Button *)data);
   if ((BTN_OK == btn_pressed) || (BTN_SAVE == btn_pressed) || (BTN_REPLACE == btn_pressed))
     if (validator && (!validator(user_data))) return;
   eflete_main_loop_quit();
}

#define BTN_ADD(TEXT, PLACE, DATA) \
{ \
   BUTTON_ADD(popup, btn, TEXT); \
   evas_object_smart_callback_add(btn, "clicked", _btn_cb, DATA); \
   elm_object_part_content_set(popup, PLACE, btn); \
}

Popup_Button
popup_want_action(const char *title,
                  const char *msg,
                  Evas_Object *content,
                  Evas_Object *to_focus,
                  Popup_Button popup_btns,
                  Popup_Validator_Func func,
                  void *data)

{
   Evas_Object *btn;

   /* only one content will be setted to popup: or message, or used content */
   assert((msg != NULL) != (content != NULL));
   validator = func;
   user_data = data;

   ui_menu_items_list_disable_set(ap.menu, MENU_ITEMS_LIST_MAIN, true);

   popup = elm_popup_add(ap.win);
   elm_object_part_text_set(popup, "title,text", title);
   elm_popup_content_text_wrap_type_set(popup, ELM_WRAP_WORD);
   if (msg) elm_object_text_set(popup, msg);
   if (content) elm_object_content_set(popup, content);

   if (popup_btns & BTN_OK)
     BTN_ADD(_("Ok"), "button1", &_btn_ok)

   if (popup_btns & BTN_SAVE)
     BTN_ADD(_("Save"), "button1", &_btn_save)

   if (popup_btns & BTN_REPLACE)
     BTN_ADD(_("Replace"), "button1", &_btn_replace)

   if (popup_btns & BTN_DONT_SAVE)
     BTN_ADD(_("Don't save"), "button2", &_btn_dont_save)

   if ((popup_btns & BTN_CANCEL) && (popup_btns & BTN_DONT_SAVE))
     BTN_ADD(_("Cancel"), "button3", &_btn_cancel)
   else
     BTN_ADD(_("Cancel"), "button2", &_btn_cancel)

   evas_object_show(popup);
   if (to_focus) elm_object_focus_set(to_focus, true);
   eflete_main_loop_begin();

   /* clear up before return the presed button */
   elm_object_content_unset(popup);
   evas_object_del(popup);
   popup = NULL;
   ui_menu_items_list_disable_set(ap.menu, MENU_ITEMS_LIST_MAIN, false);

   validator = NULL;
   user_data = NULL;

   return btn_pressed;
}
#undef BTN_ADD

void
popup_buttons_disabled_set(Popup_Button popup_btns, Eina_Bool disabled)
{
   if ((popup_btns & BTN_OK) || (popup_btns & BTN_SAVE) || (popup_btns & BTN_REPLACE))
     elm_object_disabled_set(elm_object_part_content_get(popup, "button1"), disabled);
   if (popup_btns & BTN_DONT_SAVE)
     elm_object_disabled_set(elm_object_part_content_get(popup, "button2"), disabled);
   if ((popup_btns & BTN_CANCEL) && (popup_btns & BTN_DONT_SAVE))
     elm_object_disabled_set(elm_object_part_content_get(popup, "button3"), disabled);
   if (popup_btns & BTN_CANCEL)
     elm_object_disabled_set(elm_object_part_content_get(popup, "button2"), disabled);
}

#define FS_W 430
#define FS_H 460

#define GENGRID_W 522
#define GENGRID_H 388

static void
_helper_obj_follow(void *data __UNUSED__,
                   Evas *e __UNUSED__,
                   Evas_Object *obj,
                   void *event_info __UNUSED__)
{
   int x, y, w, h, nx, ny;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   nx = x - (FS_W + 12 - w); /* 12 - it's a helper border */
   ny = y + h;
   evas_object_move(helper, nx, ny);
}

static void
_helper_win_follow(void *data __UNUSED__,
                  Evas *e __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   int w, h, nx, ny;

   evas_object_geometry_get(ap.win, NULL, NULL, &w, &h);
   nx = (w / 2) - ((FS_W + 12) / 2);
   ny = (h / 2) - ((FS_H + 12) / 2);
   evas_object_move(helper, nx, ny);
}

static void
_helper_property_follow(void *data __UNUSED__,
                        Evas *e __UNUSED__,
                        Evas_Object *obj,
                        void *event_info __UNUSED__)
{
   int x, y, w, h, nx, ny;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   nx = x - (GENGRID_W + 12 - w); /* 12 - it's a helper border */
   ny = y + h;
   evas_object_move(helper, nx, ny);
}


static void
_helper_dismiss(void *data __UNUSED__,
                Evas_Object *obj __UNUSED__,
                const char *signal __UNUSED__,
                const char *source __UNUSED__)
{
   Evas_Object *follow_up = (Evas_Object *) data;

   evas_object_event_callback_del_full(follow_up, EVAS_CALLBACK_RESIZE, _helper_obj_follow, NULL);
   evas_object_event_callback_del_full(follow_up, EVAS_CALLBACK_MOVE, _helper_obj_follow, NULL);
   evas_object_event_callback_del_full(follow_up, EVAS_CALLBACK_RESIZE, _helper_property_follow, NULL);
   evas_object_event_callback_del_full(follow_up, EVAS_CALLBACK_MOVE, _helper_property_follow, NULL);

   if (!follow_up)
     evas_object_event_callback_del_full(ap.win, EVAS_CALLBACK_RESIZE, _helper_win_follow, NULL);

   evas_object_del(helper);
}

static void
_done(void *data __UNUSED__,
      Evas_Object *obj,
      void *event_info __UNUSED__)
{
   Eina_List *selected_paths = NULL;
   Eina_Stringshare *selected;
   Eina_Bool res = true;

   if (dismiss_func)
     {
        if (elm_fileselector_multi_select_get(fs))
          selected_paths = (Eina_List *)elm_fileselector_selected_paths_get(fs);
        else
          {
             if (elm_fileselector_is_save_get(fs))
               selected = eina_stringshare_printf("%s/%s",
                                                  elm_fileselector_path_get(fs),
                                                  elm_fileselector_current_name_get(fs));
             else
               selected = eina_stringshare_add(elm_fileselector_selected_get(fs));
             selected_paths = eina_list_append(selected_paths, selected);
          }
        res = ((Helper_Done_Cb)dismiss_func)(func_data, obj, selected_paths);

        if (!elm_fileselector_multi_select_get(fs))
          EINA_LIST_FREE(selected_paths, selected)
             eina_stringshare_del(selected);
     }

   if (res)
     {
        dismiss_func = NULL;
        func_data = NULL;
        _helper_dismiss(data, NULL, NULL, NULL);
     }
}

static void
_fileselector_helper(const char *title,
                     Evas_Object *follow_up,
                     const char *path,
                     Eina_Bool multi,
                     Eina_Bool is_save,
                     Helper_Done_Cb func,
                     void *data,
                     Elm_Fileselector_Filter_Func filter_cb)
{
   Evas_Object *scroller;

   dismiss_func = func;
   func_data = data;

   helper = elm_layout_add(ap.win);
   elm_layout_theme_set(helper, "layout", "popup", title ? "hint_title" : "hint");
   elm_layout_signal_callback_add(helper, "hint,dismiss", "eflete", _helper_dismiss, follow_up);

   fs = elm_fileselector_add(ap.win);
   elm_fileselector_expandable_set(fs, false);
   elm_fileselector_is_save_set(fs, is_save);
   elm_fileselector_multi_select_set(fs, multi);
   if (filter_cb)
     {
        elm_fileselector_custom_filter_append(fs, filter_cb, NULL, "edj");
        evas_object_hide(elm_layout_content_unset(fs, "elm.swallow.filters"));
     }
   else elm_fileselector_folder_only_set(fs, true);

   elm_fileselector_path_set(fs, path ? path : profile_get()->general.projects_folder);
   evas_object_smart_callback_add(fs, "done", _done, follow_up);
   evas_object_smart_callback_add(fs, "activated", _done, follow_up);
   /* small hack, hide not necessary button */
   evas_object_hide(elm_layout_content_unset(fs, "elm.swallow.cancel"));
   evas_object_size_hint_min_set(helper, FS_W, FS_H);
   evas_object_resize(helper, FS_W, FS_H);

   /* scroller is necessary to fix issues with fileselector size */
   SCROLLER_ADD(ap.win, scroller);
   elm_object_content_set(scroller, fs);

   if (title) elm_object_text_set(helper, title);
   elm_layout_content_set(helper, "elm.swallow.content", scroller);
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   if (follow_up)
     {
        _helper_obj_follow(NULL, NULL, follow_up, NULL);
        evas_object_event_callback_add(follow_up, EVAS_CALLBACK_RESIZE, _helper_obj_follow, NULL);
        evas_object_event_callback_add(follow_up, EVAS_CALLBACK_MOVE, _helper_obj_follow, NULL);
     }
   else
     {
        _helper_win_follow(NULL, NULL, NULL, NULL);
        evas_object_event_callback_add(ap.win, EVAS_CALLBACK_RESIZE, _helper_win_follow, NULL);
     }
   evas_object_show(helper);
}

void
popup_fileselector_folder_helper(const char *title, Evas_Object *follow_up, const char *path,
                                 Helper_Done_Cb func, void *data,
                                 Eina_Bool multi, Eina_Bool is_save)
{
   _fileselector_helper(title, follow_up, path, multi, is_save, func, data, NULL);
}

static Eina_Bool
_edj_filter(const char *path,
            Eina_Bool dir,
            void *data __UNUSED__)
{
   if (dir) return true;

   if (eina_str_has_extension(path, ".edj"))
     return true;
   return false;
}

void
popup_fileselector_edj_helper(const char *title, Evas_Object *follow_up, const char *path,
                              Helper_Done_Cb func, void *data,
                              Eina_Bool multi, Eina_Bool is_save)
{
   _fileselector_helper(title, follow_up, path, multi, is_save, func, data, _edj_filter);
}

static Eina_Bool
_edc_filter(const char *path,
            Eina_Bool dir,
            void *data __UNUSED__)
{
   if (dir) return true;

   if (eina_str_has_extension(path, ".edc"))
     return true;
   return false;
}

void
popup_fileselector_edc_helper(const char *title, Evas_Object *follow_up, const char *path,
                              Helper_Done_Cb func, void *data,
                              Eina_Bool multi, Eina_Bool is_save)
{
   _fileselector_helper(title, follow_up, path, multi, is_save, func, data, _edc_filter);
}

static Eina_Bool
_images_filter(const char *path,
               Eina_Bool dir,
               void *data __UNUSED__)
{
   int i;
   Eina_Bool res;
   const char *image_formats[] = { "png", "jpg", "jpeg", "jfif", "xpm", "tif",
                                   "tiff", "gif", "pbm", "pgm", "ppm", "pnm",
                                   "bmp", "wbmp", "webp", "psd", "tga", NULL};
   if (dir) return true;
   i = 0;
   while(image_formats[i])
     {
        res = eina_str_has_extension(path, image_formats[i++]);
        if (res) return true;
     }

   return false;
}

void
popup_fileselector_image_helper(const char *title, Evas_Object *follow_up, const char *path,
                                Helper_Done_Cb func, void *data,
                                Eina_Bool multi, Eina_Bool is_save)
{
   _fileselector_helper(title, follow_up, path, multi, is_save, func, data, _images_filter);
}

#define ITEM_WIDTH 100
#define ITEM_HEIGHT 115
#define GROUP_ITEM_WIDTH 36
#define GROUP_ITEM_HEIGHT 36

static void
_done_image(void *data,
            Evas_Object *obj __UNUSED__,
            void *event_info __UNUSED__)
{
   Eina_Bool res = true;
   Item *item = NULL;
   const Eina_List *sel_list, *l;
   Eina_List *ret_list = NULL;
   Elm_Object_Item *item_list;

   if (dismiss_func)
     {
        sel_list = elm_gengrid_selected_items_get(gengrid);
        EINA_LIST_FOREACH(sel_list, l, item_list)
          {
             item = elm_object_item_data_get(item_list);
             ret_list = eina_list_append(ret_list, eina_stringshare_add(item->image_name));
          }
        res = ((Helper_Done_Cb)dismiss_func)(func_data, obj, ret_list);
        eina_list_free(ret_list);
     }

   if (res)
     {
        dismiss_func = NULL;
        func_data = NULL;
        _helper_dismiss(data, NULL, NULL, NULL);
     }
}

/* deletion callback */
static void
_grid_del(void *data,
          Evas_Object *obj __UNUSED__)
{
   Item *it = data;

   assert(it != NULL);

   eina_stringshare_del(it->image_name);
   eina_stringshare_del(it->source);
   free(it);
}

Eina_Bool
_image_gengrid_init()
{
   Eina_List *l = NULL;
   Item *it = NULL;
   Eina_List *images = NULL;
   int counter = 0;
   External_Resource *res;

   images = ap.project->images;

   if (images)
     {
        EINA_LIST_FOREACH(images, l, res)
           {
              counter++;
              if (!res->name)
                {
                   ERR("name not found for image #%d",counter);
                   continue;
                }
              it = (Item *)mem_malloc(sizeof(Item));
              it->image_name = eina_stringshare_add(res->name);
              it->source = eina_stringshare_add(res->source);
              elm_gengrid_item_append(gengrid, gic, it, NULL, NULL);
           }
         elm_gengrid_item_bring_in(elm_gengrid_first_item_get(gengrid),
                                   ELM_GENGRID_ITEM_SCROLLTO_TOP);
     }
   elm_scroller_policy_set(gengrid, ELM_SCROLLER_POLICY_OFF,
                           ELM_SCROLLER_POLICY_AUTO);
   evas_object_smart_calculate(gengrid);

   return true;
}

static char *
_grid_label_get(void *data,
                Evas_Object *obj __UNUSED__,
                const char  *part __UNUSED__)
{
   const Item *it = data;
   return strdup(it->image_name);
}

/* icon fetching callback */
#define MAX_ICON_SIZE 16
static Evas_Object *
_grid_content_get(void *data,
                  Evas_Object *obj,
                  const char  *part)
{
   Item *it = data;
   Evas_Object *image_obj = NULL;
   Evas_Object *grid = (Evas_Object *)obj;
   Resource *res;

   assert(it != NULL);
   assert(grid != NULL);

   if (!strcmp(part, "elm.swallow.icon"))
     {
        image_obj = elm_thumb_add(grid);
        elm_thumb_file_set(image_obj, it->source, NULL);
        elm_thumb_reload(image_obj);
        evas_object_show(image_obj);
     }
   else if (!strcmp(part, "elm.swallow.end"))
     {
        res = (Resource *) pm_resource_get(ap.project->images, it->image_name);
        if (eina_list_count(res->used_in) == 0)
          {
             image_obj = elm_icon_add(grid);
             elm_image_file_set(image_obj, EFLETE_THEME, "elm/image/icon/attention");
             evas_object_show(image_obj);
          }
     }

   return image_obj;
}
#undef MAX_ICON_SIZE

void
popup_gengrid_image_helper(const char *title, Evas_Object *follow_up,
                           Helper_Done_Cb func, void *data,
                           Eina_Bool multi)
{
   Evas_Object *entry, *icon, *button;

   dismiss_func = func;
   func_data = data;

   helper = elm_layout_add(ap.win);
   elm_layout_theme_set(helper, "layout", "popup", title ? "hint_title" : "hint");
   elm_layout_signal_callback_add(helper, "hint,dismiss", "eflete", _helper_dismiss, follow_up);

   fs = elm_layout_add(helper);
   elm_layout_theme_set(fs, "layout", "image_editor", "usage_info");
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(fs);

   gengrid = elm_gengrid_add(fs);
   if (multi)
     {
        elm_gengrid_multi_select_set(gengrid, true);

        BUTTON_ADD(fs, button, _("Ok"))
        elm_object_part_content_set(helper, "elm.swallow.ok", button);
        evas_object_smart_callback_add(button, "clicked", _done_image, follow_up);
        evas_object_show(button);
     }
   else
     {
        elm_gengrid_multi_select_set(gengrid, false);
        elm_gengrid_multi_select_mode_set(gengrid,
                                          ELM_OBJECT_MULTI_SELECT_MODE_WITH_CONTROL);
        evas_object_smart_callback_add(gengrid, "clicked,double", _done_image, follow_up);
     }
   elm_gengrid_item_size_set(gengrid, ITEM_WIDTH, ITEM_HEIGHT);
   elm_gengrid_align_set(gengrid, 0.0, 0.0);
   elm_scroller_policy_set(gengrid, ELM_SCROLLER_POLICY_OFF,
                           ELM_SCROLLER_POLICY_OFF);
   elm_object_part_content_set(fs, "eflete.swallow.genlist", gengrid);

   if (!gic)
     {
        gic = elm_gengrid_item_class_new();
        gic->item_style = "default";
        gic->func.text_get = _grid_label_get;
        gic->func.content_get = _grid_content_get;
        gic->func.del = _grid_del;
     }

   _image_gengrid_init();

   ENTRY_ADD(fs, entry, true);
   elm_object_part_text_set(entry, "guide", _("Search"));
   ICON_STANDARD_ADD(entry, icon, true, "search");
   elm_object_part_content_set(entry, "elm.swallow.end", icon);
   elm_object_part_content_set(fs, "eflete.swallow.search_line", entry);

   /* small hack, hide not necessary button */
   evas_object_hide(elm_layout_content_unset(fs, "elm.swallow.cancel"));
   evas_object_size_hint_min_set(helper, GENGRID_W, GENGRID_H);
   evas_object_resize(helper, GENGRID_W, GENGRID_H);

   if (title) elm_object_text_set(helper, title);
   elm_layout_content_set(helper, "elm.swallow.content", fs);
   evas_object_size_hint_weight_set(fs, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(fs, EVAS_HINT_FILL, EVAS_HINT_FILL);
   if (follow_up)
     {
        _helper_property_follow(NULL, NULL, follow_up, NULL);
        evas_object_event_callback_add(follow_up, EVAS_CALLBACK_RESIZE, _helper_property_follow, NULL);
        evas_object_event_callback_add(follow_up, EVAS_CALLBACK_MOVE, _helper_property_follow, NULL);
     }
   else
     {
        _helper_win_follow(NULL, NULL, NULL, NULL);
        evas_object_event_callback_add(ap.win, EVAS_CALLBACK_RESIZE, _helper_win_follow, NULL);
     }
   evas_object_show(helper);
}

void
popup_log_message_helper(const char *msg)
{
   Evas_Object *box, *en, *lab;

   helper = elm_layout_add(ap.win);
   elm_layout_theme_set(helper, "layout", "popup", "hint");
   elm_layout_signal_callback_add(helper, "hint,dismiss", "eflete", _helper_dismiss, ap.win);

   BOX_ADD(helper, box, false, false)
   elm_box_padding_set(box, 0, 6);
   LABEL_ADD(helper, lab, _("<font_size=14>Import edc-file error"))
   evas_object_show(lab);
   ENTRY_ADD(box, en, false)
   elm_entry_editable_set(en, false);
   elm_box_pack_end(box, lab);
   elm_box_pack_end(box, en);
   elm_layout_content_set(helper, NULL, box);

   elm_entry_entry_set(en, msg);

   _helper_win_follow(NULL, NULL, NULL, NULL);
   evas_object_event_callback_add(ap.win, EVAS_CALLBACK_RESIZE, _helper_win_follow, NULL);
   evas_object_show(helper);
}

#undef FS_W
#undef FS_H

#undef GENGRID_W
#undef GENGRID_H
