#ifndef UTILS_H  // Se a constante UTILS_H ainda não foi definida no projeto...
#define UTILS_H  // ...defina-a agora (evita que o compilador inclua este arquivo duas vezes).

#include <stdio.h>   // Inclui a biblioteca padrão de Entrada/Saída (para usar o tipo FILE).
#include <stdlib.h>  // Inclui a biblioteca padrão de utilitários (para usar malloc e free).
#include <stdint.h>  // Inclui tipos de inteiros com tamanhos fixos e garantidos.

#define TAMANHO_ASCII 256  // Define uma constante: o alfabeto ASCII tem 256 símbolos possíveis (0 a 255).

// ==========================================
// DEFINIÇÃO DAS ESTRUTURAS DE DADOS (STRUCTS)
// ==========================================

// Início da definição da estrutura que representa um nó da nossa Árvore de Huffman.
typedef struct no_arvore {
    void *item;                  // Ponteiro genérico que guarda o caractere real (alocado no utils.c).
    int frequencia;              // Peso numérico do nó (quantas vezes o caractere apareceu no arquivo).
    struct no_arvore *esquerda;  // Ponteiro para o nó filho da esquerda (representa o bit 0).
    struct no_arvore *direita;   // Ponteiro para o nó filho da direita (representa o bit 1).
} NoArvore;                      // Define o apelido "NoArvore" para facilitar o uso no código.

// Início da definição do nó interno da nossa Fila de Prioridade encadeada.
typedef struct no_fila {
    void *dado;                  // Ponteiro genérico que vai carregar um endereço do tipo (NoArvore*).
    int prioridade;              // Valor usado para ordenar a fila (será a frequência do caractere).
    struct no_fila *proximo;     // Ponteiro que aponta para o próximo elemento da fila encadeada.
} NoFila;                        // Define o apelido "NoFila".

// Início da definição do cabeçalho da Fila de Prioridade.
typedef struct fila_prioridade {
    NoFila *cabeca;              // Ponteiro que aponta estritamente para o primeiro elemento da fila.
    int tamanho;                 // Contador inteiro que rastreia quantos nós existem na fila no momento.
} FilaPrioridade;                // Define o apelido "FilaPrioridade".

// Início da definição do objeto que representa o Dicionário de Códigos de cada caractere.
typedef struct dado_objeto {
    unsigned int conteudo;       // Guarda a sequência de bits do caminho da árvore (ex: 0b101).
    int tamanho;                 // Guarda quantos bits aquele caminho específico possui.
} dado_objeto;                   // Define o apelido "dado_objeto".


// ==========================================
// ASSINATURAS DAS FUNÇÕES (CONTRATOS)
// ==========================================

// Declara a função que aloca e inicializa uma Fila de Prioridade vazia na memória Heap.
FilaPrioridade* criar_fila();

// Declara a função que insere um dado na fila de forma ordenada, baseada na sua prioridade.
void inserir_fila(FilaPrioridade *fila, void *dado, int prioridade);

// Declara a função que remove o elemento do início da fila (o de menor frequência) e o retorna.
void* remover_fila(FilaPrioridade *fila);

// Declara a função que checa se a fila está vazia (retorna 1 para sim, 0 para não).
int fila_vazia(FilaPrioridade *fila);

// Declara a função que aloca um nó de árvore na Heap e preenche seus dados e ponteiros.
NoArvore* criar_no_arvore(void *item, int frequencia, NoArvore *esq, NoArvore *dir);

// Declara a função que verifica se um nó da árvore é uma folha (não tem filhos na esquerda nem direita).
int e_folha(NoArvore *no);

// Declara a função que varre a árvore recursivamente limpando todos os nós com a função free.
void liberar_arvore(NoArvore *raiz);

// Declara a função que aloca 1 byte na memória Heap para envelopar um char primitivo em void*.
void* encapsular_byte(unsigned char byte);

// Declara a função que desfaz o envelope genérico, devolvendo o caractere como unsigned char puro.
unsigned char extrair_byte(void *item);

// Declara a função que imprime o menu textual de opções e captura a escolha do usuário.
int mensagem_inicial();

#endif // Encerra o bloco de proteção do cabeçalho (#ifndef UTILS_H).