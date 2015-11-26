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

#include "style_editor.h"
#include "main_window.h"
#include "alloc.h"
#include "editor.h"

TODO("Rename this file to textblock_style_manager")

#define FONT_DEFAULT "DEFAULT='align=middle font=Sans font_size=24 color=#000000 "
#define DIRECTION_NUM 39
#define DEFAULT_DIRECTION 2
#define WHITE_COLOR "#FFF"

typedef struct _Style_Tag_Entries Style_Tag_Entries;
typedef struct _Style_entries Style_Entries;
typedef struct _Style_Editor Style_Editor;
typedef struct _Search_Data Search_Data;

struct _Search_Data
{
   Evas_Object *search_entry;
   Elm_Object_Item *last_item_found;
};

struct _Style_Editor
{
   Evas_Object *glist;
   Evas_Object *textblock_style;
   Evas_Object *entry_prev;
   Search_Data style_search_data;
   struct {
      const char *st_name;
      const char *st_tag;
      Eina_Stringshare *stvalue;
   } current_style;
   struct {
      Evas_Object *name;
      Elm_Validator_Regexp *name_validator;
      Evas_Object *btn_add;
      Evas_Object *dialog;
   } popup;
   Elm_Object_Item *tag;
};

struct _Style_Entries
{
   Evas_Object *style_name;
   Evas_Object *default_tags;
};

struct _Style_Tag_Entries
{
   Elm_Object_Item *style_name;
   Evas_Object *tag_name;
   Evas_Object *tag_value;
};

#define POPUP style_edit->popup
#define CURRENT style_edit->current_style

static Elm_Genlist_Item_Class *_itc_style = NULL;
static Elm_Genlist_Item_Class *_itc_tags = NULL;

static void
_on_popup_bt_cancel(void *data,
                    Evas_Object *obj __UNUSED__,
                    void *ei __UNUSED__)
{
   Style_Editor *style_edit = (Style_Editor *)data;

   assert(style_edit != NULL);
   assert(POPUP.name_validator != NULL);
   elm_validator_regexp_free(POPUP.name_validator);
   POPUP.name_validator = NULL;

   evas_object_del(POPUP.dialog);
   POPUP.dialog = NULL;
}

static void
_on_glit_selected(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info)
{
   Eina_List *tags = NULL;
   Eina_List *l = NULL;
   Evas_Object *edje_edit_obj = NULL;

   const char *style_name = NULL;
   const char *tag, *value = NULL;
   Evas_Textblock_Style *ts = NULL;

   Style_Editor *style_edit = (Style_Editor *)data;
   assert(style_edit != NULL);

   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;

   Eina_Strbuf *style = eina_strbuf_new();
   eina_strbuf_append(style, FONT_DEFAULT);

   Elm_Object_Item *glit_parent = elm_genlist_item_parent_get(glit);

   Style_Data *current_style = (Style_Data *)mem_malloc(sizeof(Style_Data));
   memset(current_style, 0x0, sizeof(Style_Data));

   edje_edit_obj = ap.project->global_object;

   if (!glit_parent)
     {
        style_name = (char *)elm_object_item_data_get(glit);
        tags = edje_edit_style_tags_list_get(edje_edit_obj, style_name);

        EINA_LIST_FOREACH(tags, l, tag)
          {
             value = edje_edit_style_tag_value_get(edje_edit_obj, style_name,
                                                   tag);
             eina_strbuf_append(style, value);
          }
        eina_list_free(tags);
     }
   else
     {
        style_name = elm_object_item_data_get(glit_parent);
        tag = (char *)elm_object_item_data_get(glit);
        value = edje_edit_style_tag_value_get(edje_edit_obj, style_name, tag);
        eina_strbuf_append(style, value);
     }

   current_style->st_name = style_name;
   current_style->st_tag = tag;
   current_style->stvalue = eina_stringshare_add(value);
   current_style->style = style;
   current_style->textblock_style = style_edit->textblock_style;
   if (!elm_genlist_item_parent_get(glit))
     {
        elm_object_signal_emit(style_edit->entry_prev, "entry,hide", "eflete");
        current_style->is_parent_item = EINA_TRUE;
     }
   else
     {
        elm_object_signal_emit(style_edit->entry_prev, "entry,show", "eflete");
        eina_strbuf_append(style, "'");
        ts = evas_textblock_style_new();
        evas_textblock_style_set(ts, eina_strbuf_string_get(style));
        evas_object_textblock_style_set(style_edit->textblock_style, ts);
        evas_object_size_hint_max_set(style_edit->textblock_style, EVAS_HINT_FILL,
                                      EVAS_HINT_FILL);
        evas_textblock_style_free(ts);
     }
   evas_object_smart_callback_call(ap.win, SIGNAL_STYLE_SELECTED, current_style);
}

