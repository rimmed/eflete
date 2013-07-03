#include "ui_ruler.h"
#include "efl_tet.h"
/*
struct _Scale
{
   Eina_List	*marks;
   Eina_List	*dashes;
   int		dash_counter;
   double	step;
//   Eina_Bool	visible;
   int		start;
   int		end;
};
typedef struct _Scale Scale;
*/
struct _UI_Ruler_Data
{
   Orient	orient;
   Eina_Bool	visible;

   Eina_Bool abs_scale_visible;
   Eina_Bool rel_scale_visible;
   Eina_Bool abs_scale_step;
   Eina_Bool rel_scale_step;


   //Evas_Object	*pointer;
   double	pointer_pos;
   Eina_Bool	pointer_visible;
};
typedef struct _UI_Ruler_Data UI_Ruler_Data;
/*
static char *
_itoa(int n)
{
   int len = n == 0 ? 1 : floor(log10l(abs(n))) + 1;
   if (n < 0) len++;
   char *buf = mem_malloc((len + 1) * sizeof(char));
   snprintf(buf, len, "%d", n);
   return buf;
}
*/
/*static int
_absolute_scale_dashes_del(Evas_Object *obj, int count)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   Eina_List *l = eina_list_last(_ruler_data->abs_scale->dashes);
   Evas_Object *_line;
   int i = 0;

   for (_line = eina_list_data_get(l); i < count; i++)
     {
        _line = (Evas_Object*)eina_list_data_get(l);
        if (!_line) return 0;
        l = eina_list_remove(l, _line);
        l = eina_list_prev(l);
        evas_object_del(_line);
     }
   return i;
}

static void
_absolute_scale_marks_del(Evas_Object *obj, int count)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   Eina_List *l = eina_list_last(_ruler_data->abs_scale->marks);
   Evas_Object *_text;
   int i = 0;

   for (_text = eina_list_data_get(l); i < count; i++)
     {
        _text = (Evas_Object*)eina_list_data_get(l);
        if (!_text) return;
        l = eina_list_remove(l, _text);
        l = eina_list_prev(l);
        evas_object_del(_text);
     }
}

static void
_relative_scale_marks_del(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   if (!_ruler_data->rel_scale->marks) return;
   Eina_List *l = NULL;
   Evas_Object *_mark;

   EINA_LIST_FOREACH(_ruler_data->rel_scale->dashes, l, _mark)
      evas_object_del(_mark);

   _ruler_data->rel_scale->dashes = NULL;
   l = NULL;

   EINA_LIST_FOREACH(_ruler_data->rel_scale->marks, l, _mark)
     evas_object_del(_mark);

   _ruler_data->rel_scale->marks = NULL;
}

static void
_absolute_scale_clear(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);

   _absolute_scale_marks_del(obj,
      eina_list_count(_ruler_data->abs_scale->marks));
   _absolute_scale_dashes_del(obj,
      eina_list_count(_ruler_data->abs_scale->dashes));
   _ruler_data->abs_scale->dash_counter = 0;
   _ruler_data->abs_scale->marks = NULL;
   _ruler_data->abs_scale->dashes = NULL;
}

static int
_relative_scale_marks_add(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   Evas_Object *_text = NULL;
   Evas_Object *_line = NULL;
   double value = 0.0;
   double step = _ruler_data->rel_scale->step;
   int step_px = 0;
   char buf[4];
   int x, y, w, h, k;

   _relative_scale_marks_del(obj);
   if ((!_ruler_data->rel_scale->visible) || (!_ruler_data->visible))
     return 0;

   Evas *_canvas = evas_object_evas_get(obj);
   evas_object_geometry_get(obj, &x, &y, &w, &h);

   if ((_ruler_data->rel_scale->start != _ruler_data->rel_scale->end) &&
       (_ruler_data->rel_scale->start < _ruler_data->rel_scale->end))
     {
        if (_ruler_data->orient == HORIZONTAL)
          {
             x = _ruler_data->rel_scale->start;
             w = _ruler_data->rel_scale->end - _ruler_data->rel_scale->start;
             step_px = (int)(w * step);
          }
        else
          {
             y = _ruler_data->rel_scale->start;
             h = _ruler_data->rel_scale->end - _ruler_data->rel_scale->start;
             step_px = (int)(h * step);
          }
     }
   else
     if (_ruler_data->orient == HORIZONTAL)
       step_px = (int)(w * step);
     else
       step_px = (int)(h * step);

   for (k = 0; value <= 1 - step; value += step, k++)
     {
        sprintf(buf, "%2.1f", value);
        _text = evas_object_text_add(_canvas);
        evas_object_text_font_set(_text, "Calibri", 10);
        evas_object_color_set(_text, 53, 136, 32, 255);
        evas_object_text_text_set(_text, buf);

        if (_ruler_data->orient == HORIZONTAL)
          evas_object_move(_text, x + step_px * k - 7, y - 4);
        else
          evas_object_move(_text, x + w - 25, y + step_px * k - 8 );

        _ruler_data->rel_scale->marks=
           eina_list_append(_ruler_data->rel_scale->marks, _text);
        evas_object_show(_text);
        _line = evas_object_line_add(_canvas);

        if (_ruler_data->orient == HORIZONTAL)
          evas_object_line_xy_set(_line, x + k * step_px, y + h,
             x + k * step_px, y + h - 15);
        else
          evas_object_line_xy_set(_line, x + w, y + k * step_px,
             x + w - 5, y + k * step_px);

        evas_object_color_set(_line, 53, 136, 32, 255);
        evas_object_show(_line);
        _ruler_data->rel_scale->dashes =
           eina_list_append(_ruler_data->rel_scale->dashes, _line);

     }

   _text = evas_object_text_add(_canvas);
   evas_object_text_font_set(_text, "Calibri", 10);
   evas_object_color_set(_text, 53, 136, 32, 255);
   evas_object_text_text_set(_text, "1.0");

   if (_ruler_data->orient == HORIZONTAL)
     evas_object_move(_text, x + step_px * k - 15, y - 3);
   else
     evas_object_move(_text, x + w - 25, y + step_px * k - 15);

   _ruler_data->rel_scale->marks=
      eina_list_append(_ruler_data->rel_scale->marks, _text);
   evas_object_show(_text);
   _line = evas_object_line_add(_canvas);

   if (_ruler_data->orient == HORIZONTAL)
     evas_object_line_xy_set(_line, x + k * step_px, y + h, x + k * step_px,
        y + h - 15);
   else
     evas_object_line_xy_set(_line, x + w, y + k * step_px, x + w - 15,
        y + k *step_px);

   evas_object_color_set(_line, 53, 136, 32, 255);
   evas_object_show(_line);
   _ruler_data->rel_scale->dashes =
      eina_list_append(_ruler_data->rel_scale->dashes, _line);

   return k;
}

static int
_absolute_scale_marks_add(Evas_Object *obj, int count, int from)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   int i = _ruler_data->abs_scale->dash_counter;
   int x, y, w, h, k, z = 0;
   int _dash_size = 3;
   int _dash_from = from;
   int _pos_text_shift = 10;
   int _pos_diff = 0;
   Evas_Object *_line, *_text;
   Evas *_canvas = evas_object_evas_get(obj);
   evas_object_geometry_get(obj, &x, &y, &w, &h);
   if (_ruler_data->abs_scale->start != 0)
     {
       if (_ruler_data->orient == HORIZONTAL)
           z = (int)((_ruler_data->rel_scale->start - x) /
                  _ruler_data->abs_scale->step) * (-1);
       else
           z = (int)((_ruler_data->rel_scale->start - y) /
                  _ruler_data->abs_scale->step) * (-1);
     }
   else z = i;

   if ((z > 0) && (i == 0)) z = 0;
   if (z < 0)
     {
        if (_ruler_data->orient == HORIZONTAL)
          _pos_diff = z * (-1) * (int)_ruler_data->abs_scale->step -
             (_ruler_data->abs_scale->start - x);
        else
          _pos_diff = z * (-1) * (int)_ruler_data->abs_scale->step -
             (_ruler_data->abs_scale->start - y);
     }
   for (k = 0; k <= count; i++, k++, z++)
     {
        if (!(z % 5) && (z % 10) && i) _dash_size = 7;
        else
          if (!(z % 10) || !z)
            {
               _dash_size = 12;
               _text = evas_object_text_add(_canvas);
               evas_object_text_font_set(_text, "Calibri", 10);
               evas_object_color_set(_text, 64, 64, 64, 255);
               evas_object_text_text_set(_text, _itoa(z*100));
               if (_ruler_data->orient == HORIZONTAL)
                 {
                    if (z >= 100) _pos_text_shift = 26;
                    else _pos_text_shift = 20;

                    if (z < 0) _pos_text_shift = -2;
                    evas_object_move(_text, x + i *
                      (int)_ruler_data->abs_scale->step - _pos_text_shift
                      - _pos_diff, _dash_from - _dash_size - 5);
                    if (z == 0)
                      {
                         evas_object_text_text_set(_text, "0");
                         evas_object_color_set(_text, 255, 0, 0, 255);
                         evas_object_move(_text, x + i *
                           (int)_ruler_data->abs_scale->step - 7 - _pos_diff,
                           _dash_from - _dash_size - 5);
                      }
                 }
               else
                 {
                   evas_object_move(_text, _dash_from - _dash_size - 14,
                     y + i * (int)_ruler_data->abs_scale->step - 3 -
                     _pos_diff);
                   if (z == 0)
                      {
                         evas_object_move(_text, _dash_from - _dash_size,
                           y + i * (int)_ruler_data->abs_scale->step + 1 -
                           _pos_diff);
                         evas_object_color_set(_text, 255, 0, 0, 255);
                         evas_object_text_text_set(_text, "0");
                      }
                   if (z > 0)
                      evas_object_move(_text, _dash_from - _dash_size - 12,
                        y + i * (int)_ruler_data->abs_scale->step - 13 -
                        _pos_diff);
                 }
               _ruler_data->abs_scale->marks =
                  eina_list_append(_ruler_data->abs_scale->marks, _text);
               evas_object_show(_text);
            }
        _line = evas_object_line_add(_canvas);
        if (_ruler_data->orient == HORIZONTAL)
          evas_object_line_xy_set(_line,
            x + i * (int)_ruler_data->abs_scale->step - _pos_diff ,
            _dash_from,
            x + i * (int)_ruler_data->abs_scale->step - _pos_diff,
            _dash_from - _dash_size);
        else
          evas_object_line_xy_set(_line, _dash_from,
            y + i * (int)_ruler_data->abs_scale->step - _pos_diff,
            _dash_from - _dash_size,
            y + i * (int)_ruler_data->abs_scale->step - _pos_diff);

        if (z == 0)
          {
            evas_object_color_set(_line, 255, 0, 0, 255);
            evas_object_scale_set (_line, 2.0);
          }
        else evas_object_color_set(_line, 64, 64, 64, 255);
        evas_object_show(_line);
        _ruler_data->abs_scale->dashes =
           eina_list_append(_ruler_data->abs_scale->dashes, _line);
        _dash_size = 3;
     }
   return k;
}
*/
/*
static void
_scales_repaint(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   Eina_List *l =_ruler_data->abs_scale->dashes;
   Evas_Object *_line = (Evas_Object*)eina_list_data_get(l);

   int x, y, w, h;
   int lx1, ly1, lx2, ly2;
   int dash_counter;
   int dash_from;
   int temp_dash = 0;
   int marks_count;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   evas_object_line_xy_get(_line, &lx1, &ly1, &lx2, &ly2);

   if (_ruler_data->orient == HORIZONTAL)
     {
        dash_counter = (int)(w / _ruler_data->abs_scale->step);
        dash_from = y + h;
     }
   else
     {
        dash_counter = (int)(h / _ruler_data->abs_scale->step);
        dash_from = x + w;
     }
   if (dash_from < 0) return;

   _relative_scale_marks_add(obj);
   if ((!_ruler_data->abs_scale->visible) || (!_ruler_data->visible))
     {
        _absolute_scale_clear(obj);
        return;
     }


   temp_dash = _ruler_data->abs_scale->dash_counter - dash_counter;
   if (((_ruler_data->orient == HORIZONTAL) && (x != lx1)) ||
       ((_ruler_data->orient == VERTICAL) && (dash_from != lx1)))
     {
        _absolute_scale_clear(obj);
        temp_dash = -1;
     }

   if (temp_dash < 0)
     {
        dash_counter = dash_counter - _ruler_data->abs_scale->dash_counter;
        _ruler_data->abs_scale->dash_counter +=
           _absolute_scale_marks_add(obj, dash_counter, dash_from);
     }
   else
     {
        dash_counter =	_ruler_data->abs_scale->dash_counter - dash_counter;
        _ruler_data->abs_scale->dash_counter -=
           _absolute_scale_dashes_del(obj, dash_counter);
        marks_count = eina_list_count(_ruler_data->abs_scale->marks);
        _absolute_scale_marks_del(obj, (int)(marks_count -
           (_ruler_data->abs_scale->dash_counter / 10) - 1));
     }
}
*/
static UI_Ruler_Data *
_ruler_data_init(void)
{
   UI_Ruler_Data *_ruler_data = mem_malloc(sizeof(UI_Ruler_Data));

   _ruler_data->orient = HORIZONTAL;
   _ruler_data->visible = EINA_TRUE;

/*   _ruler_data->abs_scale = mem_malloc(sizeof(Scale));
   _ruler_data->abs_scale->step = 10.0;
   _ruler_data->abs_scale->dash_counter = 0;
   _ruler_data->abs_scale->dashes = NULL;
   _ruler_data->abs_scale->marks = NULL;
   _ruler_data->abs_scale->visible = EINA_TRUE;
   _ruler_data->abs_scale->start = 0;
   _ruler_data->abs_scale->end = 0;

   _ruler_data->rel_scale = mem_malloc(sizeof(Scale));
   _ruler_data->rel_scale->step = 0.5;
   _ruler_data->rel_scale->dash_counter = 0;
   _ruler_data->rel_scale->dashes = NULL;
   _ruler_data->rel_scale->marks = NULL;
   _ruler_data->rel_scale->visible = EINA_FALSE;
   _ruler_data->rel_scale->start = 0;
   _ruler_data->rel_scale->end = 0;
*/
   _ruler_data->abs_scale_visible = EINA_TRUE;
   _ruler_data->abs_scale_visible = EINA_TRUE;
   //_ruler_data->pointer = NULL; //TODO: Ask WTF? :)
   _ruler_data->pointer_pos = 0.0;
   _ruler_data->pointer_visible = EINA_TRUE;
   return _ruler_data;
}

