#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fornecido.h"
#include "auxiliar.h"
#include "registros.h"
#include "index.h"
#include "crimes.h"
#include "ab.h"

// Edita o REGISTRO passado como parâmetro
void edita_registro(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, REGISTRO *reg_edit, REGISTRO reg, CABECALHO *cab, PAR *vetor_edicoes, int num_edicoes, FILE *arq_bin, long int BOs_atual, char *campo);
// Monta o REGISTRO editado e, se necessário, modifica o vetor de INDEX
void montar_edicao(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, REGISTRO *reg_edit, REGISTRO reg, PAR *vetor_edicoes, int num_edicoes, int flag_cabe, long int ByteOffset, char *campo);

// Funcionalidade 1
void crimes_create(char *nome_arq_csv, char *nome_arq_bin)
{
    char linha[MAX_TAM_STRING];
    REGISTRO reg;
    CABECALHO cab;
    FILE *arq_csv = NULL, *arq_bin = NULL;

    // Inicializando o cabeçalho
    inicializa_cabecalho(&cab);

    // Abrindo o arquivo CSV, se possível
    arq_csv = fopen(nome_arq_csv, "r");
    checar_null(arq_csv);

    // Abrindo o arquivo binário, se possível
    arq_bin = fopen(nome_arq_bin, "wb+");
    checar_null(arq_bin);

    // Atualizando o status do CABECALHO e escrevendo no arquivo binário
    cab.status = INCONSISTENTE;
    escreve_cabecalho(&cab, arq_bin);

    // Pulando o cabecalho do CSV
    fgets(linha, sizeof(linha), arq_csv);

    // Lendo o CSV
    int contador = 0;
    while(!feof(arq_csv))
    {
        char buffer[MAX_TAM_STRING] = {'\0'};

        // Lendo um campo do arquivo CSV        
        ler_campo(arq_csv, buffer);

        // Um campo foi lido, incrementando o contador
        contador++;

        // Verificando qual campo foi lido e tratando o dado
        int op = contador % NUM_CAMPOS;
        switch(op)
        {
            case 1:
                ler_id(&reg, buffer); // id
                break;
            case 2:
                ler_data(&reg, buffer); // data
                break;
            case 3:
                ler_artigo(&reg, buffer); // artigo
                break;
            case 4:
                ler_local(&reg, buffer); // local
                break;
            case 5:
                ler_descricao(&reg, buffer); // descrição
                break;
            case 0:
                ler_marca(&reg, buffer); // marca
                
                // Um registro foi lido, escrevendo o registro no arquivo binário e atualizando a struct CABECALHO
                reg.removido = '0';
                escreve_registro(&reg, arq_bin);
                atualizar_cabecalho(&cab, reg.local, reg.descricao);
                break;
            default:
                break;
        }
    }
    // Leitura finalizada, atualizando o status do CABECALHO e reescrevendo-o no arquivo binário
    cab.status = CONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    // Fechando os arquivos
    fclose(arq_csv);
    fclose(arq_bin);
}

// Funcionalidade 2
void crimes_select_all(char *nome_arq_bin)
{
    REGISTRO reg;
    CABECALHO cab;
    int contador = 0;
    FILE *arq_bin = NULL;

    // Abrindo o arquivo binário, se possível
    arq_bin = fopen(nome_arq_bin, "rb");
    checar_null(arq_bin);

    // Lendo o CABECALHO
    bin_ler_cabecalho(&cab, arq_bin);

    // Verificando o status do CABECALHO
    if(cab.status == INCONSISTENTE)
        erro_arquivo();

    // Verificando se há REGISTROS no arquivo
    if(cab.NumRegArq == 0)
    {
        erro_registros();
        return;
    }

    printf("Prox BOs: %ld\nNum Reg Arq: %d\n Num Rem:%d\n", cab.proxByteOffset, cab.NumRegArq, cab.NumRegRem);

    // Lendo os registros
    while(contador < cab.NumRegArq)
    {
        // Lendo o registro
        bin_ler_registro(&reg, arq_bin);

        // Imprimindo o registro lido e incrementando o contador
        imprime_registro(reg);
        contador++;
    }

    // Fechando o arquivo binário
    fclose(arq_bin);
}

// Funcionalidade 3
void crimes_indexar(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind)
{
    FILE *arq_bin, *arq_index;
    CABECALHO cab;
    CAB_INDEX cab_ind;

    // Abrindo (se possível) o arquivo de dados
    arq_bin = fopen(nome_arq_bin, "rb");
    checar_null(arq_bin);

    // Lendo, se possível, o CABECALHO
    tentar_ler_cabecalho(&cab, &arq_bin); 

    // Abrindo (se possível) o arquivo de index
    arq_index = fopen(nome_arq_ind, "wb+");
    checar_null(arq_index);   

    // Escrevendo o CABECALHO no arquivo de INDEX
    cab_ind.status = INCONSISTENTE;
    cab_ind.NumRegs = 0;
    escreve_cab_index(&cab_ind, arq_index);

    if(strcmp(tipo_dado, "inteiro") == 0) // Indexação do tipo inteiro
    {
        // Alocando memória para um vetor de INDEX do tipo inteiro
        INT_INDEX *vec_index_int = malloc (cab.NumRegArq * sizeof(INT_INDEX));

        // Montando o vetor de INDEX
        montar_vec_int_index(vec_index_int, cab.NumRegArq, campo, arq_bin);

        // Iterando no vetor de INDEX
        for(int i = 0; i < cab.NumRegArq; i++)
        {
            // Caso sua chave não seja nula, escrevemos o INDEX no arquivo
            if(vec_index_int[i].chave != -1)
                escreve_int_index(&(vec_index_int[i]), &cab_ind, arq_index);
        }
        // Liberando a memória alocada
        free(vec_index_int);
    }
    else if(strcmp(tipo_dado, "string") == 0) // Indexação do tipo string
    {
        // Alocando memória para um vetor de INDEX do tipo string
        STR_INDEX *vec_index_str = malloc (cab.NumRegArq * sizeof(STR_INDEX));

        // Montando o vetor de INDEX
        montar_vec_str_index(vec_index_str, cab.NumRegArq, campo, arq_bin);

        // Iterando no vetor de INDEX
        for(int i = 0; i < cab.NumRegArq; i++)
        {
            // Caso sua chave não seja nula, escrevemos o INDEX no arquivo
            if(vec_index_str[i].chave[0] != LIXO)
                escreve_str_index(&(vec_index_str[i]), &cab_ind, arq_index);
        }
        // Liberando a memória alocada
        free(vec_index_str);
    }
    // Reescrevendo o CABECALHO no arquivo de index
    cab_ind.status = CONSISTENTE;
    fseek(arq_index, 0, SEEK_SET);
    escreve_cab_index(&cab_ind, arq_index);

    // Fechando os arquivos
    fclose(arq_index);
    fclose(arq_bin);
}

