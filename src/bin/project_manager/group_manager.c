/*
 * Edje Theme Editor
 * Copyright (C) 2013-2015 Samsung Electronics.
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

#include "project_manager.h"
#include "widget_list.h"

static void
_group_name_parse(Group *group)
{
   char **c;
   unsigned int count;

   assert(group != NULL);

   if (!eina_str_has_prefix(group->name, "elm")) return;

   c = eina_str_split_full(group->name, "/", 4, &count);

   TODO("move here complicated class/style parsing from live_view");
   if (count == 4)
     {
        group->widget = eina_stringshare_add(c[1]);
        group->class = eina_stringshare_add(c[2]);
        group->style = eina_stringshare_add(c[3]);
     }

   free(c[0]);
   free(c);
}

void
gm_group_edit_object_load(Project *pro, Group *group, Evas *e)
{
   assert(pro != NULL);
   assert(group != NULL);
   assert(group->edit_object == NULL);

   group->edit_object = edje_edit_object_add(e);
   if (!edje_object_mmap_set(group->edit_object, pro->mmap_file, group->name))
     {
        ERR("Can't set mmap object");
        abort();
     }
}

void
gm_group_edit_object_reload(Project *pro, Group *group)
{
   Part *part;
   Eina_List *l;

   assert(pro != NULL);
   assert(group != NULL);
   assert(group->edit_object != NULL);

   if (!edje_object_mmap_set(group->edit_object, pro->mmap_file, group->name))
     {
        ERR("Can't set mmap object");
        abort();
     }

   EINA_LIST_FOREACH(group->parts, l, part)
      edje_edit_part_selected_state_set(group->edit_object,
                                        part->name,
                                        part->current_state->name,
                                        part->current_state->val);
}

void
gm_group_edit_object_unload(Group *group)
{
   assert(group != NULL);
   assert(group->edit_object != NULL);

   evas_object_del(group->edit_object);
   group->edit_object = NULL;
}

Eina_List *
_strings_list_duplicates_del(Eina_List *list)
{
   Eina_List *l, *l2;
   Eina_Stringshare *str, *str2;

   EINA_LIST_FOREACH(list, l, str)
     {
        EINA_LIST_FOREACH(eina_list_next(l), l2, str2)
          {
             if ((str == str2) || !strcmp(str, str2))
               {
                  list = eina_list_remove_list(list, l2);
                  eina_stringshare_del(str2);
               }
          }
     }
   return list;
}

#define SOURCE_GET(FUNC) \
do { \
   source = FUNC(obj, part); \
   if (source) \
     { \
        groups = eina_list_sorted_insert(groups, sort_cb, eina_stringshare_add(source)); \
        eina_stringshare_del(source); \
     } \
   } \
while (0);

Eina_List *
gm_group_used_groups_edj_get(Evas_Object *obj)
{
   Eina_List *groups = NULL;
   Eina_List *parts, *l1;
   Eina_Stringshare *part, *source;
   Edje_Part_Type type;
   int items_count, i;

   parts = edje_edit_parts_list_get(obj);
   EINA_LIST_FOREACH(parts, l1, part)
     {
        type = edje_edit_part_type_get(obj, part);
        if ((EDJE_PART_TYPE_GROUP != type) ||
            (EDJE_PART_TYPE_TEXTBLOCK != type) ||
            (EDJE_PART_TYPE_BOX != type) ||
            (EDJE_PART_TYPE_TABLE != type))
          continue;
        SOURCE_GET(edje_edit_part_source_get)
        SOURCE_GET(edje_edit_part_source2_get)
        SOURCE_GET(edje_edit_part_source3_get)
        SOURCE_GET(edje_edit_part_source4_get)
        SOURCE_GET(edje_edit_part_source5_get)
        SOURCE_GET(edje_edit_part_source6_get)

        items_count = edje_edit_part_items_count_get(obj, part);
        for (i = 0; i < items_count; i++)
          {
             source = edje_edit_part_item_index_source_get(obj, part, i);
             if (source)
               {
                  groups = eina_list_sorted_insert(groups, sort_cb, eina_stringshare_add(source));
                  eina_stringshare_del(source);
               }
          }
     }
   edje_edit_string_list_free(parts);

   return _strings_list_duplicates_del(groups);
}
#undef SOURCE_GET

Eina_List *
gm_group_used_images_edj_get(Evas_Object *obj)
{
   Eina_List *images = NULL;
   Eina_List *parts, *l1, *states, *l2, *tweens;
   Eina_Stringshare *part, *state, *img, *pstate;
   double pvalue;

   parts = edje_edit_parts_list_get(obj);
   EINA_LIST_FOREACH(parts, l1, part)
     {
        if (EDJE_PART_TYPE_IMAGE != edje_edit_part_type_get(obj, part)) continue;
        states = edje_edit_part_states_list_get(obj, part);
        EINA_LIST_FOREACH(states, l2, state)
          {
             state_name_split(state, &pstate, &pvalue);
             img = edje_edit_state_image_get(obj, part, pstate, pvalue);
             images = eina_list_sorted_insert(images, sort_cb, eina_stringshare_add(img));
             eina_stringshare_del(img);
             eina_stringshare_del(pstate);

             tweens = edje_edit_state_tweens_list_get(obj, part, pstate, pvalue);
             images = eina_list_sorted_merge(images, tweens, sort_cb);
          }
     }
   edje_edit_string_list_free(parts);

   return _strings_list_duplicates_del(images);
}

Eina_List *
gm_group_used_sample_edj_get(Evas_Object *obj)
{
   Eina_List *samples = NULL;
   Eina_List *programs, *l1;
   Eina_Stringshare *program, *sample;

   programs = edje_edit_programs_list_get(obj);
   EINA_LIST_FOREACH(programs, l1, program)
     {
        if (EDJE_ACTION_TYPE_SOUND_SAMPLE != edje_edit_program_action_get(obj, program))
          continue;

        sample = edje_edit_program_sample_name_get(obj, program);
        if (sample)
          {
             samples = eina_list_sorted_insert(samples, sort_cb, eina_stringshare_add(sample));
             eina_stringshare_del(sample);
          }
     }
   edje_edit_string_list_free(programs);

   return _strings_list_duplicates_del(samples);
}

Eina_List *
gm_group_used_color_classes_edj_get(Evas_Object *obj)
{
   Eina_List *color_classes = NULL;
   Eina_List *parts, *l1, *states, *l2;
   Eina_Stringshare *part, *state, *color_c, *pstate;
   double pvalue;

   parts = edje_edit_parts_list_get(obj);
   EINA_LIST_FOREACH(parts, l1, part)
     {
        states = edje_edit_part_states_list_get(obj, part);
        EINA_LIST_FOREACH(states, l2, state)
          {
             state_name_split(state, &pstate, &pvalue);
             color_c = edje_edit_state_color_class_get(obj, part, pstate, pvalue);
             if (color_c)
               {
                  color_classes = eina_list_sorted_insert(color_classes, sort_cb, eina_stringshare_add(color_c));
                  eina_stringshare_del(color_c);
               }
             eina_stringshare_del(pstate);
          }
     }
   edje_edit_string_list_free(parts);

   return _strings_list_duplicates_del(color_classes);
}

Eina_List *
gm_group_used_styles_edj_get(Evas_Object *obj)
{
   Eina_List *styles = NULL;
   Eina_List *parts, *l1, *states, *l2;
   Eina_Stringshare *part, *state, *style, *pstate;
   double pvalue;

   parts = edje_edit_parts_list_get(obj);
   EINA_LIST_FOREACH(parts, l1, part)
     {
        states = edje_edit_part_states_list_get(obj, part);
        EINA_LIST_FOREACH(states, l2, state)
          {
             if (EDJE_PART_TYPE_TEXTBLOCK != edje_edit_part_type_get(obj, part)) continue;
             state_name_split(state, &pstate, &pvalue);
             style = edje_edit_state_text_style_get(obj, part, pstate, pvalue);
             if (style)
               {
                  styles = eina_list_sorted_insert(styles, sort_cb, eina_stringshare_add(style));
                  eina_stringshare_del(style);
               }
             eina_stringshare_del(pstate);
          }
     }
   edje_edit_string_list_free(parts);

   return _strings_list_duplicates_del(styles);
}

static Eina_List *
_style_font_get(Evas_Object *obj, const char *style)
{
   Eina_List *fonts = NULL;
   Eina_List *tags, *l;
   Eina_Stringshare *font, *tag_value;
   char *pch;

   tags = edje_edit_style_tags_list_get(obj, style);
   EINA_LIST_FOREACH(tags, l, tag_value)
     {
        pch = strstr(tag_value, "font");
        if (!pch) continue;
        pch += strlen("font");
        pch = strtok(pch, " ");
        font = edje_edit_font_path_get(obj, pch);
        if (font)
          {
             fonts = eina_list_sorted_insert(fonts, sort_cb, font);
             eina_stringshare_del(font);
          }
     }

   return fonts;
}

Eina_List *
gm_group_used_fonts_edj_get(Evas_Object *obj)
{
   Eina_List *fonts = NULL;
   Eina_List *parts, *l1, *states, *l2, *style_fonts;
   Eina_Stringshare *part, *state, *font, *style, *real_font, *pstate;
   double pvalue;
   Edje_Part_Type type;

   parts = edje_edit_parts_list_get(obj);
   EINA_LIST_FOREACH(parts, l1, part)
     {
        type = edje_edit_part_type_get(obj, part);
        states = edje_edit_part_states_list_get(obj, part);
        EINA_LIST_FOREACH(states, l2, state)
          {
             state_name_split(state, &pstate, &pvalue);
             if (EDJE_PART_TYPE_TEXT == type)
               {
                  font = edje_edit_state_font_get(obj, part, pstate, pvalue);
                  if (font)
                    {
                       real_font = edje_edit_font_path_get(obj, font);
                       if (real_font)
                         {
                            fonts = eina_list_sorted_insert(fonts, sort_cb, eina_stringshare_add(real_font));
                            eina_stringshare_del(real_font);
                         }
                       eina_stringshare_del(font);
                    }
               }
             if (EDJE_PART_TYPE_TEXTBLOCK == type)
               {
                  style = edje_edit_state_text_style_get(obj, part, pstate, pvalue);
                  /* sometimes I hate the edje_edit.  */
                  if (style)
                    {
                       style_fonts = _style_font_get(obj, style);
                       if (!style_fonts)
                         {
                            fonts = eina_list_sorted_merge(fonts, style_fonts, sort_cb);
                         }
                       eina_stringshare_del(style);
                       edje_edit_string_list_free(style_fonts);
                    }
               }
             eina_stringshare_del(pstate);
          }
     }
   edje_edit_string_list_free(parts);

   return _strings_list_duplicates_del(fonts);
}

