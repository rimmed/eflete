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
#define EO_BETA_API
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT

#include "main_window.h"
#include "project_manager2.h"
#include "modal_window.h"
#include "config.h"
#include "validator.h"
#include "sound_player.h"

#ifndef HAVE_TIZEN
   #define ITEM_WIDTH 100
   #define ITEM_HEIGHT 115
#else
   #define ITEM_WIDTH 87
   #define ITEM_HEIGHT 114
#endif

typedef struct _Sound_Manager Sound_Manager;
typedef struct _Search_Data Search_Data;
typedef struct _Sound Sound;

struct _Sound
{
   const char *name;
   const char *src;
   int tone_frq;
};

struct _Search_Data
{
   Evas_Object *search_entry;
   Elm_Object_Item *last_item_found;
};

struct _Sound_Manager
{
   Evas_Object *win;
   Evas_Object *panes;
   Evas_Object *btn_del;
   Evas_Object *player;
   Evas_Object *property_panes;
   Evas_Object *menu;
   Evas_Object *tone_entry, *frq_entry;
   Evas_Object *box;
   Resource_Name_Validator *tone_validator;
   Elm_Validator_Regexp *frq_validator;
   Evas_Object *gengrid;
   Evas_Object *layout;
   Elm_Object_Item *tone_header;
   Search_Data sound_search_data;
   Evas_Object *check;
   const char  *snd_src;
};

static Sound_Manager mng;
static Elm_Gengrid_Item_Class *gic = NULL;
static Elm_Gengrid_Item_Class *ggic = NULL;

static char *
_grid_label_get(void *data,
                Evas_Object *obj __UNUSED__,
                const char  *part)
{
   Sound_Data *snd = (Sound_Data *)data;

   if (!part) goto exit;

   if (!strcmp(part, "elm.text"))
     return strdup(snd->name);

   if (!strcmp(part, "elm.text.type"))
     return strdup(snd->type_label);

exit:
   return NULL;
}

static void
_grid_del(void *data, Evas_Object *obj __UNUSED__)
{
   Sound_Data *snd = (Sound_Data *)data;

   eina_stringshare_del(snd->name);
   eina_stringshare_del(snd->type_label);
   free(snd);
}

static Eina_Stringshare *
_sound_format_get(Eina_Stringshare *snd_src)
{
   Eina_Stringshare *type;
   char *dot, *up;

   assert(snd_src != NULL);

   dot = strrchr(snd_src, '.');
   if (!dot)
     return eina_stringshare_add(_("UNKNOWN"));

   dot++;
   up = strdup(dot);
   eina_str_toupper(&up);
   type = eina_stringshare_add((const char *)up);
   free(up);
   return type;
}

static void
_grid_sel_cb(void *data __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void *event_info)
{
   Resource2 *res = NULL;
   Sound_Data *snd;

   snd = elm_object_item_data_get((Elm_Gengrid_Item *)event_info);
   switch (snd->type)
     {
      case SOUND_TYPE_SAMPLE:
         res = resource_manager_find(ap.project->RM.sounds, snd->name);
         break;
      case SOUND_TYPE_TONE:
         res = resource_manager_find(ap.project->RM.tones, snd->name);
         break;
     }

   assert(res != NULL);

   if (!res->common.used_in)
     elm_object_disabled_set(mng.btn_del, false);

   sound_player_sound_set(snd);
   evas_object_smart_callback_call(ap.win, SIGNAL_SOUND_SELECT, snd);
}

static void
_sound_manager_init(void)
{
   Eina_List *l;
   Sound2 *sample;
   Tone2 *tone;
   Sound_Data *snd;

   assert(ap.project != NULL);

   snd = (Sound_Data *)mem_calloc(1, sizeof(Sound_Data));
   snd->name = eina_stringshare_add(_("Sound Samples"));
   elm_gengrid_item_append(mng.gengrid, ggic, snd, NULL, NULL);
   EINA_LIST_FOREACH(ap.project->RM.sounds, l, sample)
     {
        if (!strcmp(sample->common.name, EFLETE_DUMMY_SAMPLE_NAME)) continue;
        snd = (Sound_Data *)mem_calloc(1, sizeof(Sound_Data));
        snd->name = eina_stringshare_ref(sample->common.name);
        snd->type_label = _sound_format_get(sample->source);
        snd->type = SOUND_TYPE_SAMPLE;
        snd->resource = (Resource2 *)sample;
        elm_gengrid_item_append(mng.gengrid, gic, snd, _grid_sel_cb, NULL);
     }

   snd = (Sound_Data *)mem_calloc(1, sizeof(Sound_Data));
   snd->name = eina_stringshare_add(_("Sound Tones"));
   mng.tone_header = elm_gengrid_item_append(mng.gengrid, ggic, snd, NULL, NULL);
   EINA_LIST_FOREACH(ap.project->RM.tones, l, tone)
     {
        snd = (Sound_Data *)mem_calloc(1, sizeof(Sound_Data));
        snd->name = eina_stringshare_ref(tone->common.name);
        snd->type_label = eina_stringshare_printf("%d", tone->freq);
        snd->type = SOUND_TYPE_TONE;
        snd->resource = (Resource2 *)tone;
        elm_gengrid_item_append(mng.gengrid, gic, snd, _grid_sel_cb, NULL);
     }
}

