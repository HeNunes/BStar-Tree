#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "auxiliar.h"
#include "registros.h"
#include "modulos_ab.h"
#include "ab.h"

// Inicializa o CABECALHO da AROVRE B
void inicializa_cab_ab(CAB_AB *cab_ab)
{
    cab_ab->status = INCONSISTENTE;
    cab_ab->raiz = NIL;
    cab_ab->prox_RRN = 0;
    cab_ab->num_chaves = 0;
    cab_ab->num_niveis = 0;
}

// Le o CABECALHO do arquivo de index (ARVORE B*)
void ler_cab_ab(CAB_AB *cab_ab, FILE *arq_ind)
{
    char buffer[60];
    fread(&(cab_ab->status), sizeof(char), 1, arq_ind);
    fread(&(cab_ab->raiz), sizeof(int), 1, arq_ind);
    fread(&(cab_ab->prox_RRN), sizeof(int), 1, arq_ind);
    fread(&(cab_ab->num_niveis), sizeof(int), 1, arq_ind);
    fread(&(cab_ab->num_chaves), sizeof(int), 1, arq_ind);
    fread(buffer, sizeof(char), 59, arq_ind);
}

// Escreve o CABECALHO do arquivo de index (ARVORE B*)
void escreve_cab_ab(CAB_AB *cab_ab, FILE *arq_ind)
{
    char buffer[59];
    for(int i = 0; i < 59; i++)
        buffer[i] = LIXO;

    fwrite(&(cab_ab->status), sizeof(char), 1, arq_ind); 
    fwrite(&(cab_ab->raiz), sizeof(int), 1, arq_ind); 
    fwrite(&(cab_ab->prox_RRN), sizeof(int), 1, arq_ind); 
    fwrite(&(cab_ab->num_niveis), sizeof(int), 1, arq_ind); 
    fwrite(&(cab_ab->num_chaves), sizeof(int), 1, arq_ind); 
    fwrite(buffer, sizeof(char), 59, arq_ind);
}

// Le uma PAGINA da ARVORE B*
void ler_pagina(PAGINA *pag, FILE *arq_ind)
{
    fread(&(pag->nivel), sizeof(int), 1, arq_ind);
    fread(&(pag->ocupados), sizeof(int), 1, arq_ind);

    for(int i = 0; i < ORDEM - 1; i++)
    {
        fread(&(pag->RRN_filhos[i]), sizeof(int), 1, arq_ind);
        fread(&(pag->chaves[i].valor), sizeof(int), 1, arq_ind);
        fread(&(pag->chaves[i].ByteOffset), sizeof(long int), 1, arq_ind);
    }
    fread(&(pag->RRN_filhos[4]), sizeof(int), 1, arq_ind);
}

// Escreve uma PAGINA no arquivo de index (ARVORE B*)
void escreve_pagina(PAGINA *pag, FILE *arq_ind)
{
    fwrite(&(pag->nivel), sizeof(int), 1, arq_ind);
    fwrite(&(pag->ocupados), sizeof(int), 1, arq_ind);

    for(int i = 0; i < ORDEM - 1; i++)
    {
        fwrite(&(pag->RRN_filhos[i]), sizeof(int), 1, arq_ind);
        fwrite(&(pag->chaves[i].valor), sizeof(int), 1, arq_ind);
        fwrite(&(pag->chaves[i].ByteOffset), sizeof(long int), 1, arq_ind);
    }
    fwrite(&(pag->RRN_filhos[4]), sizeof(int), 1, arq_ind);
}

// Escreve duas PAGINAs irmãs e a PAGINA mãe das mesmas no arquivo de index (ARVORE B*)
void re_escreve_paginas(PAGINA *pag_esq, PAGINA *pag_dir, PAGINA *pag_mae,
                        int RRN_esq, int RRN_dir, int RRN_mae, FILE *arq_ind)
{
    // Reescrevendo as PAGINAs no arquivo
    fseek(arq_ind, (RRN_mae + 1) * TAM_PAG, SEEK_SET);
    escreve_pagina(pag_mae, arq_ind);
    fseek(arq_ind, (RRN_esq + 1) * TAM_PAG, SEEK_SET);
    escreve_pagina(pag_esq, arq_ind);
    fseek(arq_ind, (RRN_dir + 1) * TAM_PAG, SEEK_SET);
    escreve_pagina(pag_dir, arq_ind);
}