// As seguintes funções realizam a busca nos arquivos de dados conforme especificado no enunciado do trabalho.
// Para cada busca, é realizado um "switch case" para determinar a função de origem através de uma flag, a qual recebe o número correspondente à funcionalidade
// que chamou a função de busca.

// Realiza a rotina da função chamadora através de uma busca indexada do tipo inteiro
void rotina_indexada_int(INT_INDEX *vec_index_int, CAB_INDEX *cab_ind, CABECALHO *cab, PAR *vetor_pares, int num_pares, PAR *vetor_edicoes, int num_edicoes, FILE *arq_bin, char *campo, int flag)
{
    REGISTRO reg, reg_edit;

    // Buscando o primeiro INDEX que satisfaça o primeiro critério de busca (campo indexado)
    int pos = buscar_int_index(vec_index_int, cab_ind->NumRegs, vetor_pares[0].chave_int);
    int existe = 0; // Flag utilizada para apontar de algum REGISTRO atende aos critérios de busca (func. 4)

    if(pos != -1) // Caso o REGISTRO exista
    {
        // Enquanto houverem REGISTROS que satisfaçam a primeira condição das buscas
        while(pos < cab_ind->NumRegs && vec_index_int[pos].chave == vetor_pares[0].chave_int)
        {
            // Lendo o REGISTRO
            fseek(arq_bin, vec_index_int[pos].ByteOffset, SEEK_SET);
            bin_ler_registro(&reg, arq_bin);

            // Caso o REGISTRO satisfaça as condições de busca
            if(checar_busca(reg, vetor_pares, num_pares))
            {
                switch(flag) // Verificando qual subrotina deve ser realizada
                {
                    case 4: // Imprimindo o REGISTRO
                        imprime_registro(reg); 
                        existe = 1; // Algum REGISTRO atendeu as condições da busca
                        break;

                    case 5: // Removendo o REGISTRO e atribuíndo ao seu INDEX uma chave nula
                        remover_registro(arq_bin, cab, &reg, vec_index_int[pos].ByteOffset);
                        vec_index_int[pos].chave = -1;
                        break;

                    case 7: // Editando o REGISTRO
                        edita_registro(&vec_index_int, NULL, cab_ind, &reg_edit, reg, cab, vetor_edicoes, num_edicoes, arq_bin, vec_index_int[pos].ByteOffset, campo);
                        break;
                    
                default:
                    break;
                }
            }
            pos++;
        }
    }
    if(existe == 0 && flag == 4) // Caso nenhum REGISTRO tenha sido encontrado (func. 4)
        print_reg_inexistente();
}
// Realiza a rotina da função chamadora através de uma busca indexada do tipo string
void rotina_indexada_str(STR_INDEX *vec_index_str, CAB_INDEX *cab_ind, CABECALHO *cab, PAR *vetor_pares, int num_pares, PAR *vetor_edicoes, int num_edicoes, FILE *arq_bin, char *campo, int flag)
{
    REGISTRO reg, reg_edit;

    // Buscando o primeiro INDEX que satisfaça o primeiro critério de busca (campo indexado)
    int pos = buscar_str_index(vec_index_str, cab_ind->NumRegs, vetor_pares[0].chave_str);
    int existe = 0; // Flag utilizada para apontar de algum REGISTRO atende aos critérios de busca (func. 4)

    if(pos != -1) // Caso o REGISTRO exista
    {
        // Enquanto houverem REGISTROS que satisfaçam a primeira condição das buscas
        while(pos < cab_ind->NumRegs && strcmp(vec_index_str[pos].chave, vetor_pares[0].chave_str) == 0)
        {
            // Lendo o REGISTRO
            fseek(arq_bin, vec_index_str[pos].ByteOffset, SEEK_SET);
            bin_ler_registro(&reg, arq_bin);

            // Caso o REGISTRO satisfaça as condições de busca
            if(checar_busca(reg, vetor_pares, num_pares))
            {
                switch(flag) // Verificando qual subrotina deve ser realizada
                {
                    case 4: // Imprimindo o REGISTRO
                        imprime_registro(reg); 
                        existe = 1; // Algum REGISTRO atendeu as condições da busca
                        break;

                    case 5: // Removendo o REGISTRO e atribuíndo ao seu INDEX uma chave nula
                        remover_registro(arq_bin, cab, &reg, vec_index_str[pos].ByteOffset);
                        strcpy(vec_index_str[pos].chave, "$$$$$$$$$$$$");
                        break;

                    case 7: // Editando o REGISTRO
                        edita_registro(NULL, &vec_index_str, cab_ind, &reg_edit, reg, cab, vetor_edicoes, num_edicoes, arq_bin, vec_index_str[pos].ByteOffset, campo);
                        break;
                    
                default:
                    break;
                }
            }
            pos++;
        }
    }
    if(existe == 0 && flag == 4) // Caso nenhum REGISTRO tenha sido encontrado (func. 4)
        print_reg_inexistente();
}
// Realiza a rotina da função chamadora através de uma busca sequencial no arquivo de dados
void rotina_sequencial(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, CABECALHO *cab, PAR *vetor_pares, int num_pares, PAR *vetor_edicoes, int num_edicoes, FILE *arq_bin, char *tipo_dado, char *campo, int flag)
{
    REGISTRO reg, reg_edit;
    long int BOs_atual = sizeof(char) + 2 * sizeof(int) + sizeof(long int);
    int existe = 0; // Flag utilizada para apontar de algum REGISTRO atende aos critérios de busca (func. 4)

    // Percorrendo o arquivo de dados
    for(int i = 0; i < cab->NumRegArq; i++)
    {
        // Lendo o REGISTRO
        memset(&reg_edit, 0, sizeof(REGISTRO));
        bin_ler_registro(&reg, arq_bin);

        // Caso o REGISTRO atenda às condições de busca
        if(checar_busca(reg, vetor_pares, num_pares))
        {
            switch(flag) // Verificando qual subrotina deve ser realizada
            {
                case 4: // Imprimindo o REGISTRO
                    imprime_registro(reg);
                    existe = 1; // Algum REGISTRO atendeu as condições da busca
                    break;

                case 5: // Removendo o REGISTRO e atribuíndo ao seu INDEX uma chave nula
                    remover_registro(arq_bin, cab, &reg, BOs_atual);
                    remove_index(*vec_index_int, *vec_index_str, cab_ind->NumRegs, BOs_atual, tipo_dado);

                    // Voltando o ponteiro para a posião correta no arquivo de dados
                    int prox = tam_reg(&reg) - 1;   // Hard code: "-1" utilizado para ignorar o char que representa o status de removido do REGISTRO,
                    fseek(arq_bin, prox, SEEK_CUR); // uma vez que o mesmo já foi escrito na função "remover_registro"
                    break;

                case 7: // Editando o REGISTRO
                    edita_registro(vec_index_int, vec_index_str, cab_ind, &reg_edit, reg, cab, vetor_edicoes, num_edicoes, arq_bin, BOs_atual, campo);
                    break;
                
                default:
                    break;
            }
        }
        // Incrementando o ByteOffset da atual iteração
        BOs_atual += tam_reg(&reg);
    }
    if(existe == 0 && flag == 4) // Caso nenhum REGISTRO tenha sido encontrado (func. 4)
        print_reg_inexistente();
} 

