#include "types.h"


int recursiveCall(int);


int main(int argc, char* argv[]){
  recursiveCall(100);

  return 0;
}

int recursiveCall(int n){
  if (n == 0){
    return 1;
  }
  else{
    recursiveCall(n-1);
  }
  return 0;

}
