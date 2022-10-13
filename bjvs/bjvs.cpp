// bjvs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int uselog = 0;
int wins[20000];
int losses[20000];

money flat_bet(count tc) {
	return 10.0;
}

template <int x>money flat_bets(count tc) {
	return (money)x;
}
money ex21cnowongbet(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < 1) {
		return 5.0;
	}
	if (floor_ct == 1) {
		return 15.0;
	}
	if (floor_ct == 2) {
		return 30.0;
	}
	if (floor_ct == 3) {
		return 45.0;
	}
	return 60.0;
}
money pb_bet(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < 0) {
		return 10.0;
	}
	if (floor_ct <= 1) {
		return 25.0;
	}
	if (floor_ct == 2) {
		return 50.0;
	}
	if (floor_ct == 3) {
		return 75.0;
	}
	return 100.0;
}

money pb_bet_tight(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < 1) {
		return 5.0;
	}
	if (floor_ct == 1) {
		return 25.0;
	}
	if (floor_ct == 2) {
		return 50.0;
	}
	if (floor_ct == 3) {
		return 75.0;
	}
	if (floor_ct >= 4) {
		return 100.0;
	}
	return -99999.0;
}
money luxor_bet(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < 1) {
		return 25.0;
	}
	if (floor_ct == 1) {
		return 100.0;
	}
	if (floor_ct == 2) {
		return 150.0;
	}
	if (floor_ct == 3) {
		return 200.0;
	}
	if (floor_ct == 4) {
		return 250.0;
	}
	if (floor_ct >= 5) {
		return 300.0;
	}
	return -99999.0;
}
money pb_bet_tight_125(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < 1) {
		return 10.0;
	}
	if (floor_ct == 1) {
		return 25.0;
	}
	if (floor_ct == 2) {
		return 50.0;
	}
	if (floor_ct == 3) {
		return 75.0;
	}
	if (floor_ct == 4) {
		return 100.0;
	}
	if (floor_ct >= 5) {
		return 125.0;
	}
	return -99999.0;
}
money hilbet(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < 1) {
		return 10.0;
	}
	if (floor_ct == 1) {
		return 25.0;
	}
	if (floor_ct == 2) {
		return 50.0;
	}
	if (floor_ct == 3) {
		return 75.0;
	}
	if (floor_ct >= 4) {
		return 100.0;
	}
	return -99999.0;
}
money capebet(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < 1) {
		return 5.0;
	}
	if (floor_ct == 1) {
		return 25.0;
	}
	if (floor_ct == 2) {
		return 50.0;
	}
	if (floor_ct == 3) {
		return 75.0;
	}
	if (floor_ct >= 4) {
		return 100.0;
	}
	return -99999.0;
}
money my_bets(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < -3) {
		return 0.0;
	}
	if (floor_ct < 2) {
		return 10.0;
	}
	if (floor_ct >= 7) {
		return 100.0;
	}
	if (floor_ct < 3) {
		return 25.0;
	}
	return (floor_ct - 2.0) * 15.0 + 25.0;
	/*
	if (floor_ct >= 10) {
		return 75.0;
	}
	return floor_ct * 10.0;
	if (floor_ct < 4) {
		return 25.0;
	}
	if (floor_ct < 6) {
		return 50.0;
	}
	return 75.0;
	*/
}

action house_strat(Hand *hh, Rules *rules) {
	if (hh->value > 17) {
		return ACTION_STAND;
	} else if (hh->value < 17) {
		return ACTION_HIT;
	}
	if (rules->hit_s17) {
		return (hh->is_soft) ? ACTION_HIT : ACTION_STAND;
	}
	return ACTION_STAND;
}

Card::Card() : c('\x00') {
	value = 0;
}

Card::Card(char c) {
	if (c >= '2' && c <= '9') {
		value = (c - '2') + 2;
	}
	else if (c == 'T' || c == 'J' || c == 'Q' || c == 'K') {
		value = 10;
	}
	else if (c == 'A') {
		value = 11;
	}
	else {
		value = -99;
	}
	this->c = c;
}

bool Card::operator==(Card c) {
	return c.value == this->value;
}

void Hand::init() {
	value = 0;
	can_split = false;
	can_double = false;
	is_soft = false;
	bet = 1.0;
	active = true;
	bar_splitting = false;
	bar_doubling = false;
	num_cards = 0;
	can_blackjack = true;
}

Hand::Hand() {
	this->init();
}

Hand::Hand(Card a) {
	this->init();
	this->deal_card(a);
}

Hand::Hand(Card a, Card b) {
	this->init();
	this->deal_card(a);
	this->deal_card(b);
}

bool Hand::is_pair() {
	return num_cards == 2 && cards[0].value == cards[1].value;
}

bool Hand::is_blackjack() {
	return can_blackjack && num_cards == 2 && value == 21;
}

bool Hand::is_busted() {
	return value > 21;
}

void Hand::deal_card(Card c) {
	this->cards[this->num_cards++] = c;
	this->value = this->value + c.value;
	if (c.value != 11) {
		if (this->value > 21 && this->is_soft) {
			this->value -= 10;
			this->is_soft = false;
		}
	}
	else {
		if (this->value > 21) {
			this->value -= 10;
		}
		else {
			this->is_soft = true;
		}
	}
	this->can_double = num_cards == 2 && !this->bar_doubling;
	this->can_split = is_pair() && !this->bar_splitting;
}

