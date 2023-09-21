#define __POSIX_C_SOURCE 199309L

#include <time.h>
#include <ncurses.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <wchar.h>
#include <locale.h>

extern int nanosleep(const struct timespec*, struct timespec*);

int wideBorder(WINDOW *win, int lines, int columns, char ls[], char rs[], char ts[], char bs[], char tl[], char tr[], char bl[], char br[]);
int splashColumn, prevSplashColumn, prevPrevSplashColumn;

time_t NANOSEC_DELAY = 20000000; // best when set to 20000000
char SLASH = '/';
char SPACE = ' ';

short COLOR_GREY1 = 9;
short COLOR_GREY2 = 10;
short COLOR_GREY3 = 11;
short COLOR_GREY4 = 12;

short RAINY_WINDOW_HEIGHT = 52;
short RAINY_WINDOW_LENGTH = 200;

unsigned short phase = 0;

struct timespec delay;
struct timespec *delayp;

static void finish();

int main(void){

  delayp = &delay;

  setlocale(LC_ALL, "");

  (delayp)->tv_nsec = NANOSEC_DELAY;

  initscr();
  signal(SIGINT, finish);
  leaveok(stdscr, FALSE);
  start_color();
  curs_set(0);

  // init sub window
  short rainyWindowStartY = (LINES - RAINY_WINDOW_HEIGHT) / 2;
  short rainyWindowStartX = (COLS - RAINY_WINDOW_LENGTH) / 2;
  WINDOW* rainyWindow = newwin(RAINY_WINDOW_HEIGHT, RAINY_WINDOW_LENGTH, rainyWindowStartY, rainyWindowStartX);

  // trans colours
  init_pair(1, COLOR_CYAN, COLOR_BLACK);
  init_pair(2, COLOR_MAGENTA, COLOR_BLACK);
  init_pair(3, COLOR_WHITE, COLOR_BLACK);

  // white-black gradient colours
  init_color(COLOR_GREY1, 800, 800, 800);
  init_color(COLOR_GREY2, 600, 600, 600);
  init_color(COLOR_GREY3, 400, 400, 400);
  init_color(COLOR_GREY4, 200, 200, 200);

  init_pair(4, COLOR_WHITE, COLOR_BLACK);
  init_pair(5, COLOR_GREY1, COLOR_BLACK);
  init_pair(6, COLOR_GREY2, COLOR_BLACK);
  init_pair(7, COLOR_GREY3, COLOR_BLACK);
  init_pair(8, COLOR_GREY4, COLOR_BLACK);

  int raindrops[RAINY_WINDOW_HEIGHT][2];
  int raindropsCopy[RAINY_WINDOW_HEIGHT][2];

  //init raindrops
  for (int i = 0; i < RAINY_WINDOW_HEIGHT; i++){
    raindrops[i][0] = rand() % (RAINY_WINDOW_HEIGHT + RAINY_WINDOW_LENGTH);
    raindrops[i][1] = i;
  }

  while (1){


    phase++;
    //print drops
    for (unsigned int i = 0; i < RAINY_WINDOW_HEIGHT; i++){
//    if((i-phase) % 4 == 0){continue;}
//    attrset(COLOR_PAIR((i-phase) % 4));
      
      //leave trails behind the drops
      for (char step = 0; step < 5; step++){
        wattrset(rainyWindow, COLOR_PAIR(4 + step));
        mvwaddch(rainyWindow, raindrops[i][1] - step, raindrops[i][0] + step, SLASH);
      }

      wattrset(rainyWindow, COLOR_PAIR(4)); // lest the box be drawn with COLOR_GREY4
      
      wideBorder(rainyWindow, RAINY_WINDOW_HEIGHT, RAINY_WINDOW_LENGTH, "║", "║", "═", "═", "╔", "╗", "╚", "╝"); // draw this AFTER the drops lest the drops draw over the borders
    }

    wrefresh(rainyWindow);

    //clear screen
    wclear(rainyWindow);

    //this takes care of nyx's splash idea
    /* BEGIN NYX SPLASH */
    prevPrevSplashColumn = prevSplashColumn;
    prevSplashColumn = splashColumn;
    splashColumn = raindrops[RAINY_WINDOW_HEIGHT - 2][0];

    for (char i = -1; i < 3; i += 2){
      mvwaddch(rainyWindow, RAINY_WINDOW_HEIGHT - 2, splashColumn + i, '.');
    }

    for (char i = -1; i < 3; i += 2){
      mvwaddch(rainyWindow, RAINY_WINDOW_HEIGHT - 2, prevSplashColumn + i, '\'');
    }

    for (char i = -2; i < 5; i += 4){
      mvwaddch(rainyWindow, RAINY_WINDOW_HEIGHT - 3, prevPrevSplashColumn + i, '.');
    }

    for (unsigned int i = 0; i < RAINY_WINDOW_HEIGHT; i++){
      //copy drops
      raindropsCopy[i][0] = raindrops[i][0];
      raindropsCopy[i][1] = raindrops[i][1];
    }
    /* END NYX SPLASH */

    //make drops descend
    raindrops[0][0] = rand() % (RAINY_WINDOW_HEIGHT + RAINY_WINDOW_LENGTH);

    for (int i = 1; i < RAINY_WINDOW_HEIGHT; i++){
      raindrops[i][0] = --raindropsCopy[i-1][0];
    }
    
    nanosleep(&delay, NULL);
  }
}


static void finish(){
  endwin();
  exit(0);
}

int wideBorder(WINDOW *win, int lines, int columns, char ls[], char rs[], char ts[], char bs[], char tl[], char tr[], char bl[], char br[]){
  
  // write the corners
  mvwprintw(win, 0 , 0, tl);
  mvwprintw(win, lines - 1, columns - 1, br);
  mvwprintw(win, lines - 1, 0, bl);
  mvwprintw(win, 0, columns - 1, tr);

  // write the left- and right-side border
  for (int i = 0; i < lines; i++){
    if ((i != 0) && (i != (lines - 1))){
      mvwprintw(win, i, 0, ls);
      mvwprintw(win, i, columns - 1, rs);
    }
  }

  // write the top- and bottom-side border
  for (int i = 0; i < columns; i++){
    if ((i != 0) && (i != (columns - 1))){
      mvwprintw(win, 0, i, ts);
      mvwprintw(win, lines - 1, i, bs);
    }
  }
} 
