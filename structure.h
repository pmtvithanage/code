#include<stdio.h>
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define RESET   "\x1b[0m"

struct Player {
    short location[4];      //standard path location
    short direction[4];     //direction (clockwise or clockwise)
    short homepath[4];      //home path location
    short capture_count[4]; //capture count
    short location_count[4];//distance traveled
    short round_comleted[4];//whether completed round
    float piece_status[4];  // energised or sicked or briefing
    short on_base;          //count on the base
    short on_board;         //count on the board
    short approach;         //location of approach cells
    short start_cell;       //location of start cells

}colour[4]; //Red = 0, Green = 1, Blue = 2, Yellow = 3