// Cria a PAGINA raiz da ARVORE B*
void criar_raiz(CABECALHO *cab, CAB_AB *cab_ab, CHAVE_AB inserida, FILE *arq_ind)
{
    PAGINA raiz;
    
    // Criando a raiz e inicializando-a com NIL
    inicializa_pagina(&raiz);
    
    // Inserindo o primeiro NO na raiz da ARVORE B*
    raiz.nivel = 1;
    raiz.ocupados = 1;
    raiz.chaves[0] = inserida;

    // Atualizando o CABECALHO da ARVORE B*
    cab_ab->num_chaves = 1;
    cab_ab->num_niveis = 1;
    cab_ab->prox_RRN++;
    cab_ab->raiz++;

    // Escrevendo a raiz no arquivo de index (ARVORE B*)
    fseek(arq_ind, (cab_ab->raiz + 1) * TAM_PAG, SEEK_SET);
    escreve_pagina(&raiz, arq_ind);
}

// Insere uma nova CHAVE na PAGINA 
void insercao_simples(PAGINA *pag, int RRN, CHAVE_AB inserida, FILE *arq_ind)
{
    // Inserindo a CHAVE na ultima posição da PAGINA
    pag->chaves[ORDEM - 2] = inserida;
    
    // Posicionando a CHAVE inserida em sua devida posição no vetor de CHAVEs
    for(int i = ORDEM - 2; i > 0; i--)
    {
        if(pag->chaves[i].valor < pag->chaves[i - 1].valor || pag->chaves[i - 1].valor == -1)
            swap_chave(&(pag->chaves[i]), &(pag->chaves[i - 1]));
    }

    pag->ocupados++; // Incrementando o numero de ocupação

    // Reescrevendo a PAGINA no arquivo de index (ARVORE B*)
    fseek(arq_ind, (RRN + 1) * TAM_PAG, SEEK_SET);
    escreve_pagina(pag, arq_ind);
}

// Realiza o Split 1 para 2 na raiz da ARVORE B*
void split_1_2(CABECALHO *cab, CAB_AB *cab_ab, PAGINA *raiz, CHAVE_AB inserida, 
                FILE *arq_ind, PROP prop_split)
{
    PAGINA f_esq, f_dir, nova_raiz;
    CHAVE_AB vec_chaves[ORDEM], nova_chave;
    int vec_RRNs[ORDEM + 1];

    // Inicializando a CHAVE à ser inserida
    nova_chave = inserida;

    // Gerando o vetor intermediario de CHAVEs
    preenche_vetor_chaves_com_pagina(vec_chaves, *raiz, 0, ORDEM - 1);
    vec_chaves[ORDEM - 1] = nova_chave; 

    // Gerando o vetor intermediario de RRNs (filhas da raiz)
    preenche_vetor_rrn_com_pagina(vec_RRNs, *raiz, 0, ORDEM);
    vec_RRNs[ORDEM] = prop_split.RRN_propagado;

    // Reeordenando os vetores após a inserção da nova CHAVE
    re_ordena_vetores(vec_chaves, vec_RRNs, ORDEM - 1, 0);

    CHAVE_AB promovida = vec_chaves[ORDEM / 2]; // A CHAVE média será promovido à nova raiz

    // Inicializando as PAGINAs com NIL
    inicializa_pagina(&f_esq);
    inicializa_pagina(&f_dir);
    inicializa_pagina(&nova_raiz);

    // Distribuíndo as chaves e ponteiros entre as paginas irmãs
    preenche_pagina_com_vetores(vec_chaves, vec_RRNs, &f_esq, 0, ORDEM / 2);
    preenche_pagina_com_vetores(vec_chaves, vec_RRNs, &f_dir, (ORDEM / 2) + 1, ORDEM);
    f_esq.RRN_filhos[ORDEM / 2] = vec_RRNs[ORDEM / 2];
    f_dir.RRN_filhos[ORDEM / 2] = vec_RRNs[ORDEM];

    // Calculando os RRNs das PAGINAs no arquivo de index (ARVORE B*)
    int RRN_esq = cab_ab->raiz, RRN_dir = cab_ab->prox_RRN, RRN_nova_raiz = cab_ab->prox_RRN + 1;

    // Atualizando o CABECALHO da ARVORE B*
    cab_ab->raiz = RRN_nova_raiz;
    cab_ab->num_niveis++;
    cab_ab->prox_RRN += 2;

    // Criando a nova raiz 
    nova_raiz.nivel = cab_ab->num_niveis;
    nova_raiz.ocupados = 1;
    nova_raiz.chaves[0] = promovida;
    nova_raiz.RRN_filhos[0] = RRN_esq;
    nova_raiz.RRN_filhos[1] = RRN_dir;

    // Determinando o nivel das PAGINAs
    f_esq.nivel = cab_ab->num_niveis - 1;
    f_dir.nivel = cab_ab->num_niveis - 1;

    // Reescrevendo as PAGINAs no arquivo
    re_escreve_paginas(&f_esq, &f_dir, &nova_raiz, RRN_esq, RRN_dir, RRN_nova_raiz, arq_ind);

    *raiz = nova_raiz; // Atualizando a PAGINA raiz
}

