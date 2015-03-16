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


#include "test_history.h"
#include "main_window.h"
#include "test_common.h"

/**
 * @addtogroup history_test
 * @{
 * @addtogroup history_redo
 * @{
 * history
 * <TABLE>
 * @}
 */

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p1</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 7 Save current value of  min height param of "bg" part.
 * @step 8 Set new value [10] for min height param of "bg" part.
 * @step 9 Store diff with using history_diff_add function.
 * @step 10 Cancel change with history_undo function
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of min height param of part "bg" with the
 *         value, that was setted at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p1)
{
   App_Data *app = NULL;
   Eina_Bool result = EINA_FALSE;
   Style *style = NULL;
   int old_value = -1;
   int new_value = 10;
   int check_value = -15;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p1");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p1/history_redo_test_p1.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_min_h_set(style->obj, "bg", "default", 0.0, new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_state_min_h_set,
                    "Min h", "bg", "default", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to redo diff with VAL_INT value type.");
   check_value = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == new_value, "Value didn't restore");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p1");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p2</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  drag x param of "bg" part.
 * @step 10 Set new value [1] for drag x param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of drag x param of part "bg" with the
 *         value, that was saved at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p2)
{
   App_Data *app = NULL;
   Eina_Bool result = EINA_FALSE;
   Style *style = NULL;
   int old_value = 99;
   int new_value = 1;
   int check_value = 55;

   elm_init(0, 0);
   app_init();
   app = app_data_get();
   ui_main_window_add(app);
   setup("history_redo_test_p2");

   app->project = pm_project_open("./history_redo_test_p2/history_redo_test_p2.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_part_drag_x_get(style->obj, "bg");
   edje_edit_part_drag_x_set(style->obj, "bg", new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_part_drag_x_set,
                    "Drag x", "bg", NULL, 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to redo diff with VAL_INT value type, setted without state");
   check_value = edje_edit_part_drag_x_get(style->obj, "bg");
   ck_assert_msg(check_value == new_value, "Value didn't restore");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p2");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p3</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  drag y param of "bg" part.
 * @step 10 Set new value [1] for drag y param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Save current value of  min weight param of "bg" part.
 * @step 13 Set new value [10] for min weight param of "bg" part.
 * @step 14 Store diff with using history_diff_add function.
 * @step 15 Cancel all changes with history_undo function
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of drag y param of part "bg" with the
 *         value, that was setted at step 9 of precondition.
 * @step 4 Compare current value of min weight param of part "bg" with the
 *         value, that was saved at step 11 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p3)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value_drag_y = 11;
   int new_value_drag_y = 1;
   int check_value_drag_y = 55;
   int old_value_min_w = -1;
   int new_value_min_w = 10;
   int check_value_min_w = -15;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p3");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p3/history_redo_test_p3.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value_drag_y = edje_edit_part_drag_y_get(style->obj, "bg");
   edje_edit_part_drag_y_set(style->obj, "bg", new_value_drag_y);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value_drag_y,
                    new_value_drag_y,
                    "elm/radio/base/def", (void *)edje_edit_part_drag_y_set,
                    "Drag y", "bg", NULL, 0.0);
   old_value_min_w = edje_edit_state_min_w_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_min_w_set(style->obj, "bg", "default", 0.0, new_value_min_w);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value_min_w,
                    new_value_min_w, "elm/radio/base/def",
                    (void *)edje_edit_state_min_w_set,
                    "Min h", "bg", "default", 0.0);
   history_undo(style->obj, 2);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to redo diff with VAL_INT value type");
   check_value_drag_y = edje_edit_part_drag_y_get(style->obj, "bg");
   ck_assert_msg(check_value_drag_y == new_value_drag_y, "Didn't restore value'");
   check_value_min_w = edje_edit_state_min_w_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value_min_w == old_value_min_w, "Restored all changes");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p3");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p4</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  drag x param of "bg" part.
 * @step 10 Set new value [1] for drag x param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Save current value of  min height param of "bg" part.
 * @step 13 Set new value [10] for min height param of "bg" part.
 * @step 14 Store diff with using history_diff_add function.
 * @step 15 Cancel all changes with history_undo function
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of drag x param of part "bg" with the
 *         value, that was setted at step 9 of precondition.
 * @step 4 Compare current value of min height param of part "bg" with the
 *         value, that was setted at step 12 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 2 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p4)
{
   App_Data *app = NULL;
   Eina_Bool result = EINA_FALSE;
   Style *style = NULL;
   int old_value_drag_x = 99;
   int new_value_drag_x = 1;
   int check_value_drag_x = 55;
   int old_value_min_h = -1;
   int new_value_min_h = 10;
   int check_value_min_h = -15;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p4");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p4/history_redo_test_p4.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value_drag_x = edje_edit_part_drag_x_get(style->obj, "bg");
   edje_edit_part_drag_x_set(style->obj, "bg", new_value_drag_x);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value_drag_x, new_value_drag_x,
                    "elm/radio/base/def", (void *)edje_edit_part_drag_x_set,
                    "Drag x", "bg", NULL, 0.0);
   old_value_min_h = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_min_h_set(style->obj, "bg", "default", 0.0, new_value_min_h);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value_min_h, new_value_min_h,
                    "elm/radio/base/def", (void *)edje_edit_state_min_h_set,
                    "Min h", "bg", "default", 0.0);
   history_undo(style->obj, 2);

   result = history_redo(style->obj, 2);
   ck_assert_msg(result, "Failed to restore diffs");
   check_value_drag_x = edje_edit_part_drag_x_get(style->obj, "bg");
   ck_assert_msg(check_value_drag_x == new_value_drag_x, "First change is not restored");
   check_value_min_h = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value_min_h == new_value_min_h, "Not all changes are restored");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p4");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p5</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  aspect max param of "bg" part.
 * @step 10 Set new value [0.4] for aspect param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of aspect max param of part "bg" with the
 *         value, that was setted at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p5)
{
   App_Data *app = NULL;
   Eina_Bool result = EINA_FALSE;
   Style *style = NULL;
   double old_value = -1;
   double new_value = 0.4;
   double check_value = -5;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p5");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p5/history_redo_test_p5.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_state_aspect_max_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_aspect_max_set(style->obj, "bg", "default", 0.0, new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_DOUBLE, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_state_aspect_max_set,
                    "Min h", "bg", "default", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to recover diff with VAL_DOUBLE value type.");
   check_value = edje_edit_state_aspect_max_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == new_value, "Action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p5");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p6</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of clip to param of "bg" part.
 * @step 10 Set new value [events] for clip to param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of clip to param of part "bg" with the
 *         value, that was setted at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p6)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   Eina_Stringshare *old_value = NULL;
   Eina_Stringshare *new_value =  NULL;
   Eina_Stringshare *check_value = NULL;
   const char *tmp;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p6");

   new_value = eina_stringshare_add("events");
   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p6/history_redo_test_p6.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   tmp = edje_edit_part_clip_to_get(style->obj, "bg");
   old_value = eina_stringshare_add(tmp);
   edje_edit_part_clip_to_set(style->obj, "bg", new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, STRING, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_part_clip_to_set,
                    "clip to", "bg", NULL, 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to restore diff with STRING value type.");
   tmp = edje_edit_part_clip_to_get(style->obj, "bg");
   check_value = eina_stringshare_add(tmp);
   ck_assert_msg(check_value == new_value, "Restored action doesn't change value");

   eina_stringshare_del(new_value);
   eina_stringshare_del(old_value);
   eina_stringshare_del(check_value);

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p6");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p7</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of rel1 to x param of "radio" part.
 * @step 10 Set new value [events] for rel1 to x param of "radio" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of rel1 to x param of part "radio" with the
 *         value, that was setted at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p7)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   Eina_Stringshare *old_value = NULL;
   Eina_Stringshare *new_value = NULL;
   Eina_Stringshare *check_value = NULL;
   const char *tmp;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p7");

   new_value = eina_stringshare_add("events");
   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p7/history_redo_test_p7.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   tmp = edje_edit_state_rel1_to_x_get(style->obj, "radio", "default", 0.0);
   old_value = eina_stringshare_add(tmp);
   edje_edit_state_rel1_to_x_set(style->obj, "radio", "default", 0.0, new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, STRING, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_state_rel1_to_x_set,
                    "clip to", "radio", "default", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to recover diff with STRING value type.");
   tmp = edje_edit_state_rel1_to_x_get(style->obj, "radio", "default", 0.0);
   check_value = eina_stringshare_add(tmp);
   ck_assert_msg(check_value == new_value, "Recover action doesn't change value");

   eina_stringshare_del(new_value);
   eina_stringshare_del(old_value);
   eina_stringshare_del(check_value);

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p7");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p8</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize history module.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of color param of "bg" part.
 * @step 10 Set new value [80;90;100;120] for color param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 10 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of color param of part "bg" with the
 *         value, that was setted at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p8)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int oldr, oldg, oldb, olda;
   int newr = 80, newg = 90, newb = 100, newa = 120;
   int checkr, checkg, checkb, checka;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p8");

   app = app_data_get();
   app->history = history_init();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p8/history_redo_test_p8.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_state_color_get(style->obj, "radio", "default", 0.0, &oldr, &oldg,
                             &oldb, &olda);
   edje_edit_state_color_set(style->obj, "radio", "default", 0.0, newr, newg,
                             newb, newa);
   history_diff_add(style->obj, PROPERTY, MODIFY, FOUR, oldr, oldg, oldb, olda,
                    newr, newg, newb, newa, "elm/radio/base/def",
                    (void *)edje_edit_state_color_set,
                    "clip to", "radio", "default", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to recover diff with FOUR value type.");
   edje_edit_state_color_get(style->obj, "radio", "default", 0.0, &checkr, &checkg,
                             &checkb, &checka);
   ck_assert_msg(((checkr == newr) && (checkg == newg) && (checkb == newb) &&
                  (checka == newa )), "Recovered action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p8");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p9</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  min height param of group.
 * @step 10 Set new value [10] for min height param of group.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of min height param of group with the
 *         value, that was setted at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p9)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value = -1;
   int new_value = 10;
   int check_value = -15;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p9");

   app = app_data_get();
   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p9/history_redo_test_p9.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_group_min_h_get(style->obj);
   edje_edit_group_min_h_set(style->obj, new_value);
   edje_edit_group_max_h_set(style->obj, new_value);
   history_diff_add(style->obj, PROPERTY, CONTAINER, VAL_GROUP, old_value, new_value,
                    old_value, new_value, (void *)edje_edit_group_min_h_set,
                    "elm/radio/base/def", (void *)edje_edit_group_max_h_set,
                    "Group height", NULL, NULL, 0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to recover diff with VAL_GROUP value type.");
   check_value = edje_edit_group_min_h_get(style->obj);
   ck_assert_msg(check_value == new_value, "Recovered action doesn't change value");
   check_value = edje_edit_group_max_h_get(style->obj);
   ck_assert_msg(check_value == new_value, "Recovered action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p9");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p10</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Set new name [new] for part "bg".
 * @step 10 Set new name [new] for Part in style.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check is exist part "new" in group.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p10)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Part *part = NULL;
   Eina_Bool result = EINA_FALSE;
   char *old_value = "bg";
   char *new_value = "new";

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p10");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p10/history_redo_test_p10.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   part = wm_part_by_name_find(style, eina_stringshare_add(old_value));
   edje_edit_part_name_set(style->obj, old_value, new_value);
   part->name = eina_stringshare_add(new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, RENAME, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_part_name_set,
                    "Rename", new_value, NULL, 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to redo diff with RENAME value type.");
   result = edje_edit_part_exist(style->obj, new_value);
   ck_assert_msg(result, "Recover action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p10");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p11</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Add new  tween image ["radio.png"] to the "bg" part.
 * @step 10 Store diff with using history_diff_add function.
 * @step 11 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check count of entries in list of tween images, should be 1
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p11)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   const char *name = "radio.png";
   Eina_List *tween_list = NULL;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p11");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p11/history_redo_test_p11.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_state_tween_add(style->obj, "bg", "default", 0.0, name);
   history_diff_add(style->obj, PROPERTY, ADD, STRING, name,
                    (void *)edje_edit_state_tween_del, "elm/radio/base/def",
                    (void *)edje_edit_state_tween_add,
                    "tween add", "bg", "default", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to restore diff with ADD action and STRING value type.");
   tween_list = edje_edit_state_tweens_list_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(eina_list_count(tween_list) == 1,
                 "Restored action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p11");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p12</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Add new  tween image ["radio.png"] to the "bg" part.
 * @step 10  Del tween image, that was added at step 9 from "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check count of entries in list of tween images, should be 0
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p12)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   char *name = "radio2.png";
   Eina_List *tween_list = NULL;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p12");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p12/history_redo_test_p12.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_state_tween_add(style->obj, "bg", "default", 0.0, name);
   edje_edit_state_tween_del(style->obj, "bg", "default", 0.0, name);
   history_diff_add(style->obj, PROPERTY, DEL, STRING, name,
                    (void *)edje_edit_state_tween_add, "elm/radio/base/def",
                    (void *)edje_edit_state_tween_del,
                    "tween del", "bg", "default", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to restore diff with DEL action and STRING value type.");
   tween_list = edje_edit_state_tweens_list_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(eina_list_count(tween_list) == 0,
                 "Restored action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p12");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p13</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Set new value [10] for max height param of "bg" part.
 * @step 10 Set new value [99] for max weight param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of the max height param with value, that was
 *         setted at the step 9 of precondition.
 * @step 4 Compare current value of the max weight param with value, that was
 *         setted at the step 10 of the precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p13)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value_1 = -10, old_value_2 = -10;
   int new_value_1 = 10, new_value_2 = 99;
   int check_value = -999;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p13");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p13/history_redo_test_p13.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value_1 = edje_edit_state_max_h_get(style->obj, "bg", "default", 0.0);
   old_value_2 = edje_edit_state_max_w_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_max_h_set(style->obj, "bg", "default", 0.0, new_value_1);
   edje_edit_state_max_w_set(style->obj, "bg", "default", 0.0, new_value_2);
   history_diff_add(style->obj, PROPERTY, CONTAINER, VAL_INT, old_value_1, new_value_1,
                    old_value_2, new_value_2, (void *)edje_edit_state_max_h_set,
                    "elm/radio/base/def", (void *)edje_edit_state_max_w_set,
                    "max size", "bg", "default", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to restore diff with CONTAINER action and VAL_INT value type.");
   check_value = edje_edit_state_max_h_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == new_value_1, "Max height didn't restored");
   check_value = edje_edit_state_max_w_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == new_value_2, "Max weight didn't restored");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p13");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p14</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Set new value [0.7] for align x param of "bg" part.
 * @step 10 Set new value [0.3] for align y param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of the align x param with value, that was
 *         setted at the step 9 of precondition.
 * @step 4 Compare current value of the align y param with value, that was
 *         setted at the step 10 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p14)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   double old_value_1 = -1.0, old_value_2 = -1.0;
   double new_value_1 = 0.7, new_value_2 = 0.3;
   double check_value = -1.0;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p14");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p14/history_redo_test_p14.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value_1 = edje_edit_state_align_x_get(style->obj, "bg", "default", 0.0);
   old_value_2 = edje_edit_state_align_y_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_align_x_set(style->obj, "bg", "default", 0.0, new_value_1);
   edje_edit_state_align_y_set(style->obj, "bg", "default", 0.0, new_value_2);
   history_diff_add(style->obj, PROPERTY, CONTAINER, VAL_DOUBLE, old_value_1, new_value_1,
                    old_value_2, new_value_2, (void *)edje_edit_state_align_x_set,
                    "elm/radio/base/def", (void *)edje_edit_state_align_y_set,
                    "align", "bg", "default", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to restore diff with CONTAINER action, VAL_DOUBLE value type.");
   check_value = edje_edit_state_align_x_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == new_value_1, "Align x didn't restored");
   check_value = edje_edit_state_align_y_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == new_value_2, "Align y didn't restored");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p14");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p15</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Add new state with name "new_state" and value 0.1 to the part "bg".
 * @step 10 Store diff with using history_diff_add function.
 * @step 11 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that state "new_state" with value 0.1 for part "bg" is exist.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p15)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p15");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p15/history_redo_test_p15.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_state_add(style->obj, "bg", "new_state", 0.1);
   history_diff_add(style->obj, STATE_TARGET, ADD, "elm/radio/base/def",
                    "bg", "new_state", 0.1, "create state");
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to redo diff with target type STATE_TARGET"
                 " with ADD action.");
   result = edje_edit_state_exist(style->obj, "bg", "new_state", 0.1);
   ck_assert_msg(result, "Adding new state didn't restored");

   pm_project_close(app->project);
   app->project = NULL;   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p15");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p16</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Delete state with name "visible" and value 0.0 from the part "elm.text".
 * @step 10 Store diff with using history_diff_add function.
 * @step 11 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that state "visible" with value 0.0 for part "elm.text" didn't exist.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p16)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p16");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p16/history_redo_test_p16.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   history_diff_add(style->obj, STATE_TARGET, DEL, "elm/radio/base/def",
                    "elm.text", "visible", 0.0, "delete state");
   edje_edit_state_del(style->obj, "elm.text", "visible", 0.0);
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to redo diff with target type STATE_TARGET"
                 " with DEL action.");
   result = edje_edit_state_exist(style->obj, "elm.text", "visible", 0.0);
   ck_assert_msg(!result, "Deleting state didn't restored");

   pm_project_close(app->project);
   app->project = NULL;   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p16");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p17</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Store diff with using history_diff_add function.
 * @step 10 Delete part with name "elm.text".
 * @step 11 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that part "elm.text" not exist.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p17)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p17");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p17/history_redo_test_p17.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   history_diff_add(style->obj, PART_TARGET, DEL, "elm.text");
   edje_edit_part_del(style->obj, "elm.text");
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to restore diff with target type PART_TARGET"
                 " with DEL action.");
   result = edje_edit_part_exist(style->obj, "elm.text");
   ck_assert_msg(!result, "Deleting part didn't repeated");

   pm_project_close(app->project);
   app->project = NULL;   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p17");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p18</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 ADD part with name "part_add" with type RECT.
 * @step 10 Store diff with using history_diff_add function.
 * @step 11 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that part "part_add" is exist.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p18)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p18");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p18/history_redo_test_p18.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_part_add(style->obj, "part_add", EDJE_PART_TYPE_RECTANGLE);
   history_diff_add(style->obj, PART_TARGET, ADD, "part_add");
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to restore diff with target type PART_TARGET"
                 " with ADD action.");
   result = edje_edit_part_exist(style->obj, "part_add");
   ck_assert_msg(result, "Adding part didn't restored");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p18");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_p19</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Store diff with using history_diff_add function.
 * @step 10 Restack below part with name "radio".
 * @step 11 Store part name, that below part "radio".
 * @step 12 Cancel change with history_undo function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that part "radio" is below part, name of that was stored at step 11.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_p19)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   Eina_Stringshare *below = NULL, *check = NULL;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_p19");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_p19/history_redo_test_p19.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   history_diff_add(style->obj, PART_TARGET, RESTACK, "radio");
   edje_edit_part_restack_below(style->obj, "part_add");
   below = eina_stringshare_add(edje_edit_part_below_get(style->obj, "radio"));
   history_undo(style->obj, 1);

   result = history_redo(style->obj, 1);
   ck_assert_msg(result, "Failed to restore diff with target type PART_TARGET"
                 " with RESTACK action.");
   check = eina_stringshare_add(edje_edit_part_below_get(style->obj, "radio"));
   ck_assert_msg(check == below, "Restack part didn't restored");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_p19");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_n1</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Create canvas, that needed for creating source object.
 * @step 4 Create edje edit object, that will be source of changes.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 5 of precondition.
 * @step 2 Check returned value.
 *
 * </td>
 * <td>(Evas_Object *) source, (int) 1 </td>
 * <td>EINA_FALSE returned</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_n1)
{
   Evas *canvas = NULL;
   Ecore_Evas *ee = NULL;
   Evas_Object *source = NULL;
   Eina_Bool result = EINA_FALSE;
   char *edj;

   edj = "./edj_build/history.edj";
   elm_init(0, 0);
   setup("history_redo_test_n1");

   ee = ecore_evas_new(NULL, 0, 0, 10, 10, NULL);
   canvas = ecore_evas_get(ee);
   source = edje_edit_object_add(canvas);
   edje_object_file_set(source, edj, "elm/radio/base/def");

   result = history_redo(source, 1);
   ck_assert_msg(!result, "Change was restored with uninitialized history module.");

   ecore_evas_free(ee);
   teardown("./history_redo_test_n1");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_n2</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  min height param of "bg" part.
 * @step 10 Set new value [10] for min height param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_redo for object from step 6 of precondition.
 * @step 2 Check returned value.
 *
 * </td>
 * <td>(Evas_Object *) style->obj, (int) -1 </td>
 * <td>EINA_FALSE returned</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_n2)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value = -1;
   int new_value = 10;

   elm_init(0, 0);
   app_init();
   setup("history_redo_test_n2");

   app = app_data_get();
   ui_main_window_add(app);
   app->project = pm_project_open("./history_redo_test_n2/history_redo_test_n2.pro");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_object_mmap_set(style->obj, app->project->mmap_file, "elm/radio/base/def");
   history_module_add(style->obj);
   old_value = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_min_h_set(style->obj, "bg", "default", 0.0, new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_state_min_h_set,
                    "Min h", "bg", "default", 0.0);

   result = history_redo(style->obj, -1);
   ck_assert_msg(!result, "Restore diff with quantity of changes  '-1' .");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_redo_test_n2");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * <tr>
 * <td>history_redo</td>
 * <td>history_redo_test_n3</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 *
 * @procedure
 * @step 1 Call history_redo.
 * @step 2 Check returned value.
 *
 * </td>
 * <td>NULL, (int) 1 </td>
 * <td>EINA_FALSE returned</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_redo_test_n3)
{
   Eina_Bool result = EINA_FALSE;

   elm_init(0, 0);

   result = history_redo(NULL, 1);
   ck_assert_msg(!result, "Change was restored with NULL source param.");

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_redo
 * @{
 * </TABLE>
 * @}
 * @}
 */
