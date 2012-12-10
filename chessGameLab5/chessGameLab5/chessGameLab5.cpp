#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <GL/freeglut.h>	// GLUT

#include "model3DS.h" // 3DS model support
#include "textureTGA.h" // TGA support
#include "textureBMP.h"

#include <cstdlib>
#include<ctime>

#include "board.h"

using namespace std;

//Function prototypes:
/*----------------------------------------------------------------------------------------------------*/

//OpenGL functions:
void setupScene();
void updateScene();
void renderScene();
void exitScene();
void keypress(unsigned char key, int x, int y);
void keyReleased(unsigned char key, int x, int y);
void setViewport(int width, int height);

void camera();
void keyOperations();
void mouseClicked(int button, int state, int x, int y);
void mouseWheeled(int wheel, int direction, int x, int y);
void convertMouseCoord(int x, int y, bool click);
void updateSelectedPos(float x, float z, bool click);

//Game/Drawing functions:
void drawSkyBox();
void drawBoard();
void drawPieces(board* b);
void drawMarker();
void discoSwitch();
void resetGame();
void loadModels();
void aiPlayer();

/*####################################################################################################*/



//Constants:
/*----------------------------------------------------------------------------------------------------*/

int		win_width=640;
int		win_height=480;
float	piVal = 3.141592654f;
float	skyboxSize = 2000.0f;
float	startXZ;


//For board drawing:
board	mainBoard;
float	boardWidth = 50.0;
float	boundary = boardWidth*4.0f;
float	boardDepth = boardWidth/10.0f;
float	boardY = 0.0;
float	boardBorder = 5.0;
int		numSquares = 8;
float	squareSize = ((boardWidth*2)-(boardBorder*2))/(float)numSquares;
float	pieceHeight = 2.0f;
float	modelHeight = 4.5f;

GLUquadric* cylQuad;
float	iSquareCentre = NULL;
float	jSquareCentre = NULL;
float	iSelectedCoords = NULL;
float	jSelectedCoords = NULL;

//Bools
bool		inMenu = true;
bool		inOptions = false;
bool		wireframe=false;
bool		fullScreen=true;
bool		music = false;
bool		disco = false;
bool		aiMode = false;

int winRar = 0;

//?
int         windowId;
//GLuint      textureId;
DWORD		lastTickCount;

//Lights/Light positions
GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
GLfloat left_light_position[] = {1,0,-1, 1.0}; 
GLfloat right_light_position[] = {-1,0,-1, 1.0};

GLfloat emerald_ambient[] = {0.0215, 0.1745, 0.0215},
	emerald_diffuse[] = {0.07568, 0.61424, 0.07568},
	emerald_specular[] = {0.633, 0.727811, 0.633},
	emerald_shininess = 76.8;

GLfloat sapphire_ambient[] = {0.0215, 0.0215, 0.1745},
	sapphire_diffuse[] = {0.07568, 0.07568, 0.61424},
	sapphire_specular[] = {0.633, 0.633, 0.727811},
	sapphire_shininess = 80.0;

//Camera
float xpos = 0, ypos = 0, zpos = 100, xrot = 15, yrot = 0, angle=0.0;
float viewHeight = 25.0;

//Controlling
bool	myArray[256] = {false};
bool*	keyStates = myArray;	//For multiple simultaneous keypress

bool	selected = false;
int		iSelected = -1;
int		jSelected = -1;
int		iHover = -1;
int		jHover = -1;

playerColour player = WHITE;

//Menu/Win textures
GLuint		menuTexture;
GLuint		optionsMenuTexture;
GLuint		blackWin;
GLuint		whiteWin;
//Skybox textures
string		texDir = "Textures/moon/";
GLuint		skyBoxBack;
GLuint		skyBoxFront;
GLuint		skyBoxLeft;
GLuint		skyBoxRight;
GLuint		skyBoxTop;
GLuint		skyBoxFloor;

//Models:
bool	loaded = false;
bool	useModels = false;
model3DS *whitePawn;
model3DS *blackPawn;
model3DS *whiteKing;
model3DS *blackKing;
model3DS *whiteQueen;
model3DS *blackQueen;
model3DS *whiteBishop;
model3DS *blackBishop;
model3DS *whiteKnight;
model3DS *blackKnight;

model3DS *whiteRookB;
model3DS *blackRookB;
model3DS *whiteRookT;
model3DS *blackRookT;
float	rookRotate = 0.0f;

/*####################################################################################################*/



