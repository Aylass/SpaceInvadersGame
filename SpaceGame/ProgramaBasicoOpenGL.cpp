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

#include "Temporizador.h"

Temporizador T;
double AccumDeltaT=0;
int tecla; //movimentação segundo o usuário
float deslocamento = 0;//deslocamento de movimentação


// **********************************************************************
//  void init(void)
//  Inicializa os par�metros globais de OpenGL
//
// **********************************************************************
void init(void)
{
	// Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);// R G B
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

    glOrtho(0,100,0,100,0,1); // Universo de Trabalho - Sistema de Referencia do Universo
                            // Define os Limites da minha aplica��o - WINDOW

    // Define os limites l�gicos da �rea OpenGL dentro da Janela
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************

typedef struct
{
    int id;
    int resistance;
    float speed;
    int linhas;
    int colunas;
    int model[5][5];
}   Inimigo;

Inimigo dataInimigos[3];

int cores[10][3];

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
                cout << aux << " ";
            }
            cout << endl;
        }
        dataInimigos[0] = novoInimigo;
    }
    arquivo.close();
}

void ImportModels()
{
    //ImportCores();
   // ImportInimigos();
}


void DrawPixel(float x, float y, float r, float g, float b)
{
    glColor3i(r,g,b);
    glVertex2f(x,y);
    glVertex2f(x,y+1);
    glVertex2f(x+1,y+1);
    glVertex2f(x+1,y);
}

void DrawPixel(float x, float y)
{
    glVertex2f(x,y);
    glVertex2f(x,y+1);
    glVertex2f(x+1,y+1);
    glVertex2f(x+1,y);
}

void DesenhaInimigo()
{
    int i,j, offsetX,offsetY, r,g,b;

    Inimigo aux = dataInimigos[0];

    glPushMatrix();
        glBegin(GL_QUADS);
        glColor3i(0,255,0);
        for(i = 0; i < aux.linhas; i++)
        {
            for(j = 0; j < aux.colunas; j++)
            {
                r = cores[aux.model[i][j]][0];
                g = cores[aux.model[i][j]][1];
                b = cores[aux.model[i][j]][2];
                DrawPixel(i,j);
            }
        }
        glEnd();
    glPopMatrix();

}

void Quadrado()
{
    glBegin(GL_QUADS);
        glVertex2d(0,0);
        glVertex2d(20,20);
    glEnd();
}

void Laser()
{
    glPushMatrix();
        //glTranslated(deslocamento,0,0);
        glScalef(2,2,1);
        glBegin(GL_QUADS);
            DrawPixel(0,4,1.0,0,0);
            DrawPixel(0,3);
            DrawPixel(0,2);
            DrawPixel(0,1);
            DrawPixel(1,5);
            DrawPixel(1,4);
            DrawPixel(1,3);
            DrawPixel(1,2);
            DrawPixel(1,1);
            DrawPixel(1,0);
            DrawPixel(2,4);
            DrawPixel(2,3);
            DrawPixel(2,2);
            DrawPixel(2,1);
        glEnd();
    glPopMatrix();
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

    glTranslatef(5,5,0);

    //Laser();
    glColor3f(1.0,0,0);
    Quadrado();
    //DesenhaInimigo();




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
                if(deslocamento >= 8){ //limite da direita
                deslocamento = 7.99;
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
    //ImportModels();
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
