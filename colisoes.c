/*
 * Simulador de colisões de bolas de sinuca.
 *
 * Compilar (Linux / MSYS2 / MinGW):
 *   gcc colisoes.c -o colisoes $(sdl2-config --cflags --libs) -lm
 *
 * Executar (todos os argumentos são opcionais):
 *   ./colisoes [N] [largura] [altura] [vel_media] [dt] [raio]
 *   ./colisoes 30 900 600 200 0.002 15
 *
 * Unidades: pixels e segundos. Todas as bolas têm massa 1,
 * e as colisões são perfeitamente elásticas.
 */

#include <SDL2/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*parâmetros*/
static int    N       = 25;      /* número de bolas */
static int    LARGURA = 900;     /* largura da janela */
static int    ALTURA  = 600;     /* altura da janela */
static double VMED    = 200.0;   /* velocidade média (px/s) */
static double DT      = 0.002;   /* intervalo de integração (s) */
static double RAIO    = 15.0;    /* raio das bolas */

/*variáveis iniciais do problema*/
static double *x, *y;            /* posições   */
static double *vx, *vy;          /* velocidades */
static Uint8 (*cor)[3];          /* cor de cada bola */

static double aleatorio(double a, double b)
{
    return a + (b - a) * (rand() / (double)RAND_MAX);
}

/* sorteia posições e velocidades */
static void inicializar(void)
{
    for (int i = 0; i < N; i++) {
        int ok, tentativas = 0;
        do {
            x[i] = aleatorio(RAIO, LARGURA - RAIO);
            y[i] = aleatorio(RAIO, ALTURA - RAIO);
            ok = 1;
            for (int j = 0; j < i; j++) {
                double dx = x[i] - x[j], dy = y[i] - y[j];
                if (dx * dx + dy * dy < 4 * RAIO * RAIO) { ok = 0; break; }
            }
        } while (!ok && ++tentativas < 10000);

        double ang = aleatorio(0, 2 * M_PI);
        double v   = aleatorio(0.5 * VMED, 1.5 * VMED);
        vx[i] = v * cos(ang);
        vy[i] = v * sin(ang);

        cor[i][0] = (Uint8)aleatorio(60, 255);
        cor[i][1] = (Uint8)aleatorio(60, 255);
        cor[i][2] = (Uint8)aleatorio(60, 255);
    }
}

/*uma unidade de tempo do programa*/
static void passo(void)
{
    /*atualiza posições*/
    for (int i = 0; i < N; i++) {
        x[i] += vx[i] * DT;
        y[i] += vy[i] * DT;
    }

    /*colisões*/
    for (int i = 0; i < N; i++) {
        if (x[i] < RAIO)             { x[i] = RAIO;             vx[i] =  fabs(vx[i]); }
        if (x[i] > LARGURA - RAIO)   { x[i] = LARGURA - RAIO;   vx[i] = -fabs(vx[i]); }
        if (y[i] < RAIO)             { y[i] = RAIO;             vy[i] =  fabs(vy[i]); }
        if (y[i] > ALTURA - RAIO)    { y[i] = ALTURA - RAIO;    vy[i] = -fabs(vy[i]); }
    }

    /* colisões entre as bolas */
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            double dx = x[j] - x[i];
            double dy = y[j] - y[i];
            double dist2 = dx * dx + dy * dy;
            double min = 2 * RAIO;

            if (dist2 >= min * min || dist2 == 0.0) continue;

            double dist = sqrt(dist2);
            double nx = dx / dist;           
            double ny = dy / dist;

            /* componente da velocidade relativa ao longo da normal */
            double vn = (vx[i] - vx[j]) * nx + (vy[i] - vy[j]) * ny;

            /* só troca velocidades se estiverem se aproximando */
            if (vn > 0) {
                /* massas iguais + choque elástico:
                   as componentes normais são trocadas entre as bolas;
                   as componentes tangenciais não mudam. */
                vx[i] -= vn * nx;  vy[i] -= vn * ny;
                vx[j] += vn * nx;  vy[j] += vn * ny;
            }

            /* corrige a sobreposição */
            double overlap = min - dist;
            x[i] -= 0.5 * overlap * nx;  y[i] -= 0.5 * overlap * ny;
            x[j] += 0.5 * overlap * nx;  y[j] += 0.5 * overlap * ny;
        }
    }
}

