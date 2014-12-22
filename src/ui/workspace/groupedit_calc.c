/*
 * Edje Theme Editor
 * Copyright (C) 2013 Samsung Electronics.
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

#include "groupedit_private.h"
#include "alloc.h"

#define PART_STATE_GET(obj, part) \
   const char *state; \
   double value; \
   state = edje_edit_part_selected_state_get(obj, part, &value);

#define PART_STATE_FREE \
   edje_edit_string_free(state);

static void
_groupedit_part_free(Groupedit_Part *gp);

static Groupedit_Part *
_part_draw_add(Ws_Groupedit_Smart_Data *sd, const char *part, Edje_Part_Type type);

static void
_move_border_to_top(Ws_Groupedit_Smart_Data *sd);

static void
_part_draw_del(Ws_Groupedit_Smart_Data *sd, const char *part);

static Evas_Object *
_part_spacer_add(Evas *e);

static Evas_Object *
_part_swallow_add(Evas *e);

static void
_part_object_area_calc(Ws_Groupedit_Smart_Data *sd);

static void
_rectangle_param_update(Groupedit_Part *gp, Evas_Object *edit_obj);

static void
_image_param_update(Groupedit_Part *gp, Evas_Object *edit_obj, const char *file);

static void
_text_param_update(Groupedit_Part *gp, Evas_Object *edit_obj);

static void
_textblock_param_update(Groupedit_Part *gp, Evas_Object *edit_obj);

void
_edit_object_load(Ws_Groupedit_Smart_Data *sd)
{
   Eina_List *parts_list, *l;
   Edje_Part_Type ept;
   const char *state, *name, *text;
   double value;

   parts_list = edje_edit_parts_list_get(sd->edit_obj);
   EINA_LIST_FOREACH(parts_list, l, name)
     {
        ept = edje_edit_part_type_get(sd->edit_obj, name);
        state = edje_edit_part_selected_state_get(sd->edit_obj, name, &value);
        if (ept == EDJE_PART_TYPE_TEXT || ept == EDJE_PART_TYPE_TEXTBLOCK)
          {
             text = edje_edit_state_text_get(sd->edit_obj, name, state, value);
             if (text) edje_object_part_text_set(sd->edit_obj, name, text);
             else edje_object_part_text_set(sd->edit_obj, name, name);
             edje_edit_string_free(text);
          }
        edje_edit_string_free(state);
     }
   edje_edit_string_list_free(parts_list);
}

Eina_Bool
_edit_object_part_add(Ws_Groupedit_Smart_Data *sd, const char *part,
                      Edje_Part_Type type, const char *data)
{
   Groupedit_Part *gp;

   if (!part) return false;
   if ((type > EDJE_PART_TYPE_LAST)) return false;
   if (!edje_edit_part_add(sd->edit_obj, part, type))
     {
        ERR("Cann't add part %s to edit object %p", part, sd->edit_obj);
        return false;
     }
   if ((type == EDJE_PART_TYPE_IMAGE) && (data))
     edje_edit_state_image_set(sd->edit_obj, part, "default", 0.0, data);
   if (type == EDJE_PART_TYPE_TEXT)
     {
        edje_edit_state_font_set(sd->edit_obj, part, "default", 0.0, "Sans");
        edje_edit_state_text_size_set(sd->edit_obj, part, "default", 0.0, 10);
        edje_object_part_text_set(sd->edit_obj, part, part);
     }

   gp = _part_draw_add(sd, part, type);
   sd->parts = eina_list_append(sd->parts, gp);
   _move_border_to_top(sd);
   evas_object_smart_changed(sd->obj);

   return true;
}

Eina_Bool
_edit_object_part_del(Ws_Groupedit_Smart_Data *sd, const char *part)
{
   if ((!sd->parts) || (!part)) return false;
   if (!edje_edit_part_del(sd->edit_obj, part)) return false;

   _selected_item_return_to_place(sd);
   _part_draw_del(sd, part);
   evas_object_smart_changed(sd->obj);

   return true;
}

static void
_part_parts_layouts_update(Ws_Groupedit_Smart_Data *sd,
                           Groupedit_Part *ge_part,
                           Groupedit_Part *ge_part_to,
                           Eina_Bool mode)
{
   if (mode)
     {
        evas_object_stack_below(sd->bg, ge_part_to->draw);
        evas_object_stack_below(sd->clipper, ge_part_to->draw);
        evas_object_stack_below(ge_part->draw, ge_part_to->draw);
        evas_object_stack_below(ge_part->border, ge_part_to->draw);
        evas_object_stack_below(ge_part->item, ge_part_to->draw);
     }
   else
     {
        evas_object_stack_above(sd->bg, ge_part_to->border);
        evas_object_stack_above(sd->clipper, ge_part_to->border);
        evas_object_stack_above(ge_part->item, ge_part_to->border);
        evas_object_stack_above(ge_part->border, ge_part_to->border);
        evas_object_stack_above(ge_part->draw, ge_part_to->border);
     }
}

/**
  * Internal function, which provide restack parts at parts list and restack
  * part primitives at canvas.
  * If param part_to is NULL, part, which name stored at param part, will
  * restack below or above at one position. Else part will restack to part which
  * name stored at part_to.
  *
  * @param sd The groupedit smart data structure for get access to parts list.
  * @param part The name of part to stack.
  * @param part_to The name of part which to stack.
  * @param mode If true operation restack above will be process, else is
  * restack below.
  *
  * @return false on failure, true on success.
  */
