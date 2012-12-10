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
			boardArray[i][j] = new square(xPos,zPos,i,j);
			zPos += squareWidth;
		}
		xPos += squareWidth;
		zPos = startZ;
	}
}
board::~board(){}

//Check if move from square to square is valid (checks piece on from square for rules, and to ensure to square is empty or is takeable)
bool board::isValidMove(square from, square to, playerColour myCol){

	//Two pieces:
	piece fromPiece = from.currentP;
	piece destPiece = to.currentP;

	//No piece at starting square
	if(fromPiece == EMPTY) return false;

	//If it isn't our piece, false
	if((fromPiece > EMPTY && myCol == WHITE) || (fromPiece < EMPTY && myCol == BLACK)) return false;
	
	//Our own piece at destination, cannot take
	if(destPiece != EMPTY && ((fromPiece < EMPTY) == (destPiece < EMPTY))) return false;

	//**CHECK IF WE ARE PUTTING OURSELVES IN CHECK**

	//CASTLING?
	//EN PASSANT?

	//Valid move otherwise, as long as piece can move to that square
	return pieceMoveRules(from, to);
}

bool board::pieceMoveRules(square from, square to){
	piece thisPiece = from.currentP;
	
	//Movement
	int iDelta = to.iVal - from.iVal;
	int jDelta = to.jVal - from.jVal;
	//Direction
	bool forwards = false;
	if(thisPiece > EMPTY){
		forwards = jDelta > 0;	//BLACK
	}
	else{
		forwards = jDelta < 0;	//WHITE
		if(forwards) jDelta *= -1;
	}

	//Null move not allowed
	if(iDelta == 0 && jDelta == 0) return false;

	switch(thisPiece){

		//Pawns:
		case BLACK_PAWN:
		case WHITE_PAWN:
			//Can't move backwards, can't move more than one space
			if(!forwards) return false;

			//Starting double move onto empty square:
			if(jDelta == 2 && iDelta == 0 && to.currentP == EMPTY){
				if(thisPiece == BLACK_PAWN){	//jDelta was positive
					if(from.jVal == 1 && boardArray[from.iVal][2]->currentP == EMPTY) return true;	//If square in between is empty
				}
				else{
					if(from.jVal == 6 && boardArray[from.iVal][5]->currentP == EMPTY) return true;	//""
				}
			}
			
			//Otherwise can only move one space
			if(jDelta > 1) return false;

			//Allow to move forward one and not to the side if into empty square
			if(jDelta == 1 && iDelta == 0 && to.currentP == EMPTY) return true;
			//Allow to move one forwards and one to the side if taking a piece
			if(jDelta == 1 && (iDelta == 1 || iDelta == -1) && to.currentP != EMPTY) return true;
			//Otherwise, not allowed
			else return false;
			break;

		//Rooks:
		case BLACK_ROOK:
		case WHITE_ROOK:
			if(checkLineOfSight(from, to)){
				if(jDelta != 0) return iDelta == 0;	//If moves on the j axis, cannot move on i axis as well
				else if(iDelta != 0) return true;
				else return false;
			}
			else return false;
			break;

		//Knights:
		case BLACK_KNIGHT:
		case WHITE_KNIGHT:
			if(jDelta == 1 && (iDelta == 2 || iDelta == -2)) return true;
			else if((iDelta == 1 || iDelta == -1) && jDelta == 2) return true;			
			else return false;
			break;

		//Bishops:
		case BLACK_BISHOP:
		case WHITE_BISHOP:
			if(checkLineOfSight(from, to)){
				if(jDelta == iDelta || jDelta == -iDelta){
					//CHECK LINE OF SIGHT
					return true;
				}
				else return false;
			}
			else return false;
			break;

		//Queens:
		case BLACK_QUEEN:
		case WHITE_QUEEN:
			if(checkLineOfSight(from, to)){
				if(jDelta == iDelta || jDelta == -iDelta) return true;	//Bishop motion
				else if(jDelta != 0) return iDelta == 0;				//Rook motion
				else if(iDelta != 0) return jDelta == 0;				//""
				else return false;
			}
			else return false;
			break;

		//Kings:
		case BLACK_KING:
		case WHITE_KING:
			if(jDelta > 1 || iDelta > 1 || iDelta < -1) return false;
			else return true;
			break;
	}
}

bool board::moveMethod(int fromIVal, int fromJVal, int toIVal, int toJVal, playerColour myCol){
	square from = *boardArray[fromIVal][fromJVal];
	square to = *boardArray[toIVal][toJVal];

	if(isValidMove(from, to, myCol)){
		boardArray[toIVal][toJVal]->currentP = boardArray[fromIVal][fromJVal]->currentP;
		boardArray[fromIVal][fromJVal]->currentP = EMPTY;
		return true;
	}
	else return false;
}

bool board::checkLineOfSight(square from, square to){
	int iFrom = from.iVal;
	int jFrom = from.jVal;
	int iTo = to.iVal;
	int jTo = to.jVal;

	if(iTo == iFrom || jTo == jFrom){
		//Straight line movement, left/right or forward/backward
		if(iTo == iFrom){
			if(jTo > jFrom){
				for(int j=jTo-1; j > jFrom; j--) if(boardArray[iTo][j]->currentP != EMPTY) return false;
				return true;
			}
			else if(jTo < jFrom){
				for(int j=jTo+1; j < jFrom; j++) if(boardArray[iTo][j]->currentP != EMPTY) return false;
				return true;
			}
			else return false;
		}
		else if(jTo == jFrom){
			if(iTo > iFrom){
				for(int i=iTo-1; i > iFrom; i--) if(boardArray[i][jTo]->currentP != EMPTY) return false;
				return true;
			}
			else if(iTo < iFrom){
				for(int i=iTo+1; i < iFrom; i++) if(boardArray[i][jTo]->currentP != EMPTY) return false;
				return true;
			}
			else return false;
		}
		else return false;
	}
	else if(iTo - iFrom == jTo - jFrom || iFrom - iTo == jTo - jFrom){
		//diagonal movement
		int iMove, jMove;
		if(iTo - iFrom > 0) iMove = 1;
		else iMove = -1;
		if(jTo - jFrom > 0) jMove = 1;
		else jMove = -1;

		for(int i=iFrom+iMove, j=jFrom+jMove; i!=iTo && j!= jTo; i+=iMove,j+=jMove){
			if(boardArray[i][j]->currentP != EMPTY) return false;
		}
		return true;
	}
	else return false;
}

int board::checkKings(){
	bool whiteKing = false;
	bool blackKing = false;

	//Loop across board, if one king not found, return +/- 1 depending on black/white missing
	for(int i=0; i<8; i++){
		for(int j=0; j<8; j++){
			if(boardArray[i][j]->currentP == BLACK_KING){
				blackKing = true;
				if(whiteKing) return 0;
			}
			else if(boardArray[i][j]->currentP == WHITE_KING){
				whiteKing = true;
				if(blackKing) return 0;
			}
		}
	}

	//If both found, return 0, otherwise +/-1 for black/white
	if(whiteKing == false) return -1;	//Black wins, -1
	else return 1;						//White wins, +1
}

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

//Default constructor/destructor
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