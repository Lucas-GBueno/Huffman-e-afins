#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define TAMANHO_ASCII 256

/* ==========================================================================
 * ESTRUTURAS DE DADOS GENÉRICAS (Obrigatório por Edital)
 * Defesa: O uso de void* isola a estrutura. Fila e Árvore são Tipos 
 * Abstratos de Dados (TADs) puros e independentes da lógica do Huffman.
 * ========================================================================== */

/**
 * NoArvore: Estrutura do nó da árvore de Huffman.
 * Gatilho: O ponteiro genérico 'item' aponta para o byte encapsulado.
 * Se for nó interno, 'item' aponta para um marcador (como '*').
 */
typedef struct no_arvore {
    void *item;                  // Ponteiro genérico para armazenar o símbolo (byte)
    int frequencia;              // Peso do nó para ordenação e construção
    struct no_arvore *esquerda;  // Subárvore do bit 0
    struct no_arvore *direita;   // Subárvore do bit 1
} NoArvore;

/**
 * NoFila: Elemento interno da fila de prioridade encadeada.
 * Gatilho: Totalmente independente. O ponteiro 'dado' recebe um NoArvore*.
 * Isso prova a genericidade real da estrutura da fila.
 */
typedef struct no_fila {
    void *dado;                  // Armazena qualquer estrutura (no nosso caso, NoArvore*)
    int prioridade;              // Valor de ordenação (frequência do caractere)
    struct no_fila *proximo;     // Próximo elemento da fila
} NoFila;

/**
 * FilaPrioridade: Encapsulamento da fila.
 * Gatilho: Mantém o controle da cabeça e tamanho para operações O(1) de verificação.
 */
typedef struct fila_prioridade {
    NoFila *cabeca;
    int tamanho;
} FilaPrioridade;

/**
 * dado_objeto: Estrutura do Dicionário de Códigos.
 * Gatilho: 'conteudo' usa unsigned int para evitar extensão de sinal em shifts (<<, >>).
 * Garante que caminhos binários de até 32 bits sejam calculados sem corromper.
 */
typedef struct dado_objeto {
    unsigned int conteudo;       // Mapeamento dos bits (ex: 0b101)
    int tamanho;                 // Quantidade de bits válidos no código
} dado_objeto;


/* ==========================================================================
 * ASSINATURAS DAS FUNÇÕES DE CONTROLE
 * ========================================================================== */

// Operações da Fila Genérica (Controle de tamanho e inserção ordenada O(N))
FilaPrioridade* criar_fila();
void inserir_fila(FilaPrioridade *fila, void *dado, int prioridade);
void* remover_fila(FilaPrioridade *fila);
int fila_vazia(FilaPrioridade *fila);

// Operações da Árvore (Gerenciamento de nós e folhas)
NoArvore* criar_no_arvore(void *item, int frequencia, NoArvore *esq, NoArvore *dir);
int e_folha(NoArvore *no);
void liberar_arvore(NoArvore *raiz); // Defesa: Libera recursivamente nós e dados genéricos das folhas

// Funções de Encapsulamento (Garantem que o dado cru entre e saia do void*)
void* encapsular_byte(unsigned char byte);   // Aloca memória dinâmica para um único byte
unsigned char extrair_byte(void *item);      // Faz o cast seguro de void* para unsigned char

int mensagem_inicial();

#endif