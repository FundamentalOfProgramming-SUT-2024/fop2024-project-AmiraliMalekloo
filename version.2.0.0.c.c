#include<stdio.h>
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<math.h>
#include <wchar.h>
#include <locale.h>


#define Rows 50
#define Cols 230
#define bRow 5
#define bCol 69

#define MAX_LEALTH 10
#define COLOR_ORANGE 80
#define COLOR_GOLD 90

#define WALL 0x1F7EB
#define WALL2 0x1F7EA
#define WALL3 0x1F7E8
#define FLOOR 0x26AA
#define STAIR 0x1F3DB
#define DOOR 0x1F6AA
#define LOCK 0x1F512
#define KEY 0x1F511
#define MASTER 0x1F53A
#define TRAP 0x1F3FA
#define CORRIDOR 0x2B1C
#define PILLAR 0x1F7E0
#define MACER 0x2692
#define DAGGER 0x1F5E1
#define WAND 0x1FA84
#define ARROW 0x27BC
#define SWORD 0x2694
#define DEAMON 0x1F608
#define FIREBREATH 0x1F525
#define GIANT 0x1F479
#define UNDEED 0x1F480
#define SNAKE 0x1F409
#define RIGHT 0x27A1
#define HEALTH 0x1F34F
#define SPEED 0x26A1
#define POWER 0x1F4AA
#define FOOD 0x1F354
#define GOLD 0x1F4B0
#define BGOLD 0x1F48E
#define MACE 0x2692
#define Dagger 0x1F5E1
#define WAND 0x1FA84
#define NORMALARROW 0x27B3
#define SWORD 0x2694

int adad=0;
struct Room;
typedef struct{
    int x; // ---> points one special x
    int y; // ^ points to one special y
    //char color[10];
}pos;
typedef struct{
    pos* position;
}Trap;
typedef struct{
    int name; // D:Deamon F:fire breathing G:Giant S:Snake U:Undeed
    int life;   // if 0 : it is dead
    int moves;
    pos position;
}Monster;
typedef struct{
    int name;
    pos position;
}Enchant;
typedef struct Door{
    pos position;
    int hidden; // 0:not hidden  1:hidden
    int locked; //0: not locked at all 1:unlocked 2:locked  3:first_wrong   4:second_wrong  5:third_wrong

    int reverse;
    int makers_num;
    pos password_maker_1;
    char password_1[12];
    pos password_maker_2;
    char password_2[12];
    int change;
    time_t last_seen;
    struct Room* room;
    // Options
} Door;

typedef struct{
    pos position;
    int name;
    int num;
}Gun;

typedef struct Room{
    int ver;
    int hor;
    int length; //amoodi :  an interval of x's
    int width; //ofoghi  :  an interval of y's
    Door* door[4];
    int num_doors;
    int theme;  // 1:Regular    2:Enchant  3:Treasure
    int Trap_num;
    Trap* traps;
    int Enemy_num;
    Monster* monsters;
    pos* key;
    pos* pillar;
    int master;
    int enchant_num;
    Enchant* enchant;
    pos* gold;
    int gold_num;
    int food_num;
    pos* food;
    Gun* gun;
}Room;

typedef struct{
    int state; //0:no unfinished game   1:have unfinished game
    char name[50];  char pass[50];  char email[50];
    int number_of_games;    int level;
    int colour;
    pos player;
    int points;
    int gold;
    int rank;
    int loged_in;
    pos start[4];
    pos end[4];
    int floor;
    char what[4][Rows-bRow][(Cols-bCol)/2][15];
    int gun[5]; // 0:Mace 1:Dagger 2:wond 3:normal arrow 4:Sword
    int current_gun; // 0:Mace 1:Dagger 2:wond 3:normal arrow 4:Sword
    int direction;
    int current_enchant;
    int enchant_effect;
    //
    int health;
    time_t last_update;  // updating health
    int hunger;
    time_t want_food;
    int food;
    //
    double master_key;
    time_t temp_massage;
    //Room* current_room;
    int speed_e;
    int health_e;
    int damage_e;
    int enchant[3]; //0:health 1:speed 2:damage
    int score;
    time_t first_time;
    Room* room;
    int win;
}Game;

typedef struct{
    pos positioin;
    Room* room;
}Wall;

typedef struct{
    Door * door;
    Trap * Trap;
    Monster * Monster;
    Gun* gun;

}object;


FILE * fptr;
FILE * topten;
int Map[4][Rows-bRow][(Cols-bCol)/2];
char Show[4][Rows-bRow][(Cols-bCol)/2];
char Color[4][Rows-bRow][(Cols-bCol)/2][10];
//1:Red , 2:yellow , 3:green , 4:orange
object MAP[4][Rows-bRow][(Cols-bCol)/2];
//char what[4][Rows-bRow][(Cols-bCol)/2][10];
int M=0;


// prototypes : 
void user_menu();
int good_password(char[]);
int get_new_user(Game *);
int good_email(char[]);
int sign_in(Game *);
int before_game(Game*);
void default_game(Game *);
void game_massage(char massage[]);
void add_path(Game* g,pos start,pos current,pos** parent,int map[Rows-bRow][(Cols-bCol)/2]);
void initialize(int i_size,int j_size,int map[Rows-bRow][(Cols-bCol)/2],char*** mark, pos ***parent);
int find_path(int map[Rows-bRow][(Cols-bCol)/2],pos* start,pos* end,int x,int y,char*** mark,pos *** parent);
pos* make_map(pos* begin,Game * g,int map[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10],object obs[Rows-bRow][(Cols-bCol)/2],int floor);
void print_map(int map[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10]);
void add_room(Game* g,Room* R,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10]);
Room* gen_room(Game* g,int ver_0,int hor_0,int down,int theme,int floor,int has_lock,int key);
int move_player(char A,Game* g,pos start,pos end,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2]);
int is_block(Game* g,pos * current,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2]);
int run_game(Game* g);
char* get_massage();


void write_int(int a){
    fprintf(fptr,"%d ",a);
}

void write_char(char a){
    fprintf(fptr,"%c ",a);
}

void write_str(char a[]){
    if(a[0]=='\0') {write_int(0); return;}
    write_int(1);
    fprintf(fptr,"%s ",a);
}

void read_int(int* a){
    fscanf(fptr,"%d",a);
}

void read_char(char* a){
    fscanf(fptr," %c",a);
}

void read_str(char a[]){
    int b; read_int(&b);
    if(b==0) {a[0]='\0'; return;}
    fscanf(fptr,"%s",a);
}

void write_time_t(time_t* value) {
    fprintf(fptr, "%ld ", *value);
}

void read_time_t(time_t* value) {
    fscanf(fptr, "%ld", value);
}

void write_double(double value) {
    fprintf(fptr, "%lf ", value);
}

void read_double(double* value) {
    fscanf(fptr, "%lf", value);
}

void write_pos(pos* p){
    if(p==NULL) {write_int(0); return;}
    write_int(1);
    fprintf(fptr,"%d %d ",p->x,p->y);
}

void read_pos(pos* p){
    if(p==NULL) p=(pos*)malloc(sizeof(pos));
    int a; read_int(&a);
    if(a==0) {p=NULL; return;}
    fscanf(fptr,"%d %d ",&(p->x),&(p->y));
}

void write_trap(Trap* t){
    if(t==NULL) {write_int(0); return;}
    write_int(1);
    write_pos(t->position);
}

void read_trap(Trap* t){
    if(t==NULL) t=(Trap*)malloc(sizeof(Trap));
    int b; read_int(&b);
    if(b==0) {t=NULL; return;}
    read_pos(t->position);
}

void write_monster(Monster* monster){
    if(monster==NULL) {write_int(0); return;}
    write_int(1);
    write_pos(&(monster->position));
    write_int(monster->name);
    write_int(monster->life);
    write_int(monster->moves);
}

void read_monster(Monster* monster){
    if(monster==NULL) monster=(Monster*)malloc(sizeof(Monster));
    int a; read_int(&a); if(a==0) {monster=NULL; return;}
    write_pos(&(monster->position));
    read_int(&monster->name);
    read_int(&monster->life);
    read_int(&monster->moves);
}

void write_enchant(Enchant* enchant){
    if(enchant==NULL) {write_int(0); return;}
    write_int(1);
    write_pos(&(enchant->position));
    write_int(enchant->name);
}

void read_enchant(Enchant* enchant){
    if(enchant==NULL) enchant=(Enchant*)malloc(sizeof(Enchant));
    int a; read_int(&a); if(a==0) {enchant=NULL; return;}
    read_pos(&(enchant->position));
    read_int(&enchant->name);
}

void write_door(Door* d){
    if(d==NULL){write_int(0); return;}
    write_int(1);
    write_pos(&(d->position));
    write_int(d->hidden);
    write_int(d->locked);
    write_int(d->reverse);
    write_int(d->makers_num);
    write_pos(&(d->password_maker_1));
    write_str(d->password_1);
    if(d->makers_num>1){
        write_pos(&(d->password_maker_2));
        write_str(d->password_2);
    }
    write_int(d->change);
    fprintf(fptr,"%ld ",d->last_seen);
}

void read_door(Door* d){
    if(d==NULL) d=(Door*)malloc(sizeof(Door));
    int a; read_int(&a); if(a==0){d=NULL; return;}
    read_pos(&(d->position));
    read_int(&d->hidden);
    read_int(&d->locked);
    read_int(&d->reverse);
    read_int(&d->makers_num);
    read_pos(&(d->password_maker_1));
    read_str(d->password_1);
    if(d->makers_num>1){
        read_pos(&(d->password_maker_2));
        read_str(d->password_2);
    }
    read_int(&d->change);
    fscanf(fptr,"%ld ",&(d->last_seen));
}

void write_gun(Gun* gun){
    if(gun==NULL){write_int(0); return;}
    write_int(1);
    write_pos(&(gun->position));
    write_int(gun->name);
    write_int(gun->num);
}

void read_gun(Gun* gun){
    if(gun==NULL) gun=(Gun*)malloc(sizeof(Gun));
    int a; read_int(&a); if(a==0){gun=NULL; return;}
    read_pos(&(gun->position));
    read_int(&gun->name);
    read_int(&gun->num);
}

void write_room(Room* r){
    if(r==NULL) {write_int(0); return;}
    write_int(1);
    write_int(r->ver);
    write_int(r->hor);
    write_int(r->length);
    write_int(r->width);
    for(int i=0;i<4;i++){
        write_door(r->door[i]);
    }
    write_int(r->num_doors);
    write_int(r->theme);
    write_int(r->Trap_num);
    for(int i=0;i<r->Trap_num;i++){
        write_trap(&(r->traps[i]));
    }
    write_int(r->Enemy_num);
    for(int i=0;i<r->Enemy_num;i++){
        write_monster(&(r->monsters[i]));
    }
    write_pos(r->key);
    write_pos(r->pillar);
    write_int(r->master);
    write_int(r->enchant_num);
    for(int i=0;i<r->enchant_num;i++){
        write_enchant(&(r->enchant[i]));
    }
    write_int(r->gold_num);
    for(int i=0;i<r->gold_num;i++){
        write_pos(&(r->gold[i]));
    }
    write_int(r->gold_num);
    write_int(r->food_num);
    write_pos(r->food);
    write_gun(r->gun);
}

void read_room(Room* r){
    if(r==NULL) r=(Room*)malloc(sizeof(Room));
    int a; read_int(&a);
    if(a==0) {free(r); return;}
    read_int(&r->ver);
    read_int(&r->hor);
    read_int(&r->length);
    read_int(&r->width);
    for(int i=0;i<4;i++){
        read_door((r->door[i]));
        if(r->door[i]->room==NULL) r->door[i]->room=(Room*)malloc(sizeof(Room));
        *(r->door[i]->room)=*r;
    }
    read_int(&r->num_doors);
    read_int(&r->theme);
    read_int(&r->Trap_num);
    r->traps = (Trap*)malloc(r->Trap_num * sizeof(Trap));
    for(int i=0;i<r->Trap_num;i++){
        read_trap(&(r->traps[i]));
    }
    read_int(&r->Enemy_num);
    r->monsters = (Monster*)malloc(r->Enemy_num * sizeof(Monster));
    for (int i = 0; i < r->Enemy_num; i++) {
        read_monster(&r->monsters[i]);
    }
    read_pos(r->key);
    read_pos(r->pillar);
    read_int(&r->master);
    read_int(&r->enchant_num);
    for(int i=0;i<r->enchant_num;i++){
        read_enchant(&(r->enchant[i]));
    }
    read_int(&r->gold_num);
    for(int i=0;i<r->gold_num;i++){
        read_pos(&(r->gold[i]));
    }
    read_int(&r->gold_num);
    read_int(&r->food_num);
    read_pos(r->food);
    read_gun(r->gun);
}


