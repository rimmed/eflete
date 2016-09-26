/* Elementary Widgets Extension
 * Copyright (C) 2014 Samsung Electronics.
 *
 * This file is part of Elementary Widgets Extension.
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
#include "test_ewe_ruler.h"

/**
 * @addtogroup ewe_ruler_test
 * @{
 * @addtogroup ewe_ruler_scale_add
 * @{
 * ewe_ruler
 * <TABLE>
 * @}
 */

/**
 * @addtogroup ewe_ruler_scale_add
 * @{
 * <tr>
 * <td>ewe_ruler_scale_add</td>
 * <td>ewe_ruler_scale_add_test_p</td>
 * <td>
 * @precondition
 * @step 1 init ewe
 * @step 2 create window
 * @step 3 add ewe_ruler
 *
 * @procedure
 * @step 1 call ewe_ruler_scale_add
 * @step 2 check returned value
 * </td>
 * <td>Evas_Object *ruler, const char* style</td>
 * <td>not NULL pointer returned</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(ewe_ruler_scale_add_test_p)
{
   ewe_init(0, 0);
   Evas_Object *win = elm_win_util_standard_add("test", "test");
   Evas_Object *ruler = ewe_ruler_add(win);

   ck_assert_msg(ewe_ruler_scale_add(ruler, NULL) != NULL, "Cannot add scale to the ruler");

   evas_object_del(win);
   ewe_shutdown();
}
END_TEST

/**
 * @addtogroup ewe_ruler_scale_add
 * @{
 * <tr>
 * <td>ewe_ruler_scale_add</td>
 * <td>ewe_ruler_scale_add_test_n</td>
 * <td>
 * @precondition
 * @step 1 init ewe
 * @step 2 don't create ruler object
 *
 * @procedure
 * @step 1 call ewe_ruler_scale_add
 * @step 2 check returned value
 * </td>
 * <td>NULL, NULL</td>
 * <td>NULL pointer returned</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(ewe_ruler_scale_add_test_n)
{
   ewe_init(0, 0);

   ck_assert_msg(ewe_ruler_scale_add(NULL, NULL) == NULL, "Added scale to NULL ruler object");

   ewe_shutdown();
}
END_TEST

/**
 * @addtogroup ewe_ruler_scale_add
 * @{
 * </TABLE>
 * @}
 * @}
 */
