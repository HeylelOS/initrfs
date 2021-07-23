#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdbool.h>
#include <stddef.h>

enum configuration_section {
	CONFIGURATION_SECTION_COMMON,
	CONFIGURATION_SECTION_FSTAB,
	CONFIGURATION_SECTION_MODTAB,
	CONFIGURATION_SECTION_UNKNOWN,
};

bool
configuration_line(char *line, size_t length, char **trimmedp);

enum configuration_section
configuration_section_from_name(const char *name);

/* CONFIGURATION_H */
#endif
