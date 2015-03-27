/* These yy_* functions come from markdown_parser.c which is
 * generated from markdown_parser.leg
 * */

#include "parsings.h"
#include <cstdlib>
#include "utilities.h"
#include "lib_api.h"

// include C headers in C++
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "global_vars.h"
#include "types.h"

	// C function declarations
	// the following extern functions are defined in the core parser
	typedef int(*yyrule)();
	extern int yyparse();
	extern int yyparsefrom(yyrule);
	extern int yy_References();
	extern int yy_Notes();
	extern int yy_Doc();

#ifdef __cplusplus
}
#endif // __cplusplus


static void free_element_contents(element elt);

/* free_element_list - free list of elements recursively */
void free_element_list(element * elt) {
    element * next = nullptr;
    while (elt != nullptr) {
        next = elt->next;
        free_element_contents(*elt);
        if (elt->children != nullptr) {
            free_element_list(elt->children);
            elt->children = nullptr;
        }
        free(elt);
        elt = next;
    }
}

/* free_element_contents - free element contents depending on type */
static void free_element_contents(element elt) {
    switch (elt.key) {
      case STR:
      case SPACE:
      case RAW:
      case HTMLBLOCK:
      case HTML:
      case VERBATIM:
      case CODE:
      case NOTE:
        free(elt.contents.str);
        elt.contents.str = nullptr;
        break;
      case LINK:
      case IMAGE:
      case REFERENCE:
        free(elt.contents.link->url);
        elt.contents.link->url = nullptr;
        free(elt.contents.link->title);
        elt.contents.link->title = nullptr;
        free_element_list(elt.contents.link->label);
        free(elt.contents.link);
        elt.contents.link = nullptr;
        break;
      default:
        ;
    }
}

/* free_element - free element and contents */
void free_element(element *elt) {
    free_element_contents(*elt);
    free(elt);
}

element * parse_references(char *string, int extensions) {    
    g_syntax_extensions = extensions;
    char *oldcharbuf = g_charbuf;
    g_charbuf = string;
    yyparsefrom(yy_References);    /* first pass, just to collect references */
    g_charbuf = oldcharbuf;  // restore

    return g_references;
}

element * parse_notes(char *string, int extensions, element *reference_list) {
    g_notes = nullptr;
    g_syntax_extensions = extensions;

    if (extension(EXT_NOTES)) {
        g_references = reference_list;
        char *oldcharbuf = g_charbuf;
        g_charbuf = string;
        yyparsefrom(yy_Notes);     /* second pass for notes */
        g_charbuf = oldcharbuf;
    }

    return g_notes;
}

element * parse_markdown(char *string, int extensions, element *reference_list, element *note_list) {
    char *oldcharbuf;
    g_syntax_extensions = extensions;
    g_references = reference_list;
    g_notes = note_list;
    oldcharbuf = g_charbuf;
    g_charbuf = string;

    yyparsefrom(yy_Doc);

    g_charbuf = oldcharbuf;          /* restore charbuf to original value */
    return g_parse_result;
}
