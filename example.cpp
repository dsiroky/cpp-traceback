#include "traceback.h"

void g()
{
  print_traceback();
}

void f()
{
  g();
}

int main(int, char**)
{
  g();
  return 0;
}