State *
gm_state_add(Project *pro, Part *part, const char *state_name, double state_value)
{
   Resource *resource, request;
   State *state;
   Eina_Stringshare *image_name, *name;
   Eina_List *tween_list, *l;

   assert(pro != NULL);
   assert(part != NULL);
   assert(state_name != NULL);

   state = (State *) resource_add(state_name, RESOURCE_TYPE_STATE);
   state->val = state_value;
   state->part = part;

   resource_insert(&part->states, (Resource *)state);

   /* default 0.0 should be allways first state */
   if (part->current_state == NULL)
     part->current_state = state;

   #define USAGE_ADD(TYPE, LIST, USAGE_TYPE) \
   name = edje_edit_state_ ## TYPE ## _get(part->group->edit_object, \
                                           part->name, \
                                           state->name, \
                                           state->val); \
   if (name) \
     { \
        request.resource_type = USAGE_TYPE; \
        request.name = name; \
        resource = resource_get(LIST, &request); \
        if (resource) \
          resource_used_in_add(resource, (Resource *)state); \
        else \
          ERR("" #TYPE " \"%s\" not found", name); \
        edje_edit_string_free(name); \
     }

   #define COLORCLASS_USAGE_ADD() \
   name = edje_edit_state_color_class_get(part->group->edit_object, \
                                          part->name, \
                                          state->name, \
                                          state->val); \
   if (name) \
     { \
        request.resource_type = RESOURCE_TYPE_COLORCLASS; \
        request.name = name; \
        resource = resource_get(pro->colorclasses, &request); \
        if (!resource) \
          { \
             /* Colorclass can be specified but not defined in edc.
                If colorclass don't exist yet adding it */ \
             TODO("move this code to colorclass resource manager"); \
             edje_edit_color_class_add(pro->global_object, name); \
             Colorclass_Resource *res = (Colorclass_Resource *)resource_add(name, RESOURCE_TYPE_COLORCLASS); \
             res->color1.r = res->color1.g = res->color1.b = res->color1.a = 255; \
             res->color2.r = res->color2.g = res->color2.b = res->color2.a = 255; \
             res->color3.r = res->color3.g = res->color3.b = res->color3.a = 255; \
             resource = (Resource *) res; \
             resource_insert(&pro->colorclasses, resource); \
          } \
        resource_used_in_add(resource, (Resource *)state); \
        edje_edit_string_free(name); \
     }


   switch (part->type)
     {
      case EDJE_PART_TYPE_RECTANGLE:
      case EDJE_PART_TYPE_PROXY:
      case EDJE_PART_TYPE_BOX:
      case EDJE_PART_TYPE_TABLE:
         COLORCLASS_USAGE_ADD();
         break;
      case EDJE_PART_TYPE_IMAGE:
         COLORCLASS_USAGE_ADD();

         USAGE_ADD(image, pro->images, RESOURCE_TYPE_IMAGE);

         tween_list = edje_edit_state_tweens_list_get(part->group->edit_object,
                                                      part->name,
                                                      state->name,
                                                      state->val);
         EINA_LIST_FOREACH(tween_list, l, image_name)
           {
              request.resource_type = RESOURCE_TYPE_IMAGE;
              request.name = image_name;
              resource = resource_get(pro->images, &request);
              resource_used_in_add(resource, (Resource *)state);
           }
         edje_edit_string_list_free(tween_list);

         break;
      case EDJE_PART_TYPE_TEXT:
         COLORCLASS_USAGE_ADD();
         //USAGE_ADD(font, pro->fonts, RESOURCE_TYPE_FONT);
         break;
      case EDJE_PART_TYPE_TEXTBLOCK:
         USAGE_ADD(text_style, pro->styles, RESOURCE_TYPE_STYLE);
         break;
      default:
         break;
     }
   #undef COLORCLASS_USAGE_ADD
   #undef USAGE_ADD
   return state;
}

