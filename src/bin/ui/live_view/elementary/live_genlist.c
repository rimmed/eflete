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

#include "live_elementary_widgets.h"
#include "widget_macro.h"

static void
_on_genlist_swallow_check(void *data __UNUSED__,
                          Evas_Object *obj __UNUSED__,
                          void *ei __UNUSED__)
{
   Demo_Part *part = (Demo_Part *)ei;
   Evas_Object *object = (Evas_Object *)data;
   Eina_List *part_list = evas_object_data_get(object, SWALLOW_LIST);

   if (!eina_list_data_find(part_list, part->name))
     part_list =  eina_list_append(part_list, part);
   evas_object_data_set(object, SWALLOW_LIST, part_list);

   Elm_Object_Item *item = NULL;
   item = elm_genlist_first_item_get(object);
   while (item)
     {
        elm_genlist_item_update(item);
        item = elm_genlist_item_next_get(item);
     }
}

static void
_on_genlist_text_check(void *data,
                       Evas_Object *obj __UNUSED__,
                       void *ei)
{
   Demo_Part *part = (Demo_Part *)ei;
   Evas_Object *object = (Evas_Object *)data;
   Eina_List *part_list = evas_object_data_get(object, TEXT_LIST);

   if (!eina_list_data_find(part_list, part->name))
     part_list =  eina_list_append(part_list, part);
   evas_object_data_set(object, TEXT_LIST, part_list);

   Elm_Object_Item *item = NULL;
   item = elm_genlist_first_item_get(object);
   while (item)
     {
        elm_genlist_item_update(item);
        item = elm_genlist_item_next_get(item);
     }
}

static void
_genlist_send_signal(void *data,
                     Evas_Object *obj __UNUSED__,
                     void *ei __UNUSED__)
{
   Demo_Signal *sig = (Demo_Signal *)ei;
   Elm_Object_Item *item = NULL;

   assert(data != NULL);

   item = elm_genlist_first_item_get(data);
   assert(sig != NULL);
   assert(sig->sig_name != NULL);
   assert(sig->source_name != NULL);

   while (item)
     {
        elm_object_item_signal_emit(item, sig->sig_name, sig->source_name);
        item = elm_genlist_item_next_get(item);
     }
}

/*********** GEN LIST CREATING FUNCTIONS ****************/
static char *
_glist_text_get(void        *data __UNUSED__,
                Evas_Object *obj,
                const char  *part)
{
   Eina_List *part_list = evas_object_data_get(obj, TEXT_LIST);
   Demo_Part *demo_part;
   Eina_List *l = NULL;


   EINA_LIST_FOREACH(part_list, l, demo_part)
     {
        if (!strcmp(demo_part->name, part))
          {
             if (!demo_part->text_content)
               return NULL;
             else
               return strdup(demo_part->text_content);
          }
     }

   return NULL;
}

static Evas_Object *
_glist_content_get(void *data __UNUSED__,
                   Evas_Object *obj,
                   const char  *part)
{
   Eina_List *part_list = evas_object_data_get(obj, SWALLOW_LIST);
   Demo_Part *demo_part;
   Eina_List *l = NULL;

   EINA_LIST_FOREACH(part_list, l, demo_part)
     {
        if (!strcmp(demo_part->name, part))
          {
             if ((demo_part->swallow_content == CONTENT_NONE) && (demo_part->object))
               demo_part->object = NULL;
             else if (demo_part->swallow_content != CONTENT_NONE)
               demo_part->object = object_generate(demo_part, obj);

             if (demo_part->object)
               {
                  evas_object_color_set(demo_part->object,
                                        demo_part->r,
                                        demo_part->g,
                                        demo_part->b,
                                        demo_part->a);

                  evas_object_size_hint_min_set(demo_part->object,
                                                demo_part->min_w,
                                                demo_part->min_h);
                  evas_object_size_hint_max_set(demo_part->object,
                                                demo_part->max_w,
                                                demo_part->max_h);
               }

             return demo_part->object;
          }
     }

   return NULL;
}