void write_arrays(Game* g){
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                write_str(g->what[i][j][k]);
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                if(MAP[i][j][k].door==NULL){
                    write_int(0);
                }
                else{
                    write_int(1);
                    write_door(MAP[i][j][k].door);
                }
                if(MAP[i][j][k].Trap==NULL){
                    write_int(0);
                }
                else{
                    write_int(1);
                    write_trap(MAP[i][j][k].Trap);
                }
                if(MAP[i][j][k].Monster==NULL){
                    write_int(0);
                }
                else{
                    write_int(1);
                    write_monster(MAP[i][j][k].Monster);
                }
                if(MAP[i][j][k].gun==NULL){
                    write_int(0);
                }
                else{
                    write_int(1);
                    write_gun(MAP[i][j][k].gun);
                }
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                write_int(Map[i][j][k]);
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                write_char(Show[i][j][k]);
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                write_str(Color[i][j][k]);
            }
        }
    }
    write_int(M);
}

void read_arrays(Game* g){
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                read_str(g->what[i][j][k]);
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                int a;
                read_int(&a);
                MAP[i][j][k].door=(Door*)malloc(sizeof(Door));
                if(a==0) MAP[i][j][k].door=NULL;
                else{
                    read_door(MAP[i][j][k].door);
                }
                read_int(&a);
                MAP[i][j][k].Trap=(Trap*)malloc(sizeof(Trap));
                if(a==0) MAP[i][j][k].Trap=NULL;
                else{
                    read_trap(MAP[i][j][k].Trap);
                }
                read_int(&a);
                MAP[i][j][k].Monster=(Monster*)malloc(sizeof(Monster));
                if(a==0) MAP[i][j][k].Monster=NULL;
                else{
                    read_monster(MAP[i][j][k].Monster);
                }
                read_int(&a);
                MAP[i][j][k].gun=(Gun*)malloc(sizeof(Gun));
                if(a==0) MAP[i][j][k].gun=NULL;
                else{
                    read_gun(MAP[i][j][k].gun);
                }
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                read_int(&Map[i][j][k]);
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                read_char(&Show[i][j][k]);
            }
        }
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                read_str(Color[i][j][k]);
            }
        }
    }
    read_int(&M);
}

void write_game(Game* g) {
    if (g == NULL) {
        write_int(0);
        return;
    }
    write_int(1);

    write_str(g->name);
    write_str(g->pass);
    write_str(g->email);

    write_int(g->state);

    if(g->state==0) return;
    write_int(g->number_of_games);
    write_int(g->level);
    write_int(g->colour);

    write_pos(&(g->player));

    write_int(g->points);
    write_int(g->gold);
    write_int(g->rank);
    write_int(g->loged_in);

    for (int i = 0; i < 4; i++) {
        write_pos(&(g->start[i]));
    }
    for (int i = 0; i < 4; i++) {
        write_pos(&(g->end[i]));
    }

    write_int(g->floor);


    for (int i = 0; i < 5; i++) {
        write_int(g->gun[i]);
    }

    write_int(g->current_gun);
    write_int(g->direction);
    write_int(g->current_enchant);
    write_int(g->enchant_effect);

    write_int(g->health);
    write_time_t(&(g->last_update));
    write_int(g->hunger);
    write_time_t(&(g->want_food));
    write_int(g->food);

    write_double(g->master_key);
    write_time_t(&(g->temp_massage));

    write_int(g->speed_e);
    write_int(g->health_e);
    write_int(g->damage_e);

    for (int i = 0; i < 3; i++) {
        write_int(g->enchant[i]);
    }

    write_int(g->score);
    write_time_t(&(g->first_time));

    write_room(g->room);  // Assuming write_room handles NULL checks
    write_int(g->win);
}

void read_game(Game* g) {
    if (g == NULL) {
        g = (Game*)malloc(sizeof(Game));
    }
    

    int a;
    read_int(&a);
    if (a == 0) {
        free(g);
        g = NULL;
        return;
    }
    read_str(g->name);
    read_str(g->pass);
    read_str(g->email);

    read_int(&a); if(a==0) {g->state=0; return;}
    g->state=1;

    read_int(&(g->number_of_games));
    read_int(&(g->level));
    read_int(&(g->colour));

    read_pos(&(g->player));

    read_int(&(g->points));
    read_int(&(g->gold));
    read_int(&(g->rank));
    read_int(&(g->loged_in));

    for (int i = 0; i < 4; i++) {
        read_pos(&(g->start[i]));
    }
    for (int i = 0; i < 4; i++) {
        read_pos(&(g->end[i]));
    }

    read_int(&(g->floor));


    for (int i = 0; i < 5; i++) {
        read_int(&(g->gun[i]));
    }

    read_int(&(g->current_gun));
    read_int(&(g->direction));
    read_int(&(g->current_enchant));
    read_int(&(g->enchant_effect));

    read_int(&(g->health));
    read_time_t(&(g->last_update));
    read_int(&(g->hunger));
    read_time_t(&(g->want_food));
    read_int(&(g->food));

    read_double(&(g->master_key));
    read_time_t(&(g->temp_massage));

    read_int(&(g->speed_e));
    read_int(&(g->health_e));
    read_int(&(g->damage_e));

    for (int i = 0; i < 3; i++) {
        read_int(&(g->enchant[i]));
    }

    read_int(&(g->score));
    read_time_t(&(g->first_time));

    read_room(g->room);
    read_int(&g->win);
    
}

void save_game(Game* g){
    fptr=fopen(g->name,"w");
    //fseek(fptr,0,SEEK_SET);
    //for(int i=0;i<1000000;i++){
    //    fprintf(fptr,"     ");
    //}
    //fseek(fptr,0,SEEK_SET);


    write_game(g);
    write_arrays(g);

    for(int i=0;i<4;i++){
        
        for(int j=0;j<Rows-bRow;j++){
            for(int k=0;k<(Cols-bCol)/2;k++){
                Map[i][j][k]=0;
                MAP[i][j][k].door=NULL;
                MAP[i][j][k].Trap=NULL;
                MAP[i][j][k].Monster=NULL;
                MAP[i][j][k].gun=NULL;
                Show[i][j][k]=0;
            }
        }
    }
    //free(g);
    
}



int setting(Game *g){
    user_menu();
    pos cursor; cursor.x=Rows/2; cursor.y=Cols/2;
    mvprintw(1, 1, "settings");
    mvprintw(Rows/2-2,Cols/2-8,"hardness");
    mvprintw(Rows/2,Cols/2-6,"Color");
    mvprintw(Rows/2+2,Cols/2-6,"music");
    mvprintw(Rows/2+4,Cols/2-5,"back");
    while(1){
        move(cursor.x,cursor.y);
        refresh();
        keypad(stdscr, TRUE);
        noecho();
        int command=getch();

        if(command==KEY_UP) {
            if(cursor.x!=Rows/2-2) {
                echo();
                mvprintw(cursor.x,cursor.y,"   ");
                cursor.x -= 2;
                move(cursor.x,cursor.y);
                refresh();
            }
            else {
                echo();
                mvprintw(cursor.x,cursor.y,"   ");
                cursor.x += 6;
                move(cursor.x,cursor.y);
                refresh();
            }
        }
        if(command==KEY_DOWN) {
            if(cursor.x!=Rows/2+4) {
                echo();
                mvprintw(cursor.x,cursor.y,"   ");
                cursor.x += 2;
                move(cursor.x,cursor.y);
                refresh();
            }
            else {
                echo();
                mvprintw(cursor.x,cursor.y,"    ");
                cursor.x -= 6;
                move(cursor.x,cursor.y);
                refresh();
            }
        }
        if(command=='\n'){
            if(cursor.x==Rows/2-2){
                mvprintw(Rows/2+8,Cols/2-20,"1:Easy 2:medium 3:hard");
                refresh();
                char level =getch();
                if(level=='1') g->level=1;
                if(level=='2') g->level=2;
                if(level=='3') g->level=3;
                mvprintw(Rows/2+8,Cols/2-20,"                       ");
                //sleep(1);
            }
            else if(cursor.x==Rows/2){
                mvprintw(Rows/2+8,Cols/2-20,"O:Orange R:Red P:Pink");
                refresh();
                char color =getch();
                if(color=='O') g->colour=1;
                if(color=='R') g->colour=2;
                if(color=='P') g->colour=3;
                mvprintw(Rows/2+8,Cols/2-20,"                       ");
                //sleep(1);
            }
            else if(cursor.y==Rows/2+2){
                mvprintw(Rows/2+8,Cols/2-20,"This feature is not added yet!");
                refresh();
                sleep(1);
                mvprintw(Rows/2+8,Cols/2-20,"                               ");
                refresh();
            }
            else if(cursor.x==Rows/2+4) return 1;
        }
    }
    refresh();
    return 1;
}

int profile(Game *g){return 1;}

int leader_board(Game *g){
    clear();
    FILE* board = fopen("scoreboard.txt","r");
    char name[10][15];
    int score[10];
    int gold[10];
    int games[10];
    unsigned long long int t[10];
    char * temp=(char*)malloc(100*sizeof(char));
    int rank;
    fgets(temp,100,board);
    for(int i=0;i<10;i++){
        fscanf(board,"%d",&rank);
        fscanf(board,"%s",name[i]);
        fscanf(board,"%d",score+i);
        fscanf(board,"%d",gold+i);
        fscanf(board,"%d",games+i);
        fscanf(board,"%llu",t+i);
    }
    for(int i=0;i<10;i++){
        if(i==0){attron(COLOR_PAIR(6)); 
            move(Rows/2-8+2*i,Cols/2-36); waddstr(stdscr,"\U0001F947");
            mvprintw(Rows/2-8+2*i,Cols/2-32,"goat"); attron(A_BOLD);}
        if(i==1){attron(COLOR_PAIR(5));
            move(Rows/2-8+2*i,Cols/2-36); waddstr(stdscr,"\U0001F948");
            mvprintw(Rows/2-8+2*i,Cols/2-33,"legend"); attron(A_BOLD);}
        if(i==2){attron(COLOR_PAIR(1));
            move(Rows/2-8+2*i,Cols/2-36); waddstr(stdscr,"\U0001F949");
            mvprintw(Rows/2-8+2*i,Cols/2-32,"king"); attron(A_BOLD);}
        mvprintw(Rows/2-8+2*i,Cols/2-40,"%d",i+1);
        mvprintw(Rows/2-8+2*i,Cols/2-25,"%s",name[i]);
        mvprintw(Rows/2-8+2*i,Cols/2-20,"%d",score[i]);
        mvprintw(Rows/2-8+2*i,Cols/2-15,"%d",gold[i]);
        mvprintw(Rows/2-8+2*i,Cols/2-10,"%d",games[i]);
        t[i]=time(NULL)-t[i];
        int day,hour,minute,second;
        day=t[i]/86400; t[i] %= 86400;
        hour=t[i]/3600; t[i] %= 3600;
        minute=t[i]/60; t[i] %= 60;
        second=t[i];
        mvprintw(Rows/2-8+2*i,Cols/2-5,"%d days %d hours %d minutes %d seconds",day,hour,minute,second);
        if(i==0){attroff(COLOR_PAIR(6)); attroff(A_BOLD);}
        if(i==1){attroff(COLOR_PAIR(5)); attroff(A_BOLD);}
        if(i==2){attroff(COLOR_PAIR(1)); attroff(A_BOLD);}
    }

    fclose(board);
    refresh();
    while(getch()!='x');
    return 1;
}

int reverse(char* ramz){
    char temp;
    temp=ramz[3];   ramz[3]=ramz[0];    ramz[0]=temp;
    temp=ramz[2];   ramz[2]=ramz[1];    ramz[1]=temp;
    return 1;
}

int max(int num1, int num2) {
    if(num1>num2) return num1;
    else return num2;
}

int min(int num1, int num2) {
    if(num1<num2) return num1;
    else return num2;
}

int gen_rand(int begin,int end){
    if(begin==end) return end;
    //sleep(1);
    int res=rand();
    res=(res+10)%(end-begin);
    res += begin;
    return res;
}

int dice(double prob){
    //prob is between 0 and 1
    //we should return 0 or 1 like a coin but one side is prob and one is 1-prob
    int yes=0,no=0;
    double n=gen_rand(0,1000000);
    n /= 1000000;
    if(n<prob) return 1;
    else return 0;
}

int pro_dice(double* prob,int num){
    for(int i=1;i<num;i++){
        prob[i]=prob[i-1]+prob[i];
    }
    double n=gen_rand(0,100000);
    n /= 100000;
    for(int i=0;i<num;i++){
        if(n<prob[i]) return i;
    }
    return -1;
}

