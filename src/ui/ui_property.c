#include "ui_property.h"

struct _Prop_View_Data
{
	Evas_Object *group_prop;
	Evas_Object *part_prop;
};

typedef struct _Prop_View_Data Prop_View_Data;
#define PROP_VIEW_DATA "prop_view_data"

struct _Prop_Part_View_Data
{
	Evas_Object *box;
	Evas_Object *base;
	struct {
		Evas_Object *state;
		Evas_Object *text;
		Evas_Object *image;
	} state;
	Evas_Object *dragable;
};

typedef struct _Prop_Part_View_Data Prop_Part_View_Data;
#define PROP_PART_VIEW_DATA "prop_part_view_data"

/**
 * ref http://docs.enlightenment.org/auto/edje/group__Edje__Object__Part.html
 */

static char *part_types[] = {
	"NONE",
	"RECTANGLE",
	"TEXT",
	"IMAGE",
	"SWALLOW",
	"TEXTBLOCK",
	"GRADIENT",
	"GROUP",
	"BOX",
	"TABLE",
	"EXTERNAL",
	"PROXY",
	"SPACER"
};

void
_on_group_view_del(void *data __UNUSED__,
					Evas *e __UNUSED__,
					Evas_Object *obj,
					void *event_info __UNUSED__)
{
	free(evas_object_data_get(obj, PROP_VIEW_DATA));
}

void
_on_part_view_del(void *data __UNUSED__,
					Evas *e __UNUSED__,
					Evas_Object *obj,
					void *event_info __UNUSED__)
{
	free(evas_object_data_get(obj, PROP_PART_VIEW_DATA));
}

Evas_Object *
ui_property_view_new(Evas_Object *parent)
{
	Evas_Object *box, *scroller;
	Prop_View_Data *pvd;

	if(!parent)
		return NULL;

	pvd = malloc(sizeof(Prop_View_Data));
	if(!pvd)
		return NULL;

	pvd->group_prop = NULL;
	pvd->part_prop = NULL;

	scroller = elm_scroller_add(parent);
	evas_object_size_hint_weight_set(scroller, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(scroller, EVAS_HINT_FILL, EVAS_HINT_FILL);

	box = elm_box_add(scroller);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, EVAS_HINT_FILL);
	elm_box_align_set(box, 0.5, 0.0);
	evas_object_data_set(scroller, PROP_VIEW_DATA, pvd);

	evas_object_event_callback_add(scroller, EVAS_CALLBACK_DEL,
									_on_group_view_del, NULL);

	elm_object_content_set(scroller, box);

	return scroller;
}

void
ui_property_group_view_set(Evas_Object *prop_view, Evas_Object *group_view)
{
	Prop_View_Data *pvd;

	if(!prop_view || !group_view)
		return;

	pvd = evas_object_data_get(prop_view, PROP_VIEW_DATA);
	if(pvd->group_prop)
		evas_object_del(pvd->group_prop);
	pvd->group_prop = group_view;
	elm_box_pack_start(elm_object_content_get(prop_view), group_view);
}

void
ui_property_part_view_set(Evas_Object *prop_view, Evas_Object *part_view)
{
	Prop_View_Data *pvd;

	if(!prop_view || !part_view)
		return;

	pvd = evas_object_data_get(prop_view, PROP_VIEW_DATA);
	if(pvd->part_prop)
		evas_object_del(pvd->part_prop);
	pvd->part_prop = part_view;
	elm_box_pack_end(elm_object_content_get(prop_view), part_view);
}

Evas_Object *
ui_property_group_view_get(Evas_Object *prop_view)
{
	Prop_View_Data *pvd;

	if(!prop_view)
		return NULL;

	pvd = evas_object_data_get(prop_view, PROP_VIEW_DATA);
	return pvd->group_prop;
}

Evas_Object *
ui_property_part_view_get(Evas_Object *prop_view)
{
	Prop_View_Data *pvd;

	if(!prop_view)
		return NULL;

	pvd = evas_object_data_get(prop_view, PROP_VIEW_DATA);
	return pvd->part_prop;
}

void
ui_prop_group_info_view_update(Evas_Object *prop_view, Group *group)
{
	Evas_Object *group_view;

	if(!prop_view || !group)
		return;

	group_view = ui_prop_group_info_view_add(prop_view, group);
	evas_object_show(group_view);
	ui_property_group_view_set(prop_view, group_view);
}