// Funcionalidade 4
void crimes_buscar(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_buscas)
{
    FILE *arq_bin = NULL, *arq_index = NULL;
    CABECALHO cab;
    CAB_INDEX cab_ind;
    INT_INDEX *vec_index_int = NULL;
    STR_INDEX *vec_index_str = NULL;

    // Abrindo (se possivel) o aquivo de dados
    arq_bin = fopen(nome_arq_bin, "rb");
    checar_null(arq_bin);

    // Lendo o CABECALHO do arquivo binario
    tentar_ler_cabecalho(&cab, &arq_bin);

    // Abrindo (se possível) o arquivo de INDEX
    arq_index = fopen(nome_arq_ind, "rb");
    checar_null(arq_index);
    
    // Lendo o CABECALHO do arquivo de index
    tenta_ler_cab_index(&cab_ind, &arq_index);

    // Escrevendo o CABECALHO no arquivo de dados
    cab.status = INCONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    // Construindo o vetor de índice
    ler_vec_index(&vec_index_int, &vec_index_str, &cab_ind, tipo_dado, arq_index);
    fclose(arq_index); // Fechando o arquivo de indice

    for(int n = 0; n < num_buscas; n++)
    {
        // Lendo as condições de busca (PARES)
        int num_pares;
        scanf(" %d", &num_pares);
        PAR vetor_pares[num_pares];
        ler_pares(vetor_pares, num_pares, campo);

        printf("Resposta para a busca %d\n", n + 1);

        // Caso um campo indexado seja buscado
        if(strcmp(vetor_pares[0].campo, campo) == 0)
        {
            if(strcmp(tipo_dado, "inteiro") == 0) // Busca indexada do tipo inteiro
            {
                // Buscando e imprimindo os REGISTROS buscados através da busca indexada
                rotina_indexada_int(vec_index_int, &cab_ind, &cab, vetor_pares, num_pares, NULL, 0, arq_bin, campo, 4);
            }
            else // Busca indexada do tipo string
            {
                // Buscando e imprimindo os REGISTROS buscados através da busca indexada
                rotina_indexada_str(vec_index_str, &cab_ind, &cab, vetor_pares, num_pares, NULL, 0, arq_bin, campo, 4);
            }
        }
        else // Busca sequencial no arquivo de dados
        {
            // Buscando e imprimindo os REGISTROS buscados através da busca sequencial
            rotina_sequencial(&vec_index_int, &vec_index_str, &cab_ind, &cab, vetor_pares, num_pares, NULL, 0, arq_bin, tipo_dado, campo, 4);
        }

        // Voltando para o inicio dos dados (após o CABECALHO)
        fseek(arq_bin, sizeof(char) + 2 * sizeof(int) + sizeof(long int), SEEK_SET);
    }
    // Escrevendo o CABECALHO no arquivo de dados
    cab.status = CONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    // Fechando o arquivo
    fclose(arq_bin);

    // Liberando a memória alocada
    liberar_vetor_index(&vec_index_int, &vec_index_str);
}

