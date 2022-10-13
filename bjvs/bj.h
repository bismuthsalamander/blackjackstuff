#pragma once

#include <iostream>

#define HAND_MAX_CARDS 21
#define PLAYER_MAX_HANDS 24
#define SHOE_MAX_CARDS 500
#define NO_CARD '\x00'
#define RESULT_LOSS 'L'
#define RESULT_WIN 'W'
#define RESULT_PUSH 'P'
#define RESULT_WIN_BLACKJACK 'B'
#define ACTION_STAND 'S'
#define ACTION_SPLIT 'P'
#define ACTION_HIT 'H'
#define ACTION_DOUBLE 'D'
#define STRATEGY_STAND 'S'
#define STRATEGY_HIT 'H'
#define STRATEGY_DOUBLE_HIT 'D'
#define STRATEGY_DOUBLE_STAND 'd'
#define RESPLIT_NEVER '0'
#define RESPLIT_TO_3 '3'
#define RESPLIT_TO_4 '4'
#define RESPLIT_ALWAYS 'A'

typedef char action;
typedef double money;
typedef double count;

class Shoe;
class Rules {
public:
	bool hit_s17;
	bool allow_das;
	char resplit;
	char resplit_aces;
	int num_decks;
	int shuffle_point;
	Rules(bool, bool, char, char, int, int);
	bool must_shuffle(Shoe*);
};

class Card {
public:
	char c;
	Card();
	Card(char);
	int value;
	bool operator==(Card);
};

class Hand {
public:
	int value;
	bool can_split;
	bool can_double;
	bool is_soft;
	money bet;
	bool active;
	bool bar_splitting;
	bool bar_doubling;
	bool can_blackjack;
	Card cards[HAND_MAX_CARDS];
	int num_cards;
	Hand();
	Hand(Card);
	Hand(Card, Card);
	void init();
	bool is_pair();
	bool is_blackjack();
	bool is_busted();
	void deal_card(Card);
};

class Player {
public:
	Player(char);
	char tag;
	int split_hand(int);
	int num_hands;
	money chips;
	money total_bet;
	int hands_played, hands_skipped, hands_won, hands_lost, hands_pushed;
	int rounds_played;
	Hand *hands[PLAYER_MAX_HANDS];
	void add_hand(Hand*);
	void reset_hands();
};

class Shoe {
public:
	int num_cards;
	int shoe_index;
	int running_count;
	char shoe[SHOE_MAX_CARDS];
	Shoe(int);
	void shuffle();
	void shuffle(int, int);
	Card deal();
	void reset();
	void bring(char, int);
	void set(const char*);
	count true_count();
};

class Strategy {
public:
	char hard[22][13];
	char soft[22][13];
	char pair[12][13];
	money(*bet_amount_func)(count);
	Strategy(char[22][13], char[22][13], char[12][13], money(*bet_func)(count));
	money bet_amount(count);
	virtual char evaluate(Hand*, Card, count);
	bool insure(Hand*, Card, count);
};

class AlwaysHit : public Strategy {
public:
	AlwaysHit(char[22][13], char[22][13], char[12][13], money(*bet_func)(count));
	char evaluate(Hand*, Card, count);
};

class AlwaysStand : public Strategy {
public:
	AlwaysStand(char[22][13], char[22][13], char[12][13], money(*bet_func)(count));
	char evaluate(Hand*, Card, count);
};

class AlwaysDouble : public Strategy {
public:
	AlwaysDouble(char[22][13], char[22][13], char[12][13], money(*bet_func)(count));
	char evaluate(Hand*, Card, count);
};

class AlwaysDoubleHit : public Strategy {
public:
	AlwaysDoubleHit(char[22][13], char[22][13], char[12][13], money(*bet_func)(count));
	char evaluate(Hand*, Card, count);
};

class AlwaysDoubleStand : public Strategy {
public:
	AlwaysDoubleStand(char[22][13], char[22][13], char[12][13], money(*bet_func)(count));
	char evaluate(Hand*, Card, count);
};

std::ostream& operator<<(std::ostream&, const Hand&);
std::ostream& operator<<(std::ostream&, const Shoe&);
void play_hand(Shoe*, Player*, Strategy*, Rules*);