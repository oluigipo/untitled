#pragma once
#include "types.h"
#include "std.h"

#define locale_str(e) (locales[currentLocale][e])

enum {
	TXT_LANGUAGE_NAME,
	TXT_HELLO_WORLD,
	TXT__COUNT
};

global string** locales;
global uint localeCount;
global uint currentLocale;

void locale_init(void);
void locale_deinit(void);
void locale_reload(void);
