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

#include "container.h"
#include "cursor.h"

#define MY_CLASS_NAME "Container"

#define BASE_PADDING 10

typedef struct _Container_Smart_Data Container_Smart_Data;

static const char SIG_CHANGED[] = "container,changed";
static const char SIG_HANDLER_BR_MOVE[] = "handler,BR,moved";

static const char TEXT_TOOLTIP[] = "eflete.size.tooltip";
static const char SWALLOW[] = "eflete.swallow.container";

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_CHANGED, "(iiii)"},
   {SIG_HANDLER_BR_MOVE, "(ii)"},
   {NULL, NULL}
};

struct _Container_Smart_Data
{
   Evas_Object_Smart_Clipped_Data base;
   Evas *e;
   Evas_Object *obj;
   Evas_Object *container;
   Evas_Object *content;
   /* Minimal and maximum size of the container,
      i.e size of the edie_edit object */
   struct {
      Evas_Coord w; /* default: 0 */
      Evas_Coord h; /* default: 0 */
   } con_size_min;
   struct {
      Evas_Coord w; /* default: -1, size is not limited */
      Evas_Coord h; /* default: -1, size is not limited */
   } con_size_max;
   Container_Geom size;
   struct{
      Evas_Object *obj;
      Evas_Coord w, h;
   } handler_BR;
   double aspect;
   Eina_Stringshare *style;
   double zoom;
   Container_Geom zoom1;
   Evas_Coord downx;
   Evas_Coord downy;
   Evas_Coord dx, dy;
   Object_Protrusion_Get func;
   Eina_Bool handler_BR_pressed : 1;
   Eina_Bool lock : 1;
};

#define CONTAINER_DATA_GET(o, ptr) \
   assert(o != NULL); \
   Container_Smart_Data *ptr = evas_object_smart_data_get(o); \
   assert(ptr != NULL);

/* here we create and define some functions for creating our container smart
 * object that is inherited from general Smart Object. */
EVAS_SMART_SUBCLASS_NEW(MY_CLASS_NAME, _container,
                        Evas_Smart_Class, Evas_Smart_Class,
                        evas_object_smart_clipped_class_get, _smart_callbacks);

static void
_mouse_down_hRB_cb(void *data,
                   Evas *e __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   if (ev->button != 1) return;

   CONTAINER_DATA_GET(data, sd)
   sd->downx = ev->canvas.x;
   sd->downy = ev->canvas.y;
   sd->handler_BR_pressed = true;
   edje_object_signal_emit(sd->container, "tooltip,show", "eflete");
}

static void
_mouse_up_hRB_cb(void *data,
                 Evas *e __UNUSED__,
                 Evas_Object *obj __UNUSED__,
                 void *event_info __UNUSED__)
{
   CONTAINER_DATA_GET(data, sd)
   sd->handler_BR_pressed = false;
   edje_object_signal_emit(sd->container, "tooltip,hide", "eflete");
}

static void
_mouse_move_hBR_cb(void *data,
                   Evas *e __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Evas_Object *o = data;
   Evas_Event_Mouse_Move *ev = event_info;

   CONTAINER_DATA_GET(o, sd)
   if (!sd->handler_BR_pressed) return;

   sd->dx += ev->cur.canvas.x - sd->downx;
   sd->dy += ev->cur.canvas.y - sd->downy;
   sd->downx = ev->cur.canvas.x;
   sd->downy = ev->cur.canvas.y;

   evas_object_smart_callback_call(o, SIG_HANDLER_BR_MOVE, &sd->zoom1);
   evas_object_smart_changed(o);
}

static void
_style_set(Evas_Object *o, const char *style)
{
   char group[BUFF_MAX];
   CONTAINER_DATA_GET(o, sd)

   #define GROUP_NAME(item, style) \
      snprintf(group, BUFF_MAX, "eflete/container/%s/%s", item, style);

   GROUP_NAME("base", style)
   if (!edje_object_file_set(sd->container, ap.path.layout_edj, group))
     {
        GROUP_NAME("base", "default")
        if (!edje_object_file_set(sd->container, ap.path.layout_edj, group))
          {
             ERR("Couldn't load default style for base border of container.");
             abort();
          }
     }
   GROUP_NAME("handler_BR", style)
   if (!edje_object_file_set(sd->handler_BR.obj, ap.path.layout_edj, group))
     {
        GROUP_NAME("handler_BR", "default")
        if (!edje_object_file_set(sd->handler_BR.obj, ap.path.layout_edj, group))
          {
             ERR("Couldn't load default style for bottom-right handler of container.");
             abort();
          }
     }

   eina_stringshare_del(sd->style);
   sd->style = eina_stringshare_add(style);

   #undef GROUP_NAME
}

