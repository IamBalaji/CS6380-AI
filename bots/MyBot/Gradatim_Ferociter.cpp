/*
* @file botTemplate.cpp
* @author Arun Tejasvi Chaganty <arunchaganty@gmail.com>
* @date 2010-02-04
* Template for users to create their own bots
*/

#include "Othello.h"
#include "OthelloBoard.h"
#include "OthelloPlayer.h"
#include <cstdlib>
#include <cstdio>
using namespace std;
using namespace Desdemona;

#define K_PLY 8
#define MIN -400
#define MAX 400
Move MyFinalMove(-1,-1);
OthelloBoard CurrentBoard;
Turn MyTurn;
int boardScore[10][10] =
{
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
{ 0, 65, -3, 6, 4, 4, 6, -3, 65, 0 },
{ 0, -3, -29, 3, 1, 1, 3, -29, -3, 0 },
{ 0, 6, 3, 5, 3, 3, 5, 3, 6, 0 },
{ 0, 4, 1, 3, 1, 1, 3, 1, 4, 0 },
{ 0, 4, 1, 3, 1, 1, 3, 1, 4, 0 },
{ 0, 6, 3, 5, 3, 3, 5, 3, 6, 0 },
{ 0, -3, -29, 3, 1, 1, 3, -29, -3, 0 },
{ 0, 65, -3, 6, 4, 4, 6, -3, 65, 0 },
{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } 
};
//Nodes of the MINIMAX Tree will be 'node'
class minimaxNode
{
    
    public:
        Turn minimaxNodeType;
        OthelloBoard minimaxNodeBoard;
        //constructor
        minimaxNode(OthelloBoard minimaxNodeBoard,Turn minimaxNodeType);
        void changeTheBoard(Move move);
        //deafault destructor
        ~minimaxNode();
};
minimaxNode::minimaxNode(OthelloBoard minimaxNodeBoard, Turn minimaxNodeType)
{
    this->minimaxNodeBoard = minimaxNodeBoard;
    this->minimaxNodeType= minimaxNodeType;
}
void minimaxNode::changeTheBoard(Move move)
{
    (this->minimaxNodeBoard).makeMove(other(this->minimaxNodeType),move);
}
////

int evaluationFunction(minimaxNode* node)
{
    Turn minimaxNodeType_current = node->minimaxNodeType;
    OthelloBoard minimaxNodeBoard_current = node->minimaxNodeBoard;

    int evalValueOfnode = 0;
    //Currently handling positional part of evaluation function.
    for(int i = 1;i<=8;i++) // range of column is 1 to 8
    {
        for(int j = 1;j<=8;j++) //range of row is 1 to 8
        {
            if(minimaxNodeBoard_current.get(i,j)==minimaxNodeType_current)
                evalValueOfnode = evalValueOfnode + boardScore[i][j];
            else if(minimaxNodeBoard_current.get(i,j)==other(minimaxNodeType_current))
                evalValueOfnode = evalValueOfnode - boardScore[i][j];
        }
    }
    return evalValueOfnode;
}

//Alpha-Beta minimax algorithm
int Alpha_Beta_MiniMax(minimaxNode* treeRoot, int Alpha_min_val,int Beta_max_val, int currDepth)
{
    if(currDepth==K_PLY) // handling base case//
        return evaluationFunction(treeRoot);
    //moves that can be made given the turn and board config
    list<Move> validMoves = (treeRoot->minimaxNodeBoard).getValidMoves(treeRoot->minimaxNodeType);

    //if the valid moves are empty return the difference of coins.
    if(validMoves.empty()==true) 
    {
        if(treeRoot->minimaxNodeType==RED)
            return (treeRoot->minimaxNodeBoard).getRedCount()-(treeRoot->minimaxNodeBoard).getBlackCount();
        else return (treeRoot->minimaxNodeBoard).getBlackCount()-(treeRoot->minimaxNodeBoard).getRedCount();
    }
    list<Move>::iterator itr;
    minimaxNode* nd = NULL;
    int val;
    for(itr = validMoves.begin();itr!=validMoves.end();itr++)
    {
      nd = new minimaxNode(treeRoot->minimaxNodeBoard,other(treeRoot->minimaxNodeType));
      nd->changeTheBoard(*itr);
      val = Alpha_Beta_MiniMax(nd,Alpha_min_val, Beta_max_val,currDepth+1);
      if(treeRoot->minimaxNodeType==BLACK)
      {
         if(val>Alpha_min_val)
         {
            Alpha_min_val = val;
            if(currDepth==0)
            {
                MyFinalMove.x = itr->x;
                MyFinalMove.y = itr->y;
            }
         }
            
         if(Alpha_min_val>=Beta_max_val) //Beta Cutoff
            return Beta_max_val;
      }
      else
      {
        if(val<Beta_max_val)
        {
            Beta_max_val = val;
            if(currDepth==0)
            {
                MyFinalMove.x = itr->x;
                MyFinalMove.y = itr->y;
            }
        }
        if(Alpha_min_val >= Beta_max_val) //Alpha Cutoff
            return Alpha_min_val;
      }
    }
    if(treeRoot->minimaxNodeType==BLACK)
        return Alpha_min_val;
    else
        return Beta_max_val;
}
class Gradatim_Ferociter: public OthelloPlayer
{
    public:
        /**
         * Initialisation routines here
         * This could do anything from open up a cache of "best moves" to
         * spawning a background processing thread. 
         */
        Gradatim_Ferociter( Turn turn );

        /**
         * Play something 
         */
        virtual Move play( const OthelloBoard& board );
    private:
};

Gradatim_Ferociter::Gradatim_Ferociter( Turn turn )
    : OthelloPlayer( turn )
{
}

Move Gradatim_Ferociter::play( const OthelloBoard& board )
{
    // list<Move> moves = board.getValidMoves( turn );
    // int randNo = rand() % moves.size();
    // list<Move>::iterator it = moves.begin();

    // return *it;

    minimaxNode* root = new minimaxNode(board,turn);
    //Do-Kply search and get the final move
    Alpha_Beta_MiniMax(root,MIN,MAX,0);
    return MyFinalMove;

}

// The following lines are _very_ important to create a bot module for Desdemona

extern "C" {
    OthelloPlayer* createBot( Turn turn )
    {
        return new Gradatim_Ferociter( turn );
    }

    void destroyBot( OthelloPlayer* bot )
    {
        delete bot;
    }
}