static void
_on_st_add_bt_ok(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *ei __UNUSED__)
{
   Elm_Object_Item *glit_style = NULL;
   Evas_Object *edje_edit_obj = NULL;
   Style_Editor *style_edit = (Style_Editor *)data;
   Resource *res;

   assert(style_edit != NULL);

   const char *style_name = elm_entry_entry_get(POPUP.name);

   edje_edit_obj = ap.project->global_object;

   if ((!style_name) || (strcmp(style_name, "") == 0))
     {
        NOTIFY_WARNING(_("Style name can not be empty!"));
        return;
     }
   if (!edje_edit_style_add(edje_edit_obj, style_name))
     {
        NOTIFY_WARNING(_("Style name must be unique!"));
        return;
     }
   if (edje_edit_style_tag_add(edje_edit_obj, style_name, "DEFAULT"))
     {
        if (!edje_edit_style_tag_value_set(edje_edit_obj,
                                           style_name,
                                          "DEFAULT",
                                          "align=middle font=Sans font_size=24 color=#000000FF"))
          {
             NOTIFY_WARNING(_("Failed to add tag value. Tag will be deleted"));
             edje_edit_style_tag_del(edje_edit_obj, style_name, "DEFAULT");
             return;
          }
     }
   else
     {
        NOTIFY_WARNING(_("Failed to add tag."));
        return;
     }

   glit_style = elm_genlist_item_append(style_edit->glist, _itc_style,
                                        style_name, NULL, ELM_GENLIST_ITEM_NONE,
                                        _on_glit_selected, style_edit);
   elm_object_item_data_set(glit_style, (char *)style_name);

   elm_genlist_item_append(style_edit->glist, _itc_tags,
                           "DEFAULT", glit_style, ELM_GENLIST_ITEM_NONE,
                           _on_glit_selected, style_edit);

   TODO("Need refactoring after callback logic for modal window implementation")

   elm_genlist_item_selected_set(glit_style, true);
   elm_genlist_item_bring_in(glit_style, ELM_GENLIST_ITEM_SCROLLTO_IN);
   elm_genlist_item_show(glit_style, ELM_GENLIST_ITEM_SCROLLTO_IN);

   res = mem_calloc(1, sizeof(Resource));
   res->name = eina_stringshare_add(style_name);
   ap.project->styles = eina_list_sorted_insert(ap.project->styles,
                                                (Eina_Compare_Cb) resource_cmp,
                                                res);

   _on_popup_bt_cancel(style_edit, NULL, NULL);

   editor_save(ap.project->global_object);
   TODO("Remove this line once edje_edit API would be added into Editor Module and saving would work properly")
   ap.project->changed = true;
}

