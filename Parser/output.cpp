/// 
/// exposed APIs are declared in utilities.h
///

//#include <stdbool.h>
#include <cstdio>
#include <cassert>
#include <string>
#include <sstream>
#include <list>
#include "lib_api.h"
#include "odf.h"

// include C headers in C++
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include "types.h" 
#ifdef __cplusplus
}
#endif // __cplusplus

using std::string;
using std::list;
using std::ostringstream;
using std::endl;


static int extensions;
static int odf_type = 0;
static ostringstream *oss = nullptr;  // initialized in print_element_list

static void print_html_string(string *out, char *str, bool obfuscate);
static void print_html_element_list(string *out, element *list, bool obfuscate);
static void print_html_element(string *out, element *elt, bool obfuscate);
static void print_latex_string(string *out, char *str);
static void print_latex_element_list(string *out, element *list);
static void print_latex_element(string *out, element *elt);
static void print_groff_string(string *out, char *str);
static void print_groff_mm_element_list(string *out, element *list);
static void print_groff_mm_element(string *out, element *elt, int count);
static void print_odf_code_string(string *out, char *str);
static void print_odf_string(string *out, char *str);
static void print_odf_element_list(string *out, element *list);
static void print_odf_element(string *out, element *elt);
static bool list_contains_key(element *list, int key);


/**********************************************************************

  Utility functions for printing

 ***********************************************************************/

static int padded = 2;      /* Number of newlines after last output.
                               Starts at 2 so no newlines are needed at start.
                               */

// static GSList *endnotes = NULL; /* List of endnotes to print after main content. */
static list<element*> endnotes;
static int notenumber = 0;  /* Number of footnote. */

/* pad - add newlines if needed */
static void pad(string *out, int num) {
	if (nullptr == out)
		return;

	while (num-- > padded)
		*out += '\n';
    padded = num;
}

/* determine whether a certain element is contained within a given list */
static bool list_contains_key(element *list, int key) {
    element *step = NULL;

    step = list;
    while ( step != NULL ) {
        if (step->key == key) {
            return true;
        }
        if (step->children != NULL) {
            if (list_contains_key(step->children, key)) {
                return true;
            }
        }
       step = step->next;
    }
    return false;
}

/**********************************************************************

  Functions for printing Elements as HTML

 ***********************************************************************/

/* print_html_string - print string, escaping for HTML  
 * If obfuscate selected, convert characters to hex or decimal entities at random */
static void print_html_string(string *out, char *str, bool obfuscate) {
	if (nullptr == out || nullptr == str)
		return;

	while (*str != '\0') {
        switch (*str) {
        case '&':
            // g_string_append_printf(out, "&amp;");
			out->append("&amp;");
            break;
        case '<':
            // g_string_append_printf(out, "&lt;");
			out->append("&lt;");
            break;
        case '>':
            // g_string_append_printf(out, "&gt;");
			out->append("&gt;");
            break;
        case '"':
            // g_string_append_printf(out, "&quot;");
			out->append("&quot;");
            break;
        default:
			if (obfuscate && ((int)*str < 128) && ((int)*str >= 0)){
				if (rand() % 2 == 0) {
					//g_string_append_printf(out, "&#%d;", (int)*str);
					oss->str(std::string());  // clear the stream
					*oss << "&#" << (int)*str;
					out->append(oss->str());
				}
				else {
					// g_string_append_printf(out, "&#x%x;", (unsigned int)*str);
					oss->str(std::string());  // clear the stream
					*oss << "&#x" << std::hex << (unsigned int)*str;
					out->append(oss->str());
				}
			}
			else
				// g_string_append_c(out, *str);
				out->append(1, *str);
        }
    str++;
    }
}

/* print_html_element_list - print a list of elements as HTML */
static void print_html_element_list(string *out, element *list, bool obfuscate) {
    while (list != NULL) {
        print_html_element(out, list, obfuscate);
        list = list->next;
    }
}

/* add_endnote - add an endnote to global endnotes list. */
static void add_endnote(element *elt) {
    // endnotes = g_slist_prepend(endnotes, elt);
	endnotes.push_back(elt);
}

