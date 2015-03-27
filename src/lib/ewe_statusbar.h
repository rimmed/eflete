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

#ifndef EWE_STATUSBAR_H
#define EWE_STATUSBAR_H

/**
 * @defgroup Statusbar Statusbar
 * @ingroup EWE
 *
 * FIXME: add full description with images and other.
 */

/**
 * @addtogroup Statusbar
 * @{
 */

#include "ewe_statusbar_common.h"
#ifdef EFL_EO_API_SUPPORT
#include "ewe_statusbar_eo.h"
#endif
#ifndef EFL_NOLEGACY_API_SUPPORT
#include "ewe_statusbar_legacy.h"
#endif

/**
 * @}
 */

#endif /* EWE_STATUSBAR_H */
