#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME  50
#define TAM_PISTA 100

// -------------------------------------------------------
// Struct da Sala (nó da árvore binária de cômodos)
// -------------------------------------------------------
typedef struct Sala {
    char nome[TAM_NOME];          // Nome da sala
    char pista[TAM_PISTA];        // Pista associada (pode ser vazia)
    struct Sala *esq;             // Caminho à esquerda
    struct Sala *dir;             // Caminho à direita
} Sala;

// -------------------------------------------------------
// Struct da BST de Pistas
// Cada nó guarda uma string de pista
// -------------------------------------------------------
typedef struct PistaNode {
    char pista[TAM_PISTA];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// -------------------------------------------------------
// Função: criarSala
// Cria dinamicamente uma sala com nome e pista (opcional)
// -------------------------------------------------------
Sala* criarSala(const char *nome, const char *pista) {
    Sala *nova = (Sala *)malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memoria para a sala '%s'.\n", nome);
        exit(1);
    }

    strncpy(nova->nome, nome, TAM_NOME);
    nova->nome[TAM_NOME - 1] = '\0';

    if (pista != NULL) {
        strncpy(nova->pista, pista, TAM_PISTA);
        nova->pista[TAM_PISTA - 1] = '\0';
    } else {
        nova->pista[0] = '\0';   // pista vazia
    }

    nova->esq = NULL;
    nova->dir = NULL;

    return nova;
}

// -------------------------------------------------------
// Função: criarNoPista
// Cria dinamicamente um nó de pista para a BST
// -------------------------------------------------------
PistaNode* criarNoPista(const char *pista) {
    PistaNode *novo = (PistaNode *)malloc(sizeof(PistaNode));
    if (novo == NULL) {
        printf("Erro ao alocar memoria para no de pista.\n");
        exit(1);
    }

    strncpy(novo->pista, pista, TAM_PISTA);
    novo->pista[TAM_PISTA - 1] = '\0';
    novo->esq = NULL;
    novo->dir = NULL;

    return novo;
}

// -------------------------------------------------------
// Função: inserirPista
// Insere uma nova pista na BST de forma ordenada (alfabética)
// -------------------------------------------------------
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (pista == NULL || pista[0] == '\0') {
        // Pista vazia, nada a inserir
        return raiz;
    }

    if (raiz == NULL) {
        return criarNoPista(pista);
    }

    int cmp = strcmp(pista, raiz->pista);

    if (cmp < 0) {
        raiz->esq = inserirPista(raiz->esq, pista);
    } else if (cmp > 0) {
        raiz->dir = inserirPista(raiz->dir, pista);
    } else {
        // cmp == 0: pista igual já existe, não inserir duplicata
        // (poderia contar repetições, se quisesse)
    }

    return raiz;
}

// -------------------------------------------------------
// Função: exibirPistas
// Percorre a BST em ordem (in-order) e exibe as pistas
// -------------------------------------------------------
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;

    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// -------------------------------------------------------
// Função: explorarSalasComPistas
// Permite navegação pela mansão e coleta automática de pistas
// -------------------------------------------------------
void explorarSalasComPistas(Sala *raiz, PistaNode **raizPistas) {
    Sala *atual = raiz;
    char opcao;

    if (atual == NULL) {
        printf("Nao ha salas na mansao.\n");
        return;
    }

    printf("===== Detective Quest - Coleta de Pistas (Nivel Aventureiro) =====\n");
    printf("Voce iniciara a exploracao a partir do Hall de Entrada.\n");
    printf("Digite 'e' para ir a esquerda, 'd' para a direita e 's' para encerrar.\n");

    while (1) {
        printf("\nVoce esta em: %s\n", atual->nome);

        // Se houver pista, coletar (inserir na BST)
        if (atual->pista[0] != '\0') {
            printf("Pista encontrada aqui: \"%s\"\n", atual->pista);
            *raizPistas = inserirPista(*raizPistas, atual->pista);
        } else {
            printf("Nao ha pistas visiveis neste comodo.\n");
        }

        // Mostrar caminhos disponíveis
        printf("Caminhos disponiveis:\n");
        if (atual->esq != NULL)
            printf("  [e] Esquerda -> %s\n", atual->esq->nome);
        if (atual->dir != NULL)
            printf("  [d] Direita  -> %s\n", atual->dir->nome);
        printf("  [s] Sair da exploracao\n");

        printf("Escolha (e/d/s): ");
        scanf(" %c", &opcao);

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
// Função auxiliar: liberarArvoreSalas
// Libera memória da árvore de cômodos
// -------------------------------------------------------
void liberarArvoreSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarArvoreSalas(raiz->esq);
    liberarArvoreSalas(raiz->dir);
    free(raiz);
}

// -------------------------------------------------------
// Função auxiliar: liberarArvorePistas
// Libera memória da BST de pistas
// -------------------------------------------------------
void liberarArvorePistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarArvorePistas(raiz->esq);
    liberarArvorePistas(raiz->dir);
    free(raiz);
}

// -------------------------------------------------------
// Função main - monta a mansão e inicia exploração com pistas
// -------------------------------------------------------
int main() {
    // Montagem manual da mansão (mesma estrutura do nível novato)
    Sala *hallEntrada = criarSala("Hall de Entrada",
                                  "Carta rasgada perto da porta");
    Sala *salaEstar   = criarSala("Sala de Estar",
                                  "Pegadas no tapete");
    Sala *cozinha     = criarSala("Cozinha",
                                  "Faca suja na pia");
    Sala *biblioteca  = criarSala("Biblioteca",
                                  "Livro fora do lugar");
    Sala *salaMusica  = criarSala("Sala de Musica",
                                  "Partitura com anotacoes estranhas");
    Sala *despensa    = criarSala("Despensa",
                                  "Prateleira deslocada");
    Sala *jardim      = criarSala("Jardim",
                                  "Terra remexida perto da fonte");

    // Ligações da árvore
    hallEntrada->esq = salaEstar;
    hallEntrada->dir = cozinha;

    salaEstar->esq  = biblioteca;
    salaEstar->dir  = salaMusica;

    cozinha->esq    = despensa;
    cozinha->dir    = jardim;

    // Raiz da BST de pistas (inicialmente vazia)
    PistaNode *raizPistas = NULL;

    // Exploração com coleta de pistas
    explorarSalasComPistas(hallEntrada, &raizPistas);

    // Exibe pistas coletadas em ordem alfabética
    printf("\n===== Pistas coletadas (em ordem alfabetica) =====\n");
    if (raizPistas == NULL) {
        printf("Nenhuma pista foi coletada.\n");
    } else {
        exibirPistas(raizPistas);
    }

    // Libera memória
    liberarArvoreSalas(hallEntrada);
    liberarArvorePistas(raizPistas);

    return 0;
}
