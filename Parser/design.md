lib_api.h    //  exposed API

markdown_peg.h  // type definitions & overall function declarations

parsing_functions.h

utilities.h

odf.h


## Mixing C and C++

### Precompiled Headers

Ref: http://support.microsoft.com/en-us/kb/126717

It is not valid to use a C++ compiler created precompiled header with a source file that is to be compiled by the C compiler. Nor is it valid to use a C compiler created precompiled header with a source file that is to be compiled by the C++ compiler.

