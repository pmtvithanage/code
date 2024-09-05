#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include "structure.h"

short count;                // count extra rolls of dice
short mystery_cell = -3;    //Location of the mystery cell
short count_round;          // number of rounds
short skip = 0;             //number of skipings of peice rolls
short roll_6 = 0;           // count to roll dice value 6
short *ptr_mystery_cell = &mystery_cell;
short *ptr_count = &count;
short *ptr_round = &count_round;
short *ptr_skip = &skip;
short *ptr_roll_6 = &roll_6;

struct Player* player_colour[4] = {&colour[0],&colour[1],&colour[2],&colour[3]}; //Pointer to the structure
     
char* player[] = {"Red","Green","Yellow","Blue"}; //player colour 

int roll_dice(){
    return (rand() % 6) + 1;//Randomly get values by rolling dice
}
int coin_toss(){
    return (rand() % 2) + 1;// 1=>HEAD, 2=>TAIL
}

int start_player(){
    short R,G,Y,B,first;
    while(1){                   //To choose the first player
        R = roll_dice();
        G = roll_dice();
        Y = roll_dice();
        B = roll_dice();
        printf("\nRed rolls %d\n",R);
        printf("Green rolls %d\n",G);
        printf("Yellow rolls %d\n",Y);
        printf("Blue rolls %d\n",B);

        if(R > G && R >Y && R > B){
            first = 1;
            break;
        }
        if(G > R && G > Y && G > B){
            first = 2;
            break;
        }    
        if(Y > G && Y > R && Y > B){
            first = 3;
            break;
        } 
        if(B > G && B >Y && B > R){
            first = 4;
            break;
        } 
    } 
    char *player[4];
    switch (first) { //Order of the players
        case 1:
            player[0] = "Red";
            player[1] = "Green";
            player[2] = "Yellow";
            player[3] = "Blue";
            break;
        case 2:
            player[0] = "Green";
            player[1] = "Yellow";
            player[2] = "Blue";
            player[3] = "Red";
            break;
        case 3:
            player[0] = "Yellow";
            player[1] = "Blue";
            player[2] = "Red";
            player[3] = "Green";
            break;
        case 4:
            player[0] = "Blue";
            player[1] = "Red";
            player[2] = "Green";
            player[3] = "Yellow";
            break;
    }    
    printf("\n%s player has the highest roll and begin the game.\n", player[0]);
    printf("The order of a single round is %s, %s, %s, %s.\n", player[0], player[1], player[2], player[3]); //Print the order of a single round
    return first;
}

int available_capture(short PLAYER, short dice_number) {
    short closest = -2; // Initialize closest to a value indicating no piece has been found
    short piece = -1;   // Initialize piece to -1 indicating no piece selected
    short new_position; // To store the calculated new position of the player's piece

    // Iterate over all players except the current one
    for (short i = 0; i < 4; i++) {
        if (PLAYER == i) continue; // Skip the current player's own pieces

        // Iterate over all pieces of the opponent
        for (short j = 0; j < 4; j++) {
            
            // Check each piece of the current player
            for (short k = 0; k < 4; k++) {
                if (player_colour[PLAYER]->direction[k] == 1) {
                    new_position = (player_colour[PLAYER]->location[k] + dice_number) % 52;    
                    if (player_colour[PLAYER]->location_count[k] + dice_number < 50) {
                        // Check if new position matches opponent's and the piece isn't in home path
                        if (player_colour[i]->location[j] == new_position && player_colour[i]->homepath[j] == -1) {
                            if (closest < player_colour[i]->location_count[j]) {
                                closest = player_colour[i]->location_count[j];
                                piece = k;
                            }
                        }
                    }
                } 
                if (player_colour[PLAYER]->direction[k] == 2) {
                    new_position = (player_colour[PLAYER]->location[k] - dice_number + 52) % 52;
                    if (player_colour[PLAYER]->location_count[k] + dice_number < 54) {
                        // Check if new position matches opponent's and the piece isn't in home path
                        if (player_colour[i]->location[j] == new_position && player_colour[i]->homepath[j] == -1) {
                            if (closest < player_colour[i]->location_count[j]) {
                                closest = player_colour[i]->location_count[j];
                                piece = k;
                            }
                        }
                    }
                }
            }
        }   
    }
    return piece; // return capture available piece
}
int capture_player(short PLAYER,short piece){
    // Iterate over all players except the current one
    for (short i = 0; i < 4; i++){
        if(i!= PLAYER){
            // Iterate over all pieces of the opponent
            for (short j = 0; j < 4; j++){
                // Check if new position matches to captured position
                if((player_colour[i]->location[j] > -1) && (player_colour[PLAYER]->location[piece] > -1)){
                    if(player_colour[i]->location[j] == player_colour[PLAYER]->location[piece]){
                        player_colour[i]->location[j] = -1;
                        player_colour[i]->location_count[j] = -100;
                        player_colour[i]->capture_count[j] = 0;               //All the details are reset when return to base
                        player_colour[i]->direction[j] = 0; 
                        player_colour[i]->on_board--;
                        player_colour[i]->on_base++;
                        player_colour[PLAYER]->capture_count[piece]++; 
                        printf("%s piece %d lands on square L%d, captures %s piece %d, and return it to the base.\n", player[PLAYER], piece+1, player_colour[PLAYER]->location[piece],player[i],j+1);
                        printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base.\n",player[i],player_colour[i]->on_board,player_colour[i]->on_base);
                        return 1; //if captured
                    }
                }
                   
            }
        }
    }
    return 0;// if no captures
}