std::ostream& operator<<(std::ostream &os, const Hand &h) {
	os << "num cards " << h.num_cards << " ";
	for (int i = 0; i < h.num_cards; ++i) {
		os << h.cards[i].c;
	}
	os << " (";
	if (h.is_soft) {
		os << "soft ";
	}
	os << h.value << ")";
	return os;
}

std::ostream& operator<<(std::ostream &os, const Shoe &s) {
	for (int i = 0; i < s.num_cards; ++i) {
		os << s.shoe[i];
	}
	os << " " << s.num_cards;
	return os;
}

Rules::Rules(bool hits17, bool das, char respl, char respl_aces, int decks, int shuff) : hit_s17(hits17), allow_das(das), resplit(resplit), resplit_aces(respl_aces), num_decks(decks), shuffle_point(shuff) {}

bool Rules::must_shuffle(Shoe* s) {
	return (s->shoe_index >= (num_decks * 52) - shuffle_point);
}

Player::Player(char t) {
	this->tag = t;
	this->chips = 0;
	this->total_bet = 0;
	for (int i = 0; i < PLAYER_MAX_HANDS; ++i) {
		this->hands[i] = NULL;
	}
	this->num_hands = 0;
	this->hands_played = 0;
	this->hands_skipped = 0;
	this->hands_won = 0;
	this->hands_lost = 0;
	this->hands_pushed = 0;
	this->rounds_played = 0;
}

int Player::split_hand(int index) {
	for (int i = num_hands; i > index + 1; --i) {
		hands[i] = hands[i - 1];
	}
	assert(index == num_hands - 1 || hands[index + 1] == hands[index + 2]);
	Card c = hands[index]->cards[0];
	money b = hands[index]->bet;
	delete hands[index];
	hands[index] = new Hand(c);
	hands[index + 1] = new Hand(c);
	hands[index]->can_blackjack = false;
	hands[index + 1]->can_blackjack = false;
	hands[index]->bet = b;
	hands[index + 1]->bet = b;
	++num_hands;
	return index + 1;
}

void Player::add_hand(Hand *h) {
	hands[num_hands++] = h;
}

void Player::reset_hands() {
	for (int i = 0; i < num_hands; ++i) {
		delete hands[i];
		hands[i] = NULL;
	}
	num_hands = 0;
}
Shoe::Shoe(int num_decks) {
	for (int i = 0; i < SHOE_MAX_CARDS; ++i) {
		shoe[i] = '\x00';
	}
	const char *deck = "AAAA22223333444455556666777788889999TTTTJJJJQQQQKKKK";
	assert(strlen(deck) == 52);
	for (int i = 0; i < num_decks * 52; ++i) {
		shoe[i] = deck[i % 52];
	}
	shoe_index = 0;
	running_count = 0;
	num_cards = num_decks * 52;
	shuffle();
}

void Shoe::shuffle() {
	shuffle(0, 0);
}

void Shoe::shuffle(int start, int count = 0) {
	int num_shuffled = num_cards - start;
	if (count == 0) {
		count = num_shuffled;
	}
	//printf("start is %d, cout is %d, num_shuffled is %d\n", start, count, num_shuffled);
	for (int i = 0; i < count; ++i) {
		int choice = rand() % (num_cards - start - i);
		char temp = shoe[choice + start];
		shoe[choice + start] = shoe[i + start];
		shoe[i + start] = temp;
	}
	shoe_index = 0;
	running_count = 0;
}

Card Shoe::deal() {
	char ch = shoe[shoe_index++];
	if (ch >= '2' && ch <= '6') {
		++running_count;
	}
	else if (ch == 'T' || ch == 'J' || ch == 'Q' || ch == 'K' || ch == 'A') {
		--running_count;
	}
	return Card(ch);
}

count Shoe::true_count() {
	if (num_cards <= shoe_index) {
		return 0;
	}
	count tc = (double)running_count / (((double)num_cards - (double)shoe_index) / 52.0);
	//std::cout << running_count << " " << ((num_cards - shoe_index) / 52) << " " << tc << std::endl;
	return tc;
}

void Shoe::bring(char c, int index) {
	int found = -1;
	for (int i = index; i < num_cards; ++i) {
		if (shoe[i] == c) {
			found = i;
			break;
		}
	}
	if (found == -1) {
		printf("911; trying to bring %c but couldn't find\n", c);
	}
	char tmp = shoe[index];
	shoe[index] = shoe[found];
	shoe[found] = tmp;
}

void Shoe::set(const char *c) {
	memcpy(&(shoe[0]), c, sizeof(char) * num_cards);
	shoe_index = 0;
	running_count = 0;
}

void Shoe::reset() {
	shoe_index = 0;
	running_count = 0;
}

char eval_hand(Hand *player, Hand *house) {
	if (house->is_blackjack() || player->is_blackjack()) {
		if (house->is_blackjack() && player->is_blackjack()) {
			return RESULT_PUSH;
		}
		else if (house->is_blackjack()) {
			return RESULT_LOSS;
		}
		else {
			return RESULT_WIN_BLACKJACK;
		}
	}
	if (player->is_busted()) {
		return RESULT_LOSS;
	}
	else if (house->is_busted()) {
		return RESULT_WIN;
	}
	if (player->value > house->value) {
		return RESULT_WIN;
	}
	else if (player->value < house->value) {
		return RESULT_LOSS;
	}
	else {
		return RESULT_PUSH;
	}
}

