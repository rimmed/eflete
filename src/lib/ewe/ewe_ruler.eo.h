#ifndef _EWE_RULER_EO_H_
#define _EWE_RULER_EO_H_

#ifndef _EWE_RULER_EO_CLASS_TYPE
#define _EWE_RULER_EO_CLASS_TYPE

typedef Eo Ewe_Ruler;

#endif

#ifndef _EWE_RULER_EO_TYPES
#define _EWE_RULER_EO_TYPES


#endif
#define EWE_RULER_CLASS ewe_ruler_class_get()

EAPI const Eo_Class *ewe_ruler_class_get(void) EINA_CONST;

/**
 * @brief Change the ruler's orientation to horizontal or vertical.
 *
 * @param[in] horizontal
 *
 * @return EINA_TRUE if orientation was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_horizontal_set(Eina_Bool horizontal);

/** Get the ruler orientation
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_horizontal_get(void);

/**
 * @brief Set step between marks of the ruler.
 *
 * @param[in] scale
 * @param[in] step
 *
 * @return EINA_TRUE if step was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_step_set(Ewe_Ruler_Scale * scale, unsigned int step);

/**
 * @brief Get step between marks of the ruler.
 *
 * @param[in] scale
 *
 * @ingroup Ewe_Ruler
 */
EOAPI unsigned int  ewe_obj_ruler_step_get(Ewe_Ruler_Scale * scale);

/**
 * @brief Set step between marks values of the scale
 *
 * @param[in] scale
 * @param[in] step
 *
 * @return EINA_TRUE if step was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_value_step_set(Ewe_Ruler_Scale * scale, double step);

/**
 * @brief Get step between marks values of the ruler.
 *
 * @param[in] scale
 *
 * @ingroup Ewe_Ruler
 */
EOAPI double  ewe_obj_ruler_value_step_get(Ewe_Ruler_Scale * scale);

/**
 * @brief Change the ruler's "zero pointer" mark position
 *
 * @param[in] scale
 * @param[in] offset
 *
 * @return EINA_TRUE if offset was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_zero_offset_set(Ewe_Ruler_Scale * scale, int offset);

/**
 * @brief Get "zero pointer" mark position
 *
 * @param[in] scale
 *
 * @ingroup Ewe_Ruler
 */
EOAPI int  ewe_obj_ruler_zero_offset_get(Ewe_Ruler_Scale * scale);

/**
 * @brief Set format string for given scale
 *
 * @param[in] scale
 * @param[in] format
 *
 * @return EINA_TRUE if format was setted or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_format_set(Ewe_Ruler_Scale * scale, const char *format);

/**
 * @brief Get format string of given scale.
 *
 * @param[in] scale
 *
 * @ingroup Ewe_Ruler
 */
EOAPI const char * ewe_obj_ruler_format_get(Ewe_Ruler_Scale * scale);

/**
 * @brief Set style for given scale
 *
 * @param[in] scale
 * @param[in] style
 *
 * @return EINA_TRUE if style was setted or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_style_set(Ewe_Ruler_Scale * scale, const char *style);

/**
 * @brief Get style of given scale.
 *
 * @param[in] scale
 *
 * @ingroup Ewe_Ruler
 */
EOAPI const char * ewe_obj_ruler_style_get(Ewe_Ruler_Scale * scale);

/**
 * @brief Change the scale's visibility
 *
 * @param[in] scale
 * @param[in] visible
 *
 * @return EINA_TRUE if offset was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_scale_visible_set(Ewe_Ruler_Scale * scale, Eina_Bool visible);

/**
 * @brief Get visibility of given scale
 *
 * @param[in] scale
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_scale_visible_get(Ewe_Ruler_Scale * scale);

/**
 * @brief Change the scale middle mark visibility
 *
 * @param[in] scale
 * @param[in] middle_mark
 *
 * @return EINA_TRUE if middle_mark was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_scale_middle_mark_set(Ewe_Ruler_Scale * scale, Eina_Bool middle_mark);

/**
 * @brief Get visibility of middle mark in given scale
 *
 * @param[in] scale
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_scale_middle_mark_get(Ewe_Ruler_Scale * scale);

/**
 * @brief Set marker to folow specified value on given scale
 *
 * @param[in] marker
 * @param[in] scale
 * @param[in] rel_position
 *
 * @return EINA_TRUE if position was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_marker_relative_set(Ewe_Ruler_Marker * marker, Ewe_Ruler_Scale * scale, double rel_position);

/**
 * @brief Get marker position relative to given scale
 *
 * @param[in] marker
 * @param[in] scale
 *
 * @ingroup Ewe_Ruler
 */
