#include <stdio.h> // Inclui funções padrão de entrada/saída (printf, fopen)
#include <stdlib.h> // Inclui funções de alocação de memória (malloc, free)
#include <stdbool.h> // Inclui suporte nativo a valores booleanos (true, false)
#include <string.h> // Inclui funções para manipulação de strings (strtok)
#include <stdint.h> // Inclui tipos inteiros de tamanho fixo (intptr_t)

// ==========================================       
// 1) ESTRUTURA DE DADOS GENÉRICA (void*)          
// ==========================================       
typedef struct { // Inicia a definição de uma estrutura de dados
    void **elementos; // Array dinâmico de ponteiros genéricos (void*)
    int capacidade; // Inteiro que guarda o limite atual de itens do array
    int quantidade; // Inteiro que rastreia quantos itens já foram inseridos
} VetorDirecional; // Define o nome desta estrutura como VetorDirecional

VetorDirecional* criar_vetor(void) { // Função que inicializa e retorna um novo vetor
    VetorDirecional *v = (VetorDirecional*)malloc(sizeof(VetorDirecional)); // Aloca memória para a struct na Heap
    if (!v) return NULL; // Se a alocação falhar (sem memória), retorna nulo
    v->capacidade = 4; // Define a capacidade inicial do vetor para 4 posições
    v->quantidade = 0; // Inicia o contador de itens armazenados em 0
    v->elementos = (void**)malloc(sizeof(void*) * v->capacidade); // Aloca o array de ponteiros com base na capacidade
    return v; // Retorna o ponteiro para o vetor recém-criado
} 

void vetor_inserir(VetorDirecional *v, void *item) { // Função para adicionar um novo item genérico ao vetor
    if (v->quantidade == v->capacidade) { // Verifica se o vetor atingiu sua capacidade máxima
        v->capacidade *= 2; // Se sim, dobra a capacidade atual (estratégia de redimensionamento)
        v->elementos = (void**)realloc(v->elementos, sizeof(void*) * v->capacidade); // Realoca a memória com o novo tamanho
    } // Fim do bloco de verificação de capacidade
    v->elementos[v->quantidade++] = item; // Insere o item na posição atual e incrementa a quantidade
}

void* vetor_obter(const VetorDirecional *v, int indice) { // Função para buscar um item baseado no seu índice
    if (indice < 0 || indice >= v->quantidade) return NULL; // Retorna nulo se o índice for inválido ou fora dos limites
    return v->elementos[indice]; // Retorna o ponteiro armazenado na posição solicitada
}

void vetor_limpar(VetorDirecional *v, void (*liberar_elemento)(void*)) { // Função para destruir o vetor e liberar memória
    if (!v) return; // Se o vetor for nulo, não faz nada e sai da função
    if (liberar_elemento) { // Verifica se foi passada uma função customizada para liberar os itens
        for (int i = 0; i < v->quantidade; i++) { // Loop que percorre todos os itens armazenados no vetor
            liberar_elemento(v->elementos[i]); // Chama a função customizada passando o item atual
        } // Fim do loop de liberação de itens
    } // Fim da verificação da função customizada
    free(v->elementos); // Libera a memória alocada para o array de ponteiros
    free(v); // Libera a memória alocada para a estrutura do vetor
} 

// ==========================================    
// 2) MODELAGEM DO PROBLEMA (SAT)               
// ==========================================   
typedef struct { // Inicia a definição da estrutura que representa a fórmula
    VetorDirecional *clausulas; // Vetor onde cada item será outra estrutura contendo uma cláusula
    int total_variaveis; // Número total de variáveis distintas mapeadas na fórmula
} FormulaCNF; // Define o nome da estrutura como FormulaCNF

void liberar_clausula(void *c) { // Função callback para liberar uma cláusula individual da memória
    vetor_limpar((VetorDirecional*)c, NULL); // Chama a função de limpar vetor, já que cada cláusula é um vetor independente
}

