/*
 * Copyright (C) 2023 Snoolie K / 0xilis. All rights reserved.
 *
 * This document is the property of Snoolie K / 0xilis.
 * It is considered confidential and proprietary.
 *
 * This document may not be reproduced or transmitted in any form,
 * in whole or in part, without the express written permission of
 * Snoolie K / 0xilis.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define seajson char*

/* JSON Pathway Cache Types */

#define DICTIONARY_START 1
#define DICTIONARY_END 2
#define STRING_START 3
#define STRING_END 4

/* Functions */

seajson init_json_from_file(const char *restrict filename);
void free_json(seajson json);
char* get_string(seajson json, const char *value);
unsigned long get_int(seajson json, const char *value);
seajson get_dictionary(seajson json, const char *value);
int seaJSONBuildVersion(void);

/* Only kept for backwards compatibility with original SeaJSON library - THIS FUNCTION IS NOT SAFE !!!! DO NOT USE !!! */
char * getstring(char *funckey, char *dict);