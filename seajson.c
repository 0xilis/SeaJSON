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

#include "seajson.h"

seajson init_json_from_file(const char *restrict filename) {
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL) {
    fprintf(stderr,"SeaJSON Error: Cannot find file.\n");
    exit(1);
  }
  fseek(fp, 0L, SEEK_END);
  long sz = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  /* sz is now the file size */
  char *json = malloc(sizeof(char) * sz);
  if (json == NULL) {
    fclose(fp);
    fprintf(stderr, "SeaJSON Error: Memory allocation failed.\n");
    exit(1);
  }
  size_t bytesRead = fread(json, 1, sz, fp);
  if (bytesRead < sz) {
    fclose(fp);
    free(json);
    fprintf(stderr, "SeaJSON Error: Failed to read the entire file.\n");
    exit(1);
  }
  /* Null-terminate the string */
  json[sz] = '\0';
  /* Close the file pointer */
  fclose(fp);
  return json;
}

void free_json(seajson json) {
  free(json);
  json = NULL;
}

#define pathway char*

char* init_json_pathway_cache(unsigned long sizeOfJson) {
  pathway pathwayCache = malloc(sizeof(char) * sizeOfJson);
  return pathwayCache;
}

void free_json_pathway_cache(pathway pathwayCache) {
  free(pathwayCache);
  pathwayCache = NULL;
}

char prevCached(pathway pathwayCache, int pathwayPos) {
  if (pathwayPos < 1) {
    /*fprintf(stderr, "SeaJSON Error: Under 1 value passed into prevCached().");*/
    return 0;
  }
  return pathwayCache[pathwayPos-1];
}

#define CASH pathwayCache[pathwayPos]=
#define CASHUP pathwayPos++;
#define prevCachedAuto prevCached(pathwayCache, pathwayPos)

char* get_string(seajson json, const char *value) {
  unsigned long jsonSize = strlen(json);
  pathway pathwayCache = init_json_pathway_cache(jsonSize);
  int pathwayPos = 0;
  char* readString = malloc(sizeof(char) * strlen(value) + 1);
  int stringProgress = 0;
  int stringWaitingConformation = 0;
  int valueFound = 0;
  char* returnString = malloc(sizeof(char) * jsonSize + 1);
  for (int i = 0; i < jsonSize; i++) {
    char currentChar = json[i];
    char prev = prevCachedAuto;
    if (currentChar == '{') {
      if (prev != STRING_START) {
        CASH DICTIONARY_START;
        CASHUP
      }
    } else if (currentChar == '\"') {
      if (prev == STRING_START) {
        if (stringProgress == strlen(value)) {
          if (strcmp(value,readString) == 0) {
            /* We might have just found the string! */
            stringWaitingConformation = 1;
          }
        }
        if (valueFound == 1) {
          /* We are on the ending " so we got our string */
          free(readString);
          free_json_pathway_cache(pathwayCache);
          return returnString;
        }
        CASH STRING_END;
        CASHUP
      } else {
        stringProgress = 0;
        stringWaitingConformation = 0;
        CASH STRING_START;
        CASHUP
      }
    } else if (currentChar == ':') {
      if (prev == STRING_END && stringWaitingConformation == 1) {
        /* HOLY FUCK, we found the string!! */
        valueFound = 1;
      }
    }
    /*
    Remember that if user passes in something like "Apples"
    Don't just search for apples, as 
    */
    if (prev == STRING_START) {
      if (stringProgress > strlen(value)) {
        /* The string we are reading is bigger than the string we want - this means it is DEFINITELY not the string */
        stringProgress = 0;
        stringWaitingConformation = 0;
      } else if (valueFound) {
        returnString[stringProgress] = currentChar;
        returnString[stringProgress + 1] = '\0';
        stringProgress++;
      } else {
        readString[stringProgress] = currentChar;
        readString[stringProgress + 1] = '\0';
        stringProgress++;
      }
    }
  }
  free(readString);
  free(returnString);
  free_json_pathway_cache(pathwayCache);
  return NULL;
}

