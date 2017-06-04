#include "game.h"
#include "node.h"
#include "io.h"
#include <iostream>
#include <climits>


namespace {
	const int MAX_LEAVES = 5;
}


typedef Node* GameBoard;


Game::Game() : 
	best_node_(new Node)
{
	this->best_move_ = -1;
}


Game::~Game() {
	
}





int Game::minimax(Node* node, int depth, bool maximize) {

	if (this->won_game(node, SYMBOL::PLAYER) ||
		this->won_game(node, SYMBOL::OPPONENT) ||
		depth == 0) 
	{
		return //this->calculate_config_score(node, SYMBOL::PLAYER);
			this->calculate_config_score(node, SYMBOL::PLAYER) -
			this->calculate_config_score(node, SYMBOL::OPPONENT);
	}

	if (maximize) {
		int best_value = INT_MIN;
		std::vector<int> moves = this->query_possible_moves(node);
		for (size_t i = 0; i < moves.size(); ++i) {
			node->config_[moves[i]] = SYMBOL::PLAYER;
			int value = this->minimax(node, depth - 1, false);
			if (value > best_value) {
				best_value = value;
				this->best_move_ = moves[i];

				std::cout << depth << " " << value << " " << moves[i] << std::endl;

			}
		}
		for (size_t i = 0; i < moves.size(); ++i)
			node->config_[moves[i]] = SYMBOL::EMPTY;
		return best_value;
	}
	else {
		int best_value = INT_MAX;
		std::vector<int> moves = this->query_possible_moves(node);
		for (size_t i = 0; i < moves.size(); ++i) {
			node->config_[moves[i]] = SYMBOL::OPPONENT;
			int value = this->minimax(node, depth - 1, true);
			if (value < best_value) {
				best_value = value;
				this->best_move_ = moves[i];

				std::cout << depth << " " << value << " "  << moves[i] << std::endl;

			}
		}
		for (size_t i = 0; i < moves.size(); ++i)
			node->config_[moves[i]] = SYMBOL::EMPTY;
		return best_value;
	}
}


int Game::minimize(Node* node, int depth) {
	return 0;
}


int Game::maximize(Node* node, int depth) {
	return 1;
}



/*

int Game::minimax(Node* node, int depth, bool maximize) {

	if (depth == 0) {
		return node->player_score_;
	}

	if (this->won_game(node, SYMBOL::PLAYER)) {
		return node->player_score_;
	}
	if (this->won_game(node, SYMBOL::OPPONENT)) {
		return node->opponent_score_;
	}

	if (maximize) {
		int best_value = -10000;
		std::vector<Node*> nodes = this->possible_configs(node,SYMBOL::PLAYER);
		for (size_t i = 0; i < nodes.size(); ++i) {
			int value = this->minimax(nodes[i], depth-1, false);
			// very temporary!
			if (value > best_value) {
				//this->copy_config(best_node_, node);
				this->copy_config(best_node_, nodes[i]);
				best_value = value;
			}
		}
		this->clear_nodes(nodes);
		return best_value;
	}
	else {
		int best_value = 10000;
		std::vector<Node*> nodes = this->possible_configs(node,SYMBOL::OPPONENT);
		for (size_t i = 0; i < nodes.size(); ++i) {
			int value = this->minimax(nodes[i], depth - 1, true);
			// very temporary!
			if (value < best_value) {
				//this->copy_config(best_node_, node);
				this->copy_config(best_node_, nodes[i]);
				best_value = value;
			}
		}
		this->clear_nodes(nodes);
		return best_value;
	}
}

*/

// TODO implement this where you go across row and column
//		to get next good value
//		perhaps check score in increments of four,
//			then place node in position which maximizes utility
//			relative to that four increment value
//		make this more efficient!!!!

std::vector<Node*> Game::possible_configs(Node* root, char symbol) {
	std::vector<Node*> nodes;
	std::vector<int> indices = this->query_moves_alpha(root,symbol);
	for (size_t i = 0; i < indices.size(); ++i) {
		nodes.push_back(this->create_child_node(root,
			indices[i]/dim::SPAN,indices[i]%dim::SPAN,symbol));
	}
	return nodes;
}