static Eina_Bool
_part_restack(Ws_Groupedit_Smart_Data *sd,
              const char *part,
              const char *part_to,
              Eina_Bool mode)
{
   Eina_List *l = NULL;
   Groupedit_Part *ge_part_to, *ge_part;

   ge_part = _parts_list_find(sd->parts, part);
   if (!ge_part) return false;
   if (sd->separated) sd->to_select = ge_part;
   /* Here find part_to in parts stack. ge_part will be restack abow or below
    * ge_part_to object. */
   if (part_to)
     ge_part_to = _parts_list_find(sd->parts, part_to);
   else
     {
        l = eina_list_data_find_list(sd->parts, ge_part);
        if (mode)
          ge_part_to = eina_list_data_get(eina_list_prev(l));
        else
          ge_part_to = eina_list_data_get(eina_list_next(l));
     }

   if (!ge_part_to) return false;

   /* For changing position ge_part in Eina_List of parts, at first need to
    * remove node with data ge_part, and for second prepend/append ge_part to
    * list of parts. (Operation prepend use when restack above, and
    * append if restack below)
    */
   sd->parts = eina_list_remove(sd->parts, ge_part);

   if (mode)
     sd->parts = eina_list_prepend_relative(sd->parts, ge_part, ge_part_to);
   else
     sd->parts = eina_list_append_relative(sd->parts, ge_part, ge_part_to);

   if (sd->separated)
     {
        _select_item_move_to_top(sd);
        return true;
     }
   _part_parts_layouts_update(sd, ge_part, ge_part_to, mode);

   return true;
}

Eina_Bool
_edit_object_part_restack_above(Ws_Groupedit_Smart_Data *sd,
                                const char *part,
                                const char *part_above)
{
   Eina_Bool ret = false;
   if ((!sd->parts) || (!part)) return false;
   if (part_above)
     ret = edje_edit_part_restack_part_below(sd->edit_obj, part, part_above);
   else
     ret = edje_edit_part_restack_below(sd->edit_obj, part);

   if (!ret)
     {
        ERR("Failed restack in edje_edit object");
        return false;
     }
   _part_restack(sd, part, part_above, true);
   evas_object_smart_changed(sd->obj);
   return true;
}

Eina_Bool
_edit_object_part_restack_below(Ws_Groupedit_Smart_Data *sd,
                                const char *part,
                                const char *part_below)
{
   Eina_Bool ret = false;
   if ((!sd->parts) || (!part)) return false;
   if (part_below)
     ret = edje_edit_part_restack_part_above(sd->edit_obj, part, part_below);
   else
     ret = edje_edit_part_restack_above(sd->edit_obj, part);

   if (!ret)
     {
        ERR("Failed restack in edje_edit object");
        return false;
     }
   _part_restack(sd, part, part_below, false);
   evas_object_smart_changed(sd->obj);
   return true;
}

void
_parts_list_new(Ws_Groupedit_Smart_Data *sd)
{
   Groupedit_Part *gp;
   Eina_List *parts_list, *l;
   const char *name;
   Edje_Part_Type type;

   parts_list = edje_edit_parts_list_get(sd->edit_obj);

   evas_event_freeze(sd->e);
   EINA_LIST_FOREACH(parts_list, l, name)
     {
        type = edje_edit_part_type_get(sd->edit_obj, name);
        gp = _part_draw_add(sd, name, type);
        sd->parts = eina_list_append(sd->parts, gp);
     }
   evas_event_thaw(sd->e);
   edje_edit_string_list_free(parts_list);
   _move_border_to_top(sd);
}

void
_parts_list_free(Ws_Groupedit_Smart_Data *sd)
{
   Groupedit_Part *gp;
   if (!sd->parts) return;

   EINA_LIST_FREE(sd->parts, gp)
     _groupedit_part_free(gp);
   sd->parts = NULL;
}

static void
_groupedit_part_free(Groupedit_Part *gp)
{
   evas_object_del(gp->draw);
   eina_stringshare_del(gp->name);
   evas_object_smart_member_del(gp->border);
   evas_object_del(gp->border);
   if (gp->item)
     {
        evas_object_smart_member_del(gp->item);
        evas_object_del(gp->item);
     }

   //free(gp);
}

Groupedit_Part *
_parts_list_find(Eina_List *parts, const char *part)
{
   Eina_List *l;
   Groupedit_Part *gp;

   if ((!parts) || (!part)) return NULL;
   EINA_LIST_FOREACH(parts, l, gp)
     {
        if ((gp->name == part) || (!strcmp(part, gp->name)))
          return gp;
     }
   return NULL;
}

void
_selected_item_return_to_place(Ws_Groupedit_Smart_Data *sd)
{
   Groupedit_Part *gp;
   Eina_List *l, *ln;
   Eina_Bool is_below = false;

   if (!sd->selected) return;
   l = eina_list_data_find_list(sd->parts, sd->selected);

   ln = eina_list_prev(l);
   if (!ln)
     {
        ln = eina_list_next(l);
        if (!ln) return;
        is_below = true;
     }

   gp = (Groupedit_Part *)eina_list_data_get(ln);
   _part_parts_layouts_update(sd, sd->selected, gp, is_below);
   DBG("Separete mode, return to place part %s. Restack %s the part %s",
       (is_below) ? "belove" : "above", sd->selected->name, gp->name);

   evas_object_hide(sd->clipper);
   evas_object_clip_unset(sd->bg);
   evas_object_hide(sd->bg);
   edje_object_signal_emit(sd->selected->item, "item,unselected", "eflete");
}