/* print_html_element - print an element as HTML */
static void print_html_element(string *out, element *elt, bool obfuscate) {
    int lev;
    switch (elt->key) {
    case SPACE:
        // g_string_append_printf(out, "%s", elt->contents.str);
		out->append(elt->contents.str);
        break;
    case LINEBREAK:
        // g_string_append_printf(out, "<br/>\n");
		out->append("<br/>\n");
        break;
    case STR:
        print_html_string(out, elt->contents.str, obfuscate);
        break;
    case ELLIPSIS:
        // g_string_append_printf(out, "&hellip;");
		out->append("&hellip;");
        break;
    case EMDASH:
        // g_string_append_printf(out, "&mdash;");
		out->append("&mdash;");
        break;
    case ENDASH:
        // g_string_append_printf(out, "&ndash;");
		out->append("&ndash;");
        break;
    case APOSTROPHE:
        // g_string_append_printf(out, "&rsquo;");
		out->append("&rsquo;");
        break;
    case SINGLEQUOTED:
        // g_string_append_printf(out, "&lsquo;");
		out->append("&lsquo;");
        print_html_element_list(out, elt->children, obfuscate);
        // g_string_append_printf(out, "&rsquo;");
		out->append("&rsquo;");
        break;
    case DOUBLEQUOTED:
        // g_string_append_printf(out, "&ldquo;");
		out->append("&ldquo;");
        print_html_element_list(out, elt->children, obfuscate);
        // g_string_append_printf(out, "&rdquo;");
		out->append("&rdquo;");
        break;
    case CODE:
        // g_string_append_printf(out, "<code>");
		out->append("<code>");
        print_html_string(out, elt->contents.str, obfuscate);
        // g_string_append_printf(out, "</code>");
		out->append("</code>");
        break;
    case HTML:
        // g_string_append_printf(out, "%s", elt->contents.str);
		out->append(elt->contents.str);
        break;
    case LINK:
        if (strstr(elt->contents.link->url, "mailto:") == elt->contents.link->url)
            obfuscate = true;  /* obfuscate mailto: links */
        // g_string_append_printf(out, "<a href=\"");
		out->append("<a href=\"");
        print_html_string(out, elt->contents.link->url, obfuscate);
        // g_string_append_printf(out, "\"");
		out->append("\"");
        if (strlen(elt->contents.link->title) > 0) {
            // g_string_append_printf(out, " title=\"");
			out->append(" title=\"");
            print_html_string(out, elt->contents.link->title, obfuscate);
            // g_string_append_printf(out, "\"");
			out->append("\"");
        }
        // g_string_append_printf(out, ">");
		out->append(">");
        print_html_element_list(out, elt->contents.link->label, obfuscate);
        // g_string_append_printf(out, "</a>");
		out->append("</a>");
        break;
    case IMAGE:
        // g_string_append_printf(out, "<img src=\"");
		out->append("<img src=\"");
        print_html_string(out, elt->contents.link->url, obfuscate);
        // g_string_append_printf(out, "\" alt=\"");
		out->append("\" alt=\"");
        print_html_element_list(out, elt->contents.link->label, obfuscate);
        // g_string_append_printf(out, "\"");
		out->append("\"");
        if (strlen(elt->contents.link->title) > 0) {
            /// g_string_append_printf(out, " title=\"");
			out->append(" title=\"");
            print_html_string(out, elt->contents.link->title, obfuscate);
            // g_string_append_printf(out, "\"");
			out->append("\"");
        }
        // g_string_append_printf(out, " />");
		out->append(" />");
        break;
    case EMPH:
        // g_string_append_printf(out, "<em>");
		out->append("<em>");
        print_html_element_list(out, elt->children, obfuscate);
        // g_string_append_printf(out, "</em>");
		out->append("</em>");
        break;
    case STRONG:
        // g_string_append_printf(out, "<strong>");
		out->append("<strong>");
        print_html_element_list(out, elt->children, obfuscate);
        // g_string_append_printf(out, "</strong>");
		out->append("</strong>");
        break;
    case STRIKE:
        // g_string_append_printf(out, "<del>");
		out->append("<del>");
        print_html_element_list(out, elt->children, obfuscate);
        // g_string_append_printf(out, "</del>");
		out->append("</del>");
        break;
    case LIST:
        print_html_element_list(out, elt->children, obfuscate);
        break;
    case RAW:
        /* Shouldn't occur - these are handled by process_raw_blocks() */
        assert(elt->key != RAW);
        break;
    case H1: case H2: case H3: case H4: case H5: case H6:
        lev = elt->key - H1 + 1;  /* assumes H1 ... H6 are in order */
        pad(out, 2);
        // g_string_append_printf(out, "<h%1d>", lev);
		oss->str(string());
		*oss << "<h" << lev << ">";		
		out->append(oss->str());
        print_html_element_list(out, elt->children, obfuscate);

        // g_string_append_printf(out, "</h%1d>", lev);
		oss->str(string());
		*oss << "</h" << lev << ">";
		out->append(oss->str());
		
        padded = 0;
        break;
    case PLAIN:
        pad(out, 1);
        print_html_element_list(out, elt->children, obfuscate);
        padded = 0;
        break;
    case PARA:
        pad(out, 2);
        // g_string_append_printf(out, "<p>");
		out->append("<p>");
        print_html_element_list(out, elt->children, obfuscate);
        // g_string_append_printf(out, "</p>");
		out->append("</p>");
        padded = 0;
        break;
    case HRULE:
        pad(out, 2);
        // g_string_append_printf(out, "<hr />");
		out->append("<hr />");
        padded = 0;
        break;
    case HTMLBLOCK:
        pad(out, 2);
        // g_string_append_printf(out, "%s", elt->contents.str);
		out->append(elt->contents.str);
        padded = 0;
        break;
    case VERBATIM:
        pad(out, 2);
        // g_string_append_printf(out, "%s", "<pre><code>");
		out->append("<pre><code>");
        print_html_string(out, elt->contents.str, obfuscate);
        // g_string_append_printf(out, "%s", "</code></pre>");
		out->append("</code></pre>");
        padded = 0;
        break;
    case BULLETLIST:
        pad(out, 2);
        // g_string_append_printf(out, "%s", "<ul>");
		out->append("<ul>");
        padded = 0;
        print_html_element_list(out, elt->children, obfuscate);
        pad(out, 1);
        // g_string_append_printf(out, "%s", "</ul>");
		out->append("</ul>");
        padded = 0;
        break;
    case ORDEREDLIST:
        pad(out, 2);
        // g_string_append_printf(out, "%s", "<ol>");
		out->append("<ol>");
        padded = 0;
        print_html_element_list(out, elt->children, obfuscate);
        pad(out, 1);
        // g_string_append_printf(out, "</ol>");
		out->append("</ol>");
        padded = 0;
        break;
    case LISTITEM:
        pad(out, 1);
        // g_string_append_printf(out, "<li>");
		out->append("<li>");
        padded = 2;
        print_html_element_list(out, elt->children, obfuscate);
        // g_string_append_printf(out, "</li>");
		out->append("</li>");
        padded = 0;
        break;
    case BLOCKQUOTE:
        pad(out, 2);
        // g_string_append_printf(out, "<blockquote>\n");
		out->append("<blockquote>\n");
        padded = 2;
        print_html_element_list(out, elt->children, obfuscate);
        pad(out, 1);
        // g_string_append_printf(out, "</blockquote>");
		out->append("</blockquote>");
        padded = 0;
        break;
    case REFERENCE:
        /* Nonprinting */
        break;
    case NOTE:
        /* if contents.str == 0, then print note; else ignore, since this
         * is a note block that has been incorporated into the notes list */
        if (elt->contents.str == 0) {
            add_endnote(elt);
            ++notenumber;
            // g_string_append_printf(out, "<a class=\"noteref\" id=\"fnref%d\" href=\"#fn%d\" title=\"Jump to note %d\">[%d]</a>",
            //    notenumber, notenumber, notenumber, notenumber);			
			oss->str(string());
			*oss << "<a class=\"noteref\" id=\"fnref" << notenumber << "\" href=\"#fn" << notenumber << 
				"\" title=\"Jump to note " << notenumber << "\">[" << notenumber << "]< / a>";
			out->append(oss->str());			
        }
        break;
    default: 
        fprintf(stderr, "print_html_element encountered unknown element key = %d\n", elt->key); 
        exit(EXIT_FAILURE);
    }
}

