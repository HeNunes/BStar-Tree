#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "fornecido.h"
#include "registros.h"

// As próximas funções (ler_id até ler_marca) tratam o dado lido do CSV e armazenam no REGISTRO
void ler_id(REGISTRO *reg, char *buffer)
{
    reg->id = atoi(buffer);
}
void ler_data(REGISTRO *reg, char *buffer)
{
    // Completando a string com lixo e copiando-a para o REGISTRO
    for(int i = strlen(buffer); i < 10; i++)
        buffer[i] = LIXO;
    strcpy(reg->data, buffer);
}
void ler_artigo(REGISTRO *reg, char *buffer)
{
    // Caso o artigo seja NULO, atribuímos "-1" ao campo do REGISTRO
    if(strcmp(buffer, "\0") == 0)
        reg->artigo = -1;
    else
        reg->artigo = atoi(buffer);
}
void ler_local(REGISTRO *reg, char *buffer)
{
    // Concatenando o delimitador na string e copiando-a para o REGISTRO
    strcat(buffer, "|");
    strcpy(reg->local, buffer);
}
void ler_descricao(REGISTRO *reg, char *buffer)
{
    // Concatenando o delimitador na string e copiando-a para o REGISTRO
    strcat(buffer, "|");
    strcpy(reg->descricao, buffer);
}
void ler_marca(REGISTRO *reg, char *buffer)
{
    // Completando a string com lixo e copiando-a para o REGISTRO
    for(int i = len_campo(buffer) - 1; i < 12; i++)
        buffer[i] = LIXO;
    strcpy(reg->marca, buffer);
}

void inicializa_cabecalho(CABECALHO *cab)
{
    // Inicializando o CABECALHO conforme especificado
    cab->status = CONSISTENTE;
    cab->proxByteOffset = sizeof(char) + 2 * sizeof(int) + sizeof(long int);
    cab->NumRegArq = 0;
    cab->NumRegRem = 0;
}
// Função utilizada após um REGISTRO ser lido
void atualizar_cabecalho(CABECALHO *cab, char *local, char *descricao)
{
    // Incrementando o número de REGISTROS e adicionando seu tamanho no próximo byte offset
    cab->NumRegArq++;
    cab->proxByteOffset += (2 * sizeof(int)) + ((24 + len_campo(local) + len_campo(descricao)) * sizeof(char));
}

// Escrevendo o CABECALHO no arquivo binário
void escreve_cabecalho(CABECALHO *cab, FILE *arq_bin)
{
    fwrite(&(cab->status), sizeof(char), 1, arq_bin);
    fwrite(&(cab->proxByteOffset), sizeof(long int), 1, arq_bin);
    fwrite(&(cab->NumRegArq), sizeof(int), 1, arq_bin);
    fwrite(&(cab->NumRegRem), sizeof(int), 1, arq_bin);
}
// Escrevendo o REGISTRO no arquivo binário
void escreve_registro(REGISTRO *reg, FILE *arq_bin)
{
    // Escrevendo os dados do REGISTRO
    fwrite(&(reg->removido), sizeof(char), 1, arq_bin);
    fwrite(&(reg->id), sizeof(int), 1, arq_bin);
    fwrite(&(reg->data), sizeof(char), 10, arq_bin);
    fwrite(&(reg->artigo), sizeof(int), 1, arq_bin);
    fwrite(&(reg->marca), sizeof(char), 12, arq_bin);
    fwrite(&(reg->local), sizeof(char), len_campo(reg->local), arq_bin);
    fwrite(&(reg->descricao), sizeof(char), len_campo(reg->descricao), arq_bin);
        
    // Escrevendo o delimitador de registros
    char c = DELIM_REG;
    fwrite(&c, sizeof(char), 1, arq_bin);
}