static void
_grid_unsel_cb(void *data __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   elm_object_disabled_set(mng.btn_del, true);
   evas_object_smart_callback_call(ap.win, SIGNAL_SOUND_UNSELECTED, NULL);
}

static Eina_Bool
_add_sample_done(void *data __UNUSED__,
                 Evas_Object *obj __UNUSED__,
                 void *event_info)
{
   Sound_Data *snd;
   Attribute attribute = ATTRIBUTE_PROGRAM_SAMPLE_NAME;
   Eina_Stringshare *sound_name, *res_path;
   Eina_List *samples_list, *l;
   Eina_Bool exist = false;
   Sound2 *res;
   const char *file_name;

   Eina_List *selected_list = (Eina_List *)event_info;
   const char *selected = eina_list_data_get(selected_list);

   samples_list = ap.project->RM.sounds;

   if (!ecore_file_exists(selected))
     {
        ERR(_("File '%s' is not exist"), selected);
        return false;
     }
   if (ecore_file_is_dir(selected))
     {
        ERR(_("Unable to add folder"))
        return true;
     }

   file_name = ecore_file_file_get(selected);
   sound_name = eina_stringshare_add(file_name);
   EINA_LIST_FOREACH(samples_list, l, res)
      if (res->common.name == sound_name) /* they both are stringshares */
        {
           exist = true;
           break;
        }
   if (exist)
     {
        WARN(_("Sample '%s' is already added to project"), sound_name)
        eina_stringshare_del(sound_name);
        return false;
     }

   res_path = eina_stringshare_printf("%s/sounds/%s", ap.project->develop_path, sound_name);

   if (!ecore_file_exists(res_path))
     {
        ecore_file_cp(selected, res_path);
     }
   else
     {
        ERR(_("File '%s' exist"), res_path);
        return true;
     }

   CRIT_ON_FAIL(editor_sound_sample_add(ap.project->global_object, sound_name, selected, true));
   res = (Sound2 *)resource_manager_find(ap.project->RM.sounds, sound_name);

   snd = (Sound_Data *)mem_malloc(sizeof(Sound_Data));
   snd->name = eina_stringshare_ref(res->common.name);
   snd->type_label = _sound_format_get(res->source);
   snd->type = SOUND_TYPE_SAMPLE;
   snd->resource = (Resource2 *)res;
   elm_gengrid_item_insert_before(mng.gengrid, gic, snd, mng.tone_header, _grid_sel_cb, NULL);
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
   eina_stringshare_del(res_path);

   return true;
}

static void
_tone_add(void)
{
   Sound_Data *snd;
   Attribute attribute = ATTRIBUTE_PROGRAM_TONE_NAME;
   Eina_Stringshare *tone_name;
   int frq;
   Resource2 *tone;

   tone_name = eina_stringshare_add(elm_entry_entry_get(mng.tone_entry));
   frq = atoi(elm_entry_entry_get(mng.frq_entry));
   CRIT_ON_FAIL(editor_sound_tone_add(ap.project->global_object, tone_name, frq, true));

   tone = resource_manager_find(ap.project->RM.tones, tone_name);

   snd = (Sound_Data *)mem_malloc(sizeof(Sound_Data));
   snd->name = eina_stringshare_ref(tone->common.name);
   snd->type_label = eina_stringshare_add(elm_entry_entry_get(mng.frq_entry));
   snd->type = SOUND_TYPE_TONE;
   snd->resource = (Resource2 *)tone;
   elm_gengrid_item_append(mng.gengrid, gic, snd, _grid_sel_cb, NULL);

   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
}