// Funcionalidade 5
void crimes_remover(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_remocoes)
{
    FILE *arq_bin = NULL, *arq_index = NULL;
    CABECALHO cab;
    CAB_INDEX cab_ind;
    INT_INDEX *vec_index_int = NULL;
    STR_INDEX *vec_index_str = NULL;

    // Abrindo (se possivel) o aquivo binario
    arq_bin = fopen(nome_arq_bin, "rb+");
    checar_null(arq_bin);

    // Lendo o CABECALHO e veriricando o status do arquivo
    tentar_ler_cabecalho(&cab, &arq_bin);

    // Abrindo (se possivel) o arquivo de INDEX
    arq_index = fopen(nome_arq_ind, "rb");
    checar_null(arq_index);

    // Lendo o cabecalho de INDEX
    tenta_ler_cab_index(&cab_ind, &arq_index);

    // Escrevendo o CABECALHO no arquivo de dados
    cab.status = INCONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    // Construindo o vetor de índice
    ler_vec_index(&vec_index_int, &vec_index_str, &cab_ind, tipo_dado, arq_index);
    fclose(arq_index);

    for(int n = 0; n < num_remocoes; n++)
    {    
        // Lendo os critérios de busca (PAR)
        int num_pares;
        scanf(" %d", &num_pares);

        PAR vetor_pares[num_pares];
        ler_pares(vetor_pares, num_pares, campo);

        // Caso um campo indexado seja buscado
        if(strcmp(vetor_pares[0].campo, campo) == 0) 
        {
            if(strcmp(tipo_dado, "inteiro") == 0) // Busca indexada do tipo inteiro
            {
                // Buscando e removendo os REGISTROS buscados atrvés da busca indexada
                rotina_indexada_int(vec_index_int, &cab_ind, &cab, vetor_pares, num_pares, NULL, 0, arq_bin, campo, 5);
            }
            else // Busca indexada do tipo string
            {
                // Buscando e removendo os REGISTROS buscados atrvés da busca indexada
                rotina_indexada_str(vec_index_str, &cab_ind, &cab, vetor_pares, num_pares, NULL, 0, arq_bin, campo, 5);
            }
        }
        else // Busca sequencial no arquivo de dados
        {
            // Buscando e removendo os REGISTROS buscados através da busca sequencial
            rotina_sequencial(&vec_index_int, &vec_index_str, &cab_ind, &cab, vetor_pares, num_pares, NULL, 0, arq_bin, tipo_dado, campo, 5);
        }
        // Voltando para o inicio dos dados (após o CABECALHO)
        fseek(arq_bin, sizeof(char) + sizeof(long int) + 2 * sizeof(int), SEEK_SET);
     
        // Reordenando o veto de INDEX
        // Obs.: A ordenação deve ser feita a cada busca, uma vez que remover um campo
        // e não reordenar o vetor culminará em problemas para uma hipotérica busca binária posterior
        ordenar_vec_index(vec_index_int, vec_index_str, cab_ind, tipo_dado);
    }
    // Reescrevendo o CABECALHO
    cab.status = CONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);
    fclose(arq_bin);

    // Re-indexando os dados a partir do vetor de INDEX já criado
    re_indexar(nome_arq_ind, vec_index_int, vec_index_str, &cab_ind, cab_ind.NumRegs, tipo_dado);

    // Liberando a memória alocada
    liberar_vetor_index(&vec_index_int, &vec_index_str);
}

// Funcionalidade 6
void crimes_inserir(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_insercoes)
{
    FILE *arq_bin = NULL, *arq_index = NULL;
    CABECALHO cab;
    CAB_INDEX cab_ind;
    REGISTRO reg;
    INT_INDEX *vec_index_int = NULL;
    STR_INDEX *vec_index_str = NULL;

    // Abrindo (se possível), o arquivo de dados
    arq_bin = fopen(nome_arq_bin,"rb+");
    checar_null(arq_bin);

    // Tentando ler o CABECALHO
    tentar_ler_cabecalho(&cab, &arq_bin);
    
    // Abrindo (se possível) o arquivo de INDEX
    arq_index = fopen(nome_arq_ind, "rb");
    checar_null(arq_index);

    // Lendo o cabecalho de INDEX
    tenta_ler_cab_index(&cab_ind, &arq_index);

    // Escrevendo o CABECALHO
    cab.status = INCONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    // Construindo o vetor de índice
    ler_vec_index(&vec_index_int, &vec_index_str, &cab_ind, tipo_dado, arq_index);
    fclose(arq_index);

    // Apontando o ponteiro para o fim do arquivo
    fseek(arq_bin, 0, SEEK_END);

    int qtd_antes = cab_ind.NumRegs; // Qunatidade de REGISTROS antes das inserções
    
    // Alocando memória para as inserções no vetor de INDEX
    if(strcmp(tipo_dado, "inteiro") == 0)
        vec_index_int = realloc (vec_index_int, (qtd_antes + num_insercoes) * sizeof(INT_INDEX));
    else
        vec_index_str = realloc (vec_index_str, (qtd_antes + num_insercoes) * sizeof(STR_INDEX));


    for(int i = 0; i < num_insercoes; i++)
    {
        char buffer[MAX_TAM_STRING] = {'\0'};
        
        // Lendo o REGISTRO solicitado pelo usuário
        memset(&reg, 0, sizeof(REGISTRO));
        ler_reg_stdin(&reg);

        // Adicionando o REGISTRO lido ao vetor de INDEX
        adiciona_index(vec_index_int, vec_index_str, reg, cab, campo, qtd_antes + i);
        
        // Escrevendo o REGISTRO no fim do arquivo
        escreve_registro(&reg, arq_bin);

        // Atualizando o CABECALHO
        atualizar_cabecalho(&cab, reg.local, reg.descricao);
    }
    
    // Ordenando o vetor de INDEX
    ordenar_vec_index(vec_index_int, vec_index_str, cab_ind, tipo_dado);

    // Atualizando o CABECALHO
    fseek(arq_bin, 0, SEEK_SET);
    cab.status = CONSISTENTE;
    escreve_cabecalho(&cab, arq_bin);
    fclose(arq_bin);

    // Re-indexando os dados com base no vetor de INDEX já criado
    re_indexar(nome_arq_ind, vec_index_int, vec_index_str, &cab_ind, (qtd_antes + num_insercoes), tipo_dado);

    // Liberando a memória alocada
    liberar_vetor_index(&vec_index_int, &vec_index_str);
}

