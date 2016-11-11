#include <stdio.h>
#include <string.h>

int main()
{
  char str[] = "No Time Like the Present." ;
  char sub[] = "Like" ;

  if( strstr( str, sub ) == NULL )
  {
    printf( "\nSubstring \"Time\" Not Found\n" ) ;
  }
  else 
  { 
    printf( "\nSubstring \"Time\" Found at %p\n" , strstr( str, sub) ) ; 
    printf( "Element Index Number %ld\n\n" , strstr( str, sub) - str ) ;
  }

  printf( "%s Versus \"Time\": %d\n" , sub , strcmp( sub,"Time") ) ;
  printf( "%s Versus \"time\": %d\n" , sub , strcmp( sub, "time") ) ;
  printf( "%s Versus \"TIME\": %d\n" , sub , strcmp( sub, "TIME") ) ;

  return 0 ;
}



