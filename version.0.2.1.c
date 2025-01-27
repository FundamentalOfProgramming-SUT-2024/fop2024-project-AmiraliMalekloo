#include<stdio.h>
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<math.h>
#include <wchar.h>
#include <locale.h>


#define Rows 67
#define Cols 265
#define bRow 10
#define bCol 50

#define COLOR_ORANGE 80
#define COLOR_GOLD 90

#define WALL 0x1F7EB
#define WALL2 0x1F7EA
#define FLOOR 0x26AA
#define STAIR 0x1F3DB
#define DOOR 0x1F6AA
#define LOCK 0x1F512
#define KEY 0x1F511
#define TRAP 0x1F3FA
#define CORRIDOR 0x2B1C



int adad=0;

typedef struct{
    int x; // ---> points one special x
    int y; // ^ points to one special y
    //char color[10];
}pos;
typedef struct{
    pos* position;
}Trap;
typedef struct{
    char name; // D:Deamon F:fire breathing G:Giant S:Snake U:Undeed
    int life;   // if 0 : it is dead
    pos position;
}Monster;
typedef struct Door{
    pos position;
    int hidden; // 0:not hidden  1:hidden
    int locked; //0: not locked at all 1:unlocked 2:locked  3:first_wrong   4:second_wrong  5:third_wrong
    char color[20];
    int reverse;
    int makers_num;
    pos password_maker_1;
    char password_1[12];
    pos password_maker_2;
    char password_2[12];
    int change;

    //Room* room;
    // Options
} Door;


typedef struct Room{
    int ver;
    int hor;
    int length; //amoodi :  an interval of x's
    int width; //ofoghi  :  an interval of y's
    Door* door[4];
    int num_doors;
    int theme;  // 1:Regular    2:Treasure  3:Enchant
    int Trap_num;
    Trap* traps;
    int Enemy_num;
    Monster* monsters;
    pos* key;
}Room;

typedef struct{
    pos player;
    char name[50];
    char pass[50];
    char email[50];
    int number_of_games;
    int level;
    int grade; //marhale
    int colour;
    int points;
    int gold;
    int rank;
    bool loged_in;
    pos start[4];
    pos end[4];
    int floor;
    char what[4][Rows-bRow][(Cols-bCol)/2][10];
    int gun[5]; // 0:Mace 1:Dagger 2:wond 3:normal arrow 4:Sword

}Game;

typedef struct{
    pos positioin;
    Room* room;
}Wall;

typedef struct{
    Door * door;
    Wall * wall;
    Trap * Trap;
    Monster * Monster;

}object;



FILE * fptr;
FILE * topten;
int Map[4][Rows-bRow][(Cols-bCol)/2];
char Show[4][Rows-bRow][(Cols-bCol)/2];
char Color[4][Rows-bRow][(Cols-bCol)/2][10];
//1:Red , 2:yellow , 3:green , 4:orange
object MAP[4][Rows-bRow][(Cols-bCol)/2];
char what[4][Rows-bRow][(Cols-bCol)/2][10];

// prototypes : 
void user_menu();
int good_password(char[]);
int get_new_user(Game *);
int good_email(char[]);
int sign_in(Game *);
int before_game(Game*);
void default_game(Game *);
void game_massage(char[]);
void add_path(Game* g,pos start,pos current,pos** parent,int map[Rows-bRow][(Cols-bCol)/2]);
void initialize(int i_size,int j_size,int map[Rows-bRow][(Cols-bCol)/2],char*** mark, pos ***parent);
int find_path(int map[Rows-bRow][(Cols-bCol)/2],pos* start,pos* end,int x,int y,char*** mark,pos *** parent);
pos* make_map(pos* begin,Game * g,int map[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10],object obs[Rows-bRow][(Cols-bCol)/2],int floor);
void print_map(int map[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10]);
void add_room(Game* g,Room* R,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10]);
Room* gen_room(Game* g,int ver_0,int hor_0,int down,int theme,int floor,int has_lock,int key);
int move_player(Game* g,pos start,pos end,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2]);
int is_block(Game* g,pos * current,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2]);
void run_game(Game* g);
char* get_massage();