//OpenGL functions:
/*----------------------------------------------------------------------------------------------------*/
void setupScene(){

	cout<<"Initializing scene..."<<endl;
    
	//Set up Lighting Stuff
	glLightfv(GL_LIGHT0, GL_POSITION, left_light_position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
	glLightfv(GL_LIGHT1, GL_POSITION, right_light_position);
	glLightfv(GL_LIGHT1, GL_SPECULAR, white_light);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, white_light);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_DEPTH_TEST);

	glutFullScreen();

	//Menu screens:
	glGenTextures(1, &menuTexture);
	textureBMP menuTexture("Textures/mainMenu.bmp", menuTexture);
	glGenTextures(1, &optionsMenuTexture);
	textureBMP optionsMenuTexture("Textures/optionsMenu.bmp", optionsMenuTexture);

	//Win screens
	glGenTextures(1, &blackWin);
	textureBMP blackWin("Textures/blackWinScreen.bmp", blackWin);
	glGenTextures(1, &whiteWin);
	textureBMP whiteWin("Textures/whiteWinScreen.bmp", whiteWin);

	//SKYBOX TEXTURES	
	glGenTextures(1, &skyBoxBack);
	textureTGA skyBoxBackTexture(texDir+"back.tga", skyBoxBack);
	glGenTextures(1, &skyBoxFront);
	textureTGA skyBoxFrontPlasterTexture(texDir+"front.tga", skyBoxFront);
	glGenTextures(1, &skyBoxLeft);
	textureTGA skyBoxLeftTexture(texDir+"left.tga", skyBoxLeft);
	glGenTextures(1, &skyBoxRight);
	textureTGA skyBoxRightTexture(texDir+"right.tga", skyBoxRight);
	glGenTextures(1, &skyBoxTop);
	textureTGA skyBoxTopTexture(texDir+"top.tga", skyBoxTop);
	glGenTextures(1, &skyBoxFloor);
	textureTGA skyBoxFloorTexture(texDir+"bottom.tga", skyBoxFloor);

	cylQuad = gluNewQuadric();
}

