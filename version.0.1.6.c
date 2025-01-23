#include<stdio.h>
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<math.h>


#define Rows 50
#define Cols 199
#define bRow 5
#define bCol 60


int adad=0;

//typedef struct Room Room;

typedef struct{
    int x; // ---> points one special x
    int y; // ^ points to one special y
}pos;
typedef struct{
    pos* position;
}Trap;
typedef struct {
    pos position;
    int hidden; // 0:not hidden  1:hidden
    // Options
} Door;

typedef struct {
    int ver;
    int hor;
    int length; //amoodi :  an interval of x's
    int width; //ofoghi  :  an interval of y's
    Door* door[4];
    int num_doors;
    int theme;  // 1:Regular    2:Treasure  3:Enchant
    int Trap_num;
    Trap* traps;
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

}Game;

typedef struct{
    pos positioin;
}Wall;
typedef struct{
    Door * door;
    Wall * wall;
    Trap * Trap;
}object;



FILE * fptr;
FILE * topten;
char Map[4][Rows-bRow][Cols-bCol];
char Show[4][Rows-bRow][Cols-bCol];
object MAP[4][Rows-bRow][Cols-bCol];

// prototypes : 
void user_menu();
int good_password(char[]);
int get_new_user(Game *);
int good_email(char[]);
int sign_in(Game *);
int before_game(Game*);
void default_game(Game *);
void game_massage(char[]);
void add_path(pos start,pos current,pos** parent,char map[Rows-bRow][Cols-bCol]);
void initialize(int i_size,int j_size,char map[Rows-bRow][Cols-bCol],char*** mark, pos ***parent);
int find_path(char map[Rows-bRow][Cols-bCol],pos* start,pos* end,int x,int y,char** mark,pos ** parent);
int make_map(Game * g,char map[Rows-bRow][Cols-bCol],char show[Rows-bRow][Cols-bCol],object obs[Rows-bRow][Cols-bCol],int floor);
void print_map(char map[Rows-bRow][Cols-bCol],char show[Rows-bRow][Cols-bCol]);
void add_room(Room* R,char map[Rows-bRow][Cols-bCol],object obs[Rows-bRow][Cols-bCol],char show[Rows-bRow][Cols-bCol]);
Room* gen_room(int ver_0,int hor_0,int down,int hide_1,int hide_2,int hide_3,int Trap_num);

