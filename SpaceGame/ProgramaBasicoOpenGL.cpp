
#include <iostream>
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

#include "Temporizador.h"

typedef struct
{
    float x; //pega a posição do jogador quando aperta espaço
    float y; //pega a posição do jogador quando aperta espaço e vai almentando até chegar em 10
    int existe; //booleano, para ver se o tiro está ou não na tela
} Tiro;

Temporizador T;
double AccumDeltaT=0;
int tecla; //movimentação segundo o usuário
float deslocamento = 0;//deslocamento de movimentação

//cria o vetor de tiros
    Tiro vetortiro[10]; //maximo de tiros == 10


// **********************************************************************
//  void init(void)
//  Inicializa os par�metros globais de OpenGL
//
// **********************************************************************
void init(void)
{
	// Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);// R G B

    //zera o vetor
    for(int i=0; i<10; i++) {
        vetortiro[i].x = 0;
        vetortiro[i].y = 0;
        vetortiro[i].existe = 0;
    }
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

    glOrtho(0,10,0,10,0,1); // Universo de Trabalho - Sistema de Referencia do Universo
                            // Define os Limites da minha aplica��o - WINDOW

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
void DesenhaLinhas()
{
    glBegin(GL_LINES); // o que desenhar
    glVertex2f(0,0); // as coordenadas do desenho
    glVertex2f(5,5); //
    glEnd();

    glBegin(GL_LINES);
    glVertex2f(5,5);
    glVertex2f(10,0);
    glEnd();
}
// **********************************************************************

void Inimigo()
{

}

void DrawPixel(float x, float y, float r, float g, float b)
{
    glColor3f(r,g,b);
    glVertex2f(x,y);
    glVertex2f(x,y+0.1);
    glVertex2f(x+0.1,y+0.1);
    glVertex2f(x+0.1,y);
}

void DrawPixel(float x, float y)
{
    glVertex2f(x,y);
    glVertex2f(x,y+0.1);
    glVertex2f(x+0.1,y+0.1);
    glVertex2f(x+0.1,y);
}

void Laser(float x, float y, int i)
{
    y = y + 0.3;//desloca o laser na vertical
    //atualiza a posição no vetortiros
    vetortiro[i].y = y;

    glPushMatrix();
        glTranslated(x,y,0); //desenha o laser em determinada posição
        glScalef(0.7,0.7,1);
        glBegin(GL_QUADS);
            DrawPixel(0,0.4,1,0,0);
            DrawPixel(0,0.3);
            DrawPixel(0,0.2);
            DrawPixel(0,0.1);
            DrawPixel(0.1,0.5);
            DrawPixel(0.1,0.4);
            DrawPixel(0.1,0.3);
            DrawPixel(0.1,0.2);
            DrawPixel(0.1,0.1);
            DrawPixel(0.1,0.0);
            DrawPixel(0.2,0.4);
            DrawPixel(0.2,0.3);
            DrawPixel(0.2,0.2);
            DrawPixel(0.2,0.1);
        glEnd();
    glPopMatrix();
}

void DesenhaTiros(){
    for(int i = 0; i<10;i++){//procura os tiros disponiveis
        if(vetortiro[i].existe == 1){//se o tiro é disponivel, desenha ele
            Laser(vetortiro[i].x,vetortiro[i].y, i);
        }
    }
}

void LiberaTiro(){//é chamado pelo player quando aperta espaço
    for(int i = 0; i<10;i++){//procura um tiro disponivel
        if(vetortiro[i].existe == 0){//achou o tiro disponivel
            i = 10;
           vetortiro[i].existe = 1;
           vetortiro[i].x = 5;//recebe o x do jogador + algum valor pra o tiro sair do meio do player
           vetortiro[i].y = 5;//recebe o x do jogador + algum valor pra o tiro sair do meio do player
        }
    }
    //se não achar nenhum tiro liberado nao atira pois já atingiu o máximo da tela
}

void RetemTiros(){//controla os tiros que passarem do máximo da tela para que sejam reutilizados
    for(int i = 0; i<10;i++){//procura um tiro disponivel
        if(vetortiro[i].existe == 1){//achou o tiro disponivel
            if(vetortiro[i].y >= 10){//se o tiro atingiu o topo da tela
                //reinicia o tiro
                vetortiro[i].existe = 0;
                vetortiro[i].x = 0;
                vetortiro[i].y = 0;
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
    AccumDeltaT += dt;
    if (AccumDeltaT > 3) // imprime o frame rate a cada 3 segundos
    {
        AccumDeltaT =0;
        cout << "FPS: " << 1.0/dt << endl;
    }

    // Não desenha os tiros que já atingiram o limite da tela
    RetemTiros();
    DesenhaTiros();

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

    glTranslatef(0,5,0);






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
            //Laser();
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
            deslocamento-=0.1;
            if(deslocamento <= 0){ //limite da esquerda
                deslocamento = 0.01;
            }
            break;
        case GLUT_KEY_RIGHT:
            deslocamento += 0.1;
                if(deslocamento >= 10){ //limite da direita
                deslocamento = 8.99;
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
    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 650, 500);

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
