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
#include "sound_editor.h"
#include "main_window.h"
#include "editor.h"

TODO("Rename this file to sound_manager")

#define ITEM_WIDTH 100
#define ITEM_HEIGHT 115
#define SND_EDIT_KEY "sound_editor_key"

typedef struct _Sound_Editor Sound_Editor;
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

struct _Sound_Editor
{
   Sound_Editor_Mode mode;
   Evas_Object *popup;
   Evas_Object *popup_btn_add;
   Evas_Object *add_cmb;
   Evas_Object *tone_entry, *frq_entry;
   Elm_Validator_Regexp *tone_validator, *frq_validator;
   Evas_Object *gengrid;
   Evas_Object *markup;
   Elm_Object_Item *tone;
   Search_Data sound_search_data;
   Evas_Object *check;
   const char  *selected;
   const char  *snd_src;

   Evas_Object *fs_win;
};

static Elm_Gengrid_Item_Class *gic = NULL, *ggic = NULL;

static char *
_grid_label_get(void *data,
                Evas_Object *obj __UNUSED__,
                const char  *part)
{
   const Item *it = data;
   if (!part)
     return NULL;

   if (!strcmp(part, "elm.label.type"))
     return strdup(it->format);
   else
     return strdup(it->sound_name);
}

static char *
_grid_group_label_get(void *data,
                      Evas_Object *obj __UNUSED__,
                      const char  *part)
{
   const Item *it = data;
   if (!part)
     return NULL;

   if (!strcmp(part, "elm.text"))
     return strdup(it->sound_name);

   return NULL;
}

static void
_on_sound_editor_del(void * data __UNUSED__,
                     Evas *e __UNUSED__,
                     Evas_Object *obj __UNUSED__,
                     void *event_info __UNUSED__)
{
   evas_object_smart_callback_call(ap.win, SIGNAL_SOUND_DEL, NULL);
}

static void
_grid_del(void *data,
          Evas_Object *obj __UNUSED__)
{
   Item *it = data;
   eina_stringshare_del(it->sound_name);
   eina_stringshare_del(it->format);
   free(it);
}

Eina_Stringshare *
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
_grid_sel_sample(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   Sound_Editor *edit = (Sound_Editor *)data;
   Selected_Sound_Data *snd_data = mem_calloc(1, sizeof(Selected_Sound_Data));

   assert(edit != NULL);

   snd_data->markup = edit->markup;
   snd_data->gengrid = edit->gengrid;
   snd_data->sound_type = SOUND_TYPE_SAMPLE;

   evas_object_smart_callback_call(ap.win, SIGNAL_SOUND_ADD, snd_data);
}

static void
_grid_sel_tone(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   Sound_Editor *edit = (Sound_Editor *)data;
   Selected_Sound_Data *snd_data = mem_calloc(1, sizeof(Selected_Sound_Data));

   assert(edit != NULL);

   snd_data->markup = edit->markup;
   snd_data->gengrid = edit->gengrid;
   snd_data->sound_type = SOUND_TYPE_TONE;
   snd_data->tone = edit->tone;

   evas_object_smart_callback_call(ap.win, SIGNAL_SOUND_ADD, snd_data);
}

static void
_gengrid_content_fill(Sound_Editor *edit)
{
   Eina_List *sounds, *tones, *l;
   Item *it;
   External_Resource *res;

   assert(edit != NULL);
   assert(ap.project != NULL);

   sounds = ap.project->sounds;
   tones = ap.project->tones;

   if (edit->mode != SOUND_EDITOR_TONE_SELECT)
     {
        it = (Item *)mem_calloc(1, sizeof(Item));

        it->sound_name = eina_stringshare_add(_("<b>Sound Samples</b>"));
        elm_gengrid_item_append(edit->gengrid, ggic, it, NULL, NULL);

        if (sounds)
          {
             EINA_LIST_FOREACH(sounds, l, res)
               {
                  it = (Item *)mem_calloc(1, sizeof(Item));
                  it->sound_name = eina_stringshare_add(res->name);
                  it->format = _sound_format_get(res->source);
                  it->comp = edje_edit_sound_compression_type_get(ap.project->global_object, it->sound_name);
                  it->rate = edje_edit_sound_compression_rate_get(ap.project->global_object, it->sound_name);
                  it->src = eina_stringshare_add(res->source);
                  elm_gengrid_item_append(edit->gengrid, gic, it, _grid_sel_sample, edit);
               }
          }
     }

   if (edit->mode != SOUND_EDITOR_SAMPLE_SELECT)
     {
        it = (Item *)mem_calloc(1, sizeof(Item));
        it->sound_name = eina_stringshare_add(_("<b>Sound Tones</b>"));
        edit->tone = elm_gengrid_item_append(edit->gengrid, ggic, it, NULL, NULL);

        if (tones)
          {
             EINA_LIST_FOREACH(tones, l, res)
               {
                  it = (Item *)mem_calloc(1, sizeof(Item));
                  it->sound_name = eina_stringshare_add(res->name);
                  it->tone_frq = edje_edit_sound_tone_frequency_get(ap.project->global_object, it->sound_name);
                  it->format = eina_stringshare_printf("%d", it->tone_frq);
                  elm_gengrid_item_append(edit->gengrid, gic, it, _grid_sel_tone, edit);
               }
          }
     }
}