// Imprime um REGISTRO
void imprime_registro(REGISTRO reg)
{
    // Caso o registro seja logicamente removido, não o imprimimos
    if(reg.removido == '1')
        return;
    
    // Imprimindo os dados, caso sejam nulos, imprimimos "NULO"
    printf("%d, ", reg.id);

    if(strcmp(reg.data, "\0") != 0)
        printf("%s, ", reg.data);
    else printf("NULO, ");
    
    if(reg.artigo != -1)
        printf("%d, ", reg.artigo);
    else printf("NULO, ");

    if(reg.local[0] != '\0')
        printf("%s, ", reg.local);
    else printf("NULO, ");

    if(reg.descricao[0] != '\0')
        printf("%s, ", reg.descricao);
    else printf("NULO, ");

    if(reg.marca[0] != '\0')
        printf("%s\n", reg.marca);
    else printf("NULO\n");
}

// Lendo o CABECALHO do arquivo binário
void bin_ler_cabecalho(CABECALHO *cab, FILE *arq_bin)
{
    fread(&(cab->status), sizeof(char), 1, arq_bin);
    fread(&(cab->proxByteOffset), sizeof(long int), 1, arq_bin);
    fread(&(cab->NumRegArq), sizeof(int), 1, arq_bin);
    fread(&(cab->NumRegRem), sizeof(int), 1, arq_bin);
}

// Lendo o CABECALHO do arquivo binário e tratando as exceções
void tentar_ler_cabecalho(CABECALHO *cab, FILE **arq_bin)
{
    // Lendo o cabecalho do binario
    bin_ler_cabecalho(cab, *arq_bin);

    // Verificando o status do CABECALHO
    if(cab->status == INCONSISTENTE)
    {
        fclose(*arq_bin);
        erro_arquivo();
    }
    // Verificando se há REGISTROS no arquivo
    if(cab->NumRegArq == 0)
    {
        fclose(*arq_bin);
        erro_registros();
        exit(0);
    }
}

// Lendo um REGISTRO do arquivo binário
void bin_ler_registro(REGISTRO *reg, FILE *arq_bin)
{
    char buffer[MAX_TAM_STRING] = {'\0'};

    fread(&(reg->removido), sizeof(char), 1, arq_bin); // Lendo o status
    
    fread(&(reg->id), sizeof(int), 1, arq_bin); // Lendo o id

    fread(&(buffer), sizeof(char), 10, arq_bin); // Lendo (e tratando) a data
    trata_string(buffer, 10);
    strcpy(reg->data, buffer);

    fread(&(reg->artigo), sizeof(int), 1, arq_bin); // Lendo o artigo

    fread(&(buffer), sizeof(char), 12, arq_bin); // Lendo (e tratando) a marca
    trata_string(buffer, 12);
    strcpy(reg->marca, buffer);

    bin_ler_string_variavel(arq_bin, buffer); // Lendo o local
    strcpy(reg->local, buffer);
    
    bin_ler_string_variavel(arq_bin, buffer); // Lendo a descrição
    strcpy(reg->descricao, buffer);

    fread(&buffer, sizeof(char), 1, arq_bin); // Lendo o delimitador de REGISTROS
}

// Retorna o tamanho do REGISTRO
int tam_reg(REGISTRO *reg)
{
    int tam = 2 * sizeof(int) + (24 + len_campo(reg->local) + len_campo(reg->descricao)) * sizeof(char);
    return tam;
}

// Verifica se um REGISTRO atende determinada condição de busca
bool comp_busca(REGISTRO reg, char *campo_buscado, int int_buscado, char *str_buscado)
{
    // Caso o REGISTRO apresente a chave buscada, retorna TRUE
    if((strcmp(campo_buscado, "idCrime") == 0 && int_buscado == reg.id) || (strcmp(campo_buscado, "dataCrime") == 0 && strcmp(str_buscado, reg.data) == 0)
        || (strcmp(campo_buscado, "numeroArtigo") == 0 && int_buscado == reg.artigo) || (strcmp(campo_buscado, "lugarCrime") == 0 && strcmp(str_buscado, reg.local) == 0)
        || (strcmp(campo_buscado, "descricaoCrime") == 0 && strcmp(str_buscado, reg.descricao) == 0) || (strcmp(campo_buscado, "marcaCelular") == 0 && strcmp(str_buscado, reg.marca) == 0))
        return true;
    // Se não, retorna false
    else
        return false; 
}