void play_hand(Shoe *s, Player *p, Strategy *strat, Rules *rules) {
	Hand *h = new Hand();
	h->bet = strat->bet_amount(s->true_count());
	h->deal_card(s->deal());
	h->deal_card(s->deal());
	p->add_hand(h);
	Card upcard = s->deal();
	Hand house = Hand(upcard, s->deal());
	if (uselog == 1) std::cout << "(" << p->tag << ") " << h->cards[0].c << h->cards[1].c << " vs " << house.cards[0].c << "[" << house.cards[1].c << "]" << std::endl;
	//TODO: allow different hole card rules here
	if (house.is_blackjack()) {
		for (int i = 0; i < p->num_hands; ++i) {
			p->hands[i]->active = false;
		}
	}
	while (true) {
		Hand *the_hand = NULL;
		int index = -1;
		//Check bar_splitting and bar_doubling flags, and identify which hand we're going to play
		for (int i = 0; i < p->num_hands; ++i) {
			if (p->hands[i]->bar_splitting) {
				p->hands[i]->can_split = false;
			}
			if (p->hands[i]->bar_doubling) {
				p->hands[i]->can_double = false;
			}
			if (p->hands[i]->active) {
				if (the_hand == NULL) {
					the_hand = p->hands[i];
					index = i;
				}
			}
		}
		if (the_hand == NULL) {
			break;
		}

		char action = strat->evaluate(the_hand, upcard, 0);
		if (uselog == 1) std::cout << "(" << p->tag << ") SI " << s->shoe_index << " " << *the_hand << " versus house [" << house.cards[0].c << "] action [" << action << "]" << std::endl;
		if (action == ACTION_STAND) {
			the_hand->active = false;
		} else if (action == ACTION_HIT) {
			the_hand->deal_card(s->deal());
		} else if (action == ACTION_DOUBLE) {
			if (the_hand->num_cards != 2) {
				printf("911; doubling with %d cards\n", the_hand->num_cards);
			}
			if (rules->allow_das == false && p->num_hands > 1) {
				printf("911; doubling after split\n");
			}
			the_hand->bet *= 2;
			the_hand->deal_card(s->deal());
			the_hand->active = false;
		} else if (action == ACTION_SPLIT) {
			if (the_hand->cards[0].value != 11) {
				if (p->num_hands >= 4 && rules->resplit != RESPLIT_ALWAYS) {
					printf("911; splitting from %d to %d with resplit not RESPLIT_ALWAYS\n", p->num_hands, p->num_hands + 1);
				}
				else if (p->num_hands == 3 && rules->resplit != RESPLIT_ALWAYS && rules->resplit != RESPLIT_TO_4) {
					printf("911; splitting from %d to %d with resplit not RESPLIT_ALWAYS or RESPLIT_TO_4\n", p->num_hands, p->num_hands + 1);
				}
				else if (p->num_hands == 2 && rules->resplit != RESPLIT_ALWAYS && rules->resplit != RESPLIT_TO_4 && rules->resplit != RESPLIT_TO_3) {
					printf("911; splitting from %d to %d with resplit not RESPLIT_ALWAYS or RESPLIT_TO_4 or RESPLIT_TO_3\n", p->num_hands, p->num_hands + 1);
				}
			} else {
				if (p->num_hands != 1) ("Resplitting aces to %d\n", p->num_hands + 1);
				if (p->num_hands >= 4 && rules->resplit_aces != RESPLIT_ALWAYS) {
					printf("911; splitting aces from %d to %d with resplit_aces not RESPLIT_ALWAYS\n", p->num_hands, p->num_hands + 1);
				}
				else if (p->num_hands == 3 && rules->resplit_aces != RESPLIT_ALWAYS && rules->resplit_aces != RESPLIT_TO_4) {
					printf("911; splitting aces from %d to %d with resplit_aces not RESPLIT_ALWAYS or RESPLIT_TO_4\n", p->num_hands, p->num_hands + 1);
				}
				else if (p->num_hands == 2 && rules->resplit_aces != RESPLIT_ALWAYS && rules->resplit_aces != RESPLIT_TO_4 && rules->resplit_aces != RESPLIT_TO_3) {
					printf("911; splitting aces from %d to %d with resplit_aces not RESPLIT_ALWAYS or RESPLIT_TO_4 or RESPLIT_TO_3\n", p->num_hands, p->num_hands + 1);
				}
			}
			int i2 = p->split_hand(index);
			bool done_splitting = false;
			char split_rule = rules->resplit;
			if (the_hand->cards[0].value == 11) {
				split_rule = rules->resplit_aces;
				//if (p->num_hands != 2) printf("Splitting aces to %d; split_rule is %c\n", p->num_hands, split_rule);
			}
			if (p->num_hands >= 4 && !(split_rule == RESPLIT_ALWAYS)) {
				//if (the_hand->cards[0].value==11)printf("Split to %d; rule is %c; done\n", p->num_hands, split_rule);
				done_splitting = true;
			} else if (p->num_hands == 3 && !(split_rule == RESPLIT_ALWAYS || split_rule == RESPLIT_TO_4)) {
				//if (the_hand->cards[0].value == 11)printf("Split to %d; rule is %c; done\n", p->num_hands, split_rule);
				done_splitting = true;
			} else if (p->num_hands == 2 && !(split_rule == RESPLIT_ALWAYS || split_rule == RESPLIT_TO_4 || split_rule == RESPLIT_TO_3)) {
				//if (the_hand->cards[0].value == 11)printf("Split to %d; rule is %c; done\n", p->num_hands, split_rule);
				done_splitting = true;
			} else {
				if (the_hand->cards[0].value == 11 && false) {
					printf("Split to %d....should still be able to split.\n", p->num_hands);
					for (int i = 0; i < p->num_hands; ++i) {
						printf("%c", p->hands[i]->can_split ? 'Y' : 'N');
					}
					printf("\n");
				}
			}
			if (done_splitting) {
				for (int i = 0; i < p->num_hands; ++i) {
					p->hands[i]->bar_splitting = true;
				}
			}
			if (the_hand->cards[0].value == 11) {
				//If we can resplit the aces, don't mark the hand inactive just yet.
				the_hand->deal_card(s->deal());
				if (!done_splitting) {
					the_hand->active = false;
				}
				p->hands[i2]->deal_card(s->deal());
				if (!done_splitting) {
					p->hands[i2]->active = false;
				}
			}
			if (!(rules->allow_das)) {
				for (int i = 0; i < p->num_hands; ++i) {
					p->hands[i]->bar_doubling = true;
				}
			}
		}
		if (the_hand->is_busted()) {
			the_hand->active = false;
		}
	}
	while (house.active && !house.is_busted()) {
		action a = house_strat(&house, rules);
		if (a == 'H' && house.value == 17 && house.is_soft && !rules->hit_s17) {
			printf("911; hitting soft 17\n");
		}
		if (a == 'H') {
			house.deal_card(s->deal());
		} else {
			house.active = false;
		}
	}
	for (int i = 0; i < p->num_hands; ++i) {
		char result = eval_hand(p->hands[i], &house);
		if (uselog == 1) std::cout << "(" << p->tag << ") Player " << *(p->hands[i]) << " versus house " << house << " bet " << p->hands[i]->bet << " result " << result << std::endl;
		if (result == RESULT_WIN) {
			wins[(int)p->hands[i]->bet * 2]++;
			p->chips += p->hands[i]->bet;
			++(p->hands_won);
		}
		else if (result == RESULT_LOSS) {
			losses[(int)p->hands[i]->bet * 2]++;
			p->chips -= p->hands[i]->bet;
			++(p->hands_lost);
		}
		else if (result == RESULT_WIN_BLACKJACK) {
			wins[(int)p->hands[i]->bet * 3]++;
			p->chips += p->hands[i]->bet * 1.5;
			++(p->hands_won);
		}
		else if (result == RESULT_PUSH) {
			++(p->hands_pushed);
		}
		else {
			printf("911; unknown result\n");
		}
		p->total_bet += p->hands[i]->bet;
	}
	if (p->hands[0]->bet > 0.0) {
		p->hands_played += p->num_hands;
	} else {
		p->hands_skipped += p->num_hands;
	}
	++p->rounds_played;

	p->reset_hands();
}

