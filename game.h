#ifndef GAME_H_
#define GAME_H_
#include <vector>
#include <queue>
#include <stack>
#include "io.h"
struct Node;


// keeps track of score for rows and columns
// probably don't need this, actually
struct Coordinate {
	int row_;
	int col_;
};


class Game {

public:
	Game();
	~Game();

	// seems to be working...
	// TODO need to sort through the code to see where
	//		scores should be attributed to player vs opponent
	//		and calculate utility function
	//int minimax(Node* node, int depth, bool maximize);

	void next_move(Node* node, int depth);
	int minimize(Node* node, int depth);
	int maximize(Node* node, int depth);

	
	int calculate_config_score(Node* node, char symbol);


	std::vector<int> query_possible_moves(Node* node);

	void insert_unique(std::vector<int>& vec, int elem);

	int update_min_max_row(Node* node, int row, char player);
	int update_min_max_col(Node* node, int col, char player);
	
	void place_symbol_by_indeces(Node* node, char symbol, int row, int col);
	void place_symbol_from_prompt(Node* node, char symbol, int row, int col);
	void make_first_move(Node* node);

	void clear_nodes(std::vector<Node*>& nodes);

	Node* get_best_node();

	int get_best_move();

	Coordinate get_best_coordinate();


	//void find_taken_move(Node*);


	// TODO don't pass by symbol, just iterate
	//		any four in a row is a winner
	//		maybe return -1 for loss, +1 for win
	//		count blank squares, if 0, tie
	bool won_game(Node* node, char symbol);


	// returns	+1 for player win
	//			-1 for opponent win
	//			 0 ambiguous... spaces left
	//			 or perhaps something more discrete
	//int winning_config(Node* node);


	void reset_game_board();


private:
	IO io_;
	
	// indicates if you go first or not
	// true when you go first
	// can be used to call alpha aka maximize methods
	bool is_alpha_;

	Node* best_node_;

	Coordinate best_coordinate_;

	int best_move_;


};


#endif

