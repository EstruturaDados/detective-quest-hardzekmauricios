#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME 50

// -------------------------------------------------------
// Struct Sala - nó da árvore binária
// -------------------------------------------------------
typedef struct Sala {
    char nome[TAM_NOME];     // Nome do cômodo
    struct Sala *esq;        // Ponteiro para a sala à esquerda
    struct Sala *dir;        // Ponteiro para a sala à direita
} Sala;

// -------------------------------------------------------
// Função: criarSala
// Cria dinamicamente uma sala com o nome informado
// -------------------------------------------------------
Sala* criarSala(const char *nome) {
    Sala *nova = (Sala *)malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memoria para a sala '%s'.\n", nome);
        exit(1);
    }

    strncpy(nova->nome, nome, TAM_NOME);
    nova->nome[TAM_NOME - 1] = '\0';  // garantir fim de string
    nova->esq = NULL;
    nova->dir = NULL;

    return nova;
}

// -------------------------------------------------------
// Função: explorarSalas
// Permite que o jogador navegue pela mansão a partir
// da sala atual, escolhendo ir para esquerda (e),
// direita (d) ou sair (s).
// A exploração termina se o jogador chegar a um nó-folha.
// -------------------------------------------------------
void explorarSalas(Sala *raiz) {
    Sala *atual = raiz;
    char opcao;

    if (atual == NULL) {
        printf("Nao ha salas na mansao.\n");
        return;
    }

    printf("===== Detective Quest - Exploracao da Mansao =====\n");

    while (1) {
        // Exibe sala atual
        printf("\nVoce esta em: %s\n", atual->nome);

        // Verifica se é nó-folha (sem caminhos)
        if (atual->esq == NULL && atual->dir == NULL) {
            printf("Voce chegou a um comodo sem mais caminhos.\n");
            printf("Exploracao encerrada.\n");
            break;
        }

        // Mostra opções de caminho disponíveis
        printf("Caminhos disponiveis:\n");
        if (atual->esq != NULL)
            printf("  [e] Esquerda -> %s\n", atual->esq->nome);
        if (atual->dir != NULL)
            printf("  [d] Direita  -> %s\n", atual->dir->nome);
        printf("  [s] Sair da exploracao\n");

        printf("Escolha (e/d/s): ");
        scanf(" %c", &opcao);  // espaço antes de %c para ignorar \n

        if (opcao == 's' || opcao == 'S') {
            printf("\nVoce decidiu encerrar a exploracao.\n");
            break;
        } else if ((opcao == 'e' || opcao == 'E') && atual->esq != NULL) {
            atual = atual->esq;
        } else if ((opcao == 'd' || opcao == 'D') && atual->dir != NULL) {
            atual = atual->dir;
        } else {
            printf("Opcao invalida ou caminho inexistente. Tente novamente.\n");
        }
    }
}

// -------------------------------------------------------
// Função auxiliar: liberarArvore
// Libera toda a memória da árvore binária (pós-ordem).
// -------------------------------------------------------
void liberarArvore(Sala *raiz) {
    if (raiz == NULL) return;
    liberarArvore(raiz->esq);
    liberarArvore(raiz->dir);
    free(raiz);
}

// -------------------------------------------------------
// Função: main
// Monta a árvore da mansão manualmente e inicia a exploração
// -------------------------------------------------------
int main() {
    // Criação manual da estrutura da mansão (árvore binária)
    // Raiz
    Sala *hallEntrada   = criarSala("Hall de Entrada");

    // Nível 1
    Sala *salaEstar     = criarSala("Sala de Estar");
    Sala *cozinha       = criarSala("Cozinha");

    hallEntrada->esq = salaEstar;
    hallEntrada->dir = cozinha;

    // Nível 2 (filhos da Sala de Estar)
    Sala *biblioteca    = criarSala("Biblioteca");
    Sala *salaMusica    = criarSala("Sala de Musica");

    salaEstar->esq = biblioteca;
    salaEstar->dir = salaMusica;

    // Nível 2 (filhos da Cozinha)
    Sala *despensa      = criarSala("Despensa");
    Sala *jardim        = criarSala("Jardim");

    cozinha->esq = despensa;
    cozinha->dir = jardim;

    // Inicia a exploração a partir do Hall de Entrada
    explorarSalas(hallEntrada);

    // Libera memória ao final
    liberarArvore(hallEntrada);

    return 0;
}