static void print_html_endnotes(string *out) {
	assert(nullptr != out);

    int counter = 0;
	out->append("<hr/>\n<ol id=\"notes\">");

	for (auto note : endnotes) {
		++counter;
		pad(out, 1);
		oss->str(std::string());
		*oss << "<li id=\"fn" << counter << "\">" << endl;
		out->append(oss->str());
		padded = 2;
		print_html_element_list(out, note->children, false);
		oss->str(std::string());
		*oss << " <a href=\"#fnref" << counter << "\" title=\"Jump back to reference\">[back]</a>";
		out->append(oss->str());
		pad(out, 1);
		out->append("</li>");
	}

	pad(out, 1);
	out->append("</ol>");    
}



#ifdef __FORMATS

/**********************************************************************

  Functions for printing Elements as LaTeX

 ***********************************************************************/

/* print_latex_string - print string, escaping for LaTeX */
static void print_latex_string(string *out, char *str) {
    while (*str != '\0') {
        switch (*str) {
          case '{': case '}': case '$': case '%':
          case '&': case '_': case '#':
            g_string_append_printf(out, "\\%c", *str);
            break;
        case '^':
            g_string_append_printf(out, "\\^{}");
            break;
        case '\\':
            g_string_append_printf(out, "\\textbackslash{}");
            break;
        case '~':
            g_string_append_printf(out, "\\ensuremath{\\sim}");
            break;
        case '|':
            g_string_append_printf(out, "\\textbar{}");
            break;
        case '<':
            g_string_append_printf(out, "\\textless{}");
            break;
        case '>':
            g_string_append_printf(out, "\\textgreater{}");
            break;
        default:
            g_string_append_c(out, *str);
        }
    str++;
    }
}

/* print_latex_element_list - print a list of elements as LaTeX */
static void print_latex_element_list(string *out, element *list) {
    while (list != NULL) {
        print_latex_element(out, list);
        list = list->next;
    }
}