// Realiza a redistribuição entre duas PAGINAs irmãs da ARVORE B*
void redistribui(CAB_AB *cab_ab, PAGINA *pag_esq, PAGINA *pag_dir, PAGINA *mae, PAGINA *raiz, 
                int RRN_mae, int RRN_dir, int RRN_esq, 
                CHAVE_AB inserida, FILE *arq_ind, PROP prop_split)
{
    int qtd_esq = pag_esq->ocupados, qtd_dir = pag_dir->ocupados, total_chaves = qtd_esq + qtd_dir + 2;
    CHAVE_AB vec_chaves[total_chaves];
    int vec_RRNs[total_chaves + 1];

    // Inicializando os vetores intermediarios
    inicializa_vetor_chaves(vec_chaves, total_chaves);
    inicializa_vec_int(vec_RRNs, total_chaves);

    // Encontra o indice da CHAVE mãe das PAGINAs irmãs 
    int indice_mae = busca_bin_RRN(*mae, pag_dir->chaves[0].valor) - 1;

    // Inicializando a CHAVE mãe e a CHAVE a ser inserida
    CHAVE_AB chave_mae = mae->chaves[indice_mae];
    CHAVE_AB nova_chave = inserida;

    // Gerando o vetor intermediario de CHAVEs
    preenche_vetor_chaves_com_pagina(vec_chaves, *pag_esq, 0, qtd_esq);
    vec_chaves[qtd_esq] = chave_mae;
    preenche_vetor_chaves_com_pagina(vec_chaves, *pag_dir, qtd_esq + 1, qtd_dir);
    vec_chaves[total_chaves - 1] = nova_chave;

    // Gerando o vetor intermediario de RRNs
    preenche_vetor_rrn_com_pagina(vec_RRNs, *pag_esq, 0, qtd_esq + 1);    
    preenche_vetor_rrn_com_pagina(vec_RRNs, *pag_dir, qtd_esq + 1, qtd_dir + 1);
    vec_RRNs[total_chaves] = prop_split.RRN_propagado;

    // Re-ordenando os vetores
    re_ordena_vetores(vec_chaves, vec_RRNs, total_chaves - 1, 0);

    int med = total_chaves / 2;
    CHAVE_AB chave_promovida = vec_chaves[med]; // A CHAVE média será promovida à PAGINA mãe
    PAGINA nova_esq, nova_dir, nova_mae;

    // Inicializando as PAGINAs 
    inicializa_pagina(&nova_esq);
    inicializa_pagina(&nova_dir);
    nova_mae = *mae;
    nova_esq.nivel = pag_esq->nivel;
    nova_dir.nivel = pag_dir->nivel;

    // Distribuindo as chaves e ponteiros entre as PAGINAs irmãs
    preenche_pagina_com_vetores(vec_chaves, vec_RRNs, &nova_esq, 0, med);
    nova_esq.RRN_filhos[med] = vec_RRNs[med];
    preenche_pagina_com_vetores(vec_chaves, vec_RRNs, &nova_dir, med + 1, total_chaves);
    nova_dir.RRN_filhos[nova_dir.ocupados] = vec_RRNs[total_chaves];

    // Promovendo a CHAVE adequada
    nova_mae.chaves[indice_mae] = chave_promovida;

    // Escrevendo as paginas no arquivo de INDEX (ARVORE B*)
    re_escreve_paginas(&nova_esq, &nova_dir, &nova_mae, RRN_esq, RRN_dir, RRN_mae, arq_ind);

    // Atualizando as variaveis
    *mae = nova_mae;
    if(mae->nivel == cab_ab->num_niveis)
        *raiz = nova_mae;
}