void updateScene(){
	
	// Wait until at least 16ms passed since start of last frame
	// Effectively caps framerate at ~60fps
	while(timeGetTime()-lastTickCount<16);
	lastTickCount=timeGetTime();

	if(!inMenu && winRar == 0){
		// Do any other updates here
		//DISCO
		if(disco){
			int	flicker=5;
			if(lastTickCount % flicker ==0){
				wireframe=!wireframe;
				if(wireframe){
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}
	}	
	// Draw the next frame
    glutPostRedisplay();

}

void renderScene(){

	//Perform key presses, from buffer
	keyOperations();
    
	// Clear framebuffer & depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Reset Modelview matrix      	
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Set view position & direction
	camera();

	//Draw scene if game state has been entered
	if(!inMenu && winRar == 0){
		glPushMatrix();
			drawSkyBox();
			drawBoard();
			drawPieces(&mainBoard);
			drawMarker();
		glPopMatrix();
		if(aiMode && player == BLACK) aiPlayer();
	}
	//Otherwise, someone has won, display win screen:
	else if(!inMenu && winRar != 0){
		GLuint winTexture;
		if(winRar == -1) winTexture = blackWin;
		else winTexture = whiteWin;

		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, winTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
			glTexCoord2f(1,1);glVertex3f(683, 384, -920);
			glTexCoord2f(0,1);glVertex3f(-683, 384, -920);
			glTexCoord2f(0,0);glVertex3f(-683, -384, -920);
			glTexCoord2f(1,0);glVertex3f(683, -384, -920);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	//Otherwise, draw main menu
	else if(!inOptions){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, menuTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
			glTexCoord2f(1,1);glVertex3f(683, 384, -920);
			glTexCoord2f(0,1);glVertex3f(-683, 384, -920);
			glTexCoord2f(0,0);glVertex3f(-683, -384, -920);
			glTexCoord2f(1,0);glVertex3f(683, -384, -920);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
	else if(inOptions){
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, optionsMenuTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glBegin(GL_QUADS);
			glTexCoord2f(1,1);glVertex3f(683, 384, -920);
			glTexCoord2f(0,1);glVertex3f(-683, 384, -920);
			glTexCoord2f(0,0);glVertex3f(-683, -384, -920);
			glTexCoord2f(1,0);glVertex3f(683, -384, -920);
		glEnd();
		glDisable(GL_TEXTURE_2D);
	}
    // Swap double buffer for flicker-free animation
    glutSwapBuffers();
}

void exitScene(){
    cout<<"Exiting scene..."<<endl;

    // Close window
    glutDestroyWindow(windowId);

    // Free any allocated memory
	if(loaded){
		delete whitePawn;
		delete blackPawn;
		delete whiteKing;
		delete blackKing;
		delete whiteQueen;
		delete blackQueen;
		delete whiteBishop;
		delete blackBishop;
		delete whiteKnight;
		delete blackKnight;
		delete whiteRookT;
		delete whiteRookB;
		delete blackRookT;
		delete blackRookB;

	}
    // Exit program
    exit(0);
}

void keypress(unsigned char key, int x, int y){

	if(key==27){	// ESC(Ascii 27) to exit
		exitScene();
	}
	//else if(key == 'p' && inMenu) inMenu = false;
	else if(key == ';' && !inMenu && winRar == 0){	// ';' to turn wireframe on/off
		wireframe=!wireframe;
		if(wireframe){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if(key == 'f' && !inMenu && winRar == 0){	// 'f' to turn full screen on/off
		fullScreen = !fullScreen;
		if(fullScreen){
			glutFullScreen();
		}
		else{
			glutReshapeWindow(win_width,win_height);
			glutPositionWindow(50,50);
		}
	}
	else if(key == '.' && !inMenu && winRar == 0){
		discoSwitch();			
	}
	else if(key == 'm' && !inMenu && winRar == 0){	//Music
		if(!disco && !music){
			PlaySound(L"Sound/OneNightInBangkok.wav", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
			music = true;
		}
		else if(!disco && music){	//Music off
			PlaySound(NULL, NULL, NULL);
			music = false;
		}
	}
	else if(key == 'q' && !inMenu && winRar == 0){
		resetGame();
	}
	else if(!inMenu && winRar == 0) keyStates[key] = true;	//Otherwise, add to key buffer (for simultaneous keypresses)
}

void keyReleased(unsigned char key, int x, int y){
	keyStates[key] = false;	//Key buffer
}

void setViewport(int width, int height) {
    //Work out window ratio, avoid divide-by-zero
    if(height==0) height=1;
	float ratio = float(width)/float(height);

	//Reset projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Fill screen with viewport
	glViewport(0, 0, width, height);

	// Set a 45 degree perspective
	gluPerspective(45, ratio, .1, 30000);
}

void camera(){
    if(!inMenu && winRar == 0){
		//In game, can move camera
		glRotatef(xrot,1.0,0.0,0.0);			//rotate our camera on the x-axis (left and right)
		glRotatef(yrot,0.0,1.0,0.0);			//rotate our camera on the y-axis (up and down)
		glTranslated(-xpos,-viewHeight,-zpos);	//translate the screen to the position of our camera
		//glTranslated(-xpos,-ypos,-zpos);		//Fly mode
	}
	else{
		//In menu, do not move camera from initial point:
		glRotatef(0.0,1.0,0.0,0.0);	//rotate our camera on the x-axis (left and right)
		glRotatef(0.0,0.0,1.0,0.0);	//rotate our camera on the y-axis (up and down)
		glTranslated(0,0,0);		//translate the screen to the position of our camera
	}
}

void keyOperations(){
	//Tilt upwards
	if (keyStates['o']){
		if (xrot-2 > -70) xrot -= 2;	//Limit field of view to +-70 degrees
    }
	//Tilt Down
	if (keyStates['l']){
		if (xrot+2 < 70) xrot += 2;		// ""
    }

	//FORWARD
    if (keyStates['w']){
		float xrotrad, yrotrad;
		yrotrad = (yrot / 180 * piVal);
		xrotrad = (xrot / 180 * piVal); 
		xpos += float(sin(yrotrad)) ;
		zpos -= float(cos(yrotrad)) ;
		ypos -= float(sin(xrotrad)) ;
    }
	//BACK
    if (keyStates['s']){
		float xrotrad, yrotrad;
		yrotrad = (yrot / 180 * piVal);
		xrotrad = (xrot / 180 * piVal);
		xpos -= float(sin(yrotrad));
		zpos += float(cos(yrotrad));
		ypos += float(sin(xrotrad));
    }

	//KEEP WITHIN BOUNDS:
	if(xpos > boundary) xpos = boundary;
	if(xpos < -boundary) xpos = -boundary;
	if(zpos > boundary) zpos = boundary;
	if(zpos < -boundary) zpos = -boundary;
	

	//Pan Left
    if (keyStates['d']){
		yrot += 2;
		if (yrot >360) yrot -= 360;
    }
	//Pan Right
    if (keyStates['a']){
		yrot -= 2;
		if (yrot < -360)yrot += 360;
    }
}

void mouseClicked(int button, int state, int x, int y){
	//Menu operations:
	if(winRar != 0 && button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		resetGame();
	}
	if(inMenu && !inOptions){
		//Left click down
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
			if(x >= 930 && x <= 1261 && y>= 158 && y<= 297){
				//Vs. AI Game
				//****Activate AI****
				aiMode = true;
				inMenu = false;
			}
			else if(x >= 930 && x <= 1261 && y>= 333 && y<= 472){
				//2 Player Game
				inMenu = false;
				aiMode = false;
				cout<<"2"<<endl;
			}
			else if(x >= 930 && x <= 1261 && y>= 509 && y<= 650){
				//Option
				inOptions = true;
			}
		}
	}
	else if(inMenu && inOptions){
		//Options Menu
		if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
			if(x >= 69 && x <= 287 && y>= 166 && y<= 390){
				//Level 1
				texDir = "Textures/rainb/";
			}
			else if(x >= 348 && x <= 569 && y>= 167 && y<= 389){
				//Level 2
				texDir = "Textures/moon/";
			}
			else if(x >= 69 && x <= 284 && y>= 423 && y<= 650){
				//Level 3
				texDir = "Textures/haunt/";
			}
			else if(x >= 347 && x <= 570 && y>= 423 && y<= 650){
				//Level 4
				texDir = "Textures/sand/";
			}
			else if(x >= 700 && x <= 1312 && y>= 173 && y<= 388){
				//Pieces 1
				useModels = false;
			}
			else if(x >= 699 && x <= 1311 && y>= 428 && y<= 645){
				//Pieces 2
				useModels = true;
				if(!loaded) loadModels();
			}
			else if(x >= 371 && x <= 948 && y>= 670 && y<= 751){
				//Return to Menu
				inOptions = false;
			}
			glGenTextures(1, &skyBoxBack);
			textureTGA skyBoxBackTexture(texDir+"back.tga", skyBoxBack);
			glGenTextures(1, &skyBoxFront);
			textureTGA skyBoxFrontPlasterTexture(texDir+"front.tga", skyBoxFront);
			glGenTextures(1, &skyBoxLeft);
			textureTGA skyBoxLeftTexture(texDir+"left.tga", skyBoxLeft);
			glGenTextures(1, &skyBoxRight);
			textureTGA skyBoxRightTexture(texDir+"right.tga", skyBoxRight);
			glGenTextures(1, &skyBoxTop);
			textureTGA skyBoxTopTexture(texDir+"top.tga", skyBoxTop);
			glGenTextures(1, &skyBoxFloor);
			textureTGA skyBoxFloorTexture(texDir+"bottom.tga", skyBoxFloor);
		}
	}
	//Left-click to select/move piece
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) convertMouseCoord(x, y, true);
	//Right-click to deselect currently selected piece
	else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN && selected){
		selected = false;
		iSelected = NULL;
		jSelected = NULL;
		iSelectedCoords = NULL;
		jSelectedCoords = NULL;
	}
}
void mouseHover(int x, int y){
	//Hovering over squares updates position/colour of selection cylinder
	if(!inMenu && winRar == 0) convertMouseCoord(x, y, false);
}
void mouseWheeled(int wheel, int direction, int x, int y){
	if(!inMenu && winRar == 0){
		//Wheel up/down does same thing as keys 'o' and 'l' (but slower)
		if(direction == 1){
			//UP
			if (xrot-1 > -70) xrot -= 1;
		}
		else{
			//DOWN
			if (xrot+1 < 70) xrot += 1;
		}
	}
}

void convertMouseCoord(int x, int y, bool click){
    GLint viewport[4];								// Where The Viewport Values Will Be Stored
    glGetIntegerv(GL_VIEWPORT, viewport);			// Retrieves The Viewport Values (X, Y, Width, Height)
    GLdouble modelview[16];							// Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);	// Retrieve The Modelview Matrix
    GLdouble projection[16];						// Where The 16 Doubles Of The Projection Matrix Are To Be Stored
    glGetDoublev(GL_PROJECTION_MATRIX, projection);	// Retrieve The Projection Matrix
     
    glLoadIdentity();
     
    GLfloat tempx, tempz, tempy;
    GLdouble posx, posz, posy;	//These will hold 3D position of mouse cursor
     
    tempx = (float)x;
	tempy = (float)viewport[3] - (float)y;
     
    glReadPixels(tempx, tempy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &tempz);
    gluUnProject(tempx, tempy, tempz, modelview, projection, viewport, &posx, &posy, &posz);
	
	updateSelectedPos(posx, posz, click);
}

void updateSelectedPos(float x, float z, bool click){
	//
	float boardLeft = -boardWidth+boardBorder;
	float boardRight = boardWidth-boardBorder;
	float boardBack = boardLeft;
	float boardFront = boardRight;

	if(x < boardRight && x > boardLeft && z < boardFront && z > boardBack){
		int iBox, jBox;
		iBox = (x-boardLeft)/squareSize;
		jBox = (z-boardBack)/squareSize;
		float centreFirst = boardLeft + (squareSize/2.0);
		
		iSquareCentre = centreFirst + (iBox*squareSize);
		jSquareCentre = centreFirst + (jBox*squareSize);

		iHover = iBox;
		jHover = jBox;
		
		//Ensure can only select our own piece
		bool ourPiece = (mainBoard.boardArray[iBox][jBox]->currentP > EMPTY && player == BLACK) || (mainBoard.boardArray[iBox][jBox]->currentP < EMPTY && player == WHITE);

		if(click){
			if(!selected && ourPiece){
				selected = true;
				iSelected = iBox;
				jSelected = jBox;
				iSelectedCoords = centreFirst + (iBox*squareSize);
				jSelectedCoords = centreFirst + (jBox*squareSize);
			}
			else if(selected){				
				if(mainBoard.moveMethod(iSelected, jSelected, iHover, jHover, player)){
					winRar = mainBoard.checkKings();
					selected = false;
					iSelected = NULL;
					jSelected = NULL;
					iSelectedCoords = NULL;
					jSelectedCoords = NULL;
					if(player == WHITE){
						player = BLACK;
					}
					else player = WHITE;
				}				
			}
		}
	}
}

/*####################################################################################################*/



//Game/Drawing functions:
/*----------------------------------------------------------------------------------------------------*/

void drawSkyBox(){

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	
	//Front
	glBindTexture(GL_TEXTURE_2D, skyBoxFront);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_QUADS);
		glTexCoord2f(1,1);glVertex3f(skyboxSize, skyboxSize, -skyboxSize);
		glTexCoord2f(0,1);glVertex3f(-skyboxSize, skyboxSize, -skyboxSize);
		glTexCoord2f(0,0);glVertex3f(-skyboxSize, -skyboxSize, -skyboxSize);
		glTexCoord2f(1,0);glVertex3f(skyboxSize, -skyboxSize, -skyboxSize);
	glEnd();
	
	//Left
	glBindTexture(GL_TEXTURE_2D, skyBoxLeft);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_QUADS);
		glTexCoord2f(1,1);glVertex3f(-skyboxSize, skyboxSize, -skyboxSize);
		glTexCoord2f(0,1);glVertex3f(-skyboxSize, skyboxSize, skyboxSize);
		glTexCoord2f(0,0);glVertex3f(-skyboxSize, -skyboxSize, skyboxSize);
		glTexCoord2f(1,0);glVertex3f(-skyboxSize, -skyboxSize, -skyboxSize);
	glEnd();

	//Right
	glBindTexture(GL_TEXTURE_2D, skyBoxRight);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_QUADS);
		glTexCoord2f(1,1);glVertex3f(skyboxSize, skyboxSize, skyboxSize);
		glTexCoord2f(0,1);glVertex3f(skyboxSize, skyboxSize, -skyboxSize);
		glTexCoord2f(0,0);glVertex3f(skyboxSize, -skyboxSize, -skyboxSize);
		glTexCoord2f(1,0);glVertex3f(skyboxSize, -skyboxSize, skyboxSize);
	glEnd();
	
	//Behind
	glBindTexture(GL_TEXTURE_2D, skyBoxBack);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_QUADS);
		glTexCoord2f(1,1);glVertex3f(-skyboxSize, skyboxSize, skyboxSize);
		glTexCoord2f(0,1);glVertex3f(skyboxSize, skyboxSize, skyboxSize);
		glTexCoord2f(0,0);glVertex3f(skyboxSize, -skyboxSize, skyboxSize);
		glTexCoord2f(1,0);glVertex3f(-skyboxSize, -skyboxSize, skyboxSize);
	glEnd();
	
	//Above
	glBindTexture(GL_TEXTURE_2D, skyBoxTop);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_QUADS);
		glTexCoord2f(1,1);glVertex3f(skyboxSize, skyboxSize, skyboxSize);
		glTexCoord2f(0,1);glVertex3f(-skyboxSize, skyboxSize, skyboxSize);
		glTexCoord2f(0,0);glVertex3f(-skyboxSize, skyboxSize, -skyboxSize);
		glTexCoord2f(1,0);glVertex3f(skyboxSize, skyboxSize, -skyboxSize);
	glEnd();

	//Below
	glBindTexture(GL_TEXTURE_2D, skyBoxFloor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBegin(GL_QUADS);
		glTexCoord2f(1,1);glVertex3f(skyboxSize, -skyboxSize, -skyboxSize);
		glTexCoord2f(0,1);glVertex3f(-skyboxSize, -skyboxSize, -skyboxSize);
		glTexCoord2f(0,0);glVertex3f(-skyboxSize, -skyboxSize, skyboxSize);
		glTexCoord2f(1,0);glVertex3f(skyboxSize, -skyboxSize, skyboxSize);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

}

