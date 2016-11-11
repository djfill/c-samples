#include <stdio.h>

int main( int argc, char *argv[] )
{
  FILE *file_ptr ;
  char text[1000] ;
  char *filename = argv[1] ;
  
  if( argc == 2 )
  {
	
    file_ptr = fopen( filename , "r" ) ;

    printf("\n") ;
	
    while( fgets( text, 1000, file_ptr ) )
      {
        printf( "%s", text ) ;
      } 
    printf("\n") ;
    return 0 ; 
  }
  else
  {
	return 0 ;
  }
}
