#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep
#include <time.h> // to include time function
#include <conio.h> // to include getch
#include <windows.h> // forçar tamanho da janela

// Key code
enum {
    KEY_ESC = 27,
    ARROW_UP = 256 + 72,
    ARROW_DOWN = 256 + 80,
    ARROW_LEFT = 256 + 75,
    ARROW_RIGHT = 256 + 77
};

// Estrutura para representar um segmento da cobra
typedef struct Segmento {
    int x, y; // Coordenadas do segmento
    struct Segmento* proximo; // Próximo segmento da cobra
} Segmento;

// Função para criar um novo segmento
Segmento* criarSegmento(int x, int y) {
    Segmento* novo = (Segmento*)malloc(sizeof(Segmento));
    novo->x = x;
    novo->y = y;
    novo->proximo = NULL;
    return novo;
}

// Função para adicionar um segmento no início da lista (cabeça da cobra)
void adicionarCabeca(Segmento** cabeca, int x, int y) {
    Segmento* novo = criarSegmento(x, y);
    novo->proximo = *cabeca;
    *cabeca = novo;
}

// Função para remover o último segmento da lista (cauda da cobra)
void removerCauda(Segmento** cabeca) {
    if (*cabeca == NULL) return; // Lista vazia

    Segmento* atual = *cabeca;
    Segmento* anterior = NULL;

    // Percorre até o último segmento
    while (atual->proximo != NULL) {
        anterior = atual;
        atual = atual->proximo;
    }

    // Remove o último segmento
    if (anterior != NULL) {
        anterior->proximo = NULL;
    } else {
        *cabeca = NULL; // Se havia apenas um segmento
    }
    free(atual);
}

// Função para liberar toda a lista (usada ao final do jogo)
void liberarLista(Segmento* cabeca) {
    Segmento* atual = cabeca;
    while (atual != NULL) {
        Segmento* proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
}

// Função para mover o cursor para uma posição específica na tela
void irColunaLinha(int coluna, int linha) {
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), (COORD){coluna, linha});
}

// Função para desenhar os limites da matriz do jogo
void mostrarLimitesMatriz(void) {
    system("cls");
    for (int i = 0; i < 12; ++i) {
        for (int j = 0; j < 39; ++j) {
            if ((j == 0) || (i == 0) || (i == 11) || (j == 38)) {
                printf("+");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

// Função para mostrar a tela de Game Over
void mostrarGameOver(int pontos) {
    Sleep(600);
    mostrarLimitesMatriz();
    irColunaLinha(4, 4);
    printf("==============================");
    irColunaLinha(10, 5);
    printf("G A M E  O V E R");
    irColunaLinha(4, 6);
    printf("==============================");
    getch();
    system("cls");
}

// Função principal do jogo
void jogarCobrinha() {
    Segmento* cobra = NULL; // Lista ligada representando a cobra
    int PontoPosicaoX, PontoPosicaoY; // Posição do ponto
    int tamanhoCobra = 1; // Tamanho inicial da cobra
    int pontos = 0; // Pontuação do jogador
    int ch; // Código da tecla pressionada
    int velocidade = 300; // Velocidade do jogo

    mostrarLimitesMatriz();

    // Gerar a posição do primeiro ponto
    PontoPosicaoX = (rand() % 37) + 1;
    PontoPosicaoY = (rand() % 10) + 1;

    // Gerar a posição inicial da cobra
    int inicioX = (rand() % 37) + 1;
    int inicioY = (rand() % 10) + 1;
    adicionarCabeca(&cobra, inicioX, inicioY);

    irColunaLinha(cobra->x, cobra->y);
    printf("%c", 'H');

    ch = getch(); // Aguardar uma tecla para iniciar
    while (ch != KEY_ESC) {
        // Verificar se a cobrinha bateu na parede
        if ((cobra->y == 0) || (cobra->y == 11) || (cobra->x == 0) || (cobra->x == 38)) {
            mostrarGameOver(pontos);
            break;
        }

        // Verificar se alguma tecla foi pressionada
        if (kbhit()) {
            ch = getch();
            if (ch == 0 || ch == 224) {
                ch = 256 + getch();
            }
        }

        // Determinar a nova posição da cabeça da cobra
        int novaX = cobra->x;
        int novaY = cobra->y;
        switch ((int)ch) {
            case ARROW_UP:
                novaY--;
                break;
            case ARROW_DOWN:
                novaY++;
                break;
            case ARROW_LEFT:
                novaX--;
                break;
            case ARROW_RIGHT:
                novaX++;
                break;
            default:
                break;
        }

        // Adicionar a nova cabeça
        adicionarCabeca(&cobra, novaX, novaY);

        // Verificar se a cobrinha comeu o ponto
        if (PontoPosicaoX == cobra->x && PontoPosicaoY == cobra->y) {
            tamanhoCobra++; // Aumentar o tamanho da cobra
            pontos++; // Aumentar a pontuação
            // Gerar novo ponto
            PontoPosicaoX = (rand() % 37) + 1;
            PontoPosicaoY = (rand() % 10) + 1;
        } else {
            // Remover a cauda apenas se não comeu o ponto
            removerCauda(&cobra);
        }

        // Limpa a tela e desenha tudo de novo
        mostrarLimitesMatriz();

        // Desenha a cobra
        Segmento* atual = cobra;
        while (atual != NULL) {
            irColunaLinha(atual->x, atual->y);
            printf("%c", (atual == cobra) ? 'H' : 'o'); // Cabeça é 'H', corpo é 'o'
            atual = atual->proximo;
        }

        // Desenha o ponto
        irColunaLinha(PontoPosicaoX, PontoPosicaoY);
        printf("%c", 'c');

        // Atualiza o placar
        irColunaLinha(0, 12);
        printf("Pontos: %d", pontos);

        Sleep(velocidade);

        // Verifica se a cobra bateu nela mesma
        atual = cobra->proximo;
        while (atual != NULL) {
            if (cobra->x == atual->x && cobra->y == atual->y) {
                mostrarGameOver(pontos);
                liberarLista(cobra);
                return;
            }
            atual = atual->proximo;
        }
    }

    liberarLista(cobra); // Liberar memória ao final do jogo
}

int main() {
    time_t t;
    srand((unsigned)time(&t));
    jogarCobrinha();
    return 0;
}