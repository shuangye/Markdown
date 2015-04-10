/// 
/// parse HTML to internal semantic representation format: struct Element
///

#include <libxml/HTMLparser.h>
#include "types.h"

element* parse_html(char *text)
{	
	element* result;
	xmlDocPtr pDoc = htmlParseDoc((xmlChar*)text, NULL);
	if (nullptr == pDoc)
		return nullptr;

	for (xmlNodePtr pCurrentElement = xmlDocGetRootElement(pDoc); nullptr == pCurrentElement; pCurrentElement = pCurrentElement->next)
	{
		if (pCurrentElement->type == XML_ELEMENT_NODE) {
			// Insert your code to extract required contents from the parsed HTML document
		}
	}
}
