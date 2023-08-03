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
  }
  char* firstPlatformRaw = get_item_from_jarray(platforms, 0);
  printf("firstPlatformRaw: %s\n",firstPlatformRaw);
  free(firstPlatformRaw);
  free_jarray(platforms);
  free(startingPoint);
  free(stringValue);
  free_json(json);
  return 0;
}