char bja_h17_das_hard[22][13] = {
	{},					// 0
	{},					// 1
	{"00HHHHHHHHHH"},	// 2
	{"00HHHHHHHHHH"},	// 3
	{"00HHHHHHHHHH"},	// 4
	{"00HHHHHHHHHH"},	// 5
	{"00HHHHHHHHHH"},	// 6
	{"00HHHHHHHHHH"},	// 7
	{"00HHHHHHHHHH"},	// 8
	{"00HDDDDHHHHH"},	// 9
	{"00DDDDDDDDHH"},	// 10
	{"00DDDDDDDDDD"},	// 11
	{"00HHSSSHHHHH"},	// 12
	{"00SSSSSHHHHH"},	// 13
	{"00SSSSSHHHHH"},	// 14
	{"00SSSSSHHHHH"},	// 15
	{"00SSSSSHHHHH"},	// 16
	{"00SSSSSSSSSS"},	// 17
	{"00SSSSSSSSSS"},	// 18
	{"00SSSSSSSSSS"},	// 19
	{"00SSSSSSSSSS"},	// 20
	{"00SSSSSSSSSS"}	// 21
};
char bja_h17_das_soft[22][13] = {
	{""},					// 0
	{""},					// 1
	{""},	// 2
	{""},	// 3
	{""},	// 4
	{""},	// 5
	{""},	// 6
	{""},	// 7
	{""},	// 8
	{""},	// 9
	{""},	// 10
	{"00DDDDDDDDDD"},	// 11
	{"00HHHHHHHHHH"},	// 12
	{"00HHHDDHHHHH"},	// 13
	{"00HHHDDHHHHH"},	// 14
	{"00HHDDDHHHHH"},	// 15
	{"00HHDDDHHHHH"},	// 16
	{"00HDDDDHHHHH"},	// 17
	{"00DDDDDSSHHH"},	// 18
	{"00SSSSDSSSSS"},	// 19
	{"00SSSSSSSSSS"},	// 20
	{"00SSSSSSSSSS"}	// 21
};
char bja_h17_das_pair[12][13] = {
	{""},	// 0
	{""},	// 1
	{"00YYYYYYNNNN"},	// 2
	{"00YYYYYYNNNN"},	// 3
	{"00NNNYYNNNNN"},	// 4
	{"00NNNNNNNNNN"},	// 5
	{"00YYYYYNNNNN"},	// 6
	{"00YYYYYYNNNN"},	// 7
	{"00YYYYYYYYYY"},	// 8
	{"00YYYYYNYYNN"},	// 9
	{"00NNNNNNNNNN"},	// 10
	{"00YYYYYYYYYY"}	// 11
};