// Realiza o Split 2 para 3 entre duas PAGINAs irmãs da ARVORE B*
PROP split_2_3(CAB_AB *cab_ab, PAGINA *pag_esq, PAGINA *pag_dir, PAGINA *mae, PAGINA *raiz,
                int RRN_mae, int RRN_dir, int RRN_esq, 
                CHAVE_AB inserida, FILE *arq_ind, PROP prop_split)
{
    int num_chaves = 2 * ORDEM, num_RRNs = (2 * ORDEM) + 1;
    CHAVE_AB vec_chaves[num_chaves];
    int vec_RRNs[num_RRNs];

    // Inicializando os vetores intermediarios
    inicializa_vetor_chaves(vec_chaves, num_chaves);
    inicializa_vec_int(vec_RRNs, num_RRNs);

    // Encontra o indice da CHAVE mãe das PAGINAs irmãs 
    int indice_mae = busca_bin_RRN(*mae, pag_dir->chaves[0].valor) - 1;
    
    // Inicializando a CHAVE mãe e a CHAVE a ser inserida
    CHAVE_AB chave_mae = mae->chaves[indice_mae];
    CHAVE_AB nova_chave = inserida;

    // Gerando o vetor intermediario de CHAVEs
    preenche_vetor_chaves_com_pagina(vec_chaves, *pag_esq, 0, ORDEM - 1);
    vec_chaves[ORDEM - 1] = chave_mae;
    preenche_vetor_chaves_com_pagina(vec_chaves, *pag_dir, ORDEM, ORDEM - 1);
    vec_chaves[num_chaves - 1] = nova_chave;

    // Gerando o vetor intermediario de RRNs
    preenche_vetor_rrn_com_pagina(vec_RRNs, *pag_esq, 0, ORDEM);
    preenche_vetor_rrn_com_pagina(vec_RRNs, *pag_dir, ORDEM, ORDEM);
    vec_RRNs[num_RRNs - 1] = prop_split.RRN_propagado;

    // Re-ordenando os vetores 
    re_ordena_vetores(vec_chaves, vec_RRNs, num_chaves - 1, 0);

    PAGINA nova_esq, nova_dir, nova_mae, pagina_criada;

    // Inicializando as PAGINAs filhas
    inicializa_pagina(&nova_esq);
    inicializa_pagina(&nova_dir);
    inicializa_pagina(&pagina_criada);

    // Atualizando as variáveis
    nova_mae = *mae;
    nova_esq.nivel = pag_esq->nivel;
    nova_dir.nivel = pag_dir->nivel;
    pagina_criada.nivel = pag_esq->nivel;

    int t1 = (num_chaves + 1) / 3;       // Primeiro terço
    int t2 = ((2 * num_chaves) + 1) / 3; // Segundo terço

    // Distribuindo as CHAVEs e ponteiros entre as PAGINAs irmãs
    preenche_pagina_com_vetores(vec_chaves, vec_RRNs, &nova_esq, 0, t1);
    nova_esq.RRN_filhos[3] = vec_RRNs[t1];
    preenche_pagina_com_vetores(vec_chaves, vec_RRNs, &nova_dir, t1 + 1, t2);
    nova_dir.RRN_filhos[3] = vec_RRNs[t2];
    preenche_pagina_com_vetores(vec_chaves, vec_RRNs, &pagina_criada, t2 + 1, num_chaves);
    pagina_criada.RRN_filhos[2] = vec_RRNs[num_RRNs - 1];

    // Promovendo a CHAVE adequada
    nova_mae.chaves[indice_mae] = vec_chaves[t1]; 

    // A PAGINA criada será escrita no fim do arquivo de index (ARVORE B*)
    int RRN_nova_pag = cab_ab->prox_RRN;
    cab_ab->prox_RRN++;

    PROP retorno;
    CHAVE_AB promovida = vec_chaves[t2]; // Inicializando a CHAVE promovida à ser inserida na PAGINA mãe

    // Caso a PAGINA mãe não esteja cheia
    if(nova_mae.ocupados < ORDEM - 1)
    {
        // Inserindo a CHAVE promovida na PAGINA mãe
        nova_mae.chaves[nova_mae.ocupados] = promovida;
        nova_mae.RRN_filhos[nova_mae.ocupados + 1] = RRN_nova_pag;
        re_ordena_vetores(nova_mae.chaves, nova_mae.RRN_filhos, nova_mae.ocupados, 0);
        nova_mae.ocupados++;
        
        // Sinalizando para o restante da inserção que não houve propagação
        retorno.chave.valor = NIL;
        retorno.chave.ByteOffset = NIL;
        retorno.flag = 0;
        retorno.RRN_propagado = NIL;
    }
    else
    {
        // Propagando a CHAVE promovida e a PAGINA criada no split
        retorno.chave = promovida;
        retorno.flag = 1;
        retorno.RRN_propagado = RRN_nova_pag;
    }

    // Atualizando as variaveis
    if(nova_mae.nivel == cab_ab->num_niveis)
        *raiz = nova_mae;
    *mae = nova_mae;
    *pag_esq = nova_esq;
    *pag_dir = nova_dir;

    // Reescrevendo as PAGINAs no arquivo de index (ARVORE B*)
    re_escreve_paginas(&nova_esq, &nova_dir, &nova_mae, RRN_esq, RRN_dir, RRN_mae, arq_ind);
    fseek(arq_ind, (RRN_nova_pag + 1) * TAM_PAG, SEEK_SET);
    escreve_pagina(&pagina_criada, arq_ind);

    return retorno;
}