static void
_sample_add_cb(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   popup_fileselector_sound_helper(_("Choose a sound"), NULL, NULL, _add_sample_done, data, false, false);
}

static void
_validation(void *data,
            Evas_Object *obj __UNUSED__,
            void *event_info __UNUSED__)
{
   Evas_Object *popup = data;
   Eina_Bool validate = EINA_FALSE;

   assert(popup != NULL);

   if (resource_name_validator_status_get(mng.tone_validator) != ELM_REG_NOERROR)
     {
        validate = EINA_FALSE;
        elm_object_signal_emit(mng.tone_entry, "validation,default,fail", "elm");
     }
   else
     {
        validate = EINA_TRUE;
        elm_object_signal_emit(mng.tone_entry, "validation,default,pass", "elm");
     }

   if (elm_validator_regexp_status_get(mng.frq_validator) != ELM_REG_NOERROR)
     {
        validate &= EINA_FALSE;
        elm_object_signal_emit(mng.frq_entry, "validation,default,fail", "elm");
     }
   else
     {
        validate &= EINA_TRUE;
        elm_object_signal_emit(mng.frq_entry, "validation,default,pass", "elm");
     }


   if (!validate)
       popup_button_disabled_set(popup, BTN_OK, true);
   else
       popup_button_disabled_set(popup, BTN_OK, false);
}

Evas_Object *
_add_tone_content_get(void *data __UNUSED__, Evas_Object *popup, Evas_Object **to_focus)
{
   Evas_Object *item, *box;

   BOX_ADD(mng.win, box, false, false);
   elm_box_padding_set(box, 0, 10);
   LAYOUT_PROP_ADD(box, _("Tone name:"), "popup", "1swallow")
   ENTRY_ADD(item, mng.tone_entry, true);
   eo_do(mng.tone_entry, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, resource_name_validator_helper, mng.tone_validator));
   evas_object_smart_callback_add(mng.tone_entry, "changed", _validation, popup);
   elm_object_part_text_set(mng.tone_entry, "guide", _("Type a new tone name"));
   elm_object_part_content_set(item, "elm.swallow.content", mng.tone_entry);
   /* need to manualy set not valid string for triggered validator */
   elm_entry_entry_set(mng.tone_entry, NULL);
   elm_box_pack_end(box, item);

   LAYOUT_PROP_ADD(box, _("Frequency:"), "popup", "1swallow")
   ENTRY_ADD(item, mng.frq_entry, true);
   eo_do(mng.frq_entry, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, mng.frq_validator));
   evas_object_smart_callback_add(mng.frq_entry, "changed", _validation, popup);
   /* force validator trigger */
   elm_entry_entry_set(mng.frq_entry, " ");
   elm_object_part_text_set(mng.frq_entry, "guide", _("Type a frequency (20 - 20000)"));
   elm_object_part_content_set(item, "elm.swallow.content", mng.frq_entry);
   /* need to manualy set not valid string for triggered validator */
   elm_entry_entry_set(mng.frq_entry, NULL);
   elm_box_pack_end(box, item);
   mng.box = box;

   if (to_focus) *to_focus = mng.tone_entry;

   return box;
}

static void
_tone_add_popup_close_cb(void *data __UNUSED__,
                         Evas_Object *obj __UNUSED__,
                         void *event_info)
{
   Popup_Button btn_res = (Popup_Button) event_info;

   if (BTN_CANCEL != btn_res)
     _tone_add();

   resource_name_validator_free(mng.tone_validator);
   elm_validator_regexp_free(mng.frq_validator);
   evas_object_del(mng.box);
}

static void
_tone_add_cb(void *data __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void *event_info __UNUSED__)
{
   Evas_Object *popup;
   mng.tone_validator = resource_name_validator_new(NAME_REGEX, NULL);
   resource_name_validator_list_set(mng.tone_validator, &ap.project->RM.tones, true);
   mng.frq_validator = elm_validator_regexp_new(FREQUENCY_REGEX, NULL);

   popup = popup_add(_("Create a new layout"), NULL, BTN_OK|BTN_CANCEL, _add_tone_content_get, mng.tone_entry);
   popup_button_disabled_set(popup, BTN_OK, true);
   evas_object_smart_callback_add(popup, POPUP_CLOSE_CB, _tone_add_popup_close_cb, NULL);
}

#undef INFO_ADD

