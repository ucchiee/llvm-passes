#include <stdio.h>
int main(int argc, char **argv) {
  int a;
  if (argc == 1) {
    a += 10;
  } else {
    a += 20;
  }
  printf("%d\n", a);
  return 0;
}
