Eina_Bool _ewe_ruler_horizontal_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Eina_Bool horizontal);

EO_FUNC_BODYV(ewe_obj_ruler_horizontal_set, Eina_Bool, 0, EO_FUNC_CALL(horizontal), Eina_Bool horizontal);

Eina_Bool _ewe_ruler_horizontal_get(Eo *obj, Ewe_Ruler_Smart_Data *pd);

EO_FUNC_BODY(ewe_obj_ruler_horizontal_get, Eina_Bool, 0);

Eina_Bool _ewe_ruler_step_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale, unsigned int step);

EO_FUNC_BODYV(ewe_obj_ruler_step_set, Eina_Bool, 0, EO_FUNC_CALL(scale, step), Ewe_Ruler_Scale * scale, unsigned int step);

unsigned int _ewe_ruler_step_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale);

EO_FUNC_BODYV(ewe_obj_ruler_step_get, unsigned int, 0, EO_FUNC_CALL(scale), Ewe_Ruler_Scale * scale);

Eina_Bool _ewe_ruler_value_step_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale, double step);

EO_FUNC_BODYV(ewe_obj_ruler_value_step_set, Eina_Bool, 0, EO_FUNC_CALL(scale, step), Ewe_Ruler_Scale * scale, double step);

double _ewe_ruler_value_step_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale);

EO_FUNC_BODYV(ewe_obj_ruler_value_step_get, double, 0, EO_FUNC_CALL(scale), Ewe_Ruler_Scale * scale);

Eina_Bool _ewe_ruler_zero_offset_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale, int offset);

EO_FUNC_BODYV(ewe_obj_ruler_zero_offset_set, Eina_Bool, 0, EO_FUNC_CALL(scale, offset), Ewe_Ruler_Scale * scale, int offset);

int _ewe_ruler_zero_offset_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale);

EO_FUNC_BODYV(ewe_obj_ruler_zero_offset_get, int, 0, EO_FUNC_CALL(scale), Ewe_Ruler_Scale * scale);

Eina_Bool _ewe_ruler_format_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale, const char *format);

EO_FUNC_BODYV(ewe_obj_ruler_format_set, Eina_Bool, 0, EO_FUNC_CALL(scale, format), Ewe_Ruler_Scale * scale, const char *format);

const char * _ewe_ruler_format_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale);

EO_FUNC_BODYV(ewe_obj_ruler_format_get, const char *, 0, EO_FUNC_CALL(scale), Ewe_Ruler_Scale * scale);

Eina_Bool _ewe_ruler_style_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale, const char *style);

EO_FUNC_BODYV(ewe_obj_ruler_style_set, Eina_Bool, 0, EO_FUNC_CALL(scale, style), Ewe_Ruler_Scale * scale, const char *style);

const char * _ewe_ruler_style_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale);

EO_FUNC_BODYV(ewe_obj_ruler_style_get, const char *, 0, EO_FUNC_CALL(scale), Ewe_Ruler_Scale * scale);

Eina_Bool _ewe_ruler_scale_visible_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale, Eina_Bool visible);

EO_FUNC_BODYV(ewe_obj_ruler_scale_visible_set, Eina_Bool, 0, EO_FUNC_CALL(scale, visible), Ewe_Ruler_Scale * scale, Eina_Bool visible);

Eina_Bool _ewe_ruler_scale_visible_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale);

EO_FUNC_BODYV(ewe_obj_ruler_scale_visible_get, Eina_Bool, 0, EO_FUNC_CALL(scale), Ewe_Ruler_Scale * scale);

Eina_Bool _ewe_ruler_scale_middle_mark_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale, Eina_Bool middle_mark);

EO_FUNC_BODYV(ewe_obj_ruler_scale_middle_mark_set, Eina_Bool, 0, EO_FUNC_CALL(scale, middle_mark), Ewe_Ruler_Scale * scale, Eina_Bool middle_mark);

Eina_Bool _ewe_ruler_scale_middle_mark_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale * scale);

EO_FUNC_BODYV(ewe_obj_ruler_scale_middle_mark_get, Eina_Bool, 0, EO_FUNC_CALL(scale), Ewe_Ruler_Scale * scale);

Eina_Bool _ewe_ruler_marker_relative_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker, Ewe_Ruler_Scale * scale, double rel_position);