void mapear_dimacs(const char *caminho, FormulaCNF *f) { // Função que lê o arquivo CNF e preenche a estrutura da fórmula
    FILE *arquivo = fopen(caminho, "r"); // Tenta abrir o arquivo indicado no caminho em modo de leitura ("r")
    if (!arquivo) { // Se o arquivo não puder ser aberto (não existe ou sem permissão)
        perror("Falha ao abrir o arquivo especificado"); // Imprime o erro detalhado no terminal
        exit(EXIT_FAILURE); // Encerra a execução do programa inteiramente com código de erro
    }

    f->clausulas = criar_vetor(); // Inicializa o vetor principal que guardará todas as cláusulas
    f->total_variaveis = 0; // Inicia o contador de variáveis em 0

    char buffer[4096]; // Cria um array de caracteres (buffer) para armazenar cada linha lida

    while (fgets(buffer, sizeof(buffer), arquivo)) { // Loop que lê o arquivo linha por linha até o final do documento
        if (buffer[0] == 'c' || buffer[0] == '\n' || buffer[0] == '\r') continue; // Pula linhas de comentário ('c') ou vazias

        if (buffer[0] == 'p') { // Verifica se a linha começa com 'p', indicando a linha de cabeçalho
            sscanf(buffer, "p cnf %d", &f->total_variaveis); 
            continue; // Pula para a próxima iteração do loop sem executar o resto do código abaixo
        } // Fim da checagem do cabeçalho

        VetorDirecional *clausula_atual = criar_vetor(); // Cria um novo vetor para guardar os literais da linha atual
        char *token = strtok(buffer, " \t\r\n"); // Separa a linha lida em pequenos pedaços (tokens) usando espaços como quebra
        
        while (token) { // Loop que roda enquanto houver números (tokens) na linha extraída
            int valor_literal = atoi(token); // Converte o token, que é texto (string), em um número inteiro (literal)
            if (valor_literal == 0) break; // O formato DIMACS usa 0 para marcar o fim da cláusula; quebra o loop se achar
            
            vetor_inserir(clausula_atual, (void*)(intptr_t)valor_literal); // Converte o int em um ponteiro e salva direto no vetor
            token = strtok(NULL, " \t\r\n"); // Continua a quebrar a mesma linha para pegar o próximo número
        } // Fim do loop que lê os literais de uma única cláusula

        if (clausula_atual->quantidade > 0) { // Verifica se a cláusula recém-criada tem pelo menos um literal válido dentro
            vetor_inserir(f->clausulas, clausula_atual); // Se sim, insere o vetor dessa cláusula dentro do vetor principal da fórmula
        } else { // Caso contrário (se a linha estava vazia antes de encontrar o 0)
            vetor_limpar(clausula_atual, NULL); // Destrói o vetor da cláusula inútil para evitar que ocupe memória à toa
        } // Fim da condicional que checa se a cláusula é válida
    } // Fim do loop principal de leitura de linhas do arquivo

    fclose(arquivo); // Fecha o arquivo lido, liberando o recurso de volta para o sistema operacional
} 