// Monta o REGISTRO editado e, se necessário, modifica o vetor de INDEX
void montar_edicao(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, REGISTRO *reg_edit, REGISTRO reg, PAR *vetor_edicoes, int num_edicoes, int flag_cabe, long int ByteOffset, char *campo)
{
    // Percorrendo as edições à serem feitas
    for(int j = 0; j < num_edicoes; j++)
    {
        if(strcmp(vetor_edicoes[j].campo, "idCrime") == 0) // Caso o ID deva ser editado
        {
            // Copiando o dado para o REGISTRO editado (ID nunca é nulo)
            reg_edit->id = vetor_edicoes[j].chave_int;
        }
        else if(strcmp(vetor_edicoes[j].campo, "dataCrime") == 0) // Caso a DATA deva ser editada
        {
            // Caso a DATA seja o campo indexado, e o REGISTRO apresente DATA nula
            if(strcmp(campo, vetor_edicoes[j].campo) == 0 && reg_edit->data[0] == LIXO)
            {
                // Adicionando mais um espaço de memória no vetor de INDEX
                cab_ind->NumRegs++;
                (*vec_index_str) = realloc ((*vec_index_str), cab_ind->NumRegs * sizeof(STR_INDEX));
                
                // Atribuíndo ao vetor de INDEX o ByteOffset do REGISTRO editado para sua posterior inserção no vetor
                (*vec_index_str)[cab_ind->NumRegs - 1].ByteOffset = ByteOffset;
            }
            // Copiando o dado para o REGISTRO editado
            strcpy(reg_edit->data, vetor_edicoes[j].chave_str);
        }
        else if(strcmp(vetor_edicoes[j].campo, "numeroArtigo") == 0) // Caso o ARTIGO deva ser editado
        {
            // Caso o ARTIGO seja o campo indexado, e o REGISTRO apresente ARTIGO nulo
            if(strcmp(campo, vetor_edicoes[j].campo) == 0 && reg_edit->artigo == -1)
            {
                // Adicionando mais um espaço de memória no vetor de INDEX
                cab_ind->NumRegs++;
                (*vec_index_int) = realloc ((*vec_index_int), cab_ind->NumRegs * sizeof(INT_INDEX));
                
                // Atribuíndo ao vetor de INDEX o ByteOffset do REGISTRO editado para sua posterior inserção no vetor
                (*vec_index_int)[cab_ind->NumRegs - 1].ByteOffset = ByteOffset;
            }
            // Copiando o dado para o REGISTRO editado
            reg_edit->artigo = vetor_edicoes[j].chave_int;
        }
        else if(strcmp(vetor_edicoes[j].campo, "lugarCrime") == 0) // Caso o LOCAL deva ser editado
        {
            // Caso o LOCAL seja o campo indexado, e o REGISTRO apresente LOCAL nulo
            if(strcmp(campo, vetor_edicoes[j].campo) == 0 && (reg_edit->local[0] == LIXO || reg_edit->local[0] == DELIM_REG))
            {
                // Adicionando mais um espaço de memória no vetor de INDEX
                cab_ind->NumRegs++;
                (*vec_index_str) = realloc ((*vec_index_str), cab_ind->NumRegs * sizeof(STR_INDEX));
                
                // Atribuíndo ao vetor de INDEX o ByteOffset do REGISTRO editado para sua posterior inserção no vetor
                (*vec_index_str)[cab_ind->NumRegs - 1].ByteOffset = ByteOffset;
            }
            // Copiando o dado para o REGISTRO editado
            if(flag_cabe == 0) // Caso a edição não caiba no REGISTRO
                ler_local(reg_edit, vetor_edicoes[j].chave_str);
            else               // Caso caiba
            {
                // Obs.: Hard code "-1" utilizado para que o terminador de string seja sobrescrito
                int len = len_campo(reg.local) - 1, len_depois = len_campo(vetor_edicoes[j].chave_str) - 1;

                for(int i = 0; i < len; i++)
                {
                    if(i < len_depois)
                        reg_edit->local[i] = vetor_edicoes[j].chave_str[i];
                    else 
                        reg_edit->local[i] = LIXO;
                }
                reg_edit->local[len] = DELIM_STRING;
            }
        }
        else if(strcmp(vetor_edicoes[j].campo, "descricaoCrime") == 0) // Caso a DESCRICAO deva ser editada
        {
            // Caso a DESCRICAO seja o campo indexado, e o REGISTRO apresente DESCRICAO nula
            if(strcmp(campo, vetor_edicoes[j].campo) == 0 && reg_edit->descricao[0] == LIXO)
            {
                // Adicionando mais um espaço de memória no vetor de INDEX
                cab_ind->NumRegs++;
                (*vec_index_str) = realloc ((*vec_index_str), cab_ind->NumRegs * sizeof(STR_INDEX));
                
                // Atribuíndo ao vetor de INDEX o ByteOffset do REGISTRO editado para sua posterior inserção no vetor
                (*vec_index_str)[cab_ind->NumRegs - 1].ByteOffset = ByteOffset;
            }
            // Copiando o dado para o REGISTRO editado
            if(flag_cabe == 0) // Caso a edição não caiba no REGISTRO
                ler_descricao(reg_edit, vetor_edicoes[j].chave_str);
            else               // Caso caiba
            {
                // Obs.: Hard code "-1" utilizado para que o terminador de string seja sobrescrito
                int len_antes = len_campo(reg.descricao) - 1, len_depois = len_campo(vetor_edicoes[j].chave_str) - 1;

                for(int i = 0; i < len_antes; i++)
                {
                    if(i < len_depois)
                        reg_edit->descricao[i] = vetor_edicoes[j].chave_str[i];
                    else 
                        reg_edit->descricao[i] = LIXO;
                }
                reg_edit->descricao[len_antes] = DELIM_STRING;
            }
        }
        else // Caso a MARCA deva ser EDITADA
        {
            // Caso a MARCA seja o campo indexado, e o REGISTRO apresente MARCA nula
            if(strcmp(campo, vetor_edicoes[j].campo) == 0 && reg_edit->marca[0] == LIXO)
            {
                // Adicionando mais um espaço de memória no vetor de INDEX
                cab_ind->NumRegs++;
                (*vec_index_str) = realloc ((*vec_index_str), cab_ind->NumRegs * sizeof(STR_INDEX));
                
                // Atribuíndo ao vetor de INDEX o ByteOffset do REGISTRO editado para sua posterior inserção no vetor
                (*vec_index_str)[cab_ind->NumRegs - 1].ByteOffset = ByteOffset;
            }
            // Copiando o dado para o REGISTRO editado
            // Obs.: Hard code "-1" utilizado para que o terminador de string seja sobrescrito
            int len_edicao = len_campo(vetor_edicoes[j].chave_str) - 1;

            for(int i = 0; i < len_edicao; i++)
            {
                if(i < len_edicao)
                    reg_edit->marca[i] = vetor_edicoes[j].chave_str[i];
                else
                    reg_edit->marca[i] = LIXO;
            }
        }
    }
}