void default_game(Game *g){
    g->level = 1;
    g->grade = 1;
}

int prevoius_game(Game *g){return 1;}
int leader_board(Game *g){return 1;}
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

int new_game(Game *g){
    echo();
    //clear();
    //make_map(g,map_1,show_1);

    refresh();
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

int lockdoor(Game* g,int map[Rows-bRow][(Cols-bCol)/2],Door* d,Room* R,int floor,int theme){
    if(dice(0.4)==1) d->reverse=1;
    else d->reverse=0;
    d->locked=2;
    strcpy(g->what[g->floor][d->position.x][d->position.y],"locked");
    map[d->position.x][d->position.y]=LOCK;
    double m[3]={0.25,0.25,0.5};
    int n=pro_dice(m,3);
    int X[4]={R->ver+1,R->ver+1,R->ver+R->length-1,R->ver+R->length-1};
    int Y[4]={R->hor+1,R->hor+R->width-1,R->hor+1,R->hor+R->width-1};
    int i=gen_rand(0,3);
    d->password_maker_1.x=X[i]; d->password_maker_1.y=Y[i];
    strcpy(g->what[g->floor][X[i]][Y[i]],"maker");
    d->makers_num=1;
    for(int i=0;i<12;i++){
        d->password_1[i]='a'+gen_rand(0,26);
    }
    if(n==0){
        d->change=1;
    }
    if(n==1){
        d->change=0;
        d->makers_num=2;
        d->password_maker_2.x=2*R->ver+R->length-X[i];
        d->password_maker_2.y=2*R->hor+R->width-Y[i];
        strcpy(g->what[g->floor][X[i]][Y[i]],"maker");
        for(int i=0;i<12;i++){
            d->password_2[i]='a'+gen_rand(0,26);
        }
    }
    if(n==2){
        d->change=0;
    }
}

Room* gen_room(Game* g,int ver_0,int hor_0,int down,int theme,int floor,int lock_hide,int Key){  // ver_o : ->  hor_0 : ^
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
        if(hard==1){
            if((lock_hide/(p/3))%3==1){
                hide[i]=1;
            }
            if((lock_hide/(p/3))%3==2){
                lock[i]=1;
            }
        }
    }

    Room* R=(Room*)malloc(sizeof(Room));
    R->ver=gen_rand(0,6)+ver_0; //amoodi
    R->hor=gen_rand(0,6)+hor_0; //ofoghi
    R->length=gen_rand(10,17); if(theme==3) R->length += 4;
    R->width=gen_rand(12,17); if(theme==3) R->width += 4;
    R->num_doors = 1;
    for(int i=0;i<4;i++){
        R->door[i]=(Door *)malloc(sizeof(Door));
        R->door[i]->hidden = 0;
        R->door[i]->position.x = 0;
        R->door[i]->position.y = 0;
        R->door[i]->makers_num=0;
        R->door[i]->locked=0;
    }
    // Left Door:
    R->door[0]->position.x=gen_rand(R->ver+1,R->ver+R->length-1);
    R->door[0]->position.y=R->hor;
    strcpy(g->what[g->floor][R->door[0]->position.x][R->door[0]->position.y],"door");
    R->num_doors ++;
    if(hide[0]==1){
        R->door[0]->hidden=1;
    }
    else R->door[0]->hidden=0;
    if(lock[0]==1){
        lockdoor(g,Map[floor-1],(R->door[0]),R,floor,theme);
    }
    // North or South 
    if(down==1){
        R->door[3]=NULL;
        R->door[1]->position.y=gen_rand(R->hor+1,R->hor+R->width-1);
        R->door[1]->position.x=R->ver;
        strcpy(g->what[g->floor][R->door[1]->position.x][R->door[1]->position.y],"door");
        if(hide[1]==1){
            R->door[1]->hidden=1;
        }
        else R->door[1]->hidden=0;
        if(lock[1]==1){
            lockdoor(g,Map[floor-1],(R->door[1]),R,floor,theme);
        }
    }
    else{
        R->door[1]=NULL;
        R->door[3]->position.x=R->ver+R->length;
        R->door[3]->position.y=gen_rand(R->hor+1,R->hor+R->width-1);
        strcpy(g->what[g->floor][R->door[3]->position.x][R->door[3]->position.y],"door");
        if(hide[1]==1){
        R->door[3]->hidden=1;
        }
        else R->door[3]->hidden=0;
        if(lock[1]==1){
            lockdoor(g,Map[floor-1],(R->door[3]),R,floor,theme);
        }
    }
    //  Right door
    R->door[2]->position.x=gen_rand(R->ver+1,R->ver+R->length-1);
    R->door[2]->position.y=R->hor+R->width;
    strcpy(g->what[g->floor][R->door[2]->position.x][R->door[2]->position.y],"door");
    R->num_doors ++;
    if(hide[2]==1){
        R->door[2]->hidden=1;
    }
    else R->door[2]->hidden=0;
    if(lock[2]==1){
        lockdoor(g,Map[floor-1],(R->door[2]),R,floor,theme);
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

    //monsters
    char mons[5]={'D','F','G','S','U'};
    R->Enemy_num=gen_rand((floor+g->level)/5,(3*floor+3*g->level)/5);
    if(theme==3) R->Enemy_num=4;
    R->monsters=(Monster *)malloc(R->Enemy_num*sizeof(Monster));
    for(int i=0;i<R->Enemy_num;i++){
        int place_x=gen_rand(R->ver+3,R->ver+R->length-3);
        int place_y=gen_rand(R->hor+3,R->hor+R->width-3);
        R->monsters[i].position.x=place_x;
        R->monsters[i].position.y=place_y;
        strcpy(g->what[g->floor][R->monsters[i].position.x][R->monsters[i].position.x],"monster");
        double probs[5];
        if(floor==1) {
            probs[0]=0.4; probs[1]=0.4; probs[2]=0.1; probs[3]=0.1; probs[4]=0;
        }
        if(floor==2) {
            probs[0]=0.2; probs[1]=0.3; probs[2]=0.3; probs[3]=0.2; probs[4]=0;
        }
        if(floor==3) {
            probs[0]=0.05; probs[1]=0.1; probs[2]=0.25; probs[3]=0.3; probs[4]=0.3;
        }
        if(floor==4) {
            probs[0]=0.0; probs[1]=0.1; probs[2]=0.2; probs[3]=0.3; probs[4]=0.4;
        }
        int par=pro_dice(probs,5);
        R->monsters[i].name=mons[par];

        //Master Key
        R->key=(pos *)malloc(sizeof(pos));
        if(Key==1){
            int place_x=gen_rand(R->ver+2,R->ver+R->length-2);
            int place_y=gen_rand(R->hor+2,R->hor+R->width-2);
            R->key->x=place_x;
            R->key->y=place_y;
        }
        else{
            R->key=NULL;
        }
    

}




    return R;
}

void add_room(Game* g,Room* R,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10]){
    for(int i=R->ver;i<=R->ver+R->length;i++){
        for(int j=R->hor;j<=R->hor+R->width;j++){
            map[i][j]=FLOOR;
            show[i][j]='y';
            if(i==R->ver||i==R->ver+R->length){
                map[i][j]=WALL;
                if(R->theme==2) map[i][j]=WALL2;
                strcpy(g->what[g->floor][i][j],"wall");
            }
            if(R->hor==j||j==R->hor+R->width){
                if(i!=R->ver) {
                    map[i][j]=WALL;
                    if(R->theme==2) map[i][j]=WALL2;
                    strcpy(g->what[g->floor][i][j],"wall");
                }
            }
            for(int k=0;k<4;k++){
                if(R->door[k]!=NULL){
                    int X=0,Y=0;
                    X=R->door[k]->position.x;
                    Y=R->door[k]->position.y;
                    map[X][Y]=DOOR;
                    obs[X][Y].door=R->door[k];
                    if(R->door[k]->hidden==1){
                        map[X][Y]=WALL;
                        if(R->theme==2) map[X][Y]=WALL2;
                        strcpy(g->what[g->floor][X][Y],"wall");
                    }
                    else if(R->door[k]->locked==1){
                        map[X][Y]=LOCK;
                        strcpy(color[X][Y],"red");
                        int maker_x,maker_y;
                        maker_x=R->door[k]->password_maker_1.x;
                        maker_y=R->door[k]->password_maker_1.y;
                        map[maker_x][maker_y]=KEY;
                        strcpy(g->what[g->floor][maker_x][maker_y],"KEY");
                        strcpy(color[maker_x][maker_y],"yellow");
                        if(R->door[k]->makers_num==2){
                            maker_x=R->door[k]->password_maker_2.x;
                            maker_y=R->door[k]->password_maker_2.y;
                            map[maker_x][maker_y]=KEY;
                            strcpy(g->what[g->floor][maker_x][maker_y],"KEY");
                            strcpy(color[maker_x][maker_y],"yellow");
                        }
                    }
                }
            }
            for(int k=0;k<R->Trap_num;k++){
                int X=0,Y=0;
                X=R->traps[k].position->x;
                Y=R->traps[k].position->y;
                map[X][Y]=TRAP;
                strcpy(g->what[g->floor][X][Y],"trap");
                obs[X][Y].Trap=&R->traps[k];
            }
            for(int k=0;k<R->Enemy_num;k++){
                int X=0,Y=0;
                X=R->monsters[k].position.x;
                Y=R->monsters[k].position.y;
                map[X][Y]=R->monsters[k].name;
                strcpy(g->what[g->floor][X][Y],"monster");
                obs[X][Y].Monster=&R->monsters[k];
            }
            if(R->key!=NULL){
                int X,Y=0;
                X=R->key->x;
                Y=R->key->y;
            }
        }
    }
}

