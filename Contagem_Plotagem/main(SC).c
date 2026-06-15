#include <stdio.h> // Importa a biblioteca de entrada e saída padrão (usada para printf, fopen, fprintf).
#include <stdlib.h> // Importa a biblioteca para alocação dinâmica de memória (malloc) e geração de números (rand).
#include <time.h> // Importa a biblioteca de tempo, usada para criar a semente dos números aleatórios.

// =======================================================
// 1. ESTRUTURAS COM VOID* // =======================================================
typedef struct NoLista { // Inicia a definição da estrutura do nó para a lista encadeada.
    void* dado; // Ponteiro genérico (void*) exigido pelo projeto para armazenar o valor sem definir o tipo.
    struct NoLista* prox; // Ponteiro que aponta para o próximo nó da lista encadeada.
} NoLista; // Define o nome do tipo dessa estrutura como NoLista.

typedef struct NoArvore { // Inicia a definição da estrutura do nó para a árvore de busca binária.
    void* dado; // Ponteiro genérico (void*) para armazenar o valor do nó da árvore.
    struct NoArvore* esq; // Ponteiro para o filho à esquerda (onde ficarão armazenados os valores menores).
    struct NoArvore* dir; // Ponteiro para o filho à direita (onde ficarão armazenados os valores maiores).
} NoArvore; // Define o nome do tipo dessa estrutura como NoArvore.

// =======================================================
// 2. FUNÇÕES DA LISTA ENCADEADA
// =======================================================
NoLista* inserir_lista(NoLista* lista, void* dado) { // Função para inserir um novo dado na lista encadeada.
    NoLista* novo = (NoLista*)malloc(sizeof(NoLista)); // Aloca dinamicamente o espaço necessário para um novo nó na memória.
    novo->dado = dado; // Atribui o endereço do dado passado por parâmetro ao campo 'dado' do novo nó.
    novo->prox = lista; // Faz o novo nó apontar para o início da lista atual (inserção rápida no início).
    return novo; // Retorna o novo nó, que agora passa a ser a nova "cabeça" (primeiro elemento) da lista.
} // Fim da função de inserção na lista.

int buscar_lista(NoLista* lista, int valor_buscado) { // Função para procurar um valor na lista e contar as comparações feitas.
    int comparacoes = 0; // Inicializa a variável que vai contar quantas vezes testamos um número.
    NoLista* atual = lista; // Cria um ponteiro auxiliar 'atual' que começa apontando para o início da lista.
    
    while (atual != NULL) { // Laço de repetição que percorre a lista nó por nó até encontrar o final (NULL).
        comparacoes++; // Incrementa o contador de comparações, pois faremos um teste condicional a seguir.
        int valor_atual = *(int*)(atual->dado); // Converte o ponteiro genérico void* para int* e extrai o valor inteiro da memória.
        
        if (valor_atual == valor_buscado) { // Verifica se o valor do nó atual é igual ao valor que estamos procurando.
            return comparacoes; // Se encontrou, interrompe a função imediatamente e retorna a quantidade de comparações feitas até aqui.
        } // Fim da estrutura de verificação.
        atual = atual->prox; // Move o ponteiro 'atual' para o próximo nó da lista para continuar a busca na próxima iteração.
    } // Fim do laço while.
    return comparacoes; // Se percorreu toda a lista e não encontrou, retorna o total de comparações (representa o pior caso).
} // Fim da função de busca na lista.

// =======================================================
// 3. FUNÇÕES DA ÁRVORE DE BUSCA BINÁRIA
// =======================================================
NoArvore* inserir_arvore(NoArvore* raiz, void* dado) { // Função recursiva para inserir um dado na árvore binária.
    if (raiz == NULL) { // Verifica se chegou a uma posição vazia na árvore (este é o caso base da recursão).
        NoArvore* novo = (NoArvore*)malloc(sizeof(NoArvore)); // Aloca memória no heap para criar o novo nó da árvore.
        novo->dado = dado; // Atribui o dado genérico ao campo correspondente no novo nó.
        novo->esq = NULL; // Define o ponteiro do filho à esquerda do novo nó como vazio (NULL).
        novo->dir = NULL; // Define o ponteiro do filho à direita do novo nó como vazio (NULL).
        return novo; // Retorna este novo nó para ser ligado corretamente à estrutura da árvore.
    } // Fim da verificação do caso base.
    
    int valor_inserir = *(int*)dado; // Extrai o valor inteiro do dado que queremos inserir (convertendo de void*).
    int valor_raiz = *(int*)(raiz->dado); // Extrai o valor inteiro do nó atual (raiz) que estamos analisando no momento.

    if (valor_inserir < valor_raiz) { // Compara se o valor a ser inserido é estritamente menor que o valor do nó atual.
        raiz->esq = inserir_arvore(raiz->esq, dado); // Se for menor, chama a recursão para tentar inserir no galho da esquerda.
    } else if (valor_inserir > valor_raiz) { // Caso contrário, verifica se o valor a ser inserido é maior que o nó atual.
        raiz->dir = inserir_arvore(raiz->dir, dado); // Se for maior, chama a recursão para tentar inserir no galho da direita.
    } // Fim das comparações de inserção.
    // Ignorando valores iguais para simplificar a ABB
    return raiz; // Retorna o ponteiro do nó atual para não quebrar a estrutura e conexões da árvore ao voltar da recursão.
} // Fim da função de inserção na árvore.