void
_select_item_move_to_top(Ws_Groupedit_Smart_Data *sd)
{
   int x, y, w, h;
   if (sd->selected == sd->to_select) return;
   if (sd->selected) _selected_item_return_to_place(sd);

   evas_object_geometry_get(sd->to_select->item, &x, &y, &w, &h);
   evas_object_resize(sd->clipper, w, h);
   evas_object_move(sd->clipper, x, y);

   DBG("clipper geometry is %i %i %i %i", x, y, w, h);

   evas_object_show(sd->bg);
   evas_object_show(sd->clipper);
   evas_object_clip_set(sd->bg, sd->clipper);

   evas_object_raise(sd->bg);
   evas_object_raise(sd->clipper);
   evas_object_raise(sd->to_select->draw);
   evas_object_raise(sd->to_select->border);
   evas_object_raise(sd->to_select->item);
   sd->selected = sd->to_select;
   edje_object_signal_emit(sd->selected->item, "item,selected", "eflete");
   evas_object_geometry_get(sd->bg, &x, &y, &w, &h);
   DBG("bg geometry is %i %i %i %i", x, y, w, h);

   sd->to_select = NULL;
}

static void
_part_separete_mod_mouse_click_cb(void *data,
                                  Evas *e __UNUSED__,
                                  Evas_Object *obj __UNUSED__,
                                  void *event_info)
{
   Groupedit_Part *gp = (Groupedit_Part *)data;
   Ws_Groupedit_Smart_Data *sd = evas_object_data_get(gp->border, "sd");
   Evas_Event_Mouse_Down *emd = (Evas_Event_Mouse_Down *)event_info;

   if (emd->button != 1) return;
   sd->to_select = gp;
   _select_item_move_to_top(sd);
   evas_object_smart_callback_call(sd->obj, SIG_PART_SELECTED,
                                   (void *)sd->selected->name);
}

static void
_part_separete_mod_mouse_in_cb(void *data,
                               Evas *e __UNUSED__,
                               Evas_Object *obj __UNUSED__,
                               void *event_info __UNUSED__)
{
   Groupedit_Part *gp = (Groupedit_Part *)data;
   Ws_Groupedit_Smart_Data *sd = evas_object_data_get(gp->border, "sd");
   if (sd->selected == gp) return;
   edje_object_signal_emit(gp->item, "item,mouse,in", "eflete");
}

static void
_part_separete_mod_mouse_out_cb(void *data,
                                Evas *e __UNUSED__,
                                Evas_Object *obj __UNUSED__,
                                void *event_info __UNUSED__)
{
   Groupedit_Part *gp = (Groupedit_Part *)data;
   Ws_Groupedit_Smart_Data *sd = evas_object_data_get(gp->border, "sd");
   if (sd->selected == gp) return;
   edje_object_signal_emit(gp->item, "item,mouse,out", "eflete");
}

static void
_part_select(void *data,
             Evas *e __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void *event_info)
{
   Groupedit_Part *gp = (Groupedit_Part *)data;
   Ws_Groupedit_Smart_Data *sd = evas_object_data_get(gp->border, "sd");
   Evas_Event_Mouse_Down *emd = (Evas_Event_Mouse_Down *)event_info;

   if (emd->button != 1) return;
   sd->to_select = gp;
   evas_object_smart_callback_call(sd->obj, SIG_PART_SELECTED,
                                  (void *)gp->name);
}

#define GP_GEOMETRY_SET \
   evas_object_resize(gp->border, w * sd->zoom_factor, h * sd->zoom_factor); \
   evas_object_move(gp->border, x * sd->zoom_factor + xe + offset_x, \
                                y * sd->zoom_factor + ye + offset_y); \
   evas_object_resize(gp->item, sd->con_current_size->w, sd->con_current_size->h); \
   evas_object_move(gp->item, xe + offset_x, ye + offset_y);

#define GP_REAL_GEOMETRY_CALC \
   w *= sd->zoom_factor; h *= sd->zoom_factor; \
   Evas_Coord part_x = x * sd->zoom_factor + xe + offset_x; \
   Evas_Coord part_y = y * sd->zoom_factor + ye + offset_y; \
   Evas_Coord abs_x = abs(sd->real_size->x - part_x); \
   Evas_Coord abs_y = abs(sd->real_size->y - part_y); \
   if (part_x > sd->real_size->x) \
     w += abs_x; \
   else \
     { \
        sd->real_size->w += abs_x; \
        sd->real_size->x = part_x; \
     } \
   if (part_y > sd->real_size->y) \
     h += abs_y; \
   else \
     { \
        sd->real_size->h += abs_y; \
        sd->real_size->y = part_y; \
     } \
   if (sd->real_size->w < w) \
     sd->real_size->w = w; \
   if (sd->real_size->h < h) \
     sd->real_size->h = h;

#define ZOOM_APPLY(OBJECT) \
   if (fabs(sd->zoom_factor - 1.0) > 0.00001) \
     { \
        evas_object_geometry_get(OBJECT, &x, &y, NULL, NULL); \
        Evas_Map *m = evas_map_new(4); \
        evas_map_smooth_set(m, false); \
        evas_map_util_points_populate_from_object_full(m, OBJECT, 0); \
        evas_map_util_zoom(m, sd->zoom_factor, sd->zoom_factor, x, y); \
        evas_object_map_set(OBJECT, m); \
        evas_object_map_enable_set(OBJECT, true); \
        evas_map_free(m); \
        evas_object_repeat_events_set(OBJECT, true); \
     } \
   else \
     { \
        evas_object_map_enable_set(OBJECT, false); \
        evas_object_repeat_events_set(OBJECT, false); \
     }