EO_FUNC_BODYV(ewe_obj_ruler_marker_relative_set, Eina_Bool, 0, EO_FUNC_CALL(marker, scale, rel_position), Ewe_Ruler_Marker * marker, Ewe_Ruler_Scale * scale, double rel_position);

double _ewe_ruler_marker_relative_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker, Ewe_Ruler_Scale * scale);

EO_FUNC_BODYV(ewe_obj_ruler_marker_relative_get, double, 0, EO_FUNC_CALL(marker, scale), Ewe_Ruler_Marker * marker, Ewe_Ruler_Scale * scale);

Eina_Bool _ewe_ruler_marker_absolute_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker, int position);

EO_FUNC_BODYV(ewe_obj_ruler_marker_absolute_set, Eina_Bool, 0, EO_FUNC_CALL(marker, position), Ewe_Ruler_Marker * marker, int position);

int _ewe_ruler_marker_absolute_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker);

EO_FUNC_BODYV(ewe_obj_ruler_marker_absolute_get, int, 0, EO_FUNC_CALL(marker), Ewe_Ruler_Marker * marker);

Eina_Bool _ewe_ruler_marker_visible_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker, Eina_Bool visible);

EO_FUNC_BODYV(ewe_obj_ruler_marker_visible_set, Eina_Bool, 0, EO_FUNC_CALL(marker, visible), Ewe_Ruler_Marker * marker, Eina_Bool visible);

Eina_Bool _ewe_ruler_marker_visible_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker);

EO_FUNC_BODYV(ewe_obj_ruler_marker_visible_get, Eina_Bool, 0, EO_FUNC_CALL(marker), Ewe_Ruler_Marker * marker);

Eina_Bool _ewe_ruler_marker_style_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker, const char *style);

EO_FUNC_BODYV(ewe_obj_ruler_marker_style_set, Eina_Bool, 0, EO_FUNC_CALL(marker, style), Ewe_Ruler_Marker * marker, const char *style);

const char * _ewe_ruler_marker_style_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker);

EO_FUNC_BODYV(ewe_obj_ruler_marker_style_get, const char *, 0, EO_FUNC_CALL(marker), Ewe_Ruler_Marker * marker);

void _ewe_ruler_marker_size_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker, int size);

EO_VOID_FUNC_BODYV(ewe_obj_ruler_marker_size_set, EO_FUNC_CALL(marker, size), Ewe_Ruler_Marker * marker, int size);

int _ewe_ruler_marker_size_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker);

EO_FUNC_BODYV(ewe_obj_ruler_marker_size_get, int, 0, EO_FUNC_CALL(marker), Ewe_Ruler_Marker * marker);

void _ewe_ruler_marker_text_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker, const char *text);

EO_VOID_FUNC_BODYV(ewe_obj_ruler_marker_text_set, EO_FUNC_CALL(marker, text), Ewe_Ruler_Marker * marker, const char *text);

const char * _ewe_ruler_marker_text_get(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker);

EO_FUNC_BODYV(ewe_obj_ruler_marker_text_get, const char *, 0, EO_FUNC_CALL(marker), Ewe_Ruler_Marker * marker);

void _ewe_ruler_marker_tooltip_set(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker * marker, const char *text);

EO_VOID_FUNC_BODYV(ewe_obj_ruler_marker_tooltip_set, EO_FUNC_CALL(marker, text), Ewe_Ruler_Marker * marker, const char *text);

Ewe_Ruler_Scale * _ewe_ruler_scale_add(Eo *obj, Ewe_Ruler_Smart_Data *pd, const char *style);

EO_FUNC_BODYV(ewe_obj_ruler_scale_add, Ewe_Ruler_Scale *, 0, EO_FUNC_CALL(style), const char *style);

Eina_Bool _ewe_ruler_scale_del(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Scale *scale);

EO_FUNC_BODYV(ewe_obj_ruler_scale_del, Eina_Bool, 0, EO_FUNC_CALL(scale), Ewe_Ruler_Scale *scale);

Ewe_Ruler_Marker * _ewe_ruler_marker_add(Eo *obj, Ewe_Ruler_Smart_Data *pd, const char *style);

EO_FUNC_BODYV(ewe_obj_ruler_marker_add, Ewe_Ruler_Marker *, 0, EO_FUNC_CALL(style), const char *style);

Eina_Bool _ewe_ruler_marker_del(Eo *obj, Ewe_Ruler_Smart_Data *pd, Ewe_Ruler_Marker *marker);