int buscar_arvore(NoArvore* raiz, int valor_buscado) { // Função para buscar um valor na árvore e contar as comparações.
    int comparacoes = 0; // Inicializa o contador de comparações da árvore.
    NoArvore* atual = raiz; // Cria um ponteiro auxiliar 'atual' que inicia a busca a partir da raiz da árvore.
    
    while (atual != NULL) { // Laço de repetição que desce pelos níveis da árvore enquanto não chegar em uma folha vazia (NULL).
        comparacoes++; // Incrementa o contador de comparações antes de realizar os testes com os valores.
        int valor_atual = *(int*)(atual->dado); // Extrai o valor inteiro do nó atual (convertendo de void*).

        if (valor_buscado == valor_atual) { // Verifica se o valor procurado é exatamente igual ao valor do nó atual.
            return comparacoes; // Se encontrou, encerra a busca e retorna quantas comparações foram necessárias.
        } else if (valor_buscado < valor_atual) { // Se não encontrou, verifica se o valor procurado é menor que o nó atual.
            atual = atual->esq; // Se for menor, move a busca para o filho à esquerda (descartando toda a metade direita).
        } else { // Se o valor buscado não for igual nem menor, ele obrigatoriamente é maior.
            atual = atual->dir; // Move a busca para o filho à direita (descartando toda a metade esquerda da árvore).
        } // Fim da cadeia de verificações da busca.
    } // Fim do laço while.
    return comparacoes; // Retorna o número de comparações caso o laço termine sem encontrar o valor na árvore.
} // Fim da função de busca na árvore.

// =======================================================
// 4. PROGRAMA PRINCIPAL
// =======================================================
int main() {
    NoLista* lista = NULL; //Inicializa a lista como vazia
    NoArvore* arvore = NULL; //Inicializa a árvore como vazia
    int qtd_dados = 5000; //Define o total de elementos
    int num_sorteados[5000]; //Array para guardar valores inseridos

    srand(time(NULL)); //Semente para números aleatórios via relógio

    printf("Populando estruturas...\n"); //Mensagem de progresso no console
    for (int i = 0; i < qtd_dados; i++) { //Inicia o loop de inserção
        int* novo_valor = (int*)malloc(sizeof(int)); //Alocação dinâmica para o dado
        *novo_valor = rand() % 10000; //Sorteia valor entre 0 e 9999
        num_sorteados[i] = *novo_valor; //Guarda valor para busca futura garantida
        lista = inserir_lista(lista, novo_valor); //Insere o valor na lista
        arvore = inserir_arvore(arvore, novo_valor); //Insere o mesmo valor na árvore
    }

    FILE *f = fopen("dados.csv", "w"); //Abre arquivo CSV para escrita
    if (f == NULL) { //Verifica se houve erro ao abrir arquivo
        printf("Erro ao criar o arquivo CSV!\n"); //Mensagem de erro de escrita
        return 1; //Encerra o programa se houver erro
    }
    fprintf(f, "Sorteio,Comparacoes_Lista,Comparacoes_Arvore\n"); //Escreve cabeçalho do CSV

    int num_sorteios = 50; //Define a quantidade de buscas de teste
    printf("Realizando buscas...\n"); //Início da fase de testes
    for(int i = 0; i < num_sorteios; i++) { //Loop de sorteios de performance
        int indice = rand() % 5000; //Sorteia um índice que sabemos que existe
        int valor = num_sorteados[indice]; //Resgata o valor para a busca
        int comp_lista = buscar_lista(lista, valor); //Conta comparações na lista
        int comp_arvore = buscar_arvore(arvore, valor); //Conta comparações na árvore
        fprintf(f, "%d,%d,%d\n", i+1, comp_lista, comp_arvore); //Grava métricas no arquivo
    }

    fclose(f); //Fecha o arquivo e salva dados no disco
    printf("Sucesso! Verifique o 'dados.csv'.\n"); //Mensagem final de sucesso
    return 0; //Fim do programa com código de sucesso
}