static void
_part_text_recalc_apply(Ws_Groupedit_Smart_Data *sd,
                        Groupedit_Part *gp,
                        int offset_x,
                        int offset_y)
{
   Evas_Coord x, y, xe, ye, ro_x, ro_y;
   int w, h, ro_w, ro_h;
   const Evas_Object *ro;

   edje_object_part_geometry_get(sd->edit_obj, gp->name, &x, &y, &w, &h);
   evas_object_geometry_get(sd->edit_obj, &xe, &ye, NULL, NULL);
   ro = edje_object_part_object_get(sd->edit_obj, gp->name);
   evas_object_geometry_get(ro, &ro_x, &ro_y, &ro_w, &ro_h);

   /* all of it because of text and textblock >:C */
   ro_x = xe + offset_x + (ro_x - xe - offset_x) * sd->zoom_factor;
   ro_y = ye + offset_y + (ro_y - ye - offset_y) * sd->zoom_factor;

   evas_object_resize(gp->draw, ro_w, ro_h);
   evas_object_move(gp->draw, ro_x + offset_x, ro_y + offset_y);

   GP_GEOMETRY_SET

   GP_REAL_GEOMETRY_CALC

   ZOOM_APPLY(gp->draw)
}

static void
_part_recalc_apply(Ws_Groupedit_Smart_Data *sd,
                   Groupedit_Part *gp,
                   int offset_x,
                   int offset_y)
{
   Evas_Coord x, y, xe, ye;
   int w, h;

   edje_object_part_geometry_get(sd->edit_obj, gp->name, &x, &y, &w, &h);
   evas_object_geometry_get(sd->edit_obj, &xe, &ye, NULL, NULL);

   evas_object_resize(gp->draw, w, h);
   evas_object_move(gp->draw, (x * sd->zoom_factor + xe + offset_x),
                              (y * sd->zoom_factor + ye + offset_y));

   GP_GEOMETRY_SET

   GP_REAL_GEOMETRY_CALC

   ZOOM_APPLY(gp->draw)
}

#undef GP_GEOMETRY_SET
#undef GP_REAL_GEOMETRY_CALC
#undef ZOOM_APPLY

Eina_Bool
_parts_recalc(Ws_Groupedit_Smart_Data *sd)
{
   Eina_List *l;
   Groupedit_Part *gp;
   Edje_Part_Type ept;
   int i = 0, offset_x, offset_y;

   if (!sd->parts) return false;

   evas_object_geometry_get(sd->obj, &sd->real_size->x,
                                     &sd->real_size->y,
                                     &sd->real_size->w,
                                     &sd->real_size->h);

   EINA_LIST_FOREACH(sd->parts, l, gp)
     {
        offset_x = i * SEP_ITEM_PAD_X;
        offset_y = i * SEP_ITEM_PAD_Y;

        if (sd->separated)
          {
             i++;
             if (!gp->item)
               {
                  gp->item = edje_object_add(sd->e);
                  if (!edje_object_file_set(gp->item, EFLETE_EDJ, "eflete/group/item/default"))
                    ERR("Could not load style of part's item!");
                  evas_object_event_callback_add(gp->item, EVAS_CALLBACK_MOUSE_DOWN,
                                                 _part_separete_mod_mouse_click_cb, gp);
                  evas_object_event_callback_add(gp->item, EVAS_CALLBACK_MOUSE_IN,
                                                 _part_separete_mod_mouse_in_cb, gp);
                  evas_object_event_callback_add(gp->item, EVAS_CALLBACK_MOUSE_OUT,
                                                 _part_separete_mod_mouse_out_cb, gp);
                  evas_object_smart_member_add(gp->item, sd->obj);
                  evas_object_show(gp->item);
               }
          }
        else
          {
             if (gp->item)
               {
                  evas_object_hide(gp->item);
                  evas_object_event_callback_del_full(gp->item, EVAS_CALLBACK_MOUSE_DOWN,
                                                      _part_separete_mod_mouse_click_cb, gp);
                  evas_object_event_callback_del_full(gp->item, EVAS_CALLBACK_MOUSE_IN,
                                                      _part_separete_mod_mouse_in_cb, gp);
                  evas_object_event_callback_del_full(gp->item, EVAS_CALLBACK_MOUSE_OUT,
                                                      _part_separete_mod_mouse_out_cb, gp);

                  evas_object_smart_member_del(gp->item);
                  evas_object_del(gp->item);
                  gp->item = NULL;
               }
          }

        ept = edje_edit_part_type_get(sd->edit_obj, gp->name);
        switch (ept)
          {
           case EDJE_PART_TYPE_RECTANGLE:
              _rectangle_param_update(gp, sd->edit_obj);
              _part_recalc_apply(sd, gp, offset_x, offset_y);
              break;

           case EDJE_PART_TYPE_TEXT:
              _text_param_update(gp, sd->edit_obj);
              _part_text_recalc_apply(sd, gp, offset_x, offset_y);
              break;

           case EDJE_PART_TYPE_IMAGE:
           case EDJE_PART_TYPE_PROXY: // it part like image
              _image_param_update(gp, sd->edit_obj, sd->edit_obj_file);
              _part_recalc_apply(sd, gp, offset_x, offset_y);
              break;

           case EDJE_PART_TYPE_TEXTBLOCK:
              _textblock_param_update(gp, sd->edit_obj);
              _part_recalc_apply(sd, gp, offset_x, offset_y);
              break;

           case EDJE_PART_TYPE_SPACER:
           case EDJE_PART_TYPE_SWALLOW:
           case EDJE_PART_TYPE_GROUP:
           case EDJE_PART_TYPE_BOX:
           case EDJE_PART_TYPE_TABLE:
           case EDJE_PART_TYPE_EXTERNAL:
              _part_recalc_apply(sd, gp, offset_x, offset_y);
           default:
              break;
          }
        if (gp->visible)
          {
             evas_object_show(gp->draw);
             evas_object_show(gp->border);
          }
        else
          {
             evas_object_hide(gp->draw);
             evas_object_hide(gp->border);
          }
     }
     if (!sd->separated) _part_object_area_calc(sd);
     else
       {
          if (sd->to_select) _select_item_move_to_top(sd);
          evas_object_hide(sd->obj_area.obj);
          evas_object_smart_callback_call(sd->obj, SIG_OBJ_AREA_CHANGED, sd->obj_area.geom);
       }

   evas_object_smart_callback_call(sd->obj, SIG_GEOMETRY_CHANGED, (void *)sd->real_size);

   return true;
}
#define BORDER_ADD(R, G, B, A) \
   GET_IMAGE(gp->border, sd->e, BORDER_IMG); \
   evas_object_color_set(gp->border, R*A/255, G*A/255, B*A/255, A);