/* print_latex_element - print an element as LaTeX */
static void print_latex_element(string *out, element *elt) {
    int lev;
    int i;
    switch (elt->key) {
    case SPACE:
        g_string_append_printf(out, "%s", elt->contents.str);
        break;
    case LINEBREAK:
        g_string_append_printf(out, "\\\\\n");
        break;
    case STR:
        print_latex_string(out, elt->contents.str);
        break;
    case ELLIPSIS:
        g_string_append_printf(out, "\\ldots{}");
        break;
    case EMDASH: 
        g_string_append_printf(out, "---");
        break;
    case ENDASH: 
        g_string_append_printf(out, "--");
        break;
    case APOSTROPHE:
        g_string_append_printf(out, "'");
        break;
    case SINGLEQUOTED:
        g_string_append_printf(out, "`");
        print_latex_element_list(out, elt->children);
        g_string_append_printf(out, "'");
        break;
    case DOUBLEQUOTED:
        g_string_append_printf(out, "``");
        print_latex_element_list(out, elt->children);
        g_string_append_printf(out, "''");
        break;
    case CODE:
        g_string_append_printf(out, "\\texttt{");
        print_latex_string(out, elt->contents.str);
        g_string_append_printf(out, "}");
        break;
    case HTML:
        /* don't print HTML */
        break;
    case LINK:
        g_string_append_printf(out, "\\href{%s}{", elt->contents.link->url);
        print_latex_element_list(out, elt->contents.link->label);
        g_string_append_printf(out, "}");
        break;
    case IMAGE:
        g_string_append_printf(out, "\\includegraphics{%s}", elt->contents.link->url);
        break;
    case EMPH:
        g_string_append_printf(out, "\\emph{");
        print_latex_element_list(out, elt->children);
        g_string_append_printf(out, "}");
        break;
    case STRONG:
        g_string_append_printf(out, "\\textbf{");
        print_latex_element_list(out, elt->children);
        g_string_append_printf(out, "}");
        break;
    case STRIKE:
        g_string_append_printf(out, "\\sout{");
        print_latex_element_list(out, elt->children);
        g_string_append_printf(out, "}");
        break;
    case LIST:
        print_latex_element_list(out, elt->children);
        break;
    case RAW:
        /* Shouldn't occur - these are handled by process_raw_blocks() */
        assert(elt->key != RAW);
        break;
    case H1: case H2: case H3:
        pad(out, 2);
        lev = elt->key - H1 + 1;  /* assumes H1 ... H6 are in order */
        g_string_append_printf(out, "\\");
        for (i = elt->key; i > H1; i--)
            g_string_append_printf(out, "sub");
        g_string_append_printf(out, "section{");
        print_latex_element_list(out, elt->children);
        g_string_append_printf(out, "}");
        padded = 0;
        break;
    case H4: case H5: case H6:
        pad(out, 2);
        g_string_append_printf(out, "\\noindent\\textbf{");
        print_latex_element_list(out, elt->children);
        g_string_append_printf(out, "}");
        padded = 0;
        break;
    case PLAIN:
        pad(out, 1);
        print_latex_element_list(out, elt->children);
        padded = 0;
        break;
    case PARA:
        pad(out, 2);
        print_latex_element_list(out, elt->children);
        padded = 0;
        break;
    case HRULE:
        pad(out, 2);
        g_string_append_printf(out, "\\begin{center}\\rule{3in}{0.4pt}\\end{center}\n");
        padded = 0;
        break;
    case HTMLBLOCK:
        /* don't print HTML block */
        break;
    case VERBATIM:
        pad(out, 1);
        g_string_append_printf(out, "\\begin{verbatim}\n");
        print_latex_string(out, elt->contents.str);
        g_string_append_printf(out, "\n\\end{verbatim}");
        padded = 0;
        break;
    case BULLETLIST:
        pad(out, 1);
        g_string_append_printf(out, "\\begin{itemize}");
        padded = 0;
        print_latex_element_list(out, elt->children);
        pad(out, 1);
        g_string_append_printf(out, "\\end{itemize}");
        padded = 0;
        break;
    case ORDEREDLIST:
        pad(out, 1);
        g_string_append_printf(out, "\\begin{enumerate}");
        padded = 0;
        print_latex_element_list(out, elt->children);
        pad(out, 1);
        g_string_append_printf(out, "\\end{enumerate}");
        padded = 0;
        break;
    case LISTITEM:
        pad(out, 1);
        g_string_append_printf(out, "\\item ");
        padded = 2;
        print_latex_element_list(out, elt->children);
        g_string_append_printf(out, "\n");
        break;
    case BLOCKQUOTE:
        pad(out, 1);
        g_string_append_printf(out, "\\begin{quote}");
        padded = 0;
        print_latex_element_list(out, elt->children);
        pad(out, 1);
        g_string_append_printf(out, "\\end{quote}");
        padded = 0;
        break;
    case NOTE:
        /* if contents.str == 0, then print note; else ignore, since this
         * is a note block that has been incorporated into the notes list */
        if (elt->contents.str == 0) {
            g_string_append_printf(out, "\\footnote{");
            padded = 2;
            print_latex_element_list(out, elt->children);
            g_string_append_printf(out, "}");
            padded = 0; 
        }
        break;
    case REFERENCE:
        /* Nonprinting */
        break;
    default: 
        fprintf(stderr, "print_latex_element encountered unknown element key = %d\n", elt->key); 
        exit(EXIT_FAILURE);
    }
}

