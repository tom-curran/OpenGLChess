// chessGameLab5.cpp : Defines the entry point for the console application.
//
#include <windows.h>	// for timeGetTime()
#include <mmsystem.h>	// ditto
#include <iostream>		// I/O
#include <GL/freeglut.h>	// GLUT

//#include "model3DS.h" // 3DS model support
#include "textureTGA.h" // TGA support

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
void convertMouseCoord(int x, int y);
void updateSelectedPos(float x, float z);

//Game/Drawing functions:
void drawSkyBox();
void drawBoard();
void drawPieces(board* b);
void drawMarker();

/*####################################################################################################*/



//Constants:
/*----------------------------------------------------------------------------------------------------*/

int		win_width=640;
int		win_height=480;
float	piVal = 3.141592654f;
float	skyboxSize = 800.0f;


//For board drawing:
board	mainBoard;
float	boardWidth = 50.0;
float	boundary = boardWidth*4.0f;
float	boardDepth = boardWidth/10.0f;
float	boardY = 0.0;
float	boardBorder = 5.0;
int		numSquares = 8;
float	squareSize = ((boardWidth*2)-(boardBorder*2))/(float)numSquares;
float	pieceHeight = 2.0;

GLUquadric* cylQuad;
float	iSquareCentre = -5000.0f;
float	jSquareCentre = -5000.0f;

//Bools
bool		wireframe=false;
bool		fullScreen=true;

//?
int         windowId;
DWORD		lastTickCount;

//Lights/Light positions
GLfloat white_light[] = {1.0, 1.0, 1.0, 1.0};
GLfloat left_light_position[] = {1,0,-1, 1.0}; 
GLfloat right_light_position[] = {-1,0,-1, 1.0};

//Camera
float xpos = 0, ypos = 0, zpos = 0, xrot = 0, yrot = 0, angle=0.0;
float viewHeight = 25.0;

//Controlling
bool myArray[256] = {false};
bool* keyStates = myArray;	//For multiple simultaneous keypress
bool	selected = false;
int		selectedI = 0;
int		selectedJ = 0;
playerColour player = WHITE;

//Position array
//float

//Skybox textures
GLuint		skyBoxBack;
GLuint		skyBoxFront;
GLuint		skyBoxLeft;
GLuint		skyBoxRight;
GLuint		skyBoxTop;
GLuint		skyBoxFloor;

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

	//SKYBOX TEXTURES
	string texDir = "C:/Users/Tom/documents/visual studio 2010/Textures/rainb/";
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

	// Do any other updates here
	
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
	// (Camera at (0,0,5) looking down the negative Z-axis)
	//gluLookAt(0,1,5,  0,0,-1,  0,1,0);

	//glEnable(GL_TEXTURE_2D);
	
	//glColor3f(1.0f,1.0f,0.0f);

	glPushMatrix();
		drawSkyBox();
		drawBoard();
		drawPieces(&mainBoard);
        drawMarker();
	glPopMatrix();


	//glDisable(GL_TEXTURE_2D);
	//glPushMatrix();
	//glPopMatrix();
   
 
    // Swap double buffer for flicker-free animation
    glutSwapBuffers();
}

void exitScene(){
    cout<<"Exiting scene..."<<endl;

    // Close window
    glutDestroyWindow(windowId);

    // Free any allocated memory

    // Exit program
    exit(0);
}

