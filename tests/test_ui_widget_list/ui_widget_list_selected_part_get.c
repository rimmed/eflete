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


#include "test_ui_widget_list.h"

#define _BEFORE_DOUBLE_CLICK \
   eoi = elm_naviframe_top_item_get(widget_list); \
   glist = elm_object_item_part_content_get(eoi, "elm.swallow.content"); \
   glit = elm_genlist_first_item_get(glist); \
   glit = elm_genlist_item_next_get(glit);

/**
 * @addtogroup ui_widget_list_test
 * @{
 * @addtogroup ui_widget_list_selected_part_get
 * @{
 * ui_widget_list
 * <TABLE>
 * @}
 */

/**
 * @addtogroup ui_widget_list_selected_part_get
 * @{
 * <tr>
 * <td>ui_widget_list_selected_part_get</td>
 * <td>ui_widget_list_selected_part_get_test_p</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 add theme extension "eflete theme".
 * @step 3 create parent window
 * @step 4 open project into Project structure. Use edj file,
 *         wich contain widgets and layout groups.
 * @step 5 create widget list.
 * @step 6 set data into widget list.
 * @step 7 double click on widget in widget list
 * @step 8 double click on class/style in widget list
 * @step 9 Select second part (so it can be moved above to the first place).
 *
 * @procedure
 * @step 1 Call ui_widget_list_selected_part_get
 * @step 2 Check returned value
 * @step 3 Check part's name
 * </td>
 * <td>(Evas_Object *) widget_list</td>
 * <td>All checks passed</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (ui_widget_list_selected_part_get_test_p)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Evas_Object *parent, *widget_list, *glist;
   Project *project = NULL;
   Part *selected_part = NULL;
   Elm_Object_Item *glit, *eoi;
   const char *edj_path = "./edj_build/ui_widget_list_selected_part_get.edj";

   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   project = pm_open_project_edj("UTC", edj_path);
   widget_list = ui_widget_list_add(parent);
   ui_widget_list_data_set(widget_list, project);
   /********Choosing widget, so widget list contain parts********/
   /* Double-click on widget*/
   _BEFORE_DOUBLE_CLICK
   evas_object_smart_callback_call(glist, "clicked,double", glit);
   /* Double-click on style*/
   _BEFORE_DOUBLE_CLICK
   glit = elm_genlist_item_next_get(glit);
   evas_object_smart_callback_call(glist, "clicked,double", glit);
   /* select second part, so it can be move above or below */
   _BEFORE_DOUBLE_CLICK
   elm_genlist_item_selected_set(glit, EINA_TRUE);
   /*************************************************************/

   selected_part = ui_widget_list_selected_part_get(widget_list);
   ck_assert_msg(selected_part != NULL, "Can't return any selected part");
   ck_assert_str_eq(selected_part->name, "radio");

   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_widget_list_selected_part_get
 * @{
 * <tr>
 * <td>ui_widget_list_selected_part_get</td>
 * <td>ui_widget_list_selected_part_get_test_n1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 add theme extension "eflete theme".
 * @step 3 create parent window
 * @step 4 open project into Project structure. Use edj file,
 *         wich contain widgets and layout groups.
 * @step 5 create widget list.
 * @step 6 set data into widget list.
 * @step 7 double click on widget in widget list
 * @step 8 double click on class/style in widget list
 * @step 9 dont select any part.
 *
 * @procedure
 * @step 1 Call ui_widget_list_selected_part_get
 * @step 2 Check returned value
 * </td>
 * <td>(Evas_Object *) widget_list</td>
 * <td>NULL returned</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (ui_widget_list_selected_part_get_test_n1)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Evas_Object *parent, *widget_list, *glist;
   Project *project = NULL;
   Part *selected_part = NULL;
   Elm_Object_Item *glit, *eoi;
   const char *edj_path = "./edj_build/ui_widget_list_selected_part_get.edj";

   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   project = pm_open_project_edj("UTC", edj_path);
   widget_list = ui_widget_list_add(parent);
   ui_widget_list_data_set(widget_list, project);
   /********Choosing widget, so widget list contain parts********/
   /* Double-click on widget*/
   _BEFORE_DOUBLE_CLICK
   evas_object_smart_callback_call(glist, "clicked,double", glit);
   /* Double-click on style*/
   _BEFORE_DOUBLE_CLICK
   glit = elm_genlist_item_next_get(glit);
   evas_object_smart_callback_call(glist, "clicked,double", glit);
   /*************************************************************/

   selected_part = ui_widget_list_selected_part_get(widget_list);
   ck_assert_msg(!selected_part, "Nothing is selected in widget list, but it returned part!");

   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_widget_list_selected_part_get
 * @{
 * <tr>
 * <td>ui_widget_list_selected_part_get</td>
 * <td>ui_widget_list_selected_part_get_test_n2</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 *
 * @procedure
 * @step 1 Call ui_widget_list_selected_part_get
 * @step 2 Check returned value
 * </td>
 * <td>NULL</td>
 * <td>NULL returned</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (ui_widget_list_selected_part_get_test_n2)
{
   elm_init(0, 0);
   Part *selected_part = NULL;

   selected_part = ui_widget_list_selected_part_get(NULL);
   ck_assert_msg(selected_part == NULL, "Nothing is selected in widget list, but it returned part!");

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_widget_list_selected_part_get
 * @{
 * </TABLE>
 * @}
 * @}
 */

#undef _BEFORE_DOUBLE_CLICK
