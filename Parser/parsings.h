#pragma once
#ifndef PARSINGS_H
#define PARSINGS_H

/// This intends to be a C++ header

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include "types.h"

	/* free_element_list - free list of elements recursively */
	void free_element_list(element * elt);

	/* free_element - free element and contents */
	void free_element(element *elt);

	element * parse_references(char *string, int extensions);
	element * parse_notes(char *string, int extensions, element *reference_list);
	element * parse_markdown(char *string, int extensions, element *reference_list, element *note_list);

#ifdef __cplusplus
}
#endif // __cplusplus


#endif