static void
_create_gengrid(Evas_Object *parent,
                Sound_Editor *editor)
{
   assert(parent != NULL);
   assert(editor != NULL);

   editor->gengrid = elm_gengrid_add(parent);
   elm_gengrid_item_size_set(editor->gengrid, ITEM_WIDTH, ITEM_HEIGHT);
   elm_gengrid_group_item_size_set(editor->gengrid, ITEM_HEIGHT/5, ITEM_HEIGHT/5);
   elm_gengrid_align_set(editor->gengrid, 0.0, 0.0);
   elm_scroller_policy_set(editor->gengrid, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);

   elm_gengrid_multi_select_set(editor->gengrid, false);

   evas_object_size_hint_weight_set(editor->gengrid, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(editor->gengrid, EVAS_HINT_FILL, EVAS_HINT_FILL);

   gic = elm_gengrid_item_class_new();
   gic->item_style = "sound_editor";
   gic->func.text_get = _grid_label_get;
   gic->func.content_get = NULL;
   gic->func.del = _grid_del;

   ggic = elm_gengrid_item_class_new();
   ggic->item_style = "group_index";
   ggic->func.text_get = _grid_group_label_get;
   ggic->func.content_get = NULL;
   ggic->func.del = _grid_del;

   _gengrid_content_fill(editor);

   evas_object_show(editor->gengrid);
   elm_object_part_content_set(editor->markup, "gengrid", editor->gengrid);
}

static inline Evas_Object *
_sound_editor_search_field_create(Evas_Object *parent)
{
   Evas_Object *entry, *icon;

   assert(parent != NULL);

   ENTRY_ADD(parent, entry, true);
   elm_object_part_text_set(entry, "guide", _("Search"));
   ICON_STANDARD_ADD(entry, icon, true, "search");
   elm_object_part_content_set(entry, "elm.swallow.end", icon);
   return entry;
}

static void
_fs_del(void *data)
{
   Elm_Object_Item *it;
   Sound_Editor *edit = (Sound_Editor *)data;

   assert(edit != NULL);

   if (edit->fs_win)
     {
        evas_object_del(edit->fs_win);
        edit->fs_win = NULL;
        it = elm_gengrid_item_prev_get(edit->tone);
        elm_gengrid_item_selected_set(it, true);
     }
}

static void
_add_sample_done(void *data,
                 Evas_Object *obj,
                 void *event_info)
{
   Item *it;
   Eina_Stringshare *sound_name;
   Eina_List *samples_list, *l;
   Eina_Bool exist = false;
   External_Resource *res;
   const char *file_name;

   const char *selected = event_info;
   Sound_Editor *edit = (Sound_Editor *)data;

   assert(edit != NULL);

   if ((!selected) || (!strcmp(selected, "")))
     goto del;

   samples_list = ap.project->sounds;

   if ((ecore_file_exists(selected)) && (!ecore_file_is_dir(selected)))
     {
        file_name = ecore_file_file_get(selected);
        sound_name = eina_stringshare_add(file_name);
        EINA_LIST_FOREACH(samples_list, l, res)
           if (res->name == sound_name) /* they both are stringshares */
             {
                exist = true;
                break;
             }
        if (exist)
          {
             WIN_NOTIFY_WARNING(obj, _("Sample with this name is already added to project"))
                eina_stringshare_del(sound_name);
             return;
          }

        res = mem_calloc(1, sizeof(External_Resource));
        res->name = eina_stringshare_add(file_name);
        res->source = eina_stringshare_printf("%s/images/%s", ap.project->develop_path, file_name);

        if (!ecore_file_exists(res->source))
          {
             ecore_file_cp(selected, res->source);

             ap.project->sounds = eina_list_sorted_insert(ap.project->sounds, (Eina_Compare_Cb) resource_cmp, res);
          }
        else
          {
             WIN_NOTIFY_ERROR(obj, _("File exist"));
             free(res);
             goto del;
          }

        edje_edit_sound_sample_add(ap.project->global_object, res->name, res->source);

        it = (Item *)mem_calloc(1, sizeof(Item));
        it->sound_name = eina_stringshare_add(sound_name);
        it->format = _sound_format_get(selected);
        it->comp = EDJE_EDIT_SOUND_COMP_RAW;
        it->src = eina_stringshare_add(res->source);
        if (edit->mode != SOUND_EDITOR_SAMPLE_SELECT)
          elm_gengrid_item_insert_before(edit->gengrid, gic, it, edit->tone,
                                         _grid_sel_sample, edit);
        else
          elm_gengrid_item_append(edit->gengrid, gic, it, _grid_sel_sample, edit);

        editor_save(ap.project->global_object);
        TODO("Remove this line once edje_edit_sound_sample_add would be added into Editor Module and saving would work properly")
        ap.project->changed = true;
     }
   else
     WIN_NOTIFY_ERROR(obj, _("Error while loading file.<br>File is not exist"));

del:
   ecore_job_add(_fs_del, edit);
}

static void
_popup_close(void *data)
{
   Sound_Editor *edit = (Sound_Editor *)data;

   assert(edit != NULL);
   assert(edit->tone_validator != NULL);
   assert(edit->frq_validator != NULL);

   evas_object_del(edit->popup);

   elm_validator_regexp_free(edit->tone_validator);
   elm_validator_regexp_free(edit->frq_validator);
   edit->tone_validator = NULL;
   edit->frq_validator = NULL;

   elm_gengrid_item_selected_set(elm_gengrid_last_item_get(edit->gengrid), true);
}

static void
_add_tone_cancel(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   Sound_Editor *edit = (Sound_Editor *)data;

   ecore_job_add(_popup_close, edit);
}

static void
_add_tone_done(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   Item *it;
   Eina_Stringshare *tone_name;
   const char *str_value;
   int frq;
   Eina_List *tones_list, *l;
   Eina_Bool exist = false;
   Tone_Resource *tone;

   Sound_Editor *edit = (Sound_Editor *)data;

   assert(edit != NULL);

   tone_name = eina_stringshare_add(elm_entry_entry_get(edit->tone_entry));

   tones_list = ap.project->tones;
   EINA_LIST_FOREACH(tones_list, l, tone)
     if (tone->name == tone_name) /* they both are stringshares */
       {
          exist = true;
          break;
       }
   if (exist)
     {
        NOTIFY_WARNING(_("Tone with this name is already added to project"))
        eina_stringshare_del(tone_name);
        return;
     }

   str_value = elm_entry_entry_get(edit->frq_entry);
   frq = atoi(str_value);

   it = (Item *)mem_calloc(1, sizeof(Item));
   it->sound_name = eina_stringshare_add(tone_name);
   it->tone_frq = frq;
   it->format = eina_stringshare_printf("%d", it->tone_frq);
   elm_gengrid_item_append(edit->gengrid, gic, it, _grid_sel_tone, edit);

   tone = mem_calloc(1, sizeof(External_Resource));
   tone->name = eina_stringshare_add(tone_name);
   tone->freq = frq;
   ap.project->tones = eina_list_sorted_insert(ap.project->tones, (Eina_Compare_Cb) resource_cmp, tone);

   edje_edit_sound_tone_add(ap.project->global_object, tone_name, frq);
   editor_save(ap.project->global_object);
   TODO("Remove this line once edje_edit_image_add would be added into Editor Module and saving would work properly")
   ap.project->changed = true;

   ecore_job_add(_popup_close, edit);
}

static Eina_Bool
_samples_filter(const char *path,
                Eina_Bool dir,
                void *data __UNUSED__)
{
   if (dir) return true;

   if (eina_str_has_extension(path, "wav") || eina_str_has_extension(path, "ogg") ||
       eina_str_has_extension(path, "flac"))
     return true;
   return false;
}

static void
_sample_add_cb(void *data,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   Evas_Object *fs, *ic;
   Sound_Editor *edit = data;

   assert(edit != NULL);

   edit->fs_win  = mw_add(NULL, NULL, NULL);

   assert(edit->fs_win != NULL);

   mw_title_set(edit->fs_win, "Add sound to the library");
   ic = elm_icon_add(edit->fs_win);
   elm_icon_standard_set(ic, "folder");
   mw_icon_set(edit->fs_win, ic);
   evas_object_show(edit->fs_win);

   FILESELECTOR_ADD(fs, edit->fs_win, _add_sample_done, data);
   elm_fileselector_custom_filter_append(fs, _samples_filter, NULL, _("Sound files"));
   elm_fileselector_mime_types_filter_append(fs, "*", _("All files"));
   elm_win_inwin_content_set(edit->fs_win, fs);
}

static void
_validation(void *data __UNUSED__,
            Evas_Object *obj __UNUSED__,
            void *event_info __UNUSED__)
{
   Sound_Editor *edit = (Sound_Editor *)data;

   if ((elm_validator_regexp_status_get(edit->tone_validator) != ELM_REG_NOERROR) ||
       (elm_validator_regexp_status_get(edit->frq_validator) != ELM_REG_NOERROR))
     elm_object_disabled_set(edit->popup_btn_add, true);
   else
     elm_object_disabled_set(edit->popup_btn_add, false);
}

static void
_tone_add_cb(void *data,
             Evas_Object *obj __UNUSED__,
             void *event_info __UNUSED__)
{
   Sound_Editor *edit = data;

   assert(edit != NULL);

   Evas_Object *popup, *box, *item, *bt_no;
   Eina_Stringshare *title;

   popup = elm_popup_add(ap.win);
   title = eina_stringshare_add(_("Add new tone to the project"));
   elm_object_part_text_set(popup, "title,text", title);
   elm_popup_orient_set(popup, ELM_POPUP_ORIENT_CENTER);
   edit->popup = popup;

   assert(edit->tone_validator == NULL);
   assert(edit->frq_validator == NULL);
   edit->tone_validator = elm_validator_regexp_new(NAME_REGEX, NULL);
   edit->frq_validator = elm_validator_regexp_new(FREQUENCY_REGEX, NULL);

   BOX_ADD(popup, box, false, false);
   elm_object_content_set(popup, box);
   LAYOUT_PROP_ADD(box, _("Tone name:"), "property", "1swallow")
   ENTRY_ADD(item, edit->tone_entry, true);
   //eo_do(edit->tone_entry, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, edit->tone_validator));
   evas_object_smart_callback_add(edit->tone_entry, "changed", _validation, edit);
   elm_object_part_text_set(edit->tone_entry, "guide", _("Type a new tone name"));
   /* need to manualy set not valid string for triggered validator */
   elm_entry_entry_set(edit->tone_entry, NULL);
   elm_object_part_content_set(item, "elm.swallow.content", edit->tone_entry);
   elm_box_pack_end(box, item);

   LAYOUT_PROP_ADD(box, _("Frequency:"), "property", "1swallow")
   ENTRY_ADD(item, edit->frq_entry, true);
   //eo_do(edit->frq_entry, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, edit->frq_validator));
   evas_object_smart_callback_add(edit->frq_entry, "changed", _validation, edit);
   elm_object_part_text_set(edit->frq_entry, "guide", _("Type a frequency (20 - 20000)"));
   /* need to manualy set not valid string for triggered validator */
   elm_entry_entry_set(edit->frq_entry, NULL);
   elm_object_part_content_set(item, "elm.swallow.content", edit->frq_entry);
   elm_box_pack_end(box, item);

   BUTTON_ADD(popup, edit->popup_btn_add, _("Add"));
   evas_object_smart_callback_add (edit->popup_btn_add, "clicked", _add_tone_done, edit);
   elm_object_part_content_set(popup, "button1", edit->popup_btn_add);
   elm_object_disabled_set(edit->popup_btn_add, true);

   BUTTON_ADD(popup, bt_no, _("Cancel"));
   evas_object_smart_callback_add (bt_no, "clicked", _add_tone_cancel, edit);
   elm_object_part_content_set(popup, "button2", bt_no);

   evas_object_show(popup);
   eina_stringshare_del(title);
}

#undef INFO_ADD

static void
_on_cmb_sel(void *data,
            Evas_Object *obj __UNUSED__,
            void *event_info)
{
   Ewe_Combobox_Item *selected_item = event_info;

   if (!selected_item->index)
     _sample_add_cb(data, NULL, NULL);
   else
     _tone_add_cb(data, NULL, NULL);
}

static void
_on_delete_clicked_cb(void *data,
                      Evas_Object *obj __UNUSED__,
                      void *event_info __UNUSED__)
{
   Elm_Object_Item *grid_it;
   Item *item;
   Eina_List *list, *l, *l_next;
   Sound_Editor *edit = (Sound_Editor *)data;
   External_Resource *res;
   char buf[BUFF_MAX];
   int symbs = 0, notdeleted = 0, selected = 0;

   assert(edit != NULL);

   list = (Eina_List *)elm_gengrid_selected_items_get(edit->gengrid);
   selected = eina_list_count(list);

   if ((!list) || (!selected))
     {
        NOTIFY_WARNING("No selected items");
        return;
     }

   snprintf(buf, BUFF_MAX, _("Unable to delete: "));
   symbs = strlen(buf);
   EINA_LIST_FOREACH_SAFE(list, l, l_next, grid_it)
     {
        item = elm_object_item_data_get(grid_it);

        if (!item->tone_frq)
          {
             res = pm_resource_get(ap.project->sounds, item->sound_name);
             if (!res->used_in)
               {
                  edje_edit_sound_sample_del(ap.project->global_object, item->sound_name);
                  ap.project->sounds = pm_resource_del(ap.project->sounds, res);
                  elm_object_item_del(grid_it);
               }
             else if ((res->used_in) && (notdeleted < 4))
               {
                  snprintf(buf + symbs, BUFF_MAX - symbs, _("<br>Sound \"%s\""), res->name);
                  symbs += strlen(res->name);
                  notdeleted++;
               }
          }
        else
          {
             res = pm_resource_get(ap.project->tones, item->sound_name);
             if (!res->used_in)
               {
                  edje_edit_sound_tone_del(ap.project->global_object, item->sound_name);
                  ap.project->tones = pm_resource_del(ap.project->tones, res);
                  elm_object_item_del(grid_it);
               }
             else if ((res->used_in) && (notdeleted < 4))
               {
                  snprintf(buf + symbs, BUFF_MAX - symbs, _("<br>Tone \"%s\""), res->name);
                  symbs += strlen(res->name);
                  notdeleted++;
               }
          }
     }

   if (notdeleted >= 4)
     snprintf(buf + symbs, BUFF_MAX - symbs, "<br>...");

   if (notdeleted != 0)
     NOTIFY_WARNING("%s<br>Used in Programs", buf);

   editor_save(ap.project->global_object);
   TODO("Remove this line once edje_edit_sound_..._del would be added into Editor Modulei and saving would work properly")
   ap.project->changed = true;

}

ITEM_SEARCH_FUNC(gengrid, ELM_GENGRID_ITEM_SCROLLTO_MIDDLE, "elm.label")

static void
_search_changed(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   Sound_Editor *edit = data;

   assert(edit != NULL);

   _gengrid_item_search(edit->gengrid, &(edit->sound_search_data),
                        edit->sound_search_data.last_item_found);

}

static void
_search_nxt_gd_item(void *data __UNUSED__,
                    Evas_Object *obj __UNUSED__,
                    void *event_info __UNUSED__)
{
   Sound_Editor *edit = data;
   Elm_Object_Item *start_from = NULL;

   assert(edit != NULL);

   if (edit->sound_search_data.last_item_found)
     {
        start_from =
           elm_gengrid_item_next_get(edit->sound_search_data.last_item_found);
     }

   _gengrid_item_search(edit->gengrid, &(edit->sound_search_data), start_from);
}

static void
_search_reset_cb(void *data __UNUSED__,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   Search_Data *search_data = data;

   assert(search_data != NULL);

   search_data->last_item_found = NULL;
}

static void
_sound_editor_main_markup_create(Sound_Editor *edit)
{
   Evas_Object *btn, *ic, *search;

   assert(edit != NULL);

   edit->markup = elm_layout_add(ap.win);
   elm_layout_theme_set(edit->markup, "layout", "sound_editor", "default");
   evas_object_size_hint_weight_set(edit->markup, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_data_set(edit->markup, SND_EDIT_KEY, edit);

   evas_object_event_callback_add(edit->markup, EVAS_CALLBACK_DEL, _on_sound_editor_del, edit);

   btn = elm_button_add(edit->markup);
   evas_object_smart_callback_add(btn, "clicked", _on_delete_clicked_cb, edit);
   elm_object_part_content_set(edit->markup, "swallow.btn.del", btn);

   ic = elm_icon_add(btn);
   elm_icon_standard_set(ic, "minus");
   elm_object_part_content_set(btn, NULL, ic);

   switch (edit->mode)
     {
      case SOUND_EDITOR_EDIT:
        {
           EWE_COMBOBOX_ADD(edit->markup, edit->add_cmb);
           ewe_combobox_style_set(edit->add_cmb, "small");
           ewe_combobox_item_add(edit->add_cmb, _("Sample"));
           ewe_combobox_item_add(edit->add_cmb, _("Tone"));
           evas_object_smart_callback_add(edit->add_cmb, "selected", _on_cmb_sel, edit);
           elm_object_part_content_set(edit->markup, "swallow.btn.add", edit->add_cmb);
        }
      break;
      case SOUND_EDITOR_SAMPLE_SELECT:
        {
           btn = elm_button_add(edit->markup);
           evas_object_smart_callback_add(btn, "clicked", _sample_add_cb, edit);
           evas_object_show(btn);
           elm_object_part_content_set(edit->markup, "swallow.btn.add", btn);

           ic = elm_icon_add(btn);
           elm_icon_standard_set(ic, "plus");
           elm_object_part_content_set(btn, NULL, ic);
        }
      break;
      case SOUND_EDITOR_TONE_SELECT:
        {
           btn = elm_button_add(edit->markup);
           evas_object_smart_callback_add(btn, "clicked", _tone_add_cb, edit);
           evas_object_show(btn);
           elm_object_part_content_set(edit->markup, "swallow.btn.add", btn);

           ic = elm_icon_add(btn);
           elm_icon_standard_set(ic, "plus");
           elm_object_part_content_set(btn, NULL, ic);
        }
      break;
     }
   search = _sound_editor_search_field_create(edit->markup);
   evas_object_hide(search);
   elm_layout_content_set(edit->markup, "swallow.search_area", search);
   evas_object_smart_callback_add(search, "changed", _search_changed, edit);
   evas_object_smart_callback_add(search, "activated", _search_nxt_gd_item, edit);
   evas_object_smart_callback_add(edit->gengrid, "pressed", _search_reset_cb,
                                  &(edit->sound_search_data));
   edit->sound_search_data.search_entry = search;
   edit->sound_search_data.last_item_found = NULL;
}

Evas_Object *
sound_editor_window_add(Sound_Editor_Mode mode)
{
   Sound_Editor *edit;

   assert(ap.project != NULL);

#ifdef HAVE_AUDIO
   ecore_audio_init();
#endif

   edit = (Sound_Editor *)mem_calloc(1, sizeof(Sound_Editor));
   edit->mode = mode;

   _sound_editor_main_markup_create(edit);

   _create_gengrid(edit->markup, edit);

   elm_object_focus_set(edit->sound_search_data.search_entry, true);

   return edit->markup;
}

void sound_editor_added_sounds_free(Eina_List *add_snd)
{
   Sound *data;

   EINA_LIST_FREE(add_snd, data)
     {
        eina_stringshare_del(data->name);
        eina_stringshare_del(data->src);
        free(data);
     }
   add_snd = NULL;
}

Eina_Bool
sound_editor_file_choose(Evas_Object *win, const char *selected)
{
   Elm_Object_Item *grid_item;
   Sound_Editor *snd_edit;

   if (!selected) return false;

   assert(win != NULL);

   snd_edit = evas_object_data_get(win, SND_EDIT_KEY);

   assert(snd_edit != NULL);

   grid_item = elm_gengrid_search_by_text_item_get(snd_edit->gengrid, NULL,
                                                   "elm.text", selected, 0);
   if (grid_item)
     {
        elm_gengrid_item_selected_set(grid_item, true);
        elm_gengrid_item_bring_in(grid_item, ELM_GENGRID_ITEM_SCROLLTO_MIDDLE);
        return true;
     }
   return false;
}

#undef ITEM_WIDTH
#undef ITEM_HEIGHT

