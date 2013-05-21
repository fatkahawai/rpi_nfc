#include <stdio.h>
#include <stdlib.h>

#include <time.h>

// delay
//
void delay ( int nDelaySeconds )
{  
    time_t tStart, tNow;
    double fdElapsedSeconds;

    time(&tStart);  /* get current time; same as: timer = time(NULL)  */

    do{ 
        time( &tNow );
        fdElapsedSeconds = difftime( tNow, tStart );
        printf("elapsed secs = %.f", fdElapsedSeconds );
    } while ( fdElapsedSeconds < nDelaySeconds );
    
}

main( int argc, char *argv[] ){

  int nDelay = 0;

  if ( argc < 2 ){
  	printf("usage: %s [delay in seconds]\n",argv[0]);
    exit(EXIT_FAILURE);
  } 
  nDelay = atoi(argv[1]);
  printf("delaying %d secs\n", nDelay );
  delay(nDelay);
  printf("end\n");

}