/**********************************************************************

  Functions for printing Elements as groff (mm macros)

 ***********************************************************************/

static bool in_list_item = false; /* True if we're parsing contents of a list item. */

/* print_groff_string - print string, escaping for groff */
static void print_groff_string(string *out, char *str) {
    /* escape dots if it is the first character */
    if (*str == '.') {
        g_string_append_printf(out, "\\[char46]");
        str++;
    }

    while (*str != '\0') {
        switch (*str) {
        case '\\':
            g_string_append_printf(out, "\\e");
            break;
        default:
            g_string_append_c(out, *str);
        }
    str++;
    }
}

/* print_groff_mm_element_list - print a list of elements as groff ms */
static void print_groff_mm_element_list(string *out, element *list) {
    int count = 1;
    while (list != NULL) {
        print_groff_mm_element(out, list, count);
        list = list->next;
        count++;
    }
}

/* print_groff_mm_element - print an element as groff ms */
static void print_groff_mm_element(string *out, element *elt, int count) {
    int lev;
    switch (elt->key) {
    case SPACE:
        g_string_append_printf(out, "%s", elt->contents.str);
        padded = 0;
        break;
    case LINEBREAK:
        pad(out, 1);
        g_string_append_printf(out, ".br\n");
        padded = 0;
        break;
    case STR:
        print_groff_string(out, elt->contents.str);
        padded = 0;
        break;
    case ELLIPSIS:
        g_string_append_printf(out, "...");
        break;
    case EMDASH:
        g_string_append_printf(out, "\\[em]");
        break;
    case ENDASH:
        g_string_append_printf(out, "\\[en]");
        break;
    case APOSTROPHE:
        g_string_append_printf(out, "'");
        break;
    case SINGLEQUOTED:
        g_string_append_printf(out, "`");
        print_groff_mm_element_list(out, elt->children);
        g_string_append_printf(out, "'");
        break;
    case DOUBLEQUOTED:
        g_string_append_printf(out, "\\[lq]");
        print_groff_mm_element_list(out, elt->children);
        g_string_append_printf(out, "\\[rq]");
        break;
    case CODE:
        g_string_append_printf(out, "\\fC");
        print_groff_string(out, elt->contents.str);
        g_string_append_printf(out, "\\fR");
        padded = 0;
        break;
    case HTML:
        /* don't print HTML */
        break;
    case LINK:
        print_groff_mm_element_list(out, elt->contents.link->label);
        g_string_append_printf(out, " (%s)", elt->contents.link->url);
        padded = 0;
        break;
    case IMAGE:
        g_string_append_printf(out, "[IMAGE: ");
        print_groff_mm_element_list(out, elt->contents.link->label);
        g_string_append_printf(out, "]");
        padded = 0;
        /* not supported */
        break;
    case EMPH:
        g_string_append_printf(out, "\\fI");
        print_groff_mm_element_list(out, elt->children);
        g_string_append_printf(out, "\\fR");
        padded = 0;
        break;
    case STRONG:
        g_string_append_printf(out, "\\fB");
        print_groff_mm_element_list(out, elt->children);
        g_string_append_printf(out, "\\fR");
        padded = 0;
        break;
    case STRIKE:
        g_string_append_printf(out, "\\c\n.ST \"");
        print_groff_mm_element_list(out, elt->children);
        g_string_append_printf(out, "\"");
        pad(out, 1);
        break;
    case LIST:
        print_groff_mm_element_list(out, elt->children);
        padded = 0;
        break;
    case RAW:
        /* Shouldn't occur - these are handled by process_raw_blocks() */
        assert(elt->key != RAW);
        break;
    case H1: case H2: case H3: case H4: case H5: case H6:
        lev = elt->key - H1 + 1;
        pad(out, 1);
        g_string_append_printf(out, ".H %d \"", lev);
        print_groff_mm_element_list(out, elt->children);
        g_string_append_printf(out, "\"");
        padded = 0;
        break;
    case PLAIN:
        pad(out, 1);
        print_groff_mm_element_list(out, elt->children);
        padded = 0;
        break;
    case PARA:
        pad(out, 1);
        if (!in_list_item || count != 1)
            g_string_append_printf(out, ".P\n");
        print_groff_mm_element_list(out, elt->children);
        padded = 0;
        break;
    case HRULE:
        pad(out, 1);
        g_string_append_printf(out, "\\l'\\n(.lu*8u/10u'");
        padded = 0;
        break;
    case HTMLBLOCK:
        /* don't print HTML block */
        break;
    case VERBATIM:
        pad(out, 1);
        g_string_append_printf(out, ".VERBON 2\n");
        print_groff_string(out, elt->contents.str);
        g_string_append_printf(out, ".VERBOFF");
        padded = 0;
        break;
    case BULLETLIST:
        pad(out, 1);
        g_string_append_printf(out, ".BL");
        padded = 0;
        print_groff_mm_element_list(out, elt->children);
        pad(out, 1);
        g_string_append_printf(out, ".LE 1");
        padded = 0;
        break;
    case ORDEREDLIST:
        pad(out, 1);
        g_string_append_printf(out, ".AL");
        padded = 0;
        print_groff_mm_element_list(out, elt->children);
        pad(out, 1);
        g_string_append_printf(out, ".LE 1");
        padded = 0;
        break;
    case LISTITEM:
        pad(out, 1);
        g_string_append_printf(out, ".LI\n");
        in_list_item = true;
        padded = 2;
        print_groff_mm_element_list(out, elt->children);
        in_list_item = false;
        break;
    case BLOCKQUOTE:
        pad(out, 1);
        g_string_append_printf(out, ".DS I\n");
        padded = 2;
        print_groff_mm_element_list(out, elt->children);
        pad(out, 1);
        g_string_append_printf(out, ".DE");
        padded = 0;
        break;
    case NOTE:
        /* if contents.str == 0, then print note; else ignore, since this
         * is a note block that has been incorporated into the notes list */
        if (elt->contents.str == 0) {
            g_string_append_printf(out, "\\*F\n");
            g_string_append_printf(out, ".FS\n");
            padded = 2;
            print_groff_mm_element_list(out, elt->children);
            pad(out, 1);
            g_string_append_printf(out, ".FE\n");
            padded = 1; 
        }
        break;
    case REFERENCE:
        /* Nonprinting */
        break;
    default: 
        fprintf(stderr, "print_groff_mm_element encountered unknown element key = %d\n", elt->key); 
        exit(EXIT_FAILURE);
    }
}