char pb_s17_nodas_hard[22][13] = {
	{},					// 0
	{},					// 1
	{"00HHHHHHHHHH"},	// 2
	{"00HHHHHHHHHH"},	// 3
	{"00HHHHHHHHHH"},	// 4
	{"00HHHHHHHHHH"},	// 5
	{"00HHHHHHHHHH"},	// 6
	{"00HHHHHHHHHH"},	// 7
	{"00HHHHHHHHHH"},	// 8
	{"00HDDDDHHHHH"},	// 9
	{"00DDDDDDDDHH"},	// 10
	{"00DDDDDDDDDH"},	// 11
	{"00HHSSSHHHHH"},	// 12
	{"00SSSSSHHHHH"},	// 13
	{"00SSSSSHHHHH"},	// 14
	{"00SSSSSHHHHH"},	// 15
	{"00SSSSSHHHHH"},	// 16
	{"00SSSSSSSSSS"},	// 17
	{"00SSSSSSSSSS"},	// 18
	{"00SSSSSSSSSS"},	// 19
	{"00SSSSSSSSSS"},	// 20
	{"00SSSSSSSSSS"}	// 21
};
char pb_s17_nodas_soft[22][13] = {
	{""},					// 0
	{""},					// 1
	{ "" },	// 2
	{ "" },	// 3
	{ "" },	// 4
	{ "" },	// 5
	{ "" },	// 6
	{ "" },	// 7
	{ "" },	// 8
	{ "" },	// 9
	{ "" },	// 10
	{ "00DDDDDDDDDD" },	// 11
	{ "00HHHHHHHHHH" },	// 12
	{ "00HHHHDHHHHH" },	// 13
	{ "00HHHDDHHHHH" },	// 14
	{ "00HHDDDHHHHH" },	// 15
	{ "00HHDDDHHHHH" },	// 16
	{ "00HDDDDHHHHH" },	// 17
	{ "00SDDDDSSHHH" },	// 18
	{ "00SSSSDSSSSS" },	// 19
	{ "00SSSSSSSSSS" },	// 20
	{ "00SSSSSSSSSS" }	// 21
};
char pb_s17_nodas_pair[12][13] = {
	{""},	// 0
	{""},	// 1
	{"00NNYYYYNNNN"},	// 2
	{"00NNYYYYNNNN"},	// 3
	{"00NNNNNNNNNN"},	// 4
	{"00NNNNNNNNNN"},	// 5
	{"00NYYYYNNNNN"},	// 6
	{"00YYYYYYNNNN"},	// 7
	{"00YYYYYYYYYY"},	// 8
	{"00YYYYYNYYNN"},	// 9
	{"00NNNNNNNNNN"},	// 10
	{"00YYYYYYYYYY"}	// 11
};

char scratch_h17_das_hard[22][13] = {
	{},					// 0
	{},					// 1
	{"00??????????"},	// 2
	{"00??????????"},	// 3
	{"00??????????"},	// 4
	{"00??????????"},	// 5
	{"00??????????"},	// 6
	{"00??????????"},	// 7
	{"00??????????"},	// 8
	{"00??????????"},	// 9
	{"00??????????"},	// 10
	{"00??????????"},	// 11
	{"00??????????"},	// 12
	{"00??????????"},	// 13
	{"00??????????"},	// 14
	{"00??????????"},	// 15
	{"00??????????"},	// 16
	{"00??????????"},	// 17
	{"00??????????"},	// 18
	{"00SSSSSSSSSS"},	// 19
	{"00SSSSSSSSSS"},	// 20
	{"00SSSSSSSSSS"}	// 21
};

char scratch_h17_das_soft[22][13] = {
	{},					// 0
	{},					// 1
	{},	// 2
	{},	// 3
	{},	// 4
	{},	// 5
	{},	// 6
	{},	// 7
	{},	// 8
	{},	// 9
	{},	// 10
	{},	// 11
	{"00??????????"},	// 12
	{"00??????????"},	// 13
	{"00??????????"},	// 14
	{"00??????????"},	// 15
	{"00??????????"},	// 16
	{"00??????????"},	// 17
	{"00??????????"},	// 18
	{"00??????????"},	// 19
	{"00??????????"},	// 20
	{"00SSSSSSSSSS"}	// 21
};

char** malloc_hard() {
	char** ret = (char**)malloc(sizeof(char*) * 22);
	ret[0] = (char*)NULL;
	ret[1] = (char*)NULL;
	//XXX //TODO
}

char** malloc_soft() {

}

char** malloc_pair() {

}

