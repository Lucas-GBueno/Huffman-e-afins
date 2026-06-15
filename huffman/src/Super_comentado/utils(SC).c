#include "utils.h" // Inclui o cabeçalho "utils.h" para conectar as assinaturas com as implementações.

// Início da função que imprime o menu principal e retorna a opção digitada pelo usuário.
int mensagem_inicial() {
    puts("0 - Encerrar");     // Imprime na tela a string da opção de fechar o programa e pula linha.
    puts("1 - Comprimir");    // Imprime na tela a string da opção de compressão e pula linha.
    puts("2 - Descomprimir"); // Imprime na tela a string da opção de descompressão e pula linha.
    
    int escolha;              // Declara uma variável inteira local chamada "escolha".
    scanf("%d", &escolha);    // Lê a entrada do teclado, espera um número inteiro e salva no endereço de "escolha".
    return escolha;           // Devolve o valor armazenado em "escolha" para quem chamou a função.
}                             // Fim da função mensagem_inicial.

// ==========================================================================
// FUNÇÕES DE ENCAPSULAMENTO GENÉRICO (Ponteiros void*)
// ==========================================================================

// Início da função que envelopa um caractere primitivo dentro de um ponteiro genérico void*.
void* encapsular_byte(unsigned char byte) {
    // Aloca dinamicamente na memória Heap espaço suficiente para armazenar 1 byte (unsigned char).
    unsigned char *ptr = (unsigned char *)malloc(sizeof(unsigned char));
    
    // Verifica se a alocação de memória foi bem-sucedida (se o ponteiro ptr não é nulo).
    if (ptr != NULL) {
        *ptr = byte; // Guarda o valor do parâmetro "byte" dentro do endereço de memória apontado por ptr.
    }
    return (void *)ptr; // Retorna o ponteiro ptr, transformando-o explicitamente em um ponteiro genérico void*.
}                       // Fim da função encapsular_byte.

// Início da função que abre o envelope genérico e extrai o caractere original de volta.
unsigned char extrair_byte(void *item) {
    // Verifica se o ponteiro recebido por parâmetro é nulo para evitar falha de segmentação.
    if (item == NULL) return '\0'; // Se for nulo, retorna o caractere nulo de encerramento de string.
    
    // Converte o void* de volta para um ponteiro de caractere (unsigned char*) e acessa o valor contido nele usando *.
    return *((unsigned char *)item); 
} // Fim da função extrair_byte.

// ==========================================================================
// IMPLEMENTAÇÃO DA ÁRVORE GENÉRICA
// ==========================================================================

// Início da função que cria e popula um nó de árvore de Huffman na memória Heap.
NoArvore* criar_no_arvore(void *item, int frequencia, NoArvore *esq, NoArvore *dir) {
    // Aloca dinamicamente na memória Heap espaço suficiente para a estrutura de um NoArvore.
    NoArvore *novo_no = (NoArvore *)malloc(sizeof(NoArvore));
    
    // Verifica se o sistema operacional concedeu a memória com sucesso.
    if (novo_no != NULL) {
        novo_no->item = item;             // Atribui o ponteiro genérico do item ao campo 'item' do novo nó.
        novo_no->frequencia = frequencia; // Atribui o valor da frequência ao campo 'frequencia' do novo nó.
        novo_no->esquerda = esq;          // Conecta o nó passado por parâmetro como o filho da esquerda.
        novo_no->direita = dir;           // Conecta o nó passado por parâmetro como o filho da direita.
    }
    return novo_no; // Retorna o endereço do nó recém-criado na memória.
} // Fim da função criar_no_arvore.

// Início da função que checa se um nó é uma folha (caractere terminal).
int e_folha(NoArvore *no) {
    // Se o nó em si for nulo, ele não pode ser uma folha. Retorna 0 (falso).
    if (no == NULL) return 0;
    
    // Retorna 1 (verdadeiro) se os ponteiros da esquerda E da direita forem nulos ao mesmo tempo.
    return (no->esquerda == NULL && no->direita == NULL);
} // Fim da função e_folha.

// Início da função recursiva que limpa a árvore inteira da memória para evitar vazamentos.
void liberar_arvore(NoArvore *raiz) {
    // Se a raiz atual não for nula, significa que ainda existem nós para limpar neste ramo.
    if (raiz != NULL) {
        liberar_arvore(raiz->esquerda); // Chama a si mesma para descer e limpar a subárvore da esquerda.
        liberar_arvore(raiz->direita);  // Chama a si mesma para descer e limpar a subárvore da direita.
        
        // Verifica se o nó atual é uma folha e se o seu conteúdo genérico existe.
        if (e_folha(raiz) && raiz->item != NULL) {
            free(raiz->item); // Dá free no byte que alocamos via "encapsular_byte" lá em cima.
        }
        free(raiz); // Libera da memória Heap a estrutura do nó em si.
    }
} // Fim da função liberar_arvore.