unsigned long get_int(seajson json, const char *value) {
  unsigned long jsonSize = strlen(json);
  pathway pathwayCache = init_json_pathway_cache(jsonSize);
  int pathwayPos = 0;
  char* readString = malloc(sizeof(char) * strlen(value) + 1);
  int stringProgress = 0;
  int stringWaitingConformation = 0;
  int valueFound = 0;
  unsigned long returnInt = 0;
  for (int i = 0; i < jsonSize; i++) {
    char currentChar = json[i];
    char prev = prevCachedAuto;
    if (currentChar == '{') {
      if (prev != STRING_START) {
        CASH DICTIONARY_START;
        CASHUP
      }
    } else if (currentChar == '\"') {
      if (prev == STRING_START) {
        if (stringProgress == strlen(value)) {
          if (strcmp(value,readString) == 0) {
            /* We might have just found the string! */
            stringWaitingConformation = 1;
          }
        }
        CASH STRING_END;
        CASHUP
      } else {
        stringProgress = 0;
        stringWaitingConformation = 0;
        CASH STRING_START;
        CASHUP
      }
    } else if (currentChar == ':') {
      if (prev == STRING_END && stringWaitingConformation == 1) {
        /* HOLY FUCK, we found the string!! */
        valueFound = 1;
        continue;
      }
    }
    /*
    Remember that if user passes in something like "Apples"
    Don't just search for apples, as 
    */
    if (valueFound == 1) {
      if (currentChar == '}') {
        free(readString);
        free_json_pathway_cache(pathwayCache);
        return returnInt;
      }
      if (currentChar == '\"') {
        free(readString);
        free_json_pathway_cache(pathwayCache);
        return returnInt;
      }
      if (currentChar == ',') {
        free(readString);
        free_json_pathway_cache(pathwayCache);
        return returnInt;
      }
      returnInt *= 10;
      returnInt += currentChar - '0';
    }
    if (prev == STRING_START) {
      if (stringProgress > strlen(value)) {
        /* The string we are reading is bigger than the string we want - this means it is DEFINITELY not the string */
        stringProgress = 0;
        stringWaitingConformation = 0;
      } else if (valueFound == 0) {
        readString[stringProgress] = currentChar;
        readString[stringProgress + 1] = '\0';
        stringProgress++;
      }
    }
  }
  free(readString);
  free_json_pathway_cache(pathwayCache);
  return 0;
}

seajson get_dictionary(seajson json, const char *value) {
  unsigned long jsonSize = strlen(json);
  pathway pathwayCache = init_json_pathway_cache(jsonSize);
  int pathwayPos = 0;
  char* readString = malloc(sizeof(char) * strlen(value) + 1);
  int stringProgress = 0;
  int stringWaitingConformation = 0;
  int valueFound = 0;
  char* returnString = malloc(sizeof(char) * jsonSize + 1);
  int inception = 0;
  int inceptionInString = 0;
  for (int i = 0; i < jsonSize; i++) {
    char currentChar = json[i];
    char prev = prevCachedAuto;
    if (currentChar == '{') {
      if (prev != STRING_START) {
        CASH DICTIONARY_START;
        CASHUP
      }
    } else if (currentChar == '\"') {
      if (prev == STRING_START) {
        if (stringProgress == strlen(value)) {
          if (strcmp(value,readString) == 0) {
            /* We might have just found the string! */
            stringWaitingConformation = 1;
          }
        }
        CASH STRING_END;
        CASHUP
      } else {
        if (valueFound == 0) {
          stringProgress = 0;
          stringWaitingConformation = 0;
        }
        CASH STRING_START;
        CASHUP
      }
    } else if (currentChar == ':') {
      if (prev == STRING_END && stringWaitingConformation == 1) {
        /* HOLY FUCK, we found the string!! */
        valueFound = 1;
        stringProgress = 0;
        stringWaitingConformation = 0;
        /* continue so the : will not be added to start */
        continue;
      }
    } else if (currentChar == '}') {
      if (valueFound == 1 && inception == 1 && inceptionInString == 0) {
        /* We are on the ending } so we got our json */
        /* Append ourselves to the end of the string then ret */
        returnString[stringProgress] = '}';
        returnString[stringProgress+1] = '\0';
        free(readString);
        free_json_pathway_cache(pathwayCache);
        return returnString;
      }
    }
    /*
    Remember that if user passes in something like "Apples"
    Don't just search for apples, as 
    */
    if (valueFound) {
      returnString[stringProgress] = currentChar;
      returnString[stringProgress + 1] = '\0';
      stringProgress++;
      if (inceptionInString == 0) {
        if (currentChar == '{') {
          inception++;
        } else if (currentChar == '}') {
          inception--;
        } else if (currentChar == '\"') {
          inceptionInString = 1;
        } else if (currentChar == '[') {
          inception++;
        } else if (currentChar == ']') {
          inception--;
        }
      } else {
        /* We are currently reading chars in a string obj */
        if (currentChar == '\"') {
          /* If \" then cancel out */
          if (json[i-1] != '\\') {
            inceptionInString = 0;
          }
        }
      }
    } else if (prev == STRING_START) {
      if (stringProgress > strlen(value)) {
        /* The string we are reading is bigger than the string we want - this means it is DEFINITELY not the string */
        stringProgress = 0;
        stringWaitingConformation = 0;
      } else {
        readString[stringProgress] = currentChar;
        readString[stringProgress + 1] = '\0';
        stringProgress++;
      }
    }
  }
  free(readString);
  free(returnString);
  free_json_pathway_cache(pathwayCache);
  return NULL;
}

