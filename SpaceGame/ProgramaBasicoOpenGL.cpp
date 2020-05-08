#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>

using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

#include "Temporizador.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////
/////////////////////      Definição de Structs
/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Tiro
typedef struct
{
    float x; //pega a posição do jogador quando aperta espaço
    float y; //pega a posição do jogador quando aperta espaço e vai almentando até chegar em 10
    int existe; //booleano, para ver se o tiro está ou não na tela
    float tirohitbox[3]; //tamanho horizontal dos tiros
} Tiro;

//Inimigos
typedef struct
{
    int index;
    int id;
    int resistance; //Vida do inimigo
    float speed; //velocidade
    int linhas;
    int colunas;
    float posicaoX;
    float posicaoY;
    int model[5][5]; //modelo
    float hitbox[5];//tamanho do desenho do inimigo na horizontal
}   Inimigo;

//Informações dos tipos de inimigos
Inimigo dataInimigos[3];


//Player
typedef struct
{
    int linhas;
    int colunas;
    int model[16][16];
    float hitbox[16];//tamanho do desenho do player na horizontal
}Player;

//Instanciando o jogador;
Player jogador;


//Controlador do jogo
typedef struct
{
    int vidas; //vidas do player
    int inimigoBool[10] = {0,0,0,0,0,0,0,0,0,0};
    Inimigo inimigosAtivos[10];
    //int posicaoInimigos[10][1][1];//posicaoInimigos[x][y][z] x=id; y=x em que o inimigo inicia; z= x em que o inimigo acaba
    int existeminimigos;

}GameController;

//Instancia do controller
GameController game;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////
/////////////////////      Definição de Variáveis Globais
/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//Definição ortografia
int orthoHeight = 100;
int orthoWidth = 100;

//O valor de um "pixel"
int step = orthoHeight/orthoWidth;

//Array de cores do jogo
int cores[10][3];

Temporizador T;
double SpawnDeltaT, MoveDeltaT;

int tecla; //movimentação segundo o usuário
float deslocamento = 0;//deslocamento de movimentação

//quarda a última posição do jogador para saber de onde o tiro começa
float jogadorx;

int index;

//cria o vetor de tiros
Tiro vetortiro[10]; //maximo de tiros == 10


Inimigo getInimigo(int indexx){
    for(int i = 0; i>10;i++){
        if(game.inimigosAtivos[i].index == indexx){
            return game.inimigosAtivos[i];
        }
    }
}



// **********************************************************************
//  void init(void)
//  Inicializa os par�metros globais de OpenGL
//
// **********************************************************************
void init(void)
{
	// Define a cor do fundo da tela (preto)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);// R G B

    //zera o vetor
    for(int i=0; i<10; i++) {
        vetortiro[i].x = 0;
        vetortiro[i].y = 0;
        vetortiro[i].existe = 0;
    }

    index = 0;

    game.vidas = 3;

    srand(time(NULL));
}

// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    cout << "RESHAPE !! " << endl;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela �rea OpenGL dentro da Janela

    glViewport(0, 0, w, h);  // Sistema de Referencia do Dispositivo
                            // ONDE aparece o desenho - VIEWPORT

    glOrtho(0,orthoWidth,0,orthoHeight,0,1); // Universo de Trabalho - Sistema de Referencia do Universo
                        // Define os Limites da minha aplica��o - WINDOW

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////
/////////////////////      Importação dos Modelos do jogo
/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void ImportCores()
{
    ifstream arquivo;
    arquivo.open("Cores.txt");
    int id, r, g ,b;
    while(arquivo >> id >> r >> g >>b)
    {
        cores[id][0] = r;
        cores[id][1] = g;
        cores[id][2] = b;
    }
    arquivo.close();

}

void ImportInimigos()
{
    ifstream arquivo;
    arquivo.open("Inimigos.txt");
    int id, vida, linhas,colunas, i, j, aux;;
    float velocidade;

    while(arquivo >> id >> vida >> velocidade >> linhas >> colunas)
    {
        Inimigo novoInimigo;
        novoInimigo.id = id;
        novoInimigo.resistance = vida;
        novoInimigo.speed = velocidade;
        novoInimigo.linhas = linhas;
        novoInimigo.colunas = colunas;
        for(i = 0; i < linhas; i++)
        {
            for(j = 0; j < colunas; j++)
            {
                arquivo >> aux;
                novoInimigo.model[i][j] = aux;
            }
        }
        dataInimigos[0] = novoInimigo;
    }
    arquivo.close();
}

