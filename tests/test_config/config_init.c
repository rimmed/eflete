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

#include "test_config.h"

/**
 * @addtogroup config_test
 * @{
 * @addtogroup config_init
 * @{
 * Config
 * <TABLE>
 * @}
 */

/**
 * @addtogroup config_init
 * @{
 * <tr>
 * <td>config_init</td>
 * <td>config_init_test_p</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 initialize application data
 *
 * @procedure
 * @step 1 call config_init
 * </td>
 * <td>(App_Data *)ap</td>
 * <td>EINA_TRUE</td>
 * </tr>
 * @}
 */
EFLETE_TEST(config_init_test_p)
{
   elm_init(0,0);
   app_init();

   ck_assert_msg(config_get() == NULL, "Config not initialized.");

   app_shutdown();
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup config_init
 * @{
 * </TABLE>
 * @}
 * @}
 */