// Edita o REGISTRO passado como parâmetro
void edita_registro(INT_INDEX **vec_index_int, STR_INDEX **vec_index_str, CAB_INDEX *cab_ind, REGISTRO *reg_edit, REGISTRO reg, CABECALHO *cab, PAR *vetor_edicoes, int num_edicoes, FILE *arq_bin, long int BOs_atual, char *campo)
{
    // Verificando se a edição cabe no REGISTRO
    int flag_cabe = registro_cabe(reg, vetor_edicoes, num_edicoes);

    // Formatando os dados para a edição
    formata_reg_edit(reg_edit, reg);

    long int ultimo_BOs = cab->proxByteOffset; // ByteOffset do último REGISTRO escrito no arquivo de dados

    if(flag_cabe == 0) // Caso a edição não caiba no REGISTRO
    {
        // Removendo o REGISTRO antigo
        remover_registro(arq_bin, cab, &reg, BOs_atual);
        
        // Armazenando o Byte Offset atual
        int prox = tam_reg(&reg) + BOs_atual;

        // Montando o registro editado
        montar_edicao(vec_index_int, vec_index_str, cab_ind, reg_edit, reg, vetor_edicoes, num_edicoes, flag_cabe, BOs_atual, campo);
        
        // Escrevendo-o no fim do arquivo de dados e atualizando o CABECALHO
        fseek(arq_bin, 0, SEEK_END);
        escreve_registro(reg_edit, arq_bin);
        atualizar_cabecalho(cab, reg_edit->local, reg_edit->descricao);

        // Voltando o ponteiro para o próximo REGISTRO a ser lido
        fseek(arq_bin, prox, SEEK_SET);
    }
    else // Caso a edição caiba no REGISTRO
    {
        // Procurando o REGISTRO a ser editado no arquivo de dados
        fseek(arq_bin, BOs_atual, SEEK_SET);

        // Montando o REGISTRO editado e sobrescrevendo o antigo
        montar_edicao(vec_index_int, vec_index_str, cab_ind, reg_edit, reg, vetor_edicoes, num_edicoes, flag_cabe, BOs_atual, campo);
        escreve_registro(reg_edit, arq_bin);
    }
    // Fazendo as alterações necessárias no vetor de INDEX
    edita_vec_index(vec_index_int, vec_index_str, cab_ind, reg_edit, BOs_atual, ultimo_BOs, campo, flag_cabe);
}

// Funcionalidade 7
void crimes_editar(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_edicoes)
{
    FILE *arq_bin = NULL, *arq_index = NULL;
    CABECALHO cab;
    CAB_INDEX cab_ind;
    REGISTRO reg_edit;
    INT_INDEX *vec_index_int = NULL;
    STR_INDEX *vec_index_str = NULL;

    // Abrindo (se possivel) o aquivo binario
    arq_bin = fopen(nome_arq_bin, "rb+");
    checar_null(arq_bin);

    // Lendo o CABECALHO e veriricando o status do arquivo
    tentar_ler_cabecalho(&cab, &arq_bin);

    // Abrindo (se possível) o arquivo de INDEX
    arq_index = fopen(nome_arq_ind, "rb");
    checar_null(arq_index);

    // Lendo o cabecalho de INDEX
    tenta_ler_cab_index(&cab_ind, &arq_index);

    // Escrevendo o CABECALHO no arquivo de dados
    cab.status = INCONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    // Construindo o vetor de índice
    ler_vec_index(&vec_index_int, &vec_index_str, &cab_ind, tipo_dado, arq_index);
    fclose(arq_index);

    for(int n = 0; n < num_edicoes; n++)
    {    
        // Lendo os critérios de busca (PAR)
        int num_pares, num_edicoes;
        scanf(" %d", &num_pares);

        PAR vetor_pares[num_pares];
        ler_pares(vetor_pares, num_pares, campo);

        // Lendo as edições à serem feitas (PAR)
        scanf(" %d", &num_edicoes);
        PAR vetor_edicoes[num_edicoes];
        ler_pares(vetor_edicoes, num_edicoes, campo);

        // Caso o campo buscado seja o campo indexado
        if(strcmp(campo, vetor_pares[0].campo) == 0)
        {
            memset(&reg_edit, 0, sizeof(REGISTRO));

            if(strcmp(tipo_dado, "inteiro") == 0) // Busca indexada do tipo inteiro
            {
                // Buscando e editando os REGISTROS buscados através da busca indexada
                rotina_indexada_int(vec_index_int, &cab_ind, &cab, vetor_pares, num_pares, vetor_edicoes, num_edicoes, arq_bin, campo, 7);
            }
            else // Busca indexada do tipo string
            {
                // Buscando e editando os REGISTROS buscados através da busca indexada
                rotina_indexada_str(vec_index_str, &cab_ind, &cab, vetor_pares, num_pares, vetor_edicoes, num_edicoes, arq_bin, campo, 7);
            }
        }
        else // Busca sequencial no arquivo de dados
        {
            // Buscando e editando os REGISTROS buscados através da busca sequencial
            rotina_sequencial(&vec_index_int, &vec_index_str, &cab_ind, &cab, vetor_pares, num_pares, vetor_edicoes, num_edicoes, arq_bin, tipo_dado, campo, 7);
        }

        // Reordenando o vetor
        // Obs.: A ordenação deve ser feita a cada busca, uma vez que alterar um campo indexado
        // e não reordenar o vetor culminará em problemas para uma hipotérica busca binária posterior
        ordenar_vec_index(vec_index_int, vec_index_str, cab_ind, tipo_dado);

        // Voltando o ponteiro para o início dos dados (após o CABECALHO)
        fseek(arq_bin, sizeof(char) + sizeof(long int) + 2 * sizeof(int), SEEK_SET);
    }

    // Reescrevendo o CABECALHO
    cab.status = CONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);
    fclose(arq_bin);

    // Re-indexando os dados a partir do vetor de INDEX já criado
    re_indexar(nome_arq_ind, vec_index_int, vec_index_str, &cab_ind, cab_ind.NumRegs, tipo_dado);

    // Liberando a memória alocada
    liberar_vetor_index(&vec_index_int, &vec_index_str);
}

