#include "configuration.h"
#include "trimstr.h"

#include <string.h>
#include <ctype.h>

bool
configuration_line(char *line, size_t length, char **trimmedp) {
	/* Shorten up to the comment if there is one */
	const char *comment = strchr(line, '#');
	if(comment != NULL) {
		length = comment - line;
	}

	line = trimstr(line, length);

	bool retval = true;
	if(*line == '[' && line[length - 1] == ']') {
		/* We are a new section delimiter, trim name */
		line = trimstr(line + 1, length - 1);
		retval = false;
	}

	*trimmedp = line;

	return retval;
}

static const char * const sections[] = {
	"common",
	"fstab",
	"modtab",
};

enum configuration_section
configuration_section_from_name(const char *name) {
	const char * const *current = sections,
		* const *sectionsend = sections + sizeof(sections) / sizeof(*sections);

	while(current != sectionsend && strcmp(*current, name) != 0) {
		current++;
	}

	/* CONFIGURATION_SECTION_UNKNOWN being the next after the last, it corresponds to sectionsend */
	return current - sections;
}