void keypress(unsigned char key, int x, int y){

	if(key==27){	// ESC(Ascii 27) to exit
		exitScene();
	}
	else if(key == ';'){	// ';' to turn wireframe on/off
		wireframe=!wireframe;
		if(wireframe){
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if(key == 'f'){	// 'f' to turn full screen on/off
		fullScreen = !fullScreen;
		if(fullScreen){
			glutFullScreen();
		}
		else{
			glutReshapeWindow(win_width,win_height);
			glutPositionWindow(50,50);
		}
	}
    else if(key == 'b'){	// 'b' to test move method
		mainBoard.moveMethod(0,1,0,2, BLACK);
	}
	else keyStates[key] = true;	// Otherwise, add to key buffer (for simultaneous keypresses)
}

void keyReleased(unsigned char key, int x, int y){
	keyStates[key] = false;	//Key buffer
}

void setViewport(int width, int height) {
    // Work out window ratio, avoid divide-by-zero
    if(height==0) height=1;
	float ratio = float(width)/float(height);

	// Reset projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	// Fill screen with viewport
	glViewport(0, 0, width, height);

	// Set a 45 degree perspective
	gluPerspective(45, ratio, .1, 30000);
}

void camera(){
    glRotatef(xrot,1.0,0.0,0.0);  //rotate our camera on the x-axis (left and right)
    glRotatef(yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
	glTranslated(-xpos,-viewHeight,-zpos); //translate the screen to the position of our camera
    //glTranslated(-xpos,-ypos,-zpos); //allows up/down movement
}

void keyOperations(){
	//Tilt upwards
	if (keyStates['o']){
		/*xrot -= 1;
		if (xrot < -360) xrot += 360;*/
		if (xrot-2 > -70) xrot -= 2;
    }
	//Tilt Down
	if (keyStates['l']){
		/*xrot += 1;
		if (xrot >360) xrot -= 360;*/
		if (xrot+2 < 70) xrot += 2;
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
	//convertMouseCoord(x, y);
}
void mouseHover(int x, int y){
	convertMouseCoord(x, y);
}

void convertMouseCoord(int x, int y){
    GLint viewport[4];								// Where The Viewport Values Will Be Stored
    glGetIntegerv(GL_VIEWPORT, viewport);			// Retrieves The Viewport Values (X, Y, Width, Height)
    GLdouble modelview[16];							// Where The 16 Doubles Of The Modelview Matrix Are To Be Stored
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);	// Retrieve The Modelview Matrix
    GLdouble projection[16];						// Where The 16 Doubles Of The Projection Matrix Are To Be Stored
    glGetDoublev(GL_PROJECTION_MATRIX, projection);	// Retrieve The Projection Matrix
     
    glLoadIdentity();
     
    GLfloat tempx, tempz, tempy;
    GLdouble posx, posz, posy;
     
    tempx = (float)x;
	tempy = (float)viewport[3] - (float)y;
     
    glReadPixels(tempx, tempy, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &tempz);
    gluUnProject( tempx, tempy, tempz, modelview, projection, viewport, &posx, &posy, &posz);
	
	updateSelectedPos(posx, posz);

    //posx = true x coordinate, posy = true y coordinate, posz = true z coordinate
}

void updateSelectedPos(float x, float z){
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
		
		//cout<<"POSX: "<<x<<", i: "<<iBox<<"; POSZ: "<<z<<", j: "<<jBox<<endl;
		//cout<<"ISQUARECENTRE: "<<iSquareCentre<<" JSQUARECENTRE: "<<jSquareCentre<<endl;
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
	
	glColor3f(0.0f,1.0f,1.0f);
	
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
	glColor3f(1.0f,0.0f,0.0f);
	glBegin(GL_QUADS);
		glVertex3f(boardWidth,(boardY-0.01f),-boardWidth);		//1
		glVertex3f(-boardWidth,(boardY-0.01f),-boardWidth);		//2
		glVertex3f(-boardWidth,(boardY-0.01f),boardWidth);		//3
		glVertex3f(boardWidth,(boardY-0.01f),boardWidth);		//4
	glEnd();

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

void drawMarker(){
	glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
		glTranslatef(iSquareCentre, (boardY+0.2), jSquareCentre);
		glPushMatrix();
			glRotatef(-90.0, 1.0f, 0.0f, 0.0f);
			gluCylinder(cylQuad, 4.0, 4.0, 0.3f, 40, 40);
		glPopMatrix();
	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
}

/*####################################################################################################*/



//Main function body:
/*----------------------------------------------------------------------------------------------------*/
int main(int argc, char** argv){

	//Initialise game board
	float startXZ = (-boardWidth + boardBorder)+(squareSize/2.0);
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
	glutPassiveMotionFunc(mouseHover);

    // Setup OpenGL state & scene resources (models, textures etc)
    setupScene();

    // Show window & start update loop
    glutMainLoop();

    return 0;
}
/*####################################################################################################*/