#define IMAGE_PART_GROUP "eflete/groupedit/image/default"

static void
_image_delete(void *data __UNUSED__,
              Evas *e __UNUSED__,
              Evas_Object *obj,
              void *event_info __UNUSED__)
{
   Evas_Object *image = edje_object_part_swallow_get(obj, "swallow.image");
   if (image) evas_object_del(image);
}

static Groupedit_Part *
_part_draw_add(Ws_Groupedit_Smart_Data *sd, const char *part, Edje_Part_Type type)
{
   Groupedit_Part *gp;

   gp = mem_malloc(sizeof(Groupedit_Part));
   gp->name = eina_stringshare_add(part);
   gp->visible = true;
   gp->border = NULL;
   gp->item = NULL;

   switch (type)
     {
      case EDJE_PART_TYPE_RECTANGLE:
         gp->draw = evas_object_rectangle_add(sd->e);
         BORDER_ADD(0, 0, 0, 0)
         break;
      case EDJE_PART_TYPE_TEXT:
         gp->draw = evas_object_text_add(sd->e);
         BORDER_ADD(122, 122, 122, 255)
         break;
      case EDJE_PART_TYPE_IMAGE:
      case EDJE_PART_TYPE_PROXY: // it part like image
         gp->draw = edje_object_add(sd->e);
         if (!edje_object_file_set(gp->draw, EFLETE_EDJ, IMAGE_PART_GROUP))
           ERR("Image can't be loaded.\n");
         evas_object_event_callback_add(gp->draw, EVAS_CALLBACK_DEL,
                                        _image_delete, NULL);
         Evas_Object *o = evas_object_image_add(sd->e);
         edje_object_part_swallow(gp->draw, "swallow.image", o);
         BORDER_ADD(0, 0, 0, 0)
         break;
      case EDJE_PART_TYPE_SWALLOW:
         gp->draw = _part_swallow_add(sd->e);
         BORDER_ADD(150, 143, 158, 255)
         break;
      case EDJE_PART_TYPE_TEXTBLOCK:
         gp->draw = evas_object_textblock_add(sd->e);
         BORDER_ADD(122, 122, 122, 255)
         break;
      case EDJE_PART_TYPE_SPACER:
         gp->draw = _part_spacer_add(sd->e);
         BORDER_ADD(101, 117, 133, 255)
         break;
      case EDJE_PART_TYPE_GROUP:
      case EDJE_PART_TYPE_BOX:
      case EDJE_PART_TYPE_TABLE:
      case EDJE_PART_TYPE_EXTERNAL:
      default:
         /* Temporary solution for type parts, which not implemented yet.
          * Here created transparent rectangle as draw evas primitives.
          * TODO: add support for all part types.
          */
         gp->draw = evas_object_rectangle_add(sd->e);
         evas_object_color_set(gp->draw, 0, 0, 0, 0);
         break;
     }

   evas_object_data_set(gp->border, "sd", sd);
   evas_object_event_callback_add(gp->border, EVAS_CALLBACK_MOUSE_DOWN,
                                  _part_select, gp);

   evas_object_smart_member_add(gp->draw, sd->obj);
   evas_object_smart_member_add(gp->border, sd->obj);

   return gp;
}

#undef BORDER_ADD
#undef IMAGE_PART_GROUP

static void
_part_draw_del(Ws_Groupedit_Smart_Data *sd, const char *part)
{
   Groupedit_Part *gp;

   gp = _parts_list_find(sd->parts, part);
   if (!gp) return;
   _groupedit_part_free(gp);
   sd->parts = eina_list_remove(sd->parts, gp);
}

