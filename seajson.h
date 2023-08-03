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

typedef struct {
  int itemCount;
  char* arrayString;
  int isValid;
} jarray;

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
jarray get_array(seajson json, const char *value);
char* get_item_from_jarray(jarray array, int index);
void free_jarray(jarray array);
seajson remove_whitespace_from_json(seajson json);
jarray remove_whitespace_from_jarray(jarray array);
char* get_string_from_jarray(jarray array, int index);
int get_int_from_jarray(jarray array, int index);
jarray add_item_to_jarray(jarray array, char* item);
/* Maybe soon: jarray set_item_of_jarray(jarray array, int index, char* item); */
jarray remove_item_of_jarray(jarray array, int index);
int seaJSONBuildVersion(void);

/* Only kept for backwards compatibility with original SeaJSON library - THIS FUNCTION IS NOT SAFE !!!! DO NOT USE !!! */
char * getstring(char *funckey, char *dict);