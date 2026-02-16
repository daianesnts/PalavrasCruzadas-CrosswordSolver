#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api_client.h"

void api_init(void) {
    printf("API Client initialized (STUB MODE - Offline Only)\n");
}

void api_cleanup(void) {
}

bool fetch_word_definition(const char* word_text, char* out_definition, int max_len) {
    return false; 
}
