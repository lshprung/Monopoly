#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "color.h"

int dice_roll(int); //what happens when you roll the dice, returns the value of the dice and whether its a double
void space_action(int); //what happens when you land on the space
void property_action(int); //what happens when you land on a property
void check_monopoly(int); //function to check if a monopoly has been made (the argument is the space that has just been acquired)
void game_stats(int); //allow for the viewing of various game stats, needs to know which game stat to show
void transaction(int, int, int);
//handles all transactions in game
//first int = player gaining money (-1 if bank)
//second int = player losing money (-1 if bank)
//third int = amount 
void go_to_jail(int); //handles whenever a player is sent to jail
void build_house(int); //when a player wants to build a house/hotel
void trade_deal(int); //when a player wants to initiate a trade deal
void chance(int); //what hapens when you land on CHANCE
void community_chest(int); //what happens when you land on COMMUNITY CHEST
void chance_shuffle(void); //shuffles chance cards
void cc_shuffle(void); //shuffles community chest cards
int determine_blanks(void); //determines what goes in the blank_count array, returns the size of the biggest name

//to do: create array to show whether a player is in or not (1 or 0) and use that (instead of int player_count) to determine payouts (such as opening night seats chance card)
char player_names[8][100]; 
int player_colors[] = {14, 17, 15, 16, 18, 19, 20, 13};
int blank_count[8]; //how many spaces for proper even spacing for each name
int player_cash[8] = {-1, -1, -1, -1, -1, -1, -1, -1}; //set to -1 if the player is eliminated from the game
int player_space[8];
int property_ownership[28]; //-1 = unowned
int monopoly_status[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int group_reps[] = {0, 2, 5, 8, 11, 14, 17, 20}; //properties that represent their respective group (to determine who owns that monopoly
int house_count[28]; //number of houses on each property
//8 property groups (0 if not a monopoly (all owned by 1 player), 1 if yes), if it is set to 1, building houses becomes possible
int goojf_ownership[] = {-1, -1};
//get out of jail free cards: 0th is from chance deck, 1th is cc deck
//-1 = unowned, player # otherwise
int jail_status[8]; //0 if player is not in jail, 1 if he is
char space_names[][22] = {"GO", "Mediterranean Avenue", "Community Chest", "Baltic Avenue", "Income Tax", "Reading Railroad", "Oriental Avenue", "Chance", "Vermont Avenue", "Connecticut Avenue", "Just Visiting!", "St. Charles Place", "Electric Company", "States Avenue", "Virginia Avenue", "Pennsylvania Railroad", "St. James Place", "Community Chest", "Tennessee Avenue", "New York Avenue", "FREE PARKING", "Kentucky Avenue", "Chance", "Indiana Avenue", "Illinois Avenue", "B & O Railroad", "Atlantic Avenue", "Ventnor Avenue", "Water Works", "Marvin Gardens", "GO TO JAIL", "Pacific Avenue", "North Carolina Avenue", "Community Chest", "Pennsylvania Avenue", "Short Line", "Chance", "Park Place", "Luxury Tax", "Boardwalk"};
char prop_names[][22] = {"Mediterranean Avenue", "Baltic Avenue", "Oriental Avenue", "Vermont Avenue", "Connecticut Avenue", "St. Charles Place", "States Avenue", "Virginia Avenue", "St. James Place", "Tennessee Avenue", "New York Avenue", "Kentucky Avenue", "Indiana Avenue", "Illinois Avenue", "Atlantic Avenue", "Ventnor Avenue", "Marvin Gardens", "Pacific Avenue", "North Carolina Avenue", "Pennsylvania Avenue", "Park Place", "Boardwalk", "Reading Railroad", "Pennsylvania Railroad", "B & O Railroad", "Short Line", "Electric Company", "Water Works"}; //array for properties (no GO, JUST VISITING, etc.)
int chance_deck[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
int cc_deck[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
int player_count = 0; //how many players
int longest_name; //length in chars of the longest entered name
int dice_roll_over;
int doubles_count; //counts number of doubles rolled (if this number exceeds 2, go directly to jail)
int jail_roll_count[8]; //counts number of attempts to roll out of jail for each player
int chance_remaining_cards = 15;
int cc_remaining_cards = 15;

// Space Guide //
// 00 = GO
// 01 = Mediterranean Avenue
// 02 = Community Chest
// 03 = Baltic Avenue
// 04 = Income Tax
// 05 = Reading Railroad
// 06 = Oriental Avenue
// 07 = Chance
// 08 = Vermont Avenue
// 09 = Connecticut Avenue
// 10 = Just Visiting (JAIL)
// 11 = St. Charles Place
// 12 = Electric Company
// 13 = States Avenue
// 14 = Virginia Avenue
// 15 = Pennsylvania Railroad
// 16 = St. James Place
// 17 = Community Chest
// 18 = Tennessee Avenue
// 19 = New York Avenue
// 20 = FREE PARKING
// 21 = Kentucky Avenue
// 22 = Chance
// 23 = Indiana Avenue
// 24 = Illinois Avenue
// 25 = B & O Railroad
// 26 = Atlantic Avenue
// 27 = Ventnor Avenue
// 28 = Water Works
// 29 = Marvin Gardens
// 30 = GO TO JAIL
// 31 = Pacific Avenue
// 32 = North Carolina Avenue
// 33 = Community Chest
// 34 = Pennsylvania Avenue
// 35 = Short Line
// 36 = Chance
// 37 = Park Place
// 38 = Luxury Tax
// 39 = Boardwalk

// Property Ownership Array Guide //
// 00 = Mediterranean Avenue
// 01 = Baltic Avenue
// 02 = Oriental Avenue
// 03 = Vermont Avenue
// 04 = Connecticut Avenue
// 05 = St. Charles Place
// 06 = States Avenue
// 07 = Virginia Avenue
// 08 = St. James Place
// 09 = Tennessee Avenue
// 10 = New York Avenue
// 11 = Kentucky Avenue
// 12 = Indiana Avenue
// 13 = Illinois Avenue
// 14 = Atlantic Avenue
// 15 = Ventnor Avenue
// 16 = Marvin Gardens
// 17 = Pacific Avenue
// 18 = North Carolina Avenue
// 19 = Pennsylvania Avenue
// 20 = Park Place
// 21 = Boardwalk
// 22 = Reading Railroad (here begins non-buildable properties)
// 23 = Pennsylvania Railroad
// 24 = B & O Railroad
// 25 = Short Line
// 26 = Electric Company
// 27 = Water Works

int main(){
	int player_turn = 0; //who's turn is it?
	int int_input; //any time the user needs to input a single number
	int dice_roll_value; //used to store output of dice_roll for the case of the player in jail
	int turn_over = 0; //used to determine when a player ends their turn
	int case_flag = 1; //used to consider options only available from jail
	int *p = &player_cash[0]; //pointer for if a player is in or not
	int i;
	
	// Initializations //
	for(i = 0; i < 28; i++){ //all properties initially owned by the bank
		property_ownership[i] = -1;
		house_count[i] = 0;
	}
	
	srand((int) time(NULL));

	chance_shuffle();
	cc_shuffle();

	while(1){
		printf("How many players? ");
		fflush(stdout);
		scanf("%d", &player_count);
		if(player_count > 1 && player_count <= 8) break;
		else printf("Please enter a player count between 2 and 8\n");
	}

	for(i = 0; i < player_count; i++){ //initialize each player
		printf("Enter a name for Player %d: ", i+1);
		fflush(stdout);
		scanf("%s", player_names[i]);
		player_cash[i] = 1500; //player is given $1500 starting cash
		player_space[i] = 0; //player is initialized on the first space (GO)
		jail_status[i] = 0; //player begins outside of jail
	}

	longest_name = determine_blanks();

	// Getting into Gameplay //
	// for now, assume players play in order
	while(player_count > 1){ //when player count == 1, the game is over (because there is only one person left alive)
		turn_over = 0;
		dice_roll_over = 0;
		doubles_count = 0; //reset doubles counter each turn

		while(!turn_over && player_cash[player_turn] >= 0){ //ensure the turn is not over AND that the player has not gone bankrupt
			change_color(player_colors[player_turn]);
			printf("  ");
			change_color(13); //reset background
			printf("It is %s's turn!", player_names[player_turn]);
			change_color(player_colors[player_turn]);
			printf("  ");
			change_color(13); //reset background
			printf("\n");

			if(jail_status[player_turn]) printf("\n%s is in JAIL\n", player_names[player_turn]);

			printf("\n");
			if(!dice_roll_over){
				printf("1) Roll the Dice");
				if(jail_status[player_turn]){ //if player is in jail
					printf(" to get out of JAIL\n"); 
					change_color(0);
					printf("2) Pay $50 Fine to get out of JAIL");
					if(goojf_ownership[0] == player_turn ||  goojf_ownership[1] == player_turn) printf("\n3) Use Get Out of Jail Free Card to get out of JAIL");
				}
				printf("\n");
			}
			printf("4) View Game Stats: Money\n");
			printf("5) View Game Stats: Property Ownership and Information\n");
			printf("6) View Game Stats: Board Info\n");
			printf("7) Build/Sell Houses/Hotels\n"); //maybe make a conditional to decide whether this option appears
			printf("8) Initiate a Trade Deal\n");
			if(dice_roll_over) printf("9) End Turn\n");
			fflush(stdout);
			scanf(" %d", &int_input);
			printf("\n");

			switch(int_input){

				case 4:
				case 5:
				case 6:
					game_stats(int_input-3);
					break;

				case 7:
					build_house(player_turn);
					break;

				case 8:
					trade_deal(player_turn);
					break;

				default:
					if(!dice_roll_over){

						if(int_input == 2 && jail_status[player_turn]){ //player has payed to get out of jail
							transaction(-1, player_turn, 50);
							jail_status[player_turn] = 0;
							printf("%s is no longer in JAIL\n\n", player_names[player_turn]);
						}

						else if(int_input == 3 && jail_status[player_turn] && (goojf_ownership[0] == player_turn || goojf_ownership[1] == player_turn)){ //player has used a get out of jail free card to escape jail
							if(goojf_ownership[0] == player_turn) goojf_ownership[0] == -1;
							else goojf_ownership[1] = -1;
							jail_status[player_turn] = 0;
							printf("DEBUG OUTPUT: goojf chance: %s\ngoojf cc: %s\n", (goojf_ownership[0] >= 0 ? player_names[goojf_ownership[0]] : "the bank"), (goojf_ownership[1] >= 0 ? player_names[goojf_ownership[1]] : "the bank"));
							printf("%s is no longer in JAIL\n\n", player_names[player_turn]);
						}

						else{
							dice_roll_value = dice_roll(player_turn); //player has rolled the dice (either to attempt to escape jail or to move)

							if(doubles_count < 3){ //ensure that the player did not just roll doubles a third time and went to jail
								if(jail_status[player_turn]){ //is the player in jail?

									if(dice_roll_over){
										jail_roll_count[player_turn]++;
										if(jail_roll_count[player_turn] < 3) printf("%s shall remain in JAIL\n\n", player_names[player_turn]); //did not roll doubles
										if(jail_roll_count[player_turn] == 3){ //player has attempted to roll doubles unsuccessfully 3 times and is forced to pay the fine
											printf("%s, you have attempted to roll doubles unsuccessfully 3 times and must now pay the $50 fine\n", player_names[player_turn]);
											transaction(-1, player_turn, 50);
											printf("%s is no longer in JAIL\n\n", player_names[player_turn]);
											jail_status[player_turn] = 0;
											dice_roll_value = dice_roll(player_turn); //reroll now that out of jail
										}
									}
									else{
										printf("%s is no longer in JAIL\n", player_names[player_turn]);
										jail_status[player_turn] = 0;
										dice_roll_over = 1; //to ensure rolling a doubles only gets you out of jail and nothing else
									}
								}

								if(!jail_status[player_turn]){
									player_space[player_turn] += dice_roll_value;
									if(player_space[player_turn] > 39){
										printf("%s passed GO! Collect $200\n", player_names[player_turn]);
										transaction(player_turn, -1, 200); //player passed go (collect $200)
										player_space[player_turn] %= 40;
									}
									printf("\n%s landed on %s\n", player_names[player_turn], space_names[player_space[player_turn]]);
									space_action(player_turn);
								}
							}

							else printf("\n");
						}
					}

					else{
						turn_over = 1;
						while(1){
							player_turn++;
							p++;
							if(player_turn >= 8){
								player_turn = 0;
								p = &player_cash[0];
							}
							if(*p >= 0) break;
						}
						printf("TEST: player_turn = %d\n", player_turn);
						printf("\n");
					}

			}
		}
	}

	return 0;
}

int dice_roll(int player){
	int dice1, dice2;

	//some shit here is not working. I don't want to have to rely on flooding the code with fflush()

	if(doubles_count == 2) printf("Be Careful! One more doubles and you go to JAIL...\n"); //add changing color, maybe change place of this statement (or fix fgetc bug)
	printf("%s Roll!\n", player_names[player]);
	fflush(stdout);
	fgetc(stdin);
	fflush(stdout);

	dice1 = (rand() % 6)+1;
	dice2 = (rand() % 6)+1;
	if(dice1 == dice2){
		doubles_count++;
		printf("Doubles! ");
	}
	else dice_roll_over = 1;
	printf("%s rolled %d", player_names[player], dice1+dice2);
	if(doubles_count == 3){
		printf("\nYou rolled 3 doubles. %s, GO TO JAIL\n", player_names[player]);
		go_to_jail(player);
	}
	fflush(stdout);
	fgetc(stdin);
	fflush(stdout);

	return (dice1+dice2);
}

void space_action(int player){
	int space = player_space[player];

	switch(space){
		case 0:
			printf("Collect $200");
			//if land on go = $400 mode is on, set accordinly
			break;

		case 10:
			printf("You are just visiting :)");
			break;

		case 20:
			printf("Relax, its Free Parking!");
			//if collect jackpot rule is on, set accordingly
			break;

		case 30:
			go_to_jail(player);
			break;

		case 7:
		case 22:
		case 36:
			chance(player);
			break;

		case 2:
		case 17:
		case 33:
			community_chest(player);
			break;

		case 4:
			printf("You must pay $200 income tax\n");
			transaction(-1, player, 200);
			break;

		case 38:
			printf("You must pay $75 luxury tax\n");
			transaction(-1, player, 75);
			break;

		default:
			property_action(player);

	}

	printf("\n\n");
	
	return;
}

void property_action(int player){
	//printf("WIP");

	// Property Array Guide //
	// 00 = Mediterranean Avenue                                    --> 0 property/monopoly group
	// 01 = Baltic Avenue                                           --> 0 property/monopoly group
	// 02 = Oriental Avenue                                         --> 1 property/monopoly group
	// 03 = Vermont Avenue                                          --> 1 property/monopoly group
	// 04 = Connecticut Avenue                                      --> 1 property/monopoly group
	// 05 = St. Charles Place                                       --> 2 property/monopoly group
	// 06 = States Avenue                                           --> 2 property/monopoly group
	// 07 = Virginia Avenue                                         --> 2 property/monopoly group
	// 08 = St. James Place                                         --> 3 property/monopoly group
	// 09 = Tennessee Avenue                                        --> 3 property/monopoly group
	// 10 = New York Avenue                                         --> 3 property/monopoly group
	// 11 = Kentucky Avenue                                         --> 4 property/monopoly group
	// 12 = Indiana Avenue                                          --> 4 property/monopoly group
	// 13 = Illinois Avenue                                         --> 4 property/monopoly group
	// 14 = Atlantic Avenue                                         --> 5 property/monopoly group
	// 15 = Ventnor Avenue                                          --> 5 property/monopoly group
	// 16 = Marvin Gardens                                          --> 5 property/monopoly group
	// 17 = Pacific Avenue                                          --> 6 property/monopoly group
	// 18 = North Carolina Avenue                                   --> 6 property/monopoly group
	// 19 = Pennsylvania Avenue                                     --> 6 property/monopoly group
	// 20 = Park Place                                              --> 7 property/monopoly group
	// 21 = Boardwalk                                               --> 7 property/monopoly group
	// 22 = Reading Railroad (here begins non-buildable properties) 
	// 23 = Pennsylvania Railroad                                   
	// 24 = B & O Railroad                                          
	// 25 = Short Line                                              
	// 26 = Electric Company                                        
	// 27 = Water Works                                             

	int price[] = {60, 60, 100, 100, 120, 140, 140, 160, 180, 180, 200, 220, 220, 240, 260, 260, 280, 300, 300, 320, 350, 400, 200, 200, 200, 200, 150, 150}; //price of each unowned property
	int rent[][6] = {{2, 10, 30, 90, 160, 250}, {4, 20, 60, 180, 320, 450}, {6, 30, 90, 270, 400, 550}, {6, 30, 90, 270, 400, 550}, {8, 40, 100, 300, 450, 600}, {10, 50, 150, 450, 625, 750}, {10, 50, 150, 450, 625, 750}, {12, 60, 180, 500, 700, 900}, {14, 70, 200, 550, 750, 950}, {14, 70, 200, 550, 750, 950}, {16, 80, 220, 600, 800, 1000}, {18, 90, 250, 700, 875, 1050}, {18, 90, 250, 700, 875, 1050}, {20, 100, 300, 750, 925, 1100}, {22, 110, 330, 800, 975, 1150}, {22, 110, 330, 800, 975, 1150}, {24, 120, 360, 850, 1025, 1200}, {26, 130, 390, 900, 1100, 1275}, {26, 130, 390, 900, 1100, 1275}, {28, 150, 450, 1000, 1200, 1400}, {35, 175, 500, 1100, 1300, 1500}, {50, 200, 600, 1400, 1700, 2000}, {25, 50, 100, 200, 0, 0}, {25, 50, 100, 200, 0, 0}, {25, 50, 100, 200, 0, 0}, {25, 50, 100, 200, 0, 0}};
	//rent index guide:
	//0 = rent with 0 houses/1 railroad
	//1 = rent with 1 house/2 railroads
	//2 = '    '    2 '     3 '
	//3 = '    '    3 '     4 '
	//4 = '    '    4 '
	//5 = rent with hotel
	
	//to add: mortgage prices
	
	int space_conversion[][3] = {{1, 0, 0}, {3, 1, 0}, {5, 22, 8}, {6, 2, 1}, {8, 3, 1}, {9, 4, 1}, {11, 5, 2}, {12, 26, 9}, {13, 6, 2}, {14, 7, 2}, {15, 23, 8}, {16, 8, 3}, {18, 9, 3}, {19, 10, 3}, {21, 11, 4}, {23, 12, 4}, {24, 13, 4}, {25, 24, 8}, {26, 14, 5}, {27, 15, 5}, {28, 27, 9}, {29, 16, 5}, {31, 17, 6}, {32, 18, 6}, {34, 19, 6}, {35, 25, 8}, {37, 20, 9}, {39, 21, 9}};
	//space_conversion index guide:
	//0 = literal space
	//1 = space according to property index
	//2 = group the property belongs to (8 is railroads, 9 is utilities)
	
	int space;
	int group;
	int final_cost; //final cost assuming player owns the property after determining houses and monopolies
	int done = 0; //used to end this menu if options 1 or 2 are submitted
	int input;
	int i = 0;

	while(1){
		if(player_space[player] == space_conversion[i][0]){ //search for space that player landed on
			space = space_conversion[i][1]; //convert it from the array
			group = space_conversion[i][2]; //store the group that the property belongs to
			break;
		}
		i++;
	}

	if(property_ownership[space] == -1){ //property owned by bank, ask player if they would like to buy it
		while(!done){
			printf("%s is unowned! Would you like to buy it, %s?\n\n", space_names[player_space[player]], player_names[player]);
			printf("1) Pay $%d to buy the property\n", price[space]);
			printf("2) Decline\n");
			printf("3) View Property Info\n");
			printf("4) View Game Stats: Money\n");
			printf("5) View Game Stats: Property Ownership and Information\n");
			printf("6) View Game Stats: Board Info\n");
			fflush(stdout);
			scanf(" %d", &input); //to be changed to avoid having to press enter key
			printf("\n");

			switch(input){
				case 1:
					if(price[space] > player_cash[player]) printf("%s, you do not have enough money to buy this property!\n\n", player_names[player]);
					else{
						transaction(-1, player, price[space]);
						property_ownership[space] = player;
						change_color(4);
						printf("%s is now the owner of %s", player_names[player], space_names[player_space[player]]);
						change_color(0);
						if(space <= 21) check_monopoly(space); //function will check if a monopoly has been made and update the monopoly_status array accordingly
						done++;
					}
					break;

				case 2:
					done++;
					break;

				case 4:
				case 5:
				case 6:
					game_stats(input-3);
					break;

				default:
					printf("%s costs $%d to own\n\n", space_names[player_space[player]], price[space]);
					printf("     \"Title Deed\n%s\nRent $%d\nWith 1 House      $ %d.\nWith 2 Houses      %s%d.\nWith 3 Houses      %s%d.\nWith 4 Houses      %d\n     With Hotel $%d.\n   Mortgage Value $%d.\n  Houses cost $%d. each\nHotels, $%d. plus 4 houses\"\n\n", space_names[player_space[player]], rent[space][0], rent[space][1], (rent[space][2] < 100 ? " " : ""), rent[space][2], (rent[space][3] < 100 ? " " : ""), rent[space][3], rent[space][4], rent[space][5], 0, 0, 0);
					//be sure to add Title Deeds for Railroads and Utilities

			}
		}
	}

	else{
		final_cost = rent[space][house_count[space]]; //calculate how much it costs to land on this property
		if(!house_count[space] && monopoly_status[group]) final_cost*=2; //double this if it is a monopoly with 0 houses
		if(property_ownership[space] == player) printf("Relax, you own this property\n"); //if player owns the property
		else{ //if somebody else owns the property
			printf("%s owns %s. %s owes %s $%d\n", player_names[property_ownership[space]], space_names[player_space[player]], player_names[player], player_names[property_ownership[space]], final_cost); 
			transaction(property_ownership[space], player, final_cost); 
		}
	}

	return;
}

void check_monopoly(int space){
	int flag = 0; //set to 1 if a monopoly has been made
	int size; //is it a 2 or 3 property group?
	int group; //what group does this space belong to?
	char group_name[][13] = {"PURPLE/BROWN", "LIGHT BLUE", "PINK", "ORANGE", "RED", "YELLOW", "GREEN", "DARK BLUE"};
	int owner; //owner of the property (by int value)
	int i;

	switch(space){
		case 0:
		case 1:
			size = 2;
			space = 0;
			group = 0;
			break;
		case 2:
		case 3:
		case 4:
			size = 3;
			space = 2;
			group = 1;
			break;
		case 5:
		case 6:
		case 7:
			size = 3;
			space = 5;
			group = 2;
			break;
		case 8:
		case 9:
		case 10:
			size  = 3;
			space = 8;
			group = 3;
			break;
		case 11:
		case 12:
		case 13:
			size = 3;
			space = 11;
			group = 4;
			break;
		case 14:
		case 15:
		case 16:
			size  = 3;
			space = 14;
			group = 5;
			break;
		case 17:
		case 18:
		case 19:
			size  = 3;
			space = 17;
			group = 6;
			break;
		case 20:
		case 21:
			size = 2;
			space = 20;
			group = 7;
			break;
		default:
			printf("ERROR: invalid argument\n");
			return;
	}

	owner = property_ownership[space];

	for(i = size; i >= 2; i--){ //check if each property is owned by the same person
		space++;
		if(owner != property_ownership[space]) return;
	}

	//a monopoly has been made:
	monopoly_status[group] = 1;
	printf("\n%s now has a monopoly on the %s property group!\n", player_names[property_ownership[space]], group_name[group]);

	return;
}

void game_stats(int option){
	int group;
	int i;
	int j;

	// Property Array Guide //
	// 00 = Mediterranean Avenue
	// 01 = Baltic Avenue
	// 02 = Oriental Avenue
	// 03 = Vermont Avenue
	// 04 = Connecticut Avenue
	// 05 = St. Charles Place
	// 06 = States Avenue
	// 07 = Virginia Avenue
	// 08 = St. James Place
	// 09 = Tennessee Avenue
	// 10 = New York Avenue
	// 11 = Kentucky Avenue
	// 12 = Indiana Avenue
	// 13 = Illinois Avenue
	// 14 = Atlantic Avenue
	// 15 = Ventnor Avenue
	// 16 = Marvin Gardens
	// 17 = Pacific Avenue
	// 18 = North Carolina Avenue
	// 19 = Pennsylvania Avenue
	// 20 = Park Place
	// 21 = Boardwalk
	// 22 = Reading Railroad (here begins non-buildable properties)
	// 23 = Pennsylvania Railroad
	// 24 = B & O Railroad
	// 25 = Short Line
	// 26 = Electric Company
	// 27 = Water Works
	
	int prop_blanks[] = {1, 8, 6, 7, 3, 4, 8, 6, 6, 5, 6, 6, 7, 6, 6, 7, 7, 7, 0, 2, 11, 12, 5, 0, 7, 11, 5, 10};
	int color_blanks[] = {0, 2, 8, 6, 9, 6, 7, 3};
	//array for proper spacing when printing property statuses

	change_color(0);

	printf("\n");

	switch(option){
		case 1:
			for(i = 0; i < 8; i++){
				if(player_cash[i] >= 0){
					printf("%s ", player_names[i]);
					for(j = 0; j < blank_count[i]; j++){
						printf(" ");
					}
					printf("has $%d", player_cash[i]);
					change_color(player_colors[i]);
					printf("    ");
					change_color(13);
					printf("\n");
				}
			}
			printf("\n");
			break;

		case 2:
			for(i = 0; i < 28; i++){
				group = -1; //reset group each loop
				if(i == 0){
					printf("PROPERTIES\n");
				}
				if(i >= 0 && i < 22 && property_ownership[i] != -1){
					if(i <= 1){
						change_color(9);
						group = 0;
					}
					else if(i <= 4){
						change_color(12);
						group = 1;
					}
					else if(i <= 7){
						change_color(10);
						group = 2;
					}
					else if(i <= 10){
						change_color(5);
						group = 3;
					}
					else if(i <= 13){
						change_color(1);
						group = 4;
					}
					else if(i <= 16){
						change_color(6);
						group = 5;
					}
					else if(i <= 19){
						change_color(3);
						group = 6;
					}
					else if(i <= 21){
						change_color(11);
						group = 7;
					}
				}
				else if(i == 22) printf("RAILROADS\n");
				else if(i == 26) printf("UTILITIES\n");
				printf("%s: ", prop_names[i]);
				for(j = 0; j < prop_blanks[i]; j++){
					printf(" ");
				}
				printf("owned by %s", (property_ownership[i] >= 0 ? player_names[property_ownership[i]] : "the bank")); 
				if(i < 22){
					printf("; ");
					for(j = 0; j < (property_ownership[i] >= 0 ? blank_count[property_ownership[i]] : longest_name-8); j++){
						printf(" ");
					}
					printf("from group ");
				}
				if(i <= 1) printf("PURPLE/BROWN");
				else if(i <= 4) printf("LIGHT BLUE");
				else if(i <= 7) printf("PINK");
				else if(i <= 10) printf("ORANGE");
				else if(i <= 13) printf("RED");
				else if(i <= 16) printf("YELLOW");
				else if(i <= 19) printf("GREEN");
				else if(i <= 21) printf("DARK BLUE");
				if(group >= 0 && monopoly_status[group]){ //the property has a monopoly, so list the number of houses
					printf("; ");
					for(j = 0; i < color_blanks[group]; i++){
						printf(" ");
					}
					printf("%d house%s", house_count[i], (house_count[i] != 1 ? "s" : ""));
				}

				if(property_ownership[i] >= 0){ //color flag to show ownership
					change_color(player_colors[property_ownership[i]]);
					printf("    ");
				}
				//try to implement showing rent/price later
				change_color(13); //reset bg color
				change_color(0); //reset color
				printf("\n");
			}

			printf("GET OUT OF JAIL FREE CARDS\n");
			for(i = 0; i < 2; i++){
				if(!i) printf("Chance Get Out of Jail Free Card:          owned by ");
				else   printf("Community Chest Get Out of Jail Free Card: owned by ");

				if(goojf_ownership[i] >= 0){
					printf("%s", player_names[goojf_ownership[i]]);
					change_color(player_colors[goojf_ownership[i]]);
					printf("    ");
					change_color(13);
				}
				else printf("the bank");
				printf("\n");
			}
			printf("\n");
			break;

		case 3:
			//idea to make ascii art of the board
			for(i = 0; i < 8; i++){
				if(player_cash[i] >= 0){
					printf("%s ", player_names[i]);
					for(j = 0; j < blank_count[i]; j++){
						printf(" ");
					}
					printf("is ");
					if(jail_status[i]) printf("in JAIL (11th space)"); //in case the player is in jail
					else{
						printf("on the ");
						if(player_space[i]+1 < 10) printf(" ");
						printf("%d", player_space[i]+1);
						switch(player_space[i]){
							case 0:
							case 20:
							case 30:
								printf("st");
								break;
							case 1:
							case 21:
							case 31:
								printf("nd");
								break;
							case 2:
							case 22:
							case 32:
								printf("rd");
								break;
							default:
								printf("th");
						}
						printf(" space on the board: %s", space_names[player_space[i]]);
					}
					change_color(player_colors[i]);
					printf("    ");
					change_color(13);
					printf("\n");
				}
			}
			printf("\n");
			break;

		default:
			return;

	}

	return;
}

void transaction(int gaining, int losing, int amount){ //to do: add intricacies for bankruptcies
	/*
	player_cash[player] += amount;

	printf("%s now has $%d\n", player_names[player], player_cash[player]);

	*/
	int i;
	int flag = 0; //for use on goojf repossession

	if(losing != -1){ //if a player is losing money...
		if(amount <= player_cash[losing]){
			player_cash[losing] -= amount;
			change_color(2);
			printf("%s now has $%d", player_names[losing], player_cash[losing]);
			change_color(0);
			printf("\n");
		}
		else{ //if the player cannot pay what he owes
			//to do: write a function to deal with debt (for now, assumes player goes bankrupt)
			flag++;
			amount = player_cash[losing];
			player_cash[losing] = -1; //set to -1 to express that this player is no longer in the game (will be changed for cases of debt)
			player_count--; //there is 1 less player in the game now
			printf("%s has gone BANKRUPT! %s will repossess all of their belongings:\n", player_names[losing], (gaining >= 0 ? player_names[gaining] : "The bank"));
			if(gaining != -1){ //player when bankrupt to another player (as opposed to the bank)
				player_cash[gaining] += amount;
				change_color(4);
				printf("%s now has $%d", player_names[gaining], player_cash[gaining]);
				change_color(0);
				printf("\n");
			}
			for(i = 0; i < 28; i++){
				if(property_ownership[i] == losing){ //property repossession
					property_ownership[i] = gaining;
					printf("%s has repossessed (and is now the owner) of %s\n", (gaining >= 0 ? player_names[gaining] : "The bank"), prop_names[i]);
				}
			}
			for(i = 0; i < 2; i++){ //goojf card repossession
				if(goojf_ownership[i] == losing){
					goojf_ownership[i] = gaining;
					flag++;
				}
			}
			if(flag > 1) printf("%s has repossessed %s's Get Out of Jail Free Card%s\n", (gaining >= 0 ? player_names[gaining] : "The bank"), player_names[losing], (flag > 2 ? "s" : ""));
		}
	}

	if(!flag && gaining != -1){
		player_cash[gaining] += amount;
		change_color(4);
		printf("%s now has $%d", player_names[gaining], player_cash[gaining]);
		change_color(0);
		printf("\n");
	}

	return;
}

void go_to_jail(int player){
	jail_status[player] = 1;
	jail_roll_count[player] = 0; //reset number of attempts to roll out of jail (max 3 rolls before paying)
	player_space[player] = 10; //player put in jail, player sent to the jail space
	dice_roll_over = 1; //dice roll is immediately over
	printf("%s is now in JAIL\n", player_names[player]);
}

void build_house(int player){
	//printf("WIP\n");

	int flag; //used to ensure the player has at least one monopoly
	char group_name[][13] = {"PURPLE/BROWN", "LIGHT BLUE", "PINK", "ORANGE", "RED", "YELLOW", "GREEN", "DARK BLUE"};
	int house_costs[8] = {50, 50, 100, 100, 150, 150, 200, 200};
	int valid_build[3]; //used to ensure that you are not ahead in building in any one property
	int option; //regular option
	int group_option; //player input, necessary to remember since it relates to the group in question
	int group_prop_count; //2 or 3
	int starting_prop; //first property in the group
	int valid_option[6]; //make it easier to determine if a selected option is valid
	int prop; //later used to go through prop_names
	int *p; //used to go through monopoly statuses
	int *h; //used later for going through house counts
	int i;

	while(1){
		i = 0;
		p = &monopoly_status[0];
		flag = 0;
		while(i < 8){
			if(*p && player == property_ownership[group_reps[i]]){ //ensure there is a monopoly owned by the player before listing the option
				if(!flag) printf("What Group do you want to Build/Sell in?\n");
				flag++;
				printf("%d) Build/Sell Houses in the %s group\n", i+1, group_name[i]);
			}
			i++;
			p++;
		}
		if(!flag){ //the player does not have any monopolies: inform them and send them back
			printf("You do not have any monopolies and cannot build or sell any houses yet\n\n");
			return;
		}

		printf("9) Done\n");
		fflush(stdout);
		scanf("%d", &group_option);
		printf("\n");

		if(group_option >= 1 && group_option <= 8){
			starting_prop = group_reps[group_option-1];
			group_prop_count = (group_option == 1 || group_option == 8 ? 2 : 3);
		}
		else return;

		if(!monopoly_status[group_option-1] || property_ownership[starting_prop] != player) return; //simulate if a plyaer selects an group_option that is invalid
		else{
			while(1){
				for(i = 0; i < 6; i++){ //reset valid options
					valid_option[i] = 0; 
				}

				printf("%s, you have $%d\n", player_names[player], player_cash[player]);
				printf("What property do you want to build or sell a house on?\n");
				prop = starting_prop;
				h = &house_count[starting_prop];
				valid_build = {1, 1, 1}; //by default, all 3 are valid builds
				if(group_prop_count == 2){ //checking for properties with more houses in size 2 groups
					if(*h > *(h+1)) valid_build[0] = 0; //prop 1 has more houses
					else if(*h < *(h+1)) valid_build[1] = 0; //prop 2 has more houses
				}
				else{ //checking for properties with more houses in size 3 groups

				for(i = 1; i <= group_prop_count*2; i+=2){
					if(*h < 5){ //make sure there isn't already a hotel (5 houses)
						printf("%d) Build a House on %s (currently %d house%s, houses cost $%d)\n", i, prop_names[prop], *h, (*h != 1 ? "s" : ""), house_costs[group_option-1]); 
						valid_option[i-1] = 1;
					}
					if(*h > 0){ //make sure there is at least one house to sell
						printf("%d) Sell  a House on %s (currently ", i+1, prop_names[prop]); 
						if(*h == 5) printf("a hotel, ");
						else printf("%d house%s, ", *h, (*h != 1 ? "s" : ""));
						printf("houses sell for $%d)\n", house_costs[group_option-1]/2);
						valid_option[i] = 1;
					}
					prop++;
					h++;
				}
				printf("9) Go Back\n");
				fflush(stdout);
				scanf("%d", &option);
				printf("\n");

				if(option >= 1 && option <= 6 && valid_option[option-1]){
					prop = starting_prop;
					h = &house_count[starting_prop];
					prop+=(option-1)/2; //determine where to point (to the correct property)
					h+=(option-1)/2; //determine where to point (to the correct house count)
					printf("\n");
					switch(option){
						case 1: //player bought a house
						case 3:
						case 5:
							transaction(-1, player, house_costs[group_option-1]);
							house_count[starting_prop+((option-1)/2)]++;
							printf("%s bought a house on %s. There ", player_names[player], prop_names[prop]);
							if(*h == 5) printf("is now a hotel ");
							else printf("%s now %d house%s ", (*h != 1 ? "are" : "is"), *h, (*h != 1 ? "s" : ""));
							printf("on %s\n\n", prop_names[prop]);
							break;

						case 2: //player sold a house
						case 4:
						case 6:
							transaction(player, -1, house_costs[group_option-1]/2);
							house_count[starting_prop+((option-1)/2)]--;
							printf("%s sold a house on %s. There %s now %d houses on %s\n\n", player_names[player], prop_names[prop], (*h != 1 ? "are" : "is"), *h, prop_names[prop]);
							break;

						default:
							printf("ERROR: Shouldn't get here\n");

					}
				}
				else break;
			}
		}
	}
	return;
}

void trade_deal(int player){
	printf("WIP\n");

	return;
}

void chance(int player){
	//printf("WIP");

	// Chance Guide //
	// 00 = Go Back 3 Spaces
	// 01 = Advance to Illinois Avenue
	// 02 = Pay Poor Tax of $15
	// 03 = Take a Ride on the Reading
	// 04 = Get out of Jail Free Card
	// 05 = Bank Pays You Dividend of $50
	// 06 = Advance to Go (Collect $200)
	// 07 = Take a Walk of the Board Walk Advance Token to Board Walk
	// 08 = Your Building and Loan Matures Collect $150
	// 09 = Advance to St. Charles Place If you pass GO, Collect $200
	// 10 = Go Directly to Jail...
	// 11 = You have been Elected Chairman of the Board Pay Each Player $50
	// 12 = Advance Token to Nearest Utility...
	// 13 = Make General Repairs...
	// 14 = Advance token to the nearest railroad...
	// 15 = Advance token to the nearest railroad...
	
	int card = chance_deck[chance_remaining_cards];
	int total = 0; //for use in general repairs
	int j;
	int i;

	chance_remaining_cards--;
	if(chance_remaining_cards < 0){
		chance_shuffle();
		chance_remaining_cards = 15;
	}

	change_color(5); //change card text color to yellow
	switch(card){ 
		case 0:
			printf("\"Go Back 3 Spaces\"\n");
			change_color(0);
			player_space[player] -= 3;
			printf("%s is now on %s\n", player_names[player], space_names[player_space[player]]);
			break;

		case 1:
			printf("\"Advance to Illinois Ave.\"\n");
			change_color(0);
			player_space[player] = 24;
			printf("%s is now on %s\n", player_names[player], space_names[player_space[player]]);
			property_action(player);
			break;

		case 2:
			printf("\"Pay Poor Tax of $15\"\n");
			change_color(0);
			transaction(-1, player, 15);
			break;

		case 3:
			printf("\"Take a Ride on the Reading\nIf You pass GO Collect $200\"\n");
			change_color(0);
			if(player_space[player] > 5){
				printf("%s passed GO! Collect $200\n", player_names[player]);
				transaction(player, -1, 200);
			}
			player_space[player] = 5;
			printf("%s is now on %s\n", player_names[player], space_names[player_space[player]]);
			property_action(player);
			break;

		case 4:
			printf("\"This Card may be Kept Until Needed or Sold\nGet Out of Jail Free\"\n");
			change_color(0);
			if(goojf_ownership[0] == -1) goojf_ownership[0] = player;
			else chance(player);
			break;

		case 5:
			printf("\"Bank Pays You Dividend of $50\"\n");
			change_color(0);
			transaction(player, -1, 50);
			break;

		case 6:
			printf("\"Advance to GO\n(Collect $200)\"\n");
			change_color(0);
			player_space[player] = 0;
			transaction(player, -1, 200);
			printf("%s is now on %s\n", player_names[player], space_names[player_space[player]]);
			break;

		case 7:
			printf("\"Take a Walk on the Board Walk\nAdvance Token to Board Walk\"\n");
			change_color(0);
			player_space[player] = 39;
			printf("%s is now on %s\n", player_names[player], space_names[player_space[player]]);
			property_action(player);
			break;

		case 8:
			printf("\"Your Building and Loan Matures\nCollect $150\"\n");
			change_color(0);
			transaction(player, -1, 150);
			break;

		case 9:
			printf("\"Advance to St. Charles Place\nIf you pass GO, Collect $200\"\n");
			change_color(0);
			if(player_space[player] > 11){
				printf("%s passed GO! Collect $200\n", player_names[player]);
				transaction(player, -1, 200);
			}
			player_space[player] = 11;
			printf("%s is now on %s\n", player_names[player], space_names[player_space[player]]);
			property_action(player);
			break;

		case 10:
			printf("\"Go Directly to Jail\nDo Not Pass Go, Do Not Collect $200\"\n");
			change_color(0);
			go_to_jail(player);
			break;

		case 11:
			printf("\"You have been Elected Chairman of the Board\nPay Each Player $50\"\n");
			change_color(0);
			printf("%s owes %d players $50 (totalling $%d)\n", player_names[player], player_count-1, 50 * (player_count-1));
			for(i = 0; i < 8; i++){
				if(player_cash[i] >= 0){
					if(player != i) transaction(i, player, 50);
				}
			}
			break;

		case 12:
			printf("\"Advance Token to Nearest Utility.\nIf Unowned you may buy it from bank.\nIf Owned, throw dice and pay owner a total then times the amount thrown\"\n");
			change_color(0);
			if(player_space[player] > 12 && player_space[player] <= 28){
				player_space[player] = 28;
				i = 27; //index of Water Works in Property Ownership
			}
			else{
				player_space[player] = 12;
				i = 26; //index of Electric Company in Property Ownership
			}
			printf("%s is now on %s\n", player_names[player], space_names[player_space[player]]);
			if(property_ownership[i] == -1) property_action(player);
			else printf("WIP");
			break;

		case 13:
			printf("\"Make General Repairs on All Your Property\nFor each House Pay $25\nFor Each Hotel Pay $100\"\n");
			change_color(0);
			//printf("WIP"); //need to set up houses and such first
			for(i = 0; i < 8; i++){
				if(monopoly_status[i] && property_ownership[group_reps[i]] == player){ //is there a monopoly, and does the player own it(there can't be any houses if there isn't
					switch(i){
						case 0:
						case 7:
							j = 1; //j+1 properties in the group
							break;

						default:
							j = 2;
					}
					for(j; j >= 0; j--){
						if(house_count[group_reps[i]+j] < 5) total+=(house_count[group_reps[i]+j] * 25);
						if(house_count[group_reps[i]+j] == 5) total+=100;
					}
				}
			}
			if(total){
				printf("%s must pay $%d in repairs\n", player_names[player], total);
				transaction(-1, player, total);
			}
			else printf("%s does not own any houses or hotels\n", player_names[player]);
			break;


		default:
			printf("\"Advance token to the nearest Railroad and pay owner Twice the Rental to which he is otherwise entitled.\nIf Railroad is unowned, you may buy if from the Bank.\"\n");
			change_color(0);
			while(1){
				player_space[player]++;
				if(player_space[player] % 5 == 0 && player_space[player] % 2 != 0) break;
			}
			printf("%s is now on %s\n", player_names[player], space_names[player_space[player]]);
			property_action(player); //later, change this by either adding an optional argument to double the transaction or manualize it

		}
	return;
}

void community_chest(int player){
	//printf("WIP");

	// Community Chest Guide //
	// 00 = You Inherit $100
	// 01 = From Sale of Stock You Get $45
	// 02 = You have won Second Prize in a Beauty Contest Collect $10
	// 03 = Get out of Jail Free Card
	// 04 = Bank Error in your Favor Collect $200
	// 05 = Grand Opera Opening Collect $50 from each Player
	// 06 = Receive for Services $25
	// 07 = Go Directly to Jail...
	// 08 = Pay School Tax of $150
	// 09 = You are assessed for street repairs...
	// 10 = Pay Hospital $100
	// 11 = Advance to GO (Collect $200)
	// 12 = Income Tax Refund (Collect $20)
	// 13 = Doctor's Fee Pay $50
	// 14 = XMAS fund Matures Collect $100
	// 15 = Life Insurance Matures Collect $100
	
	int card = cc_deck[cc_remaining_cards];
	int total = 0;
	int j;
	int i;

	cc_remaining_cards--;
	if(cc_remaining_cards < 0){
		cc_shuffle();
		cc_remaining_cards = 15;
	}

	change_color(5); //make the color of the card's text yellow
	switch(card){ 
		case 0:
			printf("\"You Inherit $100\"\n");
			change_color(0);
			transaction(player, -1, 100);
			break;

		case 1:
			printf("\"From Sale of Stock You Get $45\"\n");
			change_color(0);
			transaction(player, -1, 45);
			break;

		case 2:
			printf("\"You Have Won Second Prize in a Beauty Contest\nCollect $10\"\n");
			change_color(0);
			transaction(player, -1, 10);
			break;

		case 3:
			printf("\"Get Out of Jail, Free\nThis Card may be Kept Until Needed or Sold\"\n");
			change_color(0);
			if(goojf_ownership[1] == -1) goojf_ownership[1] = player;
			else community_chest(player);
			break;

		case 4:
			printf("\"Bank Error in Your Favor\nCollect $200\"\n");
			change_color(0);
			transaction(player, -1, 200);
			break;

		case 5:
			printf("\"Grand Opera Opening\nCollect $50 from every Player for Opening Night Seats\"\n");
			change_color(0);
			printf("%s shall collect $50 from %d players (totalling $%d)\n", player_names[player], player_count-1, 50 * (player_count-1));
			for(i = 0; i < 8; i++){
				if(player_cash[i] >= 0){
					if(player != i) transaction(player, i, 50);
				}
			}
			break;

		case 6:
			printf("\"Receive for Services $25\"\n");
			change_color(0);
			transaction(player, -1, 25);
			break;

		case 7:
			printf("\"Go To Jail\nGo Directly to Jail\nDo Not Pass Go\nDo Not Collect $200\"\n");
			change_color(0);
			go_to_jail(player);
			break;

		case 8:
			printf("\"Pay School Tax of $150\"\n");
			change_color(0);
			transaction(-1, player, 150);
			break;

		case 9:
			printf("\"You are Assessed for Street Repairs\n$40 Per House\n$115 per Hotel\"\n");
			change_color(0);
			//printf("WIP"); //need to set up houses and such first
			for(i = 0; i < 8; i++){
				if(monopoly_status[i] && property_ownership[group_reps[i]] == player){ //is there a monopoly, and does the player own it(there can't be any houses if there isn't
					switch(i){
						case 0:
						case 7:
							j = 1; //j+1 properties in the group
							break;

						default:
							j = 2;
					}
					for(j; j >= 0; j--){
						if(house_count[group_reps[i]+j] < 5) total+=(house_count[group_reps[i]+j] * 40);
						if(house_count[group_reps[i]+j] == 5) total+=115;
					}
				}
			}
			if(total){
				printf("%s must pay $%d in repairs\n", player_names[player], total);
				transaction(-1, player, total);
			}
			else printf("%s does not own any houses or hotels\n", player_names[player]);
			break;

		case 10:
			printf("\"Pay Hospital $100\"\n");
			change_color(0);
			transaction(-1, player, 100);
			break;

		case 11:
			printf("\"Advance to GO\n(Collect $200\")\n");
			change_color(0);
			player_space[player] = 0;
			transaction(player, -1, 200);
			printf("%s is now on %s", player_names[player], space_names[player_space[player]]);
			break;

		case 12:
			printf("\"Income Tax Refund\nCollect $20\"\n");
			change_color(0);
			transaction(player, -1, 20);
			break;

		case 13:
			printf("\"Doctor's Fee\nPay $50\"\n");
			change_color(0);
			transaction(-1, player, 50);
			break;

		case 14:
			printf("\"XMAS Fund Matures\nCollect $100\"\n");
			change_color(0);
			transaction(player, -1, 100);
			break;

		default:
			printf("\"Life Insurance Matures\nCollect $100\"\n");
			change_color(0);
			transaction(player, -1, 100);
			
	}
	
	return;
}

void chance_shuffle(){
	int hold;
	int hold_rand;
	int i;

	for(i = 16; i > 0; i--){
		hold_rand = rand() % i;
		hold = chance_deck[i-1];
		chance_deck[i-1] = chance_deck[hold_rand];
		chance_deck[hold_rand] = hold;
	}

	return;
}

void cc_shuffle(){
	int hold;
	int hold_rand;
	int i;

	for(i = 16; i > 0; i--){
		hold_rand = rand() % i;
		hold = cc_deck[i-1];
		cc_deck[i-1] = cc_deck[hold_rand];
		cc_deck[hold_rand] = hold;
	}

	return;
}

int determine_blanks(){
	int biggest_name = 8;
	int i;

	for(i = 0; i < player_count; i++){ //determine longest name
		if(strlen(player_names[i]) > biggest_name) biggest_name = strlen(player_names[i]);
	}

	for(i = 0; i < player_count; i++){ //write spaces to array
		blank_count[i] = biggest_name - strlen(player_names[i]);
	}

	return biggest_name;
}