jarray get_array(seajson json, const char *value) {
  unsigned long jsonSize = strlen(json);
  pathway pathwayCache = init_json_pathway_cache(jsonSize);
  int pathwayPos = 0;
  char* readString = malloc(sizeof(char) * strlen(value) + 1);
  int stringProgress = 0;
  int stringWaitingConformation = 0;
  int valueFound = 0;
  char* returnString = malloc(sizeof(char) * jsonSize + 1);
  int inception = 0;
  int inceptionInString = 0;
  int itemCount = 0;
  for (int i = 0; i < jsonSize; i++) {
    char currentChar = json[i];
    char prev = prevCachedAuto;
    if (currentChar == '{') {
      if (prev != STRING_START) {
        CASH DICTIONARY_START;
        CASHUP
      }
    } else if (currentChar == '\"') {
      if (prev == STRING_START) {
        if (stringProgress == strlen(value)) {
          if (strcmp(value,readString) == 0) {
            /* We might have just found the string! */
            stringWaitingConformation = 1;
          }
        }
        CASH STRING_END;
        CASHUP
      } else {
        if (valueFound == 0) {
          stringProgress = 0;
          stringWaitingConformation = 0;
        }
        CASH STRING_START;
        CASHUP
      }
    } else if (currentChar == ':') {
      if (prev == STRING_END && stringWaitingConformation == 1) {
        /* HOLY FUCK, we found the string!! */
        valueFound = 1;
        stringProgress = 0;
        stringWaitingConformation = 0;
        /* continue so the : will not be added to start */
        continue;
      }
    } else if (currentChar == ']') {
      if (valueFound == 1 && inception == 1 && inceptionInString == 0) {
        /* We are on the ending } so we got our json */
        /* Append ourselves to the end of the string then ret */
        returnString[stringProgress] = ']';
        returnString[stringProgress+1] = '\0';
        free(readString);
        free_json_pathway_cache(pathwayCache);
        jarray jsonArray;
        jsonArray.itemCount = itemCount;
        jsonArray.arrayString = returnString;
        jsonArray.isValid = 1;
        return jsonArray;
      }
    }
    /*
    Remember that if user passes in something like "Apples"
    Don't just search for apples, as 
    */
    if (valueFound) {
      returnString[stringProgress] = currentChar;
      returnString[stringProgress + 1] = '\0';
      stringProgress++;
      if (inceptionInString == 0) {
        if (currentChar == '{') {
          inception++;
        } else if (currentChar == '}') {
          inception--;
        } else if (currentChar == '\"') {
          inceptionInString = 1;
        } else if (currentChar == '[') {
          inception++;
          if (inception == 1) {
            if (itemCount == 0) {
              itemCount++;
            }
          }
        } else if (currentChar == ']') {
          inception--;
        } else if (currentChar == ',') {
          /* Make sure the , is not inside of {}/[] */
          if (inception == 1) {
            itemCount++;
          }
        }
      } else {
        /* We are currently reading chars in a string obj */
        if (currentChar == '\"') {
          /* If \" then cancel out */
          if (json[i-1] != '\\') {
            inceptionInString = 0;
          }
        }
      }
    } else if (prev == STRING_START) {
      if (stringProgress > strlen(value)) {
        /* The string we are reading is bigger than the string we want - this means it is DEFINITELY not the string */
        stringProgress = 0;
        stringWaitingConformation = 0;
      } else {
        readString[stringProgress] = currentChar;
        readString[stringProgress + 1] = '\0';
        stringProgress++;
      }
    }
  }
  free(readString);
  free(returnString);
  free_json_pathway_cache(pathwayCache);
  jarray error;
  error.itemCount = 0;
  /* Set isValid to 0 since this is an error and not a valid jarray */
  error.isValid = 0;
  return error;
}