// TODO implement this method!
//
//
//		returns the index of the next potential moves
//		this should be different depending on alpha/beta
//
//		push pairs of coords onto vector
//		decide which has the greatest weight
//			determined by score of row and col
//		a way to weight the values would be nice
//		and it's probably adventageous to give priority
//			to more centered moves ?
//
//		if you are alpha
//			choose the highest value for player
//		if you are beta
//			choose highest value for opponent
//		test while vector.size <= 5 ?
//
//
std::vector<int> Game::query_moves_alpha(Node* node, char symbol) {

	std::vector<int> indices;
	int row_idx = -1, col_idx = -1;


	// very very temporary!
	if (symbol == SYMBOL::PLAYER) {
		int fst_row = this->greatest_index(node->row_player_, row_idx),
			fst_col = this->greatest_index(node->col_player_, col_idx);
		int sec_row = this->greatest_index(node->row_player_, row_idx),
			sec_col = this->greatest_index(node->col_player_, col_idx);
		this->create_row_coordinates(node, indices, fst_row);
		this->create_row_coordinates(node, indices, sec_row);
		this->create_col_coordinates(node, indices, fst_col);
		this->create_col_coordinates(node, indices, sec_col);
	}
	else if (symbol == SYMBOL::OPPONENT) {
		int fst_row = this->greatest_index(node->row_opponent_, row_idx),
			fst_col = this->greatest_index(node->col_opponent_, col_idx);
		int sec_row = this->greatest_index(node->row_opponent_, row_idx),
			sec_col = this->greatest_index(node->col_opponent_, col_idx);
		this->create_row_coordinates(node, indices, fst_row);
		this->create_row_coordinates(node, indices, sec_row);
		this->create_col_coordinates(node, indices, fst_col);
		this->create_col_coordinates(node, indices, sec_col);
	}
	

	return indices;
	
}


int Game::calculate_config_score(Node* node) {
	node->player_score_ = node->opponent_score_ = 0;
	for (int i = 0; i < dim::SPAN; ++i) {
		node->player_score_ += this->update_min_max_row(node, i, SYMBOL::PLAYER);
		node->player_score_ += this->update_min_max_col(node, i, SYMBOL::PLAYER);
		node->opponent_score_ += this->update_min_max_row(node, i, SYMBOL::OPPONENT);
		node->opponent_score_ += this->update_min_max_col(node, i, SYMBOL::OPPONENT);
	}
	return 0;
}


int Game::calculate_config_score(Node* node, char symbol) {
	int score = 0;
	for (int i = 0; i < dim::SPAN; ++i) {
		score += this->update_min_max_row(node, i, symbol);
		score += this->update_min_max_col(node, i, symbol);
	}
	return score;
}



// TODO make something smarter here!
std::vector<int> Game::query_possible_moves(Node* node) {
	std::vector<int> moves;
	for (int r = 0; r < dim::SPAN; ++r) {
		for (int c = 0; c < dim::SPAN; ++c) {
			int index = r*dim::SPAN+c;
			if (node->config_[index] != SYMBOL::EMPTY) {
				if (r != 0 && node->config_[index - 1] == SYMBOL::EMPTY)
					this->insert_unique(moves, index - 1);
				if (r != dim::SPAN && node->config_[index + 1] == SYMBOL::EMPTY)
					this->insert_unique(moves, index + 1);
				if (c != 0 && node->config_[index - dim::SPAN] == SYMBOL::EMPTY)
					this->insert_unique(moves, index - dim::SPAN);
				if (c != dim::SPAN && node->config_[index + dim::SPAN] == SYMBOL::EMPTY)
					this->insert_unique(moves, index + dim::SPAN);
			}
		}
	}
	return moves;
}




// TODO make more efficient
void Game::create_row_coordinates(
	Node* node, std::vector<int>& coord, int row)
{
	if (row == -1)
		return;
	for (int c = 0; c < dim::SPAN; ++c) {
		int index = row*dim::SPAN + c;
		char symbol = node->config_[index];
		if (symbol != SYMBOL::EMPTY) {
			if (c != 0 && node->config_[index-1] == SYMBOL::EMPTY)
				this->insert_unique(coord, index-1);
			if (c != dim::SPAN - 1 && node->config_[index+1] == SYMBOL::EMPTY)
				this->insert_unique(coord, index+1);
		}
	}
}


// TODO make more efficient
void Game::create_col_coordinates(
	Node* node, std::vector<int>& coord, int col)
{
	if (col == -1)
		return;
	for (int r = 0; r < dim::SPAN; ++r) {
		int index = r*dim::SPAN+col;
		char symbol = node->config_[index];
		if (symbol != SYMBOL::EMPTY) {
			if (r != 0 && node->config_[index-dim::SPAN] == SYMBOL::EMPTY)
				this->insert_unique(coord,index-dim::SPAN);
			if (r != dim::SPAN-1 && node->config_[index+dim::SPAN] == SYMBOL::EMPTY)
				this->insert_unique(coord, index+dim::SPAN);
		}
	}
}


void Game::insert_unique(std::vector<int>& vec, int elem) {
	for (size_t i = 0; i < vec.size(); ++i)
		if (vec[i] == elem)
			return;
	vec.push_back(elem);
}


// this works
int Game::greatest_index(int* config, int& bypass) {
	int max_elem = 0, max_indx = -1;
	for (int i = 0; i < dim::SPAN; ++i) {
		int cur_elem = config[i];
		if (cur_elem > max_elem && i != bypass) {
			max_elem = cur_elem;
			max_indx = i;
		}
	}
	bypass = max_indx;
	return max_indx;
}


