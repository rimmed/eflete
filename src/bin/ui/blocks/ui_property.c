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

/*
#ifdef HAVE_CONFIG_H
   #include "eflete_config.h"
#endif */
#define EO_BETA_API
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT

#include "ui_property.h"
#include "property_macros.h"
#include "main_window.h"
#include "string_common.h"

#ifndef HAVE_ENVENTOR
#include "syntax_color.h"
#endif

#define PROP_DATA "prop_data"

#define PROP_DATA_GET() \
   assert(property != NULL); \
   Prop_Data *pd = evas_object_data_get(property, PROP_DATA); \
   assert(pd != NULL);

/*
 * Callback is added for frames at property box to correct scroller
 * work while each frame would be expanded/collapsed
 */
#define FRAME_PROPERTY_ADD(PARENT, FRAME, AUTOCOLLAPSE, TITLE, SCROLLER) \
FRAME_ADD(PARENT, FRAME, AUTOCOLLAPSE, TITLE) \
evas_object_smart_callback_add(FRAME, "clicked", _on_frame_click, SCROLLER);

struct _Prop_Data
{
   Evas_Object *workspace;
   Style *wm_style;
   Part *wm_part;
   Evas_Object *tabs;
   Ewe_Tabs_Item *visual_tab;
   Ewe_Tabs_Item *code_tab;
   Evas_Object *visual;
   Evas_Object *code;
   Eina_Stringshare *item_name;
#ifndef HAVE_ENVENTOR
   color_data *color_data;
   Eina_Strbuf *strbuf;
#else
   Evas_Object *code_bg;
#endif
   struct {
      Evas_Object *frame;
      Evas_Object *info;
      Evas_Object *shared_check;
      Evas_Object *ctxpopup;
      Evas_Object *name;
      Evas_Object *min_w, *min_h;
      Evas_Object *max_w, *max_h;
      Evas_Object *current;
   } group;
   struct {
      Evas_Object *frame;
      Evas_Object *name;
      Elm_Validator_Regexp *validator;
      Evas_Object *type;
      Evas_Object *scale, *scale_item;
      Evas_Object *mouse_events, *mouse_events_item;
      Evas_Object *repeat_events, *repeat_events_item;
      Evas_Object *clip_to, *clip_to_item;
      Evas_Object *ignore_flags, *ignore_flags_item;
      Evas_Object *source, *source_item;
      unsigned int previous_source;
   } part;
   struct {
      Evas_Object *frame;
      Evas_Object *x, *step_x;
      Evas_Object *y, *step_y;
      Evas_Object *confine;
      Evas_Object *threshold;
      Evas_Object *event;
   } part_drag;
   struct {
      Evas_Object *frame;
      Evas_Object *state;
      Evas_Object *proxy_source, *proxy_source_item;
      Evas_Object *visible;
      Evas_Object *min_w, *min_h;
      Evas_Object *max_w, *max_h;
      Evas_Object *fixed_w, *fixed_h;
      Evas_Object *align_x, *align_y;
      Evas_Object *aspect_min, *aspect_max;
      Evas_Object *aspect_pref;
      Evas_Object *color_class, *color1, *color2, *color3, *color_class_item;
      Evas_Object *color, *color_obj, *color_item;
      Evas_Object *minmul_w, *minmul_h;
      Evas_Object *box_layout, *box_layout_item;
      Evas_Object *table_homogeneous, *table_homogeneous_item;
   } state;
   struct {
      Evas_Object *frame;
      Evas_Object *rel1_to_x, *rel1_to_y;
      Evas_Object *rel1_relative_x, *rel1_relative_y;
      Evas_Object *rel1_offset_x, *rel1_offset_y;
      Evas_Object *rel2_to_x, *rel2_to_y;
      Evas_Object *rel2_relative_x, *rel2_relative_y;
      Evas_Object *rel2_offset_x, *rel2_offset_y;
   } state_object_area;
   struct {
      Evas_Object *frame;
      Evas_Object *text;
      Evas_Object *font;
      Elm_Validator_Regexp *validator;
      Evas_Object *size;
      Evas_Object *align_x, *align_y;
      Evas_Object *source;
      Evas_Object *min_x, *min_y;
      Evas_Object *max_x, *max_y;
      Evas_Object *fit_x, *fit_y;
      Evas_Object *text_source;
      Evas_Object *ellipsis_toggle, *ellipsis;
      Evas_Object *effect, *effect_direction;
      Evas_Object *color2, *color2_obj;
      Evas_Object *color3, *color3_obj;
   } state_text;
   struct {
      Evas_Object *frame;
      Evas_Object *text;
      Evas_Object *style;
      Evas_Object *align_x, *align_y;
      Evas_Object *min_x, *min_y;
      Evas_Object *max_x, *max_y;
      Evas_Object *select_mode;
      Evas_Object *entry_mode;
      Evas_Object *pointer_mode;
      Evas_Object *cursor_mode;
      Evas_Object *multiline;
      Evas_Object *source;
      Evas_Object *source2;
      Evas_Object *source3;
      Evas_Object *source4;
      Evas_Object *source5;
      Evas_Object *source6;
   } state_textblock;
   struct {
      Evas_Object *frame;
      Evas_Object *image;
      Evas_Object *border;
      Elm_Validator_Regexp *validator;
      Evas_Object *border_fill;
      Evas_Object *tween;
   } state_image;
   struct {
      Evas_Object *frame;
      Evas_Object *type;
      Evas_Object *smooth;
      Evas_Object *origin_relative_x, *origin_relative_y;
      Evas_Object *origin_offset_x, *origin_offset_y;
      Evas_Object *size_relative_x, *size_relative_y;
      Evas_Object *size_offset_x, *size_offset_y;
   } state_fill;
   struct {
      Evas_Object *frame;
      Evas_Object *align, *align1;
      Evas_Object *padding, *padding1;
      Evas_Object *min, *min1;
   } state_container;
   struct {
      Evas_Object *frame;
      Evas_Object *name;
      Evas_Object *source, *source_item;
      Evas_Object *min_w, *min_h;
      Evas_Object *max_w, *max_h;
      Evas_Object *spread_w, *spread_h;
      Evas_Object *prefer_w, *prefer_h;
      Evas_Object *padding_l, *padding_r, *padding_t, *padding_b;
      Evas_Object *align_x, *align_y;
      Evas_Object *weight_x, *weight_y;
      Evas_Object *aspect_w, *aspect_h;
      Evas_Object *aspect_mode;
      Evas_Object *position, *position1, *position_item; /* Only for items in part TABLE */
      Evas_Object *span, *span1; /* Only for items in part TABLE */
   } part_item;
};
typedef struct _Prop_Data Prop_Data;

static const char *edje_aspect_pref[] = { N_("None"),
                                          N_("Vertical"),
                                          N_("Horizontal"),
                                          N_("Both"),
                                          N_("Source"),
                                          NULL};

static const char *edje_item_aspect_pref[] = { N_("None"),
                                               N_("Neither"),
                                               N_("Vertical"),
                                               N_("Horizontal"),
                                               N_("Both"),
                                               NULL};

static const char *edje_select_mode[] = { N_("Default"),
                                          N_("Explicit"),
                                          NULL};

static const char *edje_entry_mode[] = { N_("None"),
                                         N_("Plain"),
                                         N_("Editable"),
                                         N_("Password"),
                                         NULL};

static const char *edje_pointer_mode[] = { N_("Autograb"),
                                           N_("Nograb"),
                                           NULL};

static const char *edje_cursor_mode[] = { N_("Under"),
                                          N_("Before"),
                                          NULL};

static const char *edje_fill_type[] = { N_("Scale"),
                                        N_("Tile"),
                                        NULL};

static const char *edje_ignore_flags[] = { N_("None"),
                                           N_("On hold"),
                                           NULL};

static const char *edje_text_effect_type[] = { N_("None"),
                                               N_("Plain"),
                                               N_("Outline"),
                                               N_("Soft Outline"),
                                               N_("Shadow"),
                                               N_("Soft Shadow"),
                                               N_("Outline Shadow"),
                                               N_("Outline Soft Shadow"),
                                               N_("Far Shadow"),
                                               N_("Far Soft Shadow"),
                                               N_("Glow"),
                                               NULL};

static const char *edje_text_effect_direction[] = { N_("Bottom Right"),
                                                    N_("Bottom"),
                                                    N_("Bottom Left"),
                                                    N_("Left"),
                                                    N_("Top Left"),
                                                    N_("Top"),
                                                    N_("Top Right"),
                                                    N_("Right"),
                                                    NULL};

static const char *edje_middle_type[] = { N_("None"),
                                          N_("Default"),
                                          N_("Solid")};

static const char *edje_homogeneous[] = { N_("None"),
                                          N_("Table"),
                                          N_("Item"),
                                          NULL};

static const char *edje_box_layouts[] = { N_("horizontal"),
                                          N_("vertical"),
                                          N_("horizontal_homogeneous"),
                                          N_("vertical_homogeneous"),
                                          N_("horizontal_max"),
                                          N_("vertical_max"),
                                          N_("horizontal_flow"),
                                          N_("vertical_flow"),
                                          N_("stack"),
                                          // N_("Custom Layout"), not implemented yet
                                          NULL};

static Eina_Bool
ui_property_state_obj_area_set(Evas_Object *property);

static void
ui_property_state_obj_area_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_text_set(Evas_Object *property);

static void
ui_property_state_text_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_textblock_set(Evas_Object *property);

static void
ui_property_state_textblock_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_image_set(Evas_Object *property);

static void
ui_property_state_image_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_fill_set(Evas_Object *property);

static void
ui_property_state_fill_unset(Evas_Object *property);

static Eina_Bool
ui_property_state_container_set(Evas_Object *property);

static void
ui_property_state_container_unset(Evas_Object *property);

static Elm_Genlist_Item_Class *_itc_tween = NULL;

static void
_on_spinner_mouse_wheel(void *data __UNUSED__,
                        Evas *e __UNUSED__,
                        Evas_Object *obj __UNUSED__,
                        void *event_info)
{
   Evas_Event_Mouse_Wheel *mev = event_info;
   mev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
}

TODO("remove this hack after scroller would be fixed")
/*
 * Hack start
 */
static void
_on_frame_click(void *data,
                Evas_Object *obj,
                void *event_info __UNUSED__)
{
   Evas_Object *scroller = (Evas_Object *)data;
   Evas_Object *box, *frame_box;
   int h_box, h_frame_box, h_scr, y_reg, h_reg, y_frame;
   box = elm_object_content_get(scroller);
   evas_object_geometry_get(scroller, NULL, NULL, NULL, &h_scr);
   evas_object_geometry_get(box, NULL, NULL, NULL, &h_box);
   frame_box = elm_object_content_get(obj);
   evas_object_geometry_get(frame_box, NULL, &y_frame, NULL, &h_frame_box);
   elm_scroller_region_get(scroller, NULL, &y_reg, NULL, &h_reg);
   elm_scroller_region_bring_in(scroller, 0.0, y_reg + 1, 0.0, h_reg);
   if (!elm_frame_collapse_get(obj))
     {
        if (h_box == h_scr + y_reg)
          elm_scroller_region_show(scroller, 0.0, y_reg + h_frame_box, 0.0, h_reg);
        else
          elm_scroller_region_bring_in(scroller, 0.0, y_reg + 1, 0.0, h_reg);
     }
}
/* Hack end */

static void
_del_prop_data(void *data,
               Evas *e __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *ei __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

#ifndef HAVE_ENVENTOR
   color_term(pd->color_data);
   eina_strbuf_free(pd->strbuf);
#endif
   free(pd);
}

static Evas_Object *
prop_item_label_add(Evas_Object *parent,
                    Evas_Object **label,
                    const char *lab_text,
                    const char *text)
{
   assert(parent != NULL);
   assert(label != NULL);

   PROPERTY_ITEM_ADD(parent, lab_text, "1swallow")
   LABEL_ADD(item, *label, text)
   elm_object_part_content_set(item, "elm.swallow.content", *label);
   return item;
}

#define prop_part_type_add(PARENT, NAME, TEXT) prop_item_label_add(PARENT, &pd->part.type, NAME, TEXT)
#define prop_state_state_add(PARENT, NAME, TEXT) prop_item_label_add(PARENT, &pd->state.state, NAME, TEXT)
#define prop_part_item_name_add(PARENT, NAME, TEXT) prop_item_label_add(PARENT, &pd->part_item.name, NAME, TEXT)

#define prop_part_type_update(TEXT) elm_object_text_set(pd->part.type, TEXT)
#define prop_state_state_update(TEXT) elm_object_text_set(pd->state.state, TEXT)
#define prop_part_item_name_update(TEXT) elm_object_text_set(pd->part_item.name, TEXT)

#ifdef HAVE_ENVENTOR

static void
_on_tab_activated(void *data,
                  Evas_Object *obj,
                  void *event_info)
{
   App_Data *ap;
   Ewe_Tabs_Item *it = (Ewe_Tabs_Item *) event_info;
   Evas_Object *property = (Evas_Object *)data;
   PROP_DATA_GET()

   ap = app_data_get();

   if (it == pd->code_tab)
     {
        if (!pd->code_bg)
          {
             pd->code = ap->enventor;
             pd->code_bg = elm_bg_add(obj);
             elm_bg_color_set(pd->code_bg, ENVENTOR_CODE_BG_COLOR);
             elm_object_part_content_set(pd->code_bg, "elm.swallow.content", pd->code);
             ewe_tabs_item_content_set(obj, it, pd->code_bg);
          }
        code_edit_mode_switch(ap, true);

        enventor_object_project_load(ap->enventor, ap->project);

        project_changed(false);
        evas_object_smart_callback_call(ap->enventor, "enventor,mode,on", NULL);
     }
   else
     {
        code_edit_mode_switch(ap, false);
        evas_object_smart_callback_call(ap->enventor, "enventor,mode,off",
                                        ap->project->current_style);
     }

   ap->enventor_mode = !ap->enventor_mode;
}

void
ui_property_code_of_group_setup(Evas_Object *property __UNUSED__)
{
}
#else

void
ui_property_code_of_group_setup(Evas_Object *property)
{
   char *markup_code;
   const char *colorized_code;
   Eina_Stringshare *code;
   PROP_DATA_GET()

   if (ewe_tabs_active_item_get(pd->tabs) != pd->code_tab) return;
   code = edje_edit_source_generate(pd->wm_style->obj);
   if (!code)
     ERR("Something wrong. Can not generate code for style %s", pd->wm_style->name);
   markup_code = elm_entry_utf8_to_markup(code);
   colorized_code = color_apply(pd->color_data, markup_code,
                                strlen(markup_code), NULL, NULL);
   if (colorized_code) elm_object_text_set(pd->code, colorized_code);
   evas_object_show(pd->code);
   free(markup_code);
   eina_stringshare_del(code);
}

static void
_on_tab_activated(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info)
{
   Ewe_Tabs_Item *it = (Ewe_Tabs_Item *) event_info;
   Evas_Object *property = (Evas_Object *)data;
   PROP_DATA_GET()
   if (it == pd->code_tab) ui_property_code_of_group_setup(property);
}

#endif