/**********************************************************************

  Functions for printing Elements as ODF

 ***********************************************************************/

/* print_odf_code_string - print string, escaping for HTML and saving newlines 
*/
static void print_odf_code_string(string *out, char *str) {
    char *tmp;
    while (*str != '\0') {
        switch (*str) {
        case '&':
            g_string_append_printf(out, "&amp;");
            break;
        case '<':
            g_string_append_printf(out, "&lt;");
            break;
        case '>':
            g_string_append_printf(out, "&gt;");
            break;
        case '"':
            g_string_append_printf(out, "&quot;");
            break;
        case '\n':
            g_string_append_printf(out, "<text:line-break/>");
            break;
        case ' ':
            tmp = str;
            tmp++;
            if (*tmp == ' ') {
                tmp++;
                if (*tmp == ' ') {
                    tmp++;
                    if (*tmp == ' ') {
                        g_string_append_printf(out, "<text:tab/>");
                        str = tmp;
                    } else {
                        g_string_append_printf(out, " ");
                    }
                } else {
                    g_string_append_printf(out, " ");
                }
            } else {
                g_string_append_printf(out, " ");
            }
            break;
        default:
               g_string_append_c(out, *str);
        }
    str++;
    }
}

/* print_odf_string - print string, escaping for HTML and saving newlines */
static void print_odf_string(string *out, char *str) {
    char *tmp;
    while (*str != '\0') {
        switch (*str) {
        case '&':
            g_string_append_printf(out, "&amp;");
            break;
        case '<':
            g_string_append_printf(out, "&lt;");
            break;
        case '>':
            g_string_append_printf(out, "&gt;");
            break;
        case '"':
            g_string_append_printf(out, "&quot;");
            break;
        case '\n':
            tmp = str;
            tmp--;
            if (*tmp == ' ') {
                tmp--;
                if (*tmp == ' ') {
                    g_string_append_printf(out, "<text:line-break/>");
                } else {
                    g_string_append_printf(out, "\n");
                }
            } else {
                g_string_append_printf(out, "\n");
            }
            break;
        case ' ':
            tmp = str;
            tmp++;
            if (*tmp == ' ') {
                tmp++;
                if (*tmp == ' ') {
                    tmp++;
                    if (*tmp == ' ') {
                        g_string_append_printf(out, "<text:tab/>");
                        str = tmp;
                    } else {
                        g_string_append_printf(out, " ");
                    }
                } else {
                    g_string_append_printf(out, " ");
                }
            } else {
                g_string_append_printf(out, " ");
            }
            break;
        default:
               g_string_append_c(out, *str);
        }
    str++;
    }
}

/* print_odf_element_list - print an element list as ODF */
static void print_odf_element_list(string *out, element *list) {
    while (list != NULL) {
        print_odf_element(out, list);
        list = list->next;
    }
}

