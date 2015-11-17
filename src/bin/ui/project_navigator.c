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
#define EO_BETA_API
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT

#include "project_navigator.h"
#include "main_window.h"
#include "editor.h"
#include "validator.h"

#define SIG_GROUP_OPEN "group,open"

typedef struct
{
   Evas_Object *layout;
   Evas_Object *genlist;
   Evas_Object *btn_add;
   Evas_Object *btn_del;
   Elm_Genlist_Item_Class *itc_group;
   Elm_Genlist_Item_Class *itc_folder;
} Project_Navigator;

typedef struct
{
   Evas_Object *box;
   Evas_Object *entry;
   Evas_Object *layout_combo;
   Evas_Object *combobox;
   Evas_Object *check;
} Layout_Popup;

static Project_Navigator project_navigator;
static Layout_Popup layout_p;
static Resource_Name_Validator *validator = NULL;

static char *
_group_item_label_get(void *data,
                      Evas_Object *obj __UNUSED__,
                      const char *part __UNUSED__)
{
   const char *pos;
   Group *group = data;

   pos = strrchr(group->name, '/');
   if (pos) return strdup(pos + 1);
   else return strdup(group->name);
}

static char *
_folder_item_label_get(void *data,
                       Evas_Object *obj __UNUSED__,
                       const char *part __UNUSED__)
{
   const char *pos;
   char buf[BUFF_MAX];
   int symbols;
   Eina_Stringshare *prefix = data;

   symbols = strlen(prefix) - 1;
   strncpy(buf, prefix, symbols);
   buf[symbols] = '\0';
   pos = strrchr(buf, '/');
   if (pos) return strdup(pos + 1);
   else return strdup(buf);
}

static Evas_Object *
_folder_item_icon_get(void *data __UNUSED__,
                      Evas_Object *obj,
                      const char *part)
{
   Evas_Object *icon = NULL;

   if (!strcmp(part, "elm.swallow.icon"))
     {
        ICON_STANDARD_ADD(obj, icon, true, "folder");
     }

   return icon;
}

static Evas_Object *
_group_item_icon_get(void *data,
                     Evas_Object *obj,
                     const char *part)
{
   Evas_Object *icon = NULL;
   Group *group = data;

   if (!strcmp(part, "elm.swallow.icon"))
     {
        ICON_STANDARD_ADD(obj, icon, true, "file");
     }
   if ((group->main_group != NULL) && (!strcmp(part, "elm.swallow.end")))
     {
        IMAGE_ADD_NEW(obj, icon, "icon", "alias_link");
     }
   return icon;
}

static void
_folder_item_del(void *data,
                 Evas_Object *obj __UNUSED__)
{
   Eina_Stringshare *prefix = data;
   eina_stringshare_del(prefix);
}

static Eina_Stringshare *
_get_prefix(const char *group_name, int level, int *symbols)
{
   const char *pos;
   char prefix[BUFF_MAX];
   int i, len;

   assert(group_name != NULL);
   assert(level >= 0);

   pos = group_name - 1;
   for (i = 0; (pos) && (i <= level); i++)
     {
        pos = strchr(pos + 1, '/');
     }
   if (!pos) return NULL;

   len = pos - group_name + 1;
   strncpy(prefix, group_name, len);
   prefix[len] = '\0';
   if (symbols) *symbols = len;

   return eina_stringshare_add(prefix);
}

static void
_tree_items_get(const char *prefix,
                Eina_List **folders_out,
                Eina_List **groups_out)
{
   int cmp;
   int level = 0;
   const char *pos;
   Group *group, *group_next;
   size_t prefix_len;
   Eina_Stringshare *group_prefix;
   int group_prefix_len;
   Eina_List *l, *lnext;

   assert(prefix != NULL);
   assert(folders_out != NULL);
   assert(groups_out != NULL);

   prefix_len = strlen(prefix);

   pos = prefix - 1;
   while ((pos = strchr(pos + 1, '/')))
     level++;

   EINA_LIST_FOREACH(ap.project->groups, l, group)
     {
        cmp = strncmp(group->name, prefix, prefix_len);
        /* skipping all groups with different prefix */
        if (cmp < 0)
          {
             continue;
          }
        if (cmp > 0)
          {
             if (strlen(group->name) < prefix_len)
               continue;
             else
               break; /* there is no sense to check all next groups because list is sorted */
          }

        group_prefix = _get_prefix(group->name, level, &group_prefix_len);
        if (group_prefix)
          {
             *folders_out= eina_list_append(*folders_out, group_prefix);

             /* skipping other groups from the same "folder" */
             lnext = l;
             while ((lnext = eina_list_next(lnext)))
               {
                  group_next = eina_list_data_get(lnext);
                  if ((group_next) && (!strncmp(group_next->name, group_prefix, group_prefix_len)))
                    l = lnext;
                  else
                    break;
               }
          }
        else
          {
             *groups_out = eina_list_append(*groups_out, group);
          }
     }
}

