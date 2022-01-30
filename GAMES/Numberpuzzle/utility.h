#ifndef UTILITY_H
#define UTILITY_H


/*-------------------------
    makeStr
  ------------------------ */
void makeStr(char *t) {
  arduboy.setTextSize(2);

  char text[32];
  int r = 0;

  //  char a[] ="ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char a[] = "0123456789";

  r = random(strlen(a));

  strncpy(text, a + r, 1);
  text[1] = '\0';
  strcpy(t, text);
}

#endif
