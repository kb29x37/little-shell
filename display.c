#include "builtin.h"
#include "adds.h"

struct termios old, new;

void update_line_all(char**, char*, int*);
void update_line_c(char*, int, char);
void remove_c(char*, int, char);

void reset_input_mode (void) {
  //tcsetattr set the given attr to the terminal
  tcsetattr (STDIN_FILENO, TCSANOW, &old);
}

void set_input_mode (void) {

  // Make sure stdin is a terminal.
  if (!isatty (STDIN_FILENO)) {
      fprintf (stderr, "Not a terminal.\n");
      exit (EXIT_FAILURE);
    }

  // Save the terminal attributes so we can restore them later.
  tcgetattr (STDIN_FILENO, &old);
  atexit (reset_input_mode);

  // Set the funny terminal modes.
  tcgetattr (STDIN_FILENO, &new);

  /*new.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP
                          |INLCR|IGNCR|ICRNL|IXON);
  new.c_oflag &= ~OPOST;
  new.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
  new.c_cflag &= ~(CSIZE|PARENB);
  new.c_cflag |= CS8;*/

  new.c_lflag &= ~(ICANON|ECHO); //clear ICANON
  new.c_cflag = CS8 | CLOCAL | CREAD | ICRNL;
  new.c_iflag = IGNPAR;
  new.c_cc[VMIN] = 1;// minumum character readed = 1
  new.c_cc[VTIME] = 0;// timeout for reading

  // read with no timeout byte by byte (one char at a time at the time it's avaible)

  tcsetattr (STDIN_FILENO, TCSAFLUSH, &new);
}

//do all the processing in relation with keyboard entries and printing
//find a way to put the read in some if

char* get_cmd(char *line) {
  char c = '\0'; /* to store char readed */
  int counter, position;
  counter = position = 0;
  //counter is the number of char in the line
  //position is the position in the line of the cursor
  char *orig = line;

  while (c != '\n') {

    read(STDIN_FILENO, &c, 1);//change read ?

    if (c == 0x7f && counter > 0) { //backspace
      if(*line == '\t'){//TAB
        for(int i = 0; i < TAB; ++i){
          printf("\b");
          printf(" ");
          printf("\b");
        }
      } else {
      printf("\b");//update the screen
      printf(" ");
      printf("\b");
      }
      *(--line) = ' ';//update the command
      counter--;
      position--;

    } else if (c == '\033') { //escape character
      read(STDIN_FILENO, &c, 1);//skip [
      read(STDIN_FILENO, &c, 1);

      //update the line in the same time
      if(c == 'A'){//UP
        update_line_all(&orig, get_previous_cmd(), &counter);//to put in a function
        position = counter;
        line += position;

      } else if(c == 'B'){//DOWN
        update_line_all(&orig, get_next_cmd(), &counter);
        position = counter;
        line += position;

      } else if(c == 'C'){//RIGHT
        if(position < counter){
          printf("\033[1C");
          position++;
        }

      } else if(c == 'D'){//LEFT
        if(position > 0 ){
          printf("\033[1D");
          position--;
        }
      }
      //return NULL;

    } else if (c == '\r' || c == '\n'){
      c = '\n';
      //printf("\n");
    } else if (c != 0x7f){//might check for non supported characters ?
      //update_line_c(orig, position, c);
      *line++ = c;
      printf("%c", c);
      counter++;
      position++;
    }
    fflush(stdout);
  }

  *line = '\0';
  //printf("line : %s\n", orig);
  return orig;
}

//print the *line instead of the previous command seen on the screen
void update_line_all(char **orig, char *line, int *counter){
  for(int i = 0; i < *counter; ++i){//erasing the previous line
    printf("\b");
    printf(" ");
    printf("\b");
  }
  char *orig_d = *orig;
  *counter = strlen(line);
  while((*orig_d++ = *line++) != '\0')//updating the line
    ;
  printf("%s", *orig);//printing the new line
}

//add one char to the position given
void update_line_c(char *orig, int position, char c){
  int length = strlen(orig);
  char tmp[length];
  printf("orig : %s and %c\n", orig, c);
  strcpy(tmp, orig);//check if same reference
  printf("length %d and %s\n", length, orig);
  orig[position] = c;
  for(int i = position; i <= length; ++i){
    orig[i+1] = tmp[i];
  }
  printf("orig in update : %s, %d\n", orig, position);
}

//remove the char at position given
void remove_c(char* orig, int position, char c){
  //TODO
}
