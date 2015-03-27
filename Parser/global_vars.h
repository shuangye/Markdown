#pragma once

#ifndef GLOBAL_VARS_H
#define GLOBAL_VARS_H

/// This intends to be a C header

#include "types.h"

extern char *g_charbuf;          /* Buffer of characters to be parsed. */
extern element *g_references;    /* List of link references found. */
extern element *g_notes;         /* List of footnotes found. */
extern element *g_parse_result;  /* Results of parse. */
extern int g_syntax_extensions;  /* Syntax extensions selected. */


#endif