/* create and setup a new example smart object's internals */
static void
_container_smart_add(Evas_Object *o)
{
   EVAS_SMART_DATA_ALLOC(o, Container_Smart_Data)

   _container_parent_sc->add(o);
   priv->e = evas_object_evas_get(o);
   priv->container = edje_object_add(priv->e);

   priv->handler_BR.obj = edje_object_add(priv->e);
   priv->handler_BR.w = priv->handler_BR.h = 5;
   evas_object_event_callback_add(priv->handler_BR.obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  _mouse_down_hRB_cb, o);
   evas_object_event_callback_add(priv->handler_BR.obj, EVAS_CALLBACK_MOUSE_UP,
                                  _mouse_up_hRB_cb, o);
   evas_object_event_callback_add(priv->handler_BR.obj, EVAS_CALLBACK_MOUSE_MOVE,
                                  _mouse_move_hBR_cb, o);
   cursor_type_set(priv->handler_BR.obj, CURSOR_SIZING);

   priv->obj = o;
   priv->con_size_min.w = 0;
   priv->con_size_min.h = 0;
   priv->con_size_max.w = -1;
   priv->con_size_max.h = -1;
   priv->size.x = 0;
   priv->size.y = 0;
   priv->size.w = 0;
   priv->size.h = 0;
   priv->handler_BR_pressed = false;
   priv->aspect = -1.0;
   priv->zoom = 1.0;

   evas_object_smart_member_add(priv->container, o);
   evas_object_smart_member_add(priv->handler_BR.obj, o);

   _style_set(o, "default");
}

static void
_container_smart_del(Evas_Object *o)
{
   CONTAINER_DATA_GET(o, sd)

   eina_stringshare_del(sd->style);
   evas_object_del(sd->content);

   _container_parent_sc->del(o);
}

static void
_container_smart_show(Evas_Object *o)
{
   CONTAINER_DATA_GET(o, sd);

   if (!sd->lock) evas_object_show(sd->handler_BR.obj);

   evas_object_show(sd->container);
   _container_parent_sc->show(o);
}

static void
_container_smart_hide(Evas_Object *o)
{
   CONTAINER_DATA_GET(o, sd)

   evas_object_hide(sd->handler_BR.obj);
   evas_object_hide(sd->container);
   _container_parent_sc->hide(o);
}

static void
_container_smart_move(Evas_Object *o,
                      Evas_Coord x,
                      Evas_Coord y)
{
   Evas_Coord ox, oy;
   Groupview_Geom *geom = NULL;

   evas_object_geometry_get(o, &ox, &oy, NULL, NULL);
   if ((ox == x) && (oy == y)) return;

   CONTAINER_DATA_GET(o, sd)

   if (sd->func) geom = sd->func(sd->content);
   sd->size.x = ox + BASE_PADDING + (geom ? geom->x : 0);
   sd->size.y = oy + BASE_PADDING + (geom ? geom->y : 0);

   evas_object_move(sd->container, sd->size.x, sd->size.y);
   evas_object_move(sd->handler_BR.obj, sd->size.x + sd->size.w, sd->size.y + sd->size.h);

   evas_object_smart_callback_call(o, SIG_CHANGED, &sd->size);
}

static void
_container_smart_resize(Evas_Object *o,
                        Evas_Coord w,
                        Evas_Coord h)
{
   Evas_Coord ow, oh;
   evas_object_geometry_get(o, NULL, NULL, &ow, &oh);
   if ((ow == w) && (oh == h)) return;

   evas_object_smart_changed(o);
}