void drawBoard(){

	//Enable lighting
 	glEnable(GL_LIGHTING);

	//Set the material properties
	if(texDir != "Textures/rainb/"){
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, emerald_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, emerald_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, emerald_specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, emerald_shininess);
	}
	else{
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, sapphire_ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, sapphire_diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, sapphire_specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, sapphire_shininess);
	}
	
	//Base
	glBegin(GL_QUADS);
		glVertex3f(-boardWidth,(boardY-boardDepth),-boardWidth);	//2	
		glVertex3f(boardWidth,(boardY-boardDepth),-boardWidth);		//1
		glVertex3f(boardWidth,(boardY-boardDepth),boardWidth);		//4
		glVertex3f(-boardWidth,(boardY-boardDepth),boardWidth);		//3		
	glEnd();

	//Sides:
	glBegin(GL_QUADS);	//Front
		glVertex3f(boardWidth,boardY,boardWidth);
		glVertex3f(-boardWidth,boardY,boardWidth);
		glVertex3f(-boardWidth,(boardY-boardDepth),boardWidth);
		glVertex3f(boardWidth,(boardY-boardDepth),boardWidth);
	glEnd();
	glBegin(GL_QUADS);	//Back
		glVertex3f(-boardWidth,boardY,-boardWidth);
		glVertex3f(boardWidth,boardY,-boardWidth);
		glVertex3f(boardWidth,(boardY-boardDepth),-boardWidth);
		glVertex3f(-boardWidth,(boardY-boardDepth),-boardWidth);
	glEnd();
	glBegin(GL_QUADS);	//Right
		glVertex3f(boardWidth,boardY,-boardWidth);
		glVertex3f(boardWidth,boardY,boardWidth);
		glVertex3f(boardWidth,(boardY-boardDepth),boardWidth);
		glVertex3f(boardWidth,(boardY-boardDepth),-boardWidth);
	glEnd();	
	glBegin(GL_QUADS);	//Left
		glVertex3f(-boardWidth,boardY,boardWidth);
		glVertex3f(-boardWidth,boardY,-boardWidth);
		glVertex3f(-boardWidth,(boardY-boardDepth),-boardWidth);
		glVertex3f(-boardWidth,(boardY-boardDepth),boardWidth);
	glEnd();

	//Top of board:
	glBegin(GL_QUADS);
		glVertex3f(boardWidth,(boardY-0.01f),-boardWidth);		//1
		glVertex3f(-boardWidth,(boardY-0.01f),-boardWidth);		//2
		glVertex3f(-boardWidth,(boardY-0.01f),boardWidth);		//3
		glVertex3f(boardWidth,(boardY-0.01f),boardWidth);		//4
	glEnd();

	glDisable(GL_LIGHTING);

	//Coloured squares:
	float whiteCol = 1.0;
	float blackCol = 0.0;
	float currentCol = whiteCol;	

	float squareX = -boardWidth + boardBorder;
	float squareY = boardY;
	float squareZ = -boardWidth + boardBorder;
	for(int i=0; i<numSquares; i++){
		for(int j=0; j<numSquares; j++){
			glColor3f(currentCol,currentCol,currentCol);
			glBegin(GL_QUADS);
				glVertex3f((squareX+squareSize),boardY,squareZ);
				glVertex3f(squareX,boardY,squareZ);
				glVertex3f(squareX,boardY,(squareZ+squareSize));
				glVertex3f((squareX+squareSize),boardY,(squareZ+squareSize));
			glEnd();
			squareZ += squareSize;
			(currentCol == whiteCol) ? (currentCol = blackCol) : (currentCol = whiteCol);
		}
		squareZ = -boardWidth + boardBorder;
		squareX += squareSize;
		(currentCol == whiteCol) ? (currentCol = blackCol) : (currentCol = whiteCol);
	}
}

