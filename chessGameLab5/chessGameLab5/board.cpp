#include "board.h"

//BOARD
/*----------------------------------------------------------------------------------------------------*/
//Board constructor
board::board(){}
board::board(float startX, float startZ, float squareWidth){
	//2D array of squares
	float xPos = startX;
	float zPos = startZ;

	//Iterate across board building each square:
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			//square* x = new square(xPos,zPos,i,j);
			boardArray[i][j] = new square(xPos,zPos,i,j);
			zPos += squareWidth;
		}
		xPos += squareWidth;
		zPos = startZ;
	}
}
board::~board(){}

void board::printBoard(){
	using namespace std;
	for(int i=0; i < 8; i++){
		for(int j=0; j < 8; j++){
			square tempSq = *boardArray[j][i];
			char pr;
			switch(tempSq.currentP){
				case WHITE_PAWN:
					pr = 'P';
					break;
				case WHITE_ROOK:
					pr = 'R';
					break;
				case WHITE_KNIGHT:
					pr = 'N';
					break;
				case WHITE_BISHOP:
					pr = 'B';
					break;
				case WHITE_QUEEN:
					pr = 'Q';
					break;
				case WHITE_KING:
					pr = 'K';
					break;
				case EMPTY:
					pr = ' ';
					break;
				case BLACK_PAWN:
					pr = 'p';
					break;
				case BLACK_ROOK:
					pr = 'r';
					break;
				case BLACK_KNIGHT:
					pr = 'n';
					break;
				case BLACK_BISHOP:
					pr = 'b';
					break;
				case BLACK_QUEEN:
					pr = 'q';
					break;
				case BLACK_KING:
					pr = 'k';
					break;
			}

			cout<<" "<<pr;
		}
		cout<<"\n";
	}
}

/*####################################################################################################*/


//SQUARE
/*----------------------------------------------------------------------------------------------------*/
//Default constructor
square::square(){}
square::~square(){}

//Main constructor, takes in positions
square::square(float dimX, float dimZ, int i, int j){
	
	//3d and board representations of position stored:
	centreX = dimX;	centreZ = dimZ;
	iVal = i; jVal = j;

	//representation on board (conventional way, a4, b3, etc)
	numAxis = 8-j;
	letterAxis = 'a' + i;

	currentP = getStartingPiece();
}

piece square::getStartingPiece(){
	//Empty Space
	if(numAxis < 7 && numAxis > 2) return EMPTY;
	
	//Pawns
	if(numAxis == 7) return BLACK_PAWN;
	if(numAxis == 2) return WHITE_PAWN;

	//Royalty
	if(numAxis == 8){
		if(letterAxis == 'a' || letterAxis == 'h') return BLACK_ROOK;
		if(letterAxis == 'b' || letterAxis == 'g') return BLACK_KNIGHT;
		if(letterAxis == 'c' || letterAxis == 'f') return BLACK_BISHOP;
		if(letterAxis == 'd') return BLACK_QUEEN;
		//if(letterAxis == 'e') return BLACK_KING;
		else return BLACK_KING;
	}
	else{
		if(letterAxis == 'a' || letterAxis == 'h') return WHITE_ROOK;
		if(letterAxis == 'b' || letterAxis == 'g') return WHITE_KNIGHT;
		if(letterAxis == 'c' || letterAxis == 'f') return WHITE_BISHOP;
		if(letterAxis == 'd') return WHITE_QUEEN;
		//if(letterAxis == 'e') return WHITE_KING;
		else return WHITE_KING;
	}
}

/*####################################################################################################*/