#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "check.h"

int x[5] = {1,2,3,4,5};

void allocate()
{
    int i;
    int *p;
    for(i =1 ; i<1000000 ; i++)
    {
      p = malloc(500 * sizeof(int));
      if(func(i)) { free (p);} // func(i) is defined in check.c
	// func takes in input i, and while (int j=2; j<=ceil(sqrt(i)); j++), tests if i%j==0 is true, returns 0. Only after for loop has finished running will it return 1. 
	// noting that 0=false, 1=true. this implies that the only time free is called is once j<=ceil(sqrt(i)) holds.
	// it can be assumed that for the initial sizes of i, this program operates as intended, but fails as the size of i grows, allocating larger and larger memory space.    
}
}

void allocate1()
{
  int i;
  int *p;
  for (i=1 ; i<10000 ; i++)
  {
    p = malloc(1000 * sizeof(int));
    if(i & 1)
      free (p);
  }
}

void allocate2()
{
  int i;
  int *p;
  for (i=1 ; i<300000 ; i++)
  {
    p = malloc(10000 * sizeof(int));
    free (p);
  }
}


int main(int argc, char const *argv[]) {
  int i;
  int *p;
  printf("Executing the code ......\n");
  allocate();

  for (i=0 ; i<10000 ; i++)
  {
    p = malloc(1000 * sizeof(int));
    free (p);
  }
  printf("Program execution successfull\n");
  return 0;
}
