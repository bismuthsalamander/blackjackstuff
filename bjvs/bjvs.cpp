// bjvs.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

int uselog = 0;
int wins[20000];
int losses[20000];

money flat_bet(count tc) {
	return 10.0;
}

money pb_bet(count tc) {
	int floor_ct = (int)floor(tc);
	if (floor_ct < 0) {
		return 10.0;
	}
	if (floor_ct < 2) {
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
	return (floor_ct - 2) * 15.0 + 25.0;
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

action house_s17(Hand *hh) {
	return (hh->value >= 17) ? ACTION_STAND : ACTION_HIT;
}

action house_h17(Hand *hh) {
	if (hh->value > 17) {
		return ACTION_STAND;
	}
	else if (hh->value < 17) {
		return ACTION_HIT;
	}
	return (hh->is_soft) ? ACTION_HIT : ACTION_STAND;
}

Card::Card() {
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


Player::Player() {
	this->chips = 0;
	this->total_bet = 0;
	for (int i = 0; i < PLAYER_MAX_HANDS; ++i) {
		this->hands[i] = NULL;
	}
	this->num_hands = 0;
	this->hands_played = 0;
	this->hands_skipped = 0;
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
	shuffle(0);
}

void Shoe::shuffle(int start) {
	int num_shuffled = num_cards - start;
	for (int i = num_shuffled - 1; i > 0; --i) {
		int choice = rand() % (i + 1);
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
	count tc = running_count / ((num_cards - shoe_index) / 52);
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
	char tmp = shoe[index];
	shoe[index] = shoe[found];
	shoe[found] = tmp;
}

void Shoe::set(const char *c) {
	memcpy(&(shoe[0]), c, sizeof(char) * num_cards);
	shoe_index = 0;
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

void play_hand(Shoe *s, Player *p, Strategy *strat, action(*house_strat)(Hand *)) {
	Hand *h = new Hand();
	h->bet = strat->bet_amount(s->true_count());
	//std::cout << "Tc " << s->true_count() << " bet " << h->bet << std::endl;
	h->deal_card(s->deal());
	h->deal_card(s->deal());
	p->add_hand(h);
	Card upcard = s->deal();
	Hand house = Hand(upcard, s->deal());
	if (uselog == 1) std::cout << h->cards[0].c << h->cards[1].c << " vs " << house.cards[0].c << "[" << house.cards[1].c << "]" << std::endl;
	if (house.is_blackjack()) {
		for (int i = 0; i < p->num_hands; ++i) {
			p->hands[i]->active = false;
		}
	}
	while (true) {
		Hand *the_hand = NULL;
		int index = -1;
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
		if (uselog == 1) std::cout << *the_hand << " versus house " << house.cards[0].c << " " << action << std::endl;
		if (action == 'S') {
			the_hand->active = false;
		}
		else if (action == 'H') {
			the_hand->deal_card(s->deal());
		}
		else if (action == 'D') {
			the_hand->bet *= 2;
			the_hand->deal_card(s->deal());
			the_hand->active = false;
		}
		else if (action == 'P') {
			int i2 = p->split_hand(index);
			if (p->num_hands > 3) {
				for (int i = 0; i < p->num_hands; ++i) {
					p->hands[i]->bar_splitting = true;
				}
			}
			if (the_hand->cards[0].c == 'A') {
				the_hand->deal_card(s->deal());
				the_hand->active = false;
				p->hands[i2]->deal_card(s->deal());
				p->hands[i2]->active = false;
			}
		}
		if (the_hand->is_busted()) {
			the_hand->active = false;
		}
	}
	while (house.active && !house.is_busted()) {
		if (house_strat(&house) == 'H') {
			house.deal_card(s->deal());
		}
		else {
			house.active = false;
		}
	}
	for (int i = 0; i < p->num_hands; ++i) {
		char result = eval_hand(p->hands[i], &house);
		if (uselog == 1) std::cout << "Player " << *(p->hands[i]) << " versus house " << house << " bet " << p->hands[i]->bet << " result " << result << std::endl;
		if (result == RESULT_WIN) {
			wins[(int)p->hands[i]->bet * 2]++;
			p->chips += p->hands[i]->bet;
		}
		else if (result == RESULT_LOSS) {
			losses[(int)p->hands[i]->bet * 2]++;
			p->chips -= p->hands[i]->bet;
		}
		else if (result == RESULT_WIN_BLACKJACK) {
			wins[(int)p->hands[i]->bet * 3]++;
			p->chips += p->hands[i]->bet * 1.5;
		}
		p->total_bet += p->hands[i]->bet;
	}
	if (p->hands[0]->bet > 0.0) {
		p->hands_played += p->num_hands;
	}
	else {
		p->hands_skipped += p->num_hands;
	}

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
}

char Strategy::evaluate(Hand *h, Card upcard, count tc) {
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
	if (a == ACTION_DOUBLE && !h->can_double) {
		a = ACTION_HIT;
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
	if (hard[h->value][upcard.value] == '?') {
		return ACTION_HIT;
	}
	action a = Strategy::evaluate(h, upcard, tc);
	if (a == ACTION_SPLIT) {
		a = ACTION_STAND;
	}
	return a;
}

AlwaysStand::AlwaysStand(char h[22][13], char s[22][13], char p[12][13], money(*betfunc)(count)) : Strategy(h, s, p, betfunc) {}

char AlwaysStand::evaluate(Hand *h, Card upcard, count tc) {
	if (hard[h->value][upcard.value] == '?') {
		return ACTION_STAND;
	}
	action a = Strategy::evaluate(h, upcard, tc);
	if (a == ACTION_SPLIT) {
		a = ACTION_STAND;
	}
	return a;
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

void double_or_bust(money bankroll) {
	Shoe s(6);
	Player p;
	p.chips = bankroll;
	Strategy me(bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, my_bets);
	while (p.chips > 0 && p.chips < bankroll * 2) {
		if (s.shoe_index > 208) {
			s.shuffle();
		}
		play_hand(&s, &p, &me, house_h17);
	}
	if (p.chips <= 0) {
		++bust;
		hands_to_bust += p.hands_played;
	}
	if (p.chips >= bankroll * 2) {
		++doubled;
		hands_to_doubled += p.hands_played;
	}
}

void figure_basic_hard(int total, int target) {
	AlwaysHit hit = AlwaysHit(scratch_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, flat_bet);
	AlwaysStand stand = AlwaysStand(scratch_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, flat_bet);
	Shoe s(6);
	int hit_hands = 0;
	int stand_hands = 0;
	Player *hitplayers[13];
	Player *standplayers[13];
	for (int i = 0; i < 13; ++i) {
		hitplayers[i] = new Player();
		standplayers[i] = new Player();
	}
	int total_hands = 0;
	while (total_hands < target) {
		if (total_hands % 100 == 0) { printf("%d\n", total_hands); }
		while (true) {
			s.shuffle();
			Hand h = Hand(s.shoe[0], s.shoe[1]);
			if (!h.is_soft && h.value == total) {
				break;
			}
		}
		Player *p = hitplayers[Card(s.shoe[2]).value];
		int old = p->hands_played;
		play_hand(&s, p, &hit, house_h17);
		total_hands += p->hands_played - old;
	}
	total_hands = 0;
	while (total_hands < target) {
		if (total_hands % 100 == 0) { printf("%d\n", total_hands); }
		while (true) {
			s.shuffle();
			Hand h = Hand(s.shoe[0], s.shoe[1]);
			if (!h.is_soft && h.value == total) {
				break;
			}
		}
		Player *p = standplayers[Card(s.shoe[2]).value];
		int old = p->hands_played;
		play_hand(&s, p, &stand, house_h17);
		total_hands += p->hands_played - old;
	}
	for (int i = 2; i < 12; ++i) {
		printf("****************** AGAINST %d *************\n", i);
		printf("Hit:\n");
		printf("Played %d skipped %d net %f bet %f net/bet %f\n", hitplayers[i]->hands_played, hitplayers[i]->hands_skipped, hitplayers[i]->chips, hitplayers[i]->total_bet, hitplayers[i]->chips / hitplayers[i]->total_bet);
		printf("Profit per 100 hands %f\n", hitplayers[i]->chips / (hitplayers[i]->hands_played / 100));
		printf("Stand:\n");
		printf("Played %d skipped %d net %f bet %f net/bet %f\n", standplayers[i]->hands_played, standplayers[i]->hands_skipped, standplayers[i]->chips, standplayers[i]->total_bet, standplayers[i]->chips / standplayers[i]->total_bet);
		printf("Profit per 100 hands %f\n", standplayers[i]->chips / (standplayers[i]->hands_played / 100));
	}
	/*
	printf("Played %d skipped %d net %f bet %f net/bet %f\n", hitplayer.hands_played, hitplayer.hands_skipped, hitplayer.chips, hitplayer.total_bet, hitplayer.chips / hitplayer.total_bet);
	printf("Profit per 100 hands %f\n", hitplayer.chips / (hitplayer.hands_played / 100));
	printf("Played %d skipped %d net %f bet %f net/bet %f\n", standplayer.hands_played, standplayer.hands_skipped, standplayer.chips, standplayer.total_bet, standplayer.chips / standplayer.total_bet);
	printf("Profit per 100 hands %f\n", standplayer.chips / (standplayer.hands_played / 100));
	*/
}

int main() {
	for (int i = 0; i < 20000; ++i) {
		wins[i] = 0;
		losses[i] = 0;
	}
	srand(time(0));
	//Strategy bja(bja_s17_das_hard, bja_s17_das_soft, bja_s17_das_pair, linear_bet);
	//Strategy me(bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, my_bets);
	Strategy pb(bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, pb_bet);
	//Strategy basic(bja_h17_das_hard, bja_h17_das_soft, bja_h17_das_pair, flat_bet);
	Shoe s(6);
	Player p;
	time_t start = time(0);

	/*
	for (int i = 0; i < 1000; ++i) {
		double_or_bust(10000);
		std::cout << "Doubled " << doubled << " bust " << bust << std::endl;
	}
	*/
	/*
	std::cout << "Doubled " << doubled << " bust " << bust << std::endl;
	std::cout << "Avg hands to double " << (doubled == 0 ? 0 : (hands_to_doubled / doubled)) << std::endl;
	std::cout << "Avg hands to bust " << (bust == 0 ? 0 : (hands_to_bust / bust)) << std::endl;
	*/
	/*
	ifstream shoes("raw_shoes.txt");
	string shoe_raw;
	while (!shoes.eof()) {
		shoes >> shoe_raw;
		s.set(shoe_raw.c_str());
		std::cout << s << std::endl;
		while (s.shoe_index < 156) {
			if (uselog == 1) std::cout << "===========================" << p.chips << std::endl;
			play_hand(&s, &p, &bja, house_s17);
		}
	}
	*/
	figure_basic_hard(18, 1000000);
	/*
	while (p.hands_played < 100000000) {
		if (s.shoe_index > 260) {
			s.shuffle();
		}
		play_hand(&s, &p, &pb, house_h17);
	}
	time_t end = time(0);
	printf("%lld secs\n", (end - start));
	printf("Played %d skipped %d net %f bet %f net/bet %f\n", p.hands_played, p.hands_skipped, p.chips, p.total_bet, p.chips / p.total_bet);
	printf("Profit per 100 hands %f\n", p.chips / (p.hands_played / 100));
	*/
	/*
	money won = 0.0;
	for (int i = 0; i < 20000; ++i) {
		if (wins[i] != 0)
		std::cout << "Win\t" << i << "\t" << wins[i] << "\t" << wins[i] * i << std::endl;
		won += wins[i] * i;
	}
	money lost = 0.0;
	for (int i = 0; i < 20000; ++i) {
		if (losses[i] != 0)
		std::cout << "Losses\t" << i << "\t" << losses[i] << "\t" << losses[i] * i << std::endl;
		lost += losses[i] * i;
	}
	std::cout << "Total wins\t" << won << std::endl << "Total losses\t" << lost << "Net\t" << (won - lost) << std::endl;
	std::cout << "Corrected\t" << ((won - lost) / 2) << std::endl;
	*/
}