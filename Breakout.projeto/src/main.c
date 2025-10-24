/**
 * @file main.c
 * @author Prof. Dr. David Buzatto (base Pong)
 * @brief Jogo "Blackout" no estilo Breakout do Atari, feito em C com a Raylib.
 * 
 * Este jogo simula o clássico estilo Breakout, onde o jogador rebate uma bolinha
 * para quebrar blocos no topo da tela. Vence quando todos os blocos forem destruídos.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


/*---------------------------------------------
 * Library headers.
 -------------------------------------------*/
#include "raylib/raylib.h"

/*---------------------------------------------
 * Project headers.
 -------------------------------------------/




/*---------------------------------------------
 * Macros.
 --------------------------------------------*/

#define HBLOCOS 5
#define VBLOCOS 10
#define LARG_BLOCOS 80
#define ALT_BLOCOS 23
#define DIST_BLOCOS 8

/*--------------------------------------------
 * Constants. 
 ------------------------------------------*/


/*---------------------------------------------
 * Custom types (enums, structs, unions, etc.)
 -------------------------------------------*/
typedef struct Jogador {
    Rectangle barra;
    float velo;
    Color cor;
} Jogador;

typedef struct Bolinha {
    Vector2 pos;
    Vector2 vel;
    float raio;
} Bolinha;

typedef struct Bloco {
    Rectangle rec;    //forma e pocisao do bloco
    int pontos;
    int vidasblocos;
    bool vivo; // se o bloco ja foi destrudo ou nao
    Color cor;
} Bloco;

/*---------------------------------------------
 * Global variables.
 -------------------------------------------*/

Jogador jogador;
Bolinha bolinha;
Bloco blocos[HBLOCOS][VBLOCOS];
int pontuacao = 0;
bool venceu = false;
bool inicio = false;
bool vitoria = false;
bool derrota = false;
int vidas = 3;

/*---------------------------------------------
 * Function prototypes. 
 -------------------------------------------*/
/**
 * @brief Reads user input and updates the state of the game.
 */
void update( float delta );

/**
 * @brief Draws the state of the game.
 */
void draw(void);

void inicializar(void);

void atualizarJogador( Jogador *j, float delta );
void atualizarBolinha( Bolinha *b, float delta );
void resolColisoes( Bolinha *b, Jogador *j, Bloco blocos[HBLOCOS][VBLOCOS]);
void desenharJogador( Jogador *j);
void desenharBolinha(Bolinha *b);
void desenharBlocos(Bloco blocos[HBLOCOS][VBLOCOS]);
Color corVidasBloc( int vidas);

/**
 * @brief Game entry point.
 */

int main(void) {

    // local variables and initial user input

    // antialiasing
    SetConfigFlags( FLAG_MSAA_4X_HINT );

    // creates a new window 800 pixels wide and 450 pixels high
    InitWindow(800, 500, "Breakout");

    // init audio device only if your game uses sounds
    //InitAudioDevice();

    // FPS: frames per second  
    SetTargetFPS(60);
       
    inicializar();

    // you must load game resources here

    
    // Game loop
    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        if (!inicio) {
            if (IsKeyPressed(KEY_ENTER)) {
                if (vitoria || derrota) {
                    inicializar();    
                    vidas = 3;
                    pontuacao = 0;
                    vitoria = false;
                    derrota = false;
                }

                inicio = true;
            }
        } else{
            update(delta);
        }

        draw();  
       
    }

    // you should unload game resources here

    // close audio device only if your game uses sounds
    //CloseAudioDevice();

    CloseWindow();
    return 0;
}

void update( float delta ) {
    if (IsKeyPressed(KEY_V)) {
        vitoria = true;
        inicio = false;
        return;
    }

    atualizarJogador(&jogador, delta);
    atualizarBolinha(&bolinha, delta);
    resolColisoes(&bolinha, &jogador, blocos);

    venceu = true;
    for (int i = 0; i < HBLOCOS; i++) {
        for (int j = 0; j < VBLOCOS; j++) {
            if (blocos[i][j].vivo) {
                venceu = false;
                break;
            }
        }
        if (!venceu) {
            break;
        }
    }

    if (venceu) {
        vitoria = true;
        inicio = false;
    }
}

void draw( void ) {

    BeginDrawing();
    ClearBackground(BLACK);

    if (!inicio && !vitoria && !derrota) {
        DrawText("Pressione ENTER para iniciar o jogo", 210, 500 / 2, 20, RED);
    } else if (vitoria) {
        DrawText("VOCE VENCEU!", 280, 500 / 2 - 30, 30, GREEN);
        DrawText("Pressione ENTER para reiniciar", 230, 500 / 2 +10, 20, WHITE);
    } else if (derrota) {
        DrawText("GAME OVER", 300, 500 / 2 - 30, 30, RED);
        DrawText("Pressione ENTER para reiniciar", 230, 500 / 2 + 10, 20, WHITE);
    } else {
        desenharJogador(&jogador);
        desenharBolinha(&bolinha);
        desenharBlocos(blocos);

        for (int i = 0; i < vidas; i++) {
            DrawCircle(GetScreenWidth() - 20 - i * 30, 20,10, RED);
        }

        DrawText(TextFormat("Pontuacao: %d", pontuacao), 10, 10, 20, GREEN);
    }

    EndDrawing();
}