void default_game(Game *g){
    g->level = 1;
    g-> grade = 1;
}
int new_game(Game *g){
    echo();
    //clear();
    //make_map(g,map_1,show_1);

    refresh();
    return 1;
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

int max(int num1, int num2) {
    if(num1>num2) return num1;
    else return num2;
}

int min(int num1, int num2) {
    if(num1<num2) return num1;
    else return num2;
}

int gen_rand(int begin,int end){
    
    //sleep(1);
    int res=rand();
    res=(res+10)%(end-begin);
    res += begin;
    return res;
}

Room* gen_room(int ver_0,int hor_0,int down,int hide_1,int hide_2,int hide_3,int Trap_num){  // ver_o : ->  hor_0 : ^
    Room* R=(Room*)malloc(sizeof(Room));
    R->ver=gen_rand(0,6)+ver_0; //amoodi
    R->hor=gen_rand(0,6)+hor_0; //ofoghi
    R->length=gen_rand(5,11);
    R->width=gen_rand(5,11);
    R->num_doors = 1;
    for(int i=0;i<4;i++){
        R->door[i]=(Door *)malloc(sizeof(Door));
        R->door[i]->hidden = 0;
        R->door[i]->position.x = 0;
        R->door[i]->position.y = 0;
    }
    // Right Door:
    R->door[0]->position.x=gen_rand(R->ver+1,R->ver+R->length-1);
    R->door[0]->position.y=R->hor;
    R->num_doors ++;
    if(hide_1==1){
        R->door[0]->hidden=1;
    }
    else R->door[0]->hidden=0;
    // North or South 
    if(down==1){
        R->door[3]=NULL;
        R->door[1]->position.y=gen_rand(R->hor+1,R->hor+R->width-1);
        R->door[1]->position.x=R->ver;
        if(hide_2==1){
        R->door[1]->hidden=1;
        }
        else R->door[1]->hidden=0;
    }
    else{
        R->door[1]=NULL;
        R->door[3]->position.x=R->ver+R->length;
        R->door[3]->position.y=gen_rand(R->hor+1,R->hor+R->width-1);
        if(hide_2==1){
        R->door[3]->hidden=1;
        }
        else R->door[3]->hidden=0;
    }
    //  Left
    R->door[2]->position.x=gen_rand(R->ver+1,R->ver+R->length-1);
    R->door[2]->position.y=R->hor+R->width;
    R->num_doors ++;
    if(hide_3==1){
        R->door[2]->hidden=1;
    }
    else R->door[2]->hidden=0;

    //Traps
    R->traps = (Trap*)malloc(Trap_num*sizeof(Trap));
    R->Trap_num=Trap_num;
    for(int j=0;j<Trap_num;j++){
        R->traps[j].position = (pos*)malloc(sizeof(pos));
        int place_x=gen_rand(R->ver+1,R->ver+R->length-1);
        int place_y=gen_rand(R->hor+1,R->hor+R->width-1);
        R->traps[j].position->x=place_x;
        R->traps[j].position->y=place_y;
    }



    return R;
}

void add_room(Room* R,char map[Rows-bRow][Cols-bCol],object obs[Rows-bRow][Cols-bCol],char show[Rows-bRow][Cols-bCol]){
    for(int i=R->ver;i<=R->ver+R->length;i++){
        for(int j=R->hor;j<=R->hor+R->width;j++){
            map[i][j]='.';
            show[i][j]='y';
            if(i==R->ver||i==R->ver+R->length){
                map[i][j]='_';
            }
            if(R->hor==j||j==R->hor+R->width){
                if(i!=R->ver) map[i][j]='|';
            }
            for(int k=0;k<4;k++){
                if(R->door[k]!=NULL){
                    int X=0,Y=0;
                    X=R->door[k]->position.x;
                    Y=R->door[k]->position.y;
                    map[X][Y]='+';
                    obs[X][Y].door=R->door[k];
                    if(R->door[k]->hidden==1){
                        if(k%2==0) map[X][Y]='|';
                        else map[X][Y]='_';
                    }
                }
            }
            for(int k=0;k<R->Trap_num;k++){
                int X=0,Y=0;
                X=R->traps[k].position->x;
                Y=R->traps[k].position->y;
                map[X][Y]='^';
                obs[X][Y].Trap=&R->traps[k];
            }
        }
    }
}

void print_map(char map[Rows-bRow][Cols-bCol],char show[Rows-bRow][Cols-bCol]){
    for(int j=0;j<Cols-bCol;j++){
        for(int i=0;i<Rows-bRow;i++){
            if(show[i][j]=='n') continue;
            move(i+bRow,j+bCol);
            addch(map[i][j]);
            //if(show[i][j]=='+') mvprintw(i+bRow,j+bCol,"%c",map[i][j]);
            //else mvprintw(i+bRow,j+bCol,map[i][j]);
        }
    }
}

int make_map(Game * g,char map[Rows-bRow][Cols-bCol],char show[Rows-bRow][Cols-bCol],object obs[Rows-bRow][Cols-bCol],int floor){
    //making borders
    for(int i=0;i<Rows-bRow;i++){
        for(int j=0;j<Cols-bCol;j++){
            map[i][j]=' ';
            if(i==0||j==0) map[i][j]='X';
            if(i==Rows-bRow-1||j==Cols-bCol-1) map[i][j]='X';
        }
    }
    Room* R[6];
    R[0]=gen_room(2,10,0,0,0,0,1);    R[2]=gen_room(2,50,0,0,0,0,0);    R[4]=gen_room(2,100,0,0,0,0,1);
    R[1]=gen_room(22,10,1,1,1,0,0);    R[3]=gen_room(22,50,1,1,1,1,1);    R[5]=gen_room(22,100,11,0,0,0,0);
    for(int i=0;i<6;i++){
        add_room(R[i],map,obs,show);
    }
    for(int i=0;i<Cols-bCol;i++){
        for(int j=0;j<10;j++){
            int X=gen_rand(3,Rows-bRow-3);
            if(map[i][X]=='O') map[i][X]='X';
        }
    }


    // Add stairs
    int n = gen_rand(1, 5), m = gen_rand(2, 7);
    map[(floor % 2) ? m : m + 25][n] = '<';

    
    //adding path between doors
    char ** mark;
    pos ** parent;
    initialize(Rows-bRow,Cols-bCol,map,&mark,&parent);
    pos Start; Start.x=R[0]->door[3]->position.x; Start.y=R[0]->door[3]->position.y;
    // Start.x=10; Start.y=15;
    //map[Start.x][Start.y]='#';
    pos Finish; Finish.x=R[1]->door[1]->position.x; Finish.y=R[1]->door[1]->position.y;
    //map[Finish.x][Finish.y]='+';
    mark[Start.x][Start.y]='v';
    mark[Finish.x][Finish.y]='F';
    printf("%d\n",find_path(map,&Start,&Finish,Start.x,Start.y,mark,parent));
    add_path(Start,Finish,parent,map);
    // Do the same thing for the other doors 
    //



    //pos* start; pos* end; start=R_1->door_3;  end=R_2->door_2;
    //make_path(g,map,start,end);


}

int find_path(char map[Rows-bRow][Cols-bCol], pos* start, pos* end, int x, int y, char** mark, pos** parent) {
    // Base case: check if current position is the finish
    //map[x][y] = 'B';
    //printf("Visiting (%d, %d)\n", x, y);
    if (x == end->x && y == end->y) {
        return 1;
    }
    
    // Mark as visited
    mark[x][y] = 'v';

    // Fixed search directions (right, up, left, down)
    int deltax[4] = {0, -1, 0, 1};
    int deltay[4] = {1, 0, -1, 0};

    /*if(x<end->x){
        for(int i=0;i<4;i++){
            deltax[i]*=-1;
        }
    }
    if(y>end->y){
        for(int i=0;i<4;i++){
            deltay[i]*=-1;
        }
    }
    if(abs(x-end->x)>abs(y-end->y)){
        for(int i=0;i<4;i++){
            int temp=deltax[i];
            deltax[i]=deltay[i];
            deltay[i]=temp;
        }
    }*/

    for (int i = 0; i < 4; i++) {
        int nx = x + deltax[i];
        int ny = y + deltay[i];
        
        // Boundary check
        if (nx < 0 || nx >= Rows - bRow || ny < 0 || ny >= Cols - bCol) {
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
        if (mark[nx][ny] == 'n' && map[nx][ny] != '|' && map[nx][ny] != '_') {
            parent[nx][ny].x = x;
            parent[nx][ny].y = y;
            
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

void initialize(int i_size, int j_size, char map[Rows-bRow][Cols-bCol], char*** mark, pos*** parent) {
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

void add_path(pos start,pos current,pos** parent,char map[Rows-bRow][Cols-bCol]) {
    if (current.x < 0 || current.x >= Rows - bRow ||
        current.y < 0 || current.y >= Cols - bCol) {
        printf("no no no\n");
        return;
    }
    map[current.x][current.y]='#';
    //mvprintw(current.x,current.y,"#");
    refresh();
    if(current.x != start.x || current.y != start.y) {
        if (parent[current.x] == NULL || parent[current.x][current.y].x < 0 ||
            parent[current.x][current.y].x >= Rows - bRow ||
            parent[current.x][current.y].y < 0 ||
            parent[current.x][current.y].y >= Cols - bCol) {
            // Invalid parent position
            return;
        }
        printf("added (%d, %d)\n", current.x, current.y);
        add_path(start,parent[current.x][current.y], parent,map);
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



int main(){
    initscr();
    srand(time(NULL));
    Game g;
    //topten=fopen(topten,"r");
    noecho();

    make_map(&g,Map[0],Show[0],MAP[0],0);
    print_map(Map[0],Show[0]);
    refresh();
    sleep(20);
    while(1){
        clear();
        mvprintw(Rows/2-8,Cols/2-10,"1:creat account");
        mvprintw(Rows/2-6,Cols/2-10,"2:sign in");
        char choice = getch();
        if(choice=='1') while(1){
            if(get_new_user(&g)==1) break;
        }
        else if(choice == '2') while(1){
            if(sign_in(&g)==1) break;
        }
    }
    refresh();
    sleep(20);

    endwin();

    return 0;
}