static void
_on_tag_add_bt_ok(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *ei __UNUSED__)
{
   Style_Editor *style_edit = (Style_Editor *)data;
   Elm_Object_Item *glit_tag = NULL;
   Evas_Object *edje_edit_obj = NULL;

   assert(style_edit != NULL);

   const char *style_name = elm_object_item_data_get(style_edit->tag);
   const char *tag_name = elm_entry_entry_get(POPUP.name);
   edje_edit_obj = ap.project->global_object;

   if ((!tag_name) || (strcmp(tag_name, "") == 0))
     {
        NOTIFY_WARNING(_("Tag name can not be empty!"));
        return;
     }
   if (!edje_edit_style_tag_add(edje_edit_obj, style_name, tag_name))
     {
        NOTIFY_WARNING(_("Tag name must be unique!"));
        return;
     }
   else
     if (!edje_edit_style_tag_value_set(edje_edit_obj, style_name, tag_name, ""))
       {
          NOTIFY_WARNING(_("Failed to add tag value. Tag will be deleted"));
          edje_edit_style_tag_del(edje_edit_obj, style_name, tag_name);
          return;
       }
   glit_tag = elm_genlist_item_append(style_edit->glist, _itc_tags,
                                      tag_name, style_edit->tag,
                                      ELM_GENLIST_ITEM_NONE,
                                      _on_glit_selected, style_edit);
   elm_object_item_data_set(glit_tag,(char *)tag_name);
   elm_genlist_item_selected_set(glit_tag, true);
   elm_genlist_item_show(style_edit->tag, ELM_GENLIST_ITEM_SCROLLTO_IN);
   elm_genlist_item_bring_in(glit_tag, ELM_GENLIST_ITEM_SCROLLTO_MIDDLE);

   _on_popup_bt_cancel(style_edit, NULL, NULL);

   editor_save(ap.project->global_object);
   TODO("Remove this line once edje_edit API would be added into Editor Module and saving would work properly")
   ap.project->changed = true;
}

static void
_validate(void *data,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   Style_Editor *style_edit = (Style_Editor *)data;

   if (elm_validator_regexp_status_get(POPUP.name_validator) != ELM_REG_NOERROR)
     elm_object_disabled_set(POPUP.btn_add, true);
   else
     elm_object_disabled_set(POPUP.btn_add, false);
}

static void
_on_bt_style_add(Style_Editor *style_edit)
{
   Evas_Object *box, *item, *button;

   assert(style_edit != NULL);

   POPUP.dialog = elm_popup_add(ap.win);
   elm_object_part_text_set(POPUP.dialog, "title,text", _("Add textblock style"));

   BOX_ADD(POPUP.dialog, box, false, false);

   LAYOUT_PROP_ADD(box, _("Style name:"), "property", "1swallow")
   POPUP.name_validator = elm_validator_regexp_new(NAME_REGEX, NULL);
   ENTRY_ADD(item, POPUP.name, true);
   eo_do(POPUP.name, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, POPUP.name_validator));
   evas_object_smart_callback_add(POPUP.name, "changed", _validate, style_edit);
   elm_object_part_text_set(POPUP.name, "guide", _("Type a new style name"));
   elm_object_part_content_set(item, "elm.swallow.content", POPUP.name);
   elm_box_pack_end(box, item);

   elm_object_content_set(POPUP.dialog, box);
   evas_object_show(box);

   BUTTON_ADD(POPUP.dialog, POPUP.btn_add, _("Ok"));
   evas_object_smart_callback_add(POPUP.btn_add, "clicked", _on_st_add_bt_ok,  style_edit);
   elm_object_part_content_set(POPUP.dialog, "button1", POPUP.btn_add);
   elm_object_disabled_set(POPUP.btn_add, true);

   BUTTON_ADD(POPUP.dialog, button, _("Cancel"));
   evas_object_smart_callback_add(button, "clicked", _on_popup_bt_cancel, style_edit);
   elm_object_part_content_set(POPUP.dialog, "button2", button);

   evas_object_show(POPUP.dialog);
}

