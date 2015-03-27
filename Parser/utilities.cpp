/* utility_functions.c - List manipulation functions, element
 * constructors, and macro definitions for leg markdown parser. */

#include "utilities.h"
#include <string>
//#include <sstream>
#include <assert.h>
//#include <cstring>
#include "parsings.h"

// include C headers in C++
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "global_vars.h"
#include "types.h"
#ifdef __cplusplus
}
#endif // __cplusplus

using std::string;



/**********************************************************************

  List manipulation functions

 ***********************************************************************/

/* cons - cons an element onto a list, returning pointer to new head */
element * cons(element *new_element, element *list) {
	assert(new_element != NULL);
	new_element->next = list;
	return new_element;
}

/* reverse - reverse a list, returning pointer to new list */
element *reverse(element *list) {
	element *new_element = NULL;
    element *next = NULL;
    while (list != NULL) {
        next = list->next;
		new_element = cons(list, new_element);
        list = next;
    }
	return new_element;
}

/* concat_string_list - concatenates string contents of list of STR elements.
 * Frees STR elements as they are added to the concatenation. */
static string *concat_string_list(element *list) {
	string *result = new string();	
	element *next;
	
	while (list != nullptr) {
		assert(list->key == STR);
		assert(list->contents.str != NULL);
		result->append(list->contents.str);
		next = list->next;
		free_element(list);
		list = next;
	}

    return result;
}



/**********************************************************************

  Auxiliary functions for parsing actions.
  These make it easier to build up data structures (including lists)
  in the parsing actions.

 ***********************************************************************/

/* mk_element - generic constructor for element */
element* mk_element(int key) {
    element *result = (element*)malloc(sizeof(element));
    result->key = key;
    result->children = NULL;
    result->next = NULL;
    result->contents.str = NULL;
    return result;
}

/* mk_str - constructor for STR element */
element * mk_str(char *str) {
	assert(nullptr != str);

    element *result;    
    result = mk_element(STR);
    result->contents.str = _strdup(str);
    return result;
}

/* mk_str_from_list - makes STR element by concatenating a
 * reversed list of strings, adding optional extra newline */
element* mk_str_from_list(element *list, bool extra_newline) {
    element *result;		
    string *c = concat_string_list(reverse(list));
	if (extra_newline)
		//g_string_append(c, "\n");
		c->append("\n");
    result = mk_element(STR);			
	result->contents.str = _strdup(c->c_str());  // create a copy
	
	delete c;
	c = nullptr;

    return result;
}

/* mk_list - makes new list with key 'key' and children the reverse of 'lst'.
 * This is designed to be used with cons to build lists in a parser action.
 * The reversing is necessary because cons adds to the head of a list. */
element * mk_list(int key, element *lst) {
    element *result;
    result = mk_element(key);
    result->children = reverse(lst);
    return result;
}

/* mk_link - constructor for LINK element */
element * mk_link(element *label, char *url, char *title) {
    element *result;
    result = mk_element(LINK);
    result->contents.link = (link*)malloc(sizeof(link));
    result->contents.link->label = label;
    result->contents.link->url = _strdup(url);
    result->contents.link->title = _strdup(title);
    return result;
}

/* extension = returns true if extension is selected */
bool extension(int ext) {
    return 0 != (g_syntax_extensions & ext);
}

/* match_inlines - returns true if inline lists match (case-insensitive...) */
bool match_inlines(element *l1, element *l2) {
    while (l1 != NULL && l2 != NULL) {
        if (l1->key != l2->key)
            return false;
        switch (l1->key) {
        case SPACE:
        case LINEBREAK:
        case ELLIPSIS:
        case EMDASH:
        case ENDASH:
        case APOSTROPHE:
            break;
        case CODE:
        case STR:
        case HTML:
            if (strcasecmp(l1->contents.str, l2->contents.str) == 0)
                break;
            else
                return false;
        case EMPH:
        case STRONG:
        case LIST:
        case SINGLEQUOTED:
        case DOUBLEQUOTED:
            if (match_inlines(l1->children, l2->children))
                break;
            else
                return false;
        case LINK:
        case IMAGE:
            return false;  /* No links or images within links */
        default:
            fprintf(stderr, "match_inlines encountered unknown key = %d\n", l1->key);
            exit(EXIT_FAILURE);
            break;
        }
        l1 = l1->next;
        l2 = l2->next;
    }
    return (l1 == NULL && l2 == NULL);  /* return true if both lists exhausted */
}

/* find_reference - return true if link found in references matching label.
 * 'link' is modified with the matching url and title. */
bool find_reference(link *result, element *label) {
    element *cur = g_references;  /* pointer to walk up list of references */
    link *curitem;
    while (cur != NULL) {
        curitem = cur->contents.link;
        if (match_inlines(label, curitem->label)) {
            *result = *curitem;
            return true;
        }
        else
            cur = cur->next;
    }
    return false;
}

/* find_note - return true if note found in notes matching label.
if found, 'result' is set to point to matched note. */

bool find_note(element **result, char *label) {
   element *cur = g_notes;  /* pointer to walk up list of notes */
   while (cur != NULL) {
       if (strcmp(label, cur->contents.str) == 0) {
           *result = cur;
           return true;
       }
       else
           cur = cur->next;
   }
   return false;
}