// Etapa recursiva da função de inserção
PROP ab_inserir(CABECALHO *cab, CAB_AB *cab_ab, PAGINA *raiz, PAGINA atual, 
                CHAVE_AB inserida, FILE *arq_ind, RRNS pag_RRNs)
{
    PAGINA mae;
    PROP prop_flag;
    
    // Inicializando a propagação:
    prop_flag.flag = 1;           // Uma CHAVE deve ser inserida
    prop_flag.chave = inserida;   // Inicializando a CHAVE
    prop_flag.RRN_propagado = -1; // Não há RRN propagado (ainda não houve split)

    // Buscando a PAGINA filha que deve ser visitada na próxima etapa da recursão
    int pos_filha = busca_bin_RRN(atual, inserida.valor);

    // Obtendo os RRNs das PAGINAs irmãs (caso existam)
    if(atual.nivel > 1)
        get_RRN_irmas(&pag_RRNs, atual, pos_filha);

    // Caso a PAGINA atual não seja uma folha
    if(atual.nivel > 1)
    {
        // Atualizando as variáveis para a próxima etapa da recursão
        mae = atual;
        pag_RRNs.RRN_mae = pag_RRNs.RRN_filha;        
        pag_RRNs.RRN_filha = atual.RRN_filhos[pos_filha];
        fseek(arq_ind, (atual.RRN_filhos[pos_filha] + 1) * TAM_PAG, SEEK_SET);
        ler_pagina(&atual, arq_ind);
    }
    else // Caso contrário, retornamos a propagação para que a CHAVE seja inserida
    {
        return prop_flag;
    }

    PROP propagacao = ab_inserir(cab, cab_ab, raiz, atual, inserida, arq_ind, pag_RRNs);

    // Caso uma CHAVE deva ser inserida
    if(propagacao.flag == 1)
    {
        // Lendo as PAGINAs atual e mãe 
        fseek(arq_ind, (pag_RRNs.RRN_mae + 1) * TAM_PAG, SEEK_SET);
        ler_pagina(&mae, arq_ind);
        fseek(arq_ind, (pag_RRNs.RRN_filha + 1) * TAM_PAG, SEEK_SET);
        ler_pagina(&atual, arq_ind);
        
        // Caso a PAGINA não esteja cheia
        if(atual.ocupados < ORDEM - 1)
        {
            // Inserindo a CHAVE e sinalizando que não há propagação
            insercao_simples(&atual, pag_RRNs.RRN_filha, propagacao.chave, arq_ind);

            propagacao.flag = 0;
            return propagacao;
        }
        else // Caso a PAGINA esteja cheia
        {
            PAGINA esq, dir;
            esq.ocupados = -1;
            dir.ocupados = -1;

            // Caso haja irmã esquerda
            if(pag_RRNs.RRN_esq != -1)
            {
                // Lendo a PAGINA irmã esquerda
                fseek(arq_ind, (pag_RRNs.RRN_esq + 1) * TAM_PAG, SEEK_SET);
                ler_pagina(&esq, arq_ind);
            }
            // Caso haja irmã direita
            if(pag_RRNs.RRN_dir != -1)
            {
                // Lendo a PAGINA irmã direita
                fseek(arq_ind, (pag_RRNs.RRN_dir + 1) * TAM_PAG, SEEK_SET);
                ler_pagina(&dir, arq_ind);
            }
            // Caso a PAGINA irmã esquerda exista e não esteja cheia
            if(esq.ocupados < ORDEM - 1 && esq.ocupados != -1)
            {
                // Realizando redistribuição com a PAGINA irmã esquerda e 
                // sinalizando que não há propagação
                redistribui(cab_ab, &esq, &atual, &mae, raiz, pag_RRNs.RRN_mae, pag_RRNs.RRN_filha, 
                            pag_RRNs.RRN_esq, propagacao.chave, arq_ind, propagacao);
                
                propagacao.flag = 0;
                return propagacao;
            }
            // Caso a PAGINA irmã direita exista e não esteja cheia
            else if(dir.ocupados < ORDEM - 1 && dir.ocupados != -1)
            {
                // Realizando redistribuição com a PAGINA irmã direita e 
                // sinalizando que não há propagação
                redistribui(cab_ab, &atual, &dir, &mae, raiz, pag_RRNs.RRN_mae, pag_RRNs.RRN_dir, 
                            pag_RRNs.RRN_filha, propagacao.chave, arq_ind, propagacao);
                
                propagacao.flag = 0;
                return propagacao;
            }
            // Caso exista PAGINA irmã direita
            else if(pag_RRNs.RRN_dir != -1)
            {
                // Realizando Split 2 para 3 com a PAGINA irmã direita
                return split_2_3(cab_ab, &atual, &dir, &mae, raiz, pag_RRNs.RRN_mae, pag_RRNs.RRN_dir, 
                                pag_RRNs.RRN_filha, propagacao.chave, arq_ind, propagacao);
            }
            // Caso exista somente PAGINA irmã esquerda
            else if(pag_RRNs.RRN_esq != -1)
            {
                // Realizando Split 2 para 3 com a PAGINA irmã esquerda
                return split_2_3(cab_ab, &esq, &atual, &mae, raiz, pag_RRNs.RRN_mae, pag_RRNs.RRN_filha,
                                pag_RRNs.RRN_esq, propagacao.chave, arq_ind, propagacao);
            }
        }
    }
    return propagacao;
}

