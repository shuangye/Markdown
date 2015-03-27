#include "global_vars.h"
#include <stdlib.h>

char *g_charbuf = "";     /* Buffer of characters to be parsed. */
element *g_references = NULL;    /* List of link references found. */
element *g_notes = NULL;         /* List of footnotes found. */
element *g_parse_result;  /* Results of parse. */
int g_syntax_extensions;  /* Syntax extensions selected. */