static void
_sound_add_cb(void *data __UNUSED__,
              Evas_Object *obj,
              void *event_info __UNUSED__)
{
   Evas_Coord x, y, h;

   evas_object_geometry_get(obj, &x, &y, NULL, &h);

   elm_menu_move(mng.menu, x, y + h);
   evas_object_show(mng.menu);
}

static void
_sound_del_cb(void *data __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void *event_info __UNUSED__)
{
   Elm_Object_Item *grid_it;
   Sound_Data *snd;
   Eina_List *list, *l, *l_next;
   Attribute attribute;

   list = (Eina_List *)elm_gengrid_selected_items_get(mng.gengrid);
   EINA_LIST_FOREACH_SAFE(list, l, l_next, grid_it)
     {
        snd = elm_object_item_data_get(grid_it);

        switch (snd->type)
          {
           case SOUND_TYPE_SAMPLE:
              attribute = ATTRIBUTE_PROGRAM_SAMPLE_NAME;
              CRIT_ON_FAIL(editor_sound_sample_del(ap.project->global_object, snd->name, true));
              ecore_file_unlink(((Sound2 *)snd->resource)->source);
              elm_object_item_del(grid_it);
              break;
           case SOUND_TYPE_TONE:
              attribute = ATTRIBUTE_PROGRAM_TONE_NAME;
              CRIT_ON_FAIL(editor_sound_tone_del(ap.project->global_object, snd->name, true));
              elm_object_item_del(grid_it);
              break;
          }
     }

   elm_object_disabled_set(mng.btn_del, true);
   evas_object_smart_callback_call(ap.win, SIGNAL_SOUND_UNSELECTED, NULL);
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
}

ITEM_SEARCH_FUNC(gengrid, ELM_GENGRID_ITEM_SCROLLTO_MIDDLE, "elm.text")

static void
_search_changed_cb(void *data __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info __UNUSED__)
{
   _gengrid_item_search(mng.gengrid, &(mng.sound_search_data),
                        mng.sound_search_data.last_item_found);
}

static void
_find_next_cb(void *data __UNUSED__,
              Evas_Object *obj __UNUSED__,
              void *event_info __UNUSED__)
{
   Elm_Object_Item *start_from = NULL;

   if (mng.sound_search_data.last_item_found)
     {
        start_from =
           elm_gengrid_item_next_get(mng.sound_search_data.last_item_found);
     }

   _gengrid_item_search(mng.gengrid, &(mng.sound_search_data), start_from);
}

static void
_mw_cancel_cb(void *data __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void *event_info __UNUSED__)
{
   Evas_Object *content;

   /* unset and hide the image property */
   content = elm_object_content_unset(mng.win);
   evas_object_hide(content);
   sound_player_sound_unset();
}

static void
_mw_done_cb(void *data __UNUSED__,
         Evas_Object *obj __UNUSED__,
         void *event_info __UNUSED__)
{
   Evas_Object *content;

   /* unset and hide the image property */
   content = elm_object_content_unset(mng.win);
   evas_object_hide(content);
   sound_player_sound_unset();
}

static void
_project_closed_cb(void *data __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info __UNUSED__)
{
   elm_gengrid_clear(mng.gengrid);
}