// ==========================================      
// 3) MOTOR DE INFERÊNCIA (Lógica SAT)              
// ==========================================       
int avaliar_clausula(const VetorDirecional *clausula, const int *interpretacao, int max_vars) { // Função que verifica o estado atual da cláusula
    bool depende = false; // Flag para sinalizar se ainda há alguma variável não atribuída (-1) na cláusula
    
    for (int i = 0; i < clausula->quantidade; i++) { // Loop que percorre cada literal armazenado na cláusula atual
        int literal = (int)(intptr_t)vetor_obter(clausula, i); // Recupera o valor inteiro do literal que estava "escondido" no ponteiro genérico
        int variavel = abs(literal); // Obtém o número exato da variável ignorando o sinal de negação (usa o valor absoluto)
        
        // ignora variáveis acima do limite do cabeçalho
        if (variavel > max_vars) { // Se a variável lida for maior que o total máximo reportado no cabeçalho do arquivo
            depende = true; // Trata ela como "não resolvida" por segurança, ativando a flag
            continue; // Pula as verificações abaixo e vai para o próximo literal da cláusula
        } // Fim do bloqueio que lida com variáveis fantasmas/inválidas

        int valor_atual = interpretacao[variavel]; // Acessa o array global de interpretação para ver o valor atual (0, 1 ou -1) da variável
        if (valor_atual != -1) { // Verifica se a variável já recebeu um chute fixo (já é 0 ou 1)
            if ((valor_atual == 1 && literal > 0) || (valor_atual == 0 && literal < 0)) { // Verifica se a atribuição satisfaz a lógica do literal
                return 1; // Se satisfez, como a cláusula é um OR gigante, a cláusula inteira fica verdadeira (retorna 1)
            } // Fim da checagem de satisfação lógica do literal específico
        } else { // Se a variável acessada ainda está sem valor, ou seja, tem valor -1
            depende = true; // Marca na flag que a cláusula ainda depende do futuro de uma variável em aberto
        } // Fim da checagem de estado atribuído da variável
    } // Fim do loop de iteração sobre os literais
    
    return depende ? -1 : 0; // Operador ternário: se depende de algo livre, retorna -1 (indeterminado). Senão, é falso (0)
} 

bool formula_satisfeita(const FormulaCNF *f, const int *interpretacao) { // Checa se todas as cláusulas da fórmula global são verdadeiras
    for (int i = 0; i < f->clausulas->quantidade; i++) { // Percorre todas as cláusulas guardadas no vetor principal da estrutura FormulaCNF
        VetorDirecional *c = (VetorDirecional*)vetor_obter(f->clausulas, i); // Obtém o ponteiro exato da cláusula da iteração atual
        if (avaliar_clausula(c, interpretacao, f->total_variaveis) != 1) return false; // Se a avaliação da cláusula der algo diferente de 1, retorna falso
    } // Fim do loop que varre todas as cláusulas
    return true; // Se o loop conseguiu terminar inteiro sem tropeçar no return false, a fórmula é 100% verdadeira
} 

bool formula_falsificada(const FormulaCNF *f, const int *interpretacao) { // Checa se a fórmula quebrou de vez (alguma cláusula ficou falsa e sem saída)
    for (int i = 0; i < f->clausulas->quantidade; i++) { // Percorre todas as cláusulas guardadas no vetor principal da estrutura FormulaCNF
        VetorDirecional *c = (VetorDirecional*)vetor_obter(f->clausulas, i); // Obtém o ponteiro exato da cláusula da iteração atual
        if (avaliar_clausula(c, interpretacao, f->total_variaveis) == 0) return true; // Se a avaliação retornar exatamente 0, significa contradição. Retorna verdadeiro
    } // Fim do loop que varre todas as cláusulas procurando falhas críticas
    return false; // Se não encontrou nenhuma cláusula definitivamente e irremediavelmente falsa, retorna falso
} 