/* print_odf_element - print an element as ODF */
static void print_odf_element(string *out, element *elt) {
    int lev;
    int old_type = 0;
    switch (elt->key) {
    case SPACE:
        g_string_append_printf(out, "%s", elt->contents.str);
        break;
    case LINEBREAK:
        g_string_append_printf(out, "<text:line-break/>");
        break;
    case STR:
        print_html_string(out, elt->contents.str, 0);
        break;
    case ELLIPSIS:
        g_string_append_printf(out, "&hellip;");
        break;
    case EMDASH:
        g_string_append_printf(out, "&mdash;");
        break;
    case ENDASH:
        g_string_append_printf(out, "&ndash;");
        break;
    case APOSTROPHE:
        g_string_append_printf(out, "&rsquo;");
        break;
    case SINGLEQUOTED:
        g_string_append_printf(out, "&lsquo;");
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "&rsquo;");
        break;
    case DOUBLEQUOTED:
        g_string_append_printf(out, "&ldquo;");
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "&rdquo;");
        break;
    case CODE:
        g_string_append_printf(out, "<text:span text:style-name=\"Source_20_Text\">");
        print_html_string(out, elt->contents.str, 0);
        g_string_append_printf(out, "</text:span>");
        break;
    case HTML:
        break;
    case LINK:
        g_string_append_printf(out, "<text:a xlink:type=\"simple\" xlink:href=\"");
        print_html_string(out, elt->contents.link->url, 0);
        g_string_append_printf(out, "\"");
        if (strlen(elt->contents.link->title) > 0) {
            g_string_append_printf(out, " office:name=\"");
            print_html_string(out, elt->contents.link->title, 0);
            g_string_append_printf(out, "\"");
        }
        g_string_append_printf(out, ">");
        print_odf_element_list(out, elt->contents.link->label);
        g_string_append_printf(out, "</text:a>");
        break;
    case IMAGE:
        g_string_append_printf(out, "<draw:frame text:anchor-type=\"as-char\"\ndraw:z-index=\"0\" draw:style-name=\"fr1\" svg:width=\"95%%\"");
        g_string_append_printf(out, ">\n<draw:text-box><text:p><draw:frame text:anchor-type=\"as-char\" draw:z-index=\"1\" ");
        g_string_append_printf(out, "><draw:image xlink:href=\"");
        print_odf_string(out, elt->contents.link->url);
        g_string_append_printf(out,"\" xlink:type=\"simple\" xlink:show=\"embed\" xlink:actuate=\"onLoad\" draw:filter-name=\"&lt;All formats&gt;\"/>\n</draw:frame></text:p>");
        g_string_append_printf(out, "</draw:text-box></draw:frame>\n");
        break;
    case EMPH:
        g_string_append_printf(out,
            "<text:span text:style-name=\"MMD-Italic\">");
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "</text:span>");
        break;
    case STRONG:
        g_string_append_printf(out,
            "<text:span text:style-name=\"MMD-Bold\">");
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "</text:span>");
        break;
    case STRIKE:
        g_string_append_printf(out,
            "<text:span text:style-name=\"StrikeThrough\">");
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "</text:span>");
        break;
    case LIST:
        print_odf_element_list(out, elt->children);
        break;
    case RAW:
        /* Shouldn't occur - these are handled by process_raw_blocks() */
        assert(elt->key != RAW);
        break;
    case H1: case H2: case H3: case H4: case H5: case H6:
        lev = elt->key - H1 + 1;  /* assumes H1 ... H6 are in order */
        g_string_append_printf(out, "<text:h text:outline-level=\"%d\">", lev);
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "</text:h>\n");
        padded = 0;
        break;
    case PLAIN:
        print_odf_element_list(out, elt->children);
        padded = 0;
        break;
    case PARA:
        g_string_append_printf(out, "<text:p");
        switch (odf_type) {
            case BLOCKQUOTE:
                g_string_append_printf(out," text:style-name=\"Quotations\"");
                break;
            case CODE:
                g_string_append_printf(out," text:style-name=\"Preformatted Text\"");
                break;
            case VERBATIM:
                g_string_append_printf(out," text:style-name=\"Preformatted Text\"");
                break;
            case ORDEREDLIST:
            case BULLETLIST:
                g_string_append_printf(out," text:style-name=\"P2\"");
                break;
            case NOTE:
                g_string_append_printf(out," text:style-name=\"Footnote\"");
                break;
            default:
                g_string_append_printf(out," text:style-name=\"Standard\"");
                break;
        }
        g_string_append_printf(out, ">");
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "</text:p>\n");
        break;
    case HRULE:
        g_string_append_printf(out,"<text:p text:style-name=\"Horizontal_20_Line\"/>\n");
        break;
    case HTMLBLOCK:
        /* don't print HTML block */
        /* but do print HTML comments for raw ODF */
        if (strncmp(elt->contents.str,"<!--",4) == 0) {
            /* trim "-->" from end */
            elt->contents.str[strlen(elt->contents.str)-3] = '\0';
            g_string_append_printf(out, "%s", &elt->contents.str[4]);
        }
        break;
    case VERBATIM:
        old_type = odf_type;
        odf_type = VERBATIM;
        g_string_append_printf(out, "<text:p text:style-name=\"Preformatted Text\">");
        print_odf_code_string(out, elt->contents.str);
        g_string_append_printf(out, "</text:p>\n");
        odf_type = old_type;
        break;
    case BULLETLIST:
        if ((odf_type == BULLETLIST) ||
            (odf_type == ORDEREDLIST)) {
            /* I think this was made unnecessary by another change.
            Same for ORDEREDLIST below */
            /*  g_string_append_printf(out, "</text:p>"); */
        }
        old_type = odf_type;
        odf_type = BULLETLIST;
        g_string_append_printf(out, "%s", "<text:list>");
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "%s", "</text:list>");
        odf_type = old_type;
        break;
    case ORDEREDLIST:
        if ((odf_type == BULLETLIST) ||
            (odf_type == ORDEREDLIST)) {
            /* g_string_append_printf(out, "</text:p>"); */
        }
        old_type = odf_type;
        odf_type = ORDEREDLIST;
        g_string_append_printf(out, "%s", "<text:list>\n");
        print_odf_element_list(out, elt->children);
        g_string_append_printf(out, "%s", "</text:list>\n");
        odf_type = old_type;
        break;
    case LISTITEM:
        g_string_append_printf(out, "<text:list-item>\n");
        if (elt->children->children->key != PARA) {
            g_string_append_printf(out, "<text:p text:style-name=\"P2\">");
        }
        print_odf_element_list(out, elt->children);

        if ((list_contains_key(elt->children,BULLETLIST) ||
            (list_contains_key(elt->children,ORDEREDLIST)))) {
            } else {
                if (elt->children->children->key != PARA) {
                    g_string_append_printf(out, "</text:p>");
                }
            }
        g_string_append_printf(out, "</text:list-item>\n");
        break;
    case BLOCKQUOTE:
        old_type = odf_type;
        odf_type = BLOCKQUOTE;
        print_odf_element_list(out, elt->children);
        odf_type = old_type;
        break;
    case REFERENCE:
        break;
    case NOTE:
        old_type = odf_type;
        odf_type = NOTE;
        /* if contents.str == 0 then print; else ignore - like above */
        if (elt->contents.str == 0) {
            g_string_append_printf(out, "<text:note text:id=\"\" text:note-class=\"footnote\"><text:note-body>\n");
            print_odf_element_list(out, elt->children);
            g_string_append_printf(out, "</text:note-body>\n</text:note>\n");
       }
        elt->children = NULL;
        odf_type = old_type;
        break;
        break;  default:
        fprintf(stderr, "print_odf_element encountered unknown element key = %d\n", elt->key);
        exit(EXIT_FAILURE);
    }
}



