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

#include "test_groupedit.h"

/**
 * @addtogroup groupedit_test
 * @{
 * @addtogroup groupedit_add
 * @{
 * Groupedit
 * <TABLE>
 * @}
 */

/**
 * @addtogroup groupedit_add
 * @{
 * <tr>
 * <td>groupedit_add</td>
 * <td>groupedit_add_test_p</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 create parent window
 *
 * @procedure
 * @step 1 call groupedit_add
 * @step 2 check returned pointer
 * </td>
 * <td>(Evas_Object *) window</td>
 * <td>Returned pointer not NULL</td>
 * </tr>
 * @}
 */
EFLETE_TEST (groupedit_add_test_p)
{
   Evas_Object *parent, *groupedit;

   elm_init(0, 0);

   parent = elm_win_add(NULL, "test", ELM_WIN_BASIC);

   groupedit = groupedit_add(parent, NULL);
   ck_assert_msg(groupedit != NULL, "Failed create groupedit object.");
   evas_object_del(parent);

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup groupedit_add
 * @{
 * </TABLE>
 * @}
 * @}
 */