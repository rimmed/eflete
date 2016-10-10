#ifndef _ELM_COMBOBOX_EO_H_
#define _ELM_COMBOBOX_EO_H_

#ifndef _ELM_COMBOBOX_EO_CLASS_TYPE
#define _ELM_COMBOBOX_EO_CLASS_TYPE

typedef Eo Elm_Combobox;

#endif

#ifndef _ELM_COMBOBOX_EO_TYPES
#define _ELM_COMBOBOX_EO_TYPES


#endif
#define ELM_COMBOBOX_CLASS elm_combobox_class_get()

const Eo_Class *elm_combobox_class_get(void) EINA_CONST;

/**
 * @brief Returns whether the combobox is expanded.
 *
 * This will return EINA_TRUE if the combobox is expanded or EINA_FALSE if it
 * is not expanded.
 *
 * @ingroup Elm_Combobox
 */
Eina_Bool  elm_obj_combobox_expanded_get(void);

/** This triggers the combobox popup from code, the same as if the user had
 * clicked the button.
 *
 * @ingroup Elm_Combobox
 */
void  elm_obj_combobox_hover_begin(void);

/** This dismisses the combobox popup as if the user had clicked outside the
 * hover.
 *
 * @ingroup Elm_Combobox
 */
void  elm_obj_combobox_hover_end(void);

extern const Eo_Event_Description _ELM_COMBOBOX_EVENT_DISMISSED;
extern const Eo_Event_Description _ELM_COMBOBOX_EVENT_EXPANDED;
extern const Eo_Event_Description _ELM_COMBOBOX_EVENT_CLICKED;
extern const Eo_Event_Description _ELM_COMBOBOX_EVENT_ITEM_SELECTED;
extern const Eo_Event_Description _ELM_COMBOBOX_EVENT_ITEM_PRESSED;
extern const Eo_Event_Description _ELM_COMBOBOX_EVENT_FILTER_DONE;

/**
 * No description
 */
#define ELM_COMBOBOX_EVENT_DISMISSED (&(_ELM_COMBOBOX_EVENT_DISMISSED))

/**
 * No description
 */
#define ELM_COMBOBOX_EVENT_EXPANDED (&(_ELM_COMBOBOX_EVENT_EXPANDED))

/**
 * No description
 */
#define ELM_COMBOBOX_EVENT_CLICKED (&(_ELM_COMBOBOX_EVENT_CLICKED))

/**
 * No description
 */
#define ELM_COMBOBOX_EVENT_ITEM_SELECTED (&(_ELM_COMBOBOX_EVENT_ITEM_SELECTED))

/**
 * No description
 */
#define ELM_COMBOBOX_EVENT_ITEM_PRESSED (&(_ELM_COMBOBOX_EVENT_ITEM_PRESSED))

/**
 * No description
 */
#define ELM_COMBOBOX_EVENT_FILTER_DONE (&(_ELM_COMBOBOX_EVENT_FILTER_DONE))

#endif