Evas_Object *
ui_property_add(Evas_Object *parent)
{
   Evas_Object *box, *scroller, *tabs;
   Ewe_Tabs_Item *it;
   Prop_Data *pd;

   assert(parent != NULL);

   pd = mem_calloc(1, sizeof(Prop_Data));
   tabs = ewe_tabs_add(parent);
   pd->tabs = tabs;
   elm_layout_text_set(tabs, NULL, _("Properties"));

   SCROLLER_ADD(tabs, scroller);
   BOX_ADD(scroller, box, EINA_FALSE, EINA_FALSE);
   elm_box_align_set(box, 0.5, 0.0);
   elm_object_content_set(scroller, box);

   pd->visual = scroller;
   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   it = ewe_tabs_item_append(tabs, NULL, _("Visual"), NULL);
   ewe_tabs_item_content_set(tabs, it, pd->visual);
   pd->visual_tab = it;

   it = ewe_tabs_item_append(tabs, it, _("Code"), NULL);
   pd->code_tab = it;

   evas_object_smart_callback_add(tabs, "ewe,tabs,item,activated",
                                  _on_tab_activated, tabs);
#ifndef HAVE_ENVENTOR
   pd->code = elm_entry_add(tabs);
   elm_entry_single_line_set(pd->code, false);
   elm_scroller_policy_set(pd->code, ELM_SCROLLER_POLICY_ON, ELM_SCROLLER_POLICY_ON);
   evas_object_size_hint_weight_set(pd->code, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(pd->code, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_scrollable_set(pd->code, true);
   elm_entry_editable_set(pd->code, false);
   pd->strbuf = eina_strbuf_new();
   pd->color_data = color_init(pd->strbuf);
   ewe_tabs_item_content_set(tabs, it, pd->code);
#endif

   evas_object_data_set(tabs, PROP_DATA, pd);
   evas_object_event_callback_add(tabs, EVAS_CALLBACK_DEL, _del_prop_data, pd);

   return tabs;
}

static void
_on_group_name_change(void *data,
                      Evas_Object *obj,
                      void *ei __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;
   const char *value, *old_value;
   char *entry;

   old_value = eina_stringshare_add(pd->wm_style->full_group_name);
   entry = elm_entry_markup_to_utf8(elm_entry_entry_get(obj));
   value = wm_style_name_set(pd->wm_style, entry);

   if (!edje_edit_group_name_set(pd->wm_style->obj, value)) return;

   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_STRING,
                    old_value, value, pd->wm_style->full_group_name,
                    (void*)edje_edit_group_name_set, "style name");
   elm_object_focus_set(obj, true);
   project_changed(false);
   eina_stringshare_del(old_value);
   free(entry);
}

#define GROUP_ATTR_2SPINNER(TEXT, SUB1, SUB2, VALUE1, VALUE2, CHECK) \
   GROUP_ATTR_2SPINNER_CALLBACK(SUB1, SUB2, VALUE1, CHECK) \
   GROUP_ATTR_2SPINNER_CALLBACK(SUB1, SUB2, VALUE2, CHECK) \
   GROUP_ATTR_2SPINNER_UPDATE(SUB1, VALUE1, VALUE2) \
   GROUP_ATTR_2SPINNER_ADD(TEXT, SUB1, VALUE1, VALUE2)

/*
 * Edje Edit API have't API for get the name from loaded image. It's first
 * reason for create this macro.
 *
 * Second: we need to get the different names for layouts and for styles. */
#define edje_edit_group_name_get(OBJ) \
   wm_style_layout_is(pd->wm_style) ? pd->wm_style->full_group_name : pd->wm_style->name \

#define GROUP_ATTR_1ENTRY(TEXT, SUB, VALUE, MEMBER, VALIDATOR, TOOLTIP) \
   GROUP_ATTR_1ENTRY_UPDATE(SUB, VALUE, MEMBER) \
   GROUP_ATTR_1ENTRY_ADD(TEXT, SUB, VALUE, MEMBER, VALIDATOR, TOOLTIP)

GROUP_ATTR_2SPINNER(_("min"), min, max, w, h, >)
GROUP_ATTR_2SPINNER(_("max"), max, min, w, h, <)
GROUP_ATTR_1ENTRY(_("name"), group, name, group, NULL, _(""))

#define pd_group pd->group

static void
_on_dismissed(void *data __UNUSED__,
               Evas_Object *obj,
               void *event_info __UNUSED__)
{
   evas_object_hide(obj);
}
static void
_on_clicked(void *data,
             Evas *e,
             Evas_Object *obj __UNUSED__,
             void *event_info __UNUSED__)
{
   int x, y;
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

   evas_object_smart_callback_del_full(pd_group.ctxpopup, "dismissed",
                                       _on_dismissed, pd);
   evas_object_smart_callback_add(pd_group.ctxpopup, "dismissed",
                                  _on_dismissed, pd);
   evas_pointer_canvas_xy_get(e, &x, &y);
   evas_object_move(pd_group.ctxpopup, x, y);
   evas_object_show(pd_group.ctxpopup);
}
static void
_prop_item_alias_update(Prop_Data *pd, Style *style, int aliases_count)
{
   Evas_Object *label, *label_ctx;
   Eina_List *list = NULL, *l;
   const char *text_info = NULL;
   char *list_data;
   Eina_Strbuf *text_ctx;

   assert(pd != NULL);
   assert(style != NULL);

   label = elm_object_part_content_get(pd_group.info, "elm.swallow.content");
   label_ctx = elm_object_content_get(pd_group.ctxpopup);
   if (style->isAlias)
     {
        text_info = eina_stringshare_add("This is alias of <a>%s</a>");
        text_info = eina_stringshare_printf(text_info,
                                            style->main_group->full_group_name);
     }
   else
     {
        list = edje_edit_group_aliases_get(style->obj, style->full_group_name);
        text_info = eina_stringshare_add("changes in this style will also affect <a>%d elements.</a>");
        text_info = eina_stringshare_printf(text_info, aliases_count);

        int count = 1;
        text_ctx = eina_strbuf_new();

        EINA_LIST_FOREACH(list, l, list_data)
          {
             const char *step = "%d. %s";
             if (eina_strbuf_length_get(text_ctx) > 0)
               eina_strbuf_append(text_ctx, "</br>");
             step = eina_stringshare_printf(step, count++, list_data);
             eina_strbuf_append(text_ctx, step);
          }

        evas_object_event_callback_del_full(pd_group.info, EVAS_CALLBACK_MOUSE_DOWN,
                                       _on_clicked, pd);
        evas_object_event_callback_add(pd_group.info, EVAS_CALLBACK_MOUSE_DOWN,
                                       _on_clicked, pd);
        elm_object_text_set(label_ctx, eina_strbuf_string_get(text_ctx));
        eina_strbuf_free(text_ctx);
        edje_edit_string_list_free(list);
     }

   elm_object_text_set(label, text_info);
   evas_object_show(pd_group.info);
   eina_stringshare_del(text_info);
}

static void
_prop_item_shared_check_update(Evas_Object *item, int count)
{
   Evas_Object *entry;

   assert(item != NULL);

   entry = elm_object_part_content_get(item, "info");
   Eina_Bool bool = false;
   if (count > 0) bool = true;
   elm_check_state_set(entry, bool);
   evas_object_show(item);
}

Eina_Bool
ui_property_style_set(Evas_Object *property, Style *style, Evas_Object *workspace)
{
   Evas_Object *group_frame, *box, *prop_box, *info_en = NULL;
   Evas_Object *item;
   Evas_Object *info_image;
   Evas_Object *check, *label_ctx;
   Eina_List *aliases = NULL, *l;
   const char *text_info = NULL;
   int aliases_count = 0;
   char *list_data;
   Eina_Strbuf *text_ctx = NULL;

   PROP_DATA_GET()
   assert(workspace != NULL);
   assert(style != NULL);

   ui_property_style_unset(property);

   evas_object_show(property);

   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);

   ewe_tabs_active_item_set(property, pd->visual_tab);

   pd->workspace = workspace;
   pd->wm_style = style;
   if (style->isAlias) pd->wm_style = style->main_group;

   assert(pd->wm_style == workspace_edit_object_get(workspace));

#ifndef HAVE_ENVENTOR
   if (ewe_tabs_active_item_get(pd->tabs) == pd->code_tab)
     ui_property_code_of_group_setup(property);
#endif

   prop_box = elm_object_content_get(pd->visual);
   aliases = edje_edit_group_aliases_get(style->obj, style->full_group_name);
   aliases_count = eina_list_count(aliases);

   if (!pd_group.info)
     {
        LABEL_ADD(property, info_en, text_info)

        ICON_STANDARD_ADD(property, info_image, false, "info");

        pd_group.info = elm_layout_add(property);
        evas_object_size_hint_weight_set(pd_group.info, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(pd_group.info, EVAS_HINT_FILL, 0.0);
        elm_layout_file_set(pd_group.info, EFLETE_EDJ, "eflete/property/item/info");

        pd_group.ctxpopup = elm_ctxpopup_add(main_window_get());
        elm_ctxpopup_direction_priority_set(pd_group.ctxpopup,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN,
                                            ELM_CTXPOPUP_DIRECTION_DOWN);
        elm_object_style_set(pd_group.ctxpopup, "info");
        evas_object_hide(pd_group.ctxpopup);

        if (style->isAlias)
          text_info = eina_stringshare_printf("This is alias of <a>%s</a>",
                                              style->main_group->full_group_name);
        else
          {
             text_info = eina_stringshare_printf("changes in this style will also"
                                                 "affect <a>%d elements.</a>",
                                                 aliases_count);
             int count = 1;
             text_ctx = eina_strbuf_new();

             EINA_LIST_FOREACH(aliases, l, list_data)
               {
                  const char *step = "%d. %s";
                  if (eina_strbuf_length_get(text_ctx) > 0)
                    eina_strbuf_append(text_ctx, "</br>");
                  step = eina_stringshare_printf(step, count++, list_data);
                  eina_strbuf_append(text_ctx, step);
               }
             evas_object_event_callback_add(pd_group.info, EVAS_CALLBACK_MOUSE_DOWN,
                                            _on_clicked, pd);
             LABEL_ADD(property, label_ctx, eina_strbuf_string_get(text_ctx))
             eina_strbuf_free(text_ctx);
             elm_object_style_set(label_ctx, "info");
             elm_object_content_set(pd_group.ctxpopup, label_ctx);
          }

        elm_object_text_set(info_en, text_info);
        eina_stringshare_del(text_info);
        elm_object_style_set(info_en, "info");
        elm_object_part_content_set(pd_group.info, "elm.swallow.content", info_en);
        elm_object_part_content_set(pd_group.info, "info", info_image);
     }

   if (!pd_group.shared_check)
     {
        CHECK_ADD(property, check)
        elm_object_text_set(check, "Shared style");
        if (aliases_count > 0) elm_check_state_set(check, true);
        elm_object_disabled_set(check, true);

        pd_group.shared_check = elm_layout_add(property);
        evas_object_size_hint_weight_set(pd_group.shared_check, EVAS_HINT_EXPAND, 0.0);
        evas_object_size_hint_align_set(pd_group.shared_check, EVAS_HINT_FILL, 0.0);
        elm_layout_file_set(pd_group.shared_check, EFLETE_EDJ, "eflete/property/item/info");
        elm_layout_signal_emit(pd_group.shared_check, "prop_item,content,hide", "eflete");
        elm_object_part_content_set(pd_group.shared_check, "info", check);
        evas_object_show(pd_group.shared_check);
     }

   if (!pd_group.frame)
     {
        FRAME_PROPERTY_ADD(property, group_frame, true, _("Layout property"), pd->visual)
        BOX_ADD(group_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(group_frame, box);

        item = prop_group_name_add(box, pd, NULL);
        elm_box_pack_end(box, item);
        item = prop_group_min_w_h_add(box, pd,
                                      _("Minimum group width in pixels."),
                                      _("Minimum group height in pixels."));
        elm_box_pack_end(box, item);
        item = prop_group_max_w_h_add(box, pd,
                                      _("Maximum group width in pixels."),
                                      _("Maximum group height in pixels."));
        elm_box_pack_end(box, item);

        elm_box_pack_start(prop_box, group_frame);
        pd_group.frame = group_frame;
     }
   else
     {
        if ((aliases_count > 0) || (style->isAlias))
          {
             _prop_item_alias_update(pd, style, aliases_count);
             evas_object_show(pd_group.info);
          }
        _prop_item_shared_check_update(pd_group.shared_check, aliases_count);
        prop_group_name_update(pd);
        prop_group_min_w_h_update(pd);
        prop_group_max_w_h_update(pd);
        evas_object_show(pd_group.frame);
     }
   if ((aliases_count > 0) || (style->isAlias))
     {
        elm_box_pack_start(prop_box, pd_group.info);
        evas_object_show(pd_group.info);
     }
   elm_box_pack_start(prop_box, pd_group.shared_check);

   edje_edit_string_list_free(aliases);

   return true;
}

void
ui_property_style_unset(Evas_Object *property)
{
   Evas_Object *prop_box;

   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   evas_object_hide(pd_group.info);
   elm_box_unpack(prop_box, pd_group.info);
   elm_box_unpack(prop_box, pd_group.shared_check);
   evas_object_event_callback_del_full(pd_group.info, EVAS_CALLBACK_MOUSE_DOWN,
                                  _on_clicked, pd);
   evas_object_hide(pd_group.frame);
   evas_object_hide(pd_group.shared_check);
   ui_property_part_unset(property);
   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);

#ifdef HAVE_ENVENTOR
   elm_object_part_content_unset(pd->code_bg, "elm.swallow.content");
   evas_object_del(pd->code_bg);
   evas_object_hide(pd->code);
   pd->code_bg = NULL;
   pd->code = NULL;
#else
  evas_object_hide(pd->code);
#endif

  evas_object_hide(property);
}
#undef pd_group

static void
_on_part_name_unfocus(void *data,
                      Evas_Object *obj,
                      void *ei __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

   const char *value = elm_entry_entry_get(obj);

   if (strcmp(value, pd->wm_part->name))
     elm_entry_entry_set(obj, pd->wm_part->name);
}

static void
_on_part_name_change(void *data,
                     Evas_Object *obj __UNUSED__,
                     void *ei __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;
   int pos;
   const char *value;
   const char *old_value = pd->wm_part->name;

   assert(pd != NULL);
   if (elm_validator_regexp_status_get(pd->part.validator) != ELM_REG_NOERROR)
     return;

   value = elm_entry_entry_get(obj);
   if (!edje_edit_part_name_set(pd->wm_style->obj, pd->wm_part->name, value))
     {
        NOTIFY_INFO(5, "Wrong input value for name field");
        return;
     }

   project_changed(false);
   workspace_edit_object_part_rename(pd->workspace, pd->wm_part->name, value);
   pd->wm_part->name = value;
   pos = elm_entry_cursor_pos_get(obj);
   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_RENAME, old_value, value,
                      pd->wm_style->full_group_name,
                      (void*)edje_edit_part_name_set, "rename",
                      pd->wm_part->name, NULL, 0.0);
   elm_object_focus_set(obj, true);
   elm_entry_cursor_pos_set(obj, pos);
   workspace_edit_object_recalc(pd->workspace);
}

static void
prop_part_name_update(Prop_Data *pd)
{
   assert(pd != NULL);

   elm_entry_entry_set(pd->part.name, pd->wm_part->name);
}

static Evas_Object *
prop_part_name_add(Evas_Object *parent, Prop_Data *pd)
{
   assert(parent != NULL);
   assert(pd != NULL);

   PROPERTY_ITEM_ADD(parent,  _("name"), "1swallow");
   ENTRY_ADD(parent, pd->part.name, true);
   eo_do(pd->part.name, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, pd->part.validator));
   elm_entry_entry_set(pd->part.name, pd->wm_part->name);
   elm_object_tooltip_text_set(pd->part.name, _("Selected part name"));
   evas_object_smart_callback_add(pd->part.name, "changed,user", _on_part_name_change, pd);
   evas_object_smart_callback_add(pd->part.name, "unfocused", _on_part_name_unfocus, pd);
   elm_layout_content_set(item, "elm.swallow.content", pd->part.name);
   prop_part_name_update(pd);

   return item;
}

static void
prop_part_clip_to_update(Prop_Data *pd)
{
   Part *part;
   Eina_Inlist *list_n = NULL;
   Eina_Stringshare *value;

   assert(pd != NULL);

   ewe_combobox_items_list_free(pd->part.clip_to, true);
   value = edje_edit_part_clip_to_get(pd->wm_style->obj, pd->wm_part->name);
   if (value)
     ewe_combobox_text_set(pd->part.clip_to, value);
   else
     ewe_combobox_text_set(pd->part.clip_to, _("None"));
   ewe_combobox_item_add(pd->part.clip_to, _("None"));
   EINA_INLIST_FOREACH_SAFE(pd->wm_style->parts, list_n, part)
     {
        if ((part != pd->wm_part) && (part->type == EDJE_PART_TYPE_RECTANGLE))
           ewe_combobox_item_add(pd->part.clip_to, part->name);
     }
   edje_edit_string_free(value);
}

inline static void
prop_part_drag_control_disable_set(Prop_Data *pd, Eina_Bool collapse)
{
   Eina_Bool bx, by;

   assert(pd != NULL);

   bx = edje_edit_part_drag_x_get(pd->wm_style->obj, pd->wm_part->name);
   by = edje_edit_part_drag_y_get(pd->wm_style->obj, pd->wm_part->name);

   elm_object_disabled_set(pd->part_drag.step_x, !bx);
   elm_object_disabled_set(pd->part_drag.step_y, !by);
   elm_object_disabled_set(pd->part_drag.confine, !(bx | by));
   elm_object_disabled_set(pd->part_drag.threshold, !(bx | by));

   if (collapse)
     elm_frame_collapse_set(pd->part_drag.frame, !(bx | by));
}

PART_ATTR_PARTS_LIST(part_drag, confine, part_drag)
PART_ATTR_PARTS_LIST(part_drag, threshold, part_drag)
PART_ATTR_PARTS_LIST(part_drag, event, part_drag)

PART_ATTR_SOURCE_UPDATE(part, source)

#define PART_ATTR_1CHECK(TEXT, SUB, VALUE, MEMBER, TOOLTIP) \
   PART_ATTR_1CHECK_CALLBACK(SUB, VALUE, MEMBER) \
   PART_ATTR_1CHECK_ADD(TEXT, SUB, VALUE, MEMBER, TOOLTIP)

#define PART_ATTR_1COMBOBOX(TEXT, SUB, VALUE, MEMBER, TOOLTIP) \
   PART_ATTR_1COMBOBOX_CALLBACK(SUB, VALUE, MEMBER) \
   PART_ATTR_1COMBOBOX_ADD(TEXT, SUB, VALUE, MEMBER, TOOLTIP)

#define PART_ATTR_1COMBOBOX_LIST(TEXT, SUB, VALUE, MEMBER, TYPE, LIST, TOOLTIP) \
   PART_ATTR_1COMBOBOX_LIST_CALLBACK(TEXT, SUB, VALUE, TYPE) \
   PART_ATTR_1COMBOBOX_LIST_ADD(TEXT, SUB, VALUE, MEMBER, LIST, TOOLTIP)

#define PART_ATTR_DRAG(TEXT, SUB, VALUE1, VALUE2) \
   PART_ATTR_DRAG_CALLBACK(SUB, VALUE1, VALUE2) \
   PART_ATTR_DRAG_UPDATE(SUB, VALUE1, VALUE2) \
   PART_ATTR_DRAG_ADD(TEXT, SUB, VALUE1, VALUE2)

/* part property */
PART_ATTR_1CHECK(_("scalable"), part, scale, part,
                 _("Specifies whether the part will scale it's size with an edje scaling factor"))
PART_ATTR_1CHECK(_("mouse events"), part, mouse_events, part,
                 _("Enable mouse events in this part"))
PART_ATTR_1CHECK(_("event propagation"), part, repeat_events, part,
                 _("Enable repeat mouse events to the parts below"))