bool resolver_sat(FormulaCNF *f, int *interpretacao) { // Função principal de resolução utilizando algoritmo de backtracking com recursão
    if (formula_satisfeita(f, interpretacao)) return true; // Caso Base 1: Se a interpretação atual resolveu toda a fórmula, interrompe e avisa que deu certo
    if (formula_falsificada(f, interpretacao)) return false; // Caso Base 2: Se a interpretação gerou um conflito insolúvel, corta esse ramo da árvore (poda)

    int var_alvo = -1; // Declara a variável que vai armazenar qual será a próxima variável a chutarmos um valor
    // Limite de varredura atrelado estritamente ao cabeçalho
    for (int i = 1; i <= f->total_variaveis; i++) { // Percorre todas as variáveis válidas, começando sempre do índice 1 até o limite máximo
        if (interpretacao[i] == -1) { // Verifica se a variável do índice atual ainda está em branco (valor -1)
            var_alvo = i; // Elege essa variável vazia como o alvo do próximo chute
            break; // Interrompe o loop para não continuar procurando; foca apenas no alvo atual
        } // Fim da condicional que detecta variável não atribuída
    }
    
    if (var_alvo == -1) return false; // Proteção extra: se procurou tudo e não achou alvo, mas também não estava satisfeita, é erro insolúvel

    interpretacao[var_alvo] = 1; // RAMIFICAÇÃO (BRANCH): Chuta primeiramente que a variável alvo é Verdadeira (valor 1)
    if (resolver_sat(f, interpretacao)) return true; // Chama a recursão. Se o caminho derivado deste '1' der certo no futuro, passa o "true" para cima e encerra

    interpretacao[var_alvo] = 0; // RAMIFICAÇÃO (BRANCH): O '1' deu errado. Altera o chute da variável alvo para Falsa (valor 0)
    if (resolver_sat(f, interpretacao)) return true; // Chama a recursão. Se esse novo caminho derivado do '0' der certo, passa o "true" para cima e encerra

    interpretacao[var_alvo] = -1; // BACKTRACKING: Se chegou aqui, tanto o '1' quanto o '0' geraram becos sem saída. Limpa a variável (volta pra -1)
    return false; // Retorna falso para avisar o nível de cima da recursão de que essa via inteira fracassou
} 

// ==========================================       
// 4) PONTO DE ENTRADA                              
// ==========================================       
int main(int argc, char *argv[]) { // Função por onde todo programa C começa. Recebe contador de argumentos (argc) e vetor de textos (argv)
    if (argc < 2) { // Verifica se o usuário não passou o nome do arquivo .cnf junto na execução do terminal
        printf("Modo de uso: %s <arquivo.cnf>\n", argv[0]); // Se faltou, imprime na tela um aviso de como é a forma correta de usar
        return EXIT_FAILURE; // E encerra o programa imediatamente devolvendo o código de erro do sistema
    } // Fim da verificação de integridade dos parâmetros do terminal

    FormulaCNF formula; // Declara uma variável local baseada na struct FormulaCNF para alocar a fórmula na Stack
    mapear_dimacs(argv[1], &formula); // Chama o interpretador de arquivo passando o nome recebido pelo terminal e o endereço de memória da fórmula

    int *interpretacao = (int*)malloc(sizeof(int) * (formula.total_variaveis + 1)); // Aloca na Heap o array de chutes. O +1 é porque variáveis SAT iniciam no 1 e não no 0
    for (int i = 0; i <= formula.total_variaveis; i++) { // Loop que vai da posição 0 até a última variável mapeada pelo cabeçalho
        interpretacao[i] = -1; // Preenche todo o array com -1 para garantir que todas comecem sem valor atribuído no sistema
    } // Fim da rotina de inicialização segura do array de interpretação

    if (resolver_sat(&formula, interpretacao)) { // Chama a função principal de busca recursiva passando a fórmula e os chutes. Avalia o resultado booleano
        printf("SAT!\n"); // Se a função retornou verdadeiro, achamos um caminho viável. Imprime SAT na tela
    } else { // Caso o if tenha recebido falso, significa que todas as possibilidades de ramificação falharam
        printf("UNSAT!\n"); // O problema é comprovadamente sem solução. Imprime UNSAT na tela
    } // Fim da condicional que define o desfecho do programa na interface do terminal

    free(interpretacao); // Devolve a memória do array de inteiros de interpretação para o sistema operacional (evita vazamentos)
    vetor_limpar(formula.clausulas, liberar_clausula); // Inicia a reação em cadeia de destruição de vetores, limpando as cláusulas individuais e a fórmula principal

    return EXIT_SUCCESS; // Finaliza o fluxo da main e encerra o programa com código 0, indicando que tudo terminou perfeitamente bem
}