/*static void
_ruler_resize_cb(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj,
                  void *event_info __UNUSED__)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   if (_ruler_data->visible)
     _scales_repaint(obj);
}*/

/*static void
_ruler_move_cb(void *data __UNUSED__, Evas *e __UNUSED__, Evas_Object *obj,
               void *event_info __UNUSED__)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj,RULERDATAKEY);
   if (_ruler_data->visible)
     _scales_repaint(obj);
}*/

/* TODO: make correct return value at absolute coordinates*/
int
ui_ruler_pointer_pos_get(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   return _ruler_data->pointer_pos;
}

void
ui_ruler_pointer_pos_set(Evas_Object *obj, int pos)
{
   int x, y, w, h;
   double dx, dy;
   dx = dy = 0.0;
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   evas_object_geometry_get(obj, &x, &y, &w, &h);

   if (_ruler_data->orient == HORIZONTAL)
     {
        dx =(double)(pos - x) / w;
        _ruler_data->pointer_pos = dx;
     }
   else
     {
        dy = (double)(pos - y) / h;
        _ruler_data->pointer_pos = dy;
     }
   edje_object_part_drag_value_set(obj, "pointer", dx, dy);
}

Orient
ui_ruler_orient_get(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   return _ruler_data->orient;
}

void
ui_ruler_orient_set(Evas_Object *obj, Orient orient)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   _ruler_data->orient = orient;
   if (_ruler_data->orient == HORIZONTAL)
     {
        edje_object_file_set(obj, TET_EDJ, "ui/ruler/default");
//        edje_object_file_set(obj, TET_EDJ, "ui/ruler/horizontal/default");
     }
   else
     {
        edje_object_file_set(obj, TET_EDJ, "ui/ruler/default");
//        evas_object_event_callback_add(obj, EVAS_CALLBACK_MOVE, _ruler_move_cb,
//          _ruler_data);
     }

}

