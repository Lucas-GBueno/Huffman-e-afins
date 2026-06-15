#include "utils.h"
#include "huffman.h"

int main() {
    int escolha;
    char caminho[512];

    do {
        escolha = mensagem_inicial();
        if (escolha == 0) {
            break;
        } else if (escolha != 1 && escolha != 2) {
            printf("\nOpção inválida! Tente novamente.\n\n");
            continue;
        }

        printf("Digite o caminho do arquivo: ");
        // Defesa: Usamos "%255s" no scanf em vez de "%s" puro para evitar Buffer Overflow.
        // Isso impede o usuário de digitar um caminho maior que o buffer de 'caminho' suporta.
        scanf("%255s", caminho); 

        if (escolha == 1) {
            comprimir(caminho);
        } else if (escolha == 2) {
            descomprimir(caminho);
        }
        
        printf("\n-----------------------------------\n");

    } while (escolha != 0);

    return 0;
}