void inicializar( void ) {

    jogador = (Jogador) {
        .barra = { 
            .x = GetScreenWidth() / 2 - 30, 
            .y = GetScreenHeight() - 30, 90, 12 
        },
        .velo = 400,
        .cor = WHITE

    };

    bolinha = (Bolinha){
        .pos = { 
            .x = jogador.barra.x + jogador.barra.width / 2, 
            .y = jogador.barra.y - 10 
        },
        .vel = { 
            .x = 200,
            .y = -200 
        },
        .raio = 10
        
    };

    for (int i = 0; i < HBLOCOS; i++) {
        for (int j = 0; j < VBLOCOS; j++) {
            int vidas = HBLOCOS - i; 
                blocos[i][j].rec = (Rectangle){
                10 + j * (LARG_BLOCOS + DIST_BLOCOS),
                50 + i * (ALT_BLOCOS + DIST_BLOCOS),
                LARG_BLOCOS,
                ALT_BLOCOS
            };

            blocos[i][j].vivo = true;
            blocos[i][j].pontos = 10;
            blocos[i][j].vidasblocos = vidas;
            blocos[i][j].cor = corVidasBloc( vidas);
        }
    }

    venceu = false;
}

void atualizarJogador( Jogador *j,float delta ){

    if (IsKeyDown(KEY_LEFT)) {
        j -> barra.x -= j-> velo * delta;
    }
    if (IsKeyDown(KEY_RIGHT)) {
        j -> barra.x += j->velo * delta;
    }
    if (j -> barra.x < 0){ 
        j ->barra.x = 0;
        
    }else if (j -> barra.x + j->barra.width > GetScreenWidth()) {
        j -> barra.x = GetScreenWidth() - j ->barra.width;
    }
}

void desenharJogador( Jogador *j){
    DrawRectangleRec(
        j -> barra,
        j -> cor 
    );
}

void atualizarBolinha(Bolinha *b, float delta) {

    b ->pos.x += b->vel.x * delta;
    b -> pos.y += b->vel.y * delta;

    if (b->pos.x + b->raio > GetScreenWidth()) {
        b->pos.x = GetScreenWidth() - b->raio;
        b->vel.x *= -1;
    }

    if (b->pos.x - b->raio < 0) {
        b->pos.x = b->raio;
        b->vel.x *= -1;
    }

    if (b->pos.y - b->raio < 0) {
        b->pos.y = b->raio;
        b->vel.y *= -1;
    }

    if (b->pos.y + b->raio > GetScreenHeight()) {
        vidas--;

        if (vidas > 0) {

            jogador.barra.x = GetScreenWidth() / 2 - jogador.barra.width / 2;
            jogador.barra.y = GetScreenHeight() - 30;

            b->pos = (Vector2){
                jogador.barra.x + jogador.barra.width / 2,
                jogador.barra.y - b->raio - 2
            };
            b->vel = (Vector2){
                .x = 200,
                .y = 200
            };
            inicio = false;
        } else {
            derrota = true;
            inicio = false;
        }
    
    }

    if (CheckCollisionCircleRec(b->pos, b ->raio, jogador.barra)) {
        b ->vel.y *= -1;
        b->pos.y = jogador.barra.y - b->raio;
    }

}

void desenharBolinha(Bolinha *b){

    DrawCircleV(
        b -> pos, 
        b -> raio, 
        YELLOW
    );
}

void resolColisoes( Bolinha *b, Jogador *j, Bloco blocos[HBLOCOS][VBLOCOS] ) {
 
    for (int i = 0; i < HBLOCOS; i++) {
        for (int j = 0; j < VBLOCOS; j++) {
            if (blocos[i][j].vivo && CheckCollisionCircleRec(
                    b -> pos, 
                    b -> raio, 
                    blocos[i][j].rec)) {

                    blocos[i][j].vidasblocos--;

                    if(blocos[i][j].vidasblocos <= 0){
                        blocos[i][j].vivo = false;  
                        pontuacao += blocos[i][j].pontos;  
                    }else{
                        blocos[i][j].cor = corVidasBloc(blocos[i][j].vidasblocos);
                    }
                
                b -> vel.y = 200;
            }
        }
    }
}


void desenharBlocos(Bloco blocos[HBLOCOS][VBLOCOS]) {
    for (int i = 0; i < HBLOCOS; i++) {
        for (int j = 0; j <VBLOCOS; j++) {

            if (blocos[i][j].vivo) {
                DrawRectangleRec(
                    blocos[i][j].rec, 
                    blocos[i][j].cor
                );
            }
        }
    }
}

Color corVidasBloc(int vidas){
    switch( vidas){
        case 5: 
        return WHITE;
        case 4: 
        return YELLOW;
        case 3:
        return ORANGE;
        case 2:
        return RED;
        case 1:
        return PINK;
        default: 
        return GREEN;
    }
}