static void
_on_bt_tag_add(Style_Editor *style_edit)
{
   Evas_Object *box, *item, *button;

   assert(style_edit != NULL);
   assert(POPUP.name_validator == NULL);

   Elm_Object_Item *glit = elm_genlist_selected_item_get(style_edit->glist);
   Elm_Object_Item *glit_parent = elm_genlist_item_parent_get(glit);
   const char *style_name;
   Eina_Stringshare *buf;

   if (!glit)
     {
         NOTIFY_WARNING(_("Select a style!"));
         return;
     }

   if (!glit_parent)
     {
         style_name = elm_object_item_data_get(glit);
         style_edit->tag = glit;
     }
   else
     {
         style_name = elm_object_item_data_get(glit_parent);
         style_edit->tag = glit_parent;
     }

   POPUP.dialog = elm_popup_add(ap.win);
   buf = eina_stringshare_printf(_("Add tag to style: %s"), style_name);
   elm_object_part_text_set(POPUP.dialog, "title,text", buf);

   BOX_ADD(POPUP.dialog, box, false, false);

   LAYOUT_PROP_ADD(box, "Tag name:", "property", "1swallow")
   POPUP.name_validator = elm_validator_regexp_new(NAME_REGEX, NULL);
   ENTRY_ADD(item, POPUP.name, true);
   eo_do(POPUP.name, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, POPUP.name_validator));
   evas_object_smart_callback_add(POPUP.name, "changed", _validate, style_edit);
   elm_object_part_text_set(POPUP.name, "guide", _("Type a new tag name."));
   elm_object_part_content_set(item, "elm.swallow.content", POPUP.name);
   elm_box_pack_end(box, item);

   elm_object_content_set(POPUP.dialog, box);
   evas_object_show(box);

   BUTTON_ADD(POPUP.dialog, POPUP.btn_add, _("Ok"));
   evas_object_smart_callback_add(POPUP.btn_add, "clicked", _on_tag_add_bt_ok, style_edit);
   elm_object_part_content_set(POPUP.dialog, "button1", POPUP.btn_add);
   elm_object_disabled_set(POPUP.btn_add, true);

   BUTTON_ADD(POPUP.dialog, button, _("Cancel"));
   evas_object_smart_callback_add(button, "clicked", _on_popup_bt_cancel, style_edit);
   elm_object_part_content_set(POPUP.dialog, "button2", button);

   evas_object_show(POPUP.dialog);
   eina_stringshare_del(buf);
}

static void
_on_bt_del(void *data,
           Evas_Object *obj __UNUSED__,
           void *event_info __UNUSED__)
{
   Evas_Object *edje_edit_obj = NULL;
   const char *style_name, *tag;

   Style_Editor *style_edit = (Style_Editor *)data;
   State *state;
   char buf[BUFF_MAX];
   int symbs = 0;
   Eina_List *l;
   Resource *res;

   assert(style_edit != NULL);

   Elm_Object_Item *glit = elm_genlist_selected_item_get(style_edit->glist);
   if (!glit) return;
   Elm_Object_Item *glit_parent = elm_genlist_item_parent_get(glit);
   edje_edit_obj = ap.project->global_object;

   if (!glit_parent)
     {
        style_name = elm_object_item_part_text_get(glit, "elm.text");
        res = pm_resource_get(ap.project->styles, style_name);
        if (!res->used_in)
          {
             edje_edit_style_del(edje_edit_obj, style_name);
             ap.project->colorclasses = pm_resource_del(ap.project->colorclasses, res);
          }
        else
          {
             snprintf(buf, BUFF_MAX, _("Style is used in:"));
             symbs = strlen(buf);
             EINA_LIST_FOREACH(res->used_in, l, state)
               {
                  snprintf(buf + symbs, BUFF_MAX - symbs, _("<br>group: %s<br>part: %s<br>state: \"%s\" %2.1f"),
                           state->part->group->name, state->part->name, state->parsed_name, state->parsed_val);
                  symbs += strlen(res->name);
                  break; TODO("remove this break after warning style remake")
               }
             NOTIFY_WARNING("%s", buf);
             return;
          }
     }
   else
     {
        style_name = elm_object_item_part_text_get(glit_parent, "elm.text");
        tag = elm_object_item_part_text_get(glit, "elm.text");
        if (!strcmp(tag, "DEFAULT"))
          {
             NOTIFY_WARNING(_("DEFAULT tag cannot be deleted!"));
             return;
          }
        else
          edje_edit_style_tag_del(edje_edit_obj, style_name, tag);
     }

   elm_object_item_del(glit);

   editor_save(ap.project->global_object);
   TODO("Remove this line once edje_edit API would be added into Editor Module and saving would work properly")
   ap.project->changed = true;
}

/* For GenList, getting the content for showing. Tag Names. */
static char *
_item_tags_label_get(void *data,
                     Evas_Object *obj __UNUSED__,
                     const char *part __UNUSED__)
{
   char *tag_label = (char *)data;
   if (!tag_label)
     {
        ERR("It impossible, but it is occurred, tag's name is missing!");
        return NULL;
     }
   return strdup(tag_label);
}

