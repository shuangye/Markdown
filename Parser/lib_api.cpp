/**********************************************************************

  markdown_lib.c - markdown in C using a PEG grammar.
  (c) 2008 John MacFarlane (jgm at berkeley dot edu).

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License or the MIT
  license.  See LICENSE for details.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

 ***********************************************************************/

#include "lib_api.h"
#include <string>
#include <clocale>
#include "parsings.h"
#include "output.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "types.h"
#ifdef __cplusplus
}
#endif

using std::string;


static void print_tree(element * elt, int indent);

// replace tabs with spaces
// CALLER is responsible to free the allocated heap memory
static string* preformat_text(char *text) {
	if (nullptr == text)
		return nullptr;
	
	const char* tab_expansion = "    ";  // tabstop = 4
	string *result = new string();

	char next_char;
	while ((next_char = *text++) != '\0') {
		if ('\t' == next_char) {
			result->append(tab_expansion);
			continue;
		}
		*result += next_char;		
	}

	result->append("\n\n");
	return result;
}


/* process_raw_blocks - traverses an element list, replacing any RAW elements with
 * the result of parsing them as markdown text, and recursing into the children
 * of parent elements.  The result should be a tree of elements without any RAWs. */
static element * process_raw_blocks(element *input, int extensions, element *references, element *notes) {
    element *current = NULL;
    element *last_child = NULL;
    char *contents;
    current = input;

    while (current != NULL) {
        if (current->key == RAW) {
            /* \001 is used to indicate boundaries between nested lists when there
             * is no blank line.  We split the string by \001 and parse
             * each chunk separately. */
            contents = strtok(current->contents.str, "\001");
            current->key = LIST;
            current->children = parse_markdown(contents, extensions, references, notes);
            last_child = current->children;
            while ((contents = strtok(NULL, "\001"))) {
                while (last_child->next != NULL)
                    last_child = last_child->next;
                last_child->next = parse_markdown(contents, extensions, references, notes);
            }
            free(current->contents.str);
            current->contents.str = NULL;
        }
        if (current->children != NULL)
            current->children = process_raw_blocks(current->children, extensions, references, notes);
        current = current->next;
    }
    return input;
}

string* markdown_to_cpp_string(char *text, int extensions, int output_format) {
	string* out;
	string* preformatted_text;
	char *preformatted_text_c;
	element *parsing_result;
	element *references;
	element *notes;
	
	preformatted_text = preformat_text(text);
	preformatted_text_c = const_cast<char*>(preformatted_text->c_str());

	references = parse_references(preformatted_text_c, extensions);
	notes = parse_notes(preformatted_text_c, extensions, references);
	parsing_result = parse_markdown(preformatted_text_c, extensions, references, notes);
	parsing_result = process_raw_blocks(parsing_result, extensions, references, notes);

	// free resource ASAP to moderate memory pressure
	delete preformatted_text;
	preformatted_text = nullptr;
	preformatted_text_c = nullptr;

	out = new string();  // allocate memory; to be freed by caller

#ifdef _DEBUG
	print_tree(parsing_result, 10);
#endif

	print_element_list(out, parsing_result, output_format, extensions);

	// free resource ASAP
	free_element_list(parsing_result);
	free_element_list(notes);
	free_element_list(references);

	return out;
}

// consider moving the buffer to global, thus to avoid allocating it every time when calling this func
char* markdown_to_string(char *text, int extensions, int output_format) {		
    string* cpp_str_result = markdown_to_cpp_string(text, extensions, output_format);	
	char *result = _strdup(cpp_str_result->c_str());  // create a copy

	delete cpp_str_result;
	cpp_str_result = nullptr;

    return result;
}


//////////////////////////////////////////////////////////////////////
/// Misc
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG

/// for debugging only
/// print_tree - print tree of elements
static void print_tree(element * elt, int indent) {
	int i;
	char * key;
	while (elt != NULL) {
		for (i = 0; i < indent; i++)
			fputc(' ', stderr);
		switch (elt->key) {
		case LIST:               key = "LIST"; break;
		case RAW:                key = "RAW"; break;
		case SPACE:              key = "SPACE"; break;
		case LINEBREAK:          key = "LINEBREAK"; break;
		case ELLIPSIS:           key = "ELLIPSIS"; break;
		case EMDASH:             key = "EMDASH"; break;
		case ENDASH:             key = "ENDASH"; break;
		case APOSTROPHE:         key = "APOSTROPHE"; break;
		case SINGLEQUOTED:       key = "SINGLEQUOTED"; break;
		case DOUBLEQUOTED:       key = "DOUBLEQUOTED"; break;
		case STR:                key = "STR"; break;
		case LINK:               key = "LINK"; break;
		case IMAGE:              key = "IMAGE"; break;
		case CODE:               key = "CODE"; break;
		case HTML:               key = "HTML"; break;
		case EMPH:               key = "EMPH"; break;
		case STRONG:             key = "STRONG"; break;
		case PLAIN:              key = "PLAIN"; break;
		case PARA:               key = "PARA"; break;
		case LISTITEM:           key = "LISTITEM"; break;
		case BULLETLIST:         key = "BULLETLIST"; break;
		case ORDEREDLIST:        key = "ORDEREDLIST"; break;
		case H1:                 key = "H1"; break;
		case H2:                 key = "H2"; break;
		case H3:                 key = "H3"; break;
		case H4:                 key = "H4"; break;
		case H5:                 key = "H5"; break;
		case H6:                 key = "H6"; break;
		case BLOCKQUOTE:         key = "BLOCKQUOTE"; break;
		case VERBATIM:           key = "VERBATIM"; break;
		case HTMLBLOCK:          key = "HTMLBLOCK"; break;
		case HRULE:              key = "HRULE"; break;
		case REFERENCE:          key = "REFERENCE"; break;
		case NOTE:               key = "NOTE"; break;
		default:                 key = "?";
		}
		if (elt->key == STR) {
			fprintf(stderr, "0x%p: %s   '%s'\n", (void *)elt, key, elt->contents.str);
		}
		else {
			fprintf(stderr, "0x%p: %s\n", (void *)elt, key);
		}
		if (elt->children)
			print_tree(elt->children, indent + 4);
		elt = elt->next;
	}
}

#endif