// Checa os critérios de busca para um REGISTRO
bool checar_busca(REGISTRO reg, PAR *vetor_pares, int num_pares)
{
    int aux = 0; // Conta o número de condições satisfeitas

    // Iterando nas condições de busca
    for(int j = 0; j < num_pares; j++)
    {
        // Caso o REGISTRO atenda as condições, incrementamos "aux"
        if(comp_busca(reg, vetor_pares[j].campo, vetor_pares[j].chave_int, vetor_pares[j].chave_str) == true 
            && reg.removido == '0')
                aux++;
            
        // Caso contrário, encerramos o laço
        else 
            break;
    }
    // Caso todas as condiçoes sejam satisfeitas, retornamos TRUE
    if(aux == num_pares)
        return true;
    // Se não, retornamos FALSE
    else
        return false;
}

// Remove logicamente um REGISTRO
void remover_registro(FILE *arq_bin, CABECALHO *cab, REGISTRO *reg, long int byte_offset)
{
    // Incrementando o número de registros removidos
    cab->NumRegRem++;

    // Marcando o REGISTRO como lógicamente removido
    fseek(arq_bin, byte_offset, SEEK_SET);
    reg->removido = '1';
    fwrite(&(reg->removido), sizeof(char), 1, arq_bin);
}

// Le um REGISTRO da entrada padrão
void ler_reg_stdin(REGISTRO *reg)
{
    char buffer[MAX_TAM_STRING] = {'\0'};

    reg->removido = '0'; 

    scan_quote_string(buffer); // Lendo e tratando o ID
    ler_id(reg, buffer);

    scan_quote_string(buffer); // Lendo e tratando a DATA
    ler_data(reg, buffer);

    scan_quote_string(buffer); // Lendo e tratando o ARTIGO
    ler_artigo(reg, buffer);

    scan_quote_string(buffer); // Lendo e tratando o LOCAL
    ler_local(reg, buffer);

    scan_quote_string(buffer); // Lendo e tratando a DESCRICAO
    ler_descricao(reg, buffer);

    scan_quote_string(buffer); // Lendo e tratando a MARCA
    ler_marca(reg, buffer); 
}

// Checa se a edição à ser realizada cabe no REGISTRO de dados
int registro_cabe(REGISTRO reg, PAR *vetor_edicoes, int num_edicoes)
{
    int flag_cabe = 1;

    // Percorrendo as edições à serem feitas
    for(int k = 0; k < num_edicoes; k++)
    {
        // Caso um campo de tamanho variável seja editado, e o tamanho da edição seja maior que o atual
        if((strcmp(vetor_edicoes[k].campo, "lugarCrime") == 0 && len_campo(reg.local) < len_campo(vetor_edicoes[k].chave_str)) 
            || (strcmp(vetor_edicoes[k].campo, "descricaoCrime") == 0 && len_campo(reg.descricao) < len_campo(vetor_edicoes[k].chave_str)))
            {
                // A edição não cabe no REGISTRO
                flag_cabe = 0;
                break;
            }
    }
    return flag_cabe;
}
// Formata os campos do tipo string de um REGISTRO para realizar a edição
void formata_reg_edit(REGISTRO *reg_edit, REGISTRO reg)
{
    // Preenchendo os dados de tamanho fixo com LIXO ($)
    for(int i = len_campo(reg.data) - 1; i < 10; i++)
        reg.data[i] = LIXO;
    for(int i = len_campo(reg.marca) - 1; i < 12; i++)
        reg.marca[i] = LIXO;
    // Adicionando o delimitador aos campos de tamanho variável
    strcat(reg.local, "|");
    strcat(reg.descricao, "|");
    *reg_edit = reg;
}