static void
_expand_request_cb(void *data __UNUSED__,
                   Evas_Object *o __UNUSED__,
                   void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_TRUE);
}

static void
_contract_request_cb(void *data __UNUSED__,
                     Evas_Object *o __UNUSED__,
                     void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_FALSE);
}

static void
_expanded_cb(void *data __UNUSED__,
             Evas_Object *o __UNUSED__,
             void *event_info)
{
   Group *group;
   Eina_List *folders = NULL, *groups = NULL;
   Elm_Object_Item *glit = event_info;
   Eina_Stringshare *prefix = elm_object_item_data_get(glit);

   _tree_items_get(prefix, &folders, &groups);
   EINA_LIST_FREE(folders, prefix)
     {
        elm_genlist_item_append(project_navigator.genlist,
                                project_navigator.itc_folder,
                                prefix,
                                glit,
                                ELM_GENLIST_ITEM_TREE,
                                NULL,
                                NULL);
     }
   EINA_LIST_FREE(groups, group)
     {
        elm_genlist_item_append(project_navigator.genlist,
                                project_navigator.itc_group,
                                group,
                                glit,
                                ELM_GENLIST_ITEM_NONE,
                                NULL,
                                NULL);
     }
}

static void
_contracted_cb(void *data __UNUSED__,
               Evas_Object *o __UNUSED__,
               void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
_on_clicked_double(void *data __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;

   if (elm_genlist_item_type_get(glit) == ELM_GENLIST_ITEM_TREE)
     {
        elm_genlist_item_expanded_set(glit, !elm_genlist_item_expanded_get(glit));
     }
   else
     {
        Group *group = (Group *)elm_object_item_data_get(glit);
        evas_object_smart_callback_call(project_navigator.layout, SIG_GROUP_OPEN, group);
     }
}

static Elm_Object_Item *
_find_item(Elm_Object_Item *item, const char *name)
{
   while (item)
     {
        if (!strcmp(elm_object_item_text_get(item), name)) break;
        item = elm_genlist_item_next_get(item);
     }
   return item;
}

static int
_item_group_compare(const void *data1, const void *data2)
{
   const Elm_Object_Item *it1 = data1;
   const Elm_Object_Item *it2 = data2;
   if (elm_genlist_item_item_class_get(it1) == project_navigator.itc_folder) return -1;
   if (elm_genlist_item_item_class_get(it2) == project_navigator.itc_folder) return -1;
   const char *str1 = ((Group *)elm_object_item_data_get(it1))->name;
   const char *str2 = ((Group *)elm_object_item_data_get(it2))->name;

   if (!str1) return 1;
   if (!str2) return -1;

   return strcmp(str1, str2);
}

static int
_item_prefix_compare(const void *data1, const void *data2)
{
   const Elm_Object_Item *it1 = data1;
   const Elm_Object_Item *it2 = data2;
   if (elm_genlist_item_item_class_get(it1) != project_navigator.itc_folder) return 1;
   if (elm_genlist_item_item_class_get(it2) != project_navigator.itc_folder) return 1;

   const char *str1 = elm_object_item_data_get(it1);
   const char *str2 = elm_object_item_data_get(it2);

   if (!str1) return 1;
   if (!str2) return -1;

   return strcmp(str1, str2);
}

static void
_group_add(void *data __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void *event_info)
{
   Group *group;
   Elm_Object_Item *item, *parent = NULL;
   char **arr;
   unsigned int count, i;
   Eina_Stringshare *prefix;

   group = (Group *)event_info;
   item = elm_genlist_first_item_get(project_navigator.genlist);
   arr = eina_str_split_full(group->name, "/", 0, &count);

   for (i = 0; i < count - 1; i++)
     {
        parent = elm_genlist_item_parent_get(item);
        item = _find_item(item, arr[i]);
        if (!item) break;
        if (elm_genlist_item_item_class_get(item) != project_navigator.itc_folder) break;
        if (!elm_genlist_item_expanded_get(item)) goto exit;
        item = eina_list_data_get(elm_genlist_item_subitems_get(item));
     }

   if (i != count - 1)
     {
        prefix = _get_prefix(group->name, i, NULL);
        elm_genlist_item_sorted_insert(project_navigator.genlist,
                                       project_navigator.itc_folder,
                                       prefix,
                                       parent,
                                       ELM_GENLIST_ITEM_TREE,
                                       _item_prefix_compare,
                                       NULL,
                                       NULL);
     }
   else
     elm_genlist_item_sorted_insert(project_navigator.genlist,
                                    project_navigator.itc_group,
                                    group,
                                    parent,
                                    ELM_GENLIST_ITEM_NONE,
                                    _item_group_compare,
                                    NULL,
                                    NULL);

exit:
   free(arr[0]);
   free(arr);
}

static void
_alias_ch(void *data __UNUSED__,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   if (elm_check_state_get(layout_p.check))
     elm_layout_text_set(layout_p.layout_combo, NULL, _("alias of"));
   else
     elm_layout_text_set(layout_p.layout_combo, NULL, _("copy of"));
}

static void
_group_sel(void *data __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void *event_info)
{
    Ewe_Combobox_Item *item = (Ewe_Combobox_Item *)event_info;
    elm_object_disabled_set(layout_p.check, (item->index != 0) ? false : true);
}

static void
_group_validate(void *data __UNUSED__,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   if (resource_name_validator_status_get(validator) != ELM_REG_NOERROR)
     popup_buttons_disabled_set(BTN_OK, true);
   else
     popup_buttons_disabled_set(BTN_OK, false);
}

static void
_btn_add_group_cb(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   Evas_Object *item;
   Popup_Button btn_res;
   Group *group;
   Eina_List *l;
   Elm_Object_Item *glit;

   assert(validator == NULL);

   BOX_ADD(ap.win, layout_p.box, false, false)
   /* name: entry */
   validator = resource_name_validator_new(LAYOUT_NAME_REGEX, NULL);
   resource_name_validator_list_set(validator, &ap.project->groups, false);
   LAYOUT_PROP_ADD(layout_p.box, _("name"), "property", "1swallow")
   ENTRY_ADD(layout_p.box, layout_p.entry, true)
   evas_object_smart_callback_add(layout_p.entry, "changed", _group_validate, NULL);
   eo_do(layout_p.entry, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, resource_name_validator_helper, validator));
   elm_layout_content_set(item, NULL, layout_p.entry);
   elm_box_pack_end(layout_p.box, item);
   glit = elm_genlist_selected_item_get(project_navigator.genlist);
   if (glit && (elm_genlist_item_item_class_get(item) == project_navigator.itc_folder))
     elm_entry_entry_set(layout_p.entry, elm_object_item_data_get(glit));
   /* copy: combobox */
   LAYOUT_PROP_ADD(layout_p.box, _("copy of"), "property", "1swallow")
   layout_p.layout_combo = item;
   EWE_COMBOBOX_ADD(item, layout_p.combobox)
   evas_object_smart_callback_add(layout_p.combobox, "selected", _group_sel, NULL);
   elm_layout_content_set(item, NULL, layout_p.combobox);
   elm_box_pack_end(layout_p.box, item);
   /* alias: check */
   LAYOUT_PROP_ADD(layout_p.box, _("alias"), "property", "1swallow")
   CHECK_ADD(item, layout_p.check)
   evas_object_smart_callback_add(layout_p.check, "changed", _alias_ch, NULL);
   elm_object_disabled_set(layout_p.check, true);
   elm_layout_content_set(item, NULL, layout_p.check);
   elm_box_pack_end(layout_p.box, item);

   /* fill the combobox */
   ewe_combobox_item_add(layout_p.combobox, _("None"));
   EINA_LIST_FOREACH(ap.project->groups, l, group)
     {
        ewe_combobox_item_add(layout_p.combobox, group->name);
     }
   ewe_combobox_select_item_set(layout_p.combobox, 0);

   btn_res = popup_want_action(_("Create a new layout"), NULL, layout_p.box,
                               layout_p.entry, BTN_OK|BTN_CANCEL,
                               NULL, layout_p.entry);
   if (BTN_CANCEL == btn_res) goto close;

   Ewe_Combobox_Item *combo_it;
   combo_it = ewe_combobox_select_item_get(layout_p.combobox);
   if (combo_it->index  == 0)
     editor_group_add(ap.project->global_object, elm_entry_entry_get(layout_p.entry));
   else
     {
        if (!elm_check_state_get(layout_p.check))
          editor_group_copy(ap.project->global_object, combo_it->title, elm_entry_entry_get(layout_p.entry));
        else
          editor_group_alias_add(ap.project->global_object, combo_it->title, elm_entry_entry_get(layout_p.entry));
     }
   gm_group_add(ap.project, elm_entry_entry_get(layout_p.entry));

close:
   evas_object_del(layout_p.box);
   resource_name_validator_free(validator);
   validator = NULL;
}

