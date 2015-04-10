## Architecture

Input                                       Internal Semantic Representation                       Output
																                   
Markdown                               struct Element                                                   Latex
																                   
HTML                                                                                                               HTML
																                   
docx                                                                                                                 docx

ODF                                                                                                                  ODF


目前是用 C 写的。


## peg & leg

以下面一条 PEG 为例：

``` 
Doc =       BOM? a:StartList ( Block { a = cons($$, a); } )*
            { parse_result = reverse(a); }
```

peg 和 leg 程序这样处理 PEG 定义：

- 为 `Doc` 规则生成这样一个函数 `YY_RULE(int) yy_Doc(yycontext *yy);` 注意前面的宏 **YY_RULE** 标识了其类型；
- 大括号中的内容为 action, leg 程序为其生成这样两个函数 `YY_ACTION(void) yy_1_Doc(yycontext *yy, char *yytext, int yyleng)` 和 `YY_ACTION(void) yy_2_Doc(yycontext *yy, char *yytext, int yyleng)` 同样前面的宏 **YY_ACTION** 标识了其类型。

YY_PARSE(int) YYPARSE(YY_CTX_PARAM) 是对整个 PEG 解析行为的开始，它调用的是第一个规则对应的函数。在 peg 源代码提供的例子中， main 函数就是从它开始调用的。

## Conversions




## Mixing C and C++

### Precompiled Headers

Ref: http://support.microsoft.com/en-us/kb/126717

It is not valid to use a C++ compiler created precompiled header with a source file that is to be compiled by the C compiler. Nor is it valid to use a C compiler created precompiled header with a source file that is to be compiled by the C++ compiler.