static void
_move_border_to_top(Ws_Groupedit_Smart_Data *sd)
{
   evas_object_smart_member_del(sd->container);
   evas_object_smart_member_del(sd->handler_TL.obj);
   evas_object_smart_member_del(sd->handler_BR.obj);

   evas_object_smart_member_add(sd->container, sd->obj);
   evas_object_smart_member_add(sd->handler_TL.obj, sd->obj);
   evas_object_smart_member_add(sd->handler_BR.obj, sd->obj);
}

static Evas_Object *
_part_spacer_add(Evas *e)
{
   Evas_Object *spacer;

   GET_IMAGE(spacer, e, SPACER_IMG);
   evas_object_smart_calculate(spacer);

   return spacer;
}

static Evas_Object *
_part_swallow_add(Evas *e)
{
   Evas_Object *swallow;

   GET_IMAGE(swallow, e, SWALLOW_IMG);
   evas_object_smart_calculate(swallow);

   return swallow;
}

static void
_rectangle_param_update(Groupedit_Part *gp, Evas_Object *edit_obj)
{
   int r, g, b, a;
   PART_STATE_GET(edit_obj, gp->name)

   edje_edit_state_color_get(edit_obj, gp->name, state, value, &r, &g, &b, &a);
   evas_object_color_set(gp->draw, r*a/255, g*a/255, b*a/255, a);

   PART_STATE_FREE
}

static void
_image_param_update(Groupedit_Part *gp, Evas_Object *edit_obj, const char *file)
{
   Evas_Load_Error err;
   Evas_Object *image, *source_image = NULL;
   const char *image_normal, *proxy_source;
   const char *buf = NULL;
   int r, g, b, a;
   int id;
   int bl, br, bt, bb;
   int x, y, w, h;
   int img_w, img_h;
   double fill_w, fill_h, fill_x, fill_y;
   int fill_origin_offset_x, fill_origin_offset_y, fill_size_offset_x, fill_size_offset_y;
   unsigned char middle;
   Ws_Groupedit_Smart_Data *sd;
   Groupedit_Part *source;

   PART_STATE_GET(edit_obj, gp->name)

   edje_edit_state_color_get(edit_obj, gp->name, state, value, &r, &g, &b, &a);

   image = edje_object_part_swallow_get(gp->draw, "swallow.image");
   evas_object_color_set(image, r*a/255, g*a/255, b*a/255, a);

   proxy_source = edje_edit_state_proxy_source_get(edit_obj, gp->name, state, value);
   if (proxy_source)
     {
        sd = evas_object_data_get(gp->border, "sd");
        source = _parts_list_find(sd->parts, proxy_source);

        source_image = edje_object_part_swallow_get(source->draw, "swallow.image");
        if (!source_image)
          source_image = source->draw;
        evas_object_image_source_set(image, source_image);
        evas_object_image_source_clip_set(image, false);
     }
   else
     {
        image_normal = edje_edit_state_image_get(edit_obj, gp->name, state, value);
        if (!image_normal) return;
        id = edje_edit_image_id_get(edit_obj, image_normal);
        buf = eina_stringshare_printf("edje/images/%i", id);
        evas_object_image_file_set(image, file, buf);
        err = evas_object_image_load_error_get(image);
        if (err != EVAS_LOAD_ERROR_NONE)
          WARN("Could not update image:\"%s\"\n",  evas_load_error_str(err));
        edje_edit_string_free(image_normal);

        edje_edit_state_image_border_get(edit_obj, gp->name, state, value,
                                         &bl, &br, &bt, &bb);
        evas_object_image_border_set(image, bl, br, bt, bb);

        middle  = edje_edit_state_image_border_fill_get(edit_obj, gp->name, state, value);
        if (middle == 0)
          evas_object_image_border_center_fill_set(image, EVAS_BORDER_FILL_NONE);
        else if (middle == 1)
          evas_object_image_border_center_fill_set(image, EVAS_BORDER_FILL_DEFAULT);
        else if (middle == 2)
          evas_object_image_border_center_fill_set(image, EVAS_BORDER_FILL_SOLID);
     }
   /* setups settings from filled block  into evas image object*/
   evas_object_image_smooth_scale_set(image,
            edje_edit_state_fill_smooth_get(edit_obj, gp->name, state, value));

   /* take fill params here, because need to know is default params set*/
   fill_x = edje_edit_state_fill_origin_relative_x_get(edit_obj, gp->name, state, value);
   fill_y = edje_edit_state_fill_origin_relative_y_get(edit_obj, gp->name, state, value);
   fill_w = edje_edit_state_fill_size_relative_x_get(edit_obj, gp->name, state, value);
   fill_h = edje_edit_state_fill_size_relative_y_get(edit_obj, gp->name, state, value);
   fill_origin_offset_x = edje_edit_state_fill_origin_offset_x_get(edit_obj, gp->name, state, value);
   fill_origin_offset_y = edje_edit_state_fill_origin_offset_y_get(edit_obj, gp->name, state, value);
   fill_size_offset_x = edje_edit_state_fill_size_offset_x_get(edit_obj, gp->name, state, value);
   fill_size_offset_y = edje_edit_state_fill_size_offset_y_get(edit_obj, gp->name, state, value);
   if (edje_edit_state_fill_type_get(edit_obj, gp->name, state, value))
     {
        /* If image tiled, set fill param to evas image object */
        evas_object_image_size_get(image, &img_w, &img_h);
        x = (int)(img_w * fill_x) + fill_origin_offset_x;
        y = (int)(img_h * fill_y) + fill_origin_offset_y;
        w = (int)(img_w * fill_w) + fill_size_offset_x;
        h = (int)(img_h * fill_h) + fill_size_offset_y;
        evas_object_image_filled_set(image, false);
        evas_object_image_fill_set(image, x, y, w, h);
     }
   else if (fill_x || fill_y || (fill_w != 1) || (fill_h != 1) ||
            fill_origin_offset_x || fill_origin_offset_y ||
            fill_size_offset_x || fill_size_offset_y)
         {
           /* If image fill is scale and params are non default values, set
              this params to evas image object */
            evas_object_geometry_get(image, NULL, NULL, &img_w, &img_h);
            x = (int)(img_w * fill_x) + fill_origin_offset_x;
            y = (int)(img_h * fill_y) + fill_origin_offset_y;
            w = (int)(img_w * fill_w) + fill_size_offset_x;
            h = (int)(img_h * fill_h) + fill_size_offset_y;
            evas_object_image_filled_set(image, false);
            evas_object_image_fill_set(image, x, y, w, h);
         }
       else
         /* If image fill is scale with default params. */
         evas_object_image_filled_set(image, true);

   PART_STATE_FREE
   eina_stringshare_del(buf);
}

