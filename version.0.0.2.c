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
}Game;

FILE * fptr;    


void new_user_menu();
int good_password(char[]);
int get_password(Game *,FILE *);
int get_new_user(Game *);
int get_email(Game *,FILE *);
int good_email(char[]);
int sign_in(Game *);


int game_area(){
    return 1;
}
void new_user_menu(){
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
    return 2;
}
int get_password(Game *g,FILE* fptr){
    //echo();
    move(Rows/2,Cols/2);    mvscanw(Rows/2,Cols/2,"%s",g->pass);
    int accept_pass=good_password(g->pass);
    if(accept_pass==0){return 0;}
    else if(accept_pass==1){ return -1;}
    else{
        fprintf(fptr,"%s\n",g->pass);
        return 1;
    }
    //refresh();
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
int get_email(Game *g,FILE *fptr){
    //echo();
    //mvprintw(Rows/2+2,Cols/2-15,"Enter yout email");
    move(Rows/2+2,Cols/2); mvscanw(Rows/2+2,Cols/2,"%s",g->email);
    int accept_email = good_email(g->email);
    if(accept_email==0){
        return 0;
    }
    else{
        fprintf(fptr,"%s\n",g->email);
        return 1;
    }
    //refresh();

}
int get_new_user(Game *g){
    echo();
    new_user_menu();
    mvprintw(1, 1, "Create acount");
    mvprintw(Rows/2-2,Cols/2-20,"Enter your username");
    mvprintw(Rows/2,Cols/2-20,"Enter your password");
    mvprintw(Rows/2+2,Cols/2-20,"Enter your email");
    move(Rows/2-2,Cols/2);      mvscanw(Rows/2-2,Cols/2,"%s",g->name);
    
    fptr=NULL;
    fptr=fopen(g->name,"r");
    if(fptr!=NULL){
        echo();
        mvprintw(Rows/2+4,Cols/2-15,"username already exists!");
        refresh();
        sleep(1);
        mvprintw(Rows/2+4,Cols/2-15,"                               ");
        mvprintw(Rows/2-2,Cols/2,"                                  ");
        sleep(1);
        return 0;
    }

    else {
        fptr=fopen(g->name,"w");
        move(Rows/2,Cols/2);
        mvprintw(1, 1, "Creat abount");

        while(1){
            echo();
            int n=get_password(g,fptr);
            if(n==1) break;
            if(n==0) {mvprintw(Rows/2+5,Cols/2-15,"password is too short");}
            if(n==-1) {mvprintw(Rows/2+6,Cols/2-15,"password must contain number,capital and small letter");}
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
            if(get_email(g,fptr)==1) break;
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
    new_user_menu();
    mvprintw(1,1,"Sign in");
    mvprintw(Rows/2-2,Cols/2-20,"Enter your username");
    mvprintw(Rows/2,Cols/2-20,"Enter your password");
    move(Rows/2-2,Cols/2);      mvscanw(Rows/2-2,Cols/2,"%s",g->name);
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
        if(strcmp(g->pass,real_pass)==0) game_area();
        else{
            echo();
            mvprintw(Rows/2+4,Cols/2-15,"wrong password!");
            refresh();
            sleep(1);
            mvprintw(Rows/2+4,Cols/2-15,"                               ");
            mvprintw(Rows/2-2,Cols/2,"                                  ");
            sleep(1);
            return 0;
        }
    }
    return 1;
    refresh();
    getch();
}



int main(){
    initscr();
    srand(time(0));
    Game g;
    noecho();
    mvprintw(Rows/2-8,Cols/2-10,"1:creat account");
    mvprintw(Rows/2-6,Cols/2-10,"2:sign in");
    char choice = getch();
    if(choice=='1') while(1){
        if(get_new_user(&g)==1) break;
    }
    else if(choice == '2') while(1){
        if(sign_in(&g)==1) break;
    }
    refresh();
    endwin();

    return 0;
}