// TODO test this
Node* Game::create_child_node(Node* parent, int row, int col, char symbol) {
	Node* child = new Node(parent);
	child->config_[row*dim::SPAN+col] = symbol;
	this->calculate_vector_scores(child, row, col);
	this->calculate_score_from_vectors(
		child, child->player_score_, child->opponent_score_);
	child->parent_ = parent;
	return child;
}


void Game::calculate_vector_scores(Node* node, int row, int col) {
	node->row_player_[row] = this->update_min_max_row(node, row, SYMBOL::PLAYER);
	node->col_player_[col] = this->update_min_max_col(node, col, SYMBOL::PLAYER);
	node->row_opponent_[row] = this->update_min_max_row(node, row, SYMBOL::OPPONENT);
	node->col_opponent_[col] = this->update_min_max_col(node, col, SYMBOL::OPPONENT);
}



// pass player and opponent scores by reference
void Game::calculate_score_from_vectors(Node* node, int& player, int& opponent) {
	player = opponent = 0;
	for (int i = 0; i < dim::SPAN; ++i) {
		player += node->row_player_[i] + node->col_player_[i];
		opponent += node->row_opponent_[i] + node->col_opponent_[i];
	}
}



// TODO perhaps replace the player opponent args with
//		if symbol != player
//		else if symbol != SYMBOL::EMPTY
//			uninterrupted = -1;
//		then set to zero

// this works
int Game::update_min_max_row(Node* node, int row, char player) {
	int row_score = 0;
	int r = row*dim::SPAN;
	for (int c = 0; c <= dim::MAX_ADJ; ++c) {
		int uninterrupted = 0;
		for (int i = 0; i < dim::MAX_ADJ && uninterrupted != -1; ++i) {
			char symbol = node->config_[r+c+i];
			if (symbol == player)
				++uninterrupted;
			else if (symbol != SYMBOL::EMPTY)
				uninterrupted = -1;
		}
		row_score += (uninterrupted != -1 ? uninterrupted : 0);
	}
	return row_score;
}


// this works
int Game::update_min_max_col(Node* node, int col, char player) {
	int c = col;
	int column_score = 0;
	for (int r = 0; r <= dim::MAX_ADJ; ++r) {
		int uninterrupted = 0;
		for (int i = 0; i < dim::MAX_ADJ && uninterrupted != -1; ++i) {
			char symbol = node->config_[c+dim::SPAN*(r+i)];
			if (symbol == player)
				++uninterrupted;
			else if (symbol != SYMBOL::EMPTY)
				uninterrupted = -1;
		}
		column_score += (uninterrupted != -1 ? uninterrupted : 0);
	}
	return column_score;
}


// TODO optimize this based on what leads to victory!
//		perhaps:
//			row = rand() % 2 == 0 ? 3 : 4;
//			col = rand() % 2 == 0 ? 3 : 4;
void Game::make_first_move(Node* node) {
	int row = 3, col = 3;
	this->place_symbol_by_indeces(node, SYMBOL::PLAYER, row, col);
}


// should be used when placing symbols using the row and col numbers
// that are indeces in the array
void Game::place_symbol_by_indeces(Node* node, char symbol, int row, int col) {
	node->config_[row*dim::SPAN+col] = symbol;
}


// does this really need to be included..?
void Game::place_symbol_from_prompt(Node* node, char symbol, int row, int col) {
	node->config_[(row-1)*dim::SPAN+(col-1)] = symbol;
}



void Game::clear_nodes(std::vector<Node*>& nodes) {
	for (size_t i = 0; i < nodes.size(); ++i) {
		if (nodes[i] != nullptr) {
			delete nodes[i];
			nodes[i] = nullptr;
		}
	}
}


void Game::copy_config(Node* target, Node* source) {
	for (int i = 0; i < dim::SIZE; ++i) {
		target->config_[i] = source->config_[i];
	}
}


Node* Game::get_best_node() {
	return this->best_node_;
}


int Game::get_best_move() {
	return this->best_move_;
}


Coordinate Game::get_best_coordinate() {

	Coordinate coordinate;
	coordinate.row_ = this->best_move_ / dim::SPAN;
	coordinate.col_ = this->best_move_ % dim::SPAN;
	return coordinate;

}


// this works
bool Game::won_game(Node* node, char symbol) {

	for (int r = 0; r < dim::SPAN; ++r) {
		int consecutive = 0;
		for (int c = 0; c < dim::SPAN; ++c) {
			if (node->config_[r*dim::SPAN+c] == symbol) {
				++consecutive;
				if (consecutive == dim::MAX_ADJ) {
					return true;
				}
			}
			else {
				consecutive = 0;
			}
		}
	}

	for (int c = 0; c < dim::SPAN; ++c) {
		int consecutive = 0;
		for (int r = 0; r < dim::SPAN; ++r) {
			if (node->config_[r*dim::SPAN + c] == symbol) {
				++consecutive;
				if (consecutive == dim::MAX_ADJ) {
					return true;
				}
			}
			else {
				consecutive = 0;
			}
		}
	}

	return false;
}




// call after each game
// it will reset everything
void Game::reset_game_board() {
	

}