void
ui_ruler_pointer_visible_set(Evas_Object *obj, Eina_Bool visible)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   _ruler_data->pointer_visible = visible;
   if (visible)
     edje_object_signal_emit (obj, "ruler,pointer,show", "");
   else
     edje_object_signal_emit (obj, "ruler,pointer,hide", "");
}

Eina_Bool
ui_ruler_pointer_visible_get(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   return _ruler_data->pointer_visible;
}

void
ui_ruler_step_scale_absolute_set(Evas_Object *obj, int step)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   _ruler_data->abs_scale_step = (double)step;
   //_scales_repaint(obj);
}

unsigned int
ui_ruler_step_absolute_get(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   return (int)_ruler_data->abs_scale_step;
}

void
ui_ruler_step_relative_set(Evas_Object *obj, double step)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   _ruler_data->rel_scale_step = step;
   //_scales_repaint(obj);
}

double
ui_ruler_step_relative_get (Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj,RULERDATAKEY);
   return _ruler_data->rel_scale_step;
}

void
ui_ruler_scale_relative_visible_set(Evas_Object *obj, Eina_Bool enable)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   _ruler_data->rel_scale_visible = enable;
   //_scales_repaint(obj);
}

Eina_Bool
ui_ruler_scale_relative_visible_get(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   return _ruler_data->rel_scale_visible;
}