/* This is a very WIP function, it does not allow JSONs such that are formatted with new lines or spaces in the slightest currently - either convert a JSON to not have whitespace and then do rest of the function or modify the function to behave differently. */
char* get_item_from_jarray(jarray array, int index) {
  if (array.isValid == 0) {
    fprintf(stderr, "SeaJSON Error: Non-valid jarray passed into get_item_from_array.\n");
    exit(1);
  }
  if (array.itemCount <= 0) {
    fprintf(stderr, "SeaJSON Error: jarray with 0 or less items passed into get_item_from_array.\n");
    exit(1);
  }
  if (index >= array.itemCount) {
    fprintf(stderr,"SeaJSON Error: Requested OOB index from jarray.\n");
    exit(1);
  }
  char *arrayString = array.arrayString;
  unsigned long arrStrLen = strlen(arrayString);
  int itemIndex = 0;
  char* returnItem = malloc(sizeof(char) * arrStrLen);
  int returnItemIndex = 0;
  int inception = 0;
  int inceptionInString = 0;
  /* Skip the first item since it will just be a [ */
  for (int i = 1; i < arrStrLen; i++) {
    char currentChar = arrayString[i];
    if (inceptionInString == 0) {
      if (currentChar == '{') {
        inception++;
      } else if (currentChar == '}') {
        inception--;
      } else if (currentChar == '\"') {
        inceptionInString = 1;
      } else if (currentChar == '[') {
        inception++;
      } else if (currentChar == ']') {
        inception--;
      }
    } else {
      /* We are currently reading chars in a string obj */
      if (currentChar == '\"') {
        /* If \" then cancel out */
        if (arrayString[i-1] != '\\') {
          inceptionInString = 0;
        }
      }
    }
    char futureChar = arrayString[i+1];
    if (itemIndex == index) {
      returnItem[returnItemIndex] = currentChar;
      returnItem[returnItemIndex+1] = '\0';
      returnItemIndex++;
      if ((futureChar == ',' && inception == 0 && inceptionInString == 0) || i == (strlen(arrayString)-2)) {
        return returnItem;
      }
    } else {
      if (i == (strlen(arrayString)-2)) {
        fprintf(stderr, "SeaJSON Error: Failed to find item in array.\n");
        exit(1);
      }
      if (futureChar == ',' && inception == 0 && inceptionInString == 0) {
        i++;
        itemIndex++;
      }
    }
  }
  free(returnItem);
  return NULL;
}

void free_jarray(jarray array) {
  free(array.arrayString);
}

seajson remove_whitespace_from_json(seajson json) {
  unsigned long jsonSize = strlen(json);
  char* returnJson = malloc(sizeof(char) * jsonSize);
  int inception = 0;
  int inceptionInString = 0;
  int returnJsonIndex = 0;
  for (int i = 0; i < jsonSize; i++) {
    char currentChar = json[i];
    if (inceptionInString == 0) {
      if (currentChar == '{') {
        inception++;
      } else if (currentChar == '}') {
        inception--;
      } else if (currentChar == '\"') {
        inceptionInString = 1;
      } else if (currentChar == '[') {
        inception++;
      } else if (currentChar == ']') {
        inception--;
      }
    } else {
      /* We are currently reading chars in a string obj */
      if (currentChar == '\"') {
        /* If \" then cancel out */
        if (json[i-1] != '\\') {
          inceptionInString = 0;
        }
      }
    }
    
    if (!inceptionInString) {
      /* Newline */
      if (currentChar == '\n') {
        continue;
      }
      /* Space */
      if (currentChar == ' ') {
        continue;
      }
      /* Tab (i think) */
      if (currentChar == ' ') {
        continue;
      }
    }
    returnJson[returnJsonIndex] = currentChar;
    returnJson[returnJsonIndex+1] = '\0';
    returnJsonIndex++;
  }
  return returnJson;
}