static void
_glist_expanded_cb(void *data __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Elm_Object_Item *tree_main = (Elm_Object_Item *)event_info;
   Elm_Genlist_Item_Class *ic = NULL;

   int i = 0;

   if (!ic)
     {
        ic = elm_genlist_item_class_new();
        ic->item_style = "default";
        ic->func.text_get = _glist_text_get;
        ic->func.content_get = _glist_content_get;
        ic->func.state_get = NULL;
        ic->func.del = NULL;
     }
   for (i = 0; i < ELEMENTS_COUNT; i++)
     elm_genlist_item_append(obj, ic, eina_stringshare_printf("Item #%d", i),
                             tree_main, ELM_GENLIST_ITEM_NONE, NULL, NULL);
   elm_genlist_item_class_free(ic);
}

static void
_glist_contracted_cb(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
_expand_request_cb(void *data __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, true);
}

static void
_contract_request_cb(void *data __UNUSED__,
                     Evas_Object *obj __UNUSED__,
                     void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, false);
}

static Evas_Object *
_create_genlist(Evas_Object *obj, const char *class, const char *style)
{
   Elm_Genlist_Item_Class *ic = NULL;
   unsigned int i = 0;
   unsigned int count_split = 0;
   Elm_Genlist_Item_Type type = 0;
   char **style_parsed = NULL;
   char *genlist_style;
   char *item_style;
   Evas_Object *glist = NULL;

   assert(obj != NULL);
   assert(class != NULL);
   assert(style != NULL);

   glist = elm_genlist_add(obj);
   evas_object_size_hint_align_set(glist, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(glist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   if (strcmp(class, "base") != 0)
     {
        if (strstr(class, "tree")) type = ELM_GENLIST_ITEM_TREE;
        else if (strstr(class, "group")) type = ELM_GENLIST_ITEM_GROUP;
        else type = ELM_GENLIST_ITEM_NONE;
        if (strstr(class, "compress"))
          elm_genlist_mode_set(glist, ELM_LIST_COMPRESS);
        else
          elm_genlist_mode_set(glist, ELM_LIST_SCROLL);

        style_parsed = eina_str_split_full(style, "/", 2, &count_split);
        if (count_split == 2)
          {
             item_style = strdup(style_parsed[0]);
             genlist_style = strdup(style_parsed[1]);
          }
        else
          {
             item_style = strdup("default");
             genlist_style = strdup(style);
          }
        free(style_parsed[0]);
        free(style_parsed);
     }
   else
     {
        genlist_style = strdup(style);
        item_style = strdup("default");
     }

   if (!ic)
     {
        ic = elm_genlist_item_class_new();
        ic->item_style = strdup(item_style);
        ic->func.text_get = _glist_text_get;
        ic->func.content_get = _glist_content_get;
        ic->func.state_get = NULL;
        ic->func.del = NULL;
     }

   for (i = 0; i < ELEMENTS_COUNT; i++)
     elm_genlist_item_append(glist, ic, NULL,
                             NULL, type, NULL, NULL);

   if (type ==  ELM_GENLIST_ITEM_TREE)
     {
        evas_object_smart_callback_add(glist, "expand,request",
                                       _expand_request_cb, NULL);
        evas_object_smart_callback_add(glist, "contract,request",
                                       _contract_request_cb, NULL);
        evas_object_smart_callback_add(glist, "expanded",
                                       _glist_expanded_cb, ic);
        evas_object_smart_callback_add(glist, "contracted",
                                       _glist_contracted_cb, NULL);
     }

   elm_object_style_set(glist, genlist_style);
   elm_genlist_item_class_free(ic);
   free(item_style);
   free(genlist_style);
   return glist;
}

/*********** GEN LIST CREATING FUNCTIONS ****END*********/

Evas_Object *
widget_genlist_create(Evas_Object *parent, const Group *group)
{
   assert(parent != NULL);
   assert(group != NULL);
   assert(group->style != NULL);
   assert(group->class != NULL);

   Evas_Object *object;
   Eina_List *swallow_list = NULL, *text_list = NULL;

   object = _create_genlist(parent, group->class, group->style);
   evas_object_show(object);

   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_SWALLOW_SET, _on_genlist_swallow_check, object);
   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_TEXT_SET, _on_genlist_text_check, object);
   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_SIGNAL_SEND, _genlist_send_signal, object);

   evas_object_data_set(object, SWALLOW_LIST, swallow_list);
   evas_object_data_set(object, TEXT_LIST, text_list);

   return object;
}