Evas_Object *
ui_prop_group_info_view_add(Evas_Object *prop_view, Group *group)
{
	Evas_Object *group_view, *box, *item;

	if(!prop_view)
		return NULL;

	group_view = elm_frame_add(prop_view);
	elm_frame_autocollapse_set(group_view, EINA_TRUE);
	evas_object_size_hint_fill_set(group_view, EVAS_HINT_FILL, 0.0);
	elm_object_text_set(group_view, "Group property");

	box = elm_box_add(group_view);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, 0.0);
	elm_box_align_set(box, 0.5, 0.0);

	item = ui_prop_view_item_label_add(box, "group", group->full_group_name);
	evas_object_show(item);
	elm_box_pack_end(box, item);

	item = ui_prop_view_item_two_edit_int_add(box, "min",
				&group->min_w, "Minimum group width in pixels.",
				&group->min_h, "Minimum group height in pixels.");
	evas_object_show(item);

	item = ui_prop_view_item_two_edit_int_add(box, "max",
				&group->max_w, "Maximum group width in pixels.",
				&group->max_h, "Maximum group height in pixels.");
	evas_object_show(item);

	item = ui_prop_view_item_two_edit_int_add(box, "current",
				&group->current_w, "Current group width in pixels.",
				&group->current_h, "Current group height in pixels.");
	evas_object_show(item);

	evas_object_show(box);
	elm_object_content_set(group_view, box);

	return group_view;
}

