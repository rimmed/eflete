/**
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

#ifndef COMMON_MACRO_H
#define COMMON_MACRO_H

#define HIGHLIGHT_BG_COLOR 0,0,0,0
#define HIGHLIGHT_COLOR 58,110,155,255
#define OBG_AREA_BG_COLOR 64,64,64,64
#define OBG_AREA_COLOR 0,0,0,255

#define true EINA_TRUE
#define false EINA_FALSE
#define RETURN_VOID

#define FLOAT_NUMBER_REGEX "^[0-9]*(\\.[0-9]+)?$"
#define FLOAT_NUMBER_0_1_REGEX "^(0?(\\.[0-9]+)?|1|1\\.0)?$"
#define EDJE_NAME_REGEX "^[^\\\"]*$"

#define STATUSBAR_PROJECT_PATH(AP, TEXT) \
   Evas_Object *label = ewe_statusbar_item_content_get( \
         eina_list_data_get(ewe_statusbar_items_list_get(AP->statusbar))); \
   elm_object_text_set(label, TEXT);

/* Getting first object from project. Needed to access top-level blocks */
#define GET_OBJ(PROJECT, EDJE_OBJECT) \
   Eina_Inlist *_styles, *_classes, *_widgets = NULL; \
   Widget *_widget; \
   Class *_class; \
   Style *_style; \
   _widgets = PROJECT->widgets; \
   if (!_widgets) EDJE_OBJECT = NULL; \
   else\
     { \
         _widget = EINA_INLIST_CONTAINER_GET(_widgets, Widget); \
         _classes = _widget->classes; \
         if (!_classes) EDJE_OBJECT = NULL; \
         else \
           { \
               _class = EINA_INLIST_CONTAINER_GET(_classes, Class); \
               _styles = _class->styles; \
               if (!_styles) EDJE_OBJECT = NULL; \
               else\
                 { \
                     _style = EINA_INLIST_CONTAINER_GET(_styles, Style); \
                     EDJE_OBJECT = _style->obj; \
                 } \
           } \
     } \
   if ((!EDJE_OBJECT) && (PROJECT->layouts)) \
     { \
       _style = EINA_INLIST_CONTAINER_GET(PROJECT->layouts, Style); \
       EDJE_OBJECT = _style->obj;\
     }

#define GET_STYLE(PROJECT, STYLE) \
   Eina_Inlist *_styles, *_classes, *_widgets = NULL; \
   Widget *_widget; \
   Class *_class; \
   Style *_style; \
   _widgets = PROJECT->widgets; \
   if (!_widgets) STYLE = NULL; \
   else\
     { \
         _widget = EINA_INLIST_CONTAINER_GET(_widgets, Widget); \
         _classes = _widget->classes; \
         if (!_classes) STYLE = NULL; \
         else \
           { \
               _class = EINA_INLIST_CONTAINER_GET(_classes, Class); \
               _styles = _class->styles; \
               if (!_styles) STYLE = NULL; \
               else\
                 { \
                     _style = EINA_INLIST_CONTAINER_GET(_styles, Style); \
                     STYLE = _style; \
                 } \
           } \
     } \
   if ((!STYLE) && (PROJECT->layouts)) \
     { \
       _style = EINA_INLIST_CONTAINER_GET(PROJECT->layouts, Style); \
       STYLE = _style;\
     }
#endif /* COMMON_MACRO_H */
