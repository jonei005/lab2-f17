#include "types.h"
#include "user.h"

int recursiveCall(int);


int main(int argc, char* argv[]){
  printf(1, "Starting recursive calls\n");
  if (argc != 2) {
	  printf(1, "usage: stackTest <iterations>\n");
	  exit();
  }
  
  int numCalls = 0;
  
  int i;
  for (i = 0; argv[1][i] != '\0'; ++i) {
    numCalls = numCalls*10 + argv[1][i] - '0';
  }
  
  printf(1, "numCalls = %d\n", numCalls);
  
  recursiveCall(numCalls);
  exit();
  return 0;
}

int recursiveCall(int n){
  if (n == 0){
    return 1;
  }
  else{
    printf(1, "n = %d\n",n);
    recursiveCall(n-1);
  }
  return 0;

}