char * gen_pass(){
    char * A=(char*)malloc(4*sizeof(char));
    for(int i=0;i<4;i++){
        A[i]='0'+gen_rand(0,9);
    }
    return A;
}

int lockdoor(Game* g,int map[Rows-bRow][(Cols-bCol)/2],Door* d,Room* R,int floor,int theme){
    d->locked=0;
    if(dice(0.4)==1) d->reverse=1;
    else d->reverse=0;
    d->locked=2;
    for(int j=0;j<10;j++){
        g->what[g->floor][d->position.x][d->position.y][j]='\0';
    }
    strcpy(g->what[g->floor][d->position.x][d->position.y],"locked");
    map[d->position.x][d->position.y]=LOCK;
    double m[3]={0.25,0.25,0.5};
    if(floor<3){m[2]=0.75; m[0]=0;}
    int n=pro_dice(m,3);
    int X[4]={R->ver+1,R->ver+1,R->ver+R->length-1,R->ver+R->length-1};
    int Y[4]={R->hor+1,R->hor+R->width-1,R->hor+1,R->hor+R->width-1};
    int i=gen_rand(0,3);
    d->password_maker_1.x=X[i]; d->password_maker_1.y=Y[i];
    strcpy(g->what[g->floor][X[i]][Y[i]],"maker");
    MAP[g->floor][X[i]][Y[i]].door = d;
    MAP[g->floor][X[i]][Y[i]].door = (Door*)malloc(sizeof(Door));
    d->makers_num=1;
    for(int i=0;i<4;i++){
        d->password_1[i]='0'+gen_rand(0,9);
    }
    d->password_1[4]='\0';
    if(d->reverse==1) reverse(d->password_1);
    if(n==0){
        d->change=1;
        d->last_seen=time(NULL);
    }
    if(n==1){
        d->change=0;
        d->makers_num=2;
        d->password_maker_2.x=2*R->ver+R->length-X[i];
        d->password_maker_2.y=2*R->hor+R->width-Y[i];
        strcpy(g->what[g->floor][X[i]][Y[i]],"maker");
        MAP[g->floor][X[i]][Y[i]].door = (Door*)malloc(sizeof(Door));
        MAP[g->floor][X[i]][Y[i]].door = d;
        char * BB=(char*)malloc(4*sizeof(char)); BB=gen_pass();
        for(int i=0;i<4;i++) d->password_2[i]=BB[i];
        d->password_2[4]='\0';
        if(d->reverse==1) reverse(d->password_2);
    }
    if(n==2){
        d->change=0;
    }
}


void print_map(int map[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10]){
    for(int j=0;j<(Cols-bCol)/2;j++){
        for(int i=0;i<Rows-bRow;i++){
            if(show[i][j]!='y'&&M==0){
                move(i+bRow,2*j+bCol);
                waddstr(stdscr,"  ");
                continue;
            }
            int z=4;
            //if(strcmp(color[i][j],"red")==0){z=1;}
            //if(strcmp(color[i][j],"green")==0){z=3;}
            //if(strcmp(color[i][j],"yellow")==0){z=2;}
            //attron(COLOR_PAIR(z));
            char c[2]; c[0]=map[i][j]; c[1]='\0';
            //mvprintw(i+bRow,j+bCol,c);
            move(i+bRow,2*j+bCol);
            //int* A=(int*)malloc(sizeof(int)); *A=i+bRow; int* B=(int*)malloc(sizeof(int)); *B=2*j+bCol;
            if(map[i][j]== WALL)    waddstr(stdscr,"\U0001F7EB");
            else if(map[i][j]==WALL2){waddstr(stdscr,"\U0001F7EA");}
            else if(map[i][j]==WALL3){waddstr(stdscr,"\U0001F7E8");}
            else if(map[i][j]==FLOOR){waddstr(stdscr,"\U000026AA");}
            else if(map[i][j]==STAIR){waddstr(stdscr,"\U0001F6B9");}
            else if(map[i][j]==DOOR){waddstr(stdscr,"\U0001F6AA");}
            else if(map[i][j]==LOCK){waddstr(stdscr,"\U0001F512");}
            else if(map[i][j]==KEY){waddstr(stdscr,"\U0001F511");}
            else if(map[i][j]==TRAP){waddstr(stdscr,"\U0001F3FA");}
            else if(map[i][j]==CORRIDOR){waddstr(stdscr,"\U00002B1C");}
            else if(map[i][j]==PILLAR){waddstr(stdscr,"\U0001F7E0");}
            else if(map[i][j]==MASTER){waddstr(stdscr,"\U0001F53A");}
            else if(map[i][j]==RIGHT){waddstr(stdscr,"\U000027A1");}
            else if(map[i][j]==DEAMON){waddstr(stdscr,"\U0001F608");}
            else if(map[i][j]==FIREBREATH){waddstr(stdscr,"\U0001F409");}
            else if(map[i][j]==SNAKE){waddstr(stdscr,"\U0001F40D");}
            else if(map[i][j]==GIANT){waddstr(stdscr,"\U0001F479");}
            else if(map[i][j]==UNDEED){waddstr(stdscr,"\U0001F480");}
            else if(map[i][j]==FOOD){waddstr(stdscr,"\U0001F354");}
            else if(map[i][j]==GOLD){waddstr(stdscr,"\U0001F4B0");}
            else if(map[i][j]==HEALTH){waddstr(stdscr,"\U0001F34F");}
            else if(map[i][j]==SPEED){waddstr(stdscr,"\U000026A1");}
            else if(map[i][j]==POWER){waddstr(stdscr,"\U0001F4AA");}
            else if(map[i][j]==BGOLD){waddstr(stdscr,"\U0001F48E");}
            else if(map[i][j]==DAGGER){waddstr(stdscr,"\U0001F5E1");}
            else if(map[i][j]==WAND){waddstr(stdscr,"\U0001F9D9");}
            else if(map[i][j]==NORMALARROW){waddstr(stdscr,"\U000027B3");}
            else if(map[i][j]==SWORD){waddstr(stdscr,"\U00002694");}
            else waddstr(stdscr,"  ");

            //addstr("\U");
            //attroff(COLOR_PAIR(1));
            
            //if(show[i][j]==DOOR) mvprintw(i+bRow,j+bCol,"%c",map[i][j]);
            //else mvprintw(i+bRow,j+bCol,map[i][j]);
        }
    }
}