Evas_Object *
ui_prop_part_info_view_add(Evas_Object *prop_view, Part *part)
{
	Evas_Object *part_view, *box;
	Evas_Object *part_view_base, *box_base, *item;
	Prop_Part_View_Data *ppvd;

	if(!prop_view)
		return NULL;

	part_view = elm_frame_add(prop_view);
	elm_frame_autocollapse_set(part_view, EINA_TRUE);
	evas_object_size_hint_fill_set(part_view, EVAS_HINT_FILL, 0.0);
	elm_object_text_set(part_view, "Part property");

	box = elm_box_add(part_view);
	evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(box, EVAS_HINT_FILL, 0.0);
	elm_box_align_set(box, 0.5, 0.0);

	ppvd = malloc(sizeof(Prop_Part_View_Data));

	part_view_base = elm_frame_add(box);
	elm_frame_autocollapse_set(part_view_base, EINA_TRUE);
	evas_object_size_hint_fill_set(part_view_base, EVAS_HINT_FILL, 0.0);
	elm_object_style_set(part_view_base, "pad_small");
	elm_box_pack_end(box, part_view_base);
	evas_object_show(part_view_base);

	box_base = elm_box_add(part_view_base);
	evas_object_size_hint_weight_set(box_base, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(box_base, EVAS_HINT_FILL, 0.0);
	elm_box_align_set(box_base, 0.5, 0.0);
	elm_object_content_set(part_view_base, box_base);
	evas_object_show(box_base);

	ppvd->box = box;
	ppvd->base = part_view_base;
	ppvd->state.state = NULL;
	ppvd->state.text = NULL;
	ppvd->state.image = NULL;
	ppvd->dragable = NULL;

	item = ui_prop_view_item_one_edit_string_add(box_base, "name",
				part->name, "Unique name of part.");
	evas_object_show(item);

	item = ui_prop_view_item_label_add(box_base, "type", part_types[part->type]);
	evas_object_show(item);

	item = ui_prop_view_item_one_edit_string_add(box_base, "clip to",
				part->clip_to, "Reference clipper part.");
	evas_object_show(item);

	/*TODO: create a button, with provide a selecting another group */
	item = ui_prop_view_item_one_edit_string_add(box_base, "source",
				part->source, "Source group selection.");
	evas_object_show(item);

	item = ui_prop_view_item_toggle_add(box_base, "mouse events",
				part->mouse_events, "Enable mouse events in this part.");
	evas_object_show(item);

	item = ui_prop_view_item_toggle_add(box_base, "repeat events",
				part->repeat_events, "Enable repeat mouse events to the parts below.");
	evas_object_show(item);

	evas_object_show(box);
	elm_object_content_set(part_view, box);

	evas_object_data_set(part_view, PROP_PART_VIEW_DATA, ppvd);
	evas_object_event_callback_add(box, EVAS_CALLBACK_DEL,
									_on_part_view_del, NULL);

	return part_view;
}

Evas_Object *
ui_prop_part_info_state_view_add(Evas_Object *part_view, Part_State *state)
{
	Evas_Object *part_view_state, *box_state, *item;
	Evas_Object *rel1_frame, *rel1_box, *rel2_frame, *rel2_box;
	Prop_Part_View_Data *ppvd;
	char buffer[BUFF_MAX];

	if(!part_view || !state)
		return NULL;

	ppvd = evas_object_data_get(part_view, PROP_PART_VIEW_DATA);
	if(!ppvd)
		return NULL;

	part_view_state = elm_frame_add(ppvd->box);
	elm_frame_autocollapse_set(part_view_state, EINA_TRUE);
	evas_object_size_hint_fill_set(part_view_state, EVAS_HINT_FILL, 0.0);
	elm_object_style_set(part_view_state, "pad_small");

	box_state = elm_box_add(part_view_state);
	evas_object_size_hint_weight_set(box_state, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(box_state, EVAS_HINT_FILL, 0.0);
	elm_box_align_set(box_state, 0.5, 0.0);
	elm_object_content_set(part_view_state, box_state);
	evas_object_show(box_state);

	sprintf(buffer, "%s %f", state->name, state->value);
	item = ui_prop_view_item_one_edit_string_add(box_state, "state",
				buffer, "Unique state name for part.");
	evas_object_show(item);

	item = ui_prop_view_item_toggle_add(box_state, "visible",
				state->visible, "Change part's visibility.'");
	evas_object_show(item);

	item = ui_prop_view_item_two_edit_int_add(box_state, "min",
				&state->min_w, "Minimum part width in pixels.",
				&state->min_h, "Minimum part height in pixels.");
	evas_object_show(item);

	item = ui_prop_view_item_two_edit_int_add(box_state, "max",
				&state->max_w, "Maximum part width in pixels.",
				&state->max_h, "Maximum part height in pixels.");
	evas_object_show(item);

	rel1_frame = elm_frame_add(box_state);
	elm_frame_autocollapse_set(rel1_frame, EINA_FALSE);
	evas_object_size_hint_fill_set(rel1_frame, EVAS_HINT_FILL, 0.0);
	elm_object_text_set(part_view, "rel1: top-left");
	evas_object_show(rel1_frame);

	rel1_box = elm_box_add(rel1_frame);
	evas_object_size_hint_weight_set(rel1_box, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(rel1_box, EVAS_HINT_FILL, 0.0);
	elm_box_align_set(rel1_box, 0.5, 0.0);
	elm_object_content_set(rel1_frame, rel1_box);
	evas_object_show(box_state);

	/*TODO: update tooltip */
	item = ui_prop_view_item_two_edit_double_add(rel1_box, "rel1",
				&state->rel1_relative_x, "Relative 1 x",
				&state->rel1_relative_y, "Relative 1 y");
	evas_object_show(item);

	/*TODO: update name */
	item = ui_prop_view_item_two_edit_int_add(rel1_box, "offset",
				&state->rel1_offset_x, "Left offset from relative position in pixels",
				&state->rel1_offset_y, "Top offset from relative position in pixels");
	evas_object_show(item);

	item = ui_prop_view_item_two_edit_string_add(rel1_box, "to",
				state->rel1_to_x_name, "Left reference part.",
				state->rel1_to_y_name, "Top reference part.");
	evas_object_show(item);

	elm_box_pack_end(box_state, rel1_frame);

	rel2_frame = elm_frame_add(box_state);
	elm_frame_autocollapse_set(rel2_frame, EINA_FALSE);
	evas_object_size_hint_fill_set(rel2_frame, EVAS_HINT_FILL, 0.0);
	elm_object_text_set(part_view, "rel2: bottom-right");
	evas_object_show(rel2_frame);

	rel2_box = elm_box_add(rel2_frame);
	evas_object_size_hint_weight_set(rel2_box, EVAS_HINT_EXPAND, 0.0);
	evas_object_size_hint_align_set(rel2_box, EVAS_HINT_FILL, 0.0);
	elm_box_align_set(rel2_box, 0.5, 0.0);
	elm_object_content_set(rel2_frame, rel2_box);
	evas_object_show(box_state);

	/*TODO: update tooltip */
	item = ui_prop_view_item_two_edit_double_add(rel2_box, "rel2",
				&state->rel2_relative_x, "Relative 2 x",
				&state->rel2_relative_y, "Relative 2 y");
	evas_object_show(item);

	/*TODO: update name */
	item = ui_prop_view_item_two_edit_int_add(rel2_box, "offset",
				&state->rel1_offset_x, "Right offset from relative position in pixels",
				&state->rel1_offset_y, "Bottom offset from relative position in pixels");
	evas_object_show(item);

	item = ui_prop_view_item_two_edit_string_add(rel2_box, "to",
				state->rel1_to_x_name, "Right reference part.",
				state->rel1_to_y_name, "Bottom reference part.");
	evas_object_show(item);

	item = ui_prop_view_item_two_edit_double_add(rel2_box, "align",
				&state->align_x, "Part horizontal align: 0.0 = left  1.0 = right",
				&state->align_y, "Part vertical align: 0.0 = left  1.0 = right");
	evas_object_show(item);

	elm_box_pack_end(box_state, rel2_frame);

	item = ui_prop_view_item_two_edit_double_add(box_state, "aspect",
				&state->aspect_min, "Normally width and height can be"
									" resized to any values independently",
				&state->aspect_max, "Normally width and height can be"
									" resized to any values independently");
	evas_object_show(item);

	item = ui_prop_view_item_color_add(box_state, "color",
				&state->color[0], &state->color[1], &state->color[2], &state->color[3],
				"Part main color.");
	evas_object_show(item);

	return part_view_state;
}

void
ui_prop_part_info_state_set(Evas_Object *part_view, Evas_Object *state_view)
{
	Prop_Part_View_Data *ppvd;
	Evas_Object *box_part;

	if(!part_view || !state_view)
		return;

	ppvd = evas_object_data_get(part_view, PROP_PART_VIEW_DATA);
	if(!ppvd)
		return;

	box_part = elm_object_content_get(part_view);
	if(ppvd->state.state)
		evas_object_del(ppvd->state.state);

	elm_box_pack_end(box_part, state_view);
	ppvd->state.state = state_view;
}

Evas_Object *
ui_prop_part_info_state_view_get(Evas_Object *part_view)
{
	Prop_Part_View_Data *ppvd;

	if(!part_view)
		return NULL;

	ppvd = evas_object_data_get(part_view, PROP_PART_VIEW_DATA);
	if(!ppvd)
		return NULL;

	return ppvd->state.state;
}

#define ITEM_BASE_CREATE(parent, item, name)\
	item = elm_layout_add(parent); \
	elm_layout_file_set(item, TET_EDJ, "property/item/default"); \
	evas_object_size_hint_align_set(item, EVAS_HINT_FILL, EVAS_HINT_FILL); \
	evas_object_size_hint_weight_set(item, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND); \
	elm_object_part_text_set(item, "elm.text", name);

Evas_Object *
ui_prop_view_item_label_add(Evas_Object *prop, const char *name, char *label)
{
	Evas_Object *item, *_label;

	if(!prop || !name)
		return NULL;

	ITEM_BASE_CREATE(prop, item, name)

	_label = elm_label_add(item);
	elm_object_text_set(_label, label);
	elm_label_ellipsis_set(_label, 1.0);
	elm_object_part_content_set(item, "elm.swallow.content", _label);
	evas_object_show(_label);
	elm_box_pack_end(prop, item);

	return item;
}

Evas_Object *
ui_prop_view_item_two_edit_int_add(Evas_Object *prop, const char *name,
									int *data1, const char *tooltip1,
									int *data2, const char *tooltip2)
{
	Evas_Object *item, *content, *edit1, *edit2;
	char buffer [BUFF_MAX];

	if(!prop || !name)
		return NULL;

	ITEM_BASE_CREATE(prop, item, name)

	content = elm_layout_add(item);
	elm_layout_file_set(content, TET_EDJ, "property/item/content/2filed");
	evas_object_size_hint_align_set(item, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(item, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(content);

	edit1 = elm_entry_add(item);
	elm_entry_single_line_set(edit1, EINA_TRUE);
	sprintf(buffer, "%d", *data1);
	elm_object_text_set(edit1, buffer);
	elm_object_tooltip_text_set(edit1, tooltip1);
	elm_object_part_content_set(content, "field1.swallow", edit1);
	evas_object_show(edit1);

	edit2 = elm_entry_add(item);
	elm_entry_single_line_set(edit2, EINA_TRUE);
	sprintf(buffer, "%d", *data2);
	elm_object_text_set(edit2, buffer);
	elm_object_tooltip_text_set(edit2, tooltip2);
	elm_object_part_content_set(content, "field2.swallow", edit2);
	evas_object_show(edit2);

	elm_object_part_content_set(item, "elm.swallow.content", content);

	elm_box_pack_end(prop, item);

	/*TODO: add callback for modify int data */

	return item;
}

Evas_Object *
ui_prop_view_item_one_edit_string_add(Evas_Object *prop, const char *name,
									const char *data, const char *tooltip)
{
	Evas_Object *item, *edit;

	if(!prop || !name)
		return NULL;

	ITEM_BASE_CREATE(prop, item, name)

	edit = elm_entry_add(item);
	elm_entry_single_line_set(edit, EINA_TRUE);
	elm_object_text_set(edit, data);
	elm_object_tooltip_text_set(edit, tooltip);
	evas_object_show(edit);

	elm_object_part_content_set(item, "elm.swallow.content", edit);

	elm_box_pack_end(prop, item);

	/*TODO: add callback for modify data */

	return item;
}

Evas_Object *
ui_prop_view_item_two_edit_string_add(Evas_Object *prop, const char *name,
									const char *data1, const char *tooltip1,
									const char *data2, const char *tooltip2)
{
	Evas_Object *item, *content, *edit1, *edit2;

	if(!prop || !name)
		return NULL;

	ITEM_BASE_CREATE(prop, item, name)

	content = elm_layout_add(item);
	elm_layout_file_set(content, TET_EDJ, "property/item/content/2filed");
	evas_object_size_hint_align_set(item, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(item, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(content);

	edit1 = elm_entry_add(item);
	elm_entry_single_line_set(edit1, EINA_TRUE);
	elm_object_text_set(edit1, data1);
	elm_object_tooltip_text_set(edit1, tooltip1);
	elm_object_part_content_set(content, "field1.swallow", edit1);
	evas_object_show(edit1);

	edit2 = elm_entry_add(item);
	elm_entry_single_line_set(edit2, EINA_TRUE);
	elm_object_text_set(edit2, data2);
	elm_object_tooltip_text_set(edit2, tooltip2);
	elm_object_part_content_set(content, "field2.swallow", edit2);
	evas_object_show(edit2);

	elm_object_part_content_set(item, "elm.swallow.content", content);

	elm_box_pack_end(prop, item);

	/*TODO: add callback for modify data */

	return item;
}

Evas_Object *
ui_prop_view_item_toggle_add(Evas_Object *prop, const char *name,
									Eina_Bool check, const char *tooltip)
{
	Evas_Object *item, *toggle;

	if(!prop || !name)
		return NULL;

	ITEM_BASE_CREATE(prop, item, name)

	toggle = elm_check_add(item);
	elm_object_style_set(toggle, "toggle");
	//evas_object_size_hint_weight_set(toggle, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	//evas_object_size_hint_align_set(toggle, EVAS_HINT_FILL, 0.5);
	elm_object_tooltip_text_set(toggle, tooltip);
	elm_check_state_set(toggle, check);

	elm_object_part_content_set(item, "elm.swallow.content", toggle);

	elm_box_pack_end(prop, item);

	return item;
}

Evas_Object *
ui_prop_view_item_two_edit_double_add(Evas_Object *prop, const char *name,
									double *data1, const char *tooltip1,
									double *data2, const char *tooltip2)
{
	Evas_Object *item, *content, *edit1, *edit2;
	char buffer [BUFF_MAX];

	if(!prop || !name)
		return NULL;

	ITEM_BASE_CREATE(prop, item, name)

	content = elm_layout_add(item);
	elm_layout_file_set(content, TET_EDJ, "property/item/content/2filed");
	evas_object_size_hint_align_set(item, EVAS_HINT_FILL, EVAS_HINT_FILL);
	evas_object_size_hint_weight_set(item, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_show(content);

	edit1 = elm_entry_add(item);
	elm_entry_single_line_set(edit1, EINA_TRUE);
	sprintf(buffer, "%f", *data1);
	elm_object_text_set(edit1, buffer);
	elm_object_tooltip_text_set(edit1, tooltip1);
	elm_object_part_content_set(content, "field1.swallow", edit1);
	evas_object_show(edit1);

	edit2 = elm_entry_add(item);
	elm_entry_single_line_set(edit2, EINA_TRUE);
	sprintf(buffer, "%f", *data2);
	elm_object_text_set(edit2, buffer);
	elm_object_tooltip_text_set(edit2, tooltip2);
	elm_object_part_content_set(content, "field2.swallow", edit2);
	evas_object_show(edit2);

	elm_object_part_content_set(item, "elm.swallow.content", content);

	elm_box_pack_end(prop, item);

	/*TODO: add callback for modify double data */

	return item;
}

Evas_Object *
ui_prop_view_item_color_add(Evas_Object *prop, const char *name,
									int *r, int *g, int *b, int *a,
									const char *tooltip)
{
	Evas_Object *item, *content;

	if(!prop || !name)
		return NULL;

	ITEM_BASE_CREATE(prop, item, name)

	content = evas_object_rectangle_add(evas_object_evas_get(item));
	evas_object_color_set(content, *r, *g, *b, *a);
	elm_object_tooltip_text_set(content, tooltip);
	evas_object_show(content);

	/*TODO: add colorselector */

	elm_object_part_content_set(item, "elm.swallow.content", content);

	elm_box_pack_end(prop, item);

	return item;
}

#undef ITEM_BASE_CREATE

#undef PROP_PART_VIEW_DATA
#undef PROP_VIEW_DATA
