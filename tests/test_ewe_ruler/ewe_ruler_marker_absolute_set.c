/* Elementary Widgets Extension
 * Copyright (C) 2014 Samsung Electronics.
 *
 * This file is part of Elementary Widgets Extension.
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
#include "test_ewe_ruler.h"

/**
 * @addtogroup ewe_ruler_test
 * @{
 * @addtogroup ewe_ruler_marker_absolute_set
 * @{
 * ewe_ruler
 * <TABLE>
 * @}
 */

/**
 * @addtogroup ewe_ruler_marker_absolute_set
 * @{
 * <tr>
 * <td>ewe_ruler_marker_absolute_set</td>
 * <td>ewe_ruler_marker_absolute_set_test_p</td>
 * <td>
 * @precondition
 * @step 1 init ewe
 * @step 2 create window
 * @step 3 add ewe_ruler
 * @step 4 add marker to ruler
 *
 * @procedure
 * @step 1 call ewe_ruler_marker_absolute_set(ruler, marker, 125)
 * @step 2 check returned value
 * @step 3 check setted value
 * @step 4 call ewe_ruler_marker_absolute_set(ruler, marker, -125)
 * @step 5 check returned value
 * @step 6 check setted value
 * </td>
 * <td>Evas_Object *ruler, Ewe_Ruler_Marker* marker</td>
 * <td>All checks passes</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(ewe_ruler_marker_absolute_set_test_p)
{
   ewe_init(0, 0);
   Evas_Object *win = elm_win_util_standard_add("test", "test");
   Evas_Object *ruler = ewe_ruler_add(win);
   Ewe_Ruler_Marker *marker = ewe_ruler_marker_add(ruler, NULL);

   ck_assert_msg(ewe_ruler_marker_absolute_set(ruler, marker, 125) == EINA_TRUE, "Can't set abs position");
   ck_assert_msg(ewe_ruler_marker_absolute_get(ruler, marker) == 125, "Wrong value setted");
   ck_assert_msg(ewe_ruler_marker_absolute_set(ruler, marker, -125) == EINA_TRUE, "Can't set abs position");
   ck_assert_msg(ewe_ruler_marker_absolute_get(ruler, marker) == -125, "Wrong value setted");

   evas_object_del(win);
   ewe_shutdown();
}
END_TEST

/**
 * @addtogroup ewe_ruler_marker_absolute_set
 * @{
 * <tr>
 * <td>ewe_ruler_marker_absolute_set</td>
 * <td>ewe_ruler_marker_absolute_set_test_n</td>
 * <td>
 * @precondition
 * @step 1 init ewe
 * @step 2 create window
 * @step 3 add ewe_ruler
 * @step 4 add marker to ruler
 *
 * @procedure
 * @step 1 call ewe_ruler_marker_absolute_set(NULL, marker, 125)
 * @step 2 check returned value
 * @step 3 call ewe_ruler_marker_absolute_set(ruler, NULL, 125)
 * @step 4 check returned value
 * @step 5 call ewe_ruler_marker_absolute_set(NULL, NULL, 125)
 * @step 6 check returned value
 * </td>
 * <td>NULL, NULL, 125</td>
 * <td>All check passed</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(ewe_ruler_marker_absolute_set_test_n)
{
   ewe_init(0, 0);
   Evas_Object *win = elm_win_util_standard_add("test", "test");
   Evas_Object *ruler = ewe_ruler_add(win);
   Ewe_Ruler_Marker *marker = ewe_ruler_marker_add(ruler, NULL);

   ck_assert_msg(ewe_ruler_marker_absolute_set(NULL, marker, 125) == EINA_FALSE, "Setted abs position for NULL ruler object");
   ck_assert_msg(ewe_ruler_marker_absolute_set(ruler, NULL, 125) == EINA_FALSE, "Setted abs position for NULL marker");
   ck_assert_msg(ewe_ruler_marker_absolute_set(NULL, NULL, 125) == EINA_FALSE, "Setted abs position for NULL ruler object for NULL marker");

   ewe_shutdown();
}
END_TEST

/**
 * @addtogroup ewe_ruler_marker_absolute_set
 * @{
 * </TABLE>
 * @}
 * @}
 */