static void
_text_param_update(Groupedit_Part *gp, Evas_Object *edit_obj)
{
   const char *font, *text;
   int text_size;
   int r, g, b, a; // main color
   int sr, sg, sb, sa; // shadow
   int or, og, ob, oa; // outline
   double elipsis;
   Evas_Text_Style_Type style;
   Edje_Text_Effect effect;
   PART_STATE_GET(edit_obj, gp->name)

   font = edje_edit_state_font_get(edit_obj, gp->name, state, value);
   text_size = edje_edit_state_text_size_get(edit_obj, gp->name, state, value);
   evas_object_text_font_set(gp->draw, font, text_size);

   text = edje_edit_state_text_get(edit_obj, gp->name, state, value);
   if (text) evas_object_text_text_set(gp->draw, text);
   else
     {
         edje_object_part_text_set(edit_obj, gp->name, gp->name);
         evas_object_text_text_set(gp->draw, gp->name);
     }

   elipsis = edje_edit_state_text_elipsis_get(edit_obj, gp->name, state, value);
   evas_object_text_ellipsis_set(gp->draw, elipsis);

   edje_edit_state_color_get(edit_obj, gp->name, state, value, &r, &g, &b, &a);
   evas_object_color_set(gp->draw, r*a/255, g*a/255, b*a/255, a);

   edje_edit_state_color2_get(edit_obj, gp->name, state, value, &sr, &sg, &sb, &sa);
   evas_object_text_shadow_color_set(gp->draw, sr, sg, sb, sa);

   edje_edit_state_color3_get(edit_obj, gp->name, state, value, &or, &og, &ob, &oa);
   evas_object_text_outline_color_set(gp->draw, or, og, ob, oa);

   effect = edje_edit_part_effect_get(edit_obj, gp->name);
   switch (effect & EDJE_TEXT_EFFECT_MASK_BASIC)
     {
      case EDJE_TEXT_EFFECT_NONE:
      case EDJE_TEXT_EFFECT_PLAIN:
         style = EVAS_TEXT_STYLE_PLAIN;
         break;
      case EDJE_TEXT_EFFECT_OUTLINE:
         style = EVAS_TEXT_STYLE_OUTLINE;
         break;
      case EDJE_TEXT_EFFECT_SOFT_OUTLINE:
         style = EVAS_TEXT_STYLE_SOFT_OUTLINE;
         break;
      case EDJE_TEXT_EFFECT_SHADOW:
         style = EVAS_TEXT_STYLE_SHADOW;
         break;
      case EDJE_TEXT_EFFECT_SOFT_SHADOW:
         style = EVAS_TEXT_STYLE_SOFT_SHADOW;
         break;
      case EDJE_TEXT_EFFECT_OUTLINE_SHADOW:
         style = EVAS_TEXT_STYLE_OUTLINE_SHADOW;
         break;
      case EDJE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW:
         style = EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW;
         break;
      case EDJE_TEXT_EFFECT_FAR_SHADOW:
         style = EVAS_TEXT_STYLE_FAR_SHADOW;
         break;
      case EDJE_TEXT_EFFECT_FAR_SOFT_SHADOW:
         style = EVAS_TEXT_STYLE_FAR_SOFT_SHADOW;
         break;
      case EDJE_TEXT_EFFECT_GLOW:
         style = EVAS_TEXT_STYLE_GLOW;
         evas_object_text_glow_color_set(gp->draw, sr, sg, sb, sa);
         evas_object_text_glow2_color_set(gp->draw, or, og, ob, oa);
         break;
      default:
         style = EVAS_TEXT_STYLE_PLAIN;
         break;
     }

   switch (effect & EDJE_TEXT_EFFECT_MASK_SHADOW_DIRECTION)
     {
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_BOTTOM_RIGHT:
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_BOTTOM_RIGHT);
         break;
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_BOTTOM:
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_BOTTOM);
         break;
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_BOTTOM_LEFT:
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_BOTTOM_LEFT);
         break;
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_LEFT:
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_LEFT);
         break;
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_TOP_LEFT:
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_TOP_LEFT);
         break;
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_TOP:
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_TOP);
         break;
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_TOP_RIGHT:
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_TOP_RIGHT);
         break;
      case EDJE_TEXT_EFFECT_SHADOW_DIRECTION_RIGHT:
         EVAS_TEXT_STYLE_SHADOW_DIRECTION_SET
            (style, EVAS_TEXT_STYLE_SHADOW_DIRECTION_RIGHT);
         break;
      default:
         break;
     }
   evas_object_text_style_set(gp->draw, style);
   PART_STATE_FREE
   edje_edit_string_free(font);
   edje_edit_string_free(text);
}