Part *
gm_part_add(Project *pro, Group *group, const char *part_name)
{
   Resource *resource, request;
   Part_Item *item;
   Part *part;
   Eina_List *states, *items, *l;
   Eina_Stringshare *state_name, *parsed_state_name, *group_name, *item_name;
   double val;

   assert(pro != NULL);
   assert(group != NULL);
   assert(part_name != NULL);

   part = (Part*)resource_add(part_name, RESOURCE_TYPE_PART);
   part->group = group;
   part->type = edje_edit_part_type_get(group->edit_object, part_name);
   part->visible = true;
   resource_insert(&group->parts, (Resource *)part);
   states = edje_edit_part_states_list_get(group->edit_object, part_name);
   EINA_LIST_FOREACH(states, l, state_name)
     {
        state_name_split(state_name, &parsed_state_name, &val);
        gm_state_add(pro, part, parsed_state_name, val);
        eina_stringshare_del(parsed_state_name);
     }
   edje_edit_string_list_free(states);

   if ((part->type == EDJE_PART_TYPE_TABLE) ||
       (part->type == EDJE_PART_TYPE_BOX))
     {
        items = edje_edit_part_items_list_get(group->edit_object, part_name);
        EINA_LIST_FOREACH(items, l, item_name)
          {
             TODO("Remove this after switching to index API for items");
             if (!item_name) continue;
             item = (Part_Item *)resource_add(item_name, RESOURCE_TYPE_ITEM);
             item->part = part;
             resource_insert(&part->items, (Resource *)item);
          }
        edje_edit_string_list_free(items);
     }

   #define GROUP_USAGE_ADD(TYPE) \
   group_name = edje_edit_part_ ## TYPE ## _get(part->group->edit_object, \
                                                part->name); \
   if (group_name) \
     { \
        request.resource_type = RESOURCE_TYPE_GROUP; \
        request.name = group_name; \
        resource = resource_get(pro->groups, &request); \
        resource_used_in_add(resource, (Resource *)part); \
        edje_edit_string_free(group_name); \
     }

   switch (part->type)
     {
      case EDJE_PART_TYPE_BOX:
      case EDJE_PART_TYPE_TABLE:
         EINA_LIST_FOREACH(part->items, l, item)
           {
              group_name = edje_edit_part_item_source_get(group->edit_object,
                                                          part->name,
                                                          item->name);
              if (group_name)
                {
                   request.resource_type = RESOURCE_TYPE_GROUP;
                   request.name = group_name;
                   resource = resource_get(pro->groups, &request);
                   TODO("replace with item");
                   resource_used_in_add(resource, (Resource *)item);
                   edje_edit_string_free(group_name);
                }
           }
         break;
      case EDJE_PART_TYPE_TEXTBLOCK:
         GROUP_USAGE_ADD(source);
         GROUP_USAGE_ADD(source2);
         GROUP_USAGE_ADD(source3);
         GROUP_USAGE_ADD(source4);
         GROUP_USAGE_ADD(source5);
         GROUP_USAGE_ADD(source6);
         break;
      case EDJE_PART_TYPE_GROUP:
         GROUP_USAGE_ADD(source);
         break;
      default:
         break;
     }
   #undef GROUP_USAGE_ADD
   return part;
}