EO_FUNC_BODYV(ewe_obj_ruler_marker_del, Eina_Bool, 0, EO_FUNC_CALL(marker), Ewe_Ruler_Marker *marker);

Eo_Base * _ewe_ruler_eo_base_constructor(Eo *obj, Ewe_Ruler_Smart_Data *pd);


void _ewe_ruler_evas_object_smart_add(Eo *obj, Ewe_Ruler_Smart_Data *pd);


void _ewe_ruler_evas_object_smart_del(Eo *obj, Ewe_Ruler_Smart_Data *pd);


void _ewe_ruler_evas_object_smart_resize(Eo *obj, Ewe_Ruler_Smart_Data *pd, Evas_Coord w, Evas_Coord h);


void _ewe_ruler_evas_object_smart_calculate(Eo *obj, Ewe_Ruler_Smart_Data *pd);


void _ewe_ruler_evas_object_smart_move(Eo *obj, Ewe_Ruler_Smart_Data *pd, Evas_Coord x, Evas_Coord y);


void _ewe_ruler_evas_object_smart_show(Eo *obj, Ewe_Ruler_Smart_Data *pd);


void _ewe_ruler_evas_object_smart_hide(Eo *obj, Ewe_Ruler_Smart_Data *pd);


Elm_Theme_Apply _ewe_ruler_elm_widget_theme_apply(Eo *obj, Ewe_Ruler_Smart_Data *pd);


static const Eo_Op_Description _ewe_ruler_op_desc[] = {
     EO_OP_FUNC_OVERRIDE(eo_constructor, _ewe_ruler_eo_base_constructor),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_add, _ewe_ruler_evas_object_smart_add),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_del, _ewe_ruler_evas_object_smart_del),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_resize, _ewe_ruler_evas_object_smart_resize),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_calculate, _ewe_ruler_evas_object_smart_calculate),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_move, _ewe_ruler_evas_object_smart_move),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_show, _ewe_ruler_evas_object_smart_show),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_hide, _ewe_ruler_evas_object_smart_hide),
     EO_OP_FUNC_OVERRIDE(elm_obj_widget_theme_apply, _ewe_ruler_elm_widget_theme_apply),
     EO_OP_FUNC(ewe_obj_ruler_horizontal_set, _ewe_ruler_horizontal_set),
     EO_OP_FUNC(ewe_obj_ruler_horizontal_get, _ewe_ruler_horizontal_get),
     EO_OP_FUNC(ewe_obj_ruler_step_set, _ewe_ruler_step_set),
     EO_OP_FUNC(ewe_obj_ruler_step_get, _ewe_ruler_step_get),
     EO_OP_FUNC(ewe_obj_ruler_value_step_set, _ewe_ruler_value_step_set),
     EO_OP_FUNC(ewe_obj_ruler_value_step_get, _ewe_ruler_value_step_get),
     EO_OP_FUNC(ewe_obj_ruler_zero_offset_set, _ewe_ruler_zero_offset_set),
     EO_OP_FUNC(ewe_obj_ruler_zero_offset_get, _ewe_ruler_zero_offset_get),
     EO_OP_FUNC(ewe_obj_ruler_format_set, _ewe_ruler_format_set),
     EO_OP_FUNC(ewe_obj_ruler_format_get, _ewe_ruler_format_get),
     EO_OP_FUNC(ewe_obj_ruler_style_set, _ewe_ruler_style_set),
     EO_OP_FUNC(ewe_obj_ruler_style_get, _ewe_ruler_style_get),
     EO_OP_FUNC(ewe_obj_ruler_scale_visible_set, _ewe_ruler_scale_visible_set),
     EO_OP_FUNC(ewe_obj_ruler_scale_visible_get, _ewe_ruler_scale_visible_get),
     EO_OP_FUNC(ewe_obj_ruler_scale_middle_mark_set, _ewe_ruler_scale_middle_mark_set),
     EO_OP_FUNC(ewe_obj_ruler_scale_middle_mark_get, _ewe_ruler_scale_middle_mark_get),
     EO_OP_FUNC(ewe_obj_ruler_marker_relative_set, _ewe_ruler_marker_relative_set),
     EO_OP_FUNC(ewe_obj_ruler_marker_relative_get, _ewe_ruler_marker_relative_get),
     EO_OP_FUNC(ewe_obj_ruler_marker_absolute_set, _ewe_ruler_marker_absolute_set),
     EO_OP_FUNC(ewe_obj_ruler_marker_absolute_get, _ewe_ruler_marker_absolute_get),
     EO_OP_FUNC(ewe_obj_ruler_marker_visible_set, _ewe_ruler_marker_visible_set),
     EO_OP_FUNC(ewe_obj_ruler_marker_visible_get, _ewe_ruler_marker_visible_get),
     EO_OP_FUNC(ewe_obj_ruler_marker_style_set, _ewe_ruler_marker_style_set),
     EO_OP_FUNC(ewe_obj_ruler_marker_style_get, _ewe_ruler_marker_style_get),
     EO_OP_FUNC(ewe_obj_ruler_marker_size_set, _ewe_ruler_marker_size_set),
     EO_OP_FUNC(ewe_obj_ruler_marker_size_get, _ewe_ruler_marker_size_get),
     EO_OP_FUNC(ewe_obj_ruler_marker_text_set, _ewe_ruler_marker_text_set),
     EO_OP_FUNC(ewe_obj_ruler_marker_text_get, _ewe_ruler_marker_text_get),
     EO_OP_FUNC(ewe_obj_ruler_marker_tooltip_set, _ewe_ruler_marker_tooltip_set),
     EO_OP_FUNC(ewe_obj_ruler_scale_add, _ewe_ruler_scale_add),
     EO_OP_FUNC(ewe_obj_ruler_scale_del, _ewe_ruler_scale_del),
     EO_OP_FUNC(ewe_obj_ruler_marker_add, _ewe_ruler_marker_add),
     EO_OP_FUNC(ewe_obj_ruler_marker_del, _ewe_ruler_marker_del),
};

