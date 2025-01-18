#include<stdio.h>
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

#define Rows 50
#define Cols 199
#define bRow 5
#define bCol 12

typedef struct{
    int x; // ---> points one special x
    int y; // ^ points to one special y
}pos;

typedef struct {
    pos position;
    // Options
} Door;

typedef struct {
    int ver;
    int hor;
    int length; //amoodi :  an interval of x's
    int width; //ofoghi  :  an interval of y's
    Door* door[4];
    int num_doors;
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


int gen_rand(int begin,int end){
    
    //sleep(1);
    int res=rand();
    res=(res+10)%(end-begin);
    res += begin;
    return res;
}

Room* gen_room(int ver_0,int hor_0,int down){  // ver_o : ->  hor_0 : ^
    Room* R=(Room*)malloc(sizeof(Room));
    R->ver=gen_rand(0,6)+ver_0; //amoodi
    R->hor=gen_rand(0,6)+hor_0; //ofoghi
    R->length=gen_rand(5,11);
    R->width=gen_rand(5,11);
    R->num_doors = 1;
    for(int i=0;i<4;i++){
        R->door[i]=(Door *)malloc(sizeof(Door));
    }
    if(gen_rand(0,1001)%2==1){
        R->door[2]->position.x=gen_rand(R->ver+1,R->ver+R->length-1);
        R->door[2]->position.y=R->hor;
        R->num_doors ++;
    }
    else {R->door[0]=NULL;}
    if(down==1){
        R->door[3]=NULL;
        R->door[1]->position.y=gen_rand(R->hor+1,R->hor+R->width-1);
        R->door[1]->position.x=R->ver;
    }
    else{
        R->door[1]=NULL;
        R->door[3]->position.x=R->ver+R->length;
        R->door[3]->position.y=gen_rand(R->hor+1,R->hor+R->width-1);
    }
    if(gen_rand(0,1001)%2==1){
        R->door[2]->position.x=gen_rand(R->ver+1,R->ver+R->length-1);
        R->door[2]->position.y=R->hor+R->width;
        R->num_doors ++;
    }
    else { R->door[2]=NULL;}

    return R;
}

void add_room(Room* R,char map[Rows-bRow][Cols-bCol],object obs[Rows-bRow][Cols-bCol]){
    for(int i=R->ver;i<=R->ver+R->length;i++){
        for(int j=R->hor;j<=R->hor+R->width;j++){
            map[i][j]='.';
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
                }
            }
        }
    }
}

void print_map(char map[Rows-bRow][Cols-bCol],char show[Rows-bRow][Cols-bCol]){
    for(int j=0;j<Cols-bCol;j++){
        for(int i=0;i<Rows-bRow;i++){
            move(i+bRow,j+bCol);
            addch(map[i][j]);
            //if(show[i][j]=='+') mvprintw(i+bRow,j+bCol,"%c",map[i][j]);
            //else mvprintw(i+bRow,j+bCol,map[i][j]);
        }
    }
}

int make_map(Game * g,char map[Rows-bRow][Cols-bCol],char show[Rows-bRow][Cols-bCol],object obs[Rows-bRow][Cols-bCol]){
    for(int i=0;i<Rows-bRow;i++){
        for(int j=0;j<Cols-bCol;j++){
            map[i][j]=' ';
            if(i==0||j==0) map[i][j]='X';
            if(i==Rows-bRow-1||j==Cols-bCol-1) map[i][j]='X';
        }
    }
    Room* R[6];
    R[0]=gen_room(2,2,0);    R[2]=gen_room(2,37,0);    R[4]=gen_room(2,72,0);
    R[1]=gen_room(22,2,1);    R[3]=gen_room(22,37,1);    R[5]=gen_room(22,72,1);
    for(int i=0;i<6;i++){
        add_room(R[i],map,obs);
    }
    for(int i=0;i<Cols-bCol;i++){
        for(int j=0;j<10;j++){
            int X=gen_rand(3,Rows-bRow-3);
            if(map[i][X]=='O') map[i][X]='X';
        }
    }
    //pos* start; pos* end; start=R_1->door_3;  end=R_2->door_2;
    //make_path(g,map,start,end);


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

    make_map(&g,Map[0],Show[0],MAP[0]);
    print_map(Map[0],Show[0]);
    refresh();
    sleep(10);
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
