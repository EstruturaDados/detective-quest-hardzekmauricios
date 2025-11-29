#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM_NOME_SALA   50
#define TAM_PISTA       120
#define TAM_SUSPEITO    40
#define TAM_TABELA_HASH 23   // tamanho primo simples para hash

// -------------------------------------------------------
// Struct da Sala (árvore binária da mansão)
// -------------------------------------------------------
typedef struct Sala {
    char nome[TAM_NOME_SALA];   // nome da sala
    char pista[TAM_PISTA];      // texto da pista
    char suspeito[TAM_SUSPEITO]; // nome do suspeito ligado à pista
    struct Sala *esq;
    struct Sala *dir;
} Sala;

// -------------------------------------------------------
// Struct da BST de Pistas (guarda só a pista como string)
// -------------------------------------------------------
typedef struct PistaNode {
    char pista[TAM_PISTA];
    struct PistaNode *esq;
    struct PistaNode *dir;
} PistaNode;

// -------------------------------------------------------
// Entrada da tabela hash: pista -> suspeito
// -------------------------------------------------------
typedef struct HashEntry {
    char pista[TAM_PISTA];
    char suspeito[TAM_SUSPEITO];
    int ocupado;  // 0 = vazio, 1 = ocupado
} HashEntry;

// -------------------------------------------------------
// Protótipos das funções principais
// -------------------------------------------------------
Sala* criarSala(const char *nome, const char *pista, const char *suspeito);

PistaNode* inserirPista(PistaNode *raiz, const char *pista);
void exibirPistas(PistaNode *raiz);

void inicializarHash(HashEntry tabela[]);
unsigned int hashFunc(const char *chave);
void inserirNaHash(HashEntry tabela[], const char *pista, const char *suspeito);
const char* encontrarSuspeito(HashEntry tabela[], const char *pista);

void explorarSalas(Sala *raiz, PistaNode **raizPistas, HashEntry tabelaHash[]);
void verificarSuspeitoFinal(PistaNode *raizPistas, HashEntry tabelaHash[]);

void liberarArvoreSalas(Sala *raiz);
void liberarArvorePistas(PistaNode *raiz);

// -------------------------------------------------------
// Função: criarSala
// Cria dinamicamente um cômodo com pista e suspeito associados
// -------------------------------------------------------
Sala* criarSala(const char *nome, const char *pista, const char *suspeito) {
    Sala *nova = (Sala *)malloc(sizeof(Sala));
    if (nova == NULL) {
        printf("Erro ao alocar memoria para a sala '%s'.\n", nome);
        exit(1);
    }

    strncpy(nova->nome, nome, TAM_NOME_SALA);
    nova->nome[TAM_NOME_SALA - 1] = '\0';

    if (pista != NULL) {
        strncpy(nova->pista, pista, TAM_PISTA);
        nova->pista[TAM_PISTA - 1] = '\0';
    } else {
        nova->pista[0] = '\0';
    }

    if (suspeito != NULL) {
        strncpy(nova->suspeito, suspeito, TAM_SUSPEITO);
        nova->suspeito[TAM_SUSPEITO - 1] = '\0';
    } else {
        nova->suspeito[0] = '\0';
    }

    nova->esq = NULL;
    nova->dir = NULL;

    return nova;
}

// -------------------------------------------------------
// Função: criarNoPista (auxiliar interna)
// Cria nó da BST de pistas
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
// Insere pista na BST em ordem alfabética
// -------------------------------------------------------
PistaNode* inserirPista(PistaNode *raiz, const char *pista) {
    if (pista == NULL || pista[0] == '\0') {
        return raiz; // pista vazia, nada a inserir
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
        // cmp == 0: pista já existente; podemos ignorar duplicata
    }

    return raiz;
}

// -------------------------------------------------------
// Função: exibirPistas
// Percorre a BST em ordem e mostra todas as pistas
// -------------------------------------------------------
void exibirPistas(PistaNode *raiz) {
    if (raiz == NULL) return;

    exibirPistas(raiz->esq);
    printf("- %s\n", raiz->pista);
    exibirPistas(raiz->dir);
}

// -------------------------------------------------------
// Funções da Tabela Hash (pista -> suspeito)
// -------------------------------------------------------
void inicializarHash(HashEntry tabela[]) {
    for (int i = 0; i < TAM_TABELA_HASH; i++) {
        tabela[i].ocupado = 0;
        tabela[i].pista[0] = '\0';
        tabela[i].suspeito[0] = '\0';
    }
}

// Hash simples (soma de caracteres % TAM_TABELA_HASH)
unsigned int hashFunc(const char *chave) {
    unsigned int h = 0;
    while (*chave) {
        h = (h * 31) + (unsigned char)(*chave);
        chave++;
    }
    return h % TAM_TABELA_HASH;
}