Evas_Object *
sound_manager_add(void)
{
   Evas_Object *ic, *search_entry, *button;

   assert(ap.project != NULL);

#ifdef HAVE_AUDIO
   ecore_audio_init();
#endif

   mng.win = mw_add();
   mw_title_set(mng.win, _("Sound manager"));
   evas_object_smart_callback_add(mng.win, signals.eflete.modal_window.cancel, _mw_cancel_cb, NULL);
   evas_object_smart_callback_add(mng.win, signals.eflete.modal_window.done, _mw_done_cb, NULL);
#if !HAVE_TIZEN
   ic = elm_icon_add(mng.win);
   elm_icon_standard_set(ic, "image2");
#else
   IMAGE_ADD_NEW(mng.win, ic, "icon", "logo");
#endif
   mw_icon_set(mng.win, ic);

   if (mng.layout) goto done;

   mng.player = sound_player_add(mng.win);

#ifdef HAVE_TIZEN
   mng.layout = elm_layout_add(mng.win);
   elm_layout_theme_set(mng.layout, "layout", "manager", "internal");
   elm_object_part_text_set(mng.layout, "elm.text", _("Preview"));
   elm_layout_text_set(mng.layout, "elm.subtext", _("Sound Gallery"));
   mng.panes = elm_panes_add(mng.win);
   elm_object_part_content_set(mng.panes, "left", mng.layout);
   elm_object_part_content_set(mng.panes, "right", ap.property.sound_manager);
   elm_object_part_content_set(mng.layout, "elm.swallow.preview", mng.player);
   elm_object_style_set(mng.panes, "manager");
#else
   mng.layout = elm_layout_add(mng.win);
   elm_layout_theme_set(mng.layout, "layout", "sound_manager", "default");
   mng.panes = elm_panes_add(mng.win);
   elm_object_part_content_set(mng.panes, "left", mng.layout);

   mng.property_panes = elm_panes_add(mng.win);
   elm_panes_horizontal_set(mng.property_panes, true);
   elm_object_part_content_set(mng.panes, "right", mng.property_panes);

   elm_object_part_content_set(mng.property_panes, "left", mng.player);
   elm_object_part_content_set(mng.property_panes, "right", ap.property.sound_manager);
#endif
   elm_panes_content_right_size_set(mng.panes, 0);
   elm_panes_content_right_min_size_set(mng.panes, 400);

   if (!gic)
     {
        gic = elm_gengrid_item_class_new();
        gic->item_style = "sound_editor";
        gic->func.text_get = _grid_label_get;
        gic->func.content_get = NULL;
        gic->func.del = _grid_del;
     }

   if (!ggic)
     {
        ggic = elm_gengrid_item_class_new();
        ggic->item_style = "group_index";
        ggic->func.text_get = _grid_label_get;
        ggic->func.content_get = NULL;
        ggic->func.del = _grid_del;
     }

   mng.gengrid = elm_gengrid_add(mng.layout);
   elm_gengrid_item_size_set(mng.gengrid, ITEM_WIDTH, ITEM_HEIGHT);
   elm_gengrid_align_set(mng.gengrid, 0.0, 0.0);
   elm_gengrid_group_item_size_set(mng.gengrid, ITEM_HEIGHT/3, ITEM_HEIGHT/3);
   evas_object_smart_callback_add(mng.gengrid, signals.elm.gengrid.unselected, _grid_unsel_cb, NULL);
   elm_scroller_policy_set(mng.gengrid, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
   elm_object_part_content_set(mng.layout, "elm.swallow.list", mng.gengrid);

   button = elm_button_add(mng.layout);
   elm_object_style_set(button, "plus_managers");
   evas_object_smart_callback_add(button, signals.elm.button.clicked, _sound_add_cb, NULL);
   elm_object_part_content_set(mng.layout, "elm.swallow.btn_add", button);

   mng.btn_del = elm_button_add(mng.layout);
   elm_object_style_set(mng.btn_del, "minus_managers");
   evas_object_smart_callback_add(mng.btn_del, signals.elm.button.clicked, _sound_del_cb, NULL);
   elm_object_part_content_set(mng.layout, "elm.swallow.btn_del", mng.btn_del);
   elm_object_disabled_set(mng.btn_del, true);

   mng.menu = elm_menu_add(ap.win);
   elm_menu_item_add(mng.menu, NULL, "sound_sample", _("Sample"), _sample_add_cb, NULL);
   elm_menu_item_add(mng.menu, NULL, "sound_tone", _("Tone"), _tone_add_cb, NULL);

   ENTRY_ADD(mng.layout, search_entry, true);
   elm_object_part_text_set(search_entry, "guide", _("Search"));
#if !HAVE_TIZEN
   ICON_STANDARD_ADD(search_entry, ic, true, "search");
   elm_object_part_content_set(search_entry, "elm.swallow.end", ic);
 #else
   elm_object_style_set(search_entry, "search");
#endif
   elm_layout_content_set(mng.layout, "elm.swallow.search", search_entry);
   evas_object_smart_callback_add(search_entry, signals.elm.entry.changed, _search_changed_cb, NULL);
   evas_object_smart_callback_add(search_entry, signals.elm.entry.activated, _find_next_cb, NULL);
   mng.sound_search_data.search_entry = search_entry;
   mng.sound_search_data.last_item_found = NULL;

   evas_object_smart_callback_add(ap.win, SIGNAL_PROJECT_CLOSED, _project_closed_cb, NULL);

done:
   /* if gengrid is empty try to fill it. This happens if the managers called
    * first time or project was reopened */
   if (!elm_gengrid_realized_items_get(mng.gengrid))
     _sound_manager_init();

   elm_object_content_set(mng.win, mng.panes);
   evas_object_show(mng.win);
   elm_object_focus_set(mng.sound_search_data.search_entry, true);
   return mng.win;
}
