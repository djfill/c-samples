#include <stdio.h>

int main()
{
  int i, j, a, c;

  for( i = 2; i <= 100; i++ )
  {
    printf( "Is %d a prime number?\t" , i ) ;

	a = i - 1 ;
	c = 0 ;
    for( j = 2; j < a; j++ )
    {
		if ( i % j == 0 ){
			c = c + 1 ;
		}
    }
	if ( c > 0 ){
		printf( "No\n" ) ;
	}
	else{
		printf( "Yes\n" ) ;
	}
  }

  return 0;
}