Room* gen_room(Game* g,int ver_0,int hor_0,int down,int theme,int floor,int lock_hide,int Key){  // ver_o : ->  hor_0 : ^
    Room* R=(Room*)malloc(sizeof(Room));
    R->ver=gen_rand(0,3)+ver_0; //amoodi
    //if(R->ver % 2==1) R->ver ++;
    R->hor=gen_rand(0,3)+hor_0; //ofoghi
    //if(R->hor%2==1) R->hor ++;
    R->length=gen_rand(12,13); if(theme==3) R->length += 2;
    R->width=gen_rand(12,15); if(theme==3) R->width += 2;
    R->num_doors = 1;
    R->theme=theme;

    for(int i=R->ver+1;i<R->ver+R->length;i++){
        for(int j=R->hor+1;j<R->hor+R->width;j++){
            Map[g->floor][i][j]=FLOOR;
        }
    }
    //Traps
    int Trap_num=(3*g->level+2*floor+theme)/3;
    if(theme==3) Trap_num=(g->level*10)/3;
    R->traps = (Trap*)malloc(Trap_num*sizeof(Trap));
    R->Trap_num=Trap_num;
    for(int j=0;j<Trap_num;j++){
        R->traps[j].position = (pos*)malloc(sizeof(pos));
        int place_x=gen_rand(R->ver+2,R->ver+R->length-2);
        int place_y=gen_rand(R->hor+2,R->hor+R->width-2);
        R->traps[j].position->x=place_x;
        R->traps[j].position->y=place_y;
        strcpy(g->what[g->floor][R->traps[j].position->x][R->traps[j].position->y],"trap");
    }


    //Master Key
    R->key=(pos *)malloc(sizeof(pos));
    if(Key==1){
        int place_x=gen_rand(R->ver+2,R->ver+R->length-2);
        int place_y=gen_rand(R->hor+2,R->hor+R->width-2);
        R->key->x=place_x;
        R->key->y=place_y;
        strcpy(g->what[g->floor][place_x][place_y],"master");
        R->master=1;
    }
    else{
        R->key=NULL;
    }

    // Pillar 
    int place_x=gen_rand(R->ver+2,R->ver+R->length-2);
    int place_y=gen_rand(R->hor+2,R->hor+R->width-2);
    R->pillar=(pos*)malloc(sizeof(pos));
    R->pillar->x=place_x;   R->pillar->y=place_y;

    //Enchant
    if(theme==1) R->enchant_num=1;
    if(theme==2) R->enchant_num=4;
    if(theme==3) R->enchant_num=2;
    R->enchant=(Enchant*)malloc(R->enchant_num*sizeof(Enchant));
    for(int i=0;i<R->enchant_num;i++){
        place_x=gen_rand(R->ver+2,R->ver+R->length-2);
        place_y=gen_rand(R->hor+2,R->hor+R->width-2);
        R->enchant[i].position.x=place_x;
        R->enchant[i].position.y=place_y;
        int item=gen_rand(0,8)%3;
        int items[3]={HEALTH,POWER,SPEED};
        strcpy(g->what[g->floor][place_x][place_y],"enchant");
        R->enchant[i].name=items[item];
        Map[g->floor][place_x][place_y]=items[item];
    }

    //Gold
    if(theme==3) R->gold_num=5;
    else R->gold_num=3;
    R->gold=(pos*)malloc(R->gold_num*sizeof(pos));
    for(int i=0;i<R->gold_num;i++){
        place_x=gen_rand(R->ver+2,R->ver+R->length-2);
        place_y=gen_rand(R->hor+2,R->hor+R->width-2);
        R->gold[i].x=place_x;
        R->gold[i].y=place_y;
        strcpy(g->what[g->floor][place_x][place_y],"gold");
        Map[g->floor][place_x][place_y]=GOLD;
        if(dice(0.1)==1)  Map[g->floor][place_x][place_y]=BGOLD;
    }
    //Food
        place_x=gen_rand(R->ver+2,R->ver+R->length-2);
        place_y=gen_rand(R->hor+2,R->hor+R->width-2);
        R->food=(pos*)malloc(sizeof(pos));
        R->food->x=place_x;
        R->food->y=place_y;
        strcpy(g->what[g->floor][place_x][place_y],"food");
        Map[g->floor][place_x][place_y]=FOOD;
    //weapon
    place_x=gen_rand(R->ver+2,R->ver+R->length-2);
    place_y=gen_rand(R->hor+2,R->hor+R->width-2);
    R->gun=(Gun*)malloc(sizeof(Gun));
    R->gun->position.x=place_x;
    R->gun->position.y=place_y;
    int item=gen_rand(0,3);
    int items[4]={DAGGER,WAND,NORMALARROW,SWORD};
    int num[4]={10,8,20,1000};
    strcpy(g->what[g->floor][place_x][place_y],"gun");
    R->gun->name=items[item];
    R->gun->num=num[item];
    Map[g->floor][place_x][place_y]=items[item];
    MAP[g->floor][place_x][place_y].gun = (Gun*)malloc(sizeof(Gun));
    *MAP[g->floor][place_x][place_y].gun=*(R->gun);
    
    //monsters
    int mons[5]={DEAMON,FIREBREATH,GIANT,SNAKE,UNDEED};
    int moves[5]={0,0,5,1000,5};
    int lives[5]={5,10,15,20,5};
    R->Enemy_num=gen_rand((floor+g->level)/5,(3*floor+3*g->level)/5);
    if(theme==2) R->Enemy_num=0;
    if(theme==3) R->Enemy_num=4;
    R->monsters=(Monster *)malloc(R->Enemy_num*sizeof(Monster));
    for(int i=0;i<R->Enemy_num;i++){
        int place_x=gen_rand(R->ver+3,R->ver+R->length-3);
        int place_y=gen_rand(R->hor+3,R->hor+R->width-3);
        R->monsters[i].position.x=place_x;
        R->monsters[i].position.y=place_y;
        strcpy(g->what[g->floor][R->monsters[i].position.x][R->monsters[i].position.x],"monster");
        double probs[5];
        if(floor==1) {probs[0]=0.4; probs[1]=0.4; probs[2]=0.1; probs[3]=0.1; probs[4]=0;}
        if(floor==2) {probs[0]=0.2; probs[1]=0.3; probs[2]=0.3; probs[3]=0.2; probs[4]=0;}
        if(floor==3) {probs[0]=0.05; probs[1]=0.1; probs[2]=0.25; probs[3]=0.3; probs[4]=0.3;}
        if(floor==4) {probs[0]=0.0; probs[1]=0.1; probs[2]=0.2; probs[3]=0.3; probs[4]=0.4;}
        int par=pro_dice(probs,5);
        R->monsters[i].name=mons[par];
        R->monsters[i].life=lives[par];
        R->monsters[i].moves=moves[par];
        Map[g->floor][place_x][place_y]=mons[par];
        MAP[g->floor][place_x][place_y].Monster=(Monster*)malloc(sizeof(Monster));
        MAP[g->floor][place_x][place_y].Monster=&(R->monsters[i]);
    }

    int hard=0;
    int hide[3];
    int lock[3];
    int p=1;
    for(int i=0;i<3;i++){
        p*=3;
        /*double x=2*g->level+floor;  x/=7;*/
        double x=1;
        hard=dice(x);
        hide[i]=0;
        lock[i]=0;
        if((lock_hide/(p/3))%3==1){
            hide[i]=1;
            lock[i]=0;
        }
        if((lock_hide/(p/3))%3==2){
            lock[i]=1;
            hide[i]=0;
        }
        if(theme==2){
            lock[i]=0;
            hide[i]=0;
        }
    }

    
    for(int i=0;i<4;i++){
        R->door[i]=(Door *)malloc(sizeof(Door));
        R->door[i]->room=(Room*)malloc(sizeof(Room));
        R->door[i]->room=R;
        R->door[i]->hidden = 0;
        R->door[i]->position.x = 0;
        R->door[i]->position.y = 0;
        R->door[i]->makers_num=0;
        R->door[i]->locked=0;
        R->master=0;
    }
    // Left Door:
    R->door[0]->position.x=gen_rand(R->ver+1,R->ver+R->length-1);
    R->door[0]->position.y=R->hor;
    R->door[0]->room=(Room*)malloc(sizeof(Room));
    R->door[0]->room=R;
    //Map[g->floor][R->door[0]->position.x][R->door[0]->position.y]=DOOR;
    MAP[g->floor][R->door[0]->position.x][R->door[0]->position.y].door =(Door*)malloc(sizeof(Door));
    MAP[g->floor][R->door[0]->position.x][R->door[0]->position.y].door = R->door[0];
    strcpy(g->what[g->floor][R->door[0]->position.x][R->door[0]->position.y],"door");
    R->num_doors ++;
    R->door[0]->hidden=0;
    if(hide[0]==1){
        R->door[0]->hidden=1; R->door[0]->locked=0;
        strcpy(g->what[g->floor][R->door[0]->position.x][R->door[0]->position.y],"door");
    }
    else if(lock[0]==1){
        R->door[0]->locked = 2;
        lockdoor(g,Map[floor-1],(R->door[0]),R,floor,theme);
    }
    // North or South 
    if(down==1){
        R->door[3]=NULL;
        R->door[1]->position.y=gen_rand(R->hor+1,R->hor+R->width-1);
        R->door[1]->position.x=R->ver;
        R->door[1]->room=(Room*)malloc(sizeof(Room));
        //Map[g->floor][R->door[1]->position.x][R->door[1]->position.y]=DOOR;
        MAP[g->floor][R->door[1]->position.x][R->door[1]->position.y].door =(Door*)malloc(sizeof(Door));
        MAP[g->floor][R->door[1]->position.x][R->door[1]->position.y].door = R->door[0];
        R->door[1]->room=R;
        strcpy(g->what[g->floor][R->door[1]->position.x][R->door[1]->position.y],"door");
        R->door[1]->hidden=0;
        if(hide[1]==1){
            R->door[1]->hidden=1; R->door[1]->locked=0;
            strcpy(g->what[g->floor][R->door[1]->position.x][R->door[1]->position.y],"door");
        }
        else if(lock[1]==1){
            R->door[1]->locked = 2;
            lockdoor(g,Map[floor-1],(R->door[1]),R,floor,theme);
        }
    }
    else{
        R->door[1]=NULL;
        R->door[3]->position.x=R->ver+R->length;
        R->door[3]->position.y=gen_rand(R->hor+1,R->hor+R->width-1);
        R->door[3]->room=(Room*)malloc(sizeof(Room));
        MAP[g->floor][R->door[3]->position.x][R->door[3]->position.y].door =(Door*)malloc(sizeof(Door));
        MAP[g->floor][R->door[3]->position.x][R->door[3]->position.y].door = R->door[0];
        //Map[g->floor][R->door[3]->position.x][R->door[3]->position.y]=DOOR;
        R->door[3]->room=R;
        strcpy(g->what[g->floor][R->door[3]->position.x][R->door[3]->position.y],"door");
        R->door[3]->hidden=0;
        if(hide[1]==1){
            R->door[3]->hidden=1; R->door[3]->locked=0;
            strcpy(g->what[g->floor][R->door[3]->position.x][R->door[3]->position.y],"door");
        }
        else if(lock[1]==1){
            R->door[3]->locked = 2;
            lockdoor(g,Map[floor-1],(R->door[3]),R,floor,theme);
        }
    }
    //  Right door
    R->door[2]->position.x=gen_rand(R->ver+1,R->ver+R->length-1);
    R->door[2]->position.y=R->hor+R->width;
    R->door[2]->room=(Room*)malloc(sizeof(Room));
    MAP[g->floor][R->door[2]->position.x][R->door[2]->position.y].door =(Door*)malloc(sizeof(Door));
    MAP[g->floor][R->door[2]->position.x][R->door[2]->position.y].door = R->door[0];
    //Map[g->floor][R->door[2]->position.x][R->door[2]->position.y]=DOOR;
    R->door[2]->room=R;
    strcpy(g->what[g->floor][R->door[2]->position.x][R->door[2]->position.y],"door");
    R->num_doors ++;
    R->door[2]->hidden=0;
    if(hide[2]==1){
        R->door[2]->hidden=1; R->door[2]->locked=0;
        strcpy(g->what[g->floor][R->door[2]->position.x][R->door[2]->position.y],"door");
    }
    else if(lock[2]==1){
        R->door[2]->locked = 2;
        lockdoor(g,Map[floor-1],(R->door[2]),R,floor,theme);
    }
    for(int k=0;k<4;k++){
        if(R->door[k]!=NULL){
            pos* a=(pos*)malloc(sizeof(pos)); *a=R->door[k]->position;
            if(R->door[k]->hidden==1){
                if(R->theme==1){Map[g->floor][a->x][a->y]=WALL;}
                if(R->theme==2){Map[g->floor][a->x][a->y]=WALL2;}
                if(R->theme==3){Map[g->floor][a->x][a->y]=WALL3;}
            }
            if(R->door[k]->locked==2){
                Map[g->floor][a->x][a->y]=LOCK;
            }
        }
    }
    return R;
}

void add_room(Game* g,Room* R,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10]){
    for(int i=R->ver;i<=R->ver+R->length;i++){
        if(R->master==1){
            map[R->key->x][R->key->y]=MASTER;
            char Z[10]="master";
            strcpy(g->what[g->floor][R->key->x][R->key->y],Z);
        }
        for(int j=R->hor;j<=R->hor+R->width;j++){
            //map[i][j]=FLOOR;
            //show[i][j]='y';   /*  SHOULD CHANGE IT IN THE FUTURE */
            if(i==R->pillar->x&&j==R->pillar->y) {
                map[i][j]=PILLAR;
                strcpy(g->what[g->floor][i][j],"pillar");
            }
            if(i==R->ver||i==R->ver+R->length){
                map[i][j]=WALL;
                if(R->theme==2) map[i][j]=WALL2;
                if(R->theme==3) map[i][j]=WALL3;
                strcpy(g->what[g->floor][i][j],"wall");
            }
            if(R->hor==j||j==R->hor+R->width){
                map[i][j]=WALL;
                if(R->theme==2) map[i][j]=WALL2;
                if(R->theme==3) map[i][j]=WALL3;
                strcpy(g->what[g->floor][i][j],"wall");
            }
        }
    }
    for(int k=0;k<4;k++){
        if(R->door[k]!=NULL){
            int X=0,Y=0;
            X=R->door[k]->position.x;
            Y=R->door[k]->position.y;
            map[X][Y]=DOOR;
            obs[X][Y].door=(Door*)malloc(sizeof(Door));
            if (MAP[g->floor][X][Y].door != NULL) {
                free(MAP[g->floor][X][Y].door);
            }
            obs[X][Y].door=R->door[k];
            strcpy(g->what[g->floor][X][Y],"door");
            if(R->door[k]->hidden==1){
                //map[X][Y]=DOOR;
                strcpy(g->what[g->floor][X][Y],"hidden");
                map[X][Y]=LOCK;
            }
            if(R->door[k]->locked>=2){
                map[X][Y]=LOCK;
                //strcpy(g->what[g->floor][X][Y],"locked");
                //strcpy(color[X][Y],"red");
                int maker_x,maker_y;
                maker_x=R->door[k]->password_maker_1.x;
                maker_y=R->door[k]->password_maker_1.y;
                obs[maker_x][maker_y].door=(Door*)malloc(sizeof(Door));
                obs[maker_x][maker_y].door = R->door[k];
                map[maker_x][maker_y]=KEY;
                strcpy(g->what[g->floor][maker_x][maker_y],"key");
                //strcpy(color[maker_x][maker_y],"yellow");
                if(R->door[k]->makers_num==2){
                    maker_x=R->door[k]->password_maker_2.x;
                    maker_y=R->door[k]->password_maker_2.y;
                    map[maker_x][maker_y]=KEY;
                    obs[maker_x][maker_y].door=(Door*)malloc(sizeof(Door));
                    obs[maker_x][maker_y].door = R->door[k];
                    strcpy(g->what[g->floor][maker_x][maker_y],"key");
                    //strcpy(color[maker_x][maker_y],"yellow");
                }
            }
        }
    }
    for(int k=0;k<R->Trap_num;k++){
        int X=0,Y=0;
        X=R->traps[k].position->x;
        Y=R->traps[k].position->y;
        map[X][Y]=FLOOR;
        strcpy(g->what[g->floor][X][Y],"trap");
        obs[X][Y].Trap=(Trap*)malloc(sizeof(Trap));
        obs[X][Y].Trap=&R->traps[k];
    }
    for(int k=0;k<R->Enemy_num;k++){
        int X=0,Y=0;
        X=R->monsters[k].position.x;
        Y=R->monsters[k].position.y;
        map[X][Y]=R->monsters[k].name;
        strcpy(g->what[g->floor][X][Y],"monster");
        obs[X][Y].Monster=(Monster*)malloc(sizeof(Monster));
        obs[X][Y].Monster=&R->monsters[k];
    }
    if(R->key!=NULL){
        int X,Y=0;
        X=R->key->x;
        Y=R->key->y;
    }  
}