void ImportPlayer()
{
    ifstream arquivo;
    arquivo.open("Player.txt");
    int i,j,linhas,colunas,aux;
    arquivo >> linhas >> colunas;
    jogador.linhas = linhas;
    jogador.colunas = colunas;
    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {
            arquivo >> aux;
            jogador.model[i][j] = aux;
        }
    }
    arquivo.close();
}

void ImportModels()
{
    ImportCores();
    ImportInimigos();
    ImportPlayer();
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////
/////////////////////      Comportamento dos Inimigos
/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void DeslocaInimigos()
{
    for(int i = 0; i< 10;i++)
        game.inimigosAtivos[i].posicaoY = game.inimigosAtivos[i].posicaoY - game.inimigosAtivos[i].speed;
}

int LiberaIndexInimigo(){
    for(int i = 0; i<10;i++){
        if(game.inimigoBool[i] == 0){
            return i;
        }
    }
}

void SpawnEnemy(Inimigo enemy)
{
    if(game.inimigoBool[9] == 1)
    {
        //Já tem o limite de inimigos na tela
    }
    else
    {
        float spawnLocation = rand() % orthoWidth;
        //printf("spawlocation  %f",spawnLocation);

        enemy.posicaoY = orthoHeight - step*10;
        enemy.posicaoX = spawnLocation;
        enemy.index = LiberaIndexInimigo();
        enemy.hitbox[0] = enemy.posicaoX;
        for(int i = 1;i<5;i++){
            enemy.hitbox[i] = enemy.posicaoX + 1;
        }

        for(int i = 0; i < 10; i++)
        {
            if(game.inimigoBool[i] == 0)
            {
                game.inimigosAtivos[i] = enemy;
                game.inimigoBool[i] = 1;
                game.existeminimigos++;
                //printf("oiii %d,   x=  %f,   y=  %f", enemy.index,enemy.posicaoX,enemy.posicaoY);
                break;
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////
/////////////////////      Métodos de desenhar na tela
/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////



void DrawPixel(float x, float y, int r, int g, int b)
{
    glColor3ub(r,g,b);
    glVertex2f(x,y);
    glVertex2f(x,y+step);
    glVertex2f(x+step,y+step);
    glVertex2f(x+step,y);
}

void DrawPixel(int x, int y)
{
    glVertex2d(x,y);
    glVertex2d(x,y+step);
    glVertex2d(x+step,y+step);
    glVertex2d(x+step,y);
}

void DesenhaInimigo(Inimigo& enemy)
{
    int i,j, r,g,b;

    glPushMatrix();
        glTranslatef(enemy.posicaoX,enemy.posicaoY,0);
        glBegin(GL_QUADS);
        for(i = 0; i < enemy.linhas; i++)
        {
            for(j = 0; j < enemy.colunas; j++)
            {
                r = cores[enemy.model[i][j]][0];
                g = cores[enemy.model[i][j]][1];
                b = cores[enemy.model[i][j]][2];
                DrawPixel(j,enemy.linhas - i,r,g,b);
            }
        }
        glEnd();
    glPopMatrix();

}

void DesenhaInimigosAtivos()
{
    for(int i = 0; i < 10; i++)
    {
        if(game.inimigoBool[i] != 0)
        {
            DesenhaInimigo(game.inimigosAtivos[i]);
        }
    }

}

void DesenhaPlayer()
{
    int i,j,r,g,b;

    glPushMatrix();
        glTranslatef(deslocamento,0,0);
        jogadorx = deslocamento;
        glScalef(0.7,0.7,1);
        glBegin(GL_QUADS);
        for(i = 0; i < jogador.linhas; i++)
        {
            for(j = 0; j < jogador.colunas; j++)
            {
                r = cores[jogador.model[i][j]][0];
                g = cores[jogador.model[i][j]][1];
                b = cores[jogador.model[i][j]][2];
                DrawPixel(j, jogador.linhas - i,r,g,b);
            }
        }
        glEnd();
    glPopMatrix();
}

void Laser(float x, float y, int i)
{
    y = y + 0.05;//desloca o laser na vertical
    //atualiza a posição no vetortiros
    vetortiro[i].y = y;
    glPushMatrix();
        glTranslated(x,y,0); //desenha o laser em determinada posição
        glScalef(0.7,0.7,1);
        glBegin(GL_QUADS);
            DrawPixel(0,4,255,255,255);
            DrawPixel(0,3,255,255,255);
            DrawPixel(0,2,255,255,255);
            DrawPixel(0,1,255,255,255);
            DrawPixel(1,5,255,255,255);
            DrawPixel(1,4,255,255,255);
            DrawPixel(1,3,255,255,255);
            DrawPixel(1,2,255,255,255);
            DrawPixel(1,1,255,255,255);
            DrawPixel(1,0,255,255,255);
            DrawPixel(2,4,255,255,255);
            DrawPixel(2,3,255,255,255);
            DrawPixel(2,2,255,255,255);
            DrawPixel(2,1,255,255,255);
        glEnd();
    glPopMatrix();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////
/////////////////////      Métodos de gerenciamento de tiros, player e inimigos
/////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void LiberaTiro(){//é chamado pelo player quando aperta espaço
    for(int i = 0; i<10;i++){//procura um tiro disponivel
        if(vetortiro[i].existe == 0){//achou o tiro disponivel
           vetortiro[i].existe = 1;
           vetortiro[i].x = jogadorx + 5;//recebe o x do jogador + algum valor pra o tiro sair do meio do player
           vetortiro[i].y = 6;//algum valor pra o tiro sair do meio do player
           vetortiro[i].tirohitbox[0] = vetortiro[i].x;
           for(int j = 1; j<3;j++){
            vetortiro[i].tirohitbox[j] = vetortiro[i].tirohitbox[1-j] + 1;
           }
           //cai fora do for
           i = 10;
        }
    }
    //se não achar nenhum tiro liberado nao atira pois já atingiu o máximo da tela
}

void RetemTiros(){//controla os tiros que passarem do máximo da tela para que sejam reutilizados
    for(int i = 0; i<10;i++){//procura um tiro disponivel
        if(vetortiro[i].existe == 1){//achou o tiro disponivel
            if(vetortiro[i].y >= orthoHeight){//se o tiro atingiu o topo da tela
                //reinicia o tiro
                vetortiro[i].existe = 0;
                vetortiro[i].x = 0;
                vetortiro[i].y = 0;
            }
        }
    }
}

void ContaDanoInimigo(){
    Inimigo inimigo;
    Tiro tiro;
    for(int existem = 0; existem<10;existem++){//percorre game.inimigosAtivos
        inimigo = game.inimigosAtivos[existem];
        for(int existetiro= 0;existetiro<10;existetiro++){//percorre os tiros que existem
            tiro = vetortiro[existetiro];
            if(tiro.existe == 1){//se o tiro existe

                for(int i = 0; i < 5;i++){//percorre hitbox inimigo
                    for(int t = 0; t<3;t++){ //percorre hitbox tiro
                        if(tiro.tirohitbox[t] == inimigo.hitbox[i]){ //sigifica que estao no mesmo X então dependendo do Y o tiro pega
                            //printf("tiro: %f, inimigo: %f\n", tiro.tirohitbox[t], inimigo.hitbox[i]); //print ok
                            if(inimigo.posicaoY <= tiro.y){
                                //printf("Tiro pego");//ok
                                inimigo.resistance--;
                            }
                        }
                    }
                }
            }
        }
    }
}

void DestroiInimigo(int index){
    game.inimigoBool[index] = 0;
}

void PegaHitBoxPlayer(){ //para nao atualizar a hitbox do player toda vez, e somente quando um inimigo pode dar dano nele, já que o player anda na horizontal
    jogador.hitbox[0] = jogadorx;
    for(int i = 1; i<15;i++){
        jogador.hitbox[i] = jogador.hitbox[i-1] + 1;
    }
}

void ContaDanoPlayer(){//verifica se algum inimigo passou da altura do player, se sim quer dizer que ele pode ter machucado.
    Inimigo inimigo;
     for(int existem = 0; existem<10;existem++){//percorre game.inimigosAtivos
        if(game.inimigoBool[existem]==1){//existe inimigo
            inimigo = game.inimigosAtivos[existem];
            if(inimigo.posicaoY<=10){//altura do player
                //quer dizer q o inimigo pode ter dado dano no player
                PegaHitBoxPlayer();
                for(int hitplay = 0; hitplay<16;hitplay++){//percorre a hitbox do player
                   for(int hitini = 0; hitini < 5;hitini++){//percorre hitbox inimigo
                       if(jogador.hitbox[hitplay] == inimigo.hitbox[hitini]){ //hit box pega
                            game.vidas--;
                            //inimigo some
                            DestroiInimigo(existem);
                            break;
                       }
                   }
                }
            }
        }
     }
}


void GerenciaVidasInimigos(){
    for(int i = 0; i<10;i++){
            if(game.inimigoBool[i] == 1){//se o inimigo existe
                Inimigo ini = game.inimigosAtivos[i];
                if(ini.resistance<=0){
                    //inimigo morre
                    //erro pois todos os inimigos começam dom vida ZERO

                    DestroiInimigo(i);

                    //printf("morreu");
                    //printf("%d",ini.resistance);
                    //printf("Inimigo morreu %d \n",ini.index);
                }
            }
    }
}



// **********************************************************************
//  void display( void )
//
// **********************************************************************
void display( void )
{

    double dt = T.getDeltaT();
    SpawnDeltaT += dt;
    MoveDeltaT += dt;


    if (MoveDeltaT> 1)
    {
        MoveDeltaT = 0;
        DeslocaInimigos();
    }

    if (SpawnDeltaT > 5) // imprime o frame rate a cada 5 segundos
    {
        SpawnDeltaT =0;
        cout << "FPS: " << 1.0/dt << endl;

       // int type = rand() % 3;

        SpawnEnemy(dataInimigos[0]);
    }

    //Gerencia a vida dos inimigos
    if(game.existeminimigos != 0){
        ContaDanoInimigo();
        GerenciaVidasInimigos();
        ContaDanoPlayer();
        //printf("%d",game.existeminimigos);//ok
    }

    // Não desenha os tiros que já atingiram o limite da tela
    RetemTiros();

	// Limpa a tela com a cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();   // Faz a matriz das transforma��es ser a identidade.
                        // Com isto os modelos s�o desenhados sem transforma��es
                        // geom�tricas de transla��o, rota��o ou escala.

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenha os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	//desenha os tiros
    for(int i = 0; i<10;i++){//procura os tiros disponiveis
        if(vetortiro[i].existe == 1){//se o tiro é disponivel, desenha ele
            Laser(vetortiro[i].x,vetortiro[i].y, i);
        }
    }


    DesenhaPlayer();
    DesenhaInimigosAtivos();

    //vida do player
    if(game.vidas<=0){
        //printf("Morreu");//ok ta detectando
    }


	glutSwapBuffers();
}

// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
// **********************************************************************

void keyboard ( unsigned char key, int x, int y )
{

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 32://em caso de espaço
            LiberaTiro();
		default:
			break;
	}
}


// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // Se pressionar UP
			glutFullScreen ( ); // Vai para Full Screen
			break;
	    case GLUT_KEY_DOWN:     // Se pressionar UP
								// Reposiciona a janela
            glutPositionWindow (50,50);
			glutReshapeWindow ( 700, 500 );
			break;
        case GLUT_KEY_LEFT:
            deslocamento-=2;
            if(deslocamento <= 0){ //limite da esquerda
                deslocamento = 0;
            }
            break;
        case GLUT_KEY_RIGHT:
            deslocamento += 2;
                if(deslocamento >= 100 - jogador.colunas){ //limite da direita
                deslocamento = 100  - jogador.colunas;
            }
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
//
// **********************************************************************
int  main ( int argc, char** argv )
{
    ImportModels();
    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 600, 600);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de t�tulo da janela.
    glutCreateWindow    ( "Space Invaders" );

    // executa algumas inicializa��es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser� chamada automaticamente quando
    // for necess�rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalida��o da tela. A funcao "display"
    // ser� chamada automaticamente sempre que a
    // m�quina estiver ociosa (idle)
    glutIdleFunc(display);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser� chamada automaticamente quando
    // o usu�rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser� chamada automaticamente sempre
    // o usu�rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser� chamada
    // automaticamente sempre o usu�rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}
