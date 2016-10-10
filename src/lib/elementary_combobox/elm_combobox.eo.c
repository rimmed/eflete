const Eo_Event_Description _ELM_COMBOBOX_EVENT_DISMISSED =
   EO_EVENT_DESCRIPTION("dismissed");
const Eo_Event_Description _ELM_COMBOBOX_EVENT_EXPANDED =
   EO_EVENT_DESCRIPTION("expanded");
const Eo_Event_Description _ELM_COMBOBOX_EVENT_CLICKED =
   EO_EVENT_DESCRIPTION("clicked");
const Eo_Event_Description _ELM_COMBOBOX_EVENT_ITEM_SELECTED =
   EO_EVENT_DESCRIPTION("item,selected");
const Eo_Event_Description _ELM_COMBOBOX_EVENT_ITEM_PRESSED =
   EO_EVENT_DESCRIPTION("item,pressed");
const Eo_Event_Description _ELM_COMBOBOX_EVENT_FILTER_DONE =
   EO_EVENT_DESCRIPTION("filter,done");

Eina_Bool _elm_combobox_expanded_get(Eo *obj, Elm_Combobox_Data *pd);

EO_FUNC_BODY(elm_obj_combobox_expanded_get, Eina_Bool, 0);

void _elm_combobox_hover_begin(Eo *obj, Elm_Combobox_Data *pd);

EO_VOID_FUNC_BODY(elm_obj_combobox_hover_begin);

void _elm_combobox_hover_end(Eo *obj, Elm_Combobox_Data *pd);

EO_VOID_FUNC_BODY(elm_obj_combobox_hover_end);

Eo_Base * _elm_combobox_eo_base_constructor(Eo *obj, Elm_Combobox_Data *pd);


void _elm_combobox_evas_object_smart_hide(Eo *obj, Elm_Combobox_Data *pd);


void _elm_combobox_evas_object_smart_show(Eo *obj, Elm_Combobox_Data *pd);


void _elm_combobox_evas_object_smart_add(Eo *obj, Elm_Combobox_Data *pd);


void _elm_combobox_evas_object_smart_del(Eo *obj, Elm_Combobox_Data *pd);


void _elm_combobox_evas_object_smart_resize(Eo *obj, Elm_Combobox_Data *pd, Evas_Coord w, Evas_Coord h);


void _elm_combobox_elm_widget_part_text_set(Eo *obj, Elm_Combobox_Data *pd, const char * part, const char *label);


const char * _elm_combobox_elm_widget_part_text_get(Eo *obj, Elm_Combobox_Data *pd, const char * part);


Elm_Theme_Apply _elm_combobox_elm_widget_theme_apply(Eo *obj, Elm_Combobox_Data *pd);


Eina_Bool _elm_combobox_elm_widget_translate(Eo *obj, Elm_Combobox_Data *pd);


Eina_Bool _elm_combobox_elm_widget_event(Eo *obj, Elm_Combobox_Data *pd, Evas_Object *source, Evas_Callback_Type type, void *event_info);


Eina_Bool _elm_combobox_elm_button_admits_autorepeat_get(Eo *obj, Elm_Combobox_Data *pd);


void _elm_combobox_elm_genlist_filter_set(Eo *obj, Elm_Combobox_Data *pd, void *key);


const Elm_Atspi_Action * _elm_combobox_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj, Elm_Combobox_Data *pd);


static const Eo_Op_Description _elm_combobox_op_desc[] = {
     EO_OP_FUNC_OVERRIDE(eo_constructor, _elm_combobox_eo_base_constructor),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_hide, _elm_combobox_evas_object_smart_hide),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_show, _elm_combobox_evas_object_smart_show),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_add, _elm_combobox_evas_object_smart_add),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_del, _elm_combobox_evas_object_smart_del),
     EO_OP_FUNC_OVERRIDE(evas_obj_smart_resize, _elm_combobox_evas_object_smart_resize),
     EO_OP_FUNC_OVERRIDE(elm_obj_widget_part_text_set, _elm_combobox_elm_widget_part_text_set),
     EO_OP_FUNC_OVERRIDE(elm_obj_widget_part_text_get, _elm_combobox_elm_widget_part_text_get),
     EO_OP_FUNC_OVERRIDE(elm_obj_widget_theme_apply, _elm_combobox_elm_widget_theme_apply),
     EO_OP_FUNC_OVERRIDE(elm_obj_widget_translate, _elm_combobox_elm_widget_translate),
     EO_OP_FUNC_OVERRIDE(elm_obj_widget_event, _elm_combobox_elm_widget_event),
     EO_OP_FUNC_OVERRIDE(elm_obj_button_admits_autorepeat_get, _elm_combobox_elm_button_admits_autorepeat_get),
     EO_OP_FUNC_OVERRIDE(elm_obj_genlist_filter_set, _elm_combobox_elm_genlist_filter_set),
     EO_OP_FUNC_OVERRIDE(elm_interface_atspi_widget_action_elm_actions_get, _elm_combobox_elm_interface_atspi_widget_action_elm_actions_get),
     EO_OP_FUNC(elm_obj_combobox_expanded_get, _elm_combobox_expanded_get),
     EO_OP_FUNC(elm_obj_combobox_hover_begin, _elm_combobox_hover_begin),
     EO_OP_FUNC(elm_obj_combobox_hover_end, _elm_combobox_hover_end),
};

static const Eo_Event_Description *_elm_combobox_event_desc[] = {
     ELM_COMBOBOX_EVENT_DISMISSED,
     ELM_COMBOBOX_EVENT_EXPANDED,
     ELM_COMBOBOX_EVENT_CLICKED,
     ELM_COMBOBOX_EVENT_ITEM_SELECTED,
     ELM_COMBOBOX_EVENT_ITEM_PRESSED,
     ELM_COMBOBOX_EVENT_FILTER_DONE,
     NULL
};

static const Eo_Class_Description _elm_combobox_class_desc = {
     EO_VERSION,
     "Elm_Combobox",
     EO_CLASS_TYPE_REGULAR,
     EO_CLASS_DESCRIPTION_OPS(_elm_combobox_op_desc),
     _elm_combobox_event_desc,
     sizeof(Elm_Combobox_Data),
     _elm_combobox_class_constructor,
     NULL
};

EO_DEFINE_CLASS(elm_combobox_class_get, &_elm_combobox_class_desc, ELM_BUTTON_CLASS, EVAS_SELECTABLE_INTERFACE_INTERFACE, ELM_INTERFACE_ATSPI_WIDGET_ACTION_MIXIN, ELM_ENTRY_CLASS, ELM_GENLIST_CLASS, ELM_HOVER_CLASS, NULL);
Eina_Bool
elm_combobox_expanded_get(const Elm_Combobox *obj)
{
   Eina_Bool ret;
   eo_do((Elm_Combobox *)obj, ret = elm_obj_combobox_expanded_get());
   return ret;
}

void
elm_combobox_hover_begin(Elm_Combobox *obj)
{
   eo_do((Elm_Combobox *)obj, elm_obj_combobox_hover_begin());
}

void
elm_combobox_hover_end(Elm_Combobox *obj)
{
   eo_do((Elm_Combobox *)obj, elm_obj_combobox_hover_end());
}