pos* make_map(pos* begin,Game * g,int map[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10],object obs[Rows-bRow][(Cols-bCol)/2],int floor){
    floor= g->floor +1;
    memset(Map[g->floor], 0, sizeof(Map[g->floor]));
    for (int i = 0; i < Rows - bRow; i++) {
        for (int j = 0; j < (Cols - bCol)/2; j++) {
            //Map[g->floor][i][j] = 'a';
            Show[g->floor][i][j] = 'n';
            if(obs[i][j].door != NULL) obs[i][j].door = NULL;
            if(obs[i][j].Monster != NULL) obs[i][j].Monster = NULL;
            if(obs[i][j].Trap != NULL) obs[i][j].Trap = NULL;
            if(obs[i][j].gun != NULL) obs[i][j].gun = NULL;

            g->what[g->floor][i][j][0]='\0';
            g->what[g->floor][i][j][1]='\0';
            //memset(g->what[floor][i][j], 0, 10); // Clear "what" array
            //memset(&obs[i][j], 0, sizeof(object)); // Reset objects
        }
    }
    //making borders
    for(int i=0;i<Rows-bRow;i++){
        for(int j=0;j<(Cols-bCol)/2;j++){
            map[i][j]=' ';
            if(i==0||j==0) map[i][j]='X';
            if(i==Rows-bRow-1||j==(Cols-bCol)/2-1) map[i][j]='X';
            show[i][j]='n';
            map[i][j]=' ';

        }
    }
    Room* R[6];

    int x[4]={gen_rand(1,2700)%3,gen_rand(1,2700)%3,gen_rand(1,2700)%3,gen_rand(1,2700)%3};
    if(g->floor%2==1) {x[0] = 3*(x[0]); x[1]=(x[1]); x[2]=3*x[2]; x[3]=3*(x[3]);}
    else {x[0] = 9*(x[0]); x[1]=3*(x[1]); x[2]=(x[2]); x[3]=(x[3]);}
    int A=gen_rand(0,666)%6;
    int T[6]={1+(dice(0.2)==1),1+(dice(0.3)==1),1+(dice(0.5)==1),1+(dice(0.4)==1),1+dice(0.3)==1,1+(dice(0.2)==1)};
    if(floor==4) T[0]=3;
    R[0]=gen_room(g,3,7,0,T[0],4,0,A==0);    R[1]=gen_room(g,3,29,0,T[1],4,0,A==2);    R[2]=gen_room(g,3,54,0,T[2],3,0,A==4);
    R[5]=gen_room(g,25,7,1,T[5],1,0,A==1);    R[4]=gen_room(g,25,29,1,T[4],2,0,A==3);    R[3]=gen_room(g,25,54,1,T[3],3,1,A==5);

    for(int i=0;i<6;i++){
        add_room(g,R[i],map,obs,show,color);
    }
    for(int i=0;i<(Cols-bCol)/2;i++){
        for(int j=0;j<10;j++){
            int X=gen_rand(3,Rows-bRow-3);
            if(map[i][X]=='O') map[i][X]='X';
        }
    }

    // Add stairs
    int n = gen_rand(3, 5), m = gen_rand(3, 5);
    m+= (floor%2==1)*25;
    map[m][n] = STAIR;
    pos* ending=(pos*)malloc(sizeof(pos));
    ending->x=m; ending->y=n;
    map[begin->x][begin->y]=RIGHT;

    //adding path between doors
    int t_start,t_finish;
    int door_start[5]={2,2,3,0,0};
    int door_finish[5]={3,0,1,1,2};
    char ** mark;
    pos ** parent;

    for(int i=0;i<5;i++){
        int d_s,d_f;
        initialize(Rows-bRow,(Cols-bCol)/2,map,&mark,&parent);
        d_s=door_start[i];
        d_f=door_finish[i];
        pos Start; Start.x=R[i]->door[d_s]->position.x; Start.y=R[i]->door[d_s]->position.y;
        pos Finish; Finish.x=R[i+1]->door[d_f]->position.x; Finish.y=R[i+1]->door[d_f]->position.y;
        t_start=map[Start.x][Start.y];
        t_finish=map[Finish.x][Finish.y];
        mark[Start.x][Start.y]='v';
        if(find_path(map,&Start,&Finish,Start.x,Start.y,&mark,&parent)==0) return NULL;
        add_path(g,Start,Finish,parent,map);
        map[Finish.x][Finish.y]=t_finish;
        map[Start.x][Start.y]=t_start;
    }
    pos* Finish=(pos*)malloc(sizeof(pos)); Finish->x=R[(1-floor%2)*5]->door[0]->position.x; Finish->y=R[(1-floor%2)*5]->door[0]->position.y;
    initialize(Rows-bRow,(Cols-bCol)/2,map,&mark,&parent);
    t_start=map[begin->x][begin->y];
    t_finish=map[Finish->x][Finish->y];
    if(find_path(map,begin,Finish,begin->x,begin->y,&mark,&parent)==0) return NULL;
    add_path(g,*begin,*Finish,parent,map);
    map[Finish->x][Finish->y]=t_finish;
    map[begin->x][begin->y]=t_start;

    pos* Start=(pos*)malloc(sizeof(pos)); Start->x=R[(floor%2)*5]->door[0]->position.x;   Start->y=R[(floor%2)*5]->door[0]->position.y;
    initialize(Rows-bRow,(Cols-bCol)/2,map,&mark,&parent);
    t_start=map[Start->x][Start->y];
    t_finish=map[ending->x][ending->y];
    if(find_path(map,Start,ending,Start->x,Start->y,&mark,&parent)==0) return NULL;
    add_path(g,*Start,*ending,parent,map);
    map[ending->x][ending->y]=t_finish;
    map[Start->x][Start->y]=t_start;


    map[begin->x][begin->y]=RIGHT;
    strcpy(g->what[g->floor][begin->x][begin->y],"stairbegin");
    map[ending->x][ending->y]=STAIR;
    strcpy(g->what[g->floor][ending->x][ending->y],"stairend");
    //pos* start; pos* end; start=R_1->door_3;  end=R_2->door_2;
    //make_path(g,map,start,end);
    for(int i=0;i<6;i++){
        for(int k=0;k<4;k++){
            if(R[i]->door[k]!=NULL){
                if(R[i]->door[k]->hidden==1){
                    if(R[i]->theme==1) map[R[i]->door[k]->position.x][R[i]->door[k]->position.y]=WALL;
                    if(R[i]->theme==2) map[R[i]->door[k]->position.x][R[i]->door[k]->position.y]=WALL2;
                    if(R[i]->theme==3) map[R[i]->door[k]->position.x][R[i]->door[k]->position.y]=WALL3;
                }
                if(R[i]->theme==2){
                    map[R[i]->door[k]->position.x][R[i]->door[k]->position.y]=WALL2;
                    strcpy(g->what[g->floor][R[i]->door[k]->position.x][R[i]->door[k]->position.y],"hidden");
                    R[i]->door[k]->hidden=1;
                }
            }
        }
    }
    return ending;
}

int find_path(int map[Rows-bRow][(Cols-bCol)/2], pos* start, pos* end, int xx, int yy, char*** mark, pos*** parent) {
    // Base case: check if current position is the finish
    //map[x][y] = 'B';
    //printf("Visiting (%d, %d)\n", x, y);
    if (xx == end->x && yy == end->y) {
        return 1;
    }

    // Mark as visited
    (*mark)[xx][yy] = 'v';

    // Fixed search directions (right, up, left, down)
    int deltax[4] = {0, -1, 0, 1};
    int deltay[4] = {1, 0, -1, 0};

    if(xx<end->x){
        for(int i=0;i<4;i++){
            deltax[i]*=-1;
        }
    }
    if(yy>end->y){
        for(int i=0;i<4;i++){
            deltay[i]*=-1;
        }
    }
    if(abs(xx-end->x)>abs(yy-end->y)){
        for(int i=0;i<4;i++){
            int temp=deltax[i];
            deltax[i]=deltay[i];
            deltay[i]=temp;
        }
    }
    
    for (int i = 0; i < 4; i++) {
        int nx = xx + deltax[i];
        int ny = yy + deltay[i];
        
        // Boundary check
        if (nx <= 0 || nx >= Rows - bRow || ny <= 0 || ny >= (Cols - bCol)/2) {
            //printf("1\n");
            continue;
        }
        if(nx>max(start->x,end->x)+2||ny>max(start->y,end->y)+2||nx<min(start->x,end->x)-2||ny<min(start->y,end->y)-2){
            //printf("2\n");
            //printf(".");
            adad++;
            continue;
        }
        
        // Check if within search area and passable
        if ((*mark)[nx][ny] == 'n' && map[nx][ny] != WALL && map[nx][ny] != WALL2 && map[nx][ny] != WALL3 && map[nx][ny] != FLOOR /*&& map[nx][ny]!=LOCK && map[nx][ny]!=DOOR*/) {
            (*parent)[nx][ny].x = xx;
            (*parent)[nx][ny].y = yy;
            
            if (find_path(map, start, end, nx, ny, mark, parent)) {
                //map[x][y] = 'B'; // Mark backtracking step
                //printf("yes!\n");
                return 1;
            }
        }
        else{
            //printf("3\n");
        }
    }
    return 0;
}

void initialize(int i_size, int j_size, int map[Rows-bRow][(Cols-bCol)/2], char*** mark, pos*** parent) {
    // Allocate and initialize mark
    *mark = (char**)malloc(sizeof(char*) * i_size);
    for (int i = 0; i < i_size; i++) {
        (*mark)[i] = (char*)malloc(sizeof(char) * j_size);
        for (int j = 0; j < j_size; j++) {
            (*mark)[i][j] = 'n';
        }
    }

    // Allocate and initialize parent
    *parent = (pos**)malloc(sizeof(pos*) * i_size);
    for (int i = 0; i < i_size; i++) {
        (*parent)[i] = (pos*)malloc(sizeof(pos) * j_size);
        for (int j = 0; j < j_size; j++) {
            (*parent)[i][j].x = -1;  // Use . instead of ->
            (*parent)[i][j].y = -1;  // Initialize to invalid values
        }
    }
}

void add_path(Game* g,pos start,pos current,pos** parent,int map[Rows-bRow][(Cols-bCol)/2]) {
    if (current.x < 0 || current.x >= Rows - bRow ||
        current.y < 0 || current.y >= Cols - bCol) {
        //printf("no no no\n");
        return;
    }
    if (strcmp(g->what[g->floor][current.x][current.y], "key") == 0 ||
        strcmp(g->what[g->floor][current.x][current.y], "locked") == 0) {
        return;
    }

    if (strcmp(g->what[g->floor][current.x][current.y], "door") != 0) {
        map[current.x][current.y] = CORRIDOR;
        strcpy(g->what[g->floor][current.x][current.y], "corridor");
    }
    map[current.x][current.y]=CORRIDOR;
    strcpy(g->what[g->floor][current.x][current.y],"corridor");
    //mvprintw(current.x,current.y,"#");
    //refresh();
    if(current.x != start.x || current.y != start.y) {
        if (parent[current.x] == NULL || parent[current.x][current.y].x < 0 ||
            parent[current.x][current.y].x >= Rows - bRow ||
            parent[current.x][current.y].y < 0 ||
            parent[current.x][current.y].y >= Cols - bCol) {
            // Invalid parent position
            return;
        }
        //printf("added (%d, %d)\n", current.x, current.y);
        add_path(g,start,parent[current.x][current.y], parent,map);
    }
}

int is_block(Game* g,pos * current,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2]){
    
    //if(Map[g->floor][current->x][current->y]==' ') return 1;
    if (obs[current->x][current->y].door != NULL) return 0;
    if(Map[g->floor][current->x][current->y]==' ') return 1;
    if(Map[g->floor][current->x][current->y]==WALL) return 1;
    if(Map[g->floor][current->x][current->y]==WALL2) return 1;
    if(Map[g->floor][current->x][current->y]==WALL3) return 1;
    if(Map[g->floor][current->x][current->y]==PILLAR) return 1;
    //if(strcmp(g->what[g->floor][current->x][current->y],"locked")==0) return 1;
    if(map[current->x][current->y]==' ') return 1;
    if(obs[current->x][current->y].door != NULL) return 0;
    return 0;

};

void show_food(Game* g){
    int start_r=10,end_r=20;
    int start_c=10,end_c=41;
    for(int i=start_r;i<=end_r;i++){
        move(i,start_c);
        waddstr(stdscr,"\U000026AA");
        move(i,end_c);
        waddstr(stdscr,"\U000026AA");
    }
    for(int i=start_c/2;i<=end_c/2;i++){
        move(start_r,2*i);
        waddstr(stdscr,"\U000026AA");
        move(end_r,2*i);
        waddstr(stdscr,"\U000026AA");
    }
    while(1){
        mvprintw((start_r+end_r)/2,(2*start_c+end_c)/3,"food: %d",(g->food));
        char A=getch();
        if(A=='x') {clear(); return ;}
        if(A=='+'){
            if(g->food==0){
                mvprintw(18,17,"you have no food");
                refresh();
                sleep(1);
                mvprintw(18,17,"                     ");
                refresh();
            }
            else if(g->health==MAX_LEALTH){
                mvprintw(18,17,"you can't be healthier than this!");
                refresh();
                sleep(1);
                mvprintw(18,17,"                                 ");
                refresh();
            }
            else{
                g->food --;
                g->health +=10;
                g->hunger += 10;
            }
        }
    }
}

void show_gun(Game* g){
    int start_r=10,end_r=40;
    int start_c=5,end_c=48;
    for(int i=start_r;i<=end_r;i++){
        move(i,start_c);
        waddstr(stdscr,"\U000026AA");
        move(i,end_c);
        waddstr(stdscr,"\U000026AA");
    }
    for(int i=start_c/2;i<=end_c/2;i++){
        move(start_r,2*i);
        waddstr(stdscr,"\U000026AA");
        move(end_r,2*i);
        waddstr(stdscr,"\U000026AA");
    }
    while(1){
        mvprintw((2*start_r+end_r)/3,(2*start_c+end_c)/3,"1.Mace: %d",g->gun[0]);
        mvprintw((2*start_r+end_r)/3+2,(2*start_c+end_c)/3,"2.Dagger: %d",g->gun[1]);
        mvprintw((2*start_r+end_r)/3+4,(2*start_c+end_c)/3,"3.wand: %d",g->gun[2]);
        mvprintw((2*start_r+end_r)/3+6,(2*start_c+end_c)/3,"4.Normal arrow: %d",g->gun[3]);
        mvprintw((2*start_r+end_r)/3+8,(2*start_c+end_c)/3,"5.Sword: %d",g->gun[4]);
        char A=getch();
        if(A>='1'&&A<='5'){
            if(g->current_gun!=-1){
                mvprintw(18,15,"you have to free your bag");
                refresh();
                sleep(1);
                mvprintw(18,15,"                                 ");
                refresh();
                continue;
            }
            if(g->gun[A-'1']==0){
                mvprintw(18,15,"you don't have this gun!");
                refresh();
                sleep(1);
                mvprintw(18,15,"                                 ");
                refresh();
            }
            else {
                char D[5][15]={"Mace","Dagger","Wand","Normal arrow","Sword"};
                g->current_gun=A-'1';
                mvprintw(18,15,"you deault gun: %s",D[A-'1']);
                refresh();
                sleep(1);
                mvprintw(18,15,"                                 ");
                refresh();
            }
        }
        if(A=='w') {g->current_gun =-1;}
        if(A=='x') {clear(); return;}
    }
}