void print_map(int map[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10]){
    for(int j=0;j<(Cols-bCol)/2;j++){
        for(int i=0;i<Rows-bRow;i++){
            if(show[i][j]=='n') continue;
            int z=4;
            //if(strcmp(color[i][j],"red")==0){z=1;}
            //if(strcmp(color[i][j],"green")==0){z=3;}
            //if(strcmp(color[i][j],"yellow")==0){z=2;}
            //attron(COLOR_PAIR(z));
            char c[2]; c[0]=map[i][j]; c[1]='\0';
            //mvprintw(i+bRow,j+bCol,c);
            move(i+bRow,2*j+bCol);
            int* A=(int*)malloc(sizeof(int)); *A=i+bRow; int* B=(int*)malloc(sizeof(int)); *B=2*j+bCol;
            if(map[i][j]== WALL)//{addstr("\U0001F7EB");}
                waddstr(stdscr,"\U0001F7EB");
            else if(map[i][j]==WALL2){waddstr(stdscr,"\U0001F7EA");}
            else if(map[i][j]==FLOOR){waddstr(stdscr,"\U000026AA");}
            else if(map[i][j]==STAIR){waddstr(stdscr,"\U000068AF");}
            else if(map[i][j]==DOOR){waddstr(stdscr,"\U0001F6AA");}
            else if(map[i][j]==LOCK){waddstr(stdscr,"\U0001F512");}
            else if(map[i][j]==KEY){waddstr(stdscr,"\U0001F511");}
            else if(map[i][j]==TRAP){waddstr(stdscr,"\U0001F3FA");}
            else if(map[i][j]==CORRIDOR){waddstr(stdscr,"\U00002B1C");}
            else addch(map[i][j]);

            //addstr("\U");
            //attroff(COLOR_PAIR(1));
            
            //if(show[i][j]==DOOR) mvprintw(i+bRow,j+bCol,"%c",map[i][j]);
            //else mvprintw(i+bRow,j+bCol,map[i][j]);
        }
    }
}