Strategy::Strategy(char h[22][13], char s[22][13], char p[12][13], money(*betfunc)(count)) {
	bet_amount_func = betfunc;
	for (int i = 0; i < 22; ++i) {
		for (int j = 0; j < 13; ++j) {
			hard[i][j] = h[i][j];
			soft[i][j] = s[i][j];
			if (i < 12) {
				pair[i][j] = p[i][j];
			}
		}
	}
	if (!uselog) return;
	printf("HARD\n");
	for (int i = 21; i >= 4; --i) {
		printf("%2d: ", i);
		for (int j = 2; j <= 11; ++j){
			printf("%c", hard[i][j]);
		}
		printf("\n");
	}
	printf("SOFT\n");
	for (int i = 21; i >= 13; --i) {
		printf("%2d: ", i);
		for (int j = 2; j <= 11; ++j) {
			printf("%c", soft[i][j]);
		}
		printf("\n");
	}
	printf("PAIRS\n");
	for (int i = 11; i >= 2; --i) {
		printf("%2d: ", i);
		for (int j = 2; j <= 11; ++j) {
			printf("%c", pair[i][j]);
		}
		printf("\n");
	}
}

char Strategy::evaluate(Hand *h, Card upcard, count tc) {
	//printf("We're in regular evaluate.  Uh-oh.\n");
	if (h->can_split && pair[h->cards[0].value]) {
		if (pair[h->cards[0].value][upcard.value] == 'Y') {
			return ACTION_SPLIT;
		}
	}
	char a;
	if (h->is_soft) {
		a = soft[h->value][upcard.value];
	}
	else {
		a = hard[h->value][upcard.value];
	}
	if (a == STRATEGY_DOUBLE_HIT || a == STRATEGY_DOUBLE_STAND) {
		if (h->can_double) {
			a = ACTION_DOUBLE;
		} else {
			if (a == STRATEGY_DOUBLE_HIT) {
				a = ACTION_HIT;
			} else { // a == STRATEGY_DOUBLE_STAND
				a = ACTION_STAND;
			}
		}
	}
	// Deviations
	/*
	if (!h->is_soft && h->value == 16 && upcard.value == 10 && tc >= 0) {
		a = ACTION_STAND;
	}
	if (!h->is_soft && h->value == 15 && upcard.value == 10 && tc >= 4) {
		a = ACTION_STAND;
	}
	if (h->is_pair() && h->value == 20 && upcard.value == 5 && tc >= 5) {
		a = ACTION_SPLIT;
	}
	if (h->is_pair() && h->value == 20 && upcard.value == 6 && tc >= 4) {
		a = ACTION_SPLIT;
	}
	if (h->can_double && h->value == 10 && upcard.value == 10 && tc >= 4) {
		a = ACTION_DOUBLE;
	}
	*/
	return a;
}

AlwaysHit::AlwaysHit(char h[22][13], char s[22][13], char p[12][13], money(*betfunc)(count)) : Strategy(h, s, p, betfunc) {}

char AlwaysHit::evaluate(Hand *h, Card upcard, count tc) {
	char a;
	if (h->is_soft) {
		a = soft[h->value][upcard.value];
	}
	else {
		a = hard[h->value][upcard.value];
	}
	if (a == '?') {
		return ACTION_HIT;
	}
	if (a == STRATEGY_DOUBLE_HIT || a == STRATEGY_DOUBLE_STAND) {
		if (h->can_double) {
			a = ACTION_DOUBLE;
		}
		else {
			if (a == STRATEGY_DOUBLE_HIT) {
				a = ACTION_HIT;
			}
			else { // a == STRATEGY_DOUBLE_STAND
				a = ACTION_STAND;
			}
		}
	}
	return a;
}

AlwaysStand::AlwaysStand(char h[22][13], char s[22][13], char p[12][13], money(*betfunc)(count)) : Strategy(h, s, p, betfunc) {}

char AlwaysStand::evaluate(Hand *h, Card upcard, count tc) {
	char a;
	if (h->is_soft) {
		a = soft[h->value][upcard.value];
	}
	else {
		a = hard[h->value][upcard.value];
	}
	if (a == '?') {
		return ACTION_STAND;
	}
	if (a == STRATEGY_DOUBLE_HIT || a == STRATEGY_DOUBLE_STAND) {
		if (h->can_double) {
			a = ACTION_DOUBLE;
		}
		else {
			if (a == STRATEGY_DOUBLE_HIT) {
				a = ACTION_HIT;
			}
			else { // a == STRATEGY_DOUBLE_STAND
				a = ACTION_STAND;
			}
		}
	}
	return a;
}

AlwaysDouble::AlwaysDouble(char h[22][13], char s[22][13], char p[12][13], money(*betfunc)(count)) : Strategy(h, s, p, betfunc) {}

char AlwaysDouble::evaluate(Hand* h, Card upcard, count tc) {
	char a;
	if (h->is_soft) {
		a = soft[h->value][upcard.value];
	}
	else {
		a = hard[h->value][upcard.value];
	}
	if (a == '?') {
		return ACTION_DOUBLE;
	}
	if (a == STRATEGY_DOUBLE_HIT || a == STRATEGY_DOUBLE_STAND) {
		if (h->can_double) {
			a = ACTION_DOUBLE;
		}
		else {
			if (a == STRATEGY_DOUBLE_HIT) {
				a = ACTION_HIT;
			}
			else { // a == STRATEGY_DOUBLE_STAND
				a = ACTION_STAND;
			}
		}
	}
	return a;
}