PART_ATTR_1COMBOBOX(_("clipper"), part, clip_to, part,
                    _("Show only the area of part that coincides with another part's container"))
PART_ATTR_1COMBOBOX_LIST(_("ignore flags"), part, ignore_flags, part, Evas_Event_Flags, edje_ignore_flags,
                         _("Specifies whether events with the given flags should be ignored"))
PART_ATTR_1COMBOBOX(_("group source"), part, source, part,
                    _("Used for the group to be loaded and used to display GROUP part."))
/* part drag property */
PART_ATTR_DRAG(_("axis X"), part_drag, x, step_x)
PART_ATTR_DRAG(_("axis Y"), part_drag, y, step_y)
PART_ATTR_1COMBOBOX(_("drag area"), part_drag, confine, part_drag,
                    _("Limits the movement of the dragged part to another part's container"))
PART_ATTR_1COMBOBOX(_("threshold"), part_drag, threshold, part_drag,
                    _("Movement of the dragged part can only start when it get moved enough to be outside of this threshold part"))
PART_ATTR_1COMBOBOX(_("forward events"), part_drag, event, part_drag,
                    _("It causes the part to forward the drag events "
                    "to another part, thus ignoring them for itself."))

#define pd_part pd->part
#define pd_drag pd->part_drag
Eina_Bool
ui_property_part_set(Evas_Object *property, Part *part)
{
   Evas_Object *item;
   Evas_Object *box, *prop_box;
   int y_reg, h_reg, h_box;

   PROP_DATA_GET()
   assert(part != NULL);

   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_AUTO, ELM_SCROLLER_POLICY_AUTO);
   pd->wm_part = part;
   prop_box = elm_object_content_get(pd->visual);

   if (!pd_part.frame)
     {
        FRAME_PROPERTY_ADD(property, pd_part.frame, true, _("Part property"), pd->visual)
        BOX_ADD(pd_part.frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(pd_part.frame, box);

        if (pd->part.validator == NULL)
          pd->part.validator = elm_validator_regexp_new(PART_NAME_REGEX, NULL);

        item = prop_part_name_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_type_add(box, _("type"), wm_part_type_get(pd->wm_part->type));
        elm_box_pack_end(box, item);
        pd->part.scale_item = prop_part_scale_add(box, pd);
        elm_box_pack_end(box, pd->part.scale_item);
        pd->part.mouse_events_item = prop_part_mouse_events_add(box, pd);
        elm_box_pack_end(box, pd->part.mouse_events_item);
        pd->part.repeat_events_item = prop_part_repeat_events_add(box, pd);
        elm_box_pack_end(box, pd->part.repeat_events_item);
        pd->part.clip_to_item = prop_part_clip_to_add(box, pd);
        elm_box_pack_end(box, pd->part.clip_to_item);
        pd->part.ignore_flags_item = prop_part_ignore_flags_add(box, pd);
        elm_box_pack_end(box, pd->part.ignore_flags_item);
        pd->part.source_item = prop_part_source_add(box, pd);
        elm_box_pack_end(box, pd->part.source_item);

        FRAME_PROPERTY_ADD(pd->visual, pd_drag.frame, true, _("Part dragable property"), pd->visual)
        elm_object_style_set(pd_drag.frame, "outdent_top");
        elm_box_pack_end(box, pd_drag.frame);
        BOX_ADD(pd_drag.frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(pd_drag.frame, box);

        item = prop_part_drag_x_step_x_add(box, pd,
                                 _("Enable/Disable draggin along X axis"),
                                 _("Set a drag step value"));
        elm_box_pack_end(box, item);
        item = prop_part_drag_y_step_y_add(box, pd,
                                 _("Enable/Disable draggin along Y axis"),
                                 _("Set a drag step value"));
        elm_box_pack_end(box, item);
        item = prop_part_drag_confine_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_drag_threshold_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_drag_event_add(box, pd);
        elm_box_pack_end(box, item);

        elm_box_pack_after(prop_box, pd_part.frame, pd->group.frame);
     }
   else
     {
        prop_part_name_update(pd);
        prop_part_type_update(wm_part_type_get(pd->wm_part->type));
        PART_ATTR_1CHECK_UPDATE(part, scale, part)
        PART_ATTR_1CHECK_UPDATE(part, mouse_events, part)
        PART_ATTR_1CHECK_UPDATE(part, repeat_events, part)
        prop_part_clip_to_update(pd);
        PART_ATTR_1COMBOBOX_LIST_UPDATE(part, ignore_flags, part)
        prop_part_source_update(pd);

        prop_part_drag_x_step_x_update(pd);
        prop_part_drag_y_step_y_update(pd);
        prop_part_drag_confine_update(pd);
        prop_part_drag_threshold_update(pd);
        prop_part_drag_event_update(pd);
     }

   box = elm_object_content_get(pd_part.frame);
   if (pd->wm_part->type == EDJE_PART_TYPE_SPACER)
     {
        /* Unpack from box, if selected part is SPACER and hide.
         * Check the visibility of 'mouse_events_item', to identify
         * previous selected part is SPACER or not. For identification
         * enough to check only one attribute, which not available
         * for SPACER */
        if (evas_object_visible_get(pd->part.mouse_events_item))
          {
             elm_box_unpack(box, pd->part.mouse_events_item);
             elm_box_unpack(box, pd->part.repeat_events_item);
             elm_box_unpack(box, pd->part.clip_to_item);
             elm_box_unpack(box, pd->part.ignore_flags_item);
             evas_object_hide(pd->part.mouse_events_item);
             evas_object_hide(pd->part.repeat_events_item);
             evas_object_hide(pd->part.clip_to_item);
             evas_object_hide(pd->part.ignore_flags_item);
          }
     }
   else
     {
        if (!evas_object_visible_get(pd->part.mouse_events_item))
          {
             elm_box_pack_after(box, pd->part.mouse_events_item, pd->part.scale_item);
             elm_box_pack_after(box, pd->part.repeat_events_item, pd->part.mouse_events_item);
             elm_box_pack_after(box, pd->part.clip_to_item, pd->part.repeat_events_item);
             elm_box_pack_after(box, pd->part.ignore_flags_item, pd->part.clip_to_item);
             evas_object_show(pd->part.mouse_events_item);
             evas_object_show(pd->part.repeat_events_item);
             evas_object_show(pd->part.clip_to_item);
             evas_object_show(pd->part.ignore_flags_item);
             evas_object_show(pd->part_drag.frame);
          }
     }
     if (pd->wm_part->type == EDJE_PART_TYPE_GROUP)
       {
         /* pack to box, only in case when the previous selected part
          * is not GROUP. If previos selected part is GROUP then
          * this item is show and already packed to box  */
         if (!evas_object_visible_get(pd_part.source_item))
           {
              elm_box_pack_after(box, pd_part.source_item, pd->part.ignore_flags_item);
              evas_object_show(pd->part.source_item);
           }
       }
     else
       {
          elm_box_unpack(box, pd_part.source_item);
          evas_object_hide(pd_part.source_item);
       }

   prop_part_drag_control_disable_set(pd, true);
   evas_object_geometry_get(prop_box, NULL, NULL, NULL, &h_box);
   elm_scroller_region_get(pd->visual, NULL, &y_reg, NULL, &h_reg);
   elm_scroller_region_bring_in(pd->visual, 0.0, y_reg + 1, 0.0, h_reg);
   if (h_box == h_reg + y_reg)
     elm_scroller_region_show(pd->visual, 0.0, y_reg + h_box, 0.0, h_reg);

   return true;
}

#define PROP_ITEM_UNSET(BOX, ITEM) \
   if (ITEM) \
     {\
        evas_object_smart_callback_del(ITEM, "clicked", _on_frame_click); \
        elm_box_unpack(BOX, ITEM); \
        evas_object_del(ITEM); \
        ITEM = NULL; \
     }

void
ui_property_part_unset(Evas_Object *property)
{
   Evas_Object *prop_box;

   PROP_DATA_GET()

   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);
   prop_box = elm_object_content_get(pd->visual);

   /*
   if (pd->part.validator != NULL)
     {
        elm_validator_regexp_free(pd->part.validator);
        pd->part.validator = NULL;
     }
   */
   PROP_ITEM_UNSET(prop_box, pd->part.frame)
   PROP_ITEM_UNSET(prop_box, pd->state.frame)
   evas_object_del(pd->state.color1);
   evas_object_del(pd->state.color2);
   evas_object_del(pd->state.color3);
   PROP_ITEM_UNSET(prop_box, pd->state_object_area.frame)
   PROP_ITEM_UNSET(prop_box, pd->state_text.frame)
   PROP_ITEM_UNSET(prop_box, pd->state_image.frame)
   PROP_ITEM_UNSET(prop_box, pd->state_textblock.frame)
   PROP_ITEM_UNSET(prop_box, pd->state_fill.frame)
   PROP_ITEM_UNSET(prop_box, pd->state_container.frame)
   PROP_ITEM_UNSET(prop_box, pd->part_item.frame)
}

#undef PROP_ITEM_UNSET
#undef pd_part
#undef pd_drag

/*****************************************************************************/
/*                          COLOR CLASS COMBOBOX                             */
/*****************************************************************************/
static void
_on_state_color_class_change(void *data,
                             Evas_Object *obj EINA_UNUSED,
                             void *event_info)
{
   Prop_Data *pd = (Prop_Data *)data;
   int r, g, b, a, r2, g2, b2, a2, r3, g3, b3, a3;
   Eina_Stringshare *old_value = NULL, *value = NULL;
   Ewe_Combobox_Item *item = event_info;

   assert(pd != NULL);

   old_value = edje_edit_state_color_class_get(pd->wm_style->obj, pd->wm_part->name,
                                               pd->wm_part->curr_state,
                                               pd->wm_part->curr_state_value);
   value = strcmp(item->title, "None") ? item->title : NULL;
   edje_edit_state_color_class_set(pd->wm_style->obj, pd->wm_part->name,
                                   pd->wm_part->curr_state,
                                   pd->wm_part->curr_state_value,
                                   value);

   if (edje_edit_color_class_colors_get(pd->wm_style->obj, item->title,
                                        &r, &g, &b, &a,
                                        &r2, &g2, &b2, &a2,
                                        &r3, &g3, &b3, &a3))
     {
        evas_object_color_set(pd->state.color1, r * a / 255, g * a / 255, b * a / 255, a);
        evas_object_color_set(pd->state.color2, r2 * a2 / 255, g2 * a2 / 255, b2 * a2 / 255, a2);
        evas_object_color_set(pd->state.color3, r3 * a3 / 255, g3 * a3 / 255, b3 * a3 / 255, a3);
     }
   else
     {
        evas_object_color_set(pd->state.color1, 0, 0, 0, 0);
        evas_object_color_set(pd->state.color2, 0, 0, 0, 0);
        evas_object_color_set(pd->state.color3, 0, 0, 0, 0);
     }

   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_STRING, old_value, value,
                    pd->wm_style->full_group_name,
                    (void*)edje_edit_state_color_class_set, "color class",
                    pd->wm_part->name, pd->wm_part->curr_state,
                    pd->wm_part->curr_state_value);

   edje_edit_string_free(value);
   edje_edit_string_free(old_value);

   project_changed(false);
   workspace_edit_object_recalc(pd->workspace);
}

static void
_color_class_items_fill(void *data,
                        Evas_Object *obj,
                        void *event_info __UNUSED__)
{
   int r, g, b, a, r2, g2, b2, a2, r3, g3, b3, a3;
   Eina_List *items, *l;
   Ewe_Combobox_Item *item;
   Prop_Data *pd = (Prop_Data *)data;
   Evas_Object *color;
   Evas *canvas;

   assert(pd != NULL);

   items = (Eina_List *)ewe_combobox_items_list_get(obj);

   if (!items) return;

   //get combobbox item from first list item
   item = eina_list_data_get(items);
   canvas = evas_object_evas_get(item->content);
   EINA_LIST_FOREACH(items, l, item)
     {
        // this case available if user select 'None'
        if (!edje_edit_color_class_colors_get(pd->wm_style->obj, item->title,
                                              &r, &g, &b, &a,
                                              &r2, &g2, &b2, &a2,
                                              &r3, &g3, &b3, &a3))
          continue;

       /* FIXME: this is bad solition, user shoud not use edje object for add contnent to a
        * combobox item. Need to move combobox from edje ocject to layout. */
       color = edje_object_add(canvas);
       edje_object_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
       evas_object_color_set(color, r * a / 255, g * a / 255, b * a / 255, a);
       edje_object_part_swallow(item->content, "swallow.color1", color);
       color = edje_object_add(canvas);
       edje_object_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
       evas_object_color_set(color, r2 * a2 / 255, g2 * a2 /  255, b2 * a2 / 255, a2);
       edje_object_part_swallow(item->content, "swallow.color2", color);
       color = edje_object_add(canvas);
       edje_object_file_set(color, EFLETE_THEME, "elm/image/color/color_set");
       evas_object_color_set(color, r3 * a3 / 255, g3 * a3 / 255, b3 * a3 / 255, a3);
       edje_object_part_swallow(item->content, "swallow.color3", color);
     }
}

static void
prop_state_color_class_update(Prop_Data *pd)
{
   Ewe_Combobox_Item *it;
   Eina_List *cclist, *l;
   const char *ccname;
   int r, g, b, a, r2, g2, b2, a2, r3, g3, b3, a3;
   Eina_Stringshare *color_c;

   assert(pd != NULL);

   ewe_combobox_items_list_free(pd->state.color_class, true);
   ewe_combobox_item_add(pd->state.color_class, _("None"));
   cclist = edje_edit_color_classes_list_get(pd->wm_style->obj);
   EINA_LIST_FOREACH(cclist, l, ccname)
     {
        it = ewe_combobox_item_add(pd->state.color_class, ccname);
        ewe_combobox_item_style_set(pd->state.color_class, it, "color_class");
     }
   color_c = edje_edit_state_color_class_get(pd->wm_style->obj, pd->wm_part->name,
                                             pd->wm_part->curr_state,
                                             pd->wm_part->curr_state_value);
   ewe_combobox_text_set(pd->state.color_class, color_c ? color_c : _("None"));
   if (color_c && edje_edit_color_class_colors_get(pd->wm_style->obj, color_c,
                                                   &r, &g, &b, &a,
                                                   &r2, &g2, &b2, &a2,
                                                   &r3, &g3, &b3, &a3))
     {
        evas_object_color_set(pd->state.color1, r * a / 255, g * a / 255, b * a / 255, a);
        evas_object_color_set(pd->state.color2, r2 * a2 / 255, g2 * a2 / 255, b2 * a2 / 255, a2);
        evas_object_color_set(pd->state.color3, r3 * a3 / 255, g3 * a3 / 255, b3 * a3 / 255, a3);
     }
   else
     {
        evas_object_color_set(pd->state.color1, 0, 0, 0, 0);
        evas_object_color_set(pd->state.color2, 0, 0, 0, 0);
        evas_object_color_set(pd->state.color3, 0, 0, 0, 0);
     }

   edje_edit_string_list_free(cclist);
   edje_edit_string_free(color_c);
}

static Evas_Object *
prop_state_color_class_add(Evas_Object *parent, Prop_Data *pd)
{
   assert(parent != NULL);
   assert(pd != NULL);

   PROPERTY_ITEM_ADD(parent, _("color class"), "1swallow")
   EWE_COMBOBOX_ADD(parent, pd->state.color_class)
   elm_object_tooltip_text_set(pd->state.color_class, "Set the color class");
   ewe_combobox_style_set(pd->state.color_class, "color_class");
   evas_object_smart_callback_add(pd->state.color_class, "selected", _on_state_color_class_change, pd);
   evas_object_smart_callback_add(pd->state.color_class, "expanded", _color_class_items_fill, pd);
   elm_layout_content_set(item, "elm.swallow.content", pd->state.color_class);
   IMAGE_ADD_NEW(parent, pd->state.color1, "color", "color_set")
   ewe_combobox_content_set(pd->state.color_class, "swallow.color1", pd->state.color1);
   IMAGE_ADD_NEW(parent, pd->state.color2, "color", "color_set")
   ewe_combobox_content_set(pd->state.color_class, "swallow.color2", pd->state.color2);
   IMAGE_ADD_NEW(parent, pd->state.color3, "color", "color_set")
   ewe_combobox_content_set(pd->state.color_class, "swallow.color3", pd->state.color3);

   prop_state_color_class_update(pd);
   return item;
}

#define STATE_ATTR_1CHECK(TEXT, SUB, VALUE, MEMBER, TOOLTIP) \
   STATE_ATTR_1CHECK_CALLBACK(SUB, VALUE, MEMBER) \
   STATE_ATTR_1CHECK_ADD(TEXT, SUB, VALUE, MEMBER, TOOLTIP)

#define STATE_ATTR_2SPINNER(TEXT, SUB, VALUE1, VALUE2, MEMBER, MIN, MAX, STEP, FMT, \
                            L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER, \
                            TYPE, HISTORY_TYPE) \
   STATE_ATTR_SPINNER_CALLBACK(SUB, VALUE1, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER) \
   STATE_ATTR_SPINNER_CALLBACK(SUB, VALUE2, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER) \
   STATE_ATTR_2SPINNER_ADD(TEXT, "2swallow", SUB, VALUE1, VALUE2, MEMBER, TYPE, MIN, MAX, STEP, FMT, \
                           L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER)