static const Eo_Class_Description _ewe_ruler_class_desc = {
     EO_VERSION,
     "Ewe_Ruler",
     EO_CLASS_TYPE_REGULAR,
     EO_CLASS_DESCRIPTION_OPS(_ewe_ruler_op_desc),
     NULL,
     sizeof(Ewe_Ruler_Smart_Data),
     _ewe_ruler_class_constructor,
     NULL
};

EO_DEFINE_CLASS(ewe_ruler_class_get, &_ewe_ruler_class_desc, ELM_WIDGET_CLASS, NULL);
Eina_Bool
ewe_ruler_horizontal_set(Ewe_Ruler *obj, Eina_Bool horizontal)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_horizontal_set(horizontal));
   return ret;
}

Eina_Bool
ewe_ruler_horizontal_get(const Ewe_Ruler *obj)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_horizontal_get());
   return ret;
}

Eina_Bool
ewe_ruler_step_set(Ewe_Ruler *obj, Ewe_Ruler_Scale * scale, unsigned int step)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_step_set(scale, step));
   return ret;
}

unsigned int
ewe_ruler_step_get(const Ewe_Ruler *obj, Ewe_Ruler_Scale * scale)
{
   unsigned int ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_step_get(scale));
   return ret;
}

Eina_Bool
ewe_ruler_value_step_set(Ewe_Ruler *obj, Ewe_Ruler_Scale * scale, double step)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_value_step_set(scale, step));
   return ret;
}

double
ewe_ruler_value_step_get(const Ewe_Ruler *obj, Ewe_Ruler_Scale * scale)
{
   double ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_value_step_get(scale));
   return ret;
}

Eina_Bool
ewe_ruler_zero_offset_set(Ewe_Ruler *obj, Ewe_Ruler_Scale * scale, int offset)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_zero_offset_set(scale, offset));
   return ret;
}

int
ewe_ruler_zero_offset_get(const Ewe_Ruler *obj, Ewe_Ruler_Scale * scale)
{
   int ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_zero_offset_get(scale));
   return ret;
}

Eina_Bool
ewe_ruler_format_set(Ewe_Ruler *obj, Ewe_Ruler_Scale * scale, const char *format)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_format_set(scale, format));
   return ret;
}

const char *
ewe_ruler_format_get(const Ewe_Ruler *obj, Ewe_Ruler_Scale * scale)
{
   const char * ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_format_get(scale));
   return ret;
}

Eina_Bool
ewe_ruler_style_set(Ewe_Ruler *obj, Ewe_Ruler_Scale * scale, const char *style)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_style_set(scale, style));
   return ret;
}

const char *
ewe_ruler_style_get(const Ewe_Ruler *obj, Ewe_Ruler_Scale * scale)
{
   const char * ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_style_get(scale));
   return ret;
}

Eina_Bool
ewe_ruler_scale_visible_set(Ewe_Ruler *obj, Ewe_Ruler_Scale * scale, Eina_Bool visible)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_scale_visible_set(scale, visible));
   return ret;
}