static void
_folder_del(const char *prefix)
{
   Eina_List *folders = NULL, *groups = NULL;
   Eina_Stringshare *tmp;
   Group *group;

   _tree_items_get(prefix, &folders, &groups);
   EINA_LIST_FREE(folders, tmp)
     {
       _folder_del(tmp);
     }
   EINA_LIST_FREE(groups, group)
     {
       tmp = eina_stringshare_add(group->name);
       gm_group_del(ap.project, group);
       editor_group_del(ap.project->global_object, tmp);
       eina_stringshare_del(tmp);
     }
}

static int
group_cmp(Resource *res1, const char *name)
{
   return strncmp(res1->name, name, strlen(name));
}

static void
_group_del(void *data __UNUSED__,
           Evas_Object *obj __UNUSED__,
           void *event_info)
{
   Eina_Stringshare *group_name;
   Elm_Object_Item *item;
   char **arr;
   unsigned int depth, i;
   Eina_List *stack = NULL, *l;
   Eina_Strbuf *buf;

   buf = eina_strbuf_new();
   group_name = (Eina_Stringshare *)event_info;
   fprintf(stdout, "%s\n", group_name);
   item = elm_genlist_first_item_get(project_navigator.genlist);
   arr = eina_str_split_full(group_name, "/", 0, &depth);
   for (i = 0; i < depth; i++)
     {
        item  =_find_item(item, arr[i]);
        eina_strbuf_append_printf(buf, "%s", arr[i]);
        if (i != depth - 1) eina_strbuf_append(buf, "/");
        if (!item) break;
        if (!elm_genlist_item_expanded_get(item) &&
            NULL != eina_list_search_sorted_list(ap.project->groups, (Eina_Compare_Cb)group_cmp, eina_strbuf_string_get(buf)))
          break;
        stack = eina_list_append(stack, item);
        item = eina_list_data_get(elm_genlist_item_subitems_get(item));
     }
   EINA_LIST_REVERSE_FOREACH(stack, l, item)
     {
        if (0 == elm_genlist_item_subitems_count(item))
          elm_object_item_del(item);
     }

   eina_strbuf_free(buf);
   free(arr[0]);
   free(arr);
}