/* For Style names. */
static char *
_item_style_label_get(void *data,
                      Evas_Object *obj __UNUSED__,
                      const char *part __UNUSED__)
{
   char *style_label = (char *)data;
   /* char *formated_style_name; */
   if (!style_label)
     {
        ERR("It impossible, but it is occurred, style's name is missing!");
        return NULL;
     }
   /* 6 - <bold> */
   /*
   formated_style_name = mem_malloc(7 + strlen(style_label));
   sprintf(formated_style_name, "%s%s", "<bold>", style_label);
   return formated_style_name;
   */
   return strdup(style_label);
}

static inline Evas_Object *
_style_editor_search_field_create(Evas_Object *parent)
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
_on_bt_add(void *data,
           Evas_Object *obj __UNUSED__,
           void *event_info)
{
   Style_Editor *style_edit = (Style_Editor *)data;

   assert(style_edit != NULL);

   Ewe_Combobox_Item *selected_item = event_info;

   if (!selected_item->index) _on_bt_style_add(style_edit);
   else _on_bt_tag_add(style_edit);
}

ITEM_SEARCH_FUNC(genlist,ELM_GENLIST_ITEM_SCROLLTO_MIDDLE, "elm.text")

static void
_search_changed(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   Style_Editor *style_edit = data;

   assert(style_edit != NULL);

   _genlist_item_search(style_edit->glist, &(style_edit->style_search_data),
                        style_edit->style_search_data.last_item_found);
}

static void
_search_nxt_gd_item(void *data,
                    Evas_Object *obj __UNUSED__,
                    void *event_info __UNUSED__)
{
   Style_Editor *style_edit = data;
   Elm_Object_Item *start_from = NULL;

   assert(style_edit != NULL);

   if (style_edit->style_search_data.last_item_found)
     {
        start_from =
           elm_genlist_item_next_get(style_edit->style_search_data.last_item_found);
     }

   _genlist_item_search(style_edit->glist, &(style_edit->style_search_data), start_from);
}

static void
_search_reset_cb(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   Search_Data *search_data = data;

   assert(search_data != NULL);

   search_data->last_item_found = NULL;
}

static void
_expand_request_cb(void *data __UNUSED__,
                   Evas_Object *o __UNUSED__,
                   void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_TRUE);
}

static void
_expanded_cb(void *data,
             Evas_Object *o __UNUSED__,
             void *event_info)
{
   Style_Editor *style_edit = (Style_Editor *)data;
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;
   const char *name = elm_object_item_data_get(glit);
   Eina_List *tags, *l_tg;
   Elm_Object_Item *glit_tag;
   char *tag;

   tags = edje_edit_style_tags_list_get(ap.project->global_object, name);
   EINA_LIST_FOREACH(tags, l_tg, tag)
     {
        glit_tag = elm_genlist_item_append(style_edit->glist, _itc_tags,
                                           tag, glit, ELM_GENLIST_ITEM_NONE,
                                           _on_glit_selected, style_edit);
        elm_object_item_data_set(glit_tag, tag);
     }
   eina_list_free(tags);
}

static void
_contract_request_cb(void *data __UNUSED__,
                     Evas_Object *o __UNUSED__,
                     void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_FALSE);
}