int available_blocking(short PLAYER, short dice_number){
    short piece = -1;   // Initialize piece to -1 indicating no piece selected
    short new_position; // To store the calculated new position of the player's piece
    for (short i = 0; i < 4; i++){
        for (short j = 0; j < count; j++){
            if (i == j) continue; // Skip the current piece
            if (player_colour[PLAYER]->direction[i] == 1) {
                    new_position = (player_colour[PLAYER]->location[i] + dice_number) % 52;    
                    if (player_colour[PLAYER]->location_count[i] + dice_number < 50) {
                        // Check if new position matches same player and the piece isn't in home path
                        if (player_colour[PLAYER]->location[j] == new_position && player_colour[PLAYER]->homepath[j] == -1) piece = i;
                    }
                } 
                if (player_colour[PLAYER]->direction[i] == 2) {
                    new_position = (player_colour[PLAYER]->location[i] - dice_number + 52) % 52;
                    if (player_colour[PLAYER]->location_count[i] + dice_number < 54) {
                        // Check if new position matches same player and the piece isn't in home path
                        if (player_colour[PLAYER]->location[j] == new_position && player_colour[PLAYER]->homepath[j] == -1) piece = i;
                    }
                }
        }    
    }
    return piece;//return block available piece
}
void blocking_player(short PLAYER,short piece){
    for (short j = 0; j < 4; j++){
        if(j != piece){
            // Check if new position matches to create a block
            if((player_colour[PLAYER]->location[j] > -1) && (player_colour[PLAYER]->location[piece] > -1)){
                // Check the two pieces in the same position
                if(player_colour[PLAYER]->location[j] == player_colour[PLAYER]->location[piece]){ 
                    printf("%s piece %d create a block by moving to location L%d (%s piece %d and %s piece %d).\n", player[PLAYER], piece+1, player_colour[PLAYER]->location[j], player[PLAYER], piece+1, player[PLAYER],j+1);
                    return;
                }
            }
        }   
    }
    return ;
}
void Bawana_cell(short PLAYER, short piece){
    short toss = coin_toss();
    if(toss == 1){
        player_colour[PLAYER]->piece_status[piece] = 2;  // energised
        printf("%s player's %d piece feels energised and Moment speed doubles.\n", player[PLAYER], piece+1);
    }
    else if(toss == 2){
        player_colour[PLAYER]->piece_status[piece] = 0.5;  // sicked
        printf("%s player's %d piece feels sick and Moment speed halves.\n", player[PLAYER],piece+1);
    }
}
void Kotuwa_cell(short PLAYER, short piece){
    player_colour[PLAYER]->piece_status[piece] = 0; //briefing  
    printf("%s player's %d piece attends briefing and cannot move for four rounds.\n", player[PLAYER], piece+1);
}
void Pita_Kotuwa_cell(short PLAYER, short piece){
    if(player_colour[PLAYER]->direction[piece] == 1){// if piece goes from clockwise
        player_colour[PLAYER]->direction[piece] = 2; // Change the direction
        if(PLAYER == 0) player_colour[PLAYER]->location_count[piece] = -18;
        if(PLAYER == 1) player_colour[PLAYER]->location_count[piece] = -5;   //Adjust the traveled count
        if(PLAYER == 2) player_colour[PLAYER]->location_count[piece] = -44;
        if(PLAYER == 3) player_colour[PLAYER]->location_count[piece] = -31;
    }
    else if(player_colour[PLAYER]->direction[piece] == 2){// if piece goes counter-clockwise
        printf("%s player teleported to Kotuwa.\n",player[PLAYER]);
        player_colour[PLAYER]->location[piece] = 27;
        player_colour[PLAYER]->location_count[piece] = (player_colour[PLAYER]->start_cell - 27 + 52) % 52;
        capture_player(PLAYER,piece);// if any can capture
        Kotuwa_cell(PLAYER, piece);
    }
}
void create_mystery_cell(short round, short completed){
    short count_round = 0;
    short counter = 0;
    short point;
    if(completed >= 2){ //Check if two pieces are completed the round
        if((completed == 2) && *ptr_round == 0) *ptr_round = round ;
        if((round - *ptr_round) % 4 == 0){
            while (1){
                point = rand() % 52;
                for (short i = 0; i < 4; i++){
                    for (short j = 0; j < 4; j++){                              // Calculate randem position to a mystry cell
                        if((point == player_colour[i]->location[j]) || (point == *ptr_mystery_cell)) counter++;
                    }    
                }
                if(counter == 0) break;
            }
            *ptr_mystery_cell =  point; 
            printf("\nA mystery cell has spawned in the location L%d and will be at this location for the next four rounds.\n", *ptr_mystery_cell);  
        }                                                                                                                                                             // Display position of the mystry cell
       if((round - *ptr_round) % 4 != 0) printf("\nThe mystery cell is at L%d and will be at the location for the next %d values.\n", *ptr_mystery_cell,4-(round - *ptr_round)%4);   
    }
    printf("\n______________________________________________________________*** NEXT ROUND ***____________________________________________________________________________________\n");
}
void active_mystery_cell(short PLAYER, short piece){ //logic of mystry cell after piece land on a mystery cell
    if(player_colour[PLAYER]->location[piece] == mystery_cell){
        short teleport = roll_dice();
        switch (teleport)
        {
        case 1: 
            printf("%s player lands on a mystery cell and is teleported to Bhawana.\n",player[PLAYER]);
            player_colour[PLAYER]->location[piece] = 9; //go to location 9 in standerd path
            if(player_colour[PLAYER]->direction[piece] == 1){
                player_colour[PLAYER]->location_count[piece] =( 9 - player_colour[PLAYER]->start_cell + 52) % 52; 
            }
            else if(player_colour[PLAYER]->direction[piece] == 2){
                player_colour[PLAYER]->location_count[piece] = (player_colour[PLAYER]->start_cell - 9 + 52) % 52;
            }
            capture_player(PLAYER,piece);
            Bawana_cell(PLAYER, piece);
            break;
        case 2:
            printf("%s player lands on a mystery cell and is teleported to Kotuwa.\n",player[PLAYER]);
            player_colour[PLAYER]->location[piece] = 27;  //go to location 27 in standard path
            if(player_colour[PLAYER]->direction[piece] == 1){
                player_colour[PLAYER]->location_count[piece] =( 27 - player_colour[PLAYER]->start_cell + 52) % 52;
            }
            else if(player_colour[PLAYER]->direction[piece] == 2){
                player_colour[PLAYER]->location_count[piece] = (player_colour[PLAYER]->start_cell - 27 + 52) % 52;
            }
            capture_player(PLAYER,piece);
            Kotuwa_cell(PLAYER, piece);
            break;
        case 3:
            printf("%s player lands on a mystery cell and is teleported to Pita-Kotuwa.\n",player[PLAYER]);
            player_colour[PLAYER]->location[piece] = 46;        //go to location 46 in standard path
            if(player_colour[PLAYER]->direction[piece] == 1){
                player_colour[PLAYER]->location_count[piece] =( 46 - player_colour[PLAYER]->start_cell + 52) % 52;
            }
            else if(player_colour[PLAYER]->direction[piece] == 2){
                player_colour[PLAYER]->location_count[piece] = (player_colour[PLAYER]->start_cell - 46 + 52) % 52;
            }
            capture_player(PLAYER,piece);
            Pita_Kotuwa_cell(PLAYER, piece);

            break;
        case 4:
            printf("%s player lands on a mystery cell and is teleported to Base.\n",player[PLAYER]);
            player_colour[PLAYER]->location[piece] = -1;            //go to base
            player_colour[PLAYER]->location_count[piece] = -100;
            player_colour[PLAYER]->capture_count[piece] = 0; 
            player_colour[PLAYER]->direction[piece] = 0; 
            player_colour[PLAYER]->on_board--;
            player_colour[PLAYER]->on_base++;

            break;
        case 5:
            printf("%s player lands on a mystery cell and is teleported to Approach.\n",player[PLAYER]);
            player_colour[PLAYER]->location[piece] = player_colour[PLAYER]->approach;       //go to approach cell
            if(player_colour[PLAYER]->direction[piece] == 1){
                player_colour[PLAYER]->location_count[piece] = 50;
            }
            else if(player_colour[PLAYER]->direction[piece] == 2){
                player_colour[PLAYER]->location_count[piece] = 54;
            }
            capture_player(PLAYER,piece);
            break;
        case 6:
            printf("%s player lands on a mystery cell and is teleported to X.\n",player[PLAYER]);
            player_colour[PLAYER]->location[piece] = player_colour[PLAYER]->start_cell;     //go to start cell
            player_colour[PLAYER]->location_count[piece] = 0;
            capture_player(PLAYER,piece);
            break;
        
        }
    }
}
int base_to_X(short PLAYER,short piece) {
    
    if (player_colour[PLAYER]->location[piece] == -1) {
        player_colour[PLAYER]->location[piece] = player_colour[PLAYER]->start_cell;
        player_colour[PLAYER]->direction[piece] = coin_toss(); // 1 for clockwise and 2 for anti-clockwise
        player_colour[PLAYER]->on_base = player_colour[PLAYER]->on_base - 1;
        player_colour[PLAYER]->on_board = player_colour[PLAYER]->on_board + 1;
        player_colour[PLAYER]->location_count[piece] = 0;
        printf("%s player moves piece %d to the starting point.\n",player[PLAYER],piece+1);
        printf("%s player now has %d/4 pieces on the board and %d/4 pieces on the base.\n",player[PLAYER],player_colour[PLAYER]->on_board,player_colour[PLAYER]->on_base);
        active_mystery_cell(PLAYER,piece);
        return 1;
    }    
    // If the piece is not available to move from the base
    return -1;
}
int nearest_to_home(short PLAYER, short dice_number) {
    short nearest = -100; 
    short nearest_player = -1; 
    for (short i = 0; i < 4; i++) {
        // Skip pieces that are not eligible to move towards home
        if ((player_colour[PLAYER]->direction[i] == 1 && (player_colour[PLAYER]->location_count[i] + dice_number > 50) && (player_colour[PLAYER]->capture_count[i] == 0)) ||
            (player_colour[PLAYER]->direction[i] == 2 && (player_colour[PLAYER]->location_count[i] + dice_number > 54) && (player_colour[PLAYER]->capture_count[i] == 0))) {
            continue;
        }
        // Check if the current piece is closer to home and within home path entry conditions
        if ((nearest < player_colour[PLAYER]->location_count[i]) && (player_colour[PLAYER]->homepath[i] != 5) && (player_colour[PLAYER]->homepath[i] + dice_number <= 5)) {
            nearest = player_colour[PLAYER]->location_count[i];
            nearest_player = i;
        }
    }
    return nearest_player; // Return the piece closest to the home path, or -1 if none
}
int normal_move(short PLAYER, short dice_number,short piece) {
        if (player_colour[PLAYER]->direction[piece] == 1) { // when moving clockwise
            if (player_colour[PLAYER]->location[piece] > -1 && (player_colour[PLAYER]->location_count[piece] + dice_number <= 50)){
                short previous = player_colour[PLAYER]->location[piece];
                player_colour[PLAYER]->location[piece] = (player_colour[PLAYER]->location[piece] + dice_number) % 52;
                player_colour[PLAYER]->location_count[piece] += dice_number;
                printf("%s moves piece %d from location L%d to L%d by %d units in clockwise direction\n", player[PLAYER], piece+1, previous, player_colour[PLAYER]->location[piece], dice_number);
                return 1;
            }
        }
        else if (player_colour[PLAYER]->direction[piece] == 2) { // when moving anti-clockwise
            if (player_colour[PLAYER]->location[piece] > -1 && (player_colour[PLAYER]->location_count[piece] + dice_number <= 54)){
                short previous = player_colour[PLAYER]->location[piece];
                player_colour[PLAYER]->location[piece] = (player_colour[PLAYER]->location[piece] - dice_number + 52) % 52;
                player_colour[PLAYER]->location_count[piece] += dice_number;
                printf("%s moves piece %d from location L%d to L%d by %d units in counterclockwise direction\n", player[PLAYER], piece+1, previous, player_colour[PLAYER]->location[piece], dice_number);
                return 1;
            }
        }
    return 0;
}
int home_path_move(short PLAYER, short dice_number,short piece){
    if((player_colour[PLAYER]->location_count[piece] > 50) && (player_colour[PLAYER]->homepath[piece] + dice_number <= 5) && (player_colour[PLAYER]->direction[piece] == 1)){
        short previous = player_colour[PLAYER]->homepath[piece];
        player_colour[PLAYER]->homepath[piece] += dice_number;
        player_colour[PLAYER]->location_count[piece] += dice_number;
        if(player_colour[PLAYER]->homepath[piece] >= 5){
            player_colour[PLAYER]->round_comleted[piece] = 1;
        }
        printf("%s moves piece %d from location H%d to H%d by %d units \n", player[PLAYER],piece+1, previous, player_colour[PLAYER]->homepath[piece], dice_number);
        return 1;
    }  
    else if((player_colour[PLAYER]->location_count[piece] > 54) && (player_colour[PLAYER]->homepath[piece] + dice_number <= 5) && (player_colour[PLAYER]->direction[piece] == 2)){
        short previous = player_colour[PLAYER]->homepath[piece];
        player_colour[PLAYER]->homepath[piece] += dice_number;
        player_colour[PLAYER]->location_count[piece] += dice_number;
        if(player_colour[PLAYER]->homepath[piece] >= 5){
            player_colour[PLAYER]->round_comleted[piece] = 1;
        }
        printf("%s moves piece %d from location H%d to H%d by %d units \n", player[PLAYER],piece+1, previous, player_colour[PLAYER]->homepath[piece], dice_number);
        return 1;
    }  
    return 0;
}
int crossing_approach_cell(short PLAYER, short dice_number,short piece){
    if((player_colour[PLAYER]->direction[piece] ==1) && (player_colour[PLAYER]->homepath[piece] == -1)){
        if((player_colour[PLAYER]->location_count[piece] + dice_number > 50) && (player_colour[PLAYER]->capture_count[piece] > 0)){
            short previous = player_colour[PLAYER]->location[piece];
            player_colour[PLAYER]->location[piece] = -2;
            player_colour[PLAYER]->homepath[piece] = player_colour[PLAYER]->location_count[piece] + dice_number - 51;
            player_colour[PLAYER]->location_count[piece] += dice_number;
            if(player_colour[PLAYER]->homepath[piece] >= 5){
                player_colour[PLAYER]->round_comleted[piece] = 1;
            }
            printf("%s moves piece %d from location L%d to H%d by %d units \n", player[PLAYER], piece+1, previous, player_colour[PLAYER]->homepath[piece], dice_number);
            return 1;
        }
        
    }
    else if((player_colour[PLAYER]->direction[piece] == 2) && (player_colour[PLAYER]->homepath[piece] == -1)){
        if((player_colour[PLAYER]->location_count[piece] + dice_number > 54) && (player_colour[PLAYER]->capture_count[piece] > 0)){
            short previous = player_colour[PLAYER]->location[piece];
            player_colour[PLAYER]->location[piece] = -2;
            player_colour[PLAYER]->homepath[piece] = player_colour[PLAYER]->location_count[piece] + dice_number - 55;
            player_colour[PLAYER]->location_count[piece] += dice_number;
            if(player_colour[PLAYER]->homepath[piece] == 5){
                player_colour[PLAYER]->round_comleted[piece] = 1;
            }
            printf("%s moves piece %d from location L%d to H%d by %d units \n", player[PLAYER], piece+1, previous, player_colour[PLAYER]->homepath[piece], dice_number);
            return 1;
        }
        
    }
    return 0;
}
void print_current_state(){
    for (short i = 0; i < 4; i++){
        //printing the current state of the pieces
        printf("\n%s player now has %d/4 on pieces on the board and %d/4 pieces on the base.",player[i],player_colour[i]->on_board,player_colour[i]->on_base);
        printf("\n================================================================\n");
        printf("Location of the pieces %s\n",player[i]);
        printf("================================================================\n");
        for (short j = 0; j < 4; j++){
            if(player_colour[i]->location[j] == -1) printf("Piece %d -> Base\n",j+1);
            else if(player_colour[i]->location[j] > -1) printf("Piece %d -> L%d\n",j+1,player_colour[i]->location[j]);
            else if(player_colour[i]->homepath[j] >= 5) printf("Piece %d -> Home\n",j+1);
            else if(player_colour[i]->homepath[j] > -1) printf("Piece %d -> H%d\n",j+1,player_colour[i]->homepath[j]);
        }
    }    
}
void Red_Player(){
    short dice_roll;
    dice_roll = roll_dice();
    printf(RED"\nRed player rolled %d\n"RESET,dice_roll);
    short capture_available_piece = available_capture(0,dice_roll);
    short move = 0;
    if(dice_roll == 6){
        roll_6++;
        if(*ptr_roll_6 > 2 ) {
            roll_6 = 0;
            return;
        }
    }
    if(capture_available_piece != -1){      //player prioritize to capture
        normal_move(0,dice_roll,capture_available_piece);
        capture_player(0,capture_available_piece);
        active_mystery_cell(0,capture_available_piece);
        count ++;
        move = 1;
        if(*ptr_count==3){
            count = 0;
            return ;
        }
        else{
            Red_Player();
        }
    }
    else {
        if(dice_roll == 6){
            for (short i = 0; i < 4; i++){
                if(base_to_X(0,i) == 1){        //Base to X
                    blocking_player(0,i);
                    count ++;
                    move = 1;
                } 
                if(move == 1) break; 
            }       
            if(move == 0){
                count++;
                int piece = nearest_to_home(0,dice_roll);   
                move = normal_move(0,dice_roll,piece);              //standard path move
                active_mystery_cell(0,piece);
                if(move == 0) move = crossing_approach_cell(0,dice_roll,piece);
                if(move == 0) move = home_path_move(0,dice_roll,piece);            //home path move
            }     
            if(*ptr_count==3){
                count = 0;
                return ;
            }
            else{
                Red_Player();
            }
                 
        }
        if(move == 0){
            int piece = nearest_to_home(0,dice_roll);
            move = normal_move(0,dice_roll,piece);                   //standard path move
            blocking_player(0,piece);
            active_mystery_cell(0,piece);
            if(move == 0) move = crossing_approach_cell(0,dice_roll,piece);
            if(move == 0) move = home_path_move(0,dice_roll,piece);                     //home path move
            
        }     
    }
}
void Green_Player(){
    short dice_roll;
    dice_roll = roll_dice();
    printf(GREEN"\nGreen player rolled %d\n"RESET,dice_roll);
    short capture_available_piece = available_capture(1,dice_roll);
    short block_available_piece = available_blocking(1,dice_roll);
    short move = 0;
    if(dice_roll == 6){
        roll_6++;
        if(*ptr_roll_6 > 2 ) {
            roll_6 = 0;
            return;
        }
    }
    if((block_available_piece != -1)){                  //player prioritize to create  blocks
        normal_move(1,dice_roll,block_available_piece);
        blocking_player(1,block_available_piece);
        active_mystery_cell(1,capture_available_piece);    
        move = 1;
    }
    if(dice_roll == 6){
        for (short i = 0; i < 4; i++){
            if(base_to_X(1,i) == 1){
                blocking_player(1,i);
                move = 1;
            }
            if(move == 1) break;
        }
        if((capture_available_piece != -1) && move == 0){
            normal_move(1,dice_roll,capture_available_piece);           
            capture_player(1,capture_available_piece);
            active_mystery_cell(1,capture_available_piece);
            move = 1;
        }        
        if(move == 0){
            int piece = nearest_to_home(1,dice_roll);
            move = normal_move(1,dice_roll,piece);
            blocking_player(1,piece);
            active_mystery_cell(1,piece);
            if(move == 0) move = crossing_approach_cell(1,dice_roll,piece);
            if(move == 0) move = home_path_move(1,dice_roll,piece); 
        }
        count ++;
        if(*ptr_count==3){
            count = 0;
            return ;
        }
        Green_Player();
    } 
    else {   
        if(capture_available_piece != -1){
            normal_move(1,dice_roll,capture_available_piece);
            capture_player(1,capture_available_piece);  
            active_mystery_cell(1,capture_available_piece);          
            count ++;
            move = 1;
            if(*ptr_count==3){
                count = 0;
                return ;
            }
            else{
                Green_Player();
            }
        }    
        if(move == 0){
            short piece = nearest_to_home(1,dice_roll);
            move = normal_move(1,dice_roll,piece);
            blocking_player(1,piece);
            active_mystery_cell(1,piece);
            if(move == 0) move = crossing_approach_cell(1,dice_roll,piece);
            if(move == 0) move = home_path_move(1,dice_roll,piece);            
        }        
    } 
}
void Yellow_Player(){
    short dice_roll;
    dice_roll = roll_dice(); 
    printf(YELLOW"\nYellow player rolled %d\n"RESET,dice_roll);
    short capture_available_piece = available_capture(2,dice_roll);
    short move = 0;
    if(dice_roll == 6){
        roll_6++;
        if(*ptr_roll_6 > 2 ) {
            roll_6 = 0;
            return;
        }
    }
    if(dice_roll == 6){
        
        for (short i = 0; i < 4; i++){
            if(base_to_X(2,i) == 1){
                blocking_player(2,i);
                move = 1;
            }
            if(move == 1) break;
        }
        if((capture_available_piece != -1) && move == 0){
            normal_move(2,dice_roll,capture_available_piece);
            capture_player(2,capture_available_piece);
            active_mystery_cell(2,capture_available_piece);    
            move = 1;
        }        
        if(move == 0){
            int piece = nearest_to_home(2,dice_roll);
            move = normal_move(2,dice_roll,piece);
            blocking_player(2,piece);
            active_mystery_cell(2,piece);
            if(move == 0) move = crossing_approach_cell(2,dice_roll,piece);
            if(move == 0) move = home_path_move(2,dice_roll,piece); 
        }
        count ++;
        if(*ptr_count==3){
            count = 0;
            return ;
        }
        Yellow_Player();
    } 
    else {   
        if(capture_available_piece != -1){
            normal_move(2,dice_roll,capture_available_piece);
            capture_player(2,capture_available_piece);  
            active_mystery_cell(2,capture_available_piece);      
            count ++;
            move = 1;
            if(*ptr_count==3){
                count = 0;
                return ;
            }
            else{
                Yellow_Player();
            }
        }    
        if(move == 0){
            short piece = nearest_to_home(2,dice_roll);
            move = normal_move(2,dice_roll,piece);
            blocking_player(2,piece);
            active_mystery_cell(2,piece);
            if(move == 0) move = crossing_approach_cell(2,dice_roll,piece);
            if(move == 0) move = home_path_move(2,dice_roll,piece);            
        }        
    }           
}    
void Blue_Player(int round){
    short dice_roll;
    dice_roll = roll_dice();
    printf(BLUE"\nBlue player rolled %d\n"RESET,dice_roll);
    short loop = 1;
    if(dice_roll == 6){
        roll_6++;
        if(*ptr_roll_6 > 2 ) {
            roll_6 = 0;
            return;
        }
    }
    while(1){
        short piece = (round + *ptr_skip) % 4;
        short move = 0;
        short previous;
        short chapter = 0;

        if(player_colour[3]->direction[piece] == 1){
            if((player_colour[3]->location[piece] + dice_roll) % 52 == mystery_cell){
                move = normal_move(3,dice_roll,piece);
                active_mystery_cell(3,piece);
                move = 1;
            }
            else{
                move = normal_move(3,dice_roll,piece);
                if(move == 1) blocking_player(3,piece);
                if(move == 0) move = crossing_approach_cell(3,dice_roll,piece);
                if(move == 0) move = home_path_move(3,dice_roll,piece); 
                if(capture_player(3,piece) == 1){
                    count ++;
                    move = 1;
                    chapter = 1;
                    if(*ptr_count==3){
                        count = 0;
                        return ;
                    }
                    else{
                        Blue_Player(round);
                    }
                }
            }
        }
        else if(player_colour[3]->direction[piece] == 2){
            
            if((player_colour[3]->location[piece] - dice_roll + 52) % 52 == mystery_cell){
            *ptr_skip = skip + 1;
            }
            move = normal_move(3,dice_roll,piece);
            blocking_player(3,piece);
            if(move == 0) move = crossing_approach_cell(3,dice_roll,piece);
            if(move == 0) move = home_path_move(3,dice_roll,piece); 
            if(capture_player(3,piece) == 1){
                count ++;
                move = 1;
                chapter = 1;
                if(*ptr_count==3){
                    count = 0;
                    return ;
                }
                else{
                    Blue_Player(round);
                }
            }
        }
        if((dice_roll == 6) && (move == 1) && (chapter == 0)){
            count ++;
            move = 1;
            if(*ptr_count==3){
                count = 0;
                return ;
            }
            else{
                Blue_Player(round);
            }
            
        }
        if((dice_roll == 6) && (move == 0)){
            if(base_to_X(3,piece) == 1){
                count ++;
                move = 1;
                if(*ptr_count==3){
                    count = 0;
                    return ;
                }
                else{
                    Blue_Player(round);
                }
            }
        }
        if(move == 0){
            *ptr_skip = skip + 1;
        }
        if((move == 1) || (loop == 4)) break;
        loop++;
    }
}
void game_logic(){
    // Display the pieces of the players
    printf("\nThe "RED"Red player "RESET"has four (04) pieces named R1, R2, R3, and R4.");
    printf("\nThe "GREEN"Green player "RESET"has four (04) pieces named G1, G2, G3, and G4.");
    printf("\nThe "YELLOW"Yellow player "RESET"has four (04) pieces named Y1, Y2, Y3, and Y4.");
    printf("\nThe "BLUE"Blue player "RESET"has four (04) pieces named B1, B2, B3, and B4.\n");

    srand(time(NULL));
  
    //R=1, G=2, Y=3, B=4
    //Make the order or the players 
    short player_order[4];
    switch (start_player())
    {
    case 1:
        player_order[0] = 1;
        player_order[1] = 2;
        player_order[2] = 3;
        player_order[3] = 4;
        break;
    case 2:
        player_order[0] = 2;
        player_order[1] = 3;
        player_order[2] = 4;
        player_order[3] = 1;
        break;
    case 3:
        player_order[0] = 3;
        player_order[1] = 4;
        player_order[2] = 1;
        player_order[3] = 2;
        break;
    case 4:
        player_order[0] = 4;
        player_order[1] = 1;
        player_order[2] = 2;
        player_order[3] = 3;
        break;
    }
    //Initializing all details
    for (short i = 0; i < 4; i++){
        for (short k = 0; k < 4; k++){
            colour[i].location[k] = -1;
            colour[i].homepath[k] = -1; 
            colour[i].capture_count[k] = 0; 
            colour[i].location_count[k] = -100;
            colour[i].round_comleted[k] = 0;
            player_colour[i]->piece_status[k] = 1;
        }
        colour[i].on_base = 4;
        colour[i].on_board = 0;    
    }
    //initialize approach
    colour[0].approach = 26;
    colour[1].approach = 39;
    colour[2].approach = 0;
    colour[3].approach = 13;
    
    //initialize start cell
    colour[0].start_cell = 28;
    colour[1].start_cell = 41;
    colour[2].start_cell = 2;
    colour[3].start_cell = 15;

    char input;
    short player_completed ;
    short round_count = 0;
    while(1) {
        //Switch players
        for (short i = 0; i < 4; i++)
        {
            switch (player_order[i]){
                case 1:
                    Red_Player();
                    *ptr_count = 0; 
                    *ptr_roll_6 = 0;
                    player_completed = 0;
                    for (short j = 0; j < 4; j++){
                        player_completed += player_colour[0]->round_comleted[j];
                    }
                    if(player_completed == 4){ 
                        print_current_state();
                        printf(RED"\n %s player  wins!!!\n"RESET,player[0]);
                        return ;
                    }
                    break;
                case 2:
                    Green_Player();
                    *ptr_count = 0;
                    *ptr_roll_6 = 0;
                    player_completed = 0;
                    for (short j = 0; j < 4; j++){
                       player_completed += player_colour[1]->round_comleted[j];
                    }
                    if(player_completed == 4){ 
                        print_current_state();
                        printf(GREEN"\n %s player  wins!!!\n"RESET,player[1]);
                        return ;
                    }
                    break;
                case 3:
                    Yellow_Player();
                    *ptr_count = 0;
                    *ptr_roll_6 = 0;
                    player_completed = 0;
                    for (short j = 0; j < 4; j++){
                        player_completed += player_colour[2]->round_comleted[j];
                    }
                    if(player_completed == 4){ 
                        print_current_state();
                        printf(YELLOW"\n %s player  wins!!!\n"RESET,player[2]);
                        return ;
                    }
                    break;
                case 4:
                    Blue_Player(round_count);
                    *ptr_count = 0;
                    *ptr_roll_6 = 0;
                    player_completed = 0;
                    for (short j = 0; j < 4; j++){
                        player_completed += player_colour[3]->round_comleted[j];
                    }
                    if(player_completed == 4){ 
                        print_current_state();
                        printf(BLUE"\n %s player  wins!!!\n"RESET,player[3]);
                        return ;
                    }
                    break;
            }    
        }
        print_current_state();     //Display the location of pieces of the each  player
        short completed = 0;
        round_count++;

        //Check if the piece has completed the round
        for (short i = 0; i < 4; i++){
            player_completed = 0;
            for (short j = 0; j < 4; j++){
                player_completed += player_colour[i]->round_comleted[j];
            } 
            completed += player_completed;  
        }
        create_mystery_cell(round_count,completed);     //Creating mystery cells
    }
}