Eina_Bool
ewe_ruler_scale_visible_get(const Ewe_Ruler *obj, Ewe_Ruler_Scale * scale)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_scale_visible_get(scale));
   return ret;
}

Eina_Bool
ewe_ruler_scale_middle_mark_set(Ewe_Ruler *obj, Ewe_Ruler_Scale * scale, Eina_Bool middle_mark)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_scale_middle_mark_set(scale, middle_mark));
   return ret;
}

Eina_Bool
ewe_ruler_scale_middle_mark_get(const Ewe_Ruler *obj, Ewe_Ruler_Scale * scale)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_scale_middle_mark_get(scale));
   return ret;
}

Eina_Bool
ewe_ruler_marker_relative_set(Ewe_Ruler *obj, Ewe_Ruler_Marker * marker, Ewe_Ruler_Scale * scale, double rel_position)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_relative_set(marker, scale, rel_position));
   return ret;
}

double
ewe_ruler_marker_relative_get(const Ewe_Ruler *obj, Ewe_Ruler_Marker * marker, Ewe_Ruler_Scale * scale)
{
   double ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_relative_get(marker, scale));
   return ret;
}

Eina_Bool
ewe_ruler_marker_absolute_set(Ewe_Ruler *obj, Ewe_Ruler_Marker * marker, int position)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_absolute_set(marker, position));
   return ret;
}

int
ewe_ruler_marker_absolute_get(const Ewe_Ruler *obj, Ewe_Ruler_Marker * marker)
{
   int ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_absolute_get(marker));
   return ret;
}

Eina_Bool
ewe_ruler_marker_visible_set(Ewe_Ruler *obj, Ewe_Ruler_Marker * marker, Eina_Bool visible)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_visible_set(marker, visible));
   return ret;
}

Eina_Bool
ewe_ruler_marker_visible_get(const Ewe_Ruler *obj, Ewe_Ruler_Marker * marker)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_visible_get(marker));
   return ret;
}

Eina_Bool
ewe_ruler_marker_style_set(Ewe_Ruler *obj, Ewe_Ruler_Marker * marker, const char *style)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_style_set(marker, style));
   return ret;
}

const char *
ewe_ruler_marker_style_get(const Ewe_Ruler *obj, Ewe_Ruler_Marker * marker)
{
   const char * ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_style_get(marker));
   return ret;
}

void
ewe_ruler_marker_size_set(Ewe_Ruler *obj, Ewe_Ruler_Marker * marker, int size)
{
   eo_do((Ewe_Ruler *)obj, ewe_obj_ruler_marker_size_set(marker, size));
}

int
ewe_ruler_marker_size_get(const Ewe_Ruler *obj, Ewe_Ruler_Marker * marker)
{
   int ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_size_get(marker));
   return ret;
}

void
ewe_ruler_marker_text_set(Ewe_Ruler *obj, Ewe_Ruler_Marker * marker, const char *text)
{
   eo_do((Ewe_Ruler *)obj, ewe_obj_ruler_marker_text_set(marker, text));
}

const char *
ewe_ruler_marker_text_get(const Ewe_Ruler *obj, Ewe_Ruler_Marker * marker)
{
   const char * ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_text_get(marker));
   return ret;
}

void
ewe_ruler_marker_tooltip_set(Ewe_Ruler *obj, Ewe_Ruler_Marker * marker, const char *text)
{
   eo_do((Ewe_Ruler *)obj, ewe_obj_ruler_marker_tooltip_set(marker, text));
}

Ewe_Ruler_Scale *
ewe_ruler_scale_add(Ewe_Ruler *obj, const char *style)
{
   Ewe_Ruler_Scale * ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_scale_add(style));
   return ret;
}

Eina_Bool
ewe_ruler_scale_del(Ewe_Ruler *obj, Ewe_Ruler_Scale *scale)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_scale_del(scale));
   return ret;
}

Ewe_Ruler_Marker *
ewe_ruler_marker_add(Ewe_Ruler *obj, const char *style)
{
   Ewe_Ruler_Marker * ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_add(style));
   return ret;
}

Eina_Bool
ewe_ruler_marker_del(Ewe_Ruler *obj, Ewe_Ruler_Marker *marker)
{
   Eina_Bool ret;
   eo_do((Ewe_Ruler *)obj, ret = ewe_obj_ruler_marker_del(marker));
   return ret;
}
