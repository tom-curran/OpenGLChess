#include <string>
//#include <sstream>
#include <iostream>

using namespace std;

/*----------------------------------------------------------------------------------------------------*/
class board;
class square;
enum piece;
/*####################################################################################################*/

//Squares
/*----------------------------------------------------------------------------------------------------*/
class square{
public:

	//Actual positions in space:
	float centreX;
	float centreZ;
	//Integer points on board matrix
	int iVal,jVal;

	//Square representation on board e.g a4
	int numAxis;
	char letterAxis;

	//Piece currently on square
	piece currentP;

	//Constructors/Destructors
	square();
	square(float dimX, float dimZ, int i, int j);	//i and j used to determine numAxis and letterAxis (stored in iVal, jVal)	
	~square();

private:
	piece getStartingPiece();
};
/*####################################################################################################*/

//Pieces
/*----------------------------------------------------------------------------------------------------*/
enum piece{
			WHITE_PAWN,
			WHITE_ROOK,
			WHITE_KNIGHT,
			WHITE_BISHOP,	//WHITE
			WHITE_QUEEN,
			WHITE_KING,
			EMPTY,			//EMPTY
			BLACK_PAWN,
			BLACK_ROOK,
			BLACK_KNIGHT,
			BLACK_BISHOP,	//BLACK
			BLACK_QUEEN,
			BLACK_KING
};
/*####################################################################################################*/

//Board
/*----------------------------------------------------------------------------------------------------*/
class board{
public:
	//2D array of squares
	square* boardArray[8][8];

	//Constructors/Destructors
	board();
	board(float startX, float startZ, float squareWidth);
	~board();

	//Methods
	//void addCoord(float x, float z, int squareX, int squareZ);
	void printBoard();
};
/*####################################################################################################*/