void show_enchant(Game* g){
    int start_r=10,end_r=40;
    int start_c=5,end_c=48;
    for(int i=start_r;i<=end_r;i++){
        move(i,start_c);
        waddstr(stdscr,"\U000026AA");
        move(i,end_c);
        waddstr(stdscr,"\U000026AA");
    }
    for(int i=start_c/2;i<=end_c/2;i++){
        move(start_r,2*i);
        waddstr(stdscr,"\U000026AA");
        move(end_r,2*i);
        waddstr(stdscr,"\U000026AA");
    }
    while(1){
        mvprintw((2*start_r+end_r)/3+2,(2*start_c+end_c)/3,"1.Health: %d",g->enchant[0]);
        mvprintw((2*start_r+end_r)/3+4,(2*start_c+end_c)/3,"2.Speed: %d",g->enchant[1]);
        mvprintw((2*start_r+end_r)/3+6,(2*start_c+end_c)/3,"3.Damage: %d",g->enchant[2]);
        char A=getch();
        if(A>='1'&&A<='3'){
            if(g->enchant[A-'1']<=0){
                mvprintw(18,15,"you don't have this enchant!");
                refresh();
                g->temp_massage=time(NULL)+28;
                refresh();
            }
            else {
                if(A=='1') {g->health_e=10; g->enchant[0] --;}
                if(A=='2') {g->speed_e=10; g->enchant[1] --;}
                if(A=='3') {g->damage_e=10; g->enchant[2] --;}
            }
        }
        if(A=='x') {clear(); return;}
    }
}

int move_monster(Game* g){
    //movement of monsters
    if(g->room!=NULL){
        for(int i=0;i<g->room->Enemy_num;i++){
            int m_x=g->room->monsters[i].position.x;
            int m_y=g->room->monsters[i].position.y;
            if(Map[g->floor][m_x][m_y]!=DEAMON &&Map[g->floor][m_x][m_y]!=FIREBREATH &&Map[g->floor][m_x][m_y]!=GIANT &&
            Map[g->floor][m_x][m_y]!=SNAKE &&Map[g->floor][m_x][m_y]!=UNDEED){
                continue;
            }
            int temp=Map[g->floor][m_x][m_y];
            if(g->room->monsters[i].life<=0) continue;
            if(abs(g->player.x-m_x)+abs(g->player.y-m_y)<2){
                g->health --;
                //game_massage("")
                return 0;
            }
            if(g->room->monsters[i].moves<=0) continue;
            
            
            int dx[3]={-1,0,1},dy[3]={-1,0,1};
            if(m_x<g->player.x) {dx[0]=1; dx[1]=0; dx[2]=-1;}
            if(m_y<g->player.y) {dy[0]=1; dy[1]=0; dy[2]=-1;}
            if(m_x==g->player.x) {dx[0]=0; dx[1]=1; dx[2]=-1;}
            if(m_y==g->player.y) {dy[0]=0; dy[1]=1; dy[2]=-1;}
            for(int j=0;j<3;j++){
                int sign=0;
                for(int k=0;k<3;k++){
                    if(Map[g->floor][m_x+dx[j]][m_y+dy[k]]==FLOOR){
                        g->room->monsters[i].position.x += dx[j];
                        g->room->monsters[i].position.y += dy[k];
                        int nx=g->room->monsters[i].position.x , ny=g->room->monsters[i].position.y;
                        Map[g->floor][nx][ny]=Map[g->floor][m_x][m_y];
                        Map[g->floor][m_x][m_y]=FLOOR;
                        MAP[g->floor][nx][ny].Monster=(Monster*)malloc(sizeof(Monster));
                        *(MAP[g->floor][nx][ny].Monster)=*(MAP[g->floor][m_x][m_y].Monster);
                        MAP[g->floor][m_x][m_y].Monster =NULL;
                        g->room->monsters[i].moves --;
                        sign=1;
                        break;
                    }
                }
                if(sign==1) break;
            }
        }
    }
}

int move_player(char A,Game* g,pos start,pos end,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2]){
        //game_massage("this is a test");
        g->damage_e --; g->speed_e --; g->health_e --;
        int collect=1;
        for(int i=-5;i<=5;i++){
            for(int j=-5;j<=5;j++){
                if(g->player.x+i<=0||g->player.y+j<=0) continue;
                Show[g->floor][g->player.x+i][g->player.y+j]='y';
            }
        }
        pos current;
        current=g->player;
        pos* b=(pos*)malloc(sizeof(pos)); *b=current;
        //echo();
        if(A=='M'||A=='m') {M=1-M; /*mvprintw(4,4,"yes");*/ return 0;}
        if(A=='t'){
            show_enchant(g);
        }
        if(A=='f'){
            A=getch();
            int dx=0,dy=0;
            if(A=='7'){dy--; dx--;} if(A=='8'){dx--;} if(A=='9'){dy++; dx--;}     
            if(A=='4'){dy--;}                             if(A=='6'){dy++;}
            if(A=='1'){dy--; dx++;} if(A=='2'){dx++;} if(A=='3'){dy++; dx++;}
            
            while(1){
                g->player.x+= dx; g->player.y += dy;
                if(Map[g->floor][g->player.x][g->player.y]!=FLOOR&&Map[g->floor][g->player.x][g->player.y]!=CORRIDOR){
                    g->player.x -= dx;
                    g->player.y -= dy;
                    break;
                }
                for(int i=-5;i<=5;i++){
                    for(int j=-5;j<=5;j++){
                        if(g->player.x+i<=0||g->player.y+j<=0) continue;
                        Show[g->floor][g->player.x+i][g->player.y+j]='y';
                    }
                }
            }
            return 0;
        }
        if(A=='e'){
            show_food(g);
        }
        if(A=='s'){
            for(int dx=-1;dx<=1;dx++){
                for(int dy=-1;dy<=1;dy++){
                    if(strcmp(g->what[g->floor][g->player.x+dx][g->player.y+dy],"trap")==0) Map[g->floor][g->player.x+dx][g->player.y+dy]=TRAP;
                    if(strcmp(g->what[g->floor][g->player.x+dx][g->player.y+dy],"hidden")==0) Map[g->floor][g->player.x+dx][g->player.y+dy]=DOOR;
                    if(obs[g->player.x+dx][g->player.y+dy].door!=NULL&&Map[g->floor][g->player.x+dx][g->player.y+dy]!=DOOR)
                        Map[g->floor][g->player.x+dx][g->player.y+dy]=DOOR;
                }
            }
            return 0;
        }
        if(A=='g'){
            collect=0;
            A=getch();
        }
        if(A=='i'){
            show_gun(g);
            return 0;
        }
        int DX=0,DY=0;
        if(A=='7'){DY--; DX--;} if(A=='8'){DX--;} if(A=='9'){DY++; DX--;}     
        if(A=='4'){DY--;}                             if(A=='6'){DY++;}
        if(A=='1'){DY--; DX++;} if(A=='2'){DX++;} if(A=='3'){DY++; DX++;}
        if(g->speed_e>0) {DX*=2; DY*=2;}
        b->x += DX; b->y += DY;
        current= *b;
        if(is_block(g,b,map,obs)){
            return -1;
        }
        if(strcmp(g->what[g->floor][current.x][current.y],"hidden")==0){
            Door* door = obs[current.x][current.y].door;
            if (door != NULL) {
            door->hidden = 0;
            strcpy(g->what[g->floor][current.x][current.y], "door");
            Map[g->floor][current.x][current.y] = DOOR;
            }
        }

        if(strcmp(g->what[g->floor][current.x][current.y],"hidden")==0||Map[g->floor][current.x][current.y]==DOOR||Map[g->floor][current.x][current.y]==LOCK){
            Room* r=(Room*)malloc(sizeof(Room));
            r=obs[current.x][current.y].door->room;
            for(int i=r->ver;i<=r->ver+r->length;i++){
                for(int j=r->hor;j<=r->hor+r->width;j++){
                    if(i >= 0 && i < Rows - bRow && j >= 0 && j < (Cols - bCol)/2){
                        Show[g->floor][i][j] = 'y';
                    }
                }
            }
            g->room=r;
        }
        if(Map[g->floor][current.x][current.y]==LOCK){
            if(obs[current.x][current.y].door->locked>5) return 0;
            if(obs[current.x][current.y].door->locked>1&&obs[current.x][current.y].door->locked<=5){
                if(obs[current.x][current.y].door->change==1){
                    if(time(NULL)-obs[current.x][current.y].door->last_seen>0) {
                        return 0;
                        Door* d=(Door*)malloc(sizeof(Door)); d=obs[current.x][current.y].door;
                        char * BB=(char*)malloc(4*sizeof(char)); BB=gen_pass();
                        for(int i=0;i<4;i++) d->password_1[i]=BB[i];
                        d->password_1[4]='\0';
                        d->last_seen=time(NULL);
                    }
                    map[current.x][current.y] = DOOR;
                    strcpy(g->what[g->floor][current.x][current.y], "door");
                }
                game_massage("                                          ");
                game_massage("enter the password");
                g->temp_massage=28+time(NULL);
                refresh();
                char ramz[4];
                strcpy(ramz,get_massage());
                //move(g->player.x,g->player.y);
                if(strcmp(ramz,obs[current.x][current.y].door->password_1)!=0){
                    obs[current.x][current.y].door->locked ++;
                    int color[3]={2,5,1}; int gg=0;
                    if(obs[current.x][current.y].door->locked==3){gg=0;}
                    if(obs[current.x][current.y].door->locked==4){gg=1;}
                    if(obs[current.x][current.y].door->locked==5){gg=2;}
                    attron(COLOR_PAIR(color[gg]));
                    mvprintw(4,4,"                                    ");
                    mvprintw(6,4,"                                    ");
                    mvprintw(4,4,"wrong password!");
                    //game_massage("                                     ");
                    attroff(COLOR_PAIR(color[gg]));
                    obs[current.x][current.y].door->locked++;
                    refresh();
                    sleep(1);
                    game_massage("                                     ");
                    refresh();
                    g->temp_massage=time(NULL);
                    return 0;
                }
                if(obs[current.x][current.y].door->makers_num==2){
                    game_massage("                                   ");
                    game_massage("enter the second password");
                    strcpy(ramz,get_massage());
                    if(strcmp(ramz,obs[current.x][current.y].door->password_2)!=0){
                        obs[current.x][current.y].door->locked ++;
                        int color[3]={2,5,1}; int gg=0;
                        if(obs[current.x][current.y].door->locked==3){gg=0;}
                        if(obs[current.x][current.y].door->locked==4){gg=1;}
                        if(obs[current.x][current.y].door->locked==5){gg=2;}
                        attron(COLOR_PAIR(color[gg]));
                        mvprintw(4,4,"                                       ");
                        mvprintw(6,4,"                                     ");
                        mvprintw(4,4,"wrong password!");
                        //game_massage("                                     ");
                        attroff(COLOR_PAIR(color[gg]));
                        refresh();
                        sleep(1);
                        game_massage("                                     ");
                        refresh();
                        obs[current.x][current.y].door->locked++;
                        g->temp_massage=28+time(NULL)%10000;
                        return 0;
                    }
                }
                obs[current.x][current.y].door->locked =1;
                strcpy(g->what[g->floor][current.x][current.y],"door");
                map[current.x][current.y]=DOOR;
                game_massage("                       ");
                game_massage("Door unlocked! ");
                g->temp_massage=28+time(NULL);

            }
            if(obs[current.x][current.y].door->locked == 1){
                map[current.x][current.y] = DOOR;
                obs[current.x][current.y].door->hidden = 0; // Reveal door
                strcpy(g->what[g->floor][current.x][current.y], "door");
            }
        }

        if(Map[g->floor][current.x][current.y]==KEY){
            char A[25];
            int l=0;
            if(MAP[g->floor][current.x][current.y].door->password_maker_1.x==current.x &&
            MAP[g->floor][current.x][current.y].door->password_maker_1.y==current.y) {l=1; strcpy(A,"the first pass is  ");}
            else {l=0; strcpy(A,"the second pass is ");}
            //strcpy(A,"the pass is  ");
            for(int i=19;i<23;i++){
                if(l==1)
                {
                    A[i]=MAP[g->floor][current.x][current.y].door->password_1[i-19];
                }
                else{
                    A[i]=MAP[g->floor][current.x][current.y].door->password_2[i-19];
                }
            }
            A[23]='\0';
            game_massage(A);
            refresh();
            g->temp_massage=time(NULL)-15;
            //g->temp_massage=time(NULL);
        }
        g->player=current;
        if(Map[g->floor][current.x][current.y]==CORRIDOR){
            g->room=NULL;
        }
        if(collect==0) return 0;
        if(strcmp(g->what[g->floor][current.x][current.y],"trap")==0){
            g->health --;
            g->score --;
            Map[g->floor][current.x][current.y]=TRAP;
        }
        if(Map[g->floor][current.x][current.y]==MASTER){
            g->master_key += 0.5*dice(0.9) + 0.5;
            strcpy(g->what[g->floor][current.x][current.y],"floor");
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==FOOD){
            g->food++; g->food %= 5;
            strcpy(g->what[g->floor][current.x][current.y],"floor");
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==HEALTH){
            g->enchant[0]++;
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==SPEED){
            g->enchant[1]++;
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==POWER){
            g->enchant[2]++;
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==DAGGER){
            g->gun[1]+=MAP[g->floor][current.x][current.y].gun->num;
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==WAND){
            g->gun[2]+=MAP[g->floor][current.x][current.y].gun->num;
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==NORMALARROW){
            g->gun[3]+=MAP[g->floor][current.x][current.y].gun->num;
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==SWORD){
            g->gun[4]+=MAP[g->floor][current.x][current.y].gun->num;
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==GOLD){
            game_massage("you earned one gold");
            g->temp_massage=time(NULL)-27;
            g->gold++;
            g->score += 2;
            Map[g->floor][current.x][current.y]=FLOOR;
        }
        if(Map[g->floor][current.x][current.y]==BGOLD){
            game_massage("you earned seven golds");
            g->temp_massage=time(NULL)-27;
            g->gold+=7;
            g->score += 10;
            Map[g->floor][current.x][current.y]=FLOOR;
        }

        

        for(int i=-5;i<=5;i++){
            for(int j=-5;j<=5;j++){
                if(abs(i)+abs(j)<=5){
                    if(b->x+i<=0||b->y+j<=0||b->x+i>Rows-bRow||b->y+j>=(Cols-bCol)/2) continue;
                    Show[g->floor][b->x+i][b->y+j]='y';
                }
            }
        }
        if(map[b->x][b->y]==STAIR) return 1;
        //if()

    return 0;
};

