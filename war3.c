#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---------- Structs ---------- */
typedef struct {
    char nome[30];   // dono do territorio
    char cor[10];    // cor do exercito
    int tropas;      // quantidade de tropas
} Territorio;

typedef struct {
    char nome[30];   // nome do dono
    char* missao;    // missão atribuída
} Dono;

/* ---------- Prototipos ---------- */
Territorio* alocarMapa(int n);
void cadastrarTerritorios(Territorio* mapa, int n);
void exibirTerritorios(const Territorio* mapa, int n);
void atacar(Territorio* atacante, Territorio* defensor);
void liberarMemoria(Territorio* mapa);
int lerInteiro(const char* mensagem, int min, int max);

void atribuirMissao(char* destino, char* missoes[], int totalMissoes);
int contarTerritoriosDoNome(const char* nome, const Territorio* mapa, int tamanho);
int verificarTerritoriosConsecutivos(const char* nome, const Territorio* mapa, int tamanho, int quantidade);
int verificarDominioTotal(const char* nome, const Territorio* mapa, int tamanho, int minTerritorios);
int verificarMissao(const char* missao, const char* nomeDono, const Territorio* mapa, int tamanho);

int gerarDonos(Territorio* mapa, int total, Dono** donos);
int verificarMissaoDono(const Dono* dono, const Territorio* mapa, int tamanho);
void exibirTodasMissoes(const Dono* donos, int totalDonos);

/* ---------- Main ---------- */
int main(void) {
    srand((unsigned int)time(NULL));

    printf("=== SIMULACAO DE GUERRA TERRITORIAL ===\n\n");

    int total = lerInteiro("Informe o numero de territorios a cadastrar (min 4): ", 4, 1000);

    Territorio* mapa = alocarMapa(total);
    if (!mapa) {
        fprintf(stderr, "Erro: falha na alocacao de memoria.\n");
        return 1;
    }

    /* Missoes pre-definidas com limite de 3 territorios */
    char* missoesPossiveis[] = {
        "Conquistar 3 territorios",
        "Conquistar 3 territorios consecutivos (indices seguidos)",
        "Eliminar completamente 1 cor inimiga do mapa",
        "Dominar 3 territorios no total"
    };
    int totalMissoes = 4;

    cadastrarTerritorios(mapa, total);

    Dono* donos = NULL;
    int totalDonos = gerarDonos(mapa, total, &donos);
    if (totalDonos == 0) {
        fprintf(stderr, "Erro: falha ao criar donos.\n");
        liberarMemoria(mapa);
        return 1;
    }

    printf("\n=== ATRIBUICAO DE MISSOES AOS DONOS ===\n");
    for (int i = 0; i < totalDonos; i++) {
        atribuirMissao(donos[i].missao, missoesPossiveis, totalMissoes);
        printf("Missao do %s: %s\n", donos[i].nome, donos[i].missao);
    }

    int opcao;
    int jogoAtivo = 1;
    do {
        printf("\n=== MENU PRINCIPAL ===\n");
        printf("1 - Exibir territorios\n");
        printf("2 - Realizar ataque\n");
        printf("3 - Exibir missoes dos donos\n");
        printf("0 - Sair\n");
        opcao = lerInteiro("Escolha uma opcao: ", 0, 3);

        if (opcao == 1) {
            exibirTerritorios(mapa, total);

        } else if (opcao == 2) {
            exibirTerritorios(mapa, total);
            int idxAtacante = lerInteiro("Indice do territorio atacante: ", 1, total) - 1;
            int idxDefensor = lerInteiro("Indice do territorio defensor: ", 1, total) - 1;

            if (idxAtacante == idxDefensor) {
                printf("Erro: atacante e defensor nao podem ser o mesmo.\n");
            } else if (strcmp(mapa[idxAtacante].cor, mapa[idxDefensor].cor) == 0) {
                printf("Erro: nao e permitido atacar territorio da mesma cor.\n");
            } else if (mapa[idxAtacante].tropas <= 0) {
                printf("Erro: o territorio atacante nao possui tropas suficientes.\n");
            } else {
                atacar(&mapa[idxAtacante], &mapa[idxDefensor]);

                for (int i = 0; i < totalDonos; i++) {
                    if (verificarMissaoDono(&donos[i], mapa, total)) {
                        printf("\n*** VITORIA! ***\n");
                        printf("O dono %s completou sua missao!\n", donos[i].nome);
                        printf("Missao: %s\n", donos[i].missao);
                        jogoAtivo = 0;
                        opcao = 0;
                        break;
                    }
                }
            }

        } else if (opcao == 3) {
            exibirTodasMissoes(donos, totalDonos);
        }

    } while (opcao != 0 && jogoAtivo);

    for (int i = 0; i < totalDonos; i++) {
        free(donos[i].missao);
    }
    free(donos);
    liberarMemoria(mapa);

    printf("\nMemoria liberada. Programa finalizado.\n");
    return 0;
}

/* ---------- Implementacao ---------- */

Territorio* alocarMapa(int n) {
    return (Territorio*) calloc(n, sizeof(Territorio));
}

void cadastrarTerritorios(Territorio* mapa, int n) {
    printf("\n=== CADASTRO DOS TERRITORIOS ===\n");
    for (int i = 0; i < n; i++) {
        printf("\nTerritorio %d\n", i + 1);
        printf("Digite o nome do dono do territorio: ");
        scanf(" %[^\n]", mapa[i].nome);
        printf("Digite a cor do exercito: ");
        scanf(" %[^\n]", mapa[i].cor);
        mapa[i].tropas = lerInteiro("Digite a quantidade de tropas (>= 0): ", 0, 1000000);
    }
}