#define STATE_ATTR_2CHECK(TEXT, SUB, VALUE1, VALUE2, MEMBER, \
                          L1_START, L1_END, L2_START, L2_END, \
                          TOOLTIP1, TOOLTIP2) \
   STATE_ATTR_2CHECK_CALLBACK(SUB, VALUE1, VALUE2, MEMBER) \
   STATE_ATTR_2CHECK_ADD(TEXT, SUB, VALUE1, VALUE2, MEMBER, \
                        L1_START, L1_END, L2_START, L2_END, \
                        TOOLTIP1, TOOLTIP2)

#define STATE_ATTR_1COMBOBOX_LIST(TEXT, SUB, VALUE, MEMBER, LIST, TOOLTIP, TYPE) \
   STATE_ATTR_1COMBOBOX_LIST_CALLBACK(TEXT, SUB, VALUE, TYPE) \
   STATE_ATTR_1COMBOBOX_LIST_ADD(TEXT, SUB, VALUE, MEMBER, LIST, TOOLTIP)

#define STATE_ATTR_COLOR(TEXT, SUB, VALUE, MEMBER, TOOLTIP) \
   STATE_ATTR_COLOR_CALLBACK(SUB, VALUE, MEMBER) \
   STATE_ATTR_COLOR_LIST_UPDATE(SUB, VALUE, MEMBER) \
   STATE_ATTR_COLOR_ADD(TEXT, SUB, VALUE, MEMBER, TOOLTIP)

#define STATE_ATTR_COMBOBOX(TEXT, SUB, VALUE, MEMBER, TOOLTIP) \
   STATE_ATTR_1COMBOBOX_CALLBACK(SUB, VALUE, MEMBER) \
   STATE_ATTR_1COMBOBOX_ADD(TEXT, SUB, VALUE, MEMBER, TOOLTIP)

#define STATE_MINMAX_ATTR_2SPINNER(TEXT, SUB, VALUE1, VALUE2, MEMBER, MIN, MAX, STEP, FMT, \
                            L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER, \
                            TYPE, HISTORY_TYPE, DIF_VALUE1, DIF_VALUE2, CHECK) \
   STATE_MINMAX_ATTR_SPINNER_CALLBACK(SUB, VALUE1, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER, DIF_VALUE1, CHECK) \
   STATE_MINMAX_ATTR_SPINNER_CALLBACK(SUB, VALUE2, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER, DIF_VALUE2, CHECK) \
   STATE_ATTR_2SPINNER_ADD(TEXT, "2swallow", SUB, VALUE1, VALUE2, MEMBER, TYPE, MIN, MAX, STEP, FMT, \
                           L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER)

#define STATE_STRSHARE_ATR_1COMBOBOX_LIST(TEXT, SUB, VALUE, MEMBER, LIST, TOOLTIP) \
   STATE_STRSHARE_ATR_1COMBOBOX_LIST_CALLBACK(SUB, VALUE, MEMBER) \
   STATE_STRSHARE_ATR_1COMBOBOX_LIST_ADD(TEXT, SUB, VALUE, MEMBER, LIST, TOOLTIP)

STATE_ATTR_1CHECK(_("visible"), state, visible, state,
                  _("Set visibility for part by current state"))
STATE_MINMAX_ATTR_2SPINNER(_("min"), state, min_w, min_h, state, 0.0, 9999.0, 1.0, "%.0f", "w:", "px", "h:", "px",
                    _("Minimal size of part width in pixels."), _("Minimal part height in pixels."),
                    1, int, VAL_INT, max_w, max_h, >)
STATE_MINMAX_ATTR_2SPINNER(_("max"), state, max_w, max_h, state, -1.0, 9999.0, 1.0, "%.0f", "w:", "px", "h:", "px",
                    _("Maximal size of part width in pixels."), _("Maximal part height in pixels."),
                    1, int, VAL_INT, min_w, min_h, <)
STATE_ATTR_2SPINNER(_("align"), state, align_x, align_y, state, 0, 100, 1, NULL, "x:", "%", "y:", "%",
                    _("Part align horizontally"), _("Part align vertically"),
                    100, double, VAL_DOUBLE)
STATE_ATTR_2CHECK(_("fixed"), state, fixed_w, fixed_h, state, "w:", "", "h:", "",
                  _("This affects the minimum width calculation."),
                  _("This affects the minimum height calculation."))
STATE_ATTR_1COMBOBOX_LIST(_("aspect ratio mode"), state, aspect_pref, state, edje_aspect_pref,
                          _("The aspect control hints for this object."), unsigned char)
STATE_ATTR_2SPINNER(_("aspect ratio"), state, aspect_min, aspect_max, state, 0, 100, 1, NULL, "min:", "", "max:", "",
                   _("Normally width and height can be resized to any values independently"),
                   _("Normally width and height can be resized to any values independently"),
                   100, double, VAL_DOUBLE)
STATE_ATTR_2SPINNER(_("multiplier"), state, minmul_w, minmul_h, state, 0.0, 9999.0, 0.1, "%.1f", "w:", "", "h:", "",
                    _("The minimal part width value multiplier for current state"),
                    _("The minimal part height value multiplier for current state"),
                    1, double, VAL_DOUBLE)
STATE_ATTR_COLOR(_("color"), state, color, state, _("Part main color"))

STATE_ATTR_SOURCE_UPDATE(state, proxy_source, state, EDJE_PART_TYPE_SPACER, !=)
STATE_ATTR_COMBOBOX(_("proxy source"), state, proxy_source, state,
                    _("Causes the part to use another part content as"
                    "the content of this part. Only work with PROXY part."))
STATE_STRSHARE_ATR_1COMBOBOX_LIST(_("box layout"), state, box_layout, state, edje_box_layouts,
                          _("The aspect control hints for this object"))
STATE_ATTR_1COMBOBOX_LIST(_("table homogeneous"), state, table_homogeneous, state, edje_homogeneous,
                          _("The table homogeneous mode"), unsigned char)

Eina_Bool
ui_property_state_set(Evas_Object *property, Part *part)
{
   Evas_Object *item;
   Evas_Object *state_frame, *box, *prop_box;
   char state[BUFF_MAX];

   PROP_DATA_GET()
   assert(part != NULL);
   assert(pd->wm_part == part);

   #define pd_state pd->state

   snprintf(state, BUFF_MAX, "%s %.2f", part->curr_state, part->curr_state_value);

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_state.frame);

   if (!pd_state.frame)
     {
        FRAME_PROPERTY_ADD(property, state_frame, true, _("State"), pd->visual)
        BOX_ADD(state_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(state_frame, box);

        item = prop_state_state_add(box, _("state"), state);
        elm_box_pack_end(box, item);
        item = prop_state_visible_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_min_w_min_h_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_max_w_max_h_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_fixed_w_fixed_h_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_align_x_align_y_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_aspect_pref_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_aspect_min_aspect_max_add(box, pd);
        elm_box_pack_end(box, item);
        pd_state.color_class_item = prop_state_color_class_add(box, pd);
        elm_box_pack_end(box, pd_state.color_class_item);
        pd_state.color_item = prop_state_color_add(box, pd);
        elm_box_pack_after(box, pd_state.color_item, pd_state.color_class_item);
        item = prop_state_minmul_w_minmul_h_add(box, pd);
        elm_box_pack_end(box, item);
        pd_state.proxy_source_item = prop_state_proxy_source_add(box, pd);
        evas_object_hide(pd_state.proxy_source_item);
        pd_state.box_layout_item = prop_state_box_layout_add(box, pd);
        evas_object_hide(pd_state.box_layout_item);
        pd_state.table_homogeneous_item = prop_state_table_homogeneous_add(box, pd);
        evas_object_hide(pd_state.table_homogeneous_item);

        prop_box = elm_object_content_get(pd->visual);
        elm_box_pack_after(prop_box, state_frame, pd->part.frame);
        pd_state.frame = state_frame;
     }
   else
     {
        box = elm_object_content_get(pd_state.frame);
        prop_state_state_update(state);
        STATE_ATTR_1CHECK_UPDATE(state, visible, state)
        STATE_ATTR_2SPINNER_UPDATE(state, min_w, min_h, state, int, 1)
        STATE_ATTR_2SPINNER_UPDATE(state, max_w, max_h, state, int, 1)
        STATE_ATTR_2CHECK_UPDATE(state, fixed_w, fixed_h, state)
        STATE_ATTR_2SPINNER_UPDATE(state, align_x, align_y, state, double, 100)
        STATE_ATTR_1COMBOBOX_LIST_UPDATE(state, aspect_pref, state)
        STATE_ATTR_2SPINNER_UPDATE(state, aspect_min, aspect_max, state, double, 100)
        prop_state_color_class_update(pd);
        prop_state_color_update(pd);
        STATE_ATTR_2SPINNER_UPDATE(state, minmul_w, minmul_h, state, double, 1)
        prop_state_proxy_source_update(pd);
        STATE_STRSHARE_ATTR_1COMBOBOX_LIST_UPDATE(state, box_layout, state)
        STATE_ATTR_1COMBOBOX_LIST_UPDATE(state, table_homogeneous, state)

        prop_box = elm_object_content_get(pd->visual);
        elm_box_pack_end(prop_box, pd_state.frame);
        evas_object_show(pd_state.frame);
     }

   ui_property_state_obj_area_set(property);
   if (part->type == EDJE_PART_TYPE_TEXT) ui_property_state_text_set(property);
   else ui_property_state_text_unset(property);
   if (part->type == EDJE_PART_TYPE_TEXTBLOCK) ui_property_state_textblock_set(property);
   else ui_property_state_textblock_unset(property);
   if (part->type == EDJE_PART_TYPE_IMAGE) ui_property_state_image_set(property);
   else ui_property_state_image_unset(property);
   if ((part->type == EDJE_PART_TYPE_IMAGE) || (part->type == EDJE_PART_TYPE_PROXY))
     ui_property_state_fill_set(property);
   else if ((part->type != EDJE_PART_TYPE_IMAGE) && (part->type != EDJE_PART_TYPE_PROXY))
     ui_property_state_fill_unset(property);
   if (part->type == EDJE_PART_TYPE_TABLE)
     {
        ui_property_state_container_set(property);
        if (!evas_object_visible_get(pd_state.table_homogeneous_item))
          {
              elm_box_pack_end(box, pd_state.table_homogeneous_item);
              evas_object_show(pd_state.table_homogeneous_item);
          }
     }
   else
     {
        if (evas_object_visible_get(pd_state.table_homogeneous_item))
          {
              elm_box_unpack(box, pd_state.table_homogeneous_item);
              evas_object_hide(pd_state.table_homogeneous_item);
          }

     }
   if (part->type == EDJE_PART_TYPE_BOX)
     {
        ui_property_state_container_set(property);
        if (!evas_object_visible_get(pd_state.box_layout_item))
          {
              elm_box_pack_end(box, pd_state.box_layout_item);
              evas_object_show(pd_state.box_layout_item);
          }
     }
   else
     {
        if (evas_object_visible_get(pd_state.box_layout_item))
          {
              elm_box_unpack(box, pd_state.box_layout_item);
              evas_object_hide(pd_state.box_layout_item);
          }

     }
   if ((part->type != EDJE_PART_TYPE_TABLE) && (part->type != EDJE_PART_TYPE_BOX))
     ui_property_state_container_unset(property);

   /* hide/show the color class attribute control for GROUP */
   if ((part->type == EDJE_PART_TYPE_GROUP) ||
       (part->type == EDJE_PART_TYPE_SWALLOW) ||
       (part->type == EDJE_PART_TYPE_SPACER) ||
       (part->type == EDJE_PART_TYPE_TEXTBLOCK))
     {
        if (evas_object_visible_get(pd_state.color_class_item))
          {
             elm_box_unpack(box, pd_state.color_class_item);
             evas_object_hide(pd_state.color_class_item);
          }
     }
   else
     {
        if (!evas_object_visible_get(pd_state.color_class_item))
          {
             elm_box_pack_end(box, pd_state.color_class_item);
             evas_object_show(pd_state.color_class_item);
          }
     }
   /* hide/show the color attribute control */
   if (part->type == EDJE_PART_TYPE_TEXTBLOCK ||
       part->type == EDJE_PART_TYPE_SWALLOW ||
       part->type == EDJE_PART_TYPE_GROUP ||
       part->type == EDJE_PART_TYPE_SPACER)
     {
        if (evas_object_visible_get(pd_state.color_item))
          {
             elm_box_unpack(box, pd_state.color_item);
             evas_object_hide(pd_state.color_item);
          }
     }
   else
     {
        if (!evas_object_visible_get(pd_state.color_item))
          {
             elm_box_pack_after(box, pd_state.color_item, pd_state.color_class_item);
             evas_object_show(pd_state.color_item);
          }
     }

   if (part->type == EDJE_PART_TYPE_PROXY)
     {
        if (!evas_object_visible_get(pd_state.proxy_source_item))
          {
             elm_box_pack_end(box, pd_state.proxy_source_item);
             evas_object_show(pd_state.proxy_source_item);
          }
     }
   else
     {
        if (evas_object_visible_get(pd_state.proxy_source_item))
          {
             elm_box_unpack(box, pd_state.proxy_source_item);
             evas_object_hide(pd_state.proxy_source_item);
          }
     }

   elm_scroller_policy_set(pd->visual, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_ON);
   #undef pd_state
   return true;
}

void
ui_property_state_unset(Evas_Object *property)
{
   PROP_DATA_GET()

   evas_object_hide(pd->state.frame);
   /* hide the evas rectangles, it need to do, because those objects not a smart */
   evas_object_hide(pd->state.color1);
   evas_object_hide(pd->state.color2);
   evas_object_hide(pd->state.color3);

   ui_property_state_obj_area_unset(property);
   ui_property_state_text_unset(property);
   ui_property_state_image_unset(property);
   ui_property_state_textblock_unset(property);
   ui_property_state_fill_unset(property);
}

/* FIXME: edje_edit_state_relX_to do not update object properly.
   Setting of any other param completes the object update.
   Here min_w param is setted to its current value. */
#define ITEM_REL_TO_COMBOBOX_STATE_CALLBACK(SUB, VALUE) \
static void \
_on_combobox_##SUB##_##VALUE##_change(void *data, \
                             Evas_Object *obj EINA_UNUSED, \
                             void *ei) \
{ \
   Prop_Data *pd = (Prop_Data *)data; \
   \
   assert(pd != NULL); \
   \
   Ewe_Combobox_Item *item = ei; \
   const char *old_value = edje_edit_##SUB##_##VALUE##_get(pd->wm_style->obj, \
                                     pd->wm_part->name, pd->wm_part->curr_state, \
                                     pd->wm_part->curr_state_value); \
   const char *value = item->title; \
   if (strcmp(item->title, _("Layout"))) \
     edje_edit_##SUB##_##VALUE##_set(pd->wm_style->obj, pd->wm_part->name, \
                                     pd->wm_part->curr_state, pd->wm_part->curr_state_value, \
                                     item->title); \
   else \
     { \
        edje_edit_##SUB##_##VALUE##_set(pd->wm_style->obj, pd->wm_part->name, \
                                        pd->wm_part->curr_state, pd->wm_part->curr_state_value, \
                                        NULL); \
        value = NULL; \
     } \
   int temp = edje_edit_state_min_w_get(pd->wm_style->obj, pd->wm_part->name, \
                                       pd->wm_part->curr_state, pd->wm_part->curr_state_value); \
   edje_edit_state_min_w_set(pd->wm_style->obj, pd->wm_part->name, \
                             pd->wm_part->curr_state, \
                             pd->wm_part->curr_state_value, temp); \
   const char *text = eina_stringshare_printf("%s_%s", #SUB, #VALUE); \
   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_STRING, old_value, value, \
                    pd->wm_style->full_group_name, \
                    (void*)edje_edit_##SUB##_##VALUE##_set, text, \
                    pd->wm_part->name, pd->wm_part->curr_state, \
                    pd->wm_part->curr_state_value); \
   eina_stringshare_del(text); \
   project_changed(false); \
   workspace_edit_object_recalc(pd->workspace); \
}

#define STATE_ATTR_2COMBOBOX_V(TEXT, SUB, VALUE1, VALUE2, MEMBER, TOOLTIP1, TOOLTIP2) \
   STATE_ATTR_1COMBOBOX_CALLBACK(SUB, VALUE1, MEMBER) \
   STATE_ATTR_1COMBOBOX_CALLBACK(SUB, VALUE2, MEMBER) \
   STATE_ATTR_SOURCE_UPDATE(SUB, VALUE1, MEMBER, part->type, ==) \
   STATE_ATTR_SOURCE_UPDATE(SUB, VALUE2, MEMBER, part->type, ==) \
   STATE_ATTR_2COMBOBOX_ADD(TEXT, SUB, VALUE1, VALUE2, MEMBER, TOOLTIP1, TOOLTIP2)

#define STATE_ATTR_2SPINNER_ICON(TEXT, SUB, VALUE1, VALUE2, MEMBER, MIN, MAX, STEP, FMT, \
                                 L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER, \
                                 TYPE, HISTORY_TYPE) \
   STATE_ATTR_SPINNER_CALLBACK(SUB, VALUE1, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER) \
   STATE_ATTR_SPINNER_CALLBACK(SUB, VALUE2, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER) \
   STATE_ATTR_2SPINNER_ADD(TEXT, "2swallow_pad", SUB, VALUE1, VALUE2, MEMBER, TYPE, MIN, MAX, STEP, FMT, \
                           L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER)

STATE_ATTR_2COMBOBOX_V(_("relative to"), state, rel1_to_x, rel1_to_y, state_object_area,
                       _("Causes a corner to be positioned relatively to the X axis of another "
                         "part. Setting to \"\" will un-set this value"),
                       _("Causes a corner to be positioned relatively to the Y axis of another "
                         "part. Setting to \"\" will un-set this value"))