static void
_group_load(Project *pro, Group *group)
{
   Resource request;
   State state_request;
   Eina_Stringshare *main_group_name;
   Eina_List *parts, *l, *lt, *programs, *targets, *datas;
   Eina_Stringshare *part_name, *program_name, *target_name, *state_name,
                    *sample_name, *tone_name, *group_data_name;
   double state_val;
   Part *part;
   Resource *state, *sound, *tone;
   Program *program;

   assert(pro != NULL);
   assert(group != NULL);

   _group_name_parse(group);

   ecore_thread_main_loop_begin();
   gm_group_edit_object_load(pro, group, evas_object_evas_get(pro->global_object));
   if (edje_edit_group_alias_is(group->edit_object, group->name))
     {
        main_group_name = edje_edit_group_aliased_get(group->edit_object, group->name);
        request.resource_type = RESOURCE_TYPE_GROUP;
        request.name = main_group_name;
        group->main_group = (Group *)resource_get(pro->groups, &request);
        resource_insert(&group->main_group->aliases, (Resource *)group);
        edje_edit_string_free(main_group_name);
     }
   else
     {
        parts = edje_edit_parts_list_get(group->edit_object);
        EINA_LIST_FOREACH(parts, l, part_name)
           gm_part_add(pro, group, part_name);
        edje_edit_string_list_free(parts);

        datas = edje_edit_group_data_list_get(group->edit_object);
        EINA_LIST_FOREACH(datas, l, group_data_name)
           gm_group_data_add(pro, group, group_data_name);

        programs = edje_edit_programs_list_get(group->edit_object);
        EINA_LIST_FOREACH(programs, l, program_name)
          {
            program = (Program *)resource_add(program_name, RESOURCE_TYPE_PROGRAM);
            program->type = edje_edit_program_action_get(group->edit_object, program_name);
            resource_insert(&group->programs, (Resource *)program);
          }
        edje_edit_string_list_free(programs);
        EINA_LIST_FOREACH(group->programs, l, program)
          {
             switch (program->type)
               {
                case EDJE_ACTION_TYPE_STATE_SET:
                   targets = edje_edit_program_targets_get(group->edit_object, program->name);
                   state_name = edje_edit_program_state_get(group->edit_object, program->name);
                   state_val = edje_edit_program_value_get(group->edit_object, program->name);
                   state_request.name = state_name;
                   state_request.val = state_val;
                   state_request.resource_type = RESOURCE_TYPE_STATE;
                   eina_stringshare_del(state_name);
                   EINA_LIST_FOREACH(targets, lt, target_name)
                     {
                        request.resource_type = RESOURCE_TYPE_PART;
                        request.name = target_name;
                        part = (Part *) resource_get(group->parts, &request);
                        state = resource_get(part->states, (Resource *)&state_request);
                        if (state)
                          resource_used_in_add(state, (Resource *)program);
                        else
                          {
                             TODO("Handle programs with states that don't exist");
                          }

                     }
                   edje_edit_string_list_free(targets);
                   break;
                case EDJE_ACTION_TYPE_DRAG_VAL_SET:
                case EDJE_ACTION_TYPE_DRAG_VAL_STEP:
                case EDJE_ACTION_TYPE_DRAG_VAL_PAGE:
                   targets = edje_edit_program_targets_get(group->edit_object, program->name);
                   EINA_LIST_FOREACH(targets, lt, target_name)
                     {
                        request.resource_type = RESOURCE_TYPE_PART;
                        request.name = target_name;
                        part = (Part *) resource_get(group->parts, &request);
                        resource_used_in_add((Resource *)part, (Resource *)program);
                     }
                   edje_edit_string_list_free(targets);
                   break;
                case EDJE_ACTION_TYPE_SOUND_SAMPLE:
                   sample_name = edje_edit_program_sample_name_get(group->edit_object, program->name);
                   request.resource_type = RESOURCE_TYPE_SOUND;
                   request.name = sample_name;
                   sound = resource_get(pro->sounds, &request);
                   resource_used_in_add(sound, (Resource *)program);
                   eina_stringshare_del(sample_name);
                   break;
                case EDJE_ACTION_TYPE_SOUND_TONE:
                   tone_name = edje_edit_program_tone_name_get(group->edit_object, program->name);
                   request.resource_type = RESOURCE_TYPE_TONE;
                   request.name = tone_name;
                   tone = resource_get(pro->tones, &request);
                   resource_used_in_add(tone, (Resource *)program);
                   eina_stringshare_del(tone_name);
                   break;
                default:
                   TODO("Handle other action types");
                   break;
               }
          }
     }

   gm_group_edit_object_unload(group);
   ecore_thread_main_loop_end();
}