void
ui_ruler_scale_absolute_visible_set(Evas_Object *obj, Eina_Bool enable)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   _ruler_data->abs_scale_visible = enable;
   //_scales_repaint(obj);
}

Eina_Bool
ui_ruler_scale_absolute_visible_get(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   return _ruler_data->abs_scale_visible;
}

void
ui_ruler_scale_relative_position_set(Evas_Object *obj, int start, int end)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
//   _ruler_data->rel_scale->start = start;
//   _ruler_data->rel_scale->end = end;
//   _scales_repaint(obj);
}

void
ui_ruler_scale_relative_position_get(Evas_Object *obj, int *start, int *end)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
//   *start = _ruler_data->rel_scale->start;
//   *end = _ruler_data->rel_scale->end;
}

void
ui_ruler_scale_absolute_position_zero_set(Evas_Object *obj, int pos)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
//   _absolute_scale_clear(obj);
//   _ruler_data->abs_scale->start = pos;
//  _scales_repaint(obj);

}

int
ui_ruler_scale_absolute_position_zero_get(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   return 1;//(int)_ruler_data->abs_scale->start;
}

void
ui_ruler_hide(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   _ruler_data->visible = EINA_FALSE;
//   _absolute_scale_clear(obj);
//   _relative_scale_marks_del(obj);
   ui_ruler_pointer_visible_set(obj, EINA_FALSE);

   edje_object_signal_emit (obj, "ruler,bg,hide", "");
   evas_object_hide(obj);
}