int attack(Game* g,int direction){
    if(g->gun[g->current_gun]<=0){
        game_massage("                                     ");
        game_massage("you should choose a gun");
        g->temp_massage=28+time(NULL);
        refresh();
        return 0;
    }
    if(g->current_gun==0||g->current_gun==4){
        for(int dx=-1;dx<=1;dx++){
            for(int dy=-1;dy<=1;dy++){
                if(MAP[g->floor][g->player.x+dx][g->player.y+dy].Monster!=NULL){
                    MAP[g->floor][g->player.x+dx][g->player.y+dy].Monster->life -= 5*((2*g->current_gun+5)/5)*(1+(g->damage_e>0));
                    if(g->current_gun ==0 ) g->gun[0] -= 5;
                    if(MAP[g->floor][g->player.x+dx][g->player.y+dy].Monster->life <= 0){
                        g->score ++;
                        MAP[g->floor][g->player.x+dx][g->player.y+dy].Monster->life=0;
                        MAP[g->floor][g->player.x+dx][g->player.y+dy].Monster->moves=0;
                        MAP[g->floor][g->player.x+dx][g->player.y+dy].Monster = NULL;
                        Map[g->floor][g->player.x+dx][g->player.y+dy]=FLOOR;
                        game_massage("                                     ");
                        game_massage("you killed a monster!(:");
                        g->score ++;
                        g->temp_massage=28+time(NULL);
                        refresh();
                    }
                }

            }
        }
        return 0;
    }
    if(direction==-1){
        direction= getch();
    }
    g->direction = direction;
    int dx=0,dy=0;
    if(g->direction-'6'>0) dx=-1;
    if(g->direction-'0'<4) dx=1;
    if((g->direction-'0')%3==0) dy = 1;
    if((g->direction-'0')%3==1) dy = -1;
    pos tir; tir.x=g->player.x; tir.y=g->player.y;
    int damage=0,q=0;
    int type[3]={DAGGER,WAND,NORMALARROW};
    if(g->current_gun==1) {damage =12; q=0;}
    if(g->current_gun==2) {damage =15; q=1;}
    if(g->current_gun==3) {damage =5; q=2;}
    int num=0;
    int move[3]={5,10,5};
    while(1){
        tir.x += dx; 
        tir.y+= dy;
        num++;
        if(num>move[g->current_gun-2]){
            Map[g->floor][tir.x-dx][tir.y-dy]=type[q];
            if(MAP[g->floor][tir.x-dx][tir.y-dy].gun==NULL) MAP[g->floor][tir.x-dx][tir.y-dy].gun=(Gun*)malloc(sizeof(Gun));
            MAP[g->floor][tir.x-dx][tir.y-dy].gun->name=type[q];
            MAP[g->floor][tir.x-dx][tir.y-dy].gun->num =1;
            break;
        }
        if(Map[g->floor][tir.x][tir.y]==FLOOR) continue;
        else if(MAP[g->floor][tir.x][tir.y].Monster != NULL){
            Monster * h =(Monster*)malloc(sizeof(Monster));
            h=MAP[g->floor][tir.x][tir.y].Monster;
            h->life -= damage*(1+(g->damage_e>0));
            g->gun[g->current_gun] --;
            
            if(g->current_gun==2){
                MAP[g->floor][tir.x][tir.y].Monster->moves=0;
            }
            if(h->life <= 0) {
                g->score ++;
                MAP[g->floor][tir.x][tir.y].Monster->life=0;
                MAP[g->floor][tir.x][tir.y].Monster->moves=0;
                MAP[g->floor][tir.x][tir.y].Monster = NULL;
                Map[g->floor][tir.x][tir.y]=FLOOR;
                game_massage("                                     ");
                game_massage("you killed a monster!(:");
                g->score ++;
                g->temp_massage=28+time(NULL);
                refresh();
            }
            break;
        }
        else {
            Map[g->floor][tir.x-dx][tir.y-dy]=type[q];
            if(MAP[g->floor][tir.x-dx][tir.y-dy].gun==NULL) MAP[g->floor][tir.x-dx][tir.y-dy].gun=(Gun*)malloc(sizeof(Gun));
            MAP[g->floor][tir.x-dx][tir.y-dy].gun->name=type[q];
            MAP[g->floor][tir.x-dx][tir.y-dy].gun->num =1;
            break;
        }
    }

}

void default_game(Game* g){
    g->level=1;
    g->points=0;
    g->gold=0;
    g->loged_in=1;
    g->floor=0;
    for(int i=0;i<5;i++){
        g->gun[i]=0;
    }
    g->health=50-4*(g->level);
    g->master_key=0;
    g->room=(Room*)malloc(sizeof(Room));
    g->gun[0]=10;
    g->direction=-1;
    g->current_gun=0;
    g->speed_e=0;
    g->damage_e=0;
    g->health_e=0;
    g->direction=-1;
    g->want_food=time(NULL);
    g->last_update=time(NULL);
    g->hunger = 0;
    g->health = 50-4*g->level;

}



int check_rank(Game* g){
    FILE* board=fopen("scoreboard.txt","w");
    char temp[1000];
    fgets(temp,1000,board); 
    int rank;
    char name[10][15];
    int score[10]; 
    int gold[10];  
    int games[10]; 
    long int t[10];
    for(int i=0;i<10;i++){
        fscanf(board,"%d",&rank);
        fscanf(board,"%s",name[i]);
        fscanf(board,"%d",score+i);
        fscanf(board,"%d",gold+i);
        fscanf(board,"%d",games+i);
        fscanf(board,"%ld",t+i);
    }
    //fseek(fptr,0,SEEK_SET);
    g->rank=1000;
    for(int i=0;i<10;i++){
        if(g->score>score[i]){
            strcpy(name[i],g->name);
            score[i]=g->score;
            gold[i]=g->gold;
            games[i]=g->number_of_games;
            t[i]=g->first_time;
            g->rank=i+1;
            for(int j=9;j>i+1;j--){
                strcpy(name[j],name[j-1]);
                score[j]=score[j-1];
                gold[j]=gold[j-1];
                games[j]=games[j-1];
                t[j]=t[j-1];
            }
        }
    }
    freopen("scoreboard.txt","w",board);

    return 0;
}

void status(Game* g){
    attron(COLOR_PAIR(3));
    mvprintw(2,bCol+40,"health:%d     hunger:%d",g->health,g->hunger);
    attroff(COLOR_PAIR(3));
}


void win_page(Game* g){
    clear();
    mvprintw(Rows/2-4,Cols/2-8,"your score:  %d",g->score);
    mvprintw(Rows/2-2,Cols/2-30,"__   __                    __        __                  _ ");
    mvprintw(Rows/2-1,Cols/2-30,"\\ \\ / /   ___    _   _     \\ \\      / /   ___    _ __   | |");
    mvprintw(Rows/2,Cols/2-30,  " \\ V /   / _ \\  | | | |     \\ \\ /\\ / /   / _ \\  | '_ \\  | |");
    mvprintw(Rows/2+1,Cols/2-30,"  | |   | (_) | | |_| |      \\ V  V /   | (_) | | | | | |_|");
    mvprintw(Rows/2+2,Cols/2-30,"  |_|    \\___/   \\__,_|       \\_/\\_/     \\___/  |_| |_| (_)");
    refresh();
    char A=getch();
    if(A=='\n'){
        check_rank(g);
        before_game(g);
    }
}

void loose_game(Game* g){
    clear();
    mvprintw(Rows/2-4,Cols/2-8,"your score:  %d",g->score);
    mvprintw(Rows/2,Cols/2-8,"You lose ):");
    refresh();
    char A=getch();
    if(A=='\n'){
        check_rank(g);
        before_game(g);
    }
}

int prevoius_game(Game *g){
    if(g==NULL) g=(Game*)malloc(sizeof(Game));
    if(g->loged_in==0){
        clear();
        mvprintw(Rows/2,Cols/2-10,"You are here as a guest!");
        refresh();
        sleep(2);
        return 0;
    }
    //fclose(fptr);
    //if(fptr!=NULL) free(fptr);
    fptr=fopen(g->name,"r");
    //fseek(fptr, 0, SEEK_SET);
    read_game(g);
    //read_arrays(g);
    //fclose(fptr);
    if(g->state==0){
        clear();
        mvprintw(Rows/2,Cols/2-10,"you don't have unfinished game!");
        refresh();
        sleep(2);
        return 0;
    }
    run_game(g);
    
   before_game(g);

}

void user_menu(){
    clear();
    for(int x=0;x<Rows;x++){
        mvprintw(x, 0, "|");
        mvprintw(x, Cols-1, "|");
    }
    for(int x=0;x<Cols;x++){
        mvprintw(0, x, "-");
        mvprintw(Rows - 1, x, "-");
    }
    refresh();
}

int good_password(char password[50]){
    if(strlen(password)<7) return 0;
    int has_num=0,has_capital=0,has_small=0;
    for(int i=0;i<strlen(password);i++){
        if(password[i]>='0'&&password[i]<='9') has_num++;
        if(password[i]>='A'&&password[i]<='Z') has_capital++;
        if(password[i]>='a'&&password[i]<='z') has_small++;
    }
    if(has_num==0||has_capital==0||has_small==0) return 1;
    //mvprintw(Rows/2+10,Cols/2+5,"HELLO");
    return 2;
}

int good_email(char email[50]){
    int dot=0,atsign=0;
    int first=0,middle=0,last=0;
    for(int i=0;i<strlen(email);i++){
        if((email[i]>='a'&&email[i]<='z')||(email[i]>='A'&&email[i]<='Z')){
            if(dot>0) last++;
            else if(atsign>0) middle++;
            else first++;
        }
        else if(email[i]=='@'){
            if(dot>0||first==0||atsign>0) return 0;
            atsign ++;
        }
        else if(email[i]=='.'){
            if(dot>0||middle==0||first==0) return 0;
            dot++;
        }
        else return 0;
    }
    if(dot==0||atsign==0||first==0||middle==0||last==0) return 0;
    return 1;
}