Group *
gm_group_add(Project *pro, const char *group_name, Eina_Bool emit)
{
   Group *group;

   assert(pro != NULL);
   assert(group_name != NULL);

   group = (Group *)resource_add(group_name, RESOURCE_TYPE_GROUP);
   resource_insert(&pro->groups, (Resource *)group);

   _group_load(pro, group);
   if (emit) evas_object_smart_callback_call(ap.win, SIGNAL_GROUP_ADDED, (void *)group);
   return group;
}

void
gm_group_del(Project *pro, Group *group)
{
   Group *alias;
   Part *part;
   Resource *program;
   Eina_List *l, *ln;

   assert(pro != NULL);
   assert(group != NULL);
   assert(group->edit_object == NULL);

   /* Don't free the list data (Part), because data owners are another groups */
   group->used_in = eina_list_free(group->used_in);
   if (group->main_group)
     group->main_group->aliases = eina_list_remove(group->main_group->aliases, group);
   EINA_LIST_FREE(group->aliases, alias)
     {
        gm_group_del(pro, alias);
     }
   EINA_LIST_FOREACH_SAFE(group->parts, l, ln, part)
     {
        gm_part_del(pro, part);
     }
   EINA_LIST_FREE(group->programs, program)
     {
        assert(program->used_in == NULL);
        resource_free((Resource *)program);
     }
   pro->groups = eina_list_remove(pro->groups, group);
   eina_stringshare_del(group->widget);
   eina_stringshare_del(group->class);
   eina_stringshare_del(group->style);
   evas_object_smart_callback_call(ap.win, SIGNAL_GROUP_DELETED, (void *)group->name);
   /* delete group name after call signal, because the group name need in the
    * callbacks */
   resource_free((Resource *)group);
}