pos* make_map(pos* begin,Game * g,int map[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2],char color[Rows-bRow][(Cols-bCol)/2][10],object obs[Rows-bRow][(Cols-bCol)/2],int floor){
    //making borders
    for(int i=0;i<Rows-bRow;i++){
        for(int j=0;j<(Cols-bCol)/2;j++){
            map[i][j]=' ';
            if(i==0||j==0) map[i][j]='X';
            if(i==Rows-bRow-1||j==(Cols-bCol)/2-1) map[i][j]='X';
        }
    }
    Room* R[6];

    int x[4]={gen_rand(1,2700),gen_rand(1,2700),gen_rand(1,2700),gen_rand(1,2700)};
    int A=gen_rand(0,666)%6;
    R[0]=gen_room(g,2,5,0,1,4,0,A==0);    R[1]=gen_room(g,2,34,0,1,4,10*(x[0]%3),A==2);    R[2]=gen_room(g,2,70,0,1,3,3*(x[2]%9),A==4);
    R[5]=gen_room(g,26,5,1,1,1,0,A==1);    R[4]=gen_room(g,26,34,1,1,2,10*(x[1]%3),A==3);    R[3]=gen_room(g,26,70,1,1,3,10*(x[3]%3),A==5);

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
    int n = gen_rand(1, 5), m = gen_rand(2, 7);
    m+= (floor%2==1)*25;
    map[m][n] = '<';
    pos* ending=(pos*)malloc(sizeof(pos));
    ending->x=m; ending->y=n;
    map[begin->x][begin->y]='A';



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


    map[begin->x][begin->y]='A';
    strcpy(g->what[g->floor][begin->x][begin->y],"stair");
    map[ending->x][ending->y]='<';
    strcpy(g->what[g->floor][ending->x][ending->y],"stair");
    //pos* start; pos* end; start=R_1->door_3;  end=R_2->door_2;
    //make_path(g,map,start,end);

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
        if (nx <= 0 || nx >= Rows - bRow || ny <= 0 || ny >= Cols - bCol) {
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
        if ((*mark)[nx][ny] == 'n' && map[nx][ny] != WALL && map[nx][ny] != WALL && map[nx][ny] != FLOOR ) {
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

/*int default_game(Game* g){
    g->gold=0;
    g->level=0;
    g->grade=0;
    g->points=0;
    return 0;
}*/

int is_block(Game* g,pos * current,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2]){
    if(strcmp(g->what[g->floor][current->x][current->y],"wall")==0) return 1;
    if(strcmp(g->what[g->floor][current->x][current->y],"pillar")==0) return 1;
    if(map[current->x][current->y]==' ') return 1;
    return 0;

};
int move_player(Game* g,pos start,pos end,int map[Rows-bRow][(Cols-bCol)/2],object obs[Rows-bRow][(Cols-bCol)/2],char show[Rows-bRow][(Cols-bCol)/2]){
    while(1){
        pos current;
        current=g->player;
        pos* b=(pos*)malloc(sizeof(pos)); *b=current;
        int A=getch();
        if(A=='7'){b->x--; b->y++;} if(A=='8'){b->y++;} if(A=='9'){b->x++; b->y++;}     
        if(A=='4'){b->x--;}                             if(A=='6'){b->x++;}
        
        if(A=='1'){b->x--; b->y--;} if(A=='2'){b->y--;} if(A=='3'){b->x++; b->y--;}
        
        current= *b;
        if(map[b->x][b->y]==STAIR) return 1;
        if(is_block(g,b,map,obs)){
            continue;
        }
        if(strcmp(g->what[g->floor][current.x][current.y],"door")==0){
            if(obs[current.x][current.y].door->locked>5) continue;
            if(obs[current.x][current.y].door->locked>1&&obs[current.x][current.y].door->locked<5){
                game_massage("enter the password");
                char ramz[15];
                strcpy(ramz,get_massage());
                if(strcmp(ramz,obs[current.x][current.y].door->password_1)!=0){
                    mvprintw(4,4,"                    ");
                    mvprintw(6,4,"                    ");
                    obs[current.x][current.y].door->locked ++;
                    continue;

                }
                if(obs[current.x][current.y].door->makers_num==2){
                    strcpy(ramz,get_massage());
                    if(strcmp(ramz,obs[current.x][current.y].door->password_2)!=0){
                        game_massage("Wrong password!");
                        mvprintw(4,4,"                    ");
                        mvprintw(6,4,"                    ");
                        obs[current.x][current.y].door->locked ++;
                        continue;
                    }
                }
                obs[current.x][current.y].door->locked =1;
            }  
        }
        g->player=current;

        for(int i=-5;i<=5;i++){
            for(int j=-5;j<=5;j++){
                if(abs(i)+abs(j)<=5){
                    show[b->x+i][b->y+j]='y';
                }
            }
        }
        //if()
    }

    return 0;
};

void run_game(Game* g){
    for(int i=0;i<4;i++){
        pos* start=(pos*)malloc(sizeof(pos)); *start=g->start[i];
        pos* end=(pos*)malloc(sizeof(pos)); *end=g->end[i];
        while(1){
            echo();
            if(move_player(g,*start,*end,Map[i],MAP[i],Show[i])==1) break;
            print_map(Map[i],Show[i],Color[i]);
            move(g->player.x,g->player.y);
            waddstr(stdscr,"\U0001F9CD");
            refresh();
            sleep(20);
        }
    }
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
        else if(email[i]==LOCK){
            if(dot>0||first==0||atsign>0) return 0;
            atsign ++;
        }
        else if(email[i]==FLOOR){
            if(dot>0||middle==0||first==0) return 0;
            dot++;
        }
        else return 0;
    }
    if(dot==0||atsign==0||first==0||middle==0||last==0) return 0;
    return 1;
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
        g->grade=1;     g->points=0;    g->gold=0;
        //fclose(fptr);
        free(fptr);
        fptr=fopen(g->name,"w");
        mvprintw(1, 1, "Creat abount");

        while(1){
            echo();
            mvscanw(Rows/2,Cols/2,"%s",g->pass);
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
            mvprintw(Rows/2,Cols/2,"                                 ");
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
            if(accept_email==1){fprintf(fptr,"%s\n",g->email); fclose(fptr);   break;}
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
    return 1;

    refresh();
    getch();
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
        mvscanw(Rows/2,Cols/2,"%s",g->pass);
        char real_pass[50]; fscanf(fptr,"%s",real_pass);
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
    return 1;
    refresh();
    getch();
}

void game_massage(char massage[]){
    echo();
    init_pair(1,COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    mvprintw(4,4,"%s",massage);
    refresh();
}

char* get_massage(){
    char* mas=(char*)malloc(15*sizeof(char));
    mvscanw(6,4,"%s",mas);
    return mas;
}

int main(){
    setlocale(LC_ALL, "");
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);
    //start_color();


    init_pair(1,COLOR_RED,COLOR_BLACK);
    init_pair(2,COLOR_YELLOW,COLOR_BLACK);
    init_pair(3,COLOR_GREEN,COLOR_BLACK);
    init_pair(4,COLOR_WHITE,COLOR_BLACK);
    init_pair(5,COLOR_ORANGE,COLOR_BLACK);
    init_pair(6,COLOR_GOLD,COLOR_BLACK);
    srand(time(NULL));
    Game* g=(Game*)malloc(sizeof(Game));
    g->level=3;
    //topten=fopen(topten,"r");
    noecho();

    pos* a=(pos*)malloc(sizeof(pos)); a->x=5; a->y=2;
    g->player=*a;
    for(int i=0;i<4;i++){
        g->start[i]=*a;
        g->floor=i;
        pos* b=(pos*)malloc(sizeof(pos));
            while(1){
                b=make_map(a,g,Map[i],Show[i],Color[i],MAP[i],i+1);
                if(b!=NULL) break;
            }
        a=b;
        g->end[i]=*a;
    }

    //print_map(Map[0],Show[0],Color[0]);
    mvprintw(60,6,"here");
    run_game(g);
    unsigned int c=0x1F535;
    mvprintw( 2, 2 , "\U0001F7E3");
    refresh();
    sleep(20);
    
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


