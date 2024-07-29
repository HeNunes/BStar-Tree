#include "registros.h"

#ifndef MODULOS_AB_H
#define MODULOS_AB_H

// Defines
#define ORDEM 5
#define NIL -1
#define TAM_PAG 76

// CABECALHO da ARVORE B
typedef struct cab_ab_t
{
    char status;    // Status do CABECALHO
    int raiz;       // RRN da raiz
    int prox_RRN;   // Proximo RRN 
    int num_niveis; // Numero de niveis da ARVORE B
    int num_chaves; // Numero de chaves da ARVORE B
} CAB_AB;

// CHAVE da ARVORE B, referencia um REGISTRO no arquivo de dados
typedef struct chave_ab_t
{
    int valor;           // Valor da CHAVE (ID do REGISTRO)
    long int ByteOffset; // Byteoffset do REGISTRO no arquivo de dados
} CHAVE_AB;

// PAGINA de disco da ARVORE B
typedef struct pagina_t
{
    int ocupados;               // Numero de CHAVEs ocupados
    int nivel;                  // Nivel da PAGINA 
    int RRN_filhos[ORDEM];      // Ponteiros para as PAGINAs filhas
    CHAVE_AB chaves[ORDEM - 1]; // CHAVEs armazenados na PAGINA
} PAGINA;

// Struct auxiliar utilizada para armazenar ponteiros para as PAGINAs da ARVORE B
typedef struct rrns_t
{
    int RRN_mae;    // Ponteiro para a PAGINA mãe
    int RRN_filha;  // Ponteiro para a PAGINA filha
    int RRN_esq;    // Ponteiro para a PAGINA irmã esquerda
    int RRN_dir;    // Ponteiro para a PAGINA irmã direita
} RRNS;

// Struct auxiliar utilizada para a inserção e propagação de split
typedef struct prop_t
{
    CHAVE_AB chave;    // CHAVE à ser promovida
    int flag;          // Flag de propagação de split (assume 1 quando uma CHAVE deve ser inserida)
    int RRN_propagado; // RRN da PAGINA propagada durante o split (assume NIL quando não há propagação)
} PROP;


// Troca o conteúdo de duas CHAVEs
void swap_chave(CHAVE_AB *a, CHAVE_AB *b);
// Troca o conteúdo de dois inteiros
void swap_int(int *a, int *b);

// Funções auxiliares de busca na PAGINA:

// Buscando uma chave na PAGINA de disco através de busca binaria
long int busca_bin_chave(PAGINA atual, int inicio, int fim, int chave_busca);
// Retorna, dado uma chave, o RRN da PAGINA filha à ser visitada através de busca binária
int busca_bin_RRN(PAGINA pag, int chave);

// Funções auxiliares utilizadas nas subrotinas de Split e redistribuição

// Inicializa um vetor de CHAVEs com NIL
void inicializa_vetor_chaves(CHAVE_AB *vec_chaves, int tam);
// Inicializa um vetor de inteiros com NIL
void inicializa_vec_int(int *vec, int tam);
// Preenche um vetor com as CHAVEs de uma PAGINA
void preenche_vetor_chaves_com_pagina(CHAVE_AB *vec_chaves, PAGINA pag, int inicio, int tam);
// Preenche um vetor com os RRNs das filhas de uma PAGINA
void preenche_vetor_rrn_com_pagina(int *vec_rrns, PAGINA pag, int inicio, int tam);
// Preenche uma PAGINA com as informações armazenadas em vetores auxiliares
void preenche_pagina_com_vetores(CHAVE_AB *vec_chaves, int *vec_RRNs, PAGINA *pag, int inicio, int fim);
// Re-ordena os vetores auxiliares após a inserção de uma nova CHAVE
void re_ordena_vetores(CHAVE_AB *vec_chaves, int *vec_RRNs, int inicio, int fim);


// Funções auxiliares para a manipulação das PAGINAs

// Inicializa uma PAGINA com NIL
void inicializa_pagina(PAGINA *pag);

// Obtém os RRNs das PAGINAs irmãs de uma PAGINA da ARVORE B*
void get_RRN_irmas(RRNS *ret, PAGINA atual, int indice);

#endif