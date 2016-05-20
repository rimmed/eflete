/*
 * Edje Theme Editor
 * Copyright (C) 2013-2016 Samsung Electronics.
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

#include "property.h"
#include "property_private.h"
#include "demo_group.h"

struct _Property_Demo_Data {
   Demo_Part *part;
   Demo_Signal *signal;
   Property_Attribute items[PROPERTY_DEMO_ITEM_LAST];
};
typedef struct _Property_Demo_Data Property_Demo_Data;
static Property_Demo_Data group_pd;

struct _Property_Demo_Update_Info {
   Property_Attribute *pa;
   Property_Action *action;
};
typedef struct _Property_Demo_Update_Info Property_Demo_Update_Info;

/* array to find item by Attribute */
static Property_Demo_Update_Info attribute_map[PROPERTY_DEMO_ITEM_LAST];

/* local callbacks */
static Eina_List *
_subitems_get(Property_Attribute *pa)
{
   Eina_List *items = NULL;

   assert(pa != NULL);
#define APPEND(TYPE) items = eina_list_append(items, &group_pd.items[TYPE]);
   switch (pa->type.group_item)
     {
      case PROPERTY_DEMO_ITEM_TEXT_TITLE:
         APPEND(PROPERTY_DEMO_ITEM_TEXT_NAME);
         APPEND(PROPERTY_DEMO_ITEM_TEXT_CONTENT);
         break;
      case PROPERTY_DEMO_ITEM_SWALLOW_TITLE:
         APPEND(PROPERTY_DEMO_ITEM_SWALLOW_NAME);
         APPEND(PROPERTY_DEMO_ITEM_SWALLOW_CONTENT);
         APPEND(PROPERTY_DEMO_ITEM_SWALLOW_PICTURE);
         APPEND(PROPERTY_DEMO_ITEM_SWALLOW_WIDGET);
         APPEND(PROPERTY_DEMO_ITEM_SWALLOW_STYLE);
         APPEND(PROPERTY_DEMO_ITEM_SWALLOW_RECTANGLE);
         APPEND(PROPERTY_DEMO_ITEM_SWALLOW_MIN);
         APPEND(PROPERTY_DEMO_ITEM_SWALLOW_MAX);
         break;
      case PROPERTY_DEMO_ITEM_PROGRAM_TITLE:
         APPEND(PROPERTY_DEMO_ITEM_PROGRAM_SIGNAL);
         APPEND(PROPERTY_DEMO_ITEM_PROGRAM_SOURCE);
         APPEND(PROPERTY_DEMO_ITEM_PROGRAM_ACTION);
         APPEND(PROPERTY_DEMO_ITEM_PROGRAM_EMIT);
         APPEND(PROPERTY_DEMO_ITEM_PROGRAM_EMITTER);
         break;
      default:
         CRIT("items callback not found for %s", pa->name);
         abort();
     }

   return items;
#undef APPEND
}