AlwaysDoubleHit::AlwaysDoubleHit(char h[22][13], char s[22][13], char p[12][13], money(*betfunc)(count)) : Strategy(h, s, p, betfunc) {}

char AlwaysDoubleHit::evaluate(Hand* h, Card upcard, count tc) {
	if (hard[h->value][upcard.value] == '?') {
		if (h->can_double) {
			return ACTION_DOUBLE;
		}
		printf("Not doubling because we can't double here...hit instead\n");
		return ACTION_HIT;
	}
	return hard[h->value][upcard.value];
}

AlwaysDoubleStand::AlwaysDoubleStand(char h[22][13], char s[22][13], char p[12][13], money(*betfunc)(count)) : Strategy(h, s, p, betfunc) {}

char AlwaysDoubleStand::evaluate(Hand* h, Card upcard, count tc) {
	if (hard[h->value][upcard.value] == '?') {
		if (h->can_double) {
			return ACTION_DOUBLE;
		}
		printf("Not doubling because we can't double here...stand instead\n");
		return ACTION_STAND;
	}
	return hard[h->value][upcard.value];
}

bool Strategy::insure(Hand *h, Card upcard, count tc) {
	return false;
}

money Strategy::bet_amount(count tc) {
	return this->bet_amount_func(tc);
}

int bust = 0;
int doubled = 0;
long hands_to_bust = 0;
long hands_to_doubled = 0;

bool double_or_bust(money bankroll, Rules *r, Strategy *strategy) {
	Shoe shoe(r->num_decks);
	Player p('?');
	p.chips = bankroll;
	while (p.chips > 0 && p.chips < bankroll * 2) {
		if (r->must_shuffle(&shoe)) {
			shoe.shuffle();
		}
		play_hand(&shoe, &p, strategy, r);
	}
	if (p.chips <= 0) {
		return false;
		//++bust;
		//hands_to_bust += p.hands_played;
	}
	if (p.chips >= bankroll * 2) {
		return true;
		//++doubled;
		//hands_to_doubled += p.hands_played;
	}
}

void print_player(Player* p) {
	printf("(%c) Played %d skipped %d won %d lost %d pushed %d net %f bet %f net/bet %f\n", p->tag, p->hands_played, p->hands_skipped, p->hands_won, p->hands_lost, p->hands_pushed, p->chips, p->total_bet, p->chips / p->total_bet);
	printf("(%c) Profit per 100 hands %f\n", p->tag, p->chips / (p->hands_played / 100));
	printf("(%c) Profit per 100 rounds %f\n", p->tag, p->chips / (p->rounds_played / 100));
}

void figure_basic(Rules *r, int total, bool hard, int num_hands, char *results) {
	AlwaysHit hit = AlwaysHit(bja_h17_das_hard, scratch_h17_das_soft, bja_h17_das_pair, flat_bet);
	AlwaysStand stand = AlwaysStand(bja_h17_das_hard, scratch_h17_das_soft, bja_h17_das_pair, flat_bet);
	AlwaysDouble dbl = AlwaysDouble(bja_h17_das_hard, scratch_h17_das_soft, bja_h17_das_pair, flat_bet);

	Shoe s(6);
	int hit_hands = 0;
	int stand_hands = 0;
	Player *hitplayers[13];
	Player *standplayers[13];
	Player *doubleplayers[13];
	for (int i = 0; i < 13; ++i) {
		hitplayers[i] = new Player('H');
		standplayers[i] = new Player('S');
		doubleplayers[i] = new Player('D');
	}
	int total_hands = 0;
	int rounds = 0;
	while (rounds < num_hands) {
		s.shuffle(0, 2);
		if (rounds % 500000 == 0) printf("Round %d\n", rounds);
		while (true) {
			Hand h = Hand(s.shoe[0], s.shoe[1]);
			if (h.is_soft != hard && h.value == total) {
				break;
			}
			s.shuffle(0, 2);
		}
		for (int i = 2; i <= 11; ++i) {
			char upcard;
			if (i <= 9) {
				upcard = '0' + i;
			}
			else if (i == 10) {
				upcard = 'T';
			}
			else if (i == 11) {
				upcard = 'A';
			}
			s.bring(upcard, 2);
			s.shuffle(3, 20);
			Player* p = hitplayers[Card(s.shoe[2]).value];
			if (Hand(Card(s.shoe[0]), Card(s.shoe[1])).value != total || Card(s.shoe[2]).value != i) {
				printf("Playing the wrong hand...\n");
			}
			play_hand(&s, p, &hit, r);

			s.shuffle(3, 20);
			s.reset();
			if (total > 11) {
				p = standplayers[Card(s.shoe[2]).value];
				if (Hand(Card(s.shoe[0]), Card(s.shoe[1])).value != total || Card(s.shoe[2]).value != i) {
					printf("Playing the wrong hand...\n");
				}
				play_hand(&s, p, &stand, r);
				s.shuffle(3, 20);
				s.reset();
			}
			
			p = doubleplayers[Card(s.shoe[2]).value];
			if (Hand(Card(s.shoe[0]), Card(s.shoe[1])).value != total || Card(s.shoe[2]).value != i) {
				printf("Playing the wrong hand...\n");
			}
			play_hand(&s, p, &dbl, r);

			++rounds;
		}
	}
	total_hands = 0;
	for (int i = 2; i < 12; ++i) {
		if (false) {
			printf("****************** AGAINST %d *************\n", i);
			printf("Hit: ");
			print_player(hitplayers[i]);
			printf("Stand: ");
			print_player(standplayers[i]);
			printf("D: ");
			print_player(doubleplayers[i]);
		}
		if (standplayers[i]->hands_played == 0) {
			standplayers[i]->chips = -100.0 * hitplayers[i]->hands_played;
		}
		if (doubleplayers[i]->chips > hitplayers[i]->chips && doubleplayers[i]->chips > standplayers[i]->chips) {
			if (hitplayers[i]->chips > standplayers[i]->chips) {
				results[i] = STRATEGY_DOUBLE_HIT;
			} else {
				results[i] = STRATEGY_DOUBLE_STAND;
			}
		} else {
			if (hitplayers[i]->chips > standplayers[i]->chips) {
				results[i] = STRATEGY_HIT;
			} else {
				results[i] = STRATEGY_STAND;
			}
		}
		delete hitplayers[i];
		delete standplayers[i];
		delete doubleplayers[i];
	}
}

