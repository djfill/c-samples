#include <stdio.h>


int main()
{
  int a , b ;

  printf( "Assigned: \n" ) ;
  printf( "\tVariable a = %d \n" , a = 7 ) ;
  printf( "\tVariable b = %d \n" , b = 2 ) ;

  printf( "Modulated & assigned: \n" ) ;
  printf( "\tVariable a%%=b (7%%=2) a= %d \n", a%=b ) ;

  return 0 ;
}