#else
// stubs
static void print_latex_string(string *out, char *str){}
static void print_latex_element_list(string *out, element *list){}
static void print_latex_element(string *out, element *elt){}
static void print_groff_string(string *out, char *str){}
static void print_groff_mm_element_list(string *out, element *list){}
static void print_groff_mm_element(string *out, element *elt, int count){}
static void print_odf_code_string(string *out, char *str){}
static void print_odf_string(string *out, char *str){}
static void print_odf_element_list(string *out, element *list){}
static void print_odf_element(string *out, element *elt){}
#endif


/**********************************************************************

  Parameterized function for printing an Element.

 ***********************************************************************/

void print_element_list(string *out, element *elt, int format, int exts) {    
    notenumber = 0;
    extensions = exts;
    padded = 2;  /* set padding to 2, so no extra blank lines at beginning */
	oss = new ostringstream();

    switch (format) {
    case HTML_FORMAT:
        print_html_element_list(out, elt, false);
        if (!endnotes.empty()) {
            pad(out, 2);
            print_html_endnotes(out);
        }
        break;
    case LATEX_FORMAT:
        print_latex_element_list(out, elt);
        break;
    case GROFF_MM_FORMAT:
        if (extensions & EXT_STRIKE) {
          // g_string_append_printf(out,
          //    ".de ST\n.nr width \\w'\\\\$1'\n\\Z@\\v'-.25m'\\l'\\\\n[width]u'@\\\\$1\\c\n..\n.\n");
			out->append(".de ST\n.nr width \\w'\\\\$1'\n\\Z@\\v'-.25m'\\l'\\\\n[width]u'@\\\\$1\\c\n..\n.\n");
        }
        print_groff_mm_element_list(out, elt);
        break;
    case ODF_FORMAT:
        print_odf_header(out);
        // g_string_append_printf(out, "<office:body>\n<office:text>\n");
		out->append("<office:body>\n<office:text>\n");
        if (elt != NULL) print_odf_element_list(out,elt);
        print_odf_footer(out);
        break;
    default:
        fprintf(stderr, "print_element - unknown format = %d\n", format); 
        exit(EXIT_FAILURE);
    }

	delete oss;
	oss = nullptr;
}
