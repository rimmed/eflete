/*
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

#ifndef ABOUT_DIALOG_H
#define ABOUT_DIALOG_H

/**
 * @defgroup AboutDialog About Dialog
 *
 * This widget emits the following signals:
 *
 * TextStyle is a Inwin that contain information
 * of the widgets text styles.
 */

#include "eflete.h"

/**
 * Show the whole inwin window by using some data
 * about loaded project (edj file)
 *
 * @return the inwin object that contain style viewer
 *
 * @ingroup AboutDialog
 */
Evas_Object *
about_window_add();

#endif  /* ABOUT_DIALOG_H */