// -------------------------------------------------------
// Função: inserirNaHash
// Insere associação pista -> suspeito na tabela hash
// (usa endereçamento aberto linear simples)
// -------------------------------------------------------
void inserirNaHash(HashEntry tabela[], const char *pista, const char *suspeito) {
    if (pista == NULL || pista[0] == '\0') return;

    unsigned int idx = hashFunc(pista);
    unsigned int start = idx;

    while (tabela[idx].ocupado) {
        // Se já existir a mesma pista, apenas atualiza suspeito (opcional)
        if (strcmp(tabela[idx].pista, pista) == 0) {
            strncpy(tabela[idx].suspeito, suspeito, TAM_SUSPEITO);
            tabela[idx].suspeito[TAM_SUSPEITO - 1] = '\0';
            return;
        }
        idx = (idx + 1) % TAM_TABELA_HASH;
        if (idx == start) {
            // tabela cheia (não deve acontecer neste exercício)
            printf("Tabela hash cheia! Nao foi possivel inserir nova pista.\n");
            return;
        }
    }

    tabela[idx].ocupado = 1;
    strncpy(tabela[idx].pista, pista, TAM_PISTA);
    tabela[idx].pista[TAM_PISTA - 1] = '\0';
    strncpy(tabela[idx].suspeito, suspeito, TAM_SUSPEITO);
    tabela[idx].suspeito[TAM_SUSPEITO - 1] = '\0';
}

// -------------------------------------------------------
// Função: encontrarSuspeito
// Retorna o suspeito associado a uma pista (ou NULL se não achar)
// -------------------------------------------------------
const char* encontrarSuspeito(HashEntry tabela[], const char *pista) {
    if (pista == NULL || pista[0] == '\0') return NULL;

    unsigned int idx = hashFunc(pista);
    unsigned int start = idx;

    while (tabela[idx].ocupado) {
        if (strcmp(tabela[idx].pista, pista) == 0) {
            return tabela[idx].suspeito;
        }
        idx = (idx + 1) % TAM_TABELA_HASH;
        if (idx == start) {
            break; // percorreu tudo
        }
    }
    return NULL; // não encontrou
}

