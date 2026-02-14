#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdbool.h>
#include "structs.h"

// Inicializa o sistema do dicionário
void dict_init(void);

// Checa se a palavra é válida e popula a definição se apontador for providenciado
// Retorna true se valido/encontrado.
bool dict_check_word(const char* word, char* out_def, int max_len);

// Define o idioma ativo par buscas (ex: "PT" ou "EN")
void dict_set_language(const char* lang);

// Pesquisa pela palavra candidata pelo tamanho (no idioma ativo)
void dict_search_by_size(int size, char*** out_words, int* out_count);

#endif // DICTIONARY_H