jarray remove_whitespace_from_jarray(jarray array) {
  if (array.isValid == 0) {
    fprintf(stderr, "SeaJSON Error: Non-valid jarray passed into removeWhitespaceFromJarray.\n");
    exit(1);
  }
  char* arrayString = array.arrayString;
  unsigned long arrStrSize = strlen(arrayString);
  char* returnJarrayString = malloc(sizeof(char) * arrStrSize);
  int inception = 0;
  int inceptionInString = 0;
  int returnJarrayStrIndex = 0;
  for (int i = 0; i < arrStrSize; i++) {
    char currentChar = arrayString[i];
    if (inceptionInString == 0) {
      if (currentChar == '{') {
        inception++;
      } else if (currentChar == '}') {
        inception--;
      } else if (currentChar == '\"') {
        inceptionInString = 1;
      } else if (currentChar == '[') {
        inception++;
      } else if (currentChar == ']') {
        inception--;
      }
    } else {
      /* We are currently reading chars in a string obj */
      if (currentChar == '\"') {
        /* If \" then cancel out */
        if (arrayString[i-1] != '\\') {
          inceptionInString = 0;
        }
      }
    }
    
    if (!inceptionInString) {
      /* Newline */
      if (currentChar == '\n') {
        continue;
      }
      /* Space */
      if (currentChar == ' ') {
        continue;
      }
      /* Tab (i think) */
      if (currentChar == ' ') {
        continue;
      }
    }
    returnJarrayString[returnJarrayStrIndex] = currentChar;
    returnJarrayString[returnJarrayStrIndex+1] = '\0';
    returnJarrayStrIndex++;
  }
  jarray returnJarray;
  returnJarray.itemCount = array.itemCount;
  returnJarray.isValid = array.isValid;
  returnJarray.arrayString = returnJarrayString;
  return returnJarray;
}

char* get_string_from_jarray(jarray array, int index) {
  char* rawItem = get_item_from_jarray(array, index);
  unsigned long rawItemLen = strlen(rawItem);
  if (rawItem[0] == '\"') {
    if (rawItem[rawItemLen - 1] == '\"') {
      /* Cut the beginning and ending " */
      char *start = &rawItem[1];
      char *end = &rawItem[rawItemLen - 1];
      /* Note the + 1 here, to have a null terminated substring */
      char *substr = (char *)calloc(1, end - start + 1);
      memcpy(substr, start, end - start);
      return substr;
    }
  }
  return rawItem;
}

int get_int_from_jarray(jarray array, int index) {
  char* rawItem = get_item_from_jarray(array, index);
  unsigned long stringNumberLen = strlen(rawItem);
  int returnInt = 0;
  int isNeg = 0;
  for (int i = 0; i < stringNumberLen; i++) {
    char currentChar = rawItem[i];
    if (currentChar == '-') {
      isNeg = 1;
    } else {
      returnInt *= 10;
      returnInt += currentChar - '0';
    }
  }
  if (isNeg) {
    returnInt *= -1;
  }
  free(rawItem);
  return returnInt;
}

