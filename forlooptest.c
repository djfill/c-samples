#include <stdio.h>

int main()
{
  int i, j, a;

  for( i = 2; i <= 11; i++ )
  {
    printf( "Outer loop iteration %d\n" , i ) ;

	a = i - 1 ;
    for( j = 2; j < a; j++ )
    {
      printf( "\tInner loop iteration %d\n" , j ) ;
    } 
  }

  return 0;
}
