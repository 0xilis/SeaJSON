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
#include "seajson.h"

int main(void) {
  printf("Hello World\n");
  seajson json = init_json_from_file("level.json");
  printf("json: %s\n",json);
  char * stringValue = get_string(json, "zone_name");
  printf("zone_name: %s\n",stringValue);
  unsigned long intValue = get_int(json, "zone_id");
  printf("zone_id: %ld\n",intValue);
  unsigned long platformCount = get_int(json, "platform_count");
  printf("platform_count: %ld\n",platformCount);
  seajson startingPoint = get_dictionary(json, "starting_point");
  printf("starting_point: %s\n",startingPoint);
  jarray platforms = get_array(json, "platforms");
  if (platforms.isValid) {
    printf("platforms.arrayString: %s\n",platforms.arrayString);
    printf("platforms.itemCount: %d\n",platforms.itemCount);
    printf("platforms.isValid: %d\n",platforms.isValid);
  } else {
    printf("error: platforms is not a valid jarray, SeaJSON likely had an error retrieving the dictionary\n");
    return -1;
  }
  jarray platformsNoWhitespace = remove_whitespace_from_jarray(platforms);
  /* It's always a good idea to free the passed in jarray into remove_whitespace_from_jarray assuming you don't plan on using the jarray with whitespace */
  free_jarray(platforms);
  printf("platformsNoWhitespace: %s\n",platformsNoWhitespace.arrayString);
  char* firstPlatformRaw = get_item_from_jarray(platformsNoWhitespace, 0);
  printf("firstPlatformRaw: %s\n",firstPlatformRaw);
  jarray arrayOfStrings = get_array(json, "demo_array_of_strings");
  char* item1 = get_string_from_jarray(arrayOfStrings, 1);
  printf("item1: %s\n",item1);
  jarray platformPoints = get_array(firstPlatformRaw, "point");
  if (platformPoints.isValid == 0) {
    fprintf(stderr, "failed getting point info");
    exit(1);
  }
  int platformYPoint = get_int_from_jarray(platformPoints, 1);
  printf("platformYPoint: %d\n",platformYPoint);
  free_jarray(platformPoints);
  free(item1);
  free_jarray(arrayOfStrings);
  free(firstPlatformRaw);
  free_jarray(platformsNoWhitespace);
  free(startingPoint);
  free(stringValue);
  free_json(json);
  return 0;
}