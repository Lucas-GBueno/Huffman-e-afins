#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

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
// 2) MODELAGEM DO PROBLEMA (SAT)
// ==========================================
typedef struct {
    VetorDirecional *clausulas;
    int total_variaveis;
} FormulaCNF;

void liberar_clausula(void *c) {
    vetor_limpar((VetorDirecional*)c, NULL);
}

void mapear_dimacs(const char *caminho, FormulaCNF *f) {
    FILE *arquivo = fopen(caminho, "r");
    if (!arquivo) {
        perror("Falha ao abrir o arquivo especificado");
        exit(EXIT_FAILURE);
    }

    f->clausulas = criar_vetor();
    f->total_variaveis = 0;

    char buffer[4096];
    int clausulas_esperadas = 0;

    while (fgets(buffer, sizeof(buffer), arquivo)) {
        if (buffer[0] == 'c' || buffer[0] == '\n' || buffer[0] == '\r') continue;

        if (buffer[0] == 'p') {
            sscanf(buffer, "p cnf %d %d", &f->total_variaveis, &clausulas_esperadas);
            continue;
        }

        VetorDirecional *clausula_atual = criar_vetor();
        char *token = strtok(buffer, " \t\r\n");
        
        while (token) {
            int valor_literal = atoi(token);
            if (valor_literal == 0) break;
            
            // Refatoração: int encapsulado dentro do ponteiro genérico
            vetor_inserir(clausula_atual, (void*)(intptr_t)valor_literal);
            token = strtok(NULL, " \t\r\n");
        }

        if (clausula_atual->quantidade > 0) {
            vetor_inserir(f->clausulas, clausula_atual);
        } else {
            vetor_limpar(clausula_atual, NULL);
        }
    }

    fclose(arquivo);
}

// ==========================================
// 3) MOTOR DE INFERÊNCIA (Lógica SAT)
// ==========================================
int avaliar_clausula(const VetorDirecional *clausula, const int *interpretacao, int max_vars) {
    bool depende = false;
    
    for (int i = 0; i < clausula->quantidade; i++) {
        int literal = (int)(intptr_t)vetor_obter(clausula, i);
        int variavel = abs(literal);
        
        //ignora variáveis acima do limite do cabeçalho
        if (variavel > max_vars) {
            depende = true;
            continue;
        }

        int valor_atual = interpretacao[variavel]; 
        if (valor_atual != -1) {
            if ((valor_atual == 1 && literal > 0) || (valor_atual == 0 && literal < 0)) {
                return 1; 
            }
        } else {
            depende = true; 
        }
    }
    
    return depende ? -1 : 0; 
}

bool formula_satisfeita(const FormulaCNF *f, const int *interpretacao) {
    for (int i = 0; i < f->clausulas->quantidade; i++) {
        VetorDirecional *c = (VetorDirecional*)vetor_obter(f->clausulas, i);
        if (avaliar_clausula(c, interpretacao, f->total_variaveis) != 1) return false;
    }
    return true; 
}

bool formula_falsificada(const FormulaCNF *f, const int *interpretacao) {
    for (int i = 0; i < f->clausulas->quantidade; i++) {
        VetorDirecional *c = (VetorDirecional*)vetor_obter(f->clausulas, i);
        if (avaliar_clausula(c, interpretacao, f->total_variaveis) == 0) return true;
    }
    return false; 
}

bool resolver_sat(FormulaCNF *f, int *interpretacao) {
    if (formula_satisfeita(f, interpretacao)) return true;
    if (formula_falsificada(f, interpretacao)) return false;

    int var_alvo = -1;
    // Limite de varredura atrelado estritamente ao cabeçalho
    for (int i = 1; i <= f->total_variaveis; i++) {
        if (interpretacao[i] == -1) {
            var_alvo = i;
            break;
        }
    }
    
    if (var_alvo == -1) return false; 

    interpretacao[var_alvo] = 1;
    if (resolver_sat(f, interpretacao)) return true;

    interpretacao[var_alvo] = 0;
    if (resolver_sat(f, interpretacao)) return true;

    interpretacao[var_alvo] = -1;
    return false;
}

// ==========================================
// 4) PONTO DE ENTRADA
// ==========================================
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Modo de uso: %s <arquivo.cnf>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FormulaCNF formula;
    mapear_dimacs(argv[1], &formula);

    int *interpretacao = (int*)malloc(sizeof(int) * (formula.total_variaveis + 1));
    for (int i = 0; i <= formula.total_variaveis; i++) {
        interpretacao[i] = -1; 
    }

    if (resolver_sat(&formula, interpretacao)) {
        printf("SAT!\n");
    } else {
        printf("UNSAT!\n");
    }

    free(interpretacao);
    vetor_limpar(formula.clausulas, liberar_clausula);

    return EXIT_SUCCESS;
}
