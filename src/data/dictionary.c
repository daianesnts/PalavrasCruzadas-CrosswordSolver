#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "dictionary.h"

#define MAX_DICT_WORDS 2000
#define MAX_DEF_LEN 200

typedef struct {
    char word[TAMANHO_MAX_PALAVRA + 1];
    char definition[MAX_DEF_LEN];
    char lang[3]; // "PT" ou "EN"
} DictEntry;

static DictEntry dictionary[MAX_DICT_WORDS];
static int dictWordCount = 0;
static char currentLang[3] = "PT"; // Padrao

static void trim(char * s) {
    char * p = s;
    int l = strlen(p);

    while(isspace(p[l - 1])) p[--l] = 0;
    while(* p && isspace(* p)) ++p, --l;

    memmove(s, p, l + 1);
}

static void load_file(const char* filepath, const char* lang) {
    FILE* file = fopen(filepath, "r");
    if (!file) {
        // Tenta fallback caminhos
        char path2[128]; snprintf(path2, 128, "../%s", filepath);
        file = fopen(path2, "r");
        if (!file) {
            char path3[128]; snprintf(path3, 128, "../../%s", filepath);
            file = fopen(path3, "r");
        }
    }

    if (!file) {
        printf("Warning: Could not open dictionary file: %s\n", filepath);
        return;
    }

    char line[512];
    while (fgets(line, sizeof(line), file) && dictWordCount < MAX_DICT_WORDS) {
        // Formato: PALAVRA:Definição
        char* sep = strchr(line, ':');
        if (sep) {
            *sep = '\0';
            char* word = line;
            char* def = sep + 1;
            
            trim(word);
            trim(def);

            if (strlen(word) > 0 && strlen(word) <= TAMANHO_MAX_PALAVRA) {
                strncpy(dictionary[dictWordCount].word, word, TAMANHO_MAX_PALAVRA);
                dictionary[dictWordCount].word[TAMANHO_MAX_PALAVRA] = '\0';
                
                strncpy(dictionary[dictWordCount].definition, def, MAX_DEF_LEN - 1);
                dictionary[dictWordCount].definition[MAX_DEF_LEN - 1] = '\0';

                strncpy(dictionary[dictWordCount].lang, lang, 2);
                dictionary[dictWordCount].lang[2] = '\0';

                dictWordCount++;
            }
        }
    }
    fclose(file);
    printf("Loaded %d words from %s (%s)\n", dictWordCount, filepath, lang);
}

void dict_init(void) {
    dictWordCount = 0;
    load_file("data_files/dictionaries/pt.txt", "PT");
    load_file("data_files/dictionaries/en.txt", "EN");
}

void dict_set_language(const char* lang) {
    if (lang) {
        strncpy(currentLang, lang, 2);
        currentLang[2] = '\0';
    }
}

bool dict_check_word(const char* word, char* out_def, int max_len) {
    for (int i = 0; i < dictWordCount; i++) {
        if (strcasecmp(dictionary[i].word, word) == 0) {
            if (strcmp(dictionary[i].lang, currentLang) == 0) {
                 if (out_def) {
                    strncpy(out_def, dictionary[i].definition, max_len - 1);
                    out_def[max_len - 1] = '\0';
                 }
                 return true; // Retorna true se encontra no indioma correto
            }
        }
    }
    return false;
}

void dict_search_by_size(int size, char*** out_words, int* out_count) {
    // 1. Conta matches
    int matches = 0;
    for (int i = 0; i < dictWordCount; i++) {
        if (strlen(dictionary[i].word) == size && strcmp(dictionary[i].lang, currentLang) == 0) {
            matches++;
        }
    }

    *out_count = matches;
    if (matches == 0) {
        *out_words = NULL;
        return;
    }

    // 2. Aloca
    *out_words = malloc(sizeof(char*) * matches);
    
    // 3. Completa
    int idx = 0;
    for (int i = 0; i < dictWordCount; i++) {
        if (strlen(dictionary[i].word) == size && strcmp(dictionary[i].lang, currentLang) == 0) {
            (*out_words)[idx] = strdup(dictionary[i].word);
            idx++;
        }
    }
}