static void
_container_smart_calculate(Evas_Object *o)
{
   Evas_Coord x, y, w, h;
   Evas_Coord cw, ch;
   Evas_Coord hrb_x, hrb_y;
   Groupview_Geom *geom = NULL;
   int divisior = 1;
   char buff[16];

   CONTAINER_DATA_GET(o, sd)
   evas_object_geometry_get(o, &x, &y, &w, &h);

   /* 1. calculate the container size */
   sd->size.w += sd->zoom * (int)(sd->dx / sd->zoom);
   if (sd->aspect < 0)
     sd->size.h += sd->zoom * (int)(sd->dy / sd->zoom);
   else
     sd->size.h = sd->size.w / sd->aspect;

   /* check the boundary values, and force max size to be <= 9999 */
   if (sd->size.w > sd->size.h)
     {
        if (sd->size.w > 9999)
          {
             sd->size.w = 9999;
             sd->size.h = sd->size.w / sd->aspect;
          }
     }
   else
     {
        if (sd->size.h > 9999)
          {
             sd->size.h = 9999;
             sd->size.w = sd->size.h * sd->aspect;
          }
     }

   /* check the boundary size values */
   if (sd->size.w < 0) sd->size.w = 0;
   if (sd->size.h < 0) sd->size.h = 0;

   if ((sd->con_size_min.w > 0) && (sd->size.w < sd->con_size_min.w * sd->zoom))
     sd->size.w = sd->con_size_min.w * sd->zoom;
   if ((sd->con_size_min.h > 0) && (sd->size.h < sd->con_size_min.h * sd->zoom))
     sd->size.h = sd->con_size_min.h * sd->zoom;

   if ((sd->con_size_max.w > -1) && (sd->size.w > sd->con_size_max.w * sd->zoom))
     sd->size.w = sd->con_size_max.w * sd->zoom;
   if ((sd->con_size_max.h > -1) && (sd->size.h > sd->con_size_max.h * sd->zoom))
     sd->size.h = sd->con_size_max.h * sd->zoom;

   evas_object_resize(sd->container, sd->size.w, sd->size.h);
   evas_object_resize(sd->content, sd->size.w, sd->size.h);

   /* 2. get the content protrusion */
   if (sd->func) geom = sd->func(sd->content);

   /* 3. calculate the container position relative to content protrusion */
   sd->size.x = x + BASE_PADDING + (geom ? geom->x : 0);
   sd->size.y = y + BASE_PADDING + (geom ? geom->y : 0);
   evas_object_move(sd->container, sd->size.x, sd->size.y);

   /* 4. move the handler */
   hrb_x = sd->size.x + sd->size.w;
   hrb_y = sd->size.y + sd->size.h;
   evas_object_resize(sd->handler_BR.obj, sd->handler_BR.w, sd->handler_BR.h);
   evas_object_move(sd->handler_BR.obj, hrb_x, hrb_y);

   /* 5. calculate the full object size */
   cw = (hrb_x + sd->handler_BR.w + (geom ? geom->w : 0) + BASE_PADDING) - x;
   ch = (hrb_y + sd->handler_BR.h + (geom ? geom->h : 0) + BASE_PADDING) - y;
   evas_object_size_hint_min_set(o, cw, ch);
   evas_object_size_hint_max_set(o, cw, ch);

   if (fabs(sd->zoom - 1.0) < DBL_EPSILON)
     {
        sd->zoom1.w = sd->size.w;
        sd->zoom1.h = sd->size.h;
     }
   else if (sd->handler_BR_pressed)
     {
        sd->zoom1.w = sd->size.w / sd->zoom;
        sd->zoom1.h = sd->size.h / sd->zoom;
     }
   sd->zoom1.x = sd->size.x;
   sd->zoom1.y = sd->size.y;
   snprintf(buff, 16, "%i %i", sd->zoom1.w, sd->zoom1.h);
   edje_object_part_text_set(sd->container, TEXT_TOOLTIP, buff);

   evas_object_smart_callback_call(o, SIG_CHANGED, &sd->zoom1);

   if ((int)sd->zoom > 1) divisior = (int)sd->zoom;
   sd->dx = sd->dx % divisior;
   sd->dy = sd->dy % divisior;
}

/* this need for macro EVAS_SMART_SUBCLASS_NEW */
static void
_container_smart_set_user(Evas_Smart_Class *sc)
{
   assert(sc != NULL);

   sc->add = _container_smart_add;
   sc->del = _container_smart_del;
   sc->show = _container_smart_show;
   sc->hide = _container_smart_hide;

   /* clipped smart object has no hook on resizes or calculations */
   sc->move = _container_smart_move;
   sc->resize = _container_smart_resize;
   sc->calculate = _container_smart_calculate;
}

Evas_Object *
container_add(Evas_Object *parent)
{
   Evas *e;
   Evas_Object *obj;

   assert(parent != NULL);

   e = evas_object_evas_get(parent);
   obj = evas_object_smart_add(e, _container_smart_class_new());

   return obj;
}

Eina_Bool
container_handler_size_set(Evas_Object *obj, int hrb_w, int hrb_h)
{
   CONTAINER_DATA_GET(obj, sd);

   if (hrb_w < 5) sd->handler_BR.w = 5;
   else sd->handler_BR.w = hrb_w;
   if (hrb_h < 5) sd->handler_BR.h = 5;
   else sd->handler_BR.h = hrb_h;

   return true;
}

Eina_Bool
container_handler_size_get(Evas_Object *obj, int *hbr_w, int *hbr_h)
{
   CONTAINER_DATA_GET(obj, sd);

   if (hbr_w) *hbr_w = sd->handler_BR.w;
   if (hbr_h) *hbr_h = sd->handler_BR.h;

   return true;
}