STATE_ATTR_2SPINNER_ICON(_("align"), state, rel1_relative_x, rel1_relative_y, state_object_area,
                         -500, 500, 1, NULL, "x:", "%", "y:", "%",
                         _("Define the position of left-up corner of the part's container. "
                           "Moves a corner to a relative position inside the container "
                           "by X axis."),
                         _("Define the position of left-up corner of the part's container. "
                           "Moves a corner to a relative position inside the container "
                           "by Y axis."),
                         100, double, VAL_DOUBLE)
STATE_ATTR_2SPINNER_ICON(_("offset"), state, rel1_offset_x, rel1_offset_y, state_object_area,
                         -9999, 9999, 1, NULL, "x:", "px", "y:", "px",
                         _("Left offset from relative position in pixels"),
                         _("Top offset from relative position in pixels"),
                         1, int, VAL_INT)
STATE_ATTR_2COMBOBOX_V(_("relative to"), state, rel2_to_x, rel2_to_y, state_object_area,
                       _("Causes a corner to be positioned relatively to the X axis of another "
                         "part. Setting to \"\" will un-set this value"),
                       _("Causes a corner to be positioned relatively to the Y axis of another "
                         "part. Setting to \"\" will un-set this value"))
STATE_ATTR_2SPINNER_ICON(_("align"), state, rel2_relative_x, rel2_relative_y, state_object_area,
                         -500, 500, 1, NULL, "x:", "%", "y:", "%",
                         _("Define the position of right-down corner of the part's container. "
                           "Moves a corner to a relative position inside the container "
                           "by X axis."),
                         _("Define the position of right-down corner of the part's container. "
                           "Moves a corner to a relative position inside the container "
                           "by Y axis."),
                         100, double, VAL_DOUBLE)
STATE_ATTR_2SPINNER_ICON(_("offset"), state, rel2_offset_x, rel2_offset_y, state_object_area,
                         -9999, 9999, 1, NULL, "x:", "px", "y:", "px",
                         _("Right offset from relative position in pixels"),
                         _("Bottom offset from relative position in pixels"),
                         1, int, VAL_INT)

#define pd_obj_area pd->state_object_area
static Eina_Bool
ui_property_state_obj_area_set(Evas_Object *property)
{
   Evas_Object *item, *icon;
   Evas_Object *obj_area_frame, *box, *prop_box;

   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_obj_area.frame);

   if (!pd_obj_area.frame)
     {
        FRAME_PROPERTY_ADD(property, obj_area_frame, true, _("Object area"), pd->visual)
        BOX_ADD(obj_area_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(obj_area_frame, box);

        item = elm_separator_add(obj_area_frame);
        elm_separator_horizontal_set(item, true);
        elm_object_style_set(item, "icon");
        elm_object_part_text_set(item, "eflete.text", _("Start point"));
        IMAGE_ADD_NEW(item, icon, "icon", "start-point")
        elm_object_part_content_set(item, "eflete.swallow.icon", icon);
        evas_object_show(item);
        elm_box_pack_end(box, item);

        item = prop_state_object_area_rel1_to_x_rel1_to_y_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_object_area_rel1_relative_x_rel1_relative_y_add(box, pd);
        IMAGE_ADD_NEW(item, icon, "icon", "align");
        elm_layout_content_set(item, NULL, icon);
        elm_box_pack_end(box, item);
        item = prop_state_object_area_rel1_offset_x_rel1_offset_y_add(box, pd);
        IMAGE_ADD_NEW(item, icon, "icon", "offset");
        elm_layout_content_set(item, NULL, icon);
        elm_box_pack_end(box, item);

        item = elm_separator_add(obj_area_frame);
        elm_separator_horizontal_set(item, true);
        elm_object_style_set(item, "icon");
        elm_object_part_text_set(item, "eflete.text", _("End point"));
        IMAGE_ADD_NEW(item, icon, "icon", "end-point");
        elm_object_part_content_set(item, "eflete.swallow.icon", icon);
        evas_object_show(item);
        elm_box_pack_end(box, item);

        item = prop_state_object_area_rel2_to_x_rel2_to_y_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_object_area_rel2_relative_x_rel2_relative_y_add(box, pd);
        IMAGE_ADD_NEW(item, icon, "icon", "align");
        elm_layout_content_set(item, NULL, icon);
        elm_box_pack_end(box, item);
        item = prop_state_object_area_rel2_offset_x_rel2_offset_y_add(box, pd);
        IMAGE_ADD_NEW(item, icon, "icon", "offset");
        elm_layout_content_set(item, NULL, icon);
        elm_box_pack_end(box, item);

        prop_box = elm_object_content_get(pd->visual);
        elm_box_pack_end(prop_box, obj_area_frame);
        pd_obj_area.frame = obj_area_frame;
     }
   else
     {
        prop_state_object_area_rel1_to_x_update(pd);
        prop_state_object_area_rel1_to_y_update(pd);
        STATE_ATTR_2SPINNER_UPDATE(state, rel1_relative_x, rel1_relative_y, state_object_area, double, 100)
        STATE_ATTR_2SPINNER_UPDATE(state, rel1_offset_x, rel1_offset_y, state_object_area, int,  1)

        prop_state_object_area_rel2_to_x_update(pd);
        prop_state_object_area_rel2_to_y_update(pd);
        STATE_ATTR_2SPINNER_UPDATE(state, rel2_relative_x, rel2_relative_y, state_object_area, double, 100)
        STATE_ATTR_2SPINNER_UPDATE(state, rel2_offset_x, rel2_offset_y, state_object_area, int, 1)

        prop_box = elm_object_content_get(pd->visual);
        elm_box_pack_end(prop_box, pd_obj_area.frame);
        evas_object_show(pd_obj_area.frame);
     }

   return true;
}

static void
ui_property_state_obj_area_unset(Evas_Object *property)
{
   Evas_Object *prop_box;

   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_obj_area.frame);
   evas_object_hide(pd_obj_area.frame);
}
#undef pd_obj_area

#define STATE_ATTR_1ENTRY(TEXT, SUB, VALUE, MEMBER, VALIDATOR, TOOLTIP) \
   STATE_ATTR_1ENTRY_CALLBACK(SUB, VALUE, VALIDATOR) \
   STATE_ATTR_1ENTRY_UPDATE(SUB, VALUE, MEMBER) \
   STATE_ATTR_1ENTRY_ADD(TEXT, SUB, VALUE, MEMBER, VALIDATOR, TOOLTIP)

#define STATE_ATTR_1SPINNER(TEXT, SUB, VALUE, MEMBER, MIN, MAX, STEP, FMT, \
                            L_START, L_END, TOOLTIP, MULTIPLIER, \
                            TYPE, HISTORY_TYPE) \
   STATE_ATTR_SPINNER_CALLBACK(SUB, VALUE, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER) \
   STATE_ATTR_1SPINNER_ADD(TEXT, SUB, VALUE, MEMBER, MIN, MAX, STEP, FMT, \
                           L_START, L_END, TOOLTIP, MULTIPLIER)

STATE_ATTR_1ENTRY(_("text"), state, text, state_text, NULL, _("The dispalyed text"))
STATE_ATTR_1ENTRY(_("font"), state, font, state_text, pd->state_text.validator,
                  _("The text font, posible set a font style. Ex: Sans:style=italic"))
STATE_ATTR_1SPINNER(_("size"), state_text, size, state_text, 1, 128, 1, "%.0f", "", "pt",
                    _("The font size"), 1, int, VAL_INT)
STATE_ATTR_2SPINNER(_("align"), state_text, align_x, align_y, state_text,
                    0.0, 100.0, 1.0, "%.0f", "x:", "%", "y:", "%",
                    _("Text horizontal align"), _("Text vertical align"),
                    100, double, VAL_DOUBLE)
STATE_ATTR_2CHECK(_("min"), state_text, min_x, min_y, state_text, "w:", "", "h:", "",
                  _("When any of the parameters is enabled it forces \t"
                  "the minimum size of the container to be equal to\t"
                  "the minimum size of the text."),
                  _("When any of the parameters is enabled it forces \t"
                  "the minimum size of the container to be equal to\t"
                  "the minimum size of the text."))
STATE_ATTR_2CHECK(_("max"), state_text, max_x, max_y, state_text, "w:", "", "h:", "",
                  _("When any of the parameters is enabled it forces \t"
                  "the maximum size of the container to be equal to\t"
                  "the maximum size of the text."),
                  _("When any of the parameters is enabled it forces \t"
                  "the maximum size of the container to be equal to\t"
                  "the maximum size of the text."));
STATE_ATTR_2CHECK(_("fit"), state_text, fit_x, fit_y, state_text, "w:", "", "h:", "",
                  _("Resize the text for it to fit in it's container by X axis"),
                  _("Resize the text for it to fit in it's container by Y axis"))
STATE_ATTR_COLOR(_("shadow color"), state, color2, state_text, NULL)
STATE_ATTR_COLOR(_("outline color"), state, color3, state_text, NULL)

STATE_ATTR_SOURCE_UPDATE(state_text, source, state_text, EDJE_PART_TYPE_TEXT, ==)
STATE_ATTR_COMBOBOX(_("source"), state_text, source, state_text,
                    _("Causes the part to use the text properties "
                      "(like font and size) of another part "
                      "and update them as they change."))
STATE_ATTR_SOURCE_UPDATE(state_text, text_source, state_text, EDJE_PART_TYPE_TEXT, ==)
STATE_ATTR_COMBOBOX(_("text source"), state_text, text_source, state_text,
                    _("Causes the part to display the text content of "
                      "another part and update them as they change."));

inline static void
_text_effect_update(Prop_Data *pd)
{
   Edje_Text_Effect effect, old_effect;

   assert(pd != NULL);

   old_effect = edje_edit_part_effect_get(pd->wm_style->obj, pd->wm_part->name);
   effect = ewe_combobox_select_item_get(pd->state_text.effect)->index |
            ewe_combobox_select_item_get(pd->state_text.effect_direction)->index << 4;
   edje_edit_part_effect_set(pd->wm_style->obj, pd->wm_part->name, effect);

   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_INT, old_effect, effect,
                    pd->wm_style->full_group_name, (void*)edje_edit_part_effect_set,
                    _("text effect"), pd->wm_part->name, NULL, 0);

   workspace_edit_object_recalc(pd->workspace);
   project_changed(false);
}

inline static void
_text_effect_contols_update(Prop_Data *pd)
{
   assert(pd != NULL);

   switch (ewe_combobox_select_item_get(pd->state_text.effect)->index)
     {
      case EDJE_TEXT_EFFECT_NONE:
      case EDJE_TEXT_EFFECT_PLAIN:
        {
           elm_object_disabled_set(pd->state_text.effect_direction, true);
           ewe_combobox_text_set(pd->state_text.effect_direction, _("None"));
           elm_object_disabled_set(pd->state_text.color3, true);
           elm_object_disabled_set(pd->state_text.color2, true);
           break;
        }
      case EDJE_TEXT_EFFECT_OUTLINE:
      case EDJE_TEXT_EFFECT_SOFT_OUTLINE:
        {
           elm_object_disabled_set(pd->state_text.effect_direction, true);
           ewe_combobox_text_set(pd->state_text.effect_direction, _("None"));
           elm_object_disabled_set(pd->state_text.color3, false);
           elm_object_disabled_set(pd->state_text.color2, true);
           break;
        }
      case EDJE_TEXT_EFFECT_OUTLINE_SHADOW:
      case EDJE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW:
      case EDJE_TEXT_EFFECT_GLOW:
        {
           elm_object_disabled_set(pd->state_text.effect_direction, false);
           elm_object_disabled_set(pd->state_text.color3, false);
           elm_object_disabled_set(pd->state_text.color2, false);
           break;
        }
      default:
        {
           elm_object_disabled_set(pd->state_text.effect_direction, false);
           elm_object_disabled_set(pd->state_text.color3, true);
           elm_object_disabled_set(pd->state_text.color2, false);
        }
     }
}

static void
_on_text_effect_direction_changed(void *data,
                                  Evas_Object *obj __UNUSED__,
                                  void *event_info __UNUSED__)
{
   assert(data != NULL);

   _text_effect_update((Prop_Data *)data);
}

static void
_on_text_effect_changed(void *data,
                        Evas_Object *obj __UNUSED__,
                        void *event_info __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

   _text_effect_contols_update(pd);
   ewe_combobox_select_item_set(pd->state_text.effect_direction, 0);
   _text_effect_update((Prop_Data *)data);
}


static void
prop_part_text_effect_update(Prop_Data *pd)
{
   int type, direction;
   Edje_Text_Effect edje_effect;

   assert(pd != NULL);

   edje_effect = edje_edit_part_effect_get(pd->wm_style->obj, pd->wm_part->name);

   type = edje_effect & EDJE_TEXT_EFFECT_MASK_BASIC;
   if ((type >= EDJE_TEXT_EFFECT_SOFT_OUTLINE) && (type != EDJE_TEXT_EFFECT_GLOW))
     direction = (edje_effect & EDJE_TEXT_EFFECT_MASK_SHADOW_DIRECTION) >> 4;
   else direction = 0;
   ewe_combobox_select_item_set(pd->state_text.effect, type);

   if ((type <= EDJE_TEXT_EFFECT_SOFT_OUTLINE) || (type == EDJE_TEXT_EFFECT_GLOW))
     elm_object_disabled_set(pd->state_text.effect_direction, true);
   else
     elm_object_disabled_set(pd->state_text.effect_direction, false);
   ewe_combobox_select_item_set(pd->state_text.effect_direction, direction);

   _text_effect_contols_update(pd);
}

static Evas_Object *
prop_part_text_effect_add(Evas_Object *parent, Prop_Data *pd)
{
   int i;

   assert(parent != NULL);
   assert(pd != NULL);

   PROPERTY_ITEM_ADD(parent, _("effect"), "2swallow_vertical")

   elm_object_part_text_set(item, "label.swallow1.start", _("type:"));
   EWE_COMBOBOX_ADD(item, pd->state_text.effect)
   for (i = 0; edje_text_effect_type[i];
        ewe_combobox_item_add(pd->state_text.effect, edje_text_effect_type[i]), i++) ;
   elm_object_tooltip_text_set(pd->state_text.effect, _("Causes Edje to draw the selected effect."));
   evas_object_smart_callback_add(pd->state_text.effect, "selected", _on_text_effect_changed, pd);
   elm_object_part_content_set(item, "swallow.content1", pd->state_text.effect);

   elm_object_part_text_set(item, "label.swallow2.start", _("direction:"));
   EWE_COMBOBOX_ADD(item, pd->state_text.effect_direction)
   for (i = 0; edje_text_effect_direction[i];
        ewe_combobox_item_add(pd->state_text.effect_direction, edje_text_effect_direction[i]), i++) ;
   elm_object_tooltip_text_set(pd->state_text.effect_direction, _("Shadow directions."));
   evas_object_smart_callback_add(pd->state_text.effect_direction, "selected", _on_text_effect_direction_changed, pd);
   elm_object_part_content_set(item, "swallow.content2", pd->state_text.effect_direction);

   return item;
}

#define pd_text pd->state_text

static void
_on_state_text_ellipsis_change(void *data,
                               Evas_Object *obj,
                               void *event_info __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

   double value = elm_spinner_value_get(obj);
   double old_value = edje_edit_state_text_elipsis_get(pd->wm_style->obj,
                                                       pd->wm_part->name,
                                                       pd->wm_part->curr_state,
                                                       pd->wm_part->curr_state_value);
   if (!edje_edit_state_text_elipsis_set(pd->wm_style->obj,
                                         pd->wm_part->name,
                                         pd->wm_part->curr_state,
                                         pd->wm_part->curr_state_value,
                                         value))
     return;
   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_DOUBLE, old_value, value,
                    pd->wm_style->full_group_name,
                    (void*)edje_edit_state_text_elipsis_set, "elipsis",
                    pd->wm_part->name, pd->wm_part->curr_state,
                    pd->wm_part->curr_state_value);
   workspace_edit_object_recalc(pd->workspace);
   project_changed(false);
}

static void
_on_state_text_ellipsis_toggle_change(void *data,
                                      Evas_Object *obj,
                                      void *event_info __UNUSED__)
{
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

   double value;
   double old_value = edje_edit_state_text_elipsis_get(pd->wm_style->obj,
                                                       pd->wm_part->name,
                                                       pd->wm_part->curr_state,
                                                       pd->wm_part->curr_state_value);

   if (elm_check_state_get(obj))
     {
        elm_object_disabled_set(pd->state_text.ellipsis, false);
        value = elm_spinner_value_get(pd->state_text.ellipsis);
     }
   else
     {
        elm_object_disabled_set(pd->state_text.ellipsis, true);
        value = -1.0;
     }
   edje_edit_state_text_elipsis_set(pd->wm_style->obj,
                                    pd->wm_part->name,
                                    pd->wm_part->curr_state,
                                    pd->wm_part->curr_state_value,
                                    value);
   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_DOUBLE, old_value, value,
                    pd->wm_style->full_group_name,
                    (void*)edje_edit_state_text_elipsis_set, "elipsis",
                    pd->wm_part->name, pd->wm_part->curr_state,
                    pd->wm_part->curr_state_value);
   workspace_edit_object_recalc(pd->workspace);
   project_changed(false);
}

