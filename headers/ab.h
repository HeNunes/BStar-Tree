#include "registros.h"
#include "modulos_ab.h"

#ifndef AB_H
#define AB_H

// Inicializa o CABECALHO da AROVRE B
void inicializa_cab_ab(CAB_AB *cab_ab);

// Le o CABECALHO do arquivo de index (ARVORE B)
void ler_cab_ab(CAB_AB *cab_ab, FILE *arq_ind);

// Escreve o CABECALHO do arquivo de index (ARVORE B)
void escreve_cab_ab(CAB_AB *cab_ab, FILE *arq_ind);

// Le uma PAGINA da ARVORE B
void ler_pagina(PAGINA *pag, FILE *arq_ind);

// Escreve uma PAGINA na ARVORE B
void escreve_pagina(PAGINA *pag, FILE *arq_ind);

// Busca um REGISTRO na ARVORE B e retorna seu ByteOffset
long int ab_buscar(PAGINA *raiz, int chave_busca, FILE *arq_ind);

// Insere uma nova CHAVE na ARVORE B
void insere(CABECALHO *cab, CAB_AB *cab_ab, PAGINA *raiz, CHAVE_AB no_inserido, FILE *arq_ind);

#endif