// ==========================================================================
// IMPLEMENTAÇÃO DA FILA DE PRIORIDADE GENÉRICA
// ==========================================================================

// Início da função que cria o cabeçalho da Fila de Prioridade na Heap.
FilaPrioridade* criar_fila() {
    // Aloca espaço suficiente para a estrutura de controle da FilaPrioridade.
    FilaPrioridade *fila = (FilaPrioridade *)malloc(sizeof(FilaPrioridade));
    
    // Verifica se a alocação foi bem-sucedida.
    if (fila != NULL) {
        fila->cabeca = NULL; // Define que, inicialmente, a fila está vazia (cabeça aponta para nulo).
        fila->tamanho = 0;   // Define o contador de tamanho inicial como zero.
    }
    return fila; // Retorna o ponteiro de controle da fila.
} // Fim da função criar_fila.

// Início da função que verifica se a fila está vazia.
int fila_vazia(FilaPrioridade *fila) {
    // Retorna 1 (verdadeiro) se o ponteiro da fila for nulo ou se a cabeça apontar para nulo.
    return (fila == NULL || fila->cabeca == NULL);
} // Fim da função fila_vazia.

// Início da função que insere elementos na fila mantendo a ordem crescente de prioridade (frequência).
void inserir_fila(FilaPrioridade *fila, void *dado, int prioridade) {
    if (fila == NULL) return; // Se a fila passada não existir na memória, encerra a função imediatamente.

    // Aloca espaço na Heap para criar um novo nó interno da fila (NoFila).
    NoFila *novo_no = (NoFila *)malloc(sizeof(NoFila));
    if (novo_no == NULL) return; // Se faltar memória no sistema, aborta a inserção.
    
    novo_no->dado = dado;                 // Salva o dado (que no nosso caso é um NoArvore*) no campo genérico.
    novo_no->prioridade = prioridade;     // Define a prioridade do nó (sua frequência).
    novo_no->proximo = NULL;              // Define que, por enquanto, o próximo nó é nulo.

    // Caso A: Se a fila estiver vazia OU o novo nó tiver menor prioridade que o atual primeiro da fila.
    if (fila_vazia(fila) || prioridade < fila->cabeca->prioridade) {
        novo_no->proximo = fila->cabeca; // O próximo do novo nó passa a apontar para a antiga cabeça.
        fila->cabeca = novo_no;          // A cabeça da fila passa a ser oficialmente o novo nó.
    } else {
        // Caso B: O novo nó deve ser encaixado no meio ou no fim da fila.
        NoFila *atual = fila->cabeca; // Cria um ponteiro auxiliar chamado "atual" que começa na cabeça.
        
        // Caminha pela fila enquanto o próximo nó não for nulo E tiver prioridade menor ou igual ao novo nó.
        while (atual->proximo != NULL && atual->proximo->prioridade <= prioridade) {
            atual = atual->proximo; // Move o ponteiro auxiliar para o próximo nó.
        }
        // Encontrou a posição correta: costura os ponteiros para encaixar o novo nó.
        novo_no->proximo = atual->proximo; // O próximo do novo nó aponta para o próximo do atual.
        atual->proximo = novo_no;          // O próximo do atual aponta para o novo nó.
    }
    fila->tamanho++; // Incrementa em 1 unidade o contador de tamanho da fila.
} // Fim da função inserir_fila.

// Início da função que remove o elemento de menor peso (a cabeça da fila).
void* remover_fila(FilaPrioridade *fila) {
    if (fila_vazia(fila)) return NULL; // Se a fila estiver vazia, não há o que remover. Retorna nulo.

    NoFila *removido = fila->cabeca; // Guarda o endereço do nó da cabeça na variável auxiliar "removido".
    void *dado = removido->dado;     // Salva o dado contido nele (o NoArvore*) para não perdê-lo no free.
    
    fila->cabeca = removido->proximo; // Desloca a cabeça da fila para o segundo elemento.
    fila->tamanho--;                  // Decrementa em 1 unidade o contador de tamanho da fila.
    
    free(removido); // Deleta da memória Heap o nó de controle da fila (o invólucro NoFila).
    return dado;    // Retorna o ponteiro do dado (NoArvore*) que estava guardado na cabeça.
} // Fim da função remover_fila.