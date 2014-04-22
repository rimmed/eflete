/**
 * Edje Theme Editor
 * Copyright (C) 2013-2014 Samsung Electronics.
 *
 * This file is part of Edje Theme Editor.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see www.gnu.org/licenses/gpl-2.0.html.
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
 * @addtogroup ui_widget_list_selected_part_above
 * @{
 * ui_widget_list
 * <TABLE>
 * @}
 */

/**
 * @addtogroup ui_widget_list_selected_part_above
 * @{
 * <tr>
 * <td>ui_widget_list_selected_part_above</td>
 * <td>ui_widget_list_selected_part_above_test_p</td>
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
 * @step 8 Get currently selected Style structure
 * @step 9 double click on class/style in widget list
 * @step 10 Select second part (so it can be moved above to the first place).
 *
 * @procedure
 * @step 1 Call ui_widget_list_selected_part_above
 * @step 2 Check returned value
 * </td>
 * <td>(Evas_Object *) widget_list, (Style *) style</td>
 * <td>EINA_TRUE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (ui_widget_list_selected_part_above_test_p)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Evas_Object *parent, *widget_list, *glist;
   Project *project = NULL;
   Eina_Bool result = EINA_FALSE;
   Elm_Object_Item *glit, *eoi;
   const char *edj_path = "./edj_build/ui_widget_list_selected_part_above.edj";

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
   Style *_style = elm_object_item_data_get(glit);
   evas_object_smart_callback_call(glist, "clicked,double", glit);
   /* select second part, so it can be move above or below */
   _BEFORE_DOUBLE_CLICK
   elm_genlist_item_selected_set(glit, EINA_TRUE);
   /*************************************************************/

   result = ui_widget_list_selected_part_above(widget_list, _style);
   ck_assert_msg(result, "Failed to move part above");

   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_widget_list_selected_part_above
 * @{
 * <tr>
 * <td>ui_widget_list_selected_part_above</td>
 * <td>ui_widget_list_selected_part_above_test_n1</td>
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
 * @step 8 Get currently selected Style structure
 * @step 9 double click on class/style in widget list
 * @step 10 Do not select any parts (unselected all parts in widget_list).
 *
 * @procedure
 * @step 1 Call ui_widget_list_selected_part_above
 * @step 2 Check returned value
 * </td>
 * <td>(Evas_Object *) widget_list, (Style *) style</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (ui_widget_list_selected_part_above_test_n1)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Evas_Object *parent, *widget_list, *glist;
   Project *project = NULL;
   Eina_Bool result = EINA_FALSE;
   Elm_Object_Item *glit, *eoi;
   const char *edj_path = "./edj_build/ui_widget_list_selected_part_above.edj";

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
   Style *_style = elm_object_item_data_get(glit);
   evas_object_smart_callback_call(glist, "clicked,double", glit);
   /*************************************************************/

   result = ui_widget_list_selected_part_above(widget_list, _style);
   ck_assert_msg(!result, "Something was moved above");

   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_widget_list_selected_part_above
 * @{
 * <tr>
 * <td>ui_widget_list_selected_part_above</td>
 * <td>ui_widget_list_selected_part_above_test_n2</td>
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
 * @step 8 Get currently selected Style structure
 * @step 9 double click on class/style in widget list
 * @step 10 Select first part.
 *
 * @procedure
 * @step 1 Call ui_widget_list_selected_part_above
 * @step 2 Check returned value
 * </td>
 * <td>(Evas_Object *) widget_list, (Style *) style</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (ui_widget_list_selected_part_above_test_n2)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Evas_Object *parent, *widget_list, *glist;
   Project *project = NULL;
   Eina_Bool result = EINA_FALSE;
   Elm_Object_Item *glit, *eoi;
   const char *edj_path = "./edj_build/ui_widget_list_selected_part_above.edj";

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
   Style *_style = elm_object_item_data_get(glit);
   evas_object_smart_callback_call(glist, "clicked,double", glit);
   /* select first part, so it can't be moved above */
   eoi = elm_naviframe_top_item_get(widget_list);
   glist = elm_object_item_part_content_get(eoi, "elm.swallow.content");
   glit = elm_genlist_first_item_get(glist);
   elm_genlist_item_selected_set(glit, EINA_TRUE);
   /*************************************************************/

   result = ui_widget_list_selected_part_above(widget_list, _style);
   ck_assert_msg(!result, "Something was moved above");

   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_widget_list_selected_part_above
 * @{
 * <tr>
 * <td>ui_widget_list_selected_part_above</td>
 * <td>ui_widget_list_selected_part_above_test_n3</td>
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
 * @step 9 Select first part.
 *
 * @procedure
 * @step 1 Call ui_widget_list_selected_part_above
 * @step 2 Check returned value
 * </td>
 * <td>(Evas_Object *) widget_list, NULL</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (ui_widget_list_selected_part_above_test_n3)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Evas_Object *parent, *widget_list, *glist;
   Project *project = NULL;
   Eina_Bool result = EINA_FALSE;
   Elm_Object_Item *glit, *eoi;
   const char *edj_path = "./edj_build/ui_widget_list_selected_part_above.edj";

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
   /* select first part, so it can't be moved above */
   eoi = elm_naviframe_top_item_get(widget_list);
   glist = elm_object_item_part_content_get(eoi, "elm.swallow.content");
   glit = elm_genlist_first_item_get(glist);
   elm_genlist_item_selected_set(glit, EINA_TRUE);
   /*************************************************************/

   result = ui_widget_list_selected_part_above(widget_list, NULL);
   ck_assert_msg(!result, "Something was moved above");

   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_widget_list_selected_part_above
 * @{
 * <tr>
 * <td>ui_widget_list_selected_part_above</td>
 * <td>ui_widget_list_selected_part_above_test_n4</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 *
 * @procedure
 * @step 1 Call ui_widget_list_selected_part_above
 * @step 2 Check returned value
 * </td>
 * <td>NULL, NULL</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (ui_widget_list_selected_part_above_test_n4)
{
   elm_init(0, 0);
   Eina_Bool result = EINA_FALSE;

   result = ui_widget_list_selected_part_above(NULL, NULL);
   ck_assert_msg(!result, "Something was moved above");

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup ui_widget_list_selected_part_above
 * @{
 * </TABLE>
 * @}
 * @}
 */

#undef _BEFORE_DOUBLE_CLICK