Eina_Bool
container_min_size_set(Evas_Object *obj, int w, int h)
{
   CONTAINER_DATA_GET(obj, sd);

   if (w < 0) sd->con_size_min.w = 0;
   else sd->con_size_min.w = w;
   if (h < 0) sd->con_size_min.h = 0;
   else sd->con_size_min.h = h;

   if (sd->size.w <= w) sd->size.w = w;
   if (sd->size.h <= h) sd->size.h = h;

   evas_object_smart_changed(obj);
   return true;
}

Eina_Bool
container_max_size_set(Evas_Object *obj, int w, int h)
{
   CONTAINER_DATA_GET(obj, sd);

   sd->con_size_max.w = w;
   sd->con_size_max.h = h;

   if ((w > -1) && (sd->size.w > w))
     sd->size.w = w;
   if ((h > -1) && (sd->size.h > h))
     sd->size.h = h;

   evas_object_smart_changed(obj);
   return true;
}

Eina_Bool
container_container_size_set(Evas_Object *obj, int w, int h)
{
   CONTAINER_DATA_GET(obj, sd);

   INFO ("%d\t%d", w, h);
   if (w <= sd->con_size_min.w) sd->size.w = sd->con_size_min.w;
   else
     {
        if ((sd->con_size_max.w > -1) && (w > sd->con_size_max.w))
          sd->size.w = sd->con_size_max.w;
        else sd->size.w = w;
     }

   if (h <= sd->con_size_min.h) sd->size.h = sd->con_size_min.h;
   else
     {
        if ((sd->con_size_max.h > -1) && (h > sd->con_size_max.h))
          sd->size.h = sd->con_size_max.h;
        else sd->size.h = h;
     }

   sd->zoom1.w = w;
   sd->zoom1.h = h;
   sd->size.w *= sd->zoom;
   sd->size.h *= sd->zoom;

   evas_object_smart_changed(obj);
   return true;
}

Container_Geom *
container_geom_get(Evas_Object *obj)
{
   CONTAINER_DATA_GET(obj, sd);

   return &sd->size;
}

Eina_Bool
container_style_set(Evas_Object *obj, const char *style)
{
   CONTAINER_DATA_GET(obj, sd);

   assert(style != NULL);

   if (!strcmp(sd->style, style)) return false;
   _style_set(obj, style);

   return true;
}

const char *
container_style_get(Evas_Object *obj)
{
   CONTAINER_DATA_GET(obj, sd);
   return sd->style;
}

Eina_Bool
container_content_set(Evas_Object *obj, Evas_Object *content)
{
   CONTAINER_DATA_GET(obj, sd);

   assert(content != NULL);

   edje_object_part_swallow(sd->container, SWALLOW, content);
   sd->content = content;

   return true;
}

Evas_Object *
container_content_unset(Evas_Object *obj)
{
   Evas_Object *ret;
   CONTAINER_DATA_GET(obj, sd);

   ret = edje_object_part_swallow_get(sd->container, SWALLOW);
   edje_object_part_unswallow(sd->container, ret);
   sd->content = NULL;
   return ret;
}

void
container_lock_set(Evas_Object *obj, Eina_Bool lock)
{
   CONTAINER_DATA_GET(obj, sd);

   sd->lock = lock;

   if (lock)
      evas_object_hide(sd->handler_BR.obj);
   else
      evas_object_show(sd->handler_BR.obj);
}

Eina_Bool
container_lock_get(Evas_Object *obj)
{
   CONTAINER_DATA_GET(obj, sd);

   return sd->lock;
}

Eina_Bool
container_padding_size_get(Evas_Object *obj, int *tl_w, int *tl_h, int *br_w, int *br_h)
{
   CONTAINER_DATA_GET(obj, sd);

   if (tl_w) *tl_w = BASE_PADDING;
   if (tl_h) *tl_h = BASE_PADDING;
   if (br_w) *br_w = BASE_PADDING;
   if (br_h) *br_h = BASE_PADDING;

   return true;
}

void
container_protrusion_func_set(Evas_Object *obj, Object_Protrusion_Get func)
{
   CONTAINER_DATA_GET(obj, sd);

   sd->func = func;
}

void
container_aspect_set(Evas_Object *obj, double aspect)
{
   CONTAINER_DATA_GET(obj, sd);

   sd->aspect = aspect;

   evas_object_smart_changed(obj);
}

double
container_aspect_get(Evas_Object *obj)
{
   CONTAINER_DATA_GET(obj, sd);

   return sd->aspect;
}

void
container_zoom_factor_set(Evas_Object *obj, double zoom)
{
   CONTAINER_DATA_GET(obj, sd);

   sd->zoom = zoom;
   container_container_size_set(obj, sd->zoom1.w, sd->zoom1.h);
}

double
container_zoom_factor_get(Evas_Object *obj)
{
   CONTAINER_DATA_GET(obj, sd);

   return sd->zoom;
}
