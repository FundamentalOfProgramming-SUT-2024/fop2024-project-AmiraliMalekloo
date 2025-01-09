#include<stdio.h>
#include<ncurses.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>

#define Rows 50
#define Cols 199

typedef struct{
    int x;
    int y;
}pos;


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



FILE * fptr;
FILE * topten;
char map[Rows-10][Cols-10];
char show[Rows-10][Cols-10];


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
    default_game(g);
    refresh();
    return 1;
}
int prevoius_game(Game *g){return 1;}
int leader_board(Game *g){return 1;}
int setting(Game *g){
    echo();
    refresh();
    return 1;
}
int profile(Game *g){return 1;}

int before_game(Game *g){
    user_menu();
    pos cursor; cursor.x=Rows/2; cursor.y=Cols/2;
    mvprintw(1, 1, "Before Gameplay");
    mvprintw(Rows/2-4,Cols/2-18,"Start a new game");
    mvprintw(Rows/2-2,Cols/2-20,"Continue last game");
    mvprintw(Rows/2,Cols/2-14,"Leader board");
    mvprintw(Rows/2+2,Cols/2-10,"Settings");
    mvprintw(Rows/2+4,Cols/2-14,"Profile menu");
    while(1){
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
                break;
            }
            if(cursor.x==Rows/2-2){
                while(prevoius_game(g)!=1){}
                break;
            }
            if(cursor.x==Rows/2){
                while(leader_board(g)!=1){}
                break;
            }
            if(cursor.x==Rows/2+2){
                while(setting(g)!=1){}
                break;
            }
            if(cursor.x==Rows/2+4){
                while(profile(g)!=1){}
                break;
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
    mvprintw(3,3,"%s",massage);
    refresh();
}


int main(){
    initscr();
    srand(time(0));
    Game g;
    //topten=fopen(topten,"r");
    noecho();

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
    endwin();

    return 0;
}