static void
prop_state_text_ellipsis_update(Prop_Data *pd)
{
   double value;

   assert(pd != NULL);

   value = edje_edit_state_text_elipsis_get(pd->wm_style->obj,
                                            pd->wm_part->name,
                                            pd->wm_part->curr_state,
                                            pd->wm_part->curr_state_value);
   if (value < 0)
     {
        elm_check_state_set(pd->state_text.ellipsis_toggle, false);
        elm_object_disabled_set(pd->state_text.ellipsis, true);
        elm_spinner_value_set(pd->state_text.ellipsis, 0.0);
     }
   else
     {
        elm_check_state_set(pd->state_text.ellipsis_toggle, true);
        elm_object_disabled_set(pd->state_text.ellipsis, false);
        elm_spinner_value_set(pd->state_text.ellipsis, value);
     }
}

static Evas_Object *
prop_state_text_ellipsis_add(Evas_Object *parent, Prop_Data *pd)
{
   assert(parent != NULL);
   assert(pd != NULL);

   PROPERTY_ITEM_ADD(parent, _("ellipsis"), "2swallow")
   CHECK_ADD(item, pd->state_text.ellipsis_toggle)
   elm_object_style_set(pd->state_text.ellipsis_toggle, "toggle");
   elm_object_tooltip_text_set(pd->state_text.ellipsis_toggle,
                               _("Turn on/off the text ellipsis"));
   elm_layout_content_set(item, "swallow.content1", pd->state_text.ellipsis_toggle);
   evas_object_smart_callback_add(pd->state_text.ellipsis_toggle, "changed",
                                  _on_state_text_ellipsis_toggle_change, pd);

   SPINNER_ADD(item, pd->state_text.ellipsis, 0.0, 1.0, 0.1, true)
   elm_spinner_label_format_set(pd->state_text.ellipsis, "%1.2f");
   elm_object_tooltip_text_set(pd->state_text.ellipsis,
                               _("Cut text if biggest then part's area"
                                 "0.0 = fix the left side  1.0 = right side"));
   elm_layout_content_set(item, "swallow.content2", pd->state_text.ellipsis);
   evas_object_smart_callback_add(pd->state_text.ellipsis, "changed",
                                  _on_state_text_ellipsis_change, pd);
   evas_object_event_callback_priority_add(pd->state_text.ellipsis, EVAS_CALLBACK_MOUSE_WHEEL,
                                           EVAS_CALLBACK_PRIORITY_BEFORE, _on_spinner_mouse_wheel, NULL);

   prop_state_text_ellipsis_update(pd);
   return item;
}

static Eina_Bool
ui_property_state_text_set(Evas_Object *property)
{
   Evas_Object *item;
   Evas_Object *text_frame, *box, *prop_box;

   PROP_DATA_GET()

   /* if previos selected part is TEXT too, unpack it */
   ui_property_state_text_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_text.frame)
     {
         FRAME_PROPERTY_ADD(property, text_frame, true, _("Text"), pd->visual)
         BOX_ADD(text_frame, box, EINA_FALSE, EINA_FALSE)
         elm_box_align_set(box, 0.5, 0.0);
         elm_object_content_set(text_frame, box);

         if (pd_text.validator == NULL)
           pd_text.validator = elm_validator_regexp_new(FONT_STYLE_REGEX, NULL);

         item = prop_state_text_add(box, pd, NULL);
         elm_box_pack_end(box, item);
         item = prop_state_font_add(box, pd, NULL);
         elm_box_pack_end(box, item);
         item = prop_state_text_size_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_text_align_x_align_y_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_text_min_x_min_y_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_text_max_x_max_y_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_text_source_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_text_text_source_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_text_ellipsis_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_text_fit_x_fit_y_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_part_text_effect_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_color3_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_color2_add(box, pd);
         elm_box_pack_end(box, item);

          /* disable/enable the effect and colors controls */
          prop_part_text_effect_update(pd);

         elm_box_pack_end(prop_box, text_frame);
         pd_text.frame = text_frame;
     }
   else
     {
        prop_state_text_update(pd);
        prop_state_font_update(pd);
        COMMON_1SPINNER_UPDATE(state_text, size, state_text, int,  1, STATE_ARGS)
        STATE_ATTR_2SPINNER_UPDATE(state_text, align_x, align_y, state_text, double, 100)
        STATE_ATTR_2CHECK_UPDATE(state_text, min_x, min_y, state_text)
        STATE_ATTR_2CHECK_UPDATE(state_text, max_x, max_y, state_text)
        prop_state_text_source_update(pd);
        prop_state_text_text_source_update(pd);
        prop_state_text_ellipsis_update(pd);
        STATE_ATTR_2CHECK_UPDATE(state_text, fit_x, fit_y, state_text)
        prop_state_color2_update(pd);
        prop_state_color3_update(pd);
        prop_part_text_effect_update(pd);
        elm_box_pack_end(prop_box, pd_text.frame);
        evas_object_show(pd_text.frame);
     }

   return true;
}

static void
ui_property_state_text_unset(Evas_Object *property)
{
   Evas_Object *prop_box;

   PROP_DATA_GET()
   /*
   if (pd_text.validator != NULL)
     {
        elm_validator_regexp_free(pd_text.validator);
        pd_text.validator = NULL;
     }
   */
   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_text.frame);
   evas_object_hide(pd_text.frame);
}
#undef pd_text

static void
prop_state_textblock_style_update(Prop_Data *pd)
{
   Eina_List *slist, *l;
   const char *sname;

   assert(pd != NULL);

   ewe_combobox_items_list_free(pd->state_textblock.style, true);
   ewe_combobox_item_add(pd->state_textblock.style, _("None"));
   slist = edje_edit_styles_list_get(pd->wm_style->obj);
   EINA_LIST_FOREACH(slist, l, sname)
     {
        ewe_combobox_item_add(pd->state_textblock.style, sname);
     }
   sname = edje_edit_state_text_style_get(pd->wm_style->obj,
                                          pd->wm_part->name,
                                          pd->wm_part->curr_state,
                                          pd->wm_part->curr_state_value);
   ewe_combobox_text_set(pd->state_textblock.style, sname);
   edje_edit_string_free(sname);
   edje_edit_string_list_free(slist);
}


PART_ATTR_SOURCE_UPDATE(state_textblock, source);
PART_ATTR_SOURCE_UPDATE(state_textblock, source2);
PART_ATTR_SOURCE_UPDATE(state_textblock, source3);
PART_ATTR_SOURCE_UPDATE(state_textblock, source4);
PART_ATTR_SOURCE_UPDATE(state_textblock, source5);
PART_ATTR_SOURCE_UPDATE(state_textblock, source6);

#define pd_textblock pd->state_textblock
STATE_ATTR_COMBOBOX(_("style"), state_text, style, state_textblock,
                    _("Set the text style of part."))
STATE_ATTR_2SPINNER(_("align"), state_text, align_x, align_y, state_textblock,
                    0.0, 100.0, 1.0, "%.0f", "x:", "%", "y:", "%",
                    _("Text horizontal align"), _("Text vertical align"),
                    100, double, VAL_DOUBLE)
STATE_ATTR_2CHECK(_("min"), state_text, min_x, min_y, state_textblock, "w:", "", "h:", "",
                  _("When any of the parameters is enabled it forces \t"
                  "the minimum size of the container to be equal to\t"
                  "the minimum size of the text."),
                  _("When any of the parameters is enabled it forces \t"
                  "the minimum size of the container to be equal to\t"
                  "the minimum size of the text."))
STATE_ATTR_2CHECK(_("max"), state_text, max_x, max_y, state_textblock, "w:", "", "h:", "",
                  _("When any of the parameters is enabled it forces \t"
                  "the maximum size of the container to be equal to\t"
                  "the maximum size of the text."),
                  _("When any of the parameters is enabled it forces \t"
                  "the maximum size of the container to be equal to\t"
                  "the maximum size of the text."));
PART_ATTR_1COMBOBOX_LIST(_("select mode"), part, select_mode, state_textblock, Edje_Edit_Select_Mode,
                         edje_select_mode, _("Sets the selection mode for a textblock part"))
PART_ATTR_1COMBOBOX_LIST(_("entry mode"), part, entry_mode, state_textblock, Edje_Edit_Entry_Mode,
                         edje_entry_mode, _("Sets the edit mode for a textblock part"))
PART_ATTR_1COMBOBOX_LIST(_("pointer mode"), part, pointer_mode, state_textblock, Evas_Object_Pointer_Mode,
                         edje_pointer_mode, _("Sets the mouse pointer behavior for a given part"))
PART_ATTR_1COMBOBOX_LIST(_("cursor mode"), part, cursor_mode, state_textblock, unsigned int,
                         edje_cursor_mode, _("Sets the cursor mode for a textblock part"))
PART_ATTR_1CHECK(_("multiline"), part, multiline, state_textblock,
                 _("It causes a textblock that is editable to allow multiple lines for editing"))
PART_ATTR_1COMBOBOX(_("under selected text"), part, source, state_textblock,
                    _("Used for the group to be loaded and used for selection \t"
                    "display UNDER the selected text the source \t"
                    "of TEXTBLOCK part."))
PART_ATTR_1COMBOBOX(_("before selected text"), part, source2, state_textblock,
                    _("It is used for the group to be loaded and used for \t"
                    "selection display OVER the selected text."))
PART_ATTR_1COMBOBOX(_("under cursor"), part, source3, state_textblock,
                    _("It is used for the group to be loaded and used for \t"
                    "cursor display UNDER the cursor position."))
PART_ATTR_1COMBOBOX(_("over cursor"), part, source4, state_textblock,
                    _("It is used for the group to be loaded and used \t"
                    "for cursor display OVER the cursor position."))
PART_ATTR_1COMBOBOX(_("under anchor"), part, source5, state_textblock,
                    _("It is used for the group to be loaded and used for \t"
                    "anchors display UNDER the anchor position."))
PART_ATTR_1COMBOBOX(_("over anchor"), part, source6, state_textblock,
                    _("It is used for the group to be loaded and used for \t"
                    "anchor display OVER the anchor position."))

static Eina_Bool
ui_property_state_textblock_set(Evas_Object *property)
{
   Evas_Object *item;
   Evas_Object *textblock_frame, *box, *prop_box;

   PROP_DATA_GET()

   /* if previos selected part is TEXTBLOCK too, unpack it */
   ui_property_state_textblock_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_textblock.frame)
     {
         FRAME_PROPERTY_ADD(property, textblock_frame, true, _("TextBlock"), pd->visual)
         BOX_ADD(textblock_frame, box, EINA_FALSE, EINA_FALSE)
         elm_box_align_set(box, 0.5, 0.0);
         elm_object_content_set(textblock_frame, box);

         item = prop_state_text_add(box, pd, NULL);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_style_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_align_x_align_y_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_min_x_min_y_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_max_x_max_y_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_select_mode_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_entry_mode_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_pointer_mode_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_cursor_mode_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_multiline_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_source_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_source2_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_source3_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_source4_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_source5_add(box, pd);
         elm_box_pack_end(box, item);
         item = prop_state_textblock_source6_add(box, pd);
         elm_box_pack_end(box, item);

         elm_box_pack_end(prop_box, textblock_frame);
         pd_textblock.frame = textblock_frame;
     }
   else
     {
        prop_state_text_update(pd);
        prop_state_textblock_style_update(pd);
        STATE_ATTR_2SPINNER_UPDATE(state_text, align_x, align_y, state_textblock, double, 100)
        STATE_ATTR_2CHECK_UPDATE(state_text, min_x, min_y, state_textblock)
        STATE_ATTR_2CHECK_UPDATE(state_text, max_x, max_y, state_textblock)
        PART_ATTR_1COMBOBOX_LIST_UPDATE(part, select_mode, state_textblock)
        PART_ATTR_1COMBOBOX_LIST_UPDATE(part, entry_mode, state_textblock)
        PART_ATTR_1COMBOBOX_LIST_UPDATE(part, pointer_mode, state_textblock)
        PART_ATTR_1COMBOBOX_LIST_UPDATE(part, cursor_mode, state_textblock)
        PART_ATTR_1CHECK_UPDATE(part, multiline, state_textblock);
        prop_state_textblock_source_update(pd);
        prop_state_textblock_source2_update(pd);
        prop_state_textblock_source3_update(pd);
        prop_state_textblock_source4_update(pd);
        prop_state_textblock_source5_update(pd);
        prop_state_textblock_source6_update(pd);
        elm_box_pack_end(prop_box, pd_textblock.frame);
        evas_object_show(pd_textblock.frame);
     }
   return true;
}

static void
ui_property_state_textblock_unset(Evas_Object *property)
{
   Evas_Object *prop_box;

   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_textblock.frame);
   evas_object_hide(pd_textblock.frame);
}
#undef pd_textblock


#define pd_image pd->state_image

static void
_on_image_editor_done(void *data,
                      Evas_Object *obj __UNUSED__,
                      void *event_info)
{
   Evas_Object * border_entry;
   Prop_Data *pd = (Prop_Data *)data;
   App_Data *ap = app_data_get();
   const char *value;
   const char *selected = (const char *)event_info;

   assert(pd != NULL);

   if (!selected) return;
   border_entry = elm_object_part_content_get(pd_image.border, "elm.swallow.content");
   value = elm_entry_entry_get(pd->state_image.image);

   if (strcmp(value, selected) == 0) return;
   elm_entry_entry_set(pd->state_image.image, selected);
   edje_edit_state_image_set(pd->wm_style->obj, pd->wm_part->name,
                             pd->wm_part->curr_state,
                             pd->wm_part->curr_state_value, selected);
   pm_save_to_dev(ap->project, pd->wm_style, false);
   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_STRING, value,
                    selected, pd->wm_style->full_group_name,
                    (void*)edje_edit_state_image_set,  "state_image",
                    pd->wm_part->name, pd->wm_part->curr_state,
                    pd->wm_part->curr_state_value);
   evas_object_smart_callback_call(pd->state_image.image, "changed,user", NULL);
   elm_entry_entry_set(border_entry, NULL);
   evas_object_smart_callback_call(border_entry, "changed,user", NULL);
   workspace_edit_object_recalc(pd->workspace);
   project_changed(false);
}

static void
_on_state_image_choose(void *data,
                       Evas_Object *obj __UNUSED__,
                       void *ei __UNUSED__)
{
   Evas_Object *img_edit;
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

   const char *selected = elm_entry_entry_get(pd->state_image.image);

   App_Data *ap = app_data_get();

   img_edit = image_editor_window_add(ap->project, SINGLE);
   image_editor_file_choose(img_edit, selected);
   evas_object_smart_callback_add(img_edit, SIG_IMAGE_SELECTED, _on_image_editor_done, pd);
}

static void
_del_tween_image(void *data,
                 Evas_Object *obj,
                 void *event_info __UNUSED__)
{
   Evas_Object *tween_list = elm_object_parent_widget_get(obj);
   const char *selected = (const char *)data;
   Elm_Object_Item *it = elm_genlist_selected_item_get(tween_list);
   Prop_Data *pd = evas_object_data_get(tween_list, PROP_DATA);

   assert(pd != NULL);

   if ((!selected) || (!it) || (!pd)) return;
   if (edje_edit_state_tween_del(pd->wm_style->obj, pd->wm_part->name,
                                 pd->wm_part->curr_state, pd->wm_part->curr_state_value,
                                 selected))
     {
        elm_object_item_del(it);
        history_diff_add(pd->wm_style->obj, PROPERTY, DEL, VAL_STRING,
                         selected, edje_edit_state_tween_add,
                         pd->wm_style->full_group_name,
                         (void*)edje_edit_state_tween_del, "tween image",
                         pd->wm_part->name, pd->wm_part->curr_state,
                         pd->wm_part->curr_state_value);
        project_changed(false);
     }
}

static void
_on_image_editor_tween_done(void *data,
                            Evas_Object *obj __UNUSED__,
                            void *ei)
{
   Evas_Object *tween_list = (Evas_Object *)data;
   Eina_List *selected = (Eina_List *)ei;
   Eina_List *l = NULL;
   const char *name = NULL;
   Prop_Data *pd = evas_object_data_get(tween_list, PROP_DATA);

   assert(pd != NULL);

   if (!selected) return;

   EINA_LIST_FOREACH(selected, l, name)
     {
        if (edje_edit_state_tween_add(pd->wm_style->obj, pd->wm_part->name,
                                      pd->wm_part->curr_state,
                                      pd->wm_part->curr_state_value, name))
          {
             elm_genlist_item_append(tween_list, _itc_tween, name, NULL,
                                     ELM_GENLIST_ITEM_NONE, NULL, NULL);
             history_diff_add(pd->wm_style->obj, PROPERTY, ADD, VAL_STRING,
                              name, edje_edit_state_tween_del,
                              pd->wm_style->full_group_name,
                              (void*)edje_edit_state_tween_add, "tween image",
                              pd->wm_part->name, pd->wm_part->curr_state,
                              pd->wm_part->curr_state_value);
             project_changed(false);
          }
     }
   elm_frame_collapse_go(pd->state_image.tween, false);
   edje_edit_string_list_free(selected);
}

static void
_add_tween_image(void *data,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   Evas_Object *img_edit;
   Evas_Object *tween_list = (Evas_Object *)data;

   App_Data *ap = app_data_get();

   img_edit = image_editor_window_add(ap->project, TWEENS);
   evas_object_smart_callback_add(img_edit, SIG_IMAGE_SELECTED,
                                  _on_image_editor_tween_done, tween_list);

   return;
}

static char *
_item_label_get(void *data,
                Evas_Object *obj __UNUSED__,
                const char *part __UNUSED__)
{
   return strdup(data);
}

