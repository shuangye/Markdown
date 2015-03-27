# HTML & Markdown

HTML: inline, block, inline-block, float.

Markdown: only block and inline (span).

解析成一个与语言、格式无关的 DOM 抽象数据结构：

* 这个 DOM 位于中心，可转换为任何具体的 representation, 如 Markdown, HTML, Word.
* 这个 DOM 可以支持增量更新，如用户只编辑了部分节点，则最终的 representation 也只需要更新相应的部分。


# Design

Able to convert between Markdown, HTML, and docx.

Export to PDF.

First level: block.

Keep 2 lines in scope: one is the current line, and the other is the next line.

On line processing: parse while splitting.

Block 可以嵌套 block 和 inline; inline 只能嵌套 inline.

## Blocks

Setext-style Heading 1
==============

Setext-style Heading 2
-----------------------

Atx-style headers supports heading 1 - 6 using hash characters. Optionally, you can close the header using (even a different number of) hash characters.

### Atx-style Heading 3 ######################

Markdown supports "hard-wrapped" text paragraphs. A line break character in a paragraph will not be translated to a `</br>` tag.
When you *do* want to insert a `<br />` break tag using Markdown, you end a line with two or more spaces, then type return.     
Paragraphs are separated by >= 1 blank lines. A blank line is a line that contain only white space characters. Normal paragraphs should not be indented.

>Markdown uses email-style `>` characters for blockquoting.
>It looks best if you hard wrap the text and put a `>` before every line.

>Markdown allows you to be lazy and only put the `>` before the first line of a hard-wrapped paragraph:


Blockquotes can be nested (i.e. a blockquote-in-a-blockquote) by adding additional levels of >:
> quote level 1. 
>> quote level 2.
>
> back to the first level.

Blockquotes can contain other Markdown elements, including headers, lists, and code blocks:

> ## Quoted header.
> 
> 1.   This is the first list item.
> 1.   This is the second list item.
> 
> Here's some example `code`:
> 
>     return shell_exec("echo $input | $markdown_script");

Unordered list begins with *, -, or +

- unordered list 1
+ unordered list 2
* unordered list 3

Ordered lists use numbers followed by periods. The actual numbers you use to mark the list have no effect on the HTML output Markdown produces:

3. Bird
1. McHale
8. Parish


## Spans

`__Nested__`

__`Nested`__




## Indent

像 Python 一样严格。

四个空格产生一个 indent.

开头空格 < 4 个，不产生任何意义。 




# References

## Syntax

http://daringfireball.net/projects/markdown/syntax


## Parsedown

https://github.com/erusev/parsedown

How does Parsedown work?

It splits text into lines. Then it looks at how these lines start and relate to each other

It tries to read Markdown like a human. First, it looks at the lines. It’s interested in how the lines start. This helps it recognise blocks. It knows, for example, that if a line start with a - then it perhaps belong to a list. Once it recognises the blocks, it continues to the content. As it reads, it watches out for special characters. This helps it recognise inline elements (or inlines).

We call this approach "line based". We believe that Parsedown is the first Markdown parser to use it. Since the release of Parsedown, other developers have used the same approach to develop other Markdown parsers in PHP and in other languages.