static void
_textblock_param_update(Groupedit_Part *gp, Evas_Object *edit_obj)
{
   Evas_Textblock_Style *ts = NULL;
   const Evas_Textblock_Style *obj_style;
   const Evas_Object *get_style;
   const char *text;
   double valign;
   PART_STATE_GET(edit_obj, gp->name)

   get_style = edje_object_part_object_get(edit_obj, gp->name);
   obj_style = evas_object_textblock_style_get(get_style);
   text = evas_textblock_style_get(obj_style);

   ts = evas_textblock_style_new();
   evas_textblock_style_set(ts, text);
   evas_object_textblock_style_set(gp->draw, ts);
   valign = edje_edit_state_text_align_y_get(edit_obj, gp->name, state, value);
   evas_object_textblock_valign_set(gp->draw, valign);

   text = edje_edit_state_text_get(edit_obj, gp->name, state, value);
   if (text) evas_object_textblock_text_markup_set(gp->draw, text);
   else
     {
        edje_object_part_text_set(edit_obj, gp->name, gp->name);
        evas_object_textblock_text_markup_set(gp->draw, gp->name);
     }

   PART_STATE_FREE
   edje_edit_string_free(text);
}

static void
_part_object_area_calc(Ws_Groupedit_Smart_Data *sd)
{
   Eina_Stringshare *rel_to;
   int xc, yc, wc, hc;
   int xg, yg, wg, hg; // groupedit geometry
   int x = 0, w = 0, y = 0, h = 0;
   double relative;

   Groupedit_Part *rel_part = NULL;

   if (!sd->selected)
     {
        x = sd->con_current_size->x; y = sd->con_current_size->y;
        w = sd->con_current_size->w; h = sd->con_current_size->h;
     }
   else
     {
        PART_STATE_GET(sd->edit_obj, sd->selected->name)
        const char *name = sd->selected->name;

        evas_object_geometry_get(sd->edit_obj, &xg, &yg, &wg, &hg);

        xc = xg;
        wc = wg;
        rel_to = edje_edit_state_rel1_to_x_get(sd->edit_obj, name, state, value);
        relative = edje_edit_state_rel1_relative_x_get(sd->edit_obj, name, state, value);
        if (rel_to)
          {
             rel_part = _parts_list_find(sd->parts, rel_to);
             evas_object_geometry_get(rel_part->draw, &xc, NULL, &wc, NULL);
          }
        x = (xc + (int)(wc * relative * sd->zoom_factor));
        edje_edit_string_free(rel_to);

        yc = yg;
        hc = hg;
        rel_to = edje_edit_state_rel1_to_y_get(sd->edit_obj, name, state, value);
        relative = edje_edit_state_rel1_relative_y_get(sd->edit_obj, name, state, value);
        if (rel_to)
          {
             rel_part = _parts_list_find(sd->parts, rel_to);
             evas_object_geometry_get(rel_part->draw, NULL, &yc, NULL, &hc);
          }
        y = (yc + (int)(hc * relative * sd->zoom_factor));
        edje_edit_string_free(rel_to);

        xc = xg; wc = sd->con_current_size->w;
        rel_to = edje_edit_state_rel2_to_x_get(sd->edit_obj, name, state, value);
        relative = edje_edit_state_rel2_relative_x_get(sd->edit_obj, name, state, value);
        if (rel_to)
          {
             rel_part = _parts_list_find(sd->parts, rel_to);
             evas_object_geometry_get(rel_part->draw, &xc, NULL, &wc, NULL);
          }
        w = ((xc - x) + (int)(wc * relative * sd->zoom_factor));
        edje_edit_string_free(rel_to);

        yc = yg; hc = sd->con_current_size->h;
        rel_to = edje_edit_state_rel2_to_y_get(sd->edit_obj, name, state, value);
        relative = edje_edit_state_rel2_relative_y_get(sd->edit_obj, name, state, value);
        if (rel_to)
          {
             rel_part = _parts_list_find(sd->parts, rel_to);
             evas_object_geometry_get(rel_part->draw, NULL, &yc, NULL, &hc);
          }
        h = ((yc - y) + (int)(hc * relative * sd->zoom_factor));
        edje_edit_string_free(rel_to);

        evas_object_geometry_get(sd->obj, &xc, &yc, NULL, NULL);

        evas_object_resize(sd->obj_area.obj, w, h);
        evas_object_move(sd->obj_area.obj, x, y);
        evas_object_stack_below(sd->obj_area.obj, sd->selected->draw);

        PART_STATE_FREE
     }

   if (sd->obj_area.visible) evas_object_show(sd->obj_area.obj);
   else evas_object_hide(sd->obj_area.obj);

   sd->obj_area.geom->x = x; sd->obj_area.geom->y = y;
   sd->obj_area.geom->w = w; sd->obj_area.geom->h = h;

   if ((sd->selected) && ((sd->obj_area.visible)))
     evas_object_show(sd->obj_area.obj);
   else
     evas_object_hide(sd->obj_area.obj);

   evas_object_smart_callback_call(sd->obj, SIG_OBJ_AREA_CHANGED, sd->obj_area.geom);
}
