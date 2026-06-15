#include <stdio.h>    
#include <stdlib.h>   
#include <stdbool.h>  
#include <string.h>   
#include <math.h>     

// ==========================================
// 1) ESTRUTURA DE DADOS GENÉRICA (void*)
// ==========================================
typedef struct {
    void **elementos; 
    int capacidade;   
    int quantidade;   
} VetorDirecional;

VetorDirecional* criar_vetor(void) {
    VetorDirecional *v = (VetorDirecional*)malloc(sizeof(VetorDirecional));
    if (!v) return NULL; 
    v->capacidade = 4; 
    v->quantidade = 0; 
    v->elementos = (void**)malloc(sizeof(void*) * v->capacidade);
    return v; 
}

void vetor_inserir(VetorDirecional *v, void *item) {
    if (v->quantidade == v->capacidade) {
        v->capacidade *= 2; 
        v->elementos = (void**)realloc(v->elementos, sizeof(void*) * v->capacidade);
    }
    v->elementos[v->quantidade++] = item;
}

void* vetor_obter(const VetorDirecional *v, int indice) {
    if (indice < 0 || indice >= v->quantidade) return NULL;
    return v->elementos[indice];
}

void vetor_limpar(VetorDirecional *v, void (*liberar_elemento)(void*)) {
    if (!v) return; 
    if (liberar_elemento) {
        for (int i = 0; i < v->quantidade; i++) {
            liberar_elemento(v->elementos[i]); 
        }
    }
    free(v->elementos); 
    free(v);            
}

// ==========================================
// 2) MODELAGEM DA TEORIA LIA
// ==========================================
typedef struct {
    int a;  // O multiplicador do x
    int b;  // O valor somado ou subtraido
    int op; // 1 para <=, 2 para >=
    int c;  // O resultado da inequacao
} Inequacao;

void liberar_inequacao(void *eq) {
    free((Inequacao*)eq);
}

// ==========================================
// 3) PARSER DO ARQUIVO (Leitor de Expressoes Literais)
// ==========================================
void mapear_arquivo_smt(const char *caminho, VetorDirecional *equacoes) {
    FILE *arquivo = fopen(caminho, "r"); 
    if (!arquivo) { 
        perror("Falha ao abrir o arquivo especificado");
        exit(EXIT_FAILURE);
    }

    char buffer[1024]; 

    while (fgets(buffer, sizeof(buffer), arquivo)) {
        // Ignora comentarios
        if (buffer[0] == 'c' || buffer[0] == 'p' || buffer[0] == '\n' || buffer[0] == '\r') continue;

        // Limpa a string: remove todos os espacos em branco da linha para facilitar a leitura
        char linha_limpa[1024];
        int j = 0;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (buffer[i] != ' ' && buffer[i] != '\t' && buffer[i] != '\n' && buffer[i] != '\r') {
                linha_limpa[j++] = buffer[i];
            }
        }
        linha_limpa[j] = '\0'; // Fecha a string limpa

        if (strlen(linha_limpa) == 0) continue; // Linha vazia
        if (linha_limpa[0] == 'c') continue;    // Comentario identado

        Inequacao *nova_eq = (Inequacao*)malloc(sizeof(Inequacao));
        nova_eq->a = 1; nova_eq->b = 0; nova_eq->op = 0; nova_eq->c = 0;

        // Procura onde estao as partes chave da expressao (o 'x' e o operador)
        char *ptr_x = strchr(linha_limpa, 'x');
        char *ptr_op = strstr(linha_limpa, "<=");
        
        if (ptr_op) {
            nova_eq->op = 1;
        } else {
            ptr_op = strstr(linha_limpa, ">=");
            if (ptr_op) nova_eq->op = 2;
        }

        // Se nao tem 'x' ou nao tem operador de limite, a linha esta incorreta
        if (!ptr_x || !ptr_op) { 
            free(nova_eq); 
            continue; 
        }

        // --- Extracao do 'A' (tudo que vem antes do x) ---
        if (ptr_x == linha_limpa) {
            nova_eq->a = 1; // Ex: "x"
        } else if (ptr_x == linha_limpa + 1 && linha_limpa[0] == '-') {
            nova_eq->a = -1; // Ex: "-x"
        } else if (ptr_x == linha_limpa + 1 && linha_limpa[0] == '+') {
            nova_eq->a = 1;  // Ex: "+x"
        } else {
            *ptr_x = '\0'; // Corta a string no 'x'
            nova_eq->a = atoi(linha_limpa); // Le os numeros antes do x
        }

        // --- Extracao do 'B' (tudo entre o x e o operador) ---
        char *ptr_b = ptr_x + 1; // Pula o 'x'
        *ptr_op = '\0';          // Corta a string no sinal de <= ou >=
        if (strlen(ptr_b) > 0) {
            nova_eq->b = atoi(ptr_b); // Le o que sobrou (ex: "+1" ou "-2")
        }

        // --- Extracao do 'C' (tudo depois do operador) ---
        char *ptr_c = ptr_op + 2; // Pula os dois caracteres do operador ("<=")
        nova_eq->c = atoi(ptr_c); // Le o lado direito

        vetor_inserir(equacoes, (void*)nova_eq);
    }
    fclose(arquivo); 
}

