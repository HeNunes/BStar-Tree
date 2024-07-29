#ifndef AUX_H
#define AUX_H

#include <stdio.h>

// Defines
#define NUM_CAMPOS 6
#define MAX_TAM_STRING 100
#define CONSISTENTE '1'
#define INCONSISTENTE '0'
#define DELIM_STRING '|'
#define DELIM_REG '#'
#define LIXO '$'

// Struct auxiliar utilizada para critérios de busca e edição
typedef struct par_t
{
    char campo[MAX_TAM_STRING];     // Campo à ser buscado
    int chave_int;                  // Chave do tipo inteiro (recebe -1 quando não utilizada)
    char chave_str[MAX_TAM_STRING]; // Chave do tipo string (recebe "\0" quando não utilizada)
} PAR;

// Mensagens de erro
void erro_arquivo();
void erro_registros();

// Checa se o arquivo existe
void checar_null(FILE *arq);

// Determina o tamanho de uma string 
int len_campo(char *buffer);

// Retira o lixo de uma string para imprimi-la na tela
void trata_string(char *buffer, int tam);

// Lê um campo do arquivo CSV
void ler_campo(FILE *arq_csv, char *buffer);

// Le uma string de tamanho variável do arquivo binário
void bin_ler_string_variavel(FILE *arq_bin, char *buffer);

// Lê os critérios de busca (ou edição) em um vetor de PARES
void ler_pares(PAR *vetor_pares, int num_pares, char *campo);

// Mensagem de erro usada nas funcionalidades 4 e 9
void print_reg_inexistente();

#endif