static void
_contracted_cb(void *data __UNUSED__,
               Evas_Object *o __UNUSED__,
               void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
_on_clicked_double(void *data __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;

   if (elm_genlist_item_type_get(glit) == ELM_GENLIST_ITEM_TREE)
     {
        elm_genlist_item_expanded_set(glit, !elm_genlist_item_expanded_get(glit));
     }
}

/* Creating the view of the mwin!!! */
Evas_Object *
_form_right_side(Style_Editor *style_edit)
{
   Elm_Object_Item *glit_style;
   Evas_Object *layout, *btn, *combobox, *search, *ic;
   Eina_List *styles, *l_st;
   Resource *res;

   assert(style_edit != NULL);

   if (!_itc_style)
     {
        _itc_style = elm_genlist_item_class_new();
        _itc_style->item_style = "navigator";
        _itc_style->func.text_get = _item_style_label_get;
        _itc_style->func.content_get = NULL;
        _itc_style->func.state_get = NULL;
        _itc_style->func.del = NULL;
     }
   if (!_itc_tags)
     {
        _itc_tags= elm_genlist_item_class_new();
        _itc_tags->item_style = "default";
        _itc_tags->func.text_get = _item_tags_label_get;
        _itc_tags->func.content_get = NULL;
        _itc_tags->func.state_get = NULL;
        _itc_tags->func.del = NULL;
     }

   layout = elm_layout_add(ap.win);
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_layout_theme_set(layout, "layout", "style_editor", "font_list");
   elm_object_part_text_set(layout, "label.font_list", _("Font list"));
   evas_object_show(layout);

   search = _style_editor_search_field_create(layout);
   elm_object_part_content_set(layout, "swallow.search", search);
   evas_object_smart_callback_add(search, "changed", _search_changed, style_edit);
   evas_object_smart_callback_add(search, "activated", _search_nxt_gd_item, style_edit);
   evas_object_smart_callback_add(style_edit->glist, "pressed", _search_reset_cb,
                                  &(style_edit->style_search_data));
   style_edit->style_search_data.search_entry = search;
   style_edit->style_search_data.last_item_found = NULL;

   style_edit->glist = elm_genlist_add(layout);
   elm_object_part_content_set(layout, "swallow.font_list", style_edit->glist);
   evas_object_smart_callback_add(style_edit->glist, "clicked,double", _on_clicked_double, NULL);
   evas_object_smart_callback_add(style_edit->glist, "expand,request", _expand_request_cb, NULL);
   evas_object_smart_callback_add(style_edit->glist, "expanded", _expanded_cb, style_edit);
   evas_object_smart_callback_add(style_edit->glist, "contract,request", _contract_request_cb, NULL);
   evas_object_smart_callback_add(style_edit->glist, "contracted", _contracted_cb, NULL);
   evas_object_show(style_edit->glist);

   evas_object_size_hint_align_set(style_edit->glist, EVAS_HINT_FILL,
                                   EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(style_edit->glist, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);

   styles = ap.project->styles;

   EINA_LIST_FOREACH(styles, l_st, res)
     {
        glit_style = elm_genlist_item_append(style_edit->glist, _itc_style,
                                             res->name, NULL, ELM_GENLIST_ITEM_TREE,
                                             _on_glit_selected, style_edit);
        elm_object_item_data_set(glit_style, (char *)res->name);
     }

   EWE_COMBOBOX_ADD(layout, combobox);
   ewe_combobox_style_set(combobox, "small");
   ewe_combobox_item_add(combobox, _("New style"));
   ewe_combobox_item_add(combobox, _("New tag"));
   evas_object_smart_callback_add(combobox, "selected", _on_bt_add, style_edit);
   elm_object_part_content_set(layout, "swallow.button_add", combobox);

   btn = elm_button_add(ap.win);
   evas_object_show(btn);
   ic = elm_icon_add(btn);
   elm_icon_standard_set(ic, "minus");
   elm_object_part_content_set(btn, NULL, ic);

   evas_object_smart_callback_add(btn, "clicked", _on_bt_del, style_edit);
   elm_object_part_content_set(layout, "swallow.button_rm", btn);

   return layout;
}

static void
_change_bg_cb(void *data,
              Evas_Object *obj,
              void *event_info __UNUSED__)
{
   Style_Editor *style_edit = (Style_Editor *)data;

   assert(style_edit != NULL);

   int state = elm_radio_state_value_get(obj);
   Evas_Object *bg = NULL;
   Evas *canvas = evas_object_evas_get(obj);
   switch (state)
     {
      case 0:
        {
           IMAGE_ADD_NEW(obj, bg, "bg", "tile");
           evas_object_show(bg);
        }
      break;
      case 1:
        {
           bg = evas_object_rectangle_add(canvas);
           evas_object_color_set(bg, 0, 0, 0, 255);
           evas_object_show(bg);
        }
      break;
      case 2:
        {
           bg = evas_object_rectangle_add(canvas);
           evas_object_color_set(bg, 255, 255, 255, 255);
           evas_object_show(bg);
        }
      break;
      default:
      break;
     }
   elm_object_part_content_set(style_edit->entry_prev, "background", bg);
}

Evas_Object*
_add_box_bg(Style_Editor *style_edit)
{
   Evas_Object *box_bg, *image_bg, *radio_group, *radio;

   BOX_ADD(ap.win, box_bg, true, false);
   elm_box_padding_set(box_bg, 6, 6);
   elm_box_align_set(box_bg, 1.0, 0.5);

#define _RADIO_ADD(RADIO, VALUE, IMAGE) \
   RADIO = elm_radio_add(ap.win); \
   elm_object_style_set(RADIO, "style_editor"); \
   elm_radio_state_value_set(RADIO, VALUE); \
   evas_object_show(RADIO); \
   IMAGE_ADD_NEW(box_bg, image_bg, "preview", IMAGE); \
   elm_object_part_content_set(RADIO, "bg", image_bg); \
   evas_object_smart_callback_add(RADIO, "changed", _change_bg_cb, style_edit); \
   elm_box_pack_end(box_bg, RADIO);

   _RADIO_ADD(radio_group, 0, "bg-tile");
   _RADIO_ADD(radio, 1, "bg-black");
   elm_radio_group_add(radio, radio_group);
   _RADIO_ADD(radio, 2, "bg-white");
   elm_radio_group_add(radio, radio_group);
#undef _RADIO_ADD

   return box_bg;
}

static void
_on_style_editor_close(void *data,
                        Evas *e __UNUSED__,
                        Evas_Object *obj __UNUSED__,
                        void *event_info __UNUSED__)
{
   Style_Editor *style_edit = (Style_Editor *)data;

   assert(style_edit != NULL);

   eina_stringshare_del(CURRENT.stvalue);
   free(style_edit);
}

Evas_Object *
style_editor_window_add()
{
   Evas_Object *main_layout, *layout_right;
   Evas_Object *bg, *btn_apply, *box_bg;
   Evas *canvas;
   Evas_Textblock_Style *ts;
   Style_Editor *style_edit;
   static const char *style_buf = FONT_DEFAULT"'";

   assert(ap.project != NULL);

   style_edit = (Style_Editor *)mem_calloc(1, sizeof(Style_Editor));

   main_layout = elm_layout_add(ap.win);
   elm_layout_theme_set(main_layout, "layout", "tab_style", "style_editor");
   elm_object_part_text_set(main_layout, "label.preview", _("Preview"));

   style_edit->entry_prev = elm_layout_add(main_layout);
   evas_object_size_hint_weight_set(style_edit->entry_prev, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   elm_layout_theme_set(style_edit->entry_prev, "layout", "style_editor", "preview");
   evas_object_show(style_edit->entry_prev);
   elm_object_signal_emit(style_edit->entry_prev, "entry,hide", "eflete");

   canvas = evas_object_evas_get(ap.win);
   IMAGE_ADD_NEW(ap.win, bg, "bg", "tile");
   elm_object_part_content_set(style_edit->entry_prev, "background", bg);

   style_edit->textblock_style = evas_object_textblock_add(canvas);
   elm_object_part_content_set(style_edit->entry_prev, "entry", style_edit->textblock_style);
   evas_object_textblock_valign_set(style_edit->textblock_style, 0.5);
   ts = evas_textblock_style_new();
   evas_textblock_style_set(ts, style_buf);
   evas_object_textblock_style_set(style_edit->textblock_style, ts);
   evas_object_textblock_text_markup_set(style_edit->textblock_style,
                       _("The quick brown fox jumps over the lazy dog"));
   evas_object_show(style_edit->textblock_style);
   elm_object_part_content_set(main_layout, "swallow.preview", style_edit->entry_prev);

   layout_right = _form_right_side(style_edit);
   elm_object_part_content_set(main_layout, "swallow.font_list", layout_right);
   evas_object_show(layout_right);

   box_bg = _add_box_bg(style_edit);
   elm_object_part_content_set(main_layout, "menu_container", box_bg);

   BUTTON_ADD(main_layout, btn_apply, _("Apply"))
   elm_object_part_content_set(main_layout, "elm.swallow.btn_apply", btn_apply);

   evas_textblock_style_free(ts);
   evas_object_event_callback_add(main_layout, EVAS_CALLBACK_DEL,
                                  _on_style_editor_close, style_edit);

   return main_layout;
}

#undef FONT_DEFAULT
#undef POPUP
#undef CURRENT