void
gm_groups_load(Project *pro)
{
   Eina_List *collections, *l;
   Eina_Stringshare *group_name;
   Group *group;

   assert(pro != NULL);
   assert(pro->dev != NULL);
   assert(pro->groups == NULL);

   collections = edje_file_collection_list(pro->dev);

   assert(collections != NULL);

   collections = eina_list_sort(collections, eina_list_count(collections), (Eina_Compare_Cb) strcmp);
   EINA_LIST_FOREACH(collections, l, group_name)
     {
        if (!strcmp(group_name, EFLETE_INTERNAL_GROUP_NAME)) continue;

        group = (Group *)resource_add(group_name, RESOURCE_TYPE_GROUP);
        pro->groups = eina_list_append(pro->groups, group);
     }
   edje_file_collection_list_free(collections);

   EINA_LIST_FOREACH(pro->groups, l, group)
     _group_load(pro, group);
}

void
gm_groups_free(Project *pro)
{
   Group *group;
   Resource *program;
   Part *part;
   State *state;
   Resource *item;

   assert(pro != NULL);

   EINA_LIST_FREE(pro->groups, group)
     {
        eina_list_free(group->used_in);
        eina_stringshare_del(group->widget);
        eina_stringshare_del(group->class);
        eina_stringshare_del(group->style);
        eina_list_free(group->aliases);

        EINA_LIST_FREE(group->parts, part)
          {
             eina_list_free(part->used_in);
             EINA_LIST_FREE(part->states, state)
               {
                  eina_list_free(state->used_in);
                  resource_free((Resource *)state);
               }
            EINA_LIST_FREE(part->items, item)
               {
                  eina_list_free(item->used_in);
                  resource_free(item);
               }
            resource_free((Resource *)part);
          }
        EINA_LIST_FREE(group->programs, program)
          {
             eina_list_free(program->used_in);
             resource_free((Resource *)program);
          }
        /* object should be deleted before freeing groups list*/
        assert(group->edit_object == NULL);
        resource_free((Resource *)group);
     }
}

void
gm_state_del(Project *pro, State *state)
{
//   Eina_Stringshare *name, *image_name;
//   Eina_List *tween_list, *l;

   assert(pro != NULL);
   assert(state != NULL);

#define USAGE_DEL(TYPE, USAGE_LIST) \
   name = edje_edit_state_ ## TYPE ## _get(state->part->group->edit_object, \
                                           state->part->name, \
                                           state->name, \
                                           state->val); \
   if (name) \
     { \
        pm_resource_usage_del(USAGE_LIST, name, state); \
        edje_edit_string_free(name); \
     }
