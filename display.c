#include "builtin.h"
#include "adds.h"

struct termios old, new;

void update_line_all(char*, char*, int*);
void update_line_c(char*, int, char);

void reset_input_mode (void) {
  //tcsetattr set the given attr to the terminal
  tcsetattr (STDIN_FILENO, TCSANOW, &old);
}

void set_input_mode (void) {

  /* Make sure stdin is a terminal. */
  if (!isatty (STDIN_FILENO)) {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
    }

  /* Save the terminal attributes so we can restore them later. */
  tcgetattr (STDIN_FILENO, &old);
  atexit (reset_input_mode);

  /* Set the funny terminal modes. */
  tcgetattr (STDIN_FILENO, &new);

  /*new.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                          |INLCR|IGNCR|ICRNL|IXON);
  new.c_oflag &= ~OPOST;
  new.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  new.c_cflag &= ~(CSIZE|PARENB);
  new.c_cflag |= CS8;*/

  new.c_lflag &= ~(ICANON|ECHO); /* clear ICANON */
  new.c_cflag = CS8 | CLOCAL | CREAD | ICRNL;
  new.c_iflag = IGNPAR;
  new.c_cc[VMIN] = 1;/* minumum character readed = 1 */
  new.c_cc[VTIME] = 0;/* timeout for reading */

  /* read with no timeout byte by byte (one char at a time at the time it's avaible) */

  tcsetattr (STDIN_FILENO, TCSAFLUSH, &new);
}

//do all the processing in relation with keyboard entries and printing
//find a way to put the read in some if

char* get_cmd(char *line) {
  char c = '\0'; /* to store char readed */
  int counter = 0;
  char *orig = line;

  while (c != '\n') {

    read(STDIN_FILENO, &c, 1);//change read ?

    if (c == 0x7f && counter > 0) { //backspace
      *(--line) = ' ';//update the command
      printf("\b");//update the screen
      printf(" ");
      printf("\b");
      counter--;
    } else if (c == '\033') { //escape character
      read(STDIN_FILENO, &c, 1);//skip [

      read(STDIN_FILENO, &c, 1);

      //update the line in the same time
      if(c == 'A'){//UP
        update_line_all(orig, get_previous_cmd(), &counter);
      } else if(c == 'B'){//DOWN
        update_line_all(orig, get_next_cmd(), &counter);
      } else if(c == 'C'){//RIGHT
        printf("\033[1C");
      } else if(c == 'D'){//LEFT
        printf("\033[1D");
      }
      //return NULL;

    } else if (c == '\r'){
      c = '\n';
      printf("\n");
    } else if (c != 0x7f){//might check for non supported characters ?
      *line++ = c;
      printf("%c", c);
      counter++;
    }
    fflush(stdout);
  }
  *line = '\0';
  //printf("orig : %s\n", line);
  return orig;
}

void update_line_all(char *orig, char *line, int *counter){
  orig = line;
  for(int i = 0; i < *counter; ++i){//erasing the previous line
    printf("\b");
    printf(" ");
    printf("\b");
  }
  printf("%s", orig);//printing the new line
  *counter = strlen(line);
}

void update_line_c(char *orig, int position, char c){
  char *tmp = orig;
  int length = strlen(orig);

  orig[position] = c;
  for(int i = position; i <= length; ++i){
    orig[i+1] = tmp[i];
  }
}
