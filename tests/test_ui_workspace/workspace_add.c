/* Edje Theme Editor
* Copyright (C) 2013 Samsung Electronics.
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
 * @addgroup test_ui_workspace
 * @{
 * @addtogroup workspace_add
 * @{
 * ui_workspace
 * <TABLE>
 * @}
 */

/**
 * @addtogroup workspace_add
 * @{
 * <tr>
 * <td>workspace_add</td>
 * <td>workspace_add_test_p</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extension theme from EFLETE_THEME file
 * @step 3 create parent window
 *
 * @procedure
 * @step 1 Call workspace_add
 * @step 2 Check returned pointer
 * </td>
 * <td>(Evas_Object *) parent</td>
 * <td>Returned pointer not NULL</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (workspace_add_test_p)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   Evas_Object *parent, *workspace;
   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);
   workspace = workspace_add(parent);
   ck_assert_msg(workspace != NULL, "Failed create workspace object.");
   evas_object_del(parent);
   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_add
 * @{
 * <tr>
 * <td>workspace_add</td>
 * <td>workspace_add_test_n</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 *
 * @procedure
 * @step 1 Call workspace_add
 * @step 2 Check returned pointer
 * </td>
 * <td>(Evas_Object *) parent</td>
 * <td>Returned pointer is NULL</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (workspace_add_test_n)
{
   elm_init(0, 0);
   Evas_Object *workspace = workspace_add(NULL);
   ck_assert_msg(workspace == NULL, "Returned pointer is not NULL");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_add
 * @{
 * </TABLE>
 * @}
 * @}
 */