Rules pbrules(false, false, RESPLIT_TO_4, RESPLIT_NEVER, 6, 52);

void sim_hands(Rules* r, int num_rounds, Player* p, money(*bets)(count), char hard[22][13], char soft[22][13], char split[12][13]) {
	Shoe s(r->num_decks);
	Strategy me(hard, soft, split, bets);
	while (num_rounds > 0) {
		//if (num_rounds % 1000000 == 0) printf("%d rounds left\n", num_rounds);
		if (r->must_shuffle(&s)) {
			s.shuffle();
		}
		play_hand(&s, p, &me, r);
		--num_rounds;
	}
	print_player(p);
}

double double_success_rate(money bankroll, Rules* rules, Strategy* strat, int iterations) {
	int dbl_yes = 0;
	int dbl_no = 0;
	for (; iterations > 0; --iterations) {
		if (double_or_bust(bankroll, rules, strat)) {
			++dbl_yes;
		} else {
			++dbl_no;
		}
	}
	return (double)dbl_yes / ((double)dbl_yes + (double)dbl_no);
}

int main() {
	Rules common(true, true, RESPLIT_TO_4, RESPLIT_NEVER, 6, 65);
	Rules luxor(true, true, RESPLIT_TO_4, RESPLIT_NEVER, 2, 35);
	Rules hil(true, true, RESPLIT_TO_4, RESPLIT_TO_4, 6, 70);
	Rules caperules(true, true, RESPLIT_TO_4, RESPLIT_TO_4, 6, 62);
	Rules esc(true, true, RESPLIT_TO_4, RESPLIT_NEVER, 2, 36);
	Rules excal(true, true, RESPLIT_TO_4, RESPLIT_NEVER, 8, 26);
	for (int i = 0; i < 20000; ++i) {
		wins[i] = 0;
		losses[i] = 0;
	}
	srand(time(0));
	//Strategy bja(bja_s17_das_hard, bja_s17_das_soft, bja_s17_das_pair, linear_bet);
	//Strategy me(bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, my_bets);
	//Strategy pbstrat(bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, pb_bet);
	//Strategy basic(bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, flat_bet);
	time_t start = time(0);
	Player p0('$');
	Player p1('$');
	Player p2('$');
	//printf("21c thing\n");
	//sim_hands(&common, 10000000, &p0, flat_bets<10>, bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair);
	//printf("10 -25 50 -75 - 100, PB rules\n");
	//sim_hands(&pbrules, 100000000, &p1, pb_bet, pb_s17_nodas_hard, pb_s17_nodas_soft, pb_s17_nodas_pair);
	//printf("5 - 25 - 50 - 75 - 100, but common rules and tighter bets\n");
	//sim_hands(&common, 100000000, &p2, pb_bet_tight, bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair);
	//printf("luxor\n");
	//sim_hands(&luxor, 100000000, &p2, luxor_bet, bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair);
	//Rules ex21cnowong(true, true, RESPLIT_TO_4, RESPLIT_TO_4, 6, 73);
	//Strategy testme(bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, ex21cnowongbet);
	//sim_hands(&ex21cnowong, 100000000, &p2, ex21cnowongbet, bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair);
	//std::cout << "RoR, 5k, ex21c: " << double_success_rate(5000, &ex21cnowong, &testme, 1000);
	//return 0;
	/*
	for (int total = 18; total > 10; --total) {
		char results[13] = "____________";
		figure_basic(total, true, 10000000, results);
		printf("%d: %s\n", total, results);
		for (int up = 2; up < 12; ++up) {
			scratch_h17_das_hard[total][up] = results[up];
		}
	}
	for (int i = 18; i > 10; --i) {
		printf("%d: ", i);
		for (int j = 2; j < 12; ++j) {
			printf("%c", scratch_h17_das_hard[i][j]);
		}
		printf("\n");
	}
	*/
	
	for (int total = 20; total > 12; --total) {
		char results[13] = "____________";
		figure_basic(&common, total, false, 10000000, results);
		printf("%d: %s\n", total, results);
		for (int up = 2; up < 12; ++up) {
			scratch_h17_das_soft[total][up] = results[up];
		}
	}
}