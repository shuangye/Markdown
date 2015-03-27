/// exposed API of this lib
/// exposed as "C" convention

#pragma once
#ifndef LIB_API_H
#define LIB_API_H

#ifdef _WIN32
#ifdef MARKDOWN_LIB_API_EXPORT
#define MARKDOWN_LIB_API __declspec(dllexport) 
#else
#define MARKDOWN_LIB_API __declspec(dllimport) 
#endif
#endif  // _WIN32

// #include <string>
// using std::string;
// class string;

#ifdef __cplusplus
extern "C" {
#endif
		
	enum markdown_extensions {
		EXT_SMART = 0x01,
		EXT_NOTES = 0x02,
		EXT_FILTER_HTML = 0x04,
		EXT_FILTER_STYLES = 0x08,
		EXT_STRIKE = 0x10
	};
		
	enum markdown_formats {
		HTML_FORMAT = 0,
		LATEX_FORMAT,
		GROFF_MM_FORMAT,
		ODF_FORMAT
	};

	// MARKDOWN_LIB_API
	// string* markdown_to_cpp_string(char *text, int extensions, int output_format);

	MARKDOWN_LIB_API
	char* markdown_to_string(char *text, int extensions, int output_format);

#ifdef __cplusplus
}
#endif

#endif  // LIB_API_H