// Funcionalidade 8
void crimes_criar_ab(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind)
{
    FILE *arq_bin = NULL, *arq_index = NULL;
    CABECALHO cab;
    CAB_AB cab_ab;
    REGISTRO reg;
    PAGINA raiz;
    long int ultimo_BOs = sizeof(char) + 2 * sizeof(int) + sizeof(long int);

    // Abrindo (se possivel) o aquivo binario
    arq_bin = fopen(nome_arq_bin, "rb+");
    checar_null(arq_bin);

    // Lendo o CABECALHO e veriricando o status do arquivo
    bin_ler_cabecalho(&cab, arq_bin);
    if(cab.status == INCONSISTENTE)
    {
        fclose(arq_bin);
        erro_arquivo();
    }          

    // Abrindo (se possível) o arquivo de index (ARVORE B)
    arq_index = fopen(nome_arq_ind, "wb+");
    checar_null(arq_index);

    // Inicializando e escrevendo o CABECALHO no arquivo de index
    inicializa_cab_ab(&cab_ab);
    fseek(arq_index, 0, SEEK_SET);
    escreve_cab_ab(&cab_ab, arq_index);

    // Escrevendo o CABECALHO no arquivo de dados
    cab.status = INCONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    // Percorrendo o arquivo de dados
    for(int i = 0; i < cab.NumRegArq; i++)
    {
        char buffer[MAX_TAM_STRING] = {'\0'};

        // Lendo o REGISTRO do arquivo de dados
        memset(&reg, 0, sizeof(REGISTRO));
        ler_reg_stdin(&reg);
        bin_ler_registro(&reg, arq_bin);

        cab_ab.num_chaves++; // Incrementando o numero de chaves na ARVORE B

        // Criando a CHAVE a ser inserida na ARVORE B
        CHAVE_AB inserida;
        inserida.valor = reg.id;
        inserida.ByteOffset = ultimo_BOs;

        // Inserindo a CHAVE na ARVORE B
        insere(&cab, &cab_ab, &raiz, inserida, arq_index);
        
        ultimo_BOs += tam_reg(&reg); // Incrementando o Byteoffset da atual iteração
    }

    // Atualizando e reescrevendo o CABECALHO no arquivo de dados
    fseek(arq_bin, 0, SEEK_SET);
    cab.status = CONSISTENTE;
    escreve_cabecalho(&cab, arq_bin);
    fclose(arq_bin);

    // Atualizando e reescrevendo o CABECALHO no arquivo de index (ARVORE B)
    fseek(arq_index, 0, SEEK_SET);
    cab_ab.status = CONSISTENTE;
    escreve_cab_ab(&cab_ab, arq_index);
    fclose(arq_index);
}

