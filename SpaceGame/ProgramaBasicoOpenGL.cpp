#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>

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
    float y; //pega a posição do jogador quando aperta espaço e vai almentando até chegar no topo da tela.
    int existe; //booleano, para ver se o tiro está ou não na tela
    int linhas;
    int colunas;
    int model[5][3];
} Tiro;

Tiro modeloTiro;

//Inimigos
typedef struct
{
    int resistance; //Vida do inimigo
    int speed; //velocidade
    int linhas;
    int colunas;
    int model[5][5]; //modelo
    float posicaoX;
    float posicaoY;
}Inimigo;


//Informações dos tipos de inimigos
Inimigo dataInimigos[1];

//Player
typedef struct
{
    float linhas;
    float colunas;
    int model[16][16];
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
//cria o vetor de tiros
Tiro vetortiro[10]; //maximo de tiros == 10


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

void PrintInimigos(){
    printf("Resistance: %d ",dataInimigos[0].resistance);
    printf("Speed: %d ",dataInimigos[0].speed);
    printf("LinhasMatriz: %d " ,dataInimigos[0].linhas);
    printf("ColunasMatriz: %d\n",dataInimigos[0].colunas);
}


void ImportInimigos()
{
    ifstream arquivo;
    arquivo.open("Inimigos.txt");
    int id = 0, vida = 0, linhas= 0, colunas= 0,i= 0, j = 0, aux = 0;
    int velocidade = 0;

    while(arquivo >> vida >> velocidade >> linhas >> colunas)
    {
        Inimigo novoInimigo;
        novoInimigo.resistance = vida;
        novoInimigo.speed = velocidade;
        novoInimigo.linhas = linhas;
        novoInimigo.colunas = colunas;
        novoInimigo.posicaoX = 0.0;
        novoInimigo.posicaoY = 0.0;
        for(i = 0; i < linhas; i++)
        {
            for(j = 0; j < colunas; j++)
            {
                arquivo >> aux;
                novoInimigo.model[i][j] = aux;
            }
        }
         //printf("%f",novoInimigo.speed);//ok
        dataInimigos[0] = novoInimigo;

        id++;
    }
    arquivo.close();

}

void ImportPlayer()
{
    ifstream arquivo;
    arquivo.open("Player.txt");
    int i,j,aux;
    float linhas,colunas;
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

void ImportTiro()
{
    ifstream arquivo;
    arquivo.open("Modelos.txt");
    int i,j,aux;
    float linhas,colunas;
    arquivo >> linhas >> colunas;
    modeloTiro.linhas = linhas;
    modeloTiro.colunas = colunas;
    for(i = 0; i < linhas; i++)
    {
        for(j = 0; j < colunas; j++)
        {
            arquivo >> aux;
            modeloTiro.model[i][j] = aux;
        }
    }
    arquivo.close();
}

void ImportModels()
{
    ImportCores();
    ImportInimigos();
    ImportPlayer();
    ImportTiro();
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
    Inimigo *inimigoAtivo;
    for(int i = 0; i< 10;i++)
    {
        if(game.inimigoBool[i] == 1)
        {
            inimigoAtivo = &game.inimigosAtivos[i];
            inimigoAtivo->posicaoY = inimigoAtivo->posicaoY - inimigoAtivo->speed;
            if ( inimigoAtivo->posicaoY <= 10)
            {
                game.vidas--;
                inimigoAtivo->posicaoY = orthoHeight - step*10; //inimigo volta para cima da tela
                float spawnLocation = rand() % orthoWidth;
                inimigoAtivo->posicaoX = spawnLocation;//inimigo vai para um lugar aleatorio
            }

        }
    }
}

int LiberaIndexInimigo(){
    for(int i = 0; i<10;i++){
        if(game.inimigoBool[i] == 0){
            return i;
        }
    }
    return -1;
}

void SpawnEnemy(int enemyId)
{
    int valid = LiberaIndexInimigo();

    cout << "Index válido: " << valid << endl;

    if(valid != -1)
    {
        float spawnLocation = rand() % orthoWidth;
        //printf("spawlocation  %f",spawnLocation);

        Inimigo newEnemy = dataInimigos[enemyId];

        newEnemy.posicaoY = orthoHeight - step*10;
        newEnemy.posicaoX = spawnLocation;

        //printf("Speed: %d",enemy.resistance);
        for(int i = 0; i < 10; i++)
        {
            if(game.inimigoBool[i] == 0)
            {
                game.inimigosAtivos[i] = newEnemy;
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
    int i,j,r,g,b;

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

float scaleLaser = 0.7;
float speedLaser = 0.5;

void Laser(float x, float y, int index)
{
    if(vetortiro[index].existe == 0)
    {
        return;
    }
    int i,j,r,g,b;

    y = y + speedLaser;//desloca o laser na vertical

    //atualiza a posição no vetortiros
    vetortiro[index].y = y;
    glPushMatrix();
        glTranslated(x,y,0); //desenha o laser em determinada posição
        glScalef(scaleLaser,scaleLaser,1);
        glBegin(GL_QUADS);
            for(i = 0; i < modeloTiro.linhas; i++)
                {
                    for(j = 0; j < modeloTiro.colunas; j++)
                    {
                        r = cores[modeloTiro.model[i][j]][0];
                        g = cores[modeloTiro.model[i][j]][1];
                        b = cores[modeloTiro.model[i][j]][2];
                        DrawPixel(j, modeloTiro.linhas - i,r,g,b);
                    }
                }
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

void DestroiInimigo(int index){
    game.inimigoBool[index] = 0;
}

void ContaDanoInimigo(){
    Inimigo inimigo;
    Tiro tiro;
    for(int existem = 0; existem<10;existem++){//percorre game.inimigosAtivos
        if(game.inimigoBool[existem] == 1){
            inimigo = game.inimigosAtivos[existem];
            for(int existetiro= 0;existetiro<10;existetiro++){//percorre os tiros que existem
                tiro = vetortiro[existetiro];
                if(tiro.existe == 1){//se o tiro existe

                    int colisionX = inimigo.posicaoX <= tiro.x + (modeloTiro.linhas * scaleLaser) && inimigo.posicaoX + inimigo.linhas >= tiro.x ? 1 : 0;
                    int colisionY = inimigo.posicaoY <= tiro.y + (modeloTiro.colunas * scaleLaser) ;


                    if(colisionX && colisionY)
                    {
                        inimigo.resistance--;
                        if(inimigo.resistance == 0)
                            DestroiInimigo(existem);

                        vetortiro[existetiro].existe = 0;
                        vetortiro[existetiro].x = 0;
                        vetortiro[existetiro].y = 0;
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

    if (SpawnDeltaT > 3) // imprime o frame rate a cada 5 segundos
    {
        SpawnDeltaT =0;
        cout << "FPS: " << 1.0/dt << endl;

       // int type = rand() % 3;

       //Spawna um inimigo do tipo 0;
       // printf("-- %d",dataInimigos[0].speed);
        SpawnEnemy(0);

    }

    //Gerencia a vida dos inimigos
    ContaDanoInimigo();
    GerenciaVidasInimigos();
    ContaDanoPlayer();

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
			glutReshapeWindow ( 700,700  );
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