void drawPieces(board* b){
	if(!useModels){
		for(int i=0; i < 8; i++){
			for(int j=0; j < 8; j++){
				square tempSq = *(b->boardArray[j][i]);
				piece thisP = tempSq.currentP;
				bool white = false;

				//Check if piece is black/white
				if(thisP < EMPTY) white = true;

				//Set colour based on piece colour
				if(white) glColor3f(0.7f, 0.7f, 0.7f);
				else glColor3f(0.3f,0.3f,0.3f);

				glPushMatrix();
					glTranslatef(tempSq.centreX, (boardY + pieceHeight), tempSq.centreZ);
					switch(thisP){
						case WHITE_KING:
						case BLACK_KING:
							glutSolidTeapot(3.0);
							break;
						case WHITE_QUEEN:
						case BLACK_QUEEN:
							glTranslatef(0.0f, -1.0f, 0.0f);
							glPushMatrix();
								glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
								glutSolidCone(3.0,4.0,40,40);
							glPopMatrix();
							break;
						case WHITE_BISHOP:
						case BLACK_BISHOP:
							glutSolidCube(4.0);
							break;
						case WHITE_KNIGHT:
						case BLACK_KNIGHT:
							glTranslatef(0.0f, 1.5f, 0.0f);
							glPushMatrix();
								glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
								glutWireTorus(1.0,2.5,40,40);
							glPopMatrix();
							break;
						case WHITE_ROOK:
						case BLACK_ROOK:
							glTranslatef(0.0f, 1.5f, 0.0f);
							glutSolidTorus(1.0,2.5,40,40);
							break;
						case WHITE_PAWN:
						case BLACK_PAWN:
							glutSolidSphere(1.25,40,40);
							break;
						default:
							break;
						
					}
				glPopMatrix();
				glColor3f(1.0f,1.0f,1.0f);
			}
		}
	}
	else{
		for(int i=0; i < 8; i++){
			for(int j=0; j < 8; j++){
				square tempSq = *(b->boardArray[j][i]);
				piece thisP = tempSq.currentP;
				
				glPushMatrix();
					glTranslatef(tempSq.centreX, (boardY + modelHeight), tempSq.centreZ);
					glEnable(GL_LIGHTING);
					switch(thisP){
						case WHITE_KING:
							glTranslatef(0.0f, 1.9f, 0.0f);
							whiteKing->draw();
							break;
						case BLACK_KING:
							glTranslatef(0.0f, 1.9f, 0.0f);
							blackKing->draw();
							break;
						case WHITE_QUEEN:
							glTranslatef(0.0f, 0.9f, 0.0f);
							whiteQueen->draw();
							break;
						case BLACK_QUEEN:
							glTranslatef(0.0f, 0.9f, 0.0f);
							blackQueen->draw();
							break;
						case WHITE_BISHOP:
							glTranslatef(0.0f, 0.9f, 0.0f);
							whiteBishop->draw();
							break;
						case BLACK_BISHOP:
							glTranslatef(0.0f, 0.9f, 0.0f);
							blackBishop->draw();
							break;
						case WHITE_KNIGHT:
							glTranslatef(0.0f, 1.1f, 0.0f);
							whiteKnight->draw();
							break;
						case BLACK_KNIGHT:
							glTranslatef(0.0f, 1.1f, 0.0f);
							blackKnight->draw();
							break;
						case WHITE_ROOK:
							whiteRookB->draw();
							glPushMatrix();
								glTranslatef(0.0f,modelHeight,0.0f);
								glRotatef(rookRotate, 0.0f, 1.0f, 0.0f);
								rookRotate += 0.4;
								whiteRookT->draw();
							glPopMatrix();
							break;
						case BLACK_ROOK:
							blackRookB->draw();
							glPushMatrix();
								glTranslatef(0.0f,modelHeight,0.0f);
								glRotatef(rookRotate, 0.0f, 1.0f, 0.0f);
								rookRotate += 0.4;
								blackRookT->draw();
							glPopMatrix();
							break;
						case WHITE_PAWN:
							glTranslatef(0.0f,-1.5f,0.0f);
							whitePawn->draw();
							break;
						case BLACK_PAWN:
							glTranslatef(0.0f,-1.5f,0.0f);
							blackPawn->draw();
							break;
						default:
							break;						
					}
				glDisable(GL_LIGHTING);
				glPopMatrix();
			}
		}
	}
}