jarray remove_item_of_jarray(jarray array, int index) {
  if (array.isValid == 0) {
    fprintf(stderr, "SeaJSON Error: Non-valid jarray passed into remove_item_of_jarray.\n");
    exit(1);
  }
  if (array.itemCount <= 0) {
    fprintf(stderr, "SeaJSON Error: jarray with 0 or less items passed into remove_item_of_jarray.\n");
    exit(1);
  }
  if (index >= array.itemCount) {
    fprintf(stderr,"SeaJSON Error: Requested OOB index from jarray (remove_item_of_jarray).\n");
    exit(1);
  }
  char *arrayString = array.arrayString;
  unsigned long arrStrLen = strlen(arrayString);
  int itemIndex = 0;
  char* returnItem = malloc(sizeof(char) * arrStrLen);
  int returnItemIndex = 0;
  int inception = 0;
  int inceptionInString = 0;
  /* Skip the first item since it will just be a [ */
  for (int i = 0; i < arrStrLen; i++) {
    char currentChar = arrayString[i];
    if (inceptionInString == 0) {
      if (currentChar == '{') {
        inception++;
      } else if (currentChar == '}') {
        inception--;
      } else if (currentChar == '\"') {
        inceptionInString = 1;
      } else if (currentChar == '[') {
        inception++;
      } else if (currentChar == ']') {
        inception--;
      }
    } else {
      /* We are currently reading chars in a string obj */
      if (currentChar == '\"') {
        /* If \" then cancel out */
        if (arrayString[i-1] != '\\') {
          inceptionInString = 0;
        }
      }
    }
    if (itemIndex != index) {
      returnItem[returnItemIndex] = currentChar;
      returnItem[returnItemIndex+1] = '\0';
      returnItemIndex++;
    }
    char futureChar = arrayString[i+1];
    if ((i+1) == (strlen(arrayString)-1)) {
      returnItem[returnItemIndex] = futureChar;
      returnItem[returnItemIndex+1] = '\0';
      jarray newJarray;
      newJarray.itemCount = array.itemCount - 1;
      newJarray.isValid = array.isValid;
      newJarray.arrayString = returnItem;
      return newJarray;
    }
    if (futureChar == ',' && inception == 1 && inceptionInString == 0) {
      i++;
      itemIndex++;
      if (itemIndex != index) {
        returnItem[returnItemIndex] = ',';
        returnItem[returnItemIndex+1] = '\0';
        returnItemIndex++;
      }
    }
  }
  free(returnItem);
  printf("SeaJSON Error: remove_item_of_jarray has encounter a problem. Returning original jarray...\n");
  return array;
}

jarray add_item_to_jarray(jarray array, char* item) {
  if (array.isValid == 0) {
    fprintf(stderr, "SeaJSON Error: Non-valid jarray passed into add_item_to_jarray.\n");
    exit(1);
  }
  char *arrayString = array.arrayString;
  unsigned long arrStrLen = strlen(arrayString);
  if (arrayString[arrStrLen - 1] == ']' && arrayString[0] == '[') {
    unsigned long itemLen = strlen(item);
    if (arrStrLen == 2) {
      /* A blank jarray has been passed in */
      char* returnItem = malloc(sizeof(char) * (3 + itemLen));
      returnItem[0] = '[';
      /*
      I was gonna do strncat(returnItem, item, strlen(item));
      But for some reason that seems buggy
      */
      for (int i = 0; i < itemLen; i++) {
        returnItem[i+1] = item[i];
      }
      returnItem[itemLen+1] = ']';
      returnItem[itemLen+2] = '\0';
      jarray newJarray;
      newJarray.arrayString = returnItem;
      newJarray.isValid = 1;
      newJarray.itemCount = 1;
      return newJarray;
    } else {
      char* returnItem = malloc(sizeof(char) * (arrStrLen + strlen(item) + 2));
      for (int i = 0; i < arrStrLen; i++) {
        returnItem[i] = arrayString[i];
      }
      returnItem[arrStrLen-1] = ',';
      for (int i = 0; i < itemLen; i++) {
        returnItem[arrStrLen+i] = item[i];
      }
      returnItem[arrStrLen+itemLen] = ']';
      returnItem[arrStrLen+itemLen+1] = '\0';
      jarray newJarray;
      newJarray.itemCount = array.itemCount + 1;
      newJarray.arrayString = returnItem;
      newJarray.isValid = 1;
      return newJarray;
    }
  } else {
    fprintf(stderr, "SeaJSON Error: Failed to find end of jarray (add_item_to_jarray).");
    exit(1);
  }
  return array;
}

