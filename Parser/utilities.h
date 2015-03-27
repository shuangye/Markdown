#pragma once

#ifndef UTILITIES
#define UTILITIES

/// This intends to be a C++ header

// #include <string>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "types.h"

	/* cons - cons an element onto a list, returning pointer to new head */
	element * cons(element *new_element, element *list);

	/* reverse - reverse a list, returning pointer to new list */
	element *reverse(element *list);
	/* concat_string_list - concatenates string contents of list of STR elements.
	 * Frees STR elements as they are added to the concatenation. */
	// string *concat_string_list(element *list);

	/**********************************************************************

	  Auxiliary functions for parsing actions.
	  These make it easier to build up data structures (including lists)
	  in the parsing actions.

	  ***********************************************************************/

	/* mk_element - generic constructor for element */
	element * mk_element(int key);

	/* mk_str - constructor for STR element */
	element * mk_str(char *string);

	/* mk_str_from_list - makes STR element by concatenating a
	 * reversed list of strings, adding optional extra newline */
	element * mk_str_from_list(element *list, bool extra_newline);

	/* mk_list - makes new list with key 'key' and children the reverse of 'lst'.
	 * This is designed to be used with cons to build lists in a parser action.
	 * The reversing is necessary because cons adds to the head of a list. */
	element * mk_list(int key, element *lst);

	/* mk_link - constructor for LINK element */
	element * mk_link(element *label, char *url, char *title);
	/* extension = returns true if extension is selected */
	bool extension(int ext);

	/* match_inlines - returns true if inline lists match (case-insensitive...) */
	bool match_inlines(element *l1, element *l2);

	/* find_reference - return true if link found in references matching label.
	 * 'link' is modified with the matching url and title. */
	bool find_reference(link *result, element *label);

	/* find_note - return true if note found in notes matching label.
	if found, 'result' is set to point to matched note. */

	bool find_note(element **result, char *label);


#ifdef __cplusplus
}
#endif // __cplusplus

#endif

