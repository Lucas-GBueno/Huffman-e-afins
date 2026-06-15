#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// =======================================================
// 1. ESTRUTURAS COM VOID* // =======================================================
typedef struct NoLista {
    void* dado;
    struct NoLista* prox;
} NoLista;

typedef struct NoArvore {
    void* dado;
    struct NoArvore* esq;
    struct NoArvore* dir;
} NoArvore;

// =======================================================
// 2. FUNÇÕES DA LISTA ENCADEADA
// =======================================================
NoLista* inserir_lista(NoLista* lista, void* dado) {
    NoLista* novo = (NoLista*)malloc(sizeof(NoLista));
    novo->dado = dado;
    novo->prox = lista;
    return novo; // Insere no início (mais rápido para popular)
}

int buscar_lista(NoLista* lista, int valor_buscado) {
    int comparacoes = 0;
    NoLista* atual = lista;
    
    while (atual != NULL) {
        comparacoes++;
        int valor_atual = *(int*)(atual->dado); // Faz o cast do void* para int
        
        if (valor_atual == valor_buscado) {
            return comparacoes; // Achou
        }
        atual = atual->prox;
    }
    return comparacoes; // Não achou, mas varreu tudo
}

// =======================================================
// 3. FUNÇÕES DA ÁRVORE DE BUSCA BINÁRIA
// =======================================================
NoArvore* inserir_arvore(NoArvore* raiz, void* dado) {
    if (raiz == NULL) {
        NoArvore* novo = (NoArvore*)malloc(sizeof(NoArvore));
        novo->dado = dado;
        novo->esq = NULL;
        novo->dir = NULL;
        return novo;
    }
    
    int valor_inserir = *(int*)dado;
    int valor_raiz = *(int*)(raiz->dado);

    if (valor_inserir < valor_raiz) {
        raiz->esq = inserir_arvore(raiz->esq, dado);
    } else if (valor_inserir > valor_raiz) {
        raiz->dir = inserir_arvore(raiz->dir, dado);
    }
    // Ignorando valores iguais para simplificar a ABB
    return raiz;
}

int buscar_arvore(NoArvore* raiz, int valor_buscado) {
    int comparacoes = 0;
    NoArvore* atual = raiz;
    
    while (atual != NULL) {
        comparacoes++;
        int valor_atual = *(int*)(atual->dado);

        if (valor_buscado == valor_atual) {
            return comparacoes; // Achou
        } else if (valor_buscado < valor_atual) {
            atual = atual->esq;
        } else {
            atual = atual->dir;
        }
    }
    return comparacoes;
}

// =======================================================
// 4. PROGRAMA PRINCIPAL
// =======================================================
int main() {
    NoLista* lista = NULL;
    NoArvore* arvore = NULL;
    int qtd_dados = 5000;
    int num_sorteados[qtd_dados];

    srand(time(NULL));

    // A. Preencher as duas estruturas com os mesmos milhares de dados
    printf("Populando as estruturas com %d valores...\n", qtd_dados);
    for (int i = 0; i < qtd_dados; i++) {
        // Aloca o int dinamicamente para poder passar como void*
        int* novo_valor = (int*)malloc(sizeof(int));
        *novo_valor = rand() % 10000;
        num_sorteados[i] = *novo_valor;
        lista = inserir_lista(lista, novo_valor);
        arvore = inserir_arvore(arvore, novo_valor);
    }

    // B. Preparar o arquivo CSV
    FILE *f = fopen("dados.csv", "w");
    if (f == NULL) {
        printf("Erro ao criar o arquivo CSV!\n");
        return 1;
    }
    fprintf(f, "Sorteio,Comparacoes_Lista,Comparacoes_Arvore\n");
    
    int num_sorteios = 50; 
    
    // C. Fazer os sorteios e contar as comparações
    printf("Realizando %d sorteios de busca...\n", num_sorteios);
    for(int i = 0; i < num_sorteios; i++) {
        int indice_sorteado = rand() % 5000;
        int valor_para_buscar = num_sorteados[indice_sorteado];
        int comp_lista = buscar_lista(lista, valor_para_buscar);
        int comp_arvore = buscar_arvore(arvore, valor_para_buscar);
        
        fprintf(f, "%d,%d,%d\n", i+1, comp_lista, comp_arvore);
    }
    
    fclose(f);
    printf("Sucesso! Os dados foram salvos no arquivo 'dados.csv'.\n");
    
    return 0;
}