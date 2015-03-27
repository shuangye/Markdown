#pragma once

#ifndef TYPES_H
#define TYPES_H

/// This intends to be a C header

#ifdef _WIN32
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
//#ifndef strdup
//#define strdup _strdup
//#endif
#endif

/* Information (label, URL and title) for a link. */
struct Link {
	struct Element   *label;
	char             *url;
	char             *title;
};

typedef struct Link link;

/* Union for contents of an Element (string, list, or link). */
union Contents {
	char             *str;
	struct Link      *link;
};

/* Types of semantic values returned by parsers. */
enum keys {
	LIST,   /* A generic list of values.  For ordered and bullet lists, see below. */
	RAW,    /* Raw markdown to be processed further */
	SPACE,
	LINEBREAK,
	ELLIPSIS,
	EMDASH,
	ENDASH,
	APOSTROPHE,
	SINGLEQUOTED,
	DOUBLEQUOTED,
	STR,
	LINK,
	IMAGE,
	CODE,
	HTML,
	EMPH,
	STRONG,
	STRIKE,
	PLAIN,
	PARA,
	LISTITEM,
	BULLETLIST,
	ORDEREDLIST,
	H1, H2, H3, H4, H5, H6,  /* Code assumes that these are in order. */
	BLOCKQUOTE,
	VERBATIM,
	HTMLBLOCK,
	HRULE,
	REFERENCE,
	NOTE
};

/* Semantic value of a parsing action. */
struct Element {
	int               key;
	union Contents    contents;
	struct Element    *children;
	struct Element    *next;
};

typedef struct Element element;


#endif  // TYPES_H