TODO("fix usage adding on properties change before using this code")
#if 0
   switch (state->part->type)
     {
      case EDJE_PART_TYPE_RECTANGLE:
      case EDJE_PART_TYPE_PROXY:
      case EDJE_PART_TYPE_BOX:
      case EDJE_PART_TYPE_TABLE:
         USAGE_DEL(color_class, pro->colorclasses);
         break;
      case EDJE_PART_TYPE_IMAGE:
         USAGE_DEL(color_class, pro->colorclasses);

         USAGE_DEL(image, pro->images);

         tween_list = edje_edit_state_tweens_list_get(state->part->group->edit_object,
                                                      state->part->name,
                                                      state->name,
                                                      state->val);
         EINA_LIST_FOREACH(tween_list, l, image_name)
            pm_resource_usage_del(pro->images, image_name, state);
         edje_edit_string_list_free(tween_list);

         break;
      case EDJE_PART_TYPE_TEXT:
         USAGE_DEL(color_class, pro->colorclasses);
         TODO("Uncomment this when font editor would be implemented,"
              "or else it caused eflete to be aborted on part deletion");

         /* USAGE_DEL(font, pro->fonts); */
         break;
      case EDJE_PART_TYPE_TEXTBLOCK:
         USAGE_DEL(text_style, pro->styles);
         break;
      default:
         break;
     }
#endif
   state->used_in = eina_list_free(state->used_in);
   resource_remove(&state->part->states, (Resource *)state);
   resource_free((Resource *)state);

#undef USAGE_DEL
}

void
gm_part_item_add(Project *pro, Part *part, Eina_Stringshare *item_name)
{
   Part_Item *item;

   assert(pro != NULL);
   assert(part != NULL);
   assert(item_name != NULL);
   assert((part->type ==  EDJE_PART_TYPE_BOX) ||
          (part->type ==  EDJE_PART_TYPE_TABLE));

   item = (Part_Item *)resource_add(item_name, RESOURCE_TYPE_ITEM);
   item->part = part;
   resource_insert(&part->items, (Resource *)item);
}

static void
_part_item_free(Project *pro, Part_Item *item)
{
   Resource *group, request;
   Eina_Stringshare *group_name;

   assert(item != NULL);

   group_name = edje_edit_part_item_source_get(item->part->group->edit_object,
                                               item->part->name,
                                               item->name);
   if (group_name)
     {
        request.resource_type = RESOURCE_TYPE_GROUP;
        request.name = group_name;
        group = resource_get(pro->groups, &request);
        resource_used_in_del(group, (Resource *)item);
        edje_edit_string_free(group_name);
     }

   resource_free((Resource *)item);
}
void
gm_part_item_del(Project *pro, Part *part, Eina_Stringshare *item_name)
{
   Resource request;
   Part_Item *item;

   assert(pro != NULL);
   assert(part != NULL);
   assert(item_name != NULL);
   assert((part->type ==  EDJE_PART_TYPE_BOX) ||
          (part->type ==  EDJE_PART_TYPE_TABLE));

   request.resource_type = RESOURCE_TYPE_ITEM;
   request.name = item_name;
   item = (Part_Item *)resource_remove(&part->items, &request);

   _part_item_free(pro, item);
}

void
gm_part_del(Project *pro, Part* part)
{
   Part_Item *item;
   State *state;
   Resource *group, request;
   const char *group_name;
   Eina_List *l;

   assert(pro != NULL);
   assert(part != NULL);

   #define GROUP_USAGE_DEL(TYPE) \
   group_name = edje_edit_part_ ## TYPE ## _get(part->group->edit_object, \
                                                part->name); \
   if (group_name) \
     { \
        request.resource_type = RESOURCE_TYPE_GROUP; \
        request.name = group_name; \
        group = resource_get(pro->groups, &request); \
        resource_used_in_del(group, (Resource *)part); \
        edje_edit_string_free(group_name); \
     }

   switch (part->type)
     {
      case EDJE_PART_TYPE_BOX:
      case EDJE_PART_TYPE_TABLE:
         EINA_LIST_FOREACH(part->items, l, item)
           {
              TODO("clean usage");
           }
         break;
      case EDJE_PART_TYPE_TEXTBLOCK:
         GROUP_USAGE_DEL(source);
         GROUP_USAGE_DEL(source2);
         GROUP_USAGE_DEL(source3);
         GROUP_USAGE_DEL(source4);
         GROUP_USAGE_DEL(source5);
         GROUP_USAGE_DEL(source6);
         break;
      case EDJE_PART_TYPE_GROUP:
         GROUP_USAGE_DEL(source);
         break;
      default:
         break;
     }
   #undef GROUP_USAGE_DEL

   if ((part->type == EDJE_PART_TYPE_TABLE) ||
       (part->type == EDJE_PART_TYPE_BOX))
     {
        EINA_LIST_FREE(part->items, item)
           _part_item_free(pro, item);
     }

   EINA_LIST_FREE(part->states, state)
      gm_state_del(pro, state);

   part->used_in = eina_list_free(part->used_in);
   part->group->parts = eina_list_remove(part->group->parts, part);
   eina_stringshare_del(part->name);
   free(part);
}