static void
_btn_del_group_cb(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   Popup_Button btn_res;
   Group *group;
   Elm_Object_Item *glit;
   Eina_Stringshare *tmp;

   glit = elm_genlist_selected_item_get(project_navigator.genlist);
   if (elm_genlist_item_type_get(glit) == ELM_GENLIST_ITEM_TREE)
     {
        btn_res = popup_want_action(_("Confirm delete layouts"),
                                    _("Are you sure you want to delete the selected layouts?<br>"
                                      "All aliases will be delete too."),
                                    NULL, NULL, BTN_OK|BTN_CANCEL, NULL, NULL);
        if (BTN_CANCEL == btn_res) return;
        _folder_del(elm_object_item_data_get(glit));
        return;
     }
   else
     {
        group = (Group *)elm_object_item_data_get(glit);
        /* if Group have an object then we cann't close tab with this object
         * and delete it */
        if (group->edit_object)
          {
             popup_want_action(_("Warning: Delete layout"),
                               _("Cann't delete the opened layout. Please, "
                                 "close the layout tab before delete it."),
                               NULL, NULL, BTN_CANCEL, NULL, NULL);
             return;
          }
        btn_res = popup_want_action(_("Confirm delete layout"),
                                    _("Are you sure you want to delete the selected layout?<br>"
                                      "All aliases will be delete too."),
                                    NULL, NULL, BTN_OK|BTN_CANCEL, NULL, NULL);
        if (BTN_CANCEL == btn_res) return;
        tmp = eina_stringshare_add(group->name);
        gm_group_del(ap.project, group);
        editor_group_del(ap.project->global_object, tmp);
        eina_stringshare_del(tmp);
     }
}

static void
_selected_cb(void *data __UNUSED__,
             Evas_Object *obj __UNUSED__,
             void *event_info __UNUSED__)
{
   elm_object_disabled_set(project_navigator.btn_del, false);
}

static void
_unselected_cb(void *data __UNUSED__,
               Evas_Object *obj __UNUSED__,
               void *event_info __UNUSED__)
{
   elm_object_disabled_set(project_navigator.btn_del, true);
}

