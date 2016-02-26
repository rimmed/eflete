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

#include "property_private.h"
#include "property_macros.h"
#include "main_window.h"
#include "history.h"
#include "project_manager.h"

#define DEMO_TEXT_PROP_DATA "image_prop_data"

#define DEMO_TEXT_PROP_DATA_GET() \
   assert(property != NULL); \
   Demo_Text_Prop_Data *pd = evas_object_data_get(property, DEMO_TEXT_PROP_DATA); \
   assert(pd != NULL);

struct _Demo_Text_Prop_Data
{
   Demo_Part *part;

   Evas_Object *scroller;
   Evas_Object *frame;
   Evas_Object *box;
   Evas_Object *name;
   Evas_Object *text;
};
typedef struct _Demo_Text_Prop_Data Demo_Text_Prop_Data;

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

static void
_on_part_name_change(void *data,
                     Evas_Object *obj __UNUSED__,
                     void *ei __UNUSED__)
{
   Demo_Text_Prop_Data *pd = (Demo_Text_Prop_Data *)data;
   const char *text;
   char *value;
   assert(pd != NULL);
   text = elm_entry_entry_get(obj);
   value = elm_entry_markup_to_utf8(text);

   if (pd->part->text_content)
     eina_stringshare_del(pd->part->text_content);
   pd->part->text_content = eina_stringshare_add(value);

   evas_object_smart_callback_call(ap.win, SIGNAL_DEMO_TEXT_SET, pd->part);

   free(value);
}

static Evas_Object *
prop_part_content_add(Evas_Object *parent, Demo_Text_Prop_Data *pd)
{
   assert(parent != NULL);
   assert(pd != NULL);

   PROPERTY_ITEM_ADD(parent,  _("content"), "1swallow");
   ENTRY_ADD(parent, pd->text, true);
   elm_object_tooltip_text_set(pd->text, _("Set text"));
   evas_object_smart_callback_add(pd->text, "changed,user", _on_part_name_change, pd);
   elm_layout_content_set(item, "elm.swallow.content", pd->text);

   return item;
}

void
ui_property_demo_text_part_set(Evas_Object *property, Demo_Part *part)
{
   DEMO_TEXT_PROP_DATA_GET()

   /* setting */
   if (part)
     {
        elm_object_text_set(pd->name, part->name);
        elm_entry_entry_set(pd->text, part->text_content);
     }

   pd->part = part;

   /* showing */
   evas_object_show(property);
}

Evas_Object *
ui_property_demo_text_add(Evas_Object *parent)
{
   Demo_Text_Prop_Data *pd;
   Evas_Object *item, *scroller, *box;

   assert(parent != NULL);

   pd = mem_calloc(1, sizeof(Demo_Text_Prop_Data));

   SCROLLER_ADD(parent, scroller);
   BOX_ADD(scroller, box, false, false);
   elm_box_align_set(box, 0.5, 0.0);
   elm_object_content_set(scroller, box);
   pd->scroller = scroller;
   elm_scroller_policy_set(pd->scroller, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_OFF);

   evas_object_data_set(pd->scroller, DEMO_TEXT_PROP_DATA, pd);

   /* editors */
   FRAME_ADD(box, pd->frame, true, _("Demo Text Property"))
   elm_box_pack_end(box, pd->frame);
   BOX_ADD(pd->frame, pd->box, false, false)
   elm_box_align_set(pd->box, 0.5, 0.0);
   evas_object_hide(pd->box);

   /* Frame with info */
   elm_box_align_set(pd->box, 0.5, 0.0);
   item = prop_item_label_add(pd->box, &pd->name, _("name"), _(" - "));
   elm_box_pack_end(pd->box, item);
   item = prop_part_content_add(pd->box, pd);
   elm_box_pack_end(pd->box, item);

   elm_object_content_set(pd->frame, pd->box);

   return pd->scroller;
}
