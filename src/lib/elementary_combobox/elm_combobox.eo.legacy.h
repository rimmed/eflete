#ifndef _ELM_COMBOBOX_EO_LEGACY_H_
#define _ELM_COMBOBOX_EO_LEGACY_H_

#ifndef _ELM_COMBOBOX_EO_CLASS_TYPE
#define _ELM_COMBOBOX_EO_CLASS_TYPE

typedef Eo Elm_Combobox;

#endif

#ifndef _ELM_COMBOBOX_EO_TYPES
#define _ELM_COMBOBOX_EO_TYPES


#endif

/**
 * @brief Returns whether the combobox is expanded.
 *
 * This will return EINA_TRUE if the combobox is expanded or EINA_FALSE if it
 * is not expanded.
 *
 * @ingroup Elm_Combobox
 */
Eina_Bool elm_combobox_expanded_get(const Elm_Combobox *obj);

/** This triggers the combobox popup from code, the same as if the user had
 * clicked the button.
 *
 * @ingroup Elm_Combobox
 */
void elm_combobox_hover_begin(Elm_Combobox *obj);

/** This dismisses the combobox popup as if the user had clicked outside the
 * hover.
 *
 * @ingroup Elm_Combobox
 */
void elm_combobox_hover_end(Elm_Combobox *obj);

#endif