// Insere um NO na ARVORE B*
void insere(CABECALHO *cab, CAB_AB *cab_ab, PAGINA *raiz, CHAVE_AB inserida, FILE *arq_ind)
{
    PAGINA atual = *raiz;
    RRNS pag_RRNs;

    // Inicializando os RRNs em relação à raiz
    pag_RRNs.RRN_filha = cab_ab->raiz;
    pag_RRNs.RRN_dir = NIL;
    pag_RRNs.RRN_esq = NIL;
    pag_RRNs.RRN_mae = NIL;

    // Caso a ARVORE B* esteja vazia
	if(cab_ab->num_chaves == 0 || cab_ab->raiz == NIL) 
    {
        // Criando e lendo a raiz da ARVORE B*
        criar_raiz(cab, cab_ab, inserida, arq_ind);
        fseek(arq_ind, (cab_ab->raiz + 1) * TAM_PAG, SEEK_SET);
        ler_pagina(raiz, arq_ind);
    }
    // Caso contrario
    else
    {
        // Caso a ARVORE B* apresente apenas a raiz, e a mesma não esteja cheia
        if(cab_ab->num_niveis == 1 && raiz->ocupados < ORDEM - 1)
        {
            // Inserindo o NO na PAGINA raiz
            insercao_simples(raiz, cab_ab->raiz, inserida, arq_ind);
        }
        else
        {
            // Inserindo o NO na ARVORE B*
            PROP propagacao = ab_inserir(cab, cab_ab, raiz, atual, inserida, arq_ind, pag_RRNs);

            // Caso tenha ocorrido propagação de split até a raiz
            if(propagacao.flag == 1)
            {
                // Realizando o split 1 para 2 na raiz da ARVORE B*
                split_1_2(cab, cab_ab, raiz, propagacao.chave, arq_ind, propagacao);
            }
        }
    }
}

// Busca uma chave na ARVORE B*
long int ab_buscar(PAGINA *atual, int chave_busca, FILE *arq_ind)
{
    // Procurando a chave buscada por busca binária
    long int RRN_busca = busca_bin_chave(*atual, 0, ORDEM - 2, chave_busca);
    if(RRN_busca != -1)
        return RRN_busca;

    // Procura a posição do proximo filho
    int indice_filha = busca_bin_RRN(*atual, chave_busca);

    // Caso não haja próximo filho, retornamos NIL
    if(atual->RRN_filhos[indice_filha] == NIL)
        return NIL;

    // Descendo para a PAGINA filha e reiniciando a recursao
    fseek(arq_ind, ((atual->RRN_filhos[indice_filha] + 1) * TAM_PAG), SEEK_SET);
    ler_pagina(atual, arq_ind);

    return ab_buscar(atual, chave_busca, arq_ind);
}