Eina_Bool
ui_ruler_visible_get(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   return  _ruler_data->visible;
}

void
ui_ruler_show(Evas_Object *obj)
{
   UI_Ruler_Data *_ruler_data = evas_object_data_get(obj, RULERDATAKEY);
   _ruler_data->visible = EINA_TRUE;
//   _scales_repaint(obj);
   ui_ruler_pointer_visible_set(obj, EINA_TRUE);
   edje_object_signal_emit (obj, "ruler,bg,show", "");
  evas_object_show(obj);
}

void
ui_ruler_update(Evas_Object *obj)
{
//   _absolute_scale_clear(obj);
//   _relative_scale_marks_del(obj);
//   _scales_repaint(obj);
}

Evas_Object *
ui_ruler_add(Evas_Object *parent)
{
   Evas_Object *ruler;
   UI_Ruler_Data *ruler_data;

   ruler_data = _ruler_data_init();
   if (!ruler_data)
     {
        ERR ("Unable initialize ruler data structure");
        return NULL;
     }

   ruler = edje_object_add(evas_object_evas_get(parent));
   if (!ruler)
     {
        ERR("Unable to create ruler");
        free(ruler_data);
        return NULL;
     }
   evas_object_data_set(ruler, RULERDATAKEY, ruler_data);
//   evas_object_event_callback_add(ruler, EVAS_CALLBACK_RESIZE, _ruler_resize_cb,
//     ruler_data);
   ui_ruler_orient_set(ruler, HORIZONTAL);
    evas_object_resize(ruler, 100, 40);
    evas_object_show(ruler);

   return ruler;
}