/* desenha um círculo preenchido com linhas horizontais */
static void desenhar_bola(SDL_Renderer *r, int cx, int cy, int raio)
{
    for (int dy = -raio; dy <= raio; dy++) {
        int dx = (int)sqrt((double)(raio * raio - dy * dy));
        SDL_RenderDrawLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

/* energia cinética total e módulo do momento linear total */
static void grandezas(double *energia, double *momento)
{
    double e = 0, px = 0, py = 0;
    for (int i = 0; i < N; i++) {
        e  += 0.5 * (vx[i] * vx[i] + vy[i] * vy[i]);
        px += vx[i];
        py += vy[i];
    }
    *energia = e;
    *momento = sqrt(px * px + py * py);
}

int main(int argc, char *argv[])
{
    /* leitura dos parâmetros pela linha de comando */
    if (argc > 1) N       = atoi(argv[1]);
    if (argc > 2) LARGURA = atoi(argv[2]);
    if (argc > 3) ALTURA  = atoi(argv[3]);
    if (argc > 4) VMED    = atof(argv[4]);
    if (argc > 5) DT      = atof(argv[5]);
    if (argc > 6) RAIO    = atof(argv[6]);
    if (N < 1 || LARGURA < 100 || ALTURA < 100 || DT <= 0 || RAIO <= 0) {
        fprintf(stderr, "Parametros invalidos.\n"
                "Uso: %s [N] [largura] [altura] [vel_media] [dt] [raio]\n", argv[0]);
        return 1;
    }

    srand((unsigned)time(NULL));

    x  = malloc(N * sizeof(double));
    y  = malloc(N * sizeof(double));
    vx = malloc(N * sizeof(double));
    vy = malloc(N * sizeof(double));
    cor = malloc(N * sizeof(*cor));
    if (!x || !y || !vx || !vy || !cor) {
        fprintf(stderr, "Sem memoria.\n");
        return 1;
    }
    inicializar();

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Erro SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *janela = SDL_CreateWindow("Simulador de Colisoes",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LARGURA, ALTURA, SDL_WINDOW_SHOWN);
    SDL_Renderer *rend = SDL_CreateRenderer(janela, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!janela || !rend) {
        fprintf(stderr, "Erro SDL: %s\n", SDL_GetError());
        return 1;
    }

    int rodando = 1;
    Uint32 anterior = SDL_GetTicks();
    double acumulador = 0.0;      /* tempo real ainda não simulado */
    Uint32 ultimo_titulo = 0;

    while (rodando) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) rodando = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) rodando = 0;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r) inicializar();
        }

        /* roda quantos passos de DT forem necessários para acompanhar o tempo real */
        Uint32 agora = SDL_GetTicks();
        acumulador += (agora - anterior) / 1000.0;
        anterior = agora;
        if (acumulador > 0.1) acumulador = 0.1;   /* evita "espiral da morte" */
        while (acumulador >= DT) {
            passo();
            acumulador -= DT;
        }

        /* desenho */
        SDL_SetRenderDrawColor(rend, 15, 90, 45, 255);    /* feltro verde */
        SDL_RenderClear(rend);
        for (int i = 0; i < N; i++) {
            SDL_SetRenderDrawColor(rend, cor[i][0], cor[i][1], cor[i][2], 255);
            desenhar_bola(rend, (int)x[i], (int)y[i], (int)RAIO);
        }
        SDL_RenderPresent(rend);

        /* mostra E e |P| no título */
        if (agora - ultimo_titulo > 500) {
            double en, mo;
            grandezas(&en, &mo);
            char titulo[128];
            snprintf(titulo, sizeof titulo,
                     "Colisoes | N=%d | Energia = %.1f | |P| = %.2f", N, en, mo);
            SDL_SetWindowTitle(janela, titulo);
            ultimo_titulo = agora;
        }
    }

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(janela);
    SDL_Quit();
    free(x); free(y); free(vx); free(vy); free(cor);
    return 0;
}