// ==========================================
// 4) MOTOR SMT (Normalização e Interseção)
// ==========================================
void resolver_smt_lia(VetorDirecional *equacoes) {
    if (equacoes->quantidade == 0) {
        printf("Erro: Nenhuma equacao valida foi lida.\n");
        return;
    }

    int limite_inferior = -999999;
    int limite_superior = 999999;

    for (int i = 0; i < equacoes->quantidade; i++) {
        Inequacao *eq = (Inequacao*)vetor_obter(equacoes, i);
        
        double lado_direito = (double)(eq->c - eq->b);
        double a = (double)eq->a; 

        if (a == 0) continue; 

        double valor_exato = lado_direito / a;
        int op_atual = eq->op; 

        if (a < 0) {
            op_atual = (op_atual == 1) ? 2 : 1; 
        }

        if (op_atual == 1) { 
            int limite = (int)floor(valor_exato);
            if (limite < limite_superior) limite_superior = limite;
        } 
        else if (op_atual == 2) { 
            int limite = (int)ceil(valor_exato);
            if (limite > limite_inferior) limite_inferior = limite;
        }
    }

    if (limite_inferior <= limite_superior) { 
        printf("SAT! Solucoes na intersecao:\n");

        if (limite_inferior == -999999 && limite_superior == 999999) {
            printf("x E Todos os Numeros Inteiros (Reta Infinita)\n"); 
        } 
        else if (limite_inferior == -999999) {
            printf("Limites exatos: x <= %d\n", limite_superior); 
        } 
        else if (limite_superior == 999999) {
            printf("Limites exatos: x >= %d\n", limite_inferior); 
        } 
        else {
            printf("x E [%d, %d]\n", limite_inferior, limite_superior);
            printf("Inteiros validos: ");
            for (int x = limite_inferior; x <= limite_superior; x++) {
                printf("x=%d ", x); 
                if (x < limite_superior) printf("or "); 
            }
            printf("\n");
        }
    } else {
        printf("UNSAT! Limites em conflito (x >= %d e x <= %d)\n", limite_inferior, limite_superior);
    }
}

// ==========================================
// 5) PONTO DE ENTRADA
// ==========================================
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Modo de uso: %s <arquivo.smt>\n", argv[0]);
        return EXIT_FAILURE; 
    }

    VetorDirecional *equacoes = criar_vetor();
    mapear_arquivo_smt(argv[1], equacoes);
    resolver_smt_lia(equacoes);
    vetor_limpar(equacoes, liberar_inequacao);
    
    return EXIT_SUCCESS; 
}