#include <stdio.h>

#define LINE  "______________________________" 
#define TITLE "C Programming in easy steps"
#define BOLLOCKS "Mike McGrath"

#ifdef __WIN32
#define SYSTEM "Windows"
#endif

#ifdef linux
#define SYSTEM "Linux"
#endif

#ifdef __APPLE__
#define SYSTEM "OS X"
#endif


int main()
{
  printf( "\n \t %s \n \t %s \n" , LINE , TITLE ) ;
  printf( "\t by %s \n \t %s \n" , BOLLOCKS , LINE ) ;
  printf( "\t Operating System: %s \n\n" , SYSTEM ) ;

  return 0 ;
}