/* Only kept for backwards compatibility with original SeaJSON library - THIS FUNCTION IS NOT SAFE !!!! DO NOT USE !!! */
char * getstring(char *funckey, char *dict) {
  printf("WARNING!!! THIS FUNCTION IS DEPRECATED AND IS INTENDED HERE ONLY FOR BACKWARDS COMPATIBILITY WITH THE OLD SEAJSON. THIS IS NOT SAFE AND BUGGY, USE get_string() INSTEAD!!! DO NOT USE THIS!!!\n");
  char funckey1[strlen(funckey)];
  for (size_t i = 0; i < strlen(funckey); i++) {
    /* Access each char in the string */
    funckey1[i] = funckey[i];
  }
  char test[strlen(dict)];
  for (size_t i = 0; i < strlen(dict); i++) {
    /* Access each char in the string */
    test[i] = dict[i];
  }
  if (!(funckey1[strlen(funckey1) - 1] == '.')){
    strcat(funckey1,".");
  }
  int testt3, testt4, beginkey, endkey = 0;
  int c = 0;
  int keylength = strlen(funckey1);
  char sub[sizeof test + 1];
  char *returnkey;
  int ignore = 0;
  int instring = 0;
  int tempvar = 0;
  int charindex = 0;
  int dotcount2 = 0;
  int getdot = 0;
  int beginkeysegment = 0;
  int endkeysegment = 0;
  int keysegmentlength = 0;
  char dictkeysegment[sizeof funckey1];
  int c2 = 0;
  /* Count number of dots in dictkey */
  while (charindex < strlen(funckey1)){
    if (funckey1[charindex] == '.'){
      getdot++;
    } else {
    }
    charindex++;
  }
  charindex = 0;
  /* Get part of dictkey after a dot */
  while (tempvar == 0){
    if (funckey1[charindex] == '.'){
      dotcount2++;
    } else {
      if (dotcount2 == getdot - 1) {
        keysegmentlength++;
        endkeysegment = charindex;
      }
    }
    charindex++;
    if (!(charindex < sizeof funckey1)) {
      tempvar++;
    }
  }
  beginkeysegment = (endkeysegment - keysegmentlength) + 1;
  while (c2 < keysegmentlength) {
   dictkeysegment[c2] = funckey1[beginkeysegment+c2];
   c2++;
  }
  for(int i = 0; i < sizeof test; ++i) {
    if (test[i] == '\"'){
      /* is " */
      testt3 = 0;
      testt4 = 0;
      while (testt3 < keylength){
        testt3++;
        if (dictkeysegment[testt3-1] == test[i+testt3]){
          testt4++;
        }
      }
      if (keysegmentlength == testt4){
        testt4 = 0;
        if (test[i + keysegmentlength + 2] == ':'){
          if (test[i + keysegmentlength + 3] == '\"'){
            instring = 1;
          }
          while (testt3){
            if (test[i + keysegmentlength + 4 + testt4] == '}' && ignore > 0){
              ignore = ignore - 1;
            } else if (test[i + keysegmentlength + 3 + testt4] == '{'){
              ignore++;
            } else if (((test[i + keysegmentlength + 4 + testt4] == '\"') && instring == 1 && ignore == 0) || ((test[i + keysegmentlength + 4 + testt4] == ',') && instring == 0 && ignore == 0) || ((test[i + keysegmentlength + 4 + testt4] == '}') && instring == 0 && ignore == 0)){
              if (ignore == 0){
                if (test[i + keysegmentlength + 3] == '\"'){
                  beginkey = i + keysegmentlength + 5;
                } else {
                  beginkey = i + keysegmentlength + 4;
                }
                endkey = i + keysegmentlength + 4 + testt4;
                break;
              }
            }
            testt4++;
          }
          while (c < endkey - beginkey + 1) {
            sub[c] = test[beginkey+c-1];
            c++;
          }
          sub[c] = '\0';
          returnkey = sub;
          printf("%sIwouldLikeToRemoveThisButForSomeReasonCant", sub); /* I have no idea why but getstring() doesn't work correctly if I don't print it, I don't know why, it just doesn't */
        }
      };
    }
  }
  if (c == 0){
    return 0;
  } else {
    return returnkey;
  }
}

/* Just a function to return SeaJSON build version in case a program ever needs to check */
int seaJSONBuildVersion(void) {
  return 7;
}