EOAPI double  ewe_obj_ruler_marker_relative_get(Ewe_Ruler_Marker * marker, Ewe_Ruler_Scale * scale);

/**
 * @brief Set marker absolute posiiton
 *
 * @param[in] marker
 * @param[in] position
 *
 * @return EINA_TRUE if position was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_marker_absolute_set(Ewe_Ruler_Marker * marker, int position);

/**
 * @brief Get marker position
 *
 * @param[in] marker
 *
 * @ingroup Ewe_Ruler
 */
EOAPI int  ewe_obj_ruler_marker_absolute_get(Ewe_Ruler_Marker * marker);

/**
 * @brief Change the marker's visibility
 *
 * @param[in] marker
 * @param[in] visible
 *
 * @return EINA_TRUE if visibility was set or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_marker_visible_set(Ewe_Ruler_Marker * marker, Eina_Bool visible);

/**
 * @brief Get visibility of given marker
 *
 * @param[in] marker
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_marker_visible_get(Ewe_Ruler_Marker * marker);

/**
 * @brief Set style for given marker
 *
 * @param[in] marker
 * @param[in] style
 *
 * @return EINA_TRUE if style was setted or EINA_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_marker_style_set(Ewe_Ruler_Marker * marker, const char *style);

/**
 * @brief Get style of given marker.
 *
 * @param[in] marker
 *
 * @ingroup Ewe_Ruler
 */
EOAPI const char * ewe_obj_ruler_marker_style_get(Ewe_Ruler_Marker * marker);

/**
 * @brief Set new size for given marker
 *
 * @param[in] marker
 * @param[in] size
 *
 * @ingroup Ewe_Ruler
 */
EOAPI void  ewe_obj_ruler_marker_size_set(Ewe_Ruler_Marker * marker, int size);

/**
 * @brief Get size of given marker.
 *
 * @param[in] marker
 *
 * @ingroup Ewe_Ruler
 */
EOAPI int  ewe_obj_ruler_marker_size_get(Ewe_Ruler_Marker * marker);

/**
 * @brief Set text to given marker
 *
 * @param[in] marker
 * @param[in] text
 *
 * @ingroup Ewe_Ruler
 */
EOAPI void  ewe_obj_ruler_marker_text_set(Ewe_Ruler_Marker * marker, const char *text);

/**
 * @brief Get text of given marker.
 *
 * @param[in] marker
 *
 * @ingroup Ewe_Ruler
 */
EOAPI const char * ewe_obj_ruler_marker_text_get(Ewe_Ruler_Marker * marker);

/**
 * @brief Set text to given marker
 *
 * @param[in] marker
 * @param[in] text
 *
 * @ingroup Ewe_Ruler
 */
EOAPI void  ewe_obj_ruler_marker_tooltip_set(Ewe_Ruler_Marker * marker, const char *text);

/**
 * @brief Add new scale to ruler.
 *
 * @param[in] style
 *
 * @return Scale object or NULL on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Ewe_Ruler_Scale * ewe_obj_ruler_scale_add(const char *style);

/**
 * @brief Delete given scale from ruler.
 *
 * @param[in] scale
 *
 * @return EINA_TRUE on success, EIAN_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_scale_del(Ewe_Ruler_Scale *scale);

/**
 * @brief Add new marker to ruler.
 *
 * @param[in] style
 *
 * @return Marker object or NULL on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Ewe_Ruler_Marker * ewe_obj_ruler_marker_add(const char *style);

/**
 * @brief Delete given marker from ruler.
 *
 * @param[in] marker
 *
 * @return EINA_TRUE on success, EIAN_FALSE on failure.
 *
 * @ingroup Ewe_Ruler
 */
EOAPI Eina_Bool  ewe_obj_ruler_marker_del(Ewe_Ruler_Marker *marker);


#endif
