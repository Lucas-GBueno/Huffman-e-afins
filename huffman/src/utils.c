#include "utils.h"

int mensagem_inicial() {
    puts("0 - Encerrar");
    puts("1 - Comprimir");
    puts("2 - Descomprimir");
    
    int escolha;
    scanf("%d", &escolha);
    return escolha;
}

/* ==========================================================================
 * FUNÇÕES DE ENCAPSULAMENTO GENÉRICO
 * Defesa: O C não permite passar tipos primitivos (como char) diretamente
 * para um void*. Por isso, alocamos 1 byte dinamicamente na Heap. 
 * É o preço a se pagar para ter uma estrutura 100% genérica.
 * ========================================================================== */

void* encapsular_byte(unsigned char byte) {
    unsigned char *ptr = (unsigned char *)malloc(sizeof(unsigned char));
    if (ptr != NULL) {
        *ptr = byte;
    }
    return (void *)ptr;
}

unsigned char extrair_byte(void *item) {
    if (item == NULL) return '\0';
    return *((unsigned char *)item); // Cast explícito de volta para unsigned char
}

/* ==========================================================================
 * IMPLEMENTAÇÃO DA ÁRVORE GENÉRICA
 * ========================================================================== */

NoArvore* criar_no_arvore(void *item, int frequencia, NoArvore *esq, NoArvore *dir) {
    NoArvore *novo_no = (NoArvore *)malloc(sizeof(NoArvore));
    if (novo_no != NULL) {
        novo_no->item = item;
        novo_no->frequencia = frequencia;
        novo_no->esquerda = esq;
        novo_no->direita = dir;
    }
    return novo_no;
}

int e_folha(NoArvore *no) {
    if (no == NULL) return 0;
    return (no->esquerda == NULL && no->direita == NULL);
}

void liberar_arvore(NoArvore *raiz) {
    // Defesa: Travessia em Pós-Ordem (Esq -> Dir -> Raiz) para evitar
    // Memory Leak. Se acessarmos a raiz antes, perdemos os filhos.
    if (raiz != NULL) {
        liberar_arvore(raiz->esquerda);
        liberar_arvore(raiz->direita);
        
        // Como encapsulamos o byte dinamicamente (malloc no utils), 
        // TEMOS que dar free no item genérico antes de matar o nó.
        if (e_folha(raiz) && raiz->item != NULL) {
            free(raiz->item);
        }
        free(raiz);
    }
}

/* ==========================================================================
 * IMPLEMENTAÇÃO DA FILA DE PRIORIDADE GENÉRICA
 * Defesa: A inserção é O(N) no pior caso, pois precisamos varrer a fila 
 * para achar a posição. Mas a remoção (que pega os de menor frequência)
 * é O(1), pois tiramos sempre da cabeça.
 * ========================================================================== */

FilaPrioridade* criar_fila() {
    FilaPrioridade *fila = (FilaPrioridade *)malloc(sizeof(FilaPrioridade));
    if (fila != NULL) {
        fila->cabeca = NULL;
        fila->tamanho = 0;
    }
    return fila;
}

int fila_vazia(FilaPrioridade *fila) {
    return (fila == NULL || fila->cabeca == NULL);
}

void inserir_fila(FilaPrioridade *fila, void *dado, int prioridade) {
    if (fila == NULL) return;

    NoFila *novo_no = (NoFila *)malloc(sizeof(NoFila));
    if (novo_no == NULL) return;
    
    novo_no->dado = dado;
    novo_no->prioridade = prioridade;
    novo_no->proximo = NULL;

    // Se estiver vazia ou o novo elemento for o de menor peso, vira a cabeça
    if (fila_vazia(fila) || prioridade < fila->cabeca->prioridade) {
        novo_no->proximo = fila->cabeca;
        fila->cabeca = novo_no;
    } else {
        // Percorre até achar o nó com prioridade maior ou igual
        NoFila *atual = fila->cabeca;
        while (atual->proximo != NULL && atual->proximo->prioridade <= prioridade) {
            atual = atual->proximo;
        }
        novo_no->proximo = atual->proximo;
        atual->proximo = novo_no;
    }
    fila->tamanho++;
}

void* remover_fila(FilaPrioridade *fila) {
    if (fila_vazia(fila)) return NULL;

    NoFila *removido = fila->cabeca;
    void *dado = removido->dado; // Resgata o NoArvore* guardado no void*
    
    fila->cabeca = removido->proximo;
    fila->tamanho--;
    
    // Defesa: Damos free no nó da FILA, mas o DADO (a árvore) continua intacto na memória.
    free(removido); 
    return dado;
}