// -------------------------------------------------------
// Função: explorarSalas
// Navega pela árvore da mansão, mostra pistas e
// armazena-as na BST e na hash (pista -> suspeito)
// -------------------------------------------------------
void explorarSalas(Sala *raiz, PistaNode **raizPistas, HashEntry tabelaHash[]) {
    Sala *atual = raiz;
    char opcao;

    if (atual == NULL) {
        printf("Nao ha salas na mansao.\n");
        return;
    }

    printf("===== Detective Quest - Nivel Mestre =====\n");
    printf("Voce ira explorar a mansao, coletar pistas e relaciona-las a suspeitos.\n");
    printf("Use 'e' para ir a esquerda, 'd' para a direita e 's' para encerrar.\n");

    while (1) {
        printf("\nVoce esta em: %s\n", atual->nome);

        // Mostrar pista e associar ao suspeito via hash
        if (atual->pista[0] != '\0') {
            printf("Pista neste comodo: \"%s\"\n", atual->pista);
            if (atual->suspeito[0] != '\0') {
                printf("Esta pista parece apontar para: %s\n", atual->suspeito);
            } else {
                printf("Esta pista nao aponta claramente para um suspeito.\n");
            }

            // Inserir na BST de pistas
            *raizPistas = inserirPista(*raizPistas, atual->pista);

            // Inserir na hash: pista -> suspeito (se existir suspeito)
            if (atual->suspeito[0] != '\0') {
                inserirNaHash(tabelaHash, atual->pista, atual->suspeito);
            }
        } else {
            printf("Nao ha pistas visiveis neste comodo.\n");
        }

        // Opções de navegação
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
// Função auxiliar recursiva: conta pistas por suspeito
// Percorre a BST de pistas e usa a hash para ver a quem
// cada pista aponta, somando contagens
// -------------------------------------------------------
void contarPistasPorSuspeitoRec(PistaNode *raiz, HashEntry tabelaHash[],
                                const char *suspeitoAlvo, int *contador) {
    if (raiz == NULL) return;

    contarPistasPorSuspeitoRec(raiz->esq, tabelaHash, suspeitoAlvo, contador);

    const char *sus = encontrarSuspeito(tabelaHash, raiz->pista);
    if (sus != NULL && strcmp(sus, suspeitoAlvo) == 0) {
        (*contador)++;
    }

    contarPistasPorSuspeitoRec(raiz->dir, tabelaHash, suspeitoAlvo, contador);
}

// -------------------------------------------------------
// Função: verificarSuspeitoFinal
// Pede ao jogador o nome do suspeito acusado e verifica
// quantas pistas na BST apontam para ele via hash
// -------------------------------------------------------
void verificarSuspeitoFinal(PistaNode *raizPistas, HashEntry tabelaHash[]) {
    char acusacao[TAM_SUSPEITO];
    int contador = 0;

    printf("\n===== Fase Final: Julgamento =====\n");

    if (raizPistas == NULL) {
        printf("Nenhuma pista foi coletada. Nao ha base para acusar ninguem.\n");
        return;
    }

    printf("Pistas coletadas (em ordem alfabetica):\n");
    exibirPistas(raizPistas);

    printf("\nDigite o nome do suspeito que voce deseja acusar: ");
    // ler até a quebra de linha; primeiro consome '\n' pendente
    getchar();
    fgets(acusacao, TAM_SUSPEITO, stdin);
    // remover '\n'
    size_t len = strlen(acusacao);
    if (len > 0 && acusacao[len - 1] == '\n') {
        acusacao[len - 1] = '\0';
    }

    if (acusacao[0] == '\0') {
        printf("Nome de suspeito vazio. Encerrando julgamento.\n");
        return;
    }

    // Conta quantas pistas da BST apontam para esse suspeito
    contarPistasPorSuspeitoRec(raizPistas, tabelaHash, acusacao, &contador);

    printf("\nTotal de pistas que apontam para '%s': %d\n", acusacao, contador);

    if (contador >= 2) {
        printf("Veredito: ACUSACAO SUSTENTADA! Ha evidencias suficientes contra %s.\n",
               acusacao);
    } else if (contador == 1) {
        printf("Veredito: DUVIDOSO. Apenas 1 pista aponta para %s. Investigacao inconclusiva.\n",
               acusacao);
    } else {
        printf("Veredito: INOCENTE (por falta de provas). Nenhuma pista aponta claramente para %s.\n",
               acusacao);
    }
}

// -------------------------------------------------------
// Liberação de memória
// -------------------------------------------------------
void liberarArvoreSalas(Sala *raiz) {
    if (raiz == NULL) return;
    liberarArvoreSalas(raiz->esq);
    liberarArvoreSalas(raiz->dir);
    free(raiz);
}

void liberarArvorePistas(PistaNode *raiz) {
    if (raiz == NULL) return;
    liberarArvorePistas(raiz->esq);
    liberarArvorePistas(raiz->dir);
    free(raiz);
}

// -------------------------------------------------------
// main - monta a mansão, executa a exploração e o julgamento
// -------------------------------------------------------
int main() {
    // Suspeitos: você pode mudar os nomes se quiser
    const char *MORDOMO    = "Mordomo";
    const char *COZINHEIRA = "Cozinheira";
    const char *JARDINEIRO = "Jardineiro";

    // Montagem manual da mansão
    Sala *hallEntrada = criarSala(
        "Hall de Entrada",
        "Luvas manchadas deixadas perto do cabideiro",
        MORDOMO
    );

    Sala *salaEstar = criarSala(
        "Sala de Estar",
        "Pegadas de sapato engraxado no tapete caro",
        MORDOMO
    );

    Sala *cozinha = criarSala(
        "Cozinha",
        "Faca suja escondida atras da pia",
        COZINHEIRA
    );

    Sala *biblioteca = criarSala(
        "Biblioteca",
        "Livro de receitas com paginas rasgadas",
        COZINHEIRA
    );

    Sala *salaMusica = criarSala(
        "Sala de Musica",
        "Partitura com anotacoes sobre o horario do crime",
        MORDOMO
    );

    Sala *despensa = criarSala(
        "Despensa",
        "Caixa de ferramentas aberta e poeira remexida",
        JARDINEIRO
    );

    Sala *jardim = criarSala(
        "Jardim",
        "Pegadas na terra molhada perto da estufa",
        JARDINEIRO
    );

    // Ligações da árvore da mansão
    hallEntrada->esq = salaEstar;
    hallEntrada->dir = cozinha;

    salaEstar->esq  = biblioteca;
    salaEstar->dir  = salaMusica;

    cozinha->esq    = despensa;
    cozinha->dir    = jardim;

    // Raiz da BST de pistas
    PistaNode *raizPistas = NULL;

    // Tabela hash para associar pista -> suspeito
    HashEntry tabelaHash[TAM_TABELA_HASH];
    inicializarHash(tabelaHash);

    // Exploração da mansão com coleta de pistas e hash
    explorarSalas(hallEntrada, &raizPistas, tabelaHash);

    // Julgamento final
    verificarSuspeitoFinal(raizPistas, tabelaHash);

    // Liberação de memória
    liberarArvoreSalas(hallEntrada);
    liberarArvorePistas(raizPistas);

    return 0;
}
