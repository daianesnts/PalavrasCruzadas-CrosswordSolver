#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "api_client.h"

// Buffer para comando output
#define CMD_BUF_SIZE 4096

void api_init(void) {
    printf("API Client initialized (Curl Mode)\n");
}

void api_cleanup(void) {
}

static void extract_definition(const char* json, char* out_def, int max_len) {
    // Método simples de passar para evitar dependencias externas
    // Procurando por "definition":"..."
    const char* key = "\"definition\":\"";
    char* start = strstr(json, key);
    
    if (!start) {
         // Tenta fallback para outras APIs (OpenDicio)
         return;
    }

    start += strlen(key);
    char* end = strchr(start, '"');
    if (end) {
        int len = end - start;
        if (len > max_len - 1) len = max_len - 1;
        strncpy(out_def, start, len);
        out_def[len] = '\0';
    }
}

// Auxiliar para escaparcommand args (basico)
static void sanitize_cmd(char* dest, const char* src) {
    while (*src) {
        if ((*src >= 'a' && *src <= 'z') || (*src >= 'A' && *src <= 'Z') || (*src >= '0' && *src <= '9')) {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

bool fetch_word_definition(const char* word, char* out_definition, int max_len) {
    char clean_word[128];
    sanitize_cmd(clean_word, word);
    
    char cmd[512];
    
    snprintf(cmd, sizeof(cmd), "curl -s -m 2 \"https://api.dictionaryapi.dev/api/v2/entries/en/%s\"", clean_word);
    
    FILE* pipe = popen(cmd, "r");
    if (!pipe) return false;

    char buffer[CMD_BUF_SIZE];
    size_t read_bytes = fread(buffer, 1, sizeof(buffer) - 1, pipe);
    buffer[read_bytes] = '\0';
    pclose(pipe);

    if (strstr(buffer, "\"definition\"")) {
        extract_definition(buffer, out_definition, max_len);
        return true;
    }

    // Fallback: OpenDicio (PT)
    // URL: https://api.dicionario-aberto.net/word/{word}
    // Retorna JSON array do objeto normamente
    snprintf(cmd, sizeof(cmd), "curl -s -m 2 \"https://api.dicionario-aberto.net/word/%s\"", clean_word);
    pipe = popen(cmd, "r");
    if (!pipe) return false;

    read_bytes = fread(buffer, 1, sizeof(buffer) - 1, pipe);
    buffer[read_bytes] = '\0';
    pclose(pipe);
    
    return false;
}