char* make_password(Game* g){
    char* A=(char*)malloc(10*sizeof(char));
    for(int i=0;i<10;i++){
        int a=i%3;
        if(a=0){
            A[i]='a'+gen_rand(0,25);
        }
        if(a==1){
            A[i]='0'+gen_rand(0,9);
        }
        if(a==2){
            A[i]='A'+gen_rand(0,25);
        }
    }
    return A;
}

int sign_in(Game *g){
    user_menu();
    mvprintw(1,1,"Sign in");
    mvprintw(Rows/2-5,Cols/2-20,"to play as a guest, tap ~ ,otherwise tap enter");
    mvprintw(Rows/2-2,Cols/2-20,"Enter your username");
    mvprintw(Rows/2,Cols/2-20,"Enter your password");
    char choose=getch();
    if(choose=='~'){
        g->loged_in=0;
        fptr=NULL;  fptr=fopen("guest","r");
        if(fptr!=NULL){
            remove("guest");
        }
        fptr=fopen("guest","w");
        fprintf(fptr,"guest\n");
        fprintf(fptr,"no password \n");
        while(1){
            if(before_game(g)==1) break;
        }
        return 1;
    }
    g->loged_in = 1;
    echo();
    mvscanw(Rows/2-2,Cols/2,"%s",g->name);
    refresh();
    fptr=NULL;
    fptr=fopen(g->name,"r");
    if(fptr==NULL){
        echo();
        mvprintw(Rows/2+4,Cols/2-15,"There is not an account with this name ):");
        refresh();
        sleep(1);
        mvprintw(Rows/2+4,Cols/2-15,"                               ");
        mvprintw(Rows/2-2,Cols/2,"                                  ");
        sleep(1);
        return 0;
    }
    else{
        int d; read_int(&d); 
        read_str(g->name);
        read_int(&d);
        mvscanw(Rows/2,Cols/2,"%s",g->pass);
        char real_pass[50]; read_str(real_pass);
        if(strcmp(g->pass,real_pass)==0) {
            while(1){if(before_game(g)==1) break;}
        }
        else{
            echo();
            mvprintw(Rows/2+4,Cols/2-15,"wrong password!");
            refresh();
            sleep(1);
            mvprintw(Rows/2+4,Cols/2-15,"                               ");
            mvprintw(Rows/2,Cols/2,"                                  ");
            sleep(1);
            return 0;
        }
    }
    fclose(fptr);
    return 1;
    refresh();
    getch();
}

void game_massage(char massage[]){
    //echo();
    attron(COLOR_PAIR(7));
    mvprintw(4,4,"%s",massage);
    attroff(COLOR_PAIR(7));
    refresh();
}

int new_game(Game *g){
    clear();
    //fptr=fopen(g->name,"w");
    //write_str(g->name);
    //write_str(g->pass);
    //write_str(g->email);
    g=(Game*)malloc(sizeof(Game));
    g->number_of_games++;
    default_game(g);
    pos* a=(pos*)malloc(sizeof(pos)); a->x=5; a->y=2;
    g->player=*a;
    for(int i=0;i<4;i++){
        g->start[i]=*a;
        g->floor=i;
        pos* b=(pos*)malloc(sizeof(pos));
        b=NULL;
        while(1){
            b=make_map(a,g,Map[i],Show[i],Color[i],MAP[i],i+1);
            if(b!=NULL) break;
            else {
                for(int j=0;j<Rows-bRow;j++){
                    for(int k=0;k<(Cols-bCol)/2;k++){
                        Map[i][j][k]=' ';
                        Show[i][j][k]='n';
                        MAP[i][j][k].door=NULL;
                        for(int t=0;t<10;t++){
                            g->what[i][j][k][t]='\0';
                        }
                    }
                }
            }
        }
        a=b;
        g->end[i]=*a;
    }
    //print_map(Map[0],Show[0],Color[0]);
    g->floor=0;
    run_game(g);
    refresh();

    return 1;
}

char* get_massage(){
    char* mas=(char*)malloc(5*sizeof(char));
    mvscanw(6,4,"%s",mas);
    return mas;
}

int before_game(Game *g){
    user_menu();
    pos cursor; cursor.x=Rows/2; cursor.y=Cols/2;

    while(1){
        clear();
        mvprintw(1, 1, "Before Gameplay");
        mvprintw(Rows/2-4,Cols/2-18,"Start a new game");
        mvprintw(Rows/2-2,Cols/2-20,"Continue last game");
        mvprintw(Rows/2,Cols/2-14,"Leader board");
        mvprintw(Rows/2+2,Cols/2-10,"Settings");
        mvprintw(Rows/2+4,Cols/2-14,"Profile menu");
        //noecho();
        move(cursor.x,cursor.y);
        refresh();
        keypad(stdscr, TRUE);
        noecho();
        int command=getch();

        if(command==KEY_UP) {
            if(cursor.x!=Rows/2-4) {
                echo();
                mvprintw(cursor.x,cursor.y,"   ");
                cursor.x -= 2;
                move(cursor.x,cursor.y);
                refresh();
            }
            else {
                echo();
                mvprintw(cursor.x,cursor.y,"   ");
                cursor.x += 8;
                move(cursor.x,cursor.y);
                refresh();
            }
        }
        if(command==KEY_DOWN) {
            if(cursor.x!=Rows/2+4) {
                echo();
                mvprintw(cursor.x,cursor.y,"   ");
                cursor.x += 2;
                move(cursor.x,cursor.y);
                refresh();
            }
            else {
                echo();
                mvprintw(cursor.x,cursor.y,"    ");
                cursor.x -= 8;
                move(cursor.x,cursor.y);
                refresh();
            }
        }
        if(command=='\n'){
            if(cursor.x==Rows/2-4){
                while(new_game(g)!=1){}
                //break;
            }
            if(cursor.x==Rows/2-2){
                while(prevoius_game(g)!=1){}
                //break;
            }
            if(cursor.x==Rows/2){
                while(leader_board(g)!=1){}
                //break;
            }
            if(cursor.x==Rows/2+2){
                while(setting(g)!=1){}
                //break;
            }
            if(cursor.x==Rows/2+4){
                while(profile(g)!=1){}
                //break;
            }
        }
    }
    refresh();
    getch();
}

int get_new_user(Game *g){
    echo();
    user_menu();
    mvprintw(1, 1, "Create acount");
    mvprintw(Rows/2-2,Cols/2-20,"Enter your username");
    mvprintw(Rows/2,Cols/2-20,"Enter your password");
    mvprintw(Rows/2+2,Cols/2-20,"Enter your email");
    mvscanw(Rows/2-2,Cols/2,"%s",g->name);

    fptr=NULL;
    fptr=fopen(g->name,"r");
    g->first_time=time(NULL);
    g->number_of_games=0;
    if(fptr!=NULL){
        echo();
        mvprintw(Rows/2+4,Cols/2-15,"username already exists!");
        sleep(1);
        refresh();
        mvprintw(Rows/2+4,Cols/2-15,"                               ");
        mvprintw(Rows/2-2,Cols/2,"                                  ");
        sleep(1);
        refresh();
        return 0;
    }




    else {
        g->number_of_games =0;  g->level=1;
        g->points=0;    g->gold=0;
        //fclose(fptr);
        fptr=fopen(g->name,"w");
        write_str(g->name);
        write_int(1); 
        mvprintw(1, 1, "Creat abount");

        while(1){
            echo();
            char * s=(char*)malloc(11*sizeof(char));
            for(int i=0;i<10;i++){
                if(i%3==0){
                    s[i]='0'+gen_rand(0,9);
                }
                if(i%3==1){
                    s[i]='a'+gen_rand(0,25);
                }
                if(i%3==2){
                    s[i]='A'+gen_rand(0,25);
                }
            }
            mvprintw(Rows/2+10,Cols/2-10,"                                                   ");
            mvprintw(Rows/2+10,Cols/2-16,"our recommended random password: %s ",s);
            refresh();
            sleep(1);
            mvscanw(Rows/2,Cols/2,"%s",g->pass);
            
            refresh();
            int n=good_password(g->pass);
            if(n==2){
                fprintf(fptr,"%s\n",g->pass);
                break;
            }
            if(n==0) {mvprintw(Rows/2+5,Cols/2-15,"password is too short");}
            if(n==1) {mvprintw(Rows/2+6,Cols/2-15,"password must contain number,capital and small letter");}
            move(Rows/2,Cols/2);
            refresh();
            sleep(1);
            mvprintw(Rows/2,Cols/2,"                                    ");
            mvprintw(Rows/2+5,Cols/2-15,"                                                          ");
            mvprintw(Rows/2+6,Cols/2-15,"                                                           ");
            sleep(1);
            refresh();
        }
        move(Rows/2+2,Cols/2);
        while(1){
            echo();
            mvscanw(Rows/2+2,Cols/2,"%s",g->email);
            int accept_email = good_email(g->email);
            if(accept_email==1){fprintf(fptr,"%s\n",g->email);  write_int(0); fclose(fptr);   break;}
            mvprintw(Rows/2+8,Cols/2-15,"email form must be like xxx@yyy.zzz");
            move(Rows/2+2,Cols/2);
            refresh();
            sleep(1);
            mvprintw(Rows/2+2,Cols/2,"                                            ");
            mvprintw(Rows/2+8,Cols/2-15,"                                               ");
            sleep(1);

            refresh();
        }
    }
    
    //fclose(fptr);
    return 1;
}

int run_game(Game* g){
    int c=g->floor;
    for(int i=c;i<4;i++){
        pos* start=(pos*)malloc(sizeof(pos)); *start=g->start[i];
        pos* end=(pos*)malloc(sizeof(pos)); *end=g->end[i];
        g->player=g->start[i];
        g->floor=i;
        while(1){
            noecho();
            if((time(NULL)-g->temp_massage)>30) {
                game_massage("                              ");
                g->temp_massage=time(NULL);
            }
            //checking and updating health and hunger
            status(g);
            if(g->hunger<-5){
                if(time(NULL)-g->last_update>7){
                    g->health --;
                    g->last_update=time(NULL);
                }
            }
            if(g->hunger>0){
                if(time(NULL)-g->last_update>7){
                    g->health ++;
                    if(g->health_e >0) g->health++;
                    g->last_update=time(NULL);
                }
            }
            if(time(NULL)-g->want_food>7){
                g->hunger--;
                g->want_food=time(NULL);
            }
            //
            print_map(Map[i],Show[i],Color[i]);
            move(bRow+g->player.x,bCol+2*g->player.y);
            waddstr(stdscr,"\U0001F9CD");
            refresh();
            char A;
            A=getch();
            if(A==' '){
                attack(g,-1);
            }
            if(A=='a'){
                if(g->direction!=-1){
                    attack(g,g->direction);
                }
            }
            if(A=='q'){  //save , not finished
                g->state=1;
                save_game(g);
                //before_game(g);
                return 0;
            }
            int fin=move_player(A,g,*start,*end,Map[i],MAP[i],Show[i]);
            g->score = 2*g->gold + g->health;
            if(fin==1) {win_page(g); g->win++;}   //win
            if(g->health <=0 ) loose_game(g);  //loose
            move_monster(g);
        }
        char A=getch();
        if(A=='4') i-=2;
        if(i==-2) i=0;
        clear();
    }
    return 1;
}



int main(){

    M=0;
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    //start_color();
    init_pair(7,COLOR_GREEN, COLOR_BLACK);
    init_pair(1,COLOR_RED,COLOR_BLACK);
    init_pair(2,COLOR_YELLOW,COLOR_BLACK);
    init_pair(3,COLOR_GREEN,COLOR_BLACK);
    init_pair(4,COLOR_WHITE,COLOR_BLACK);
    init_pair(5,COLOR_ORANGE,COLOR_BLACK);
    init_pair(6,COLOR_GOLD,COLOR_BLACK);
    srand(time(NULL));

    Game* g=(Game*)malloc(sizeof(Game));
    g->state=0;
    default_game(g);

    //win_page(g);
    
    


    while(1){
        clear();
        mvprintw(Rows/2-8,Cols/2-10,"1:creat account");
        mvprintw(Rows/2-6,Cols/2-10,"2:sign in");
        char choice = getch();
        user_menu();
        if(choice=='1') while(1){
            if(get_new_user(g)==1) break;
        }
        else if(choice == '2') while(1){
            if(sign_in(g)==1) break;
        }
    }
    refresh();
    sleep(20);

    endwin();

    return 0;
}