// Funcionalidade 9
void crimes_buscar_ab(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_buscas)
{    
    FILE *arq_bin = NULL, *arq_index = NULL;
    CABECALHO cab;
    CAB_AB cab_ab;
    REGISTRO reg;
    PAGINA pag, raiz;

    // Abrindo (se possivel) o aquivo binario
    arq_bin = fopen(nome_arq_bin, "rb");
    checar_null(arq_bin);

    // Lendo o CABECALHO e veriricando o status do arquivo
    bin_ler_cabecalho(&cab, arq_bin);
    if(cab.status == INCONSISTENTE)
    {
        fclose(arq_bin);
        erro_arquivo();
    }

    // Abrindo (se possivel) o arquivo de indice
    arq_index = fopen(nome_arq_ind, "rb");
    checar_null(arq_index);

    // Lendo o CABECALHO e verificando o status do arquivo
    ler_cab_ab(&cab_ab, arq_index);
    if(cab_ab.status == INCONSISTENTE)
        erro_arquivo();

    // Caso existam chaves no arquivo de index, lemos a raiz da arvore
    if(cab_ab.num_chaves > 0)
    {
        fseek(arq_index, ((cab_ab.raiz + 1) * TAM_PAG), SEEK_SET);
        ler_pagina(&raiz, arq_index);
    }

    // Escrevendo o CABECALHO no arquivo de dados
    cab.status = INCONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    for(int n = 0; n < num_buscas; n++)
    {    
        printf("Resposta para a busca %d\n", n + 1);

        int num_pares;
        scanf(" %d", &num_pares);

        // Lendo os critérios de busca (PAR)
        PAR vetor_pares[num_pares];
        ler_pares(vetor_pares, num_pares, campo);

        // Caso não hajam REGISTROS
        if(cab.NumRegArq == 0 || cab_ab.num_chaves == 0)
        {
            // A busca não apresenta respostas
            print_reg_inexistente();
            continue;
        }

        // Caso o campo buscado seja o campo indexado, realizamos busca na ARVORE B
        if(strcmp(campo, vetor_pares[0].campo) == 0)
        {
            // Buscando o Byteoffset do REGISTRO na ARVORE B
            pag = raiz;
            long int BOs_busca = ab_buscar(&pag, vetor_pares[0].chave_int, arq_index);
            
            // Caso o REGISTRO exista
            if(BOs_busca != -1)
            {
                // Lendo, no arquivo de dados, o REGISTRO encontrado 
                fseek(arq_bin, BOs_busca, SEEK_SET);
                bin_ler_registro(&reg, arq_bin);

                // Caso o REGISTRO corresponda aos demais criterios de busca
                if(checar_busca(reg, vetor_pares, num_pares))
                    imprime_registro(reg); // Imprimindo o REGISTRO
                
                else // Caso contrário, a busca não apresentou resposta
                    print_reg_inexistente();
            }
            else // Caso o REGISTRO não exista, a busca não apresentou resposta
                print_reg_inexistente();
        }
        // Caso contrário, realizamos busca sequencial no arquivo de dados
        else
        {
            long int BOs_atual = sizeof(char) + 2 * sizeof(int) + sizeof(long int);
            int existe = 0; // Flag utilizada para apontar se algum REGISTRO atende aos critérios de busca

            // Percorrendo o arquivo de dados
            for(int i = 0; i < cab.NumRegArq; i++)
            {
                // Lendo o REGISTRO
                bin_ler_registro(&reg, arq_bin);

                // Caso o REGISTRO corresponda aos criterios de busca
                if(checar_busca(reg, vetor_pares, num_pares))
                {
                    // Imprimindo o REGISTRO
                    imprime_registro(reg);
                    existe = 1;
                }
                BOs_atual += tam_reg(&reg); // Atualizando o Byteoffset da atual iteração
            }
            if(existe == 0) // Caso nenhum REGISTRO tenha sido encontrado
                print_reg_inexistente();
        }

        // Voltando o ponteiro para o início dos dados (após o CABECALHO)
        fseek(arq_bin, sizeof(char) + sizeof(long int) + 2 * sizeof(int), SEEK_SET);
    }

    // Reescrevendo o CABECALHO no arquivo de dados
    cab.status = CONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);
    fclose(arq_bin);
}

// Funcionalidade 10
void crimes_inserir_ab(char *nome_arq_bin, char *campo, char *tipo_dado, char *nome_arq_ind, int num_insercoes)
{
    FILE *arq_bin = NULL, *arq_index = NULL;
    CABECALHO cab;
    CAB_AB cab_ab;
    REGISTRO reg;
    PAGINA raiz;
 
    // Abrindo (se possivel) o aquivo binario
    arq_bin = fopen(nome_arq_bin, "rb+");
    checar_null(arq_bin);

    // Lendo o CABECALHO e veriricando o status do arquivo
    bin_ler_cabecalho(&cab, arq_bin);
    if(cab.status == INCONSISTENTE)
    {
        fclose(arq_bin);
        erro_arquivo();
    }          

    // Abrindo (se possivel) o arquivo de index (ARVORE B)
    arq_index = fopen(nome_arq_ind, "rb+");
    checar_null(arq_index);

    // Lendo o CABECALHO do arquivo de INDEX e verificando seu status
    ler_cab_ab(&cab_ab, arq_index);
    if(cab_ab.status == INCONSISTENTE)
        erro_arquivo();

    // Escrevendo o CABECALHO no arquivo de dados
    cab.status = INCONSISTENTE;
    fseek(arq_bin, 0, SEEK_SET);
    escreve_cabecalho(&cab, arq_bin);

    // Escrevendo o CABECALHO no arquivo de index
    cab_ab.status = INCONSISTENTE;
    fseek(arq_index, 0, SEEK_SET);
    escreve_cab_ab(&cab_ab, arq_index);

    // Caso hajam chaves no arquivo de INDEX, lemos a raiz da ARVORE B
    if(cab_ab.num_chaves > 0)
    {
        fseek(arq_index, ((cab_ab.raiz + 1) * TAM_PAG), SEEK_SET);
        ler_pagina(&raiz, arq_index);
    }
    
    fseek(arq_bin, 0, SEEK_END);
    
    for(int i = 0; i < num_insercoes; i++)
    {
        char buffer[MAX_TAM_STRING] = {'\0'};

        // Lendo o REGISTRO do arquivo de dados
        memset(&reg, 0, sizeof(REGISTRO));
        ler_reg_stdin(&reg);

        // Inserindo o REGISTRO no arquivo de dados
        long int ultimo_BOs = cab.proxByteOffset;
        escreve_registro(&reg, arq_bin);
        atualizar_cabecalho(&cab, reg.local, reg.descricao);

        // Criando a CHAVE a ser inserida na ARVORE B
        CHAVE_AB inserida;
        inserida.valor = reg.id;
        inserida.ByteOffset = ultimo_BOs;

        // Inserindo a CHAVE na ARVORE B
        insere(&cab, &cab_ab, &raiz, inserida, arq_index);
        
        cab_ab.num_chaves++; // Atualizando o numero de chaves da ARVORE B
    }

    // Atualizando e reescrevendo o CABECALHO do arquivo de dados
    fseek(arq_bin, 0, SEEK_SET);
    cab.status = CONSISTENTE;
    escreve_cabecalho(&cab, arq_bin);
    fclose(arq_bin);

    // Atualizando e reescrevendo o CABECALHO no arquivo de index (ARVORE B)
    fseek(arq_index, 0, SEEK_SET);
    cab_ab.status = CONSISTENTE;
    escreve_cab_ab(&cab_ab, arq_index);
    fclose(arq_index);
}