static Evas_Object *
_item_content_get(void *data, Evas_Object *obj, const char *part)
{
   const char* buf = NULL;
   const char *file = NULL, *group = NULL;
   Evas_Load_Error err;
   Evas_Object *button, *image;
   Prop_Data *pd = NULL;

   if (!strcmp(part, "elm.swallow.icon"))
    {
       pd = evas_object_data_get(obj, PROP_DATA);

       assert(pd != NULL);

       edje_object_file_get((const Eo*)pd->wm_style->obj, &file, &group);
       image = evas_object_image_add(evas_object_evas_get(obj));
       buf = eina_stringshare_printf("edje/images/%i",
                        edje_edit_image_id_get(pd->wm_style->obj, (const char*)data));
       evas_object_image_file_set(image, file, buf);
       err = evas_object_image_load_error_get(image);
       if (err != EVAS_LOAD_ERROR_NONE)
         {
            WARN("Image [%s] from file [%s] loaded with errors", buf, file);
            return NULL;
         }
       evas_object_image_filled_set(image, true);
       return image;
    }
   if (!strcmp(part, "elm.swallow.end"))
    {
       BUTTON_ADD(obj, button, NULL)
       ICON_STANDARD_ADD(button, image, true, "minus");
       elm_object_part_content_set(button, NULL, image);
       evas_object_smart_callback_add(button, "clicked", _del_tween_image,
                                      (const char*)data);
       elm_object_style_set(button, "anchor");
       return button;
    }
    return NULL;
}

static void
_tween_image_moved(Evas_Object *data,
                   Evas_Object *obj,
                   Elm_Object_Item *item EINA_UNUSED)
{
   Prop_Data *pd = (Prop_Data *)data;
   Eina_List *images_list, *l;
   Elm_Object_Item *next;
   const char *image_name;

   assert(pd != NULL);

   images_list = edje_edit_state_tweens_list_get(pd->wm_style->obj,
                                                 pd->wm_part->name,
                                                 pd->wm_part->curr_state,
                                                 pd->wm_part->curr_state_value);

   EINA_LIST_FOREACH(images_list, l, image_name)
     {
        edje_edit_state_tween_del(pd->wm_style->obj, pd->wm_part->name,
                                  pd->wm_part->curr_state, pd->wm_part->curr_state_value,
                                  image_name);
     }
   edje_edit_string_list_free(images_list);

   next = elm_genlist_first_item_get(obj);
   while (next)
     {
        image_name  = elm_object_item_data_get(next);
        edje_edit_state_tween_add(pd->wm_style->obj, pd->wm_part->name,
                                  pd->wm_part->curr_state, pd->wm_part->curr_state_value,
                                  image_name);
        next = elm_genlist_item_next_get(next);
     }
   project_changed(false);
}

Evas_Object *
prop_item_state_image_tween_add(Evas_Object *box, Prop_Data *pd)
{
   Evas_Object *tween_frame, *tween_list;
   Evas_Object *button, *icon;
   Eina_List *images_list, *l;
   char *image_name;

   assert(box != NULL);
   assert(pd != NULL);

   FRAME_PROPERTY_ADD(box, tween_frame, true, _("Tweens"), pd->visual)
   elm_object_style_set(tween_frame, "tween");

   tween_list = elm_genlist_add(tween_frame);
   elm_genlist_longpress_timeout_set(tween_list, 0.2);
   elm_genlist_reorder_mode_set(tween_list, true);
   evas_object_data_set(tween_list, PROP_DATA, pd);

   if (!_itc_tween)
     {
        _itc_tween = elm_genlist_item_class_new();
        _itc_tween->item_style = "default";
        _itc_tween->func.text_get = _item_label_get;
        _itc_tween->func.content_get = _item_content_get;
        _itc_tween->func.state_get = NULL;
        _itc_tween->func.del = NULL;
     }

   images_list = edje_edit_state_tweens_list_get(pd->wm_style->obj,
                                                 pd->wm_part->name,
                                                 pd->wm_part->curr_state,
                                                 pd->wm_part->curr_state_value);
   if (!images_list) elm_frame_collapse_go(tween_frame, true);
   EINA_LIST_FOREACH(images_list, l, image_name)
     {
       elm_genlist_item_append(tween_list, _itc_tween,
                               eina_stringshare_add(image_name), NULL,
                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
     }
   edje_edit_string_list_free(images_list);
   elm_object_content_set(tween_frame, tween_list);

   BUTTON_ADD(tween_frame, button, NULL)
   ICON_STANDARD_ADD(button, icon, true, "plus");
   elm_object_part_content_set(button, NULL, icon);
   evas_object_smart_callback_add(button, "clicked", _add_tween_image,
                                  tween_list);
   elm_object_style_set(button, "anchor");
   elm_object_part_content_set(tween_frame, "elm.swallow.add", button);
   evas_object_smart_callback_add(tween_list, "moved",
                                  (Evas_Smart_Cb)_tween_image_moved, pd);

   evas_object_show(tween_list);

   return tween_frame;
}

void
prop_item_state_image_tween_update(Evas_Object *tween, Prop_Data *pd)
{
   Evas_Object *tween_list;
   Eina_List *images_list, *l;
   const char *image_name = NULL;

   assert(tween != NULL);
   assert(pd != NULL);

   tween_list = elm_object_content_get(tween);
   elm_genlist_clear(tween_list);
   images_list = edje_edit_state_tweens_list_get(pd->wm_style->obj,
                                                 pd->wm_part->name,
                                                 pd->wm_part->curr_state,
                                                 pd->wm_part->curr_state_value);
   if (!images_list) elm_frame_collapse_go(tween, true);
   else elm_frame_collapse_go(tween, false);

   EINA_LIST_FOREACH(images_list, l, image_name)
     {
       elm_genlist_item_append(tween_list, _itc_tween, image_name, NULL,
                               ELM_GENLIST_ITEM_NONE, NULL, NULL);
     }
   edje_edit_string_list_free(images_list);
}

static void
_on_state_image_border_change(void *data,
                              Evas_Object *obj,
                              void *ei __UNUSED__)
{ \
   unsigned int tok_elm;
   char **c = NULL;
   int lb = 0, rb = 0, tb = 0, bb = 0;
   int old_lb, old_rb, old_tb, old_bb;
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

   const char *value = elm_entry_entry_get(obj);
   edje_edit_state_image_border_get(pd->wm_style->obj, pd->wm_part->name,
                                    pd->wm_part->curr_state,
                                    pd->wm_part->curr_state_value,
                                    &old_lb, &old_rb, &old_tb, &old_bb);
   if (elm_validator_regexp_status_get(pd->state_image.validator) != ELM_REG_NOERROR) return;
   if (!value || !strcmp(value, ""))
     lb = rb = tb = bb = 0;
   else
     {
        c = eina_str_split_full (value, " ", 4, &tok_elm);
        if (tok_elm < 4)
          {
             free(c[0]);
             free(c);
             return;
          }
        lb = atoi(c[0]); rb = atoi(c[1]); tb = atoi(c[2]); bb = atoi(c[3]);
        free(c[0]);
        free(c);
     }
   edje_edit_state_image_border_set(pd->wm_style->obj, pd->wm_part->name,
                                    pd->wm_part->curr_state, pd->wm_part->curr_state_value,
                                    lb, rb, tb, bb);
   history_diff_add(pd->wm_style->obj, PROPERTY, MODIFY, VAL_FOUR, old_lb, old_rb,
                    old_tb, old_bb, lb, rb, tb, bb, pd->wm_style->full_group_name,
                    (void*)edje_edit_state_image_border_set, "state image border",
                    pd->wm_part->name, pd->wm_part->curr_state,
                    pd->wm_part->curr_state_value);

   project_changed(false);
   workspace_edit_object_recalc(pd->workspace);
}

static void
prop_state_image_border_update(Prop_Data *pd)
{
   int l, r, t, b;

   assert(pd != NULL);

   char buff[strlen("255 255 255 255") + 1];
   edje_edit_state_image_border_get(pd->wm_style->obj, pd->wm_part->name,
                                    pd->wm_part->curr_state, pd->wm_part->curr_state_value,
                                    &l, &r, &t, &b);
   if (!l && !r && !t && !b)
     elm_entry_entry_set(pd->state_image.border, NULL);
   else
     {
        snprintf(buff, strlen("255 255 255 255") + 1, "%i %i %i %i", l, r, t, b);
        elm_entry_entry_set(pd->state_image.border, buff);
     }
}

STATE_ATTR_1ENTRY(_("image"), state, image, state_image, NULL, NULL)
STATE_ATTR_1ENTRY_ADD(_("border"), state_image, border, state_image,
                      pd->state_image.validator, _("Image's border values"))
STATE_ATTR_1COMBOBOX_LIST(_("border fill"), state_image, border_fill, state_image,\
                          edje_middle_type, NULL, unsigned char)

static Eina_Bool
ui_property_state_image_set(Evas_Object *property)
{
   Evas_Object *item;
   Evas_Object *image_frame, *box, *prop_box;

   PROP_DATA_GET()

   /* if previos selected part is IMAGE too, unpack it */
   ui_property_state_image_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_image.frame)
     {
        FRAME_PROPERTY_ADD(property, image_frame, true, _("Image"), pd->visual)
        BOX_ADD(image_frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(image_frame, box);

        if (pd_image.validator == NULL)
          pd_image.validator = elm_validator_regexp_new(IMAGE_BORDER_REGEX, NULL);

        item = prop_state_image_add(box, pd, _on_state_image_choose);
        elm_box_pack_end(box, item);
        item = prop_state_image_border_add(box, pd, NULL);
        elm_box_pack_end(box, item);
        item = prop_state_image_border_fill_add(box, pd);
        elm_box_pack_end(box, item);

        pd_image.tween = prop_item_state_image_tween_add(box, pd);
        elm_box_pack_end(box, pd_image.tween);

        elm_box_pack_end(prop_box, image_frame);
        pd_image.frame = image_frame;
     }
   else
     {
        prop_state_image_update(pd);
        prop_state_image_border_update(pd);
        STATE_ATTR_1COMBOBOX_LIST_UPDATE(state_image, border_fill, state_image)
        prop_item_state_image_tween_update(pd_image.tween, pd);
        elm_box_pack_end(prop_box, pd_image.frame);
        evas_object_show(pd_image.frame);
     }

   return true;
}

static void
ui_property_state_image_unset(Evas_Object *property)
{
   Evas_Object *prop_box;

   PROP_DATA_GET()
   /*
   if (pd_image.validator != NULL)
     {
        elm_validator_regexp_free(pd_image.validator);
        pd_image.validator = NULL;
     }
   */
   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_image.frame);
   evas_object_hide(pd_image.frame);
}
#undef pd_image

#define pd_fill pd->state_fill
STATE_ATTR_1COMBOBOX_LIST(_("type"), state_fill, type, state_fill, edje_fill_type,
                          _("Set the image fill type"), unsigned char)
STATE_ATTR_1CHECK(_("smooth"), state_fill, smooth, state_fill,
                  _("On/off image smooth on scaling"))
STATE_ATTR_2SPINNER_ICON(_("align"), state_fill, origin_relative_x, origin_relative_y, state_fill,
                         -500, 500, 1, NULL, "x:", "%", "y:", "%",
                         _("Sets the starting point X coordinate relatively to displayed element's content"),
                         _("Sets the starting point Y coordinate relatively to displayed element's content"),
                         100, double, VAL_DOUBLE)
TODO("Fix offset edje_edit API: use int instead of double param")
STATE_ATTR_2SPINNER_ICON(_("offset"), state_fill, origin_offset_x, origin_offset_y, state_fill,
                         -9999, 9999, 1, NULL, "x:", "px", "y:", "px",
                         _("Affects the starting point a fixed number of pixels along X axis"),
                         _("Affects the starting point a fixed number of pixels along Y axis"),
                         1, double, VAL_DOUBLE)
STATE_ATTR_2SPINNER_ICON(_("align"), state_fill, size_relative_x, size_relative_y, state_fill,
                         -500, 500, 1, NULL, "x:", "%", "y:", "%",
                         _("Value that represent the percentual value of the original size of the element by X axis"),
                         _("Value that represent the percentual value of the original size of the element by Y axis."),
                         100, double, VAL_DOUBLE)
TODO("Fix offset edje_edit API: use int instead of double param")
STATE_ATTR_2SPINNER_ICON(_("offset"), state_fill, size_offset_x, size_offset_y, state_fill,
                         -9999, 9999, 1, NULL, "x:", "px", "y:", "px",
                         _("Affects the size of the tile a fixed number of pixels along X axis"),
                         _("Affects the size of the tile a fixed number of pixels along Y axis"),
                         1, double, VAL_DOUBLE)

static Eina_Bool
ui_property_state_fill_set(Evas_Object *property)
{
   Evas_Object *item, *icon;
   Evas_Object *box, *prop_box;

   PROP_DATA_GET()

   /* if previos selected part is IMAGE or PROXY too, unpack it */
   ui_property_state_fill_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_fill.frame)
     {
        FRAME_PROPERTY_ADD(property, pd->state_fill.frame, true, _("Fill"), pd->visual)
        BOX_ADD(pd->state_fill.frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(pd->state_fill.frame, box);

        item = prop_state_fill_type_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_fill_smooth_add(box, pd);
        elm_box_pack_end(box, item);

        /* origin subblock of fill block */
        item = elm_separator_add(pd->state_fill.frame);
        elm_separator_horizontal_set(item, true);
        elm_object_style_set(item, "icon");
        elm_layout_text_set(item, "eflete.text", _("Origin point"));
        IMAGE_ADD_NEW(item, icon, "icon", "start-point")
        elm_layout_content_set(item, "eflete.swallow.icon", icon);
        evas_object_show(item);
        elm_box_pack_end(box, item);

        item = prop_state_fill_origin_relative_x_origin_relative_y_add(box, pd);
        IMAGE_ADD_NEW(item, icon, "icon", "align");
        elm_layout_content_set(item, NULL, icon);
        elm_box_pack_end(box, item);

        item = prop_state_fill_origin_offset_x_origin_offset_y_add(box, pd);
        IMAGE_ADD_NEW(item, icon, "icon", "offset");
        elm_layout_content_set(item, NULL, icon);
        elm_box_pack_end(box, item);

        /* size subblock of fill block */
        item = elm_separator_add(pd->state_fill.frame);
        elm_separator_horizontal_set(item, true);
        elm_object_style_set(item, "icon");
        elm_object_part_text_set(item, "eflete.text", _("Size"));
        IMAGE_ADD_NEW(item, icon, "icon", "end-point")
        elm_layout_content_set(item, "eflete.swallow.icon", icon);
        evas_object_show(item);
        elm_box_pack_end(box, item);

        item = prop_state_fill_size_relative_x_size_relative_y_add(box, pd);
        IMAGE_ADD_NEW(item, icon, "icon", "align");
        elm_layout_content_set(item, NULL, icon);
        elm_box_pack_end(box, item);

        item = prop_state_fill_size_offset_x_size_offset_y_add(box, pd);
        IMAGE_ADD_NEW(item, icon, "icon", "offset");
        elm_layout_content_set(item, NULL, icon);
        elm_box_pack_end(box, item);

        elm_box_pack_end(prop_box, pd_fill.frame);
     }
   else
     {
        STATE_ATTR_1COMBOBOX_LIST_UPDATE(state_fill, type, state_fill);
        STATE_ATTR_1CHECK_UPDATE(state_fill, smooth, state_fill)
        STATE_ATTR_2SPINNER_UPDATE(state_fill, origin_relative_x, origin_relative_y, state_fill, double, 100)
        STATE_ATTR_2SPINNER_UPDATE(state_fill, origin_offset_x, origin_offset_y, state_fill, int, 1)
        STATE_ATTR_2SPINNER_UPDATE(state_fill, size_relative_x, size_relative_y, state_fill, double, 100)
        STATE_ATTR_2SPINNER_UPDATE(state_fill, size_offset_x, size_offset_y, state_fill, int, 1)

        elm_box_pack_end(prop_box, pd_fill.frame);
     }
   evas_object_show(pd_fill.frame);
   return true;
}

static void
ui_property_state_fill_unset(Evas_Object *property)
{
   Evas_Object *prop_box;

   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_fill.frame);
   evas_object_hide(pd_fill.frame);
}
#undef pd_fill


static void
prop_part_item_padding_update(Prop_Data *pd)
{
   assert(pd != NULL);

   int l = 0, r = 0, t = 0, b = 0;

   edje_edit_part_item_padding_get(pd->wm_style->obj, pd->wm_part->name,
                                   pd->item_name, &l, &r, &t, &b);
   elm_spinner_value_set(pd->part_item.padding_l, l);
   elm_spinner_value_set(pd->part_item.padding_r, r);
   elm_spinner_value_set(pd->part_item.padding_t, t);
   elm_spinner_value_set(pd->part_item.padding_b, b);
}

static void
_on_part_item_padding_change(void *data,
                             Evas_Object *obj __UNUSED__,
                             void *event_info __UNUSED__)
{
   int l, r, t, b;
   Prop_Data *pd = (Prop_Data *)data;

   assert(pd != NULL);

   l = (int)elm_spinner_value_get(pd->part_item.padding_l);
   r = (int)elm_spinner_value_get(pd->part_item.padding_r);
   t = (int)elm_spinner_value_get(pd->part_item.padding_t);
   b = (int)elm_spinner_value_get(pd->part_item.padding_b);
   if (!edje_edit_part_item_padding_set(pd->wm_style->obj, pd->wm_part->name,
                                        pd->item_name, l, r, t, b))
     return;
   project_changed(false);
   workspace_edit_object_recalc(pd->workspace);
}