void drawMarker(){
	if(selected && iSelectedCoords != NULL && jSelectedCoords != NULL){
		glColor3f(0.0f, 0.0f, 1.0f);
		glPushMatrix();
			glTranslatef(iSelectedCoords, (boardY+0.2), jSelectedCoords);
			glPushMatrix();
				glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
				gluCylinder(cylQuad, 4.0, 4.0, 0.3f, 40, 40);
			glPopMatrix();
		glPopMatrix();
	}

	if(iSquareCentre != NULL && jSquareCentre != NULL && iHover != -1 && jHover != -1){
		
		//Green marker if on your piece (when no piece yet selected), red marker otherwise
		square test = *(mainBoard.boardArray[iHover][jHover]);
		if(!selected){
			if((test.currentP < EMPTY && player == WHITE) || (test.currentP > EMPTY && player == BLACK)) glColor3f(0.0f, 1.0f, 0.0f);
			else glColor3f(1.0f, 0.0f, 0.0f);
		}
		else{
			//Green marker when on valid point to move to
			if(mainBoard.isValidMove(*(mainBoard.boardArray[iSelected][jSelected]), *(mainBoard.boardArray[iHover][jHover]), player)) glColor3f(0.0f, 1.0f, 0.0f);
			else glColor3f(1.0f, 0.0f, 0.0f);
		}

		glPushMatrix();
			glTranslatef(iSquareCentre, (boardY+0.2), jSquareCentre);
			glPushMatrix();
				glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
				gluCylinder(cylQuad, 4.0, 4.0, 0.3f, 40, 40);
			glPopMatrix();
		glPopMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
}

void discoSwitch(){
	disco = !disco;
	if(disco){
		PlaySound(L"Sound/BangkokRemix.wav", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
		string discoTex = "Textures/tron/";
		glGenTextures(1, &skyBoxBack);
		textureTGA skyBoxBackTexture(discoTex+"back.tga", skyBoxBack);
		glGenTextures(1, &skyBoxFront);
		textureTGA skyBoxFrontPlasterTexture(discoTex+"front.tga", skyBoxFront);
		glGenTextures(1, &skyBoxLeft);
		textureTGA skyBoxLeftTexture(discoTex+"left.tga", skyBoxLeft);
		glGenTextures(1, &skyBoxRight);
		textureTGA skyBoxRightTexture(discoTex+"right.tga", skyBoxRight);
		glGenTextures(1, &skyBoxTop);
		textureTGA skyBoxTopTexture(discoTex+"top.tga", skyBoxTop);
		glGenTextures(1, &skyBoxFloor);
		textureTGA skyBoxFloorTexture(discoTex+"bottom.tga", skyBoxFloor);
	}
	else{
		if(music) PlaySound(L"Sound/OneNightInBangkok.wav", NULL, SND_ASYNC|SND_FILENAME|SND_LOOP);
		else PlaySound(NULL, NULL, NULL);
		glGenTextures(1, &skyBoxBack);
		textureTGA skyBoxBackTexture(texDir+"back.tga", skyBoxBack);
		glGenTextures(1, &skyBoxFront);
		textureTGA skyBoxFrontPlasterTexture(texDir+"front.tga", skyBoxFront);
		glGenTextures(1, &skyBoxLeft);
		textureTGA skyBoxLeftTexture(texDir+"left.tga", skyBoxLeft);
		glGenTextures(1, &skyBoxRight);
		textureTGA skyBoxRightTexture(texDir+"right.tga", skyBoxRight);
		glGenTextures(1, &skyBoxTop);
		textureTGA skyBoxTopTexture(texDir+"top.tga", skyBoxTop);
		glGenTextures(1, &skyBoxFloor);
		textureTGA skyBoxFloorTexture(texDir+"bottom.tga", skyBoxFloor);
		if(wireframe){
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe = false;
		}
	}
}

void resetGame(){
	//Variables for music etc
	inMenu = true;
	music = false;
	PlaySound(NULL, NULL, NULL);
	if(disco){
		discoSwitch();
	}
	if(wireframe){
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wireframe = false;
	}
	//Game state
	mainBoard = board(startXZ, startXZ, squareSize);
	winRar = 0;
	player = WHITE;
}

void loadModels(){
	whitePawn = new model3DS("pieces//White Pieces//whitePawn.3DS", 0.15);
	blackPawn = new model3DS("pieces//Black Pieces//blackPawn.3DS", 0.15);

	whiteKing = new model3DS("pieces//White Pieces//whiteKing.3DS", 0.2);
	blackKing = new model3DS("pieces//Black Pieces//blackKing.3DS", 0.2);	

	whiteQueen = new model3DS("pieces//White Pieces//whiteQueen.3DS", 0.2);
	blackQueen = new model3DS("pieces//Black Pieces//blackQueen.3DS", 0.2);
	
	whiteBishop = new model3DS("pieces//White Pieces//whiteBishop.3DS", 0.2);
	blackBishop = new model3DS("pieces//Black Pieces//blackBishop.3DS", 0.2);

	whiteKnight = new model3DS("pieces//White Pieces//whiteKnight.3DS", 0.1);
	blackKnight = new model3DS("pieces//Black Pieces//blackKnight.3DS", 0.1);
		
	whiteRookB = new model3DS("pieces//White Pieces//whiteRookBottom.3DS", 0.2);
	whiteRookT = new model3DS("pieces//White Pieces//whiteRookTop.3DS", 0.2);
	blackRookB = new model3DS("pieces//Black Pieces//blackRookBottom.3DS", 0.2);
	blackRookT = new model3DS("pieces//Black Pieces//blackRookTop.3DS", 0.2);

	loaded = true;
}

void aiPlayer(){
	//Generates square to move to, ensures legal move
	int iFrom=7,jFrom=7,iTo=7,jTo=7;
	vector<square> aiPieces;

	//Find all black pieces:
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			if(mainBoard.boardArray[i][j]->currentP > EMPTY) aiPieces.push_back(*mainBoard.boardArray[i][j]);
		}
	}

	do{
		
		srand((unsigned)time(0));
		int selectedSquare = rand()%(aiPieces.size()-1);
		square fromSquare = aiPieces.at(selectedSquare);
		iFrom = fromSquare.iVal;
		jFrom = fromSquare.jVal;

		for(int i=0; i<10; i++){
			if(fromSquare.currentP == BLACK_BISHOP || fromSquare.currentP == BLACK_QUEEN){
				iTo = rand()%8;
				jTo = rand()%8;
				if(mainBoard.isValidMove(*mainBoard.boardArray[iFrom][jFrom], *mainBoard.boardArray[iTo][jTo], BLACK)) break;
			}
			else{
				iTo = rand()%8;
				jTo = jFrom + 1;
				if(mainBoard.isValidMove(*mainBoard.boardArray[iFrom][jFrom], *mainBoard.boardArray[iTo][jTo], BLACK)) break;
			}
		}

	}while(!mainBoard.isValidMove(*mainBoard.boardArray[iFrom][jFrom], *mainBoard.boardArray[iTo][jTo], BLACK));

	mainBoard.moveMethod(iFrom, jFrom, iTo, jTo, BLACK);
	player = WHITE;

	winRar = mainBoard.checkKings();
}

/*####################################################################################################*/



//Main function body:
/*----------------------------------------------------------------------------------------------------*/
int main(int argc, char** argv){

	//Initialise game board
	startXZ = (-boardWidth + boardBorder)+(squareSize/2.0);
	mainBoard = board(startXZ, startXZ, squareSize);

	// Initialise OpenGL
    glutInit(&argc, argv); 
	
    // Set window position, size & create window
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(50,50);
    glutInitWindowSize(win_width,win_height);

	windowId = glutCreateWindow("Chess: Lab 5");
    
    // Set GLUT callback functions
    glutReshapeFunc(setViewport);
    glutDisplayFunc(renderScene);
    glutIdleFunc(updateScene);

    glutKeyboardFunc(keypress);
	glutKeyboardUpFunc(keyReleased);
	glutMouseFunc(mouseClicked);
	glutMouseWheelFunc(mouseWheeled);
	
	glutPassiveMotionFunc(mouseHover);

    // Setup OpenGL state & scene resources (models, textures etc)
    setupScene();
	
    // Show window & start update loop
    glutMainLoop();

    return 0;
}
/*####################################################################################################*/

