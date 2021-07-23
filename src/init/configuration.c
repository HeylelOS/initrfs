#include "configuration.h"

#include <string.h>
#include <ctype.h>

bool
configuration_line(char *line, size_t length, char **trimmedp) {
	/* Shorten up to the comment if there is one */
	const char *comment = strchr(line, '#');
	if(comment != NULL) {
		length = comment - line;
	}

	/* Trim the beginning */
	while(isspace(*line)) {
		length--;
		line++;
	}

	/* Trim the end */
	while(isspace(line[length - 1])) {
		length--;
	}
	/* If there wasn't anything to end-trim, no problem,
	we will just be replacing the previous nul delimiter */
	line[length] = '\0';

	bool retval = true;
	if(*line == '[' && line[length - 1] == ']') {
		/* We are a new section delimiter */
		/* Trim begnning of section name */
		do {
			length--;
			line++;
		} while(isspace(*line));

		/* Trim end of section name */
		do {
			length--;
		} while(isspace(line[length - 1]));
		line[length] = '\0';

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