static void
_init_cb(Property_Attribute *pa, Property_Action *action)
{
   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);

   switch (pa->type.demo_item)
     {
      case PROPERTY_DEMO_ITEM_TEXT_NAME:
      case PROPERTY_DEMO_ITEM_TEXT_CONTENT:
      case PROPERTY_DEMO_ITEM_SWALLOW_NAME:
      case PROPERTY_DEMO_ITEM_SWALLOW_CONTENT:
      case PROPERTY_DEMO_ITEM_SWALLOW_PICTURE:
      case PROPERTY_DEMO_ITEM_SWALLOW_WIDGET:
      case PROPERTY_DEMO_ITEM_SWALLOW_STYLE:
      case PROPERTY_DEMO_ITEM_SWALLOW_RECTANGLE:
      case PROPERTY_DEMO_ITEM_SWALLOW_MIN:
      case PROPERTY_DEMO_ITEM_SWALLOW_MAX:
      case PROPERTY_DEMO_ITEM_PROGRAM_SIGNAL:
      case PROPERTY_DEMO_ITEM_PROGRAM_SOURCE:
      case PROPERTY_DEMO_ITEM_PROGRAM_ACTION:
      case PROPERTY_DEMO_ITEM_PROGRAM_EMIT:
      case PROPERTY_DEMO_ITEM_PROGRAM_EMITTER:
         break;

      default:
         TODO("remove default case after all attributes will be added");
         CRIT("init callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
}

/* blocks */
static inline void
_action_internal(Property_Action *action, const char *name, const char *units,
                 Property_Control control_type)
{
   /* check that we haven't inited this action already */
   assert(action->update_cb == NULL);

   action->name = name;
   action->units = units;
   action->control_type = control_type;
   action->init_cb = _init_cb;
}

static inline void
_action1(Property_Attribute *pa, const char *name, const char *units,
         Property_Control control_type)
{
   _action_internal(&pa->action1, name, units, control_type);

   assert(attribute_map[control_type].pa == NULL);
   assert(attribute_map[control_type].action == NULL);

   attribute_map[control_type].pa = pa;
   attribute_map[control_type].action = &pa->action1;
}

static inline void
_action2(Property_Attribute *pa, const char *name, const char *units,
         Property_Control control_type)
{
   _action_internal(&pa->action2, name, units, control_type);

   assert(attribute_map[control_type].pa == NULL);
   assert(attribute_map[control_type].action == NULL);

   attribute_map[control_type].pa = pa;
   attribute_map[control_type].action = &pa->action2;
}

static void
_init_items()
{
   Property_Demo_Item it;
#define IT group_pd.items[it]
   for (it = 0 /* first element of enum */; it < PROPERTY_DEMO_ITEM_LAST; it++)
     {
        switch(it)
          {
           case PROPERTY_DEMO_ITEM_TEXT_TITLE:
              IT.name = "text";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_DEMO_ITEM_TEXT_NAME:
              break;
           case PROPERTY_DEMO_ITEM_TEXT_CONTENT:
              break;

           case PROPERTY_DEMO_ITEM_SWALLOW_TITLE:
              IT.name = "swallow";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_DEMO_ITEM_SWALLOW_NAME:
              break;
           case PROPERTY_DEMO_ITEM_SWALLOW_CONTENT:
              break;
           case PROPERTY_DEMO_ITEM_SWALLOW_PICTURE:
              break;
           case PROPERTY_DEMO_ITEM_SWALLOW_WIDGET:
              break;
           case PROPERTY_DEMO_ITEM_SWALLOW_STYLE:
              break;
           case PROPERTY_DEMO_ITEM_SWALLOW_RECTANGLE:
              break;
           case PROPERTY_DEMO_ITEM_SWALLOW_MIN:
              break;
           case PROPERTY_DEMO_ITEM_SWALLOW_MAX:
              break;

           case PROPERTY_DEMO_ITEM_PROGRAM_TITLE:
              IT.name = "program";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_DEMO_ITEM_PROGRAM_SIGNAL:
              break;
           case PROPERTY_DEMO_ITEM_PROGRAM_SOURCE:
              break;
           case PROPERTY_DEMO_ITEM_PROGRAM_ACTION:
              break;
           case PROPERTY_DEMO_ITEM_PROGRAM_EMIT:
              break;
           case PROPERTY_DEMO_ITEM_PROGRAM_EMITTER:
              break;

           case PROPERTY_DEMO_ITEM_LAST:
              break;
          }
     }
#undef IT
}

/* public */
void
property_demo_init(Property_Data *pd)
{
   assert(pd != NULL);

   _init_items();

   /* register global callbacks */
//   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_TEXT_PART_CLICKED, _on_text_part_clicked, pd->layout);
//   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_SWALLOW_PART_CLICKED, _on_swallow_part_clicked, pd->layout);
//   evas_object_smart_callback_add(ap.win, SIGNAL_DEMO_PROGRAM_PART_CLICKED, _on_demo_program_clicked, pd->layout);
}

Eina_List *
property_demo_items_get()
{
   Eina_List *items = NULL;

   items = eina_list_append(items, &group_pd.items[PROPERTY_DEMO_ITEM_TEXT_TITLE]);
   items = eina_list_append(items, &group_pd.items[PROPERTY_DEMO_ITEM_SWALLOW_TITLE]);
   items = eina_list_append(items, &group_pd.items[PROPERTY_DEMO_ITEM_PROGRAM_TITLE]);

   return items;
}