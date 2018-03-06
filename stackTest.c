#include "types.h"
#include "user.h"

int recursiveCall(int);


int main(int argc, char* argv[]){
  printf(1,"Starting recursive calls\n");
  recursiveCall(5000);
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