void
gm_part_rename(Part* part, const char *new_part_name)
{
   assert(part != NULL);
   assert(new_part_name != NULL);

   eina_stringshare_del(part->name);
   part->name = eina_stringshare_add(new_part_name);
}

void
gm_part_restack(Part *part, Part *rel_part)
{
   Eina_List *rel_l;

   assert(part != NULL);

   part->group->parts = eina_list_remove(part->group->parts, part);

   if (rel_part)
     {
        rel_l = eina_list_data_find_list(part->group->parts, rel_part);
        assert (rel_l != NULL);
        part->group->parts = eina_list_prepend_relative_list(part->group->parts, part, rel_l);
     }
   else
     part->group->parts = eina_list_append(part->group->parts, part);
}

void
gm_part_item_restack(Part_Item *part_item, Part_Item *relative_part_item)
{
   Part *part;

   assert(part_item != NULL);

   part = part_item->part;
   part->items = eina_list_remove(part->items, part_item);
   if (relative_part_item)
     part->items = eina_list_prepend_relative(part->items,
                                              part_item,
                                              relative_part_item);
   else
     part->items = eina_list_append(part->items, part_item);
}

void
gm_program_add(Project *pro, Group *group, Eina_Stringshare *program_name)
{
   Program *program;

   assert(pro != NULL);
   assert(program_name != NULL);
   assert(group != NULL);

   program = (Program *)resource_add(program_name, RESOURCE_TYPE_PROGRAM);
   program->type = edje_edit_program_action_get(group->edit_object, program_name);
   resource_insert(&group->programs, (Resource *)program);
}

void
gm_program_del(Project *pro, Group *group, Eina_Stringshare *program_name)
{
   Resource *program, request;

   assert(pro != NULL);
   assert(program_name != NULL);
   assert(group != NULL);

   request.resource_type = RESOURCE_TYPE_PROGRAM;
   request.name = program_name;
   program = resource_remove(&group->programs, &request);
   resource_free(program);
}

void
gm_group_data_add(Project *pro, Group *group, Eina_Stringshare *group_data_name)
{
   Resource *group_data;

   assert(pro != NULL);
   assert(group_data_name != NULL);
   assert(group != NULL);

   group_data = (Resource *)resource_add(group_data_name, RESOURCE_TYPE_DATA);
   resource_insert(&group->data_items, group_data);
}

void
gm_group_data_del(Project *pro, Group *group, Eina_Stringshare *group_data_name)
{
   Resource *group_data, request;

   assert(pro != NULL);
   assert(group_data_name != NULL);
   assert(group != NULL);

   request.resource_type = RESOURCE_TYPE_DATA;
   request.name = group_data_name;
   group_data = resource_remove(&group->data_items, &request);
   resource_free(group_data);
}

void
gm_group_data_rename(Project *pro, Group *group, Resource* group_data, const char *new_data_name)
{
   assert(pro != NULL);
   assert(group != NULL);
   assert(group_data != NULL);
   assert(new_data_name != NULL);

   resource_remove(&group->data_items, group_data);
   eina_stringshare_del(group_data->name);
   group_data->name = eina_stringshare_add(new_data_name);
   resource_insert(&group->data_items, group_data);
}

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
     "SPACER",
     "MESH NODE",
     "LIGHT",
     "CAMERA",
     "SNAPSHOT"
};
static unsigned int part_types_count = 16;

const char *
gm_part_type_text_get(Edje_Part_Type part_type)
{
   assert(part_type <= part_types_count);

   return part_types[part_type];
}
