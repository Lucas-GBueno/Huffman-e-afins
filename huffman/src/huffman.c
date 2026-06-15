#include "huffman.h"
#include "algoritimo_compressao.h"
#include "algoritimo_descompressao.h"
#include <string.h>

void comprimir(char *caminho_arquivo) {
    FILE *arquivo_entrada = fopen(caminho_arquivo, "rb");
    if (!arquivo_entrada) {
        printf("Erro ao abrir o arquivo original. Verifique o caminho.\n");
        return;
    }

    char caminho_saida[512];
    strncpy(caminho_saida, caminho_arquivo, sizeof(caminho_saida) - 6);
    strcat(caminho_saida, ".huff");

    FILE *arquivo_saida = fopen(caminho_saida, "wb");
    if (!arquivo_saida) {
        printf("Erro ao criar o arquivo comprimido.\n");
        fclose(arquivo_entrada);
        return;
    }

    // ==========================================
    // ETAPAS GLOBAIS DE COMPRESSÃO
    // ==========================================
    int frequencias[TAMANHO_ASCII] = {0};
    FilaPrioridade *fila = obter_frequencia_caracter(arquivo_entrada, frequencias);
    NoArvore *raiz = construir_arvore_huffman(fila);

    dado_objeto tabela[TAMANHO_ASCII] = {0};
    gerar_tabela_huffman(raiz, tabela, 0, 0);

    int tamanho_lixo = calcular_tamanho_lixo(frequencias, tabela);
    int tamanho_arvore = calcular_tamanho_arvore(raiz);

    escrever_cabecalho_inicial(arquivo_saida, tamanho_lixo, tamanho_arvore);
    escrever_arvore_pre_ordem(arquivo_saida, raiz);

    fseek(arquivo_entrada, 0, SEEK_SET); // Retorna ponteiro ao início para a gravação
    gravarCodigos(arquivo_saida, arquivo_entrada, tabela);

    // Defesa: Evitando Memory Leaks fechando ponteiros abertos
    fclose(arquivo_entrada);
    fclose(arquivo_saida);
    free(fila); 
    liberar_arvore(raiz);

    printf("Sucesso! Arquivo comprimido salvo como: %s\n", caminho_saida);
}

void descomprimir(char *caminho_arquivo_comprimido) {
    FILE *arquivo_entrada = fopen(caminho_arquivo_comprimido, "rb");
    if (!arquivo_entrada) {
        printf("Erro ao abrir o arquivo comprimido.\n");
        return;
    }

    char caminho_saida[512];
    strncpy(caminho_saida, caminho_arquivo_comprimido, sizeof(caminho_saida) - 1);
    char *extensao = strstr(caminho_saida, ".huff");
    if (extensao) {
        *extensao = '\0';
    }

    FILE *arquivo_saida = fopen(caminho_saida, "wb");
    if (!arquivo_saida) {
        printf("Erro ao criar o arquivo de saida.\n");
        fclose(arquivo_entrada);
        return;
    }

    // ==========================================
    // ETAPAS GLOBAIS DE DESCOMPRESSÃO
    // ==========================================
    short int lixo = obter_lixo(arquivo_entrada);
    short int tamanho_arvore = obter_tamanho_arvore(arquivo_entrada);
    
    // (Total do Arquivo) - (2 bytes do cabeçalho de lixo e tam. árvore) - (Tamanho serializado da árvore)
    unsigned long long int tamanho_total_dados = obterTamanhoCompressao(arquivo_entrada) - (2 + tamanho_arvore);

    NoArvore *raiz = reconstruir_arvore_huffman(arquivo_entrada, &tamanho_arvore);
    descomprimir_arquivo_usando_huffman(arquivo_entrada, lixo, tamanho_total_dados, raiz, arquivo_saida);

    fclose(arquivo_entrada);
    fclose(arquivo_saida);
    liberar_arvore(raiz);

    printf("Sucesso! Arquivo descomprimido salvo como: %s\n", caminho_saida);
}