static void
prop_part_item_padding_add(Evas_Object *box, Prop_Data *pd)
{
   Evas_Object *item;

   assert(box != NULL);
   assert(pd != NULL);

   #define SPINNER_SET(SPINNER, PART) \
      SPINNER_ADD(item, SPINNER, 0.0, 999.0, 1.0, true) \
      evas_object_smart_callback_add(SPINNER, "changed", \
                                     _on_part_item_padding_change, pd); \
      evas_object_event_callback_priority_add(SPINNER, EVAS_CALLBACK_MOUSE_WHEEL, \
                                           EVAS_CALLBACK_PRIORITY_BEFORE, \
                                           _on_spinner_mouse_wheel, NULL); \
      elm_layout_content_set(item, PART, SPINNER);

   LAYOUT_PROP_ADD(box, _("padding"), "property", "2swallow");
   elm_object_part_text_set(item, "label.swallow1.start", "left:");
   SPINNER_SET(pd->part_item.padding_l, "swallow.content1")
   elm_object_part_text_set(item, "label.swallow2.start", "right:");
   SPINNER_SET(pd->part_item.padding_r, "swallow.content2")
   elm_box_pack_end(box, item);

   LAYOUT_PROP_ADD(box, NULL, "property", "2swallow");
   elm_object_part_text_set(item, "label.swallow1.start", "top:");
   SPINNER_SET(pd->part_item.padding_t, "swallow.content1")
   elm_object_part_text_set(item, "label.swallow2.start", "bottom:");
   SPINNER_SET(pd->part_item.padding_b, "swallow.content2")
   elm_box_pack_end(box, item);

   prop_part_item_padding_update(pd);

   #undef SPINNER_SET
}

#define pd_item pd->part_item

static void
prop_part_item_source_update(Prop_Data *pd)
{
   Eina_List *collections, *l;
   const char *group, *value;
   App_Data *ap = app_data_get();

   assert(pd != NULL);

   unsigned int i = 0;
   ewe_combobox_items_list_free(pd->part_item.source, true);
   value = edje_edit_part_item_source_get(pd->wm_style->obj, pd->wm_part->name, pd->item_name);
   ewe_combobox_text_set(pd->part_item.source, value ? value : _("None"));

   collections = edje_mmap_collection_list(ap->project->mmap_file);
   collections = eina_list_sort(collections, eina_list_count(collections), sort_cb);
   EINA_LIST_FOREACH(collections, l, group)
     {
        if (group != pd->wm_style->full_group_name)
          ewe_combobox_item_add(pd->part_item.source, group);
        if (group == value)
          pd->part.previous_source = i;
        i++;
     }
   edje_edit_string_free(value);
   edje_mmap_collection_list_free(collections);
}

#define PART_ITEM_ATTR_1COMBOBOX(TEXT, SUB, VALUE, MEMBER, TOOLTIP) \
   PART_ITEM_ATTR_1COMBOBOX_CALLBACK(SUB, VALUE, MEMBER) \
   PART_ITEM_ATTR_1COMBOBOX_ADD(TEXT, SUB, VALUE, MEMBER, TOOLTIP)

#define PART_ITEM_MINMAX_ATTR_2SPINNER(TEXT, SUB, VALUE1, VALUE2, MEMBER, MIN, MAX, STEP, FMT, \
                                       L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER, \
                                       TYPE, HISTORY_TYPE, DIF_VALUE1, DIF_VALUE2, CHECK) \
   PART_ITEM_MINMAX_ATTR_SPINNER_CALLBACK(SUB, VALUE1, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER, DIF_VALUE1, CHECK) \
   PART_ITEM_MINMAX_ATTR_SPINNER_CALLBACK(SUB, VALUE2, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER, DIF_VALUE2, CHECK) \
   PART_ITEM_ATTR_2SPINNER_ADD(TEXT, "2swallow", SUB, VALUE1, VALUE2, MEMBER, TYPE, MIN, \
                               MAX, STEP, FMT, L1_START, L1_END, L2_START, L2_END, \
                               TOOLTIP1, TOOLTIP2, MULTIPLIER)

#define PART_ITEM_ATTR_2SPINNER(TEXT, SUB, VALUE1, VALUE2, MEMBER, MIN, MAX, STEP, FMT, \
                                L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER, \
                                TYPE, HISTORY_TYPE) \
   PART_ITEM_ATTR_SPINNER_CALLBACK(SUB, VALUE1, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER) \
   PART_ITEM_ATTR_SPINNER_CALLBACK(SUB, VALUE2, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER) \
   PART_ITEM_ATTR_2SPINNER_ADD(TEXT, "2swallow", SUB, VALUE1, VALUE2, MEMBER, TYPE, MIN, \
                               MAX, STEP, FMT, L1_START, L1_END, L2_START, L2_END, \
                               TOOLTIP1, TOOLTIP2, MULTIPLIER)

#define PART_ITEM_ATTR_1COMBOBOX_LIST(TEXT, SUB, VALUE, MEMBER, TYPE, LIST, TOOLTIP) \
   PART_ITEM_ATTR_1COMBOBOX_LIST_CALLBACK(TEXT, SUB, VALUE, TYPE) \
   PART_ITEM_ATTR_1COMBOBOX_LIST_ADD(TEXT, SUB, VALUE, MEMBER, LIST, TOOLTIP)

#define PART_ITEM_DOUBLEVAL_ATTR_2SPINNER(TEXT, SUB, VALUE1, VALUE2, MEMBER, MIN, MAX, STEP, FMT, \
                                          L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER, \
                                          TYPE, HISTORY_TYPE) \
   PART_ITEM_DOUBLEVAL_ATTR_SPINNER_CALLBACK(SUB, VALUE1, VALUE2, MEMBER, TYPE, HISTORY_TYPE, MULTIPLIER) \
   PART_ITEM_DOUBLEVAL_ATTR_2SPINNER_ADD(TEXT, "2swallow", SUB, VALUE1, VALUE2, MEMBER, TYPE, \
                                         MIN, MAX, STEP, FMT, L1_START, L1_END, L2_START, L2_END, \
                                         TOOLTIP1, TOOLTIP2, MULTIPLIER)

PART_ITEM_ATTR_1COMBOBOX(_("source"), part_item, source, part_item, _("Sets the group this object will be made from."))
PART_ITEM_MINMAX_ATTR_2SPINNER(_("min"), part_item, min_w, min_h, part_item,
                    0.0, 999.0, 1.0, NULL, "x:", "%", "y:", "%",
                    _("Set the item minimum size hint width in pixels"), _("Set the item minimum size hint height in pixels"),
                    1, int, VAL_INT, max_w, max_h, >)
PART_ITEM_MINMAX_ATTR_2SPINNER(_("max"), part_item, max_w, max_h, part_item,
                    0.0, 999.0, 1.0, NULL, "x:", "%", "y:", "%",
                    _("Set the item maximum size hint width in pixels"), _("Set the item maximum size hint height in pixels"),
                    1, int, VAL_INT, min_w, min_h, <)
PART_ITEM_ATTR_2SPINNER(_("prefer"), part_item, prefer_w, prefer_h, part_item,
                    0.0, 999.0, 1.0, NULL, "w:", "px", "h:", "px",
                    _("Set the item prefered size hint width in pixels"), _("Set the item prefered size hint height in pixels"),
                    1, int, VAL_INT)
PART_ITEM_ATTR_2SPINNER(_("align"), part_item, align_x, align_y, part_item,
                    0.0, 999.0, 1.0, NULL, "x:", "px", "y:", "px",
                    _("Sets the alignment hint by x axiss"), _("Sets the alignment hint by y axiss"),
                    100, double, VAL_DOUBLE)
PART_ITEM_ATTR_2SPINNER(_("weight"), part_item, weight_x, weight_y, part_item,
                    0.0, 999.0, 1.0, NULL, "x:", "", "y:", "",
                    _("Sets the weight hint by x axiss"),_("Sets the weight hint by y axiss"),
                    1, int, VAL_INT)
PART_ITEM_ATTR_2SPINNER(_("aspect"), part_item, aspect_w, aspect_h, part_item,
                    0.0, 999.0, 1.0, NULL, "w:", "", "h:", "",
                    _("Set the item aspect width hint"), _("Set the item aspect height hint"),
                    1, int, VAL_INT)
PART_ITEM_ATTR_2SPINNER(_("spread"), part_item, spread_w, spread_h, part_item,
                    1.0, 255.0, 1.0, NULL, "colunm:", "", "row:", "",
                    _("Replicate the item in width, starting from the current position"),
                    _("Replicate the item in height, starting from the current position"),
                    1, int, VAL_INT)
PART_ITEM_ATTR_1COMBOBOX_LIST(_("aspect mode"), part_item, aspect_mode, part_item, int, edje_item_aspect_pref,
                              _("Sets the aspect control hints for this object."))
PART_ITEM_DOUBLEVAL_ATTR_2SPINNER(_("span"), part_item, span, span1, part_item,
                                  1.0, 999.0, 1.0, NULL, "column:", "", "row:", "",
                                  _("Sets how many columns this item will use"), _("Sets how many rows this item will use"),
                                  1, unsigned char, VAL_INT)
PART_ITEM_DOUBLEVAL_ATTR_2SPINNER(_("position"), part_item, position, position1, part_item,
                                  0.0, 999.0, 1.0, NULL, "column:", "", "row:", "",
                                  _("Sets the column position this item"), _("Sets the row position this item"),
                                  1, unsigned short, VAL_INT)

Eina_Bool
ui_property_item_set(Evas_Object *property, Eina_Stringshare *item_name)
{
   Evas_Object *item;
   Evas_Object *box, *prop_box;

   PROP_DATA_GET()

   ui_property_item_unset(property);
   pd->item_name = item_name;
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_item.frame)
     {
        FRAME_ADD(property, pd_item.frame, true, _("Item"))
        BOX_ADD(pd_item.frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(pd_item.frame, box);

        item = prop_part_item_name_add(box, _("name"), pd->item_name);
        elm_box_pack_end(box, item);
        pd->part_item.source_item = prop_part_item_source_add(box, pd);
        elm_box_pack_end(box, pd->part_item.source_item);
        item = prop_part_item_min_w_min_h_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_item_max_w_max_h_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_item_prefer_w_prefer_h_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_item_align_x_align_y_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_item_weight_x_weight_y_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_item_aspect_mode_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_item_aspect_w_aspect_h_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_item_spread_w_spread_h_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_part_item_span_span1_add(box, pd);
        elm_box_pack_end(box, item);
        prop_part_item_padding_add(box, pd);

        pd_item.position_item = prop_part_item_position_position1_add(box, pd);
        evas_object_hide(pd_item.position_item);

        elm_box_pack_after(prop_box, pd_item.frame, pd->state_container.frame);
     }
   else
     {
        box = elm_object_content_get(pd->part_item.frame);
        prop_part_item_name_update(item_name);
        prop_part_item_source_update(pd);
        PART_ITEM_ATTR_2SPINNER_UPDATE(part_item, min_w, min_h, part_item, int,1)
        PART_ITEM_ATTR_2SPINNER_UPDATE(part_item, max_w, max_h, part_item, int,1)
        PART_ITEM_ATTR_2SPINNER_UPDATE(part_item, prefer_w, prefer_h, part_item, int, 1)
        PART_ITEM_ATTR_2SPINNER_UPDATE(part_item, align_x, align_y, part_item, double, 100)
        PART_ITEM_ATTR_2SPINNER_UPDATE(part_item, weight_x, weight_y, part_item, int, 1)
        PART_ITEM_ATTR_1COMBOBOX_LIST_UPDATE(part_item, aspect_mode, part_item)
        PART_ITEM_ATTR_2SPINNER_UPDATE(part_item, aspect_w, aspect_h, part_item, int, 1)
        PART_ITEM_ATTR_2SPINNER_UPDATE(part_item, spread_w, spread_h, part_item, int, 1)
        PART_ITEM_DOUBLEVAL_ATTR_2SPINNER_UPDATE(part_item, span, span1, part_item, unsigned char, 1);
        PART_ITEM_DOUBLEVAL_ATTR_2SPINNER_UPDATE(part_item, position, position1, part_item, unsigned short, 1);
        prop_part_item_padding_update(pd);

        elm_box_pack_after(prop_box, pd_item.frame, pd->state_container.frame);
        evas_object_show(pd_item.frame);
     }

     if (pd->wm_part->type == EDJE_PART_TYPE_TABLE)
       {
          if (!evas_object_visible_get(pd->part_item.position_item))
            {
               elm_box_pack_after(box, pd->part_item.position_item, pd->part_item.source_item);
               evas_object_show(pd->part_item.position_item);
            }
       }
     else
       {
          if (evas_object_visible_get(pd->part_item.position_item))
            {
               elm_box_unpack(box, pd->part_item.position_item);
               evas_object_hide(pd->part_item.position_item);
            }
       }

   return true;
}

void
ui_property_item_unset(Evas_Object *property)
{
   Evas_Object *prop_box, *item_box;

   PROP_DATA_GET()

   if (!pd_item.frame) return;

   pd->item_name = NULL;
   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_item.frame);

   item_box = elm_object_content_get(pd_item.frame);
   elm_box_unpack(item_box, pd_item.position_item);

   evas_object_hide(pd_item.frame);
   evas_object_hide(pd_item.position_item);
}
#undef pd_item

#define pd_container pd->state_container

#define STATE_DOUBLEVAL_ATTR_2SPINNER(TEXT, SUB, VALUE1, VALUE2, MEMBER, MIN, MAX, STEP, FMT, \
                                      L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2, MULTIPLIER, \
                                      TYPE, HISTORY_TYPE) \
   STATE_DOUBLEVAL_ATTR_2SPINNER_CALLBACK(SUB, VALUE1, VALUE2, MEMBER, TYPE, MULTIPLIER) \
   STATE_DOUBLEVAL_ATTR_2SPINNER_ADD(TEXT, SUB, VALUE1, VALUE2, MEMBER, TYPE, \
                                     MIN, MAX, STEP, FMT, L1_START, L1_END, L2_START, L2_END, \
                                     TOOLTIP1, TOOLTIP2, MULTIPLIER)
#define STATE_DOUBLEVAL_ATTR_2CHECK(TEXT, SUB, VALUE1, VALUE2, MEMBER, \
                                    L1_START, L1_END, L2_START, L2_END, TOOLTIP1, TOOLTIP2) \
   STATE_DOUBLEVAL_ATTR_2CHECK_CALLBACK(SUB, VALUE1, VALUE2, MEMBER) \
   STATE_DOUBLEVAL_ATTR_2CHECK_ADD(TEXT, SUB, VALUE1, VALUE2, MEMBER, \
                                   L1_START, L1_END, L2_START, L2_END, \
                                   TOOLTIP1, TOOLTIP2) \

STATE_DOUBLEVAL_ATTR_2SPINNER(_("align"), state_container, align, align1, state_container,
                              0.0, 100.0, 1.0, NULL, "x:", "%", "y:", "%",
                              _("Change the position of the point of balance inside the container"),
                              _("Change the position of the point of balance inside the container"),
                              100, double, VAL_DOUBLE)
STATE_DOUBLEVAL_ATTR_2SPINNER(_("padding"), state_container, padding, padding1, state_container,
                              0.0, 999.0, 1.0, NULL, "x:", "px", "y:", "px",
                              _("Sets the horizontal space between cells in pixels"),
                              _("Sets the vertcal space between cells in pixels"),
                              1, int, VAL_INT)
STATE_DOUBLEVAL_ATTR_2CHECK(_("min"), state_container, min, min1, state_container,
                            "x:", NULL, "y:", NULL,
                            _("This affects the minimum width calculation"),
                            _("This affects the minimum height calculation"))

static Eina_Bool
ui_property_state_container_set(Evas_Object *property)
{
   Evas_Object *item, *box, *prop_box;

   PROP_DATA_GET()

   ui_property_state_container_unset(property);
   prop_box = elm_object_content_get(pd->visual);
   if (!pd_container.frame)
     {
        FRAME_ADD(property, pd_container.frame, true, _("Container"))
        BOX_ADD(pd_container.frame, box, EINA_FALSE, EINA_FALSE)
        elm_box_align_set(box, 0.5, 0.0);
        elm_object_content_set(pd_container.frame, box);

        item = prop_state_container_align_align1_add(box, pd);
        elm_box_pack_end(box, item);
        item = prop_state_container_padding_padding1_add(box, pd);
        elm_box_pack_end(box, item);

        item = prop_state_container_min_min1_add(box, pd);
        elm_box_pack_end(box, item);
        elm_box_pack_after(prop_box, pd_container.frame, pd->state_object_area.frame);
     }
   else
     {
        STATE_DOUBLEVAL_ATTR_2SPINNER_UPDATE(state_container, align, align1, state_container, double, 100)
        STATE_DOUBLEVAL_ATTR_2SPINNER_UPDATE(state_container, padding, padding1, state_container, int, 1)
        STATE_DOUBLEVAL_ATTR_2CHECK_UPDATE(state_container, min, min1, state_container)
        elm_box_pack_after(prop_box, pd_container.frame, pd->state_object_area.frame);
        evas_object_show(pd_container.frame);
     }
   return true;
}
static void
ui_property_state_container_unset(Evas_Object *property)
{
   Evas_Object *prop_box;

   PROP_DATA_GET()

   prop_box = elm_object_content_get(pd->visual);
   elm_box_unpack(prop_box, pd_container.frame);
   evas_object_hide(pd_container.frame);
}

#undef pd_container

#undef PROP_DATA
#undef PROP_DATA_GET
