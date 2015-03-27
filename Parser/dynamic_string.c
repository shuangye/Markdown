#include <stdio.h>
#include <malloc.h>
#define char_t char

#ifdef _WIN32
#define malloc_usable_size(a) _msize(a)
#endif

static size_t _length(char_t* str)
{
	// note strlen() accepts only char * parameter
	if (NULL == str)
		return 0;

	size_t len = 0;
	while (*str++ != '\0')
		++len;
	return len;
}

char_t* init(int init_size)
{
	if (init_size <= 0)
		init_size = sizeof(char_t);

	char_t *result = (char_t *)calloc(1, init_size);
	// if (NULL == result)
	//    return NULL;
	return result;
}

char_t* append(char_t *src, char_t *to_append)
{
	if (NULL == src || NULL == to_append)
		return src;

	char_t *result = src;
	const size_t unit = sizeof(src[0]);
	const size_t usable_size = malloc_usable_size(src);
	const size_t src_len = _length(src);
	const size_t to_append_len = _length(to_append);
	const size_t desired_size = (src_len + to_append_len) * unit + 1;

	if (usable_size < desired_size) {
		result = (char_t *)realloc(src, desired_size);
		if (NULL == result)
			return NULL;
	}

	// use memcpy instead of strcat, since the latter accepts only char*	
	// sprintf(src + src_len, "%s", to_append);
	memcpy(src + src_len, to_append, (to_append_len + 1) * unit);
	return result;
}

char_t* append_printf(char *str, const char *format, ...)
{
	if (NULL == str || NULL == format)
		return str;

	char_t *result = str;
	const size_t unit = sizeof(str[0]);
	const size_t usable_size = malloc_usable_size(str);
	const size_t src_len = _length(str);
	const size_t to_append_len = _length(to_append);
	const size_t desired_size = (src_len + to_append_len) * unit + 1;

	if (usable_size < desired_size) {
		result = (char_t *)realloc(src, desired_size);
		if (NULL == result)
			return NULL;
	}
}
