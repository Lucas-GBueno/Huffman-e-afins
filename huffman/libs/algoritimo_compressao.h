#ifndef ALGORITMOS_COMPRESSAO_H
#define ALGORITMOS_COMPRESSAO_H

#include "utils.h"

/* ==========================================================================
 * FLUXO DE COMPRESSÃO (ETAPAS 1 À 5)
 * Defesa: Processo dividido em varredura, montagem, mapeamento e escrita em disco.
 * ========================================================================== */

/**
 * Passo 1: Varre o arquivo original byte a byte (0 a 255) e popula a fila.
 * Gatilho: Abre em modo binário ("rb") para suportar qualquer formato de arquivo.
 */
FilaPrioridade* obter_frequencia_caracter(FILE *arquivo, int arr_frequencia[TAMANHO_ASCII]);

/**
 * Passo 2: Consome os dois nós de menor frequência da fila e monta uma nova subárvore.
 * Gatilho: Algoritmo guloso (Greedy). Repete até restar apenas a raiz na fila.
 */
NoArvore* construir_arvore_huffman(FilaPrioridade *fila);

/**
 * Passo 3: Percorre a árvore recursivamente gerando o dicionário de caminhos.
 * Gatilho: Esquerda incrementa bit 0, Direita incrementa bit 1. Armazena na tabela de busca rápida.
 */
void gerar_tabela_huffman(NoArvore *raiz, dado_objeto tabela[], unsigned int caminho_atual, int tamanho_atual);

/**
 * Passo 4: Cálculos matemáticos estruturais para a montagem exata do cabeçalho.
 * Gatilho (Tamanho Árvore): Conta escape '\\' para nós que contêm '*' ou '\\' literais.
 * Gatilho (Lixo): Bits totais mod 8 indicam quantos bits do último byte não são dados reais.
 */
int calcular_tamanho_lixo(int frequencia[], dado_objeto tabela[]);
int calcular_tamanho_arvore(NoArvore *raiz);

/**
 * Passo 5: Serialização e escrita dos metadados e do fluxo binário compactado.
 * Gatilho (Cabeçalho): Escreve exatamente 2 bytes (3 bits lixo + 13 bits tamanho árvore).
 * Gatilho (Pré-Ordem): Serializa a árvore (Raiz-Esq-Dir) salvando a topologia estrutural para a descompressão.
 * Gatilho (GravarCódigos): Lê o arquivo pela 2ª vez, empacota em blocos de 8 bits usando buffers e descarrega.
 */
void escrever_cabecalho_inicial(FILE *arquivo_comprimido, int tamanho_lixo, int tamanho_arvore);
void escrever_arvore_pre_ordem(FILE *arquivo_comprimido, NoArvore *raiz);
void gravarCodigos(FILE *arquivo_comprimido, FILE *arquivo_para_comprimir, dado_objeto tabela[]);

#endif