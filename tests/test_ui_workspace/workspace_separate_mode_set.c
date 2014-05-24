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

#include "test_ui_workspace.h"

/**
 * @addtogroup ui_workspace_test
 * @{
 * @addtogroup workspace_separate_mode_set
 * @{
 * ui_workspace
 * <TABLE>
 * @}
 */

/**
 * @addtogroup workspace_separate_mode_set
 * @{
 * <tr>
 * <td>workspace_separate_mode_set</td>
 * <td>workspace_separate_mode_set_test_p1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extenstion theme from EFLETE_THEME file
 * @step 3 create parent window
 * @step 4 create workspace object
 * @step 5 create style object
 * @step 6 set edit object into workspace
 *
 * @procedure
 * @step 1 call workspace_separate_mode_set
 * @step 2 check returned value
 * </td>
 * <td>(Evas_Object *) workspace, EINA_TRUE</td>
 * <td>Returned EINA_TRUE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(workspace_separate_mode_set_test_p1)
{
   Eina_Bool res = EINA_FALSE;
   Evas_Object *parent, *workspace;
   Style *style = NULL;
   Evas *e = NULL;

   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   workspace = workspace_add(parent);
   e = evas_object_evas_get(parent);
   style = wm_style_add("test", "elm/radio/base/test", STYLE);
   style->obj = edje_edit_object_add(e);
   edje_object_file_set(style->obj, "./edj_build/workspace_separate_mode_set.edj",
                        style->full_group_name);
   workspace_edit_object_set(workspace, style,
                             "./edj_build/workspace_separate_mode_set.edj");

   res = workspace_separate_mode_set(workspace, EINA_TRUE);
   ck_assert_msg(res == EINA_TRUE, "Failed set separate mode for workspace");

   wm_style_free(style);
   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_separate_mode_set
 * @{
 * <tr>
 * <td>workspace_separate_mode_set</td>
 * <td>workspace_separate_mode_set_test_p2</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extenstion theme from EFLETE_THEME file
 * @step 3 create parent window
 * @step 4 create workspace object
 * @step 5 create style object
 * @step 6 set edit object into workspace
 *
 * @procedure
 * @step 1 call workspace_separate_mode_set(EINA_TRUE)
 * @step 2 call workspace_separate_mode_set(EINA_FALSE)
 * @step 3 check returned value
 * </td>
 * <td>(Evas_Object *) workspace, EINA_TRUE</td>
 * <td>Returned EINA_TRUE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(workspace_separate_mode_set_test_p2)
{
   Eina_Bool res = EINA_FALSE;
   Evas_Object *parent, *workspace;
   Style *style = NULL;
   Evas *e = NULL;

   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   workspace = workspace_add(parent);
   e = evas_object_evas_get(parent);
   style = wm_style_add("test", "elm/radio/base/test", STYLE);
   style->obj = edje_edit_object_add(e);
   edje_object_file_set(style->obj, "./edj_build/workspace_separate_mode_set.edj",
                        style->full_group_name);
   workspace_edit_object_set(workspace, style,
                             "./edj_build/workspace_separate_mode_set.edj");

   workspace_separate_mode_set(workspace, EINA_TRUE);
   res = workspace_separate_mode_set(workspace, EINA_FALSE);
   ck_assert_msg(res == EINA_TRUE, "Failed set separate mode for workspace");

   wm_style_free(style);
   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_separate_mode_set
 * @{
 * <tr>
 * <td>workspace_separate_mode_set</td>
 * <td>workspace_separate_mode_set_test_n1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 *
 * @procedure
 * @step 1 call workspace_separate_mode_set(EINA_TRUE)
 * @step 2 check returned value
 * </td>
 * <td>NULL, EINA_TRUE</td>
 * <td>Returned EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(workspace_separate_mode_set_test_n1)
{
   Eina_Bool res;

   elm_init(0, 0);

   res = workspace_separate_mode_set(NULL, EINA_TRUE);
   ck_assert_msg(res == EINA_FALSE, "Failed set separate mode for workspace");

   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_separate_mode_set
 * @{
 * <tr>
 * <td>workspace_separate_mode_set</td>
 * <td>workspace_separate_mode_set_test_n2</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extenstion theme from EFLETE_THEME file
 * @step 3 create parent window
 * @step 4 create workspace object
 *
 * @procedure
 * @step 1 call workspace_separate_mode_set(EINA_TRUE)
 * @step 2 check returned value
 * </td>
 * <td>(Evas_Object *) workspace, EINA_TRUE</td>
 * <td>Returned EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(workspace_separate_mode_set_test_n2)
{
   Eina_Bool res;
   Evas_Object *parent, *workspace;

   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   workspace = workspace_add(parent);

   res = workspace_separate_mode_set(workspace, EINA_TRUE);
   ck_assert_msg(res == EINA_FALSE, "Failed set separate mode for workspace");

   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_separate_mode_set
 * @{
 * </TABLE>
 * @}
 * @}
 */
