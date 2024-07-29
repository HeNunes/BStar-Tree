#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliar.h"
#include "registros.h"
#include "modulos_ab.h"

// Troca o conteúdo de duas CHAVEs
void swap_chave(CHAVE_AB *a, CHAVE_AB *b)
{
    CHAVE_AB aux = *a;
    *a = *b;
    *b = aux;
}
// Troca o conteúdo de dois inteiros
void swap_int(int *a, int *b)
{
    int aux = *a;
    *a = *b;
    *b = aux;
}

// Inicializa uma PAGINA com NIL
void inicializa_pagina(PAGINA *pag)
{
    // Inicializando os campos da PAGINA com NIL
    for(int i = 0; i < ORDEM - 1; i++)
    {
        pag->chaves[i].valor = NIL;
        pag->chaves[i].ByteOffset = NIL;
        pag->RRN_filhos[i] = NIL;
    }
    pag->RRN_filhos[ORDEM - 1] = NIL;
    pag->nivel = NIL;
    pag->ocupados = 0;
}

// Buscando uma chave na PAGINA de disco através de busca binaria
long int busca_bin_chave(PAGINA atual, int inicio, int fim, int chave_busca)
{
    int meio = (inicio + fim) / 2;

    if(inicio > fim) // Caso base, a chave não existe na PAGINA
        return -1;
    
    // Caso a chave seja encontrada, retornamos o Byteoffset do REGISTRO
    if(atual.chaves[meio].valor == chave_busca) 
        return atual.chaves[meio].ByteOffset;

    // Reiniciando a recursao para fração adequada da PAGINA
    if(atual.chaves[meio].valor > chave_busca)
        return busca_bin_chave(atual, inicio, meio - 1, chave_busca);
    else
        return busca_bin_chave(atual, meio + 1, fim, chave_busca);
}

// Realiza a etapa recursiva da busca binária e retorna o RRN da PAGINA filha
int busca_bin_RRN_rec(PAGINA pag, int chave, int inicio, int fim)
{
    int meio = (inicio + fim) / 2;

    // Caso base (a filha não existe na PAGINA)
    if(inicio >= fim)
    { 
        // Caso a CHAVE mãe seja menor que a CHAVE buscada
        if(pag.chaves[meio].valor > chave)
            return meio; // Retorna o RRN esquerdo da CHAVE mãe

        else // Caso contrário
            return meio + 1; // Retorna o RRN direito da CHAVE mãe
    }

    if(pag.chaves[meio].valor > chave)
        return busca_bin_RRN_rec(pag, chave, inicio, meio - 1);
    
    else
        return busca_bin_RRN_rec(pag, chave, meio + 1, fim);
}

// Retorna, dado uma chave, o RRN da PAGINA filha à ser visitada através de busca binária
int busca_bin_RRN(PAGINA pag, int chave)
{
    return busca_bin_RRN_rec(pag, chave, 0, pag.ocupados - 1);
}


// Inicializa um vetor de CHAVEs com NIL
void inicializa_vetor_chaves(CHAVE_AB *vec_chaves, int tam)
{
    for(int i = 0; i < tam; i++)
    {
        vec_chaves[i].valor = NIL;
        vec_chaves[i].ByteOffset = NIL;
    }
}
// Inicializa um vetor de inteiros com NIL
void inicializa_vec_int(int *vec, int tam)
{
    for(int i = 0; i < tam; i++)
        vec[i] = NIL;
}

// Preenche um vetor com as CHAVEs de uma PAGINA
void preenche_vetor_chaves_com_pagina(CHAVE_AB *vec_chaves, PAGINA pag, int inicio, int tam)
{
    for(int i = 0; i < tam; i++)
        vec_chaves[i + inicio] = pag.chaves[i];
}
// Preenche um vetor com os RRNs das filhas de uma PAGINA
void preenche_vetor_rrn_com_pagina(int *vec_rrns, PAGINA pag, int inicio, int tam)
{
    for(int i = 0; i < tam; i++)
        vec_rrns[i + inicio] = pag.RRN_filhos[i];
}

// Preenche uma PAGINA com as informações armazenadas em vetores auxiliares
void preenche_pagina_com_vetores(CHAVE_AB *vec_chaves, int *vec_RRNs, PAGINA *pag, int inicio, int fim)
{
    for(int i = inicio; i < fim; i++)
    {
        pag->chaves[i - inicio] = vec_chaves[i];
        pag->RRN_filhos[i - inicio] = vec_RRNs[i];
        if(vec_chaves[i].valor != -1)
            pag->ocupados++; 
    }
}

// Re-ordena os vetores auxiliares após a inserção de uma nova CHAVE
void re_ordena_vetores(CHAVE_AB *vec_chaves, int *vec_RRNs, int inicio, int fim)
{
    // Iterando nos vetores 
    for(int i = inicio; i > fim; i--)
    {
        // Caso a CHAVE atual seja menor que a anterior
        if(vec_chaves[i].valor < vec_chaves[i - 1].valor)
        {
            // Trocando as CHAVEs os RRNs de suas PAGINAs filhas direitas
            swap_chave(&vec_chaves[i], &vec_chaves[i - 1]);
            swap_int(&vec_RRNs[i + 1], &vec_RRNs[i]);
        }
    }
}

// Obtém os RRNs das PAGINAs irmãs de uma PAGINA da ARVORE B*
void get_RRN_irmas(RRNS *ret, PAGINA atual, int indice)
{
    // Caso a PAGINA não seja a primeira filha, obtemos o RRN da irmã esquerda
    if(indice > 0) 
        ret->RRN_esq = atual.RRN_filhos[indice - 1];
    // Caso contrário, o RRN da irmã esquerda é NIL
    else           
        ret->RRN_esq = -1;
    
    // Caso a PAGINA não seja a última filha, obtemos o RRN da irmã direita
    if(indice < ORDEM - 1) 
        ret->RRN_dir = atual.RRN_filhos[indice + 1];
    // Caso contrário, o RRN da irmã direita é NIL
    else                   
        ret->RRN_dir = -1;
}