void exibirTerritorios(const Territorio* mapa, int n) {
    printf("\n--- MAPA DE TERRITORIOS ---\n");
    for (int i = 0; i < n; i++) {
        printf("[%2d] Dono: %-20s | Cor: %-8s | Tropas: %d\n",
               i + 1, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("---------------------------\n");
}

void atacar(Territorio* atacante, Territorio* defensor) {
    printf("\n=== ATAQUE INICIADO ===\n");
    printf("Atacante: %s (Cor: %s, Tropas: %d)\n", atacante->nome, atacante->cor, atacante->tropas);
    printf("Defensor: %s (Cor: %s, Tropas: %d)\n", defensor->nome, defensor->cor, defensor->tropas);

    int dadoAtacante = (rand() % 6) + 1;
    int dadoDefensor = (rand() % 6) + 1;
    printf("Rolagem -> Atacante: %d | Defensor: %d\n", dadoAtacante, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        int transferidas = atacante->tropas / 2;
        if (transferidas < 1) transferidas = (atacante->tropas > 0) ? 1 : 0;

        // Dono e cor passam a ser do atacante
        strcpy(defensor->nome, atacante->nome);
        strcpy(defensor->cor, atacante->cor);

        defensor->tropas = transferidas;
        atacante->tropas -= transferidas;

        printf("Resultado: Atacante VENCEU! Territorio conquistado.\n");
        printf("%d tropas transferidas.\n", transferidas);
    } else {
        atacante->tropas -= 1;
        if (atacante->tropas < 0) atacante->tropas = 0;
        printf("Resultado: Atacante perdeu 1 tropa.\n");
    }

    printf("Estado apos o combate:\n");
    printf("Atacante: %s | Tropas: %d\n", atacante->nome, atacante->tropas);
    printf("Defensor: %s | Tropas: %d\n", defensor->nome, defensor->tropas);
}

void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

int lerInteiro(const char* mensagem, int min, int max) {
    int valor, result;
    do {
        printf("%s", mensagem);
        result = scanf("%d", &valor);
        if (result != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            printf("Entrada invalida.\n");
            continue;
        }
        if (valor < min || valor > max) {
            printf("Valor fora da faixa (%d a %d).\n", min, max);
        } else break;
    } while (1);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    return valor;
}

void atribuirMissao(char* destino, char* missoes[], int totalMissoes) {
    int indice = rand() % totalMissoes;
    strcpy(destino, missoes[indice]);
}

int contarTerritoriosDoNome(const char* nome, const Territorio* mapa, int tamanho) {
    int count = 0;
    for (int i = 0; i < tamanho; i++)
        if (strcmp(mapa[i].nome, nome) == 0) count++;
    return count;
}

int verificarTerritoriosConsecutivos(const char* nome, const Territorio* mapa, int tamanho, int quantidade) {
    int consecutivos = 0;
    for (int i = 0; i < tamanho; i++) {
        if (strcmp(mapa[i].nome, nome) == 0) consecutivos++;
        else consecutivos = 0;
        if (consecutivos >= quantidade) return 1;
    }
    return 0;
}

int verificarDominioTotal(const char* nome, const Territorio* mapa, int tamanho, int minTerritorios) {
    int count = contarTerritoriosDoNome(nome, mapa, tamanho);
    return count >= minTerritorios;
}

int verificarMissao(const char* missao, const char* nomeDono, const Territorio* mapa, int tamanho) {
    if (strstr(missao, "Conquistar 3 territorios") != NULL)
        return contarTerritoriosDoNome(nomeDono, mapa, tamanho) >= 3;

    if (strstr(missao, "consecutivos") != NULL)
        return verificarTerritoriosConsecutivos(nomeDono, mapa, tamanho, 3);

    if (strstr(missao, "Eliminar") != NULL)
        return contarTerritoriosDoNome(nomeDono, mapa, tamanho) > (tamanho / 2);

    if (strstr(missao, "Dominar 3 territorios") != NULL)
        return verificarDominioTotal(nomeDono, mapa, tamanho, 3);

    return 0;
}

int gerarDonos(Territorio* mapa, int total, Dono** donos) {
    int totalDonos = 0;
    *donos = (Dono*) malloc(total * sizeof(Dono));
    if (!*donos) return 0;

    for (int i = 0; i < total; i++) {
        int existe = 0;
        for (int j = 0; j < totalDonos; j++)
            if (strcmp(mapa[i].nome, (*donos)[j].nome) == 0) existe = 1;

        if (!existe) {
            strcpy((*donos)[totalDonos].nome, mapa[i].nome);
            (*donos)[totalDonos].missao = (char*) malloc(100 * sizeof(char));
            totalDonos++;
        }
    }
    return totalDonos;
}

int verificarMissaoDono(const Dono* dono, const Territorio* mapa, int tamanho) {
    return verificarMissao(dono->missao, dono->nome, mapa, tamanho);
}

void exibirTodasMissoes(const Dono* donos, int totalDonos) {
    printf("\n=== MISSOES POR DONO DE TERRITORIO ===\n");
    for (int i = 0; i < totalDonos; i++) {
        printf("Missao do %s: %s\n", donos[i].nome, donos[i].missao);
    }
    printf("=====================================\n");
}
