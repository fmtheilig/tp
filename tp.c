
/*
   tp.c
   Version 1.00
   F. Michael Theilig   fmtheilig@gmail.com
*/

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#ifndef FALSE
   #define FALSE 0
   #define TRUE !FALSE
#endif
#define PROMPTLINES 22
#define filter(x)  ( (x >= ' ') && (x <= '~') ? x : '.' )
#define SPACE 8

//int main(int, char **);

char vers[]  = "tp 1.02 (02.08.1994)";
char usage[] = "Usage: tp [-HNBFPXTD?] <Filename>";

short hex    = FALSE,
      numb   = FALSE,
      b      = FALSE,
      form   = FALSE,
      prompt = FALSE,
      x      = FALSE,
      tail   = FALSE,
      detab  = FALSE;

int param( int ac, char *av[] )
{
   long i, j, retcode = FALSE;

   for ( i = 1; i < ac; i++ )
      if ( av[i][0] == '-' )  for ( j = 1; j < (int)strlen( av[i] ); j++ )
      {
         switch ( tolower( av[i][j] ) )
         {
            case 'h' : hex    = TRUE; break;
            case 'n' : numb   = TRUE; break;
            case 'b' : b      = TRUE; break;
            case 'f' : form   = TRUE; break;
            case 'p' : prompt = TRUE; break;
            case 'x' : x      = TRUE; break;
            case 't' : tail   = TRUE; break;
            case 'd' : detab  = TRUE; break;
            case '?' :
            {
               printf( "\n%s -  A Better Type.\n", vers );
               puts( usage );
               puts( "      H - Hex listing." );
               puts( "      N - Display line numbers." );
               puts( "      B - Display filename at begining of text." );
               puts( "      F - Formfeed at end of text." );
               printf( "      P - Prompt after every %d lines.\n", PROMPTLINES );
               puts( "      X - Filtered text." );
               puts( "      T - Tail of text." );
               puts( "      D - Detab text." );
               puts( "      ? - This text." );
               return( FALSE );
            }
            default  : printf( "\nIllegal Option: %lc\n", av[i][j] );
         }
      }
      else retcode = TRUE;

   if ( ( ac < 2 ) || ( !retcode ) )
   {
      puts( usage );
      return( FALSE );
   }
}


int tp( char *fname )
{
   int i, ch, slot, col = 0;
   unsigned long line = 1, pline = 0;
   char buff[16];
// char error[60];
   int hbuff[16], bpos = 0, pch = '\n';
   FILE *in;

   if ( fname[0] == '-' )  return( TRUE );

   if ( !( in = fopen( fname, "r" ) ) )
   {
      printf( "Error With File: %s\n", fname );
      return( FALSE );
   }

   if ( b )
   {
      pline += 2;
      printf( "\n*** File %s ***\n", fname );
   }

   if ( tail )
   {
      if ( hex )
      {
/*       fseek( in, 0, 2 );
         i = ftell( in );

         line = i - ( 16 * PROMPTLINES );
         fseek( in, -line, 2 );  */
         fseek( in, ( -16 * PROMPTLINES ), 2 );
      }
      else
      {
         /* Find number of carrage returns, or characters */
         i = 0;
         while ( ( ch = getc( in ) ) != -1 )
         {
            if ( hex )  i++;
            else  if ( ch == '\n' )  i++;
         }

         fseek( in, 0, 0 );

         /* Find line number one page up from bottom, and position */
         line = i - PROMPTLINES + 1;
         while ( i > PROMPTLINES )
         {
            ch = fgetc( in );
            if ( ch == '\n' )  i--;
         }
      }
   }

   if ( hex )
   {
      line--;
      x = numb = FALSE;
   }

   while ( ( ch = fgetc( in ) ) != -1 )              /* Maine loop! */
   {
      if ( ( numb ) && ( pch == '\n' ) )  printf( "%4ld ", line );

      pch = ch;
      col ++;

      if ( hex )
      {
         hbuff[bpos++] = ch;
         if ( bpos == 16 )
         {
            printf( "%05lx: ", line );
            bpos = 0;
            line += 16;
            pline++;
            for ( i = 0; i < 16; i++ )  printf( "%02lx ", hbuff[i] );
            fputc( ' ', stdout );
            for ( i = 0; i < 16; i++ )  fputc( filter( hbuff[i] ), stdout );
            fputc( '\n', stdout );
            ch = '\n';
         }
      }
      else if ( detab && ( ch == '\t' ) )
      {
         slot = col % SPACE;
         slot = SPACE - slot;
         if ( slot == 0 )  slot = SPACE;
         for ( i = 0; i < slot; i++ )  fputc( ' ', stdout );
         col = SPACE;
      }
//    else if ( todos && ch == '\n' )  puts( "\0x0d" );
      else  fputc( ( x && ( ch != '\n' ) ) ? filter( ch ) : ch, stdout );

      if ( ch == '\n' )
      {
         col = 0;
         if ( !hex )
         {
            line++;
            pline++;
         }

         if ( ( prompt ) && ( pline == PROMPTLINES ) )
         {
            pline = 0;
            printf( " -- More -- " );
            fflush( stdout );
            fflush( stdin );

            fgets( buff, 15, stdin );
            if ( buff[0] == '-' )
            {
               for ( i = 1; i < (int)strlen( buff ); i++ )
               {
                  buff[i] = (char)tolower( buff[i] );
                  switch ( buff[i] )
                  {
                     case 'n' : numb   = !numb;   break;
                     case 'h' : hex    = !hex;    break;
                     case 'b' : b      = !b;      break;
                     case 'f' : form   = !form;   break;
                     case 'p' : prompt = !prompt; break;
                     case 'x' : x      = !x;      break;
                     case 'd' : detab  = !detab;
                  }
               }
            }
            if ( ( buff[0] == 'n' ) || ( buff[0] == 'q' ) )  break;
         }
      }
   }

   if ( ( hex ) && ( buff[0] != 'n' ) && ( bpos != 0 ) )
   {
      printf( "%05lx: ", line );
      for ( i = 0; i < bpos; i++)  printf( "%02lx ", hbuff[i] );
      fputc( ' ', stdout );
      for ( i = 0; i < ( 3 * ( 16 - bpos ) ); i++)  fputc( ' ', stdout );
      for ( i = 0; i < bpos; i++ )  fputc( filter( hbuff[i] ), stdout );
      fputc( '\n', stdout );
   }

   if ( form )  fputc( '\f', stdout );
// fflush( in );
   fclose( in );

   return( buff[0] != 'q' );
}


int main( int argc, char *argv[] )
{
   int i;

   if ( !param( argc, argv ) )
   {
      return( 1 );
   }

   for ( i = 1; i < argc; i++ )
      if ( !tp( argv[i] ) )  break;

   return( 0 );
}
