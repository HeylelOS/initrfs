/* SPDX-License-Identifier: BSD-3-Clause */
#include "configuration.h"
#include "mountutils.h"
#include "trimstr.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mount.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <err.h>

enum configuration_section {
	CONFIGURATION_SECTION_FSTAB,
	CONFIGURATION_SECTION_MODTAB,
	CONFIGURATION_SECTION_UNSUPPORTED,
};

static bool
configuration_line(char *line, size_t length, char **trimmedp) {
	/* Shorten up to the comment if there is one */
	const char * const comment = strchr(line, '#');
	if (comment != NULL) {
		length = comment - line;
	}

	line = trimstr(line, length);
	length = strlen(line);

	bool retval = true;
	if (*line == '[' && line[length - 1] == ']') {
		/* We are a new section delimiter, trim name */
		line = trimstr(line + 1, length - 2);
		retval = false;
	}

	*trimmedp = line;

	return retval;
}

static const char * const sections[] = {
	"fstab",
	"modtab",
};

static enum configuration_section
configuration_section_from_name(const char *name) {
	const char * const *current = sections,
		* const * const sectionsend = sections + sizeof (sections) / sizeof (*sections);

	while (current != sectionsend && strcmp(*current, name) != 0) {
		current++;
	}

	/* CONFIGURATION_SECTION_UNSUPPORTED being the next after the last, it corresponds to sectionsend */
	return current - sections;
}

static const char *
tab_field(char **fieldsp) {
	char *field = *fieldsp;

	while (isspace(*field)) {
		field++;
	}

	char *end = field;
	while (*end != '\0' && !isspace(*end)) {
		end++;
	}

	if (*end != '\0') {
		*end = '\0';
		*fieldsp = end + 1;
	}

	return field;
}

static void
configure_section_fstab(char *tab) {
	struct mount_description desc = {
		.source = tab_field(&tab),
		.target = tab_field(&tab),
		.fstype = tab_field(&tab),
	};
	const char * const options = tab_field(&tab);
	[[gnu::unused]] const unsigned long freq = strtoul(tab_field(&tab), NULL, 0);
	[[gnu::unused]] const unsigned long passno = strtoul(tab_field(&tab), NULL, 0);
	char *data;

	desc.flags = mount_resolve_options(options, &data);
	desc.data = data;

	/* EBUSY is sorta trying a remount, which we can just discard at this point. */
	if (mount(desc.source, desc.target, desc.fstype, desc.flags, desc.data) != 0 && errno != EBUSY) {
		err(1, "Unable to mount root '%s' (%s) to '%s'", desc.source, desc.fstype, desc.target);
	}

	free(data);
}

static void
configure_section_modtab(char *tab) {
}

void
configure_system(const char *configsys) {
	enum configuration_section section = CONFIGURATION_SECTION_UNSUPPORTED;
	FILE *filep = fopen(configsys, "r");
	size_t capacity = 0;
	char *line = NULL;
	ssize_t length;

	if (filep == NULL) {
		warn("Skipping configuration, unable to open '%s'", configsys);
		return;
	}

	while (length = getline(&line, &capacity, filep), length != -1) {
		char *trimmed;

		if (configuration_line(line, length, &trimmed)) {
			if (*trimmed == '\0') {
				continue;
			}

			switch (section) {
			case CONFIGURATION_SECTION_FSTAB:
				configure_section_fstab(trimmed);
				break;
			case CONFIGURATION_SECTION_MODTAB:
				configure_section_modtab(trimmed);
				break;
			case CONFIGURATION_SECTION_UNSUPPORTED:
				break;
			}
		} else {
			section = configuration_section_from_name(trimmed);
		}
	}

	free(line);
	fclose(filep);
}