Evas_Object *
project_navigator_add(void)
{
   Evas_Object *icon;

   assert(ap.win != NULL);

   project_navigator.itc_folder = elm_genlist_item_class_new();
   project_navigator.itc_folder->item_style = "navigator";
   project_navigator.itc_folder->func.text_get = _folder_item_label_get;
   project_navigator.itc_folder->func.content_get = _folder_item_icon_get;
   project_navigator.itc_folder->func.state_get = NULL;
   project_navigator.itc_folder->func.del = _folder_item_del;

   project_navigator.itc_group = elm_genlist_item_class_new();
   project_navigator.itc_group->item_style = "navigator";
   project_navigator.itc_group->func.text_get = _group_item_label_get;
   project_navigator.itc_group->func.content_get = _group_item_icon_get;
   project_navigator.itc_group->func.state_get = NULL;
   project_navigator.itc_group->func.del = NULL;

   project_navigator.layout = elm_layout_add(ap.win);
   elm_layout_theme_set(project_navigator.layout, "layout", "navigator", "default");
   evas_object_show(project_navigator.layout);

   project_navigator.btn_add = elm_button_add(project_navigator.layout);
   ICON_STANDARD_ADD(project_navigator.btn_add, icon, true, "plus");
   elm_object_part_content_set(project_navigator.btn_add, NULL, icon);
   evas_object_smart_callback_add(project_navigator.btn_add, "clicked", _btn_add_group_cb, NULL);
   elm_object_style_set(project_navigator.btn_add, "anchor");
   elm_object_part_content_set(project_navigator.layout, "elm.swallow.btn1", project_navigator.btn_add);

   project_navigator.btn_del = elm_button_add(project_navigator.layout);
   ICON_STANDARD_ADD(project_navigator.btn_del, icon, true, "minus");
   elm_object_part_content_set(project_navigator.btn_del, NULL, icon);
   evas_object_smart_callback_add (project_navigator.btn_del, "clicked", _btn_del_group_cb, NULL);
   elm_object_style_set(project_navigator.btn_del, "anchor");
   elm_object_part_content_set(project_navigator.layout, "elm.swallow.btn0", project_navigator.btn_del);
   elm_object_disabled_set(project_navigator.btn_del, true);

   project_navigator.genlist = elm_genlist_add(project_navigator.layout);
   evas_object_show(project_navigator.genlist);
   elm_object_content_set(project_navigator.layout, project_navigator.genlist);
   evas_object_smart_callback_add (project_navigator.genlist, "selected", _selected_cb, NULL);
   evas_object_smart_callback_add (project_navigator.genlist, "unselected", _unselected_cb, NULL);

   elm_object_text_set(project_navigator.layout, _("None"));
   elm_object_disabled_set(project_navigator.layout, true);

   evas_object_smart_callback_add(project_navigator.genlist, "clicked,double", _on_clicked_double, NULL);
   evas_object_smart_callback_add(project_navigator.genlist, "expand,request", _expand_request_cb, NULL);
   evas_object_smart_callback_add(project_navigator.genlist, "contract,request", _contract_request_cb, NULL);
   evas_object_smart_callback_add(project_navigator.genlist, "expanded", _expanded_cb, NULL);
   evas_object_smart_callback_add(project_navigator.genlist, "contracted", _contracted_cb, NULL);

   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_ADDED, _group_add, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_DELETED, _group_del, NULL);

   TODO("Add deletion callback and free resources");

   return project_navigator.layout;
}

void
project_navigator_project_set(void)
{
   Eina_List *folders = NULL, *groups = NULL;
   Eina_Stringshare *prefix;
   Group *group;

   elm_object_text_set(project_navigator.layout, ap.project->name);
   _tree_items_get("", &folders, &groups);

   EINA_LIST_FREE(folders, prefix)
     {
        elm_genlist_item_append(project_navigator.genlist,
                                project_navigator.itc_folder,
                                prefix,
                                NULL,
                                ELM_GENLIST_ITEM_TREE,
                                NULL,
                                NULL);
     }
   EINA_LIST_FREE(groups, group)
     {
        elm_genlist_item_append(project_navigator.genlist,
                                project_navigator.itc_group,
                                group,
                                NULL,
                                ELM_GENLIST_ITEM_NONE,
                                NULL,
                                NULL);
     }

   elm_object_disabled_set(project_navigator.layout, false);
}

void
project_navigator_project_unset(void)
{
   elm_object_text_set(project_navigator.layout, _("None"));
   elm_genlist_clear(project_navigator.genlist);
   elm_object_disabled_set(project_navigator.layout, true);
}
