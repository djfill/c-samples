#include <stdio.h>

int main()
{
  FILE *file_ptr ;
  char text[50] = { "Hello you mofos, one character at a time." } ;
  int i ;

  file_ptr = fopen("chars.txt", "w");

  if( file_ptr != NULL )
  {
    printf( "File chars.txt created\n" ) ;
    
    for( i = 0 ; text[i] ; i++ )
    {
      fputc( text[i], file_ptr ) ;
    }
    fclose( file_ptr ) ;
    return 0 ;
  }
  else
  {
    printf( "Unable to create file\n" ) ;
  }
}
