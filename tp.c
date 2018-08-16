
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

char vers[]  = "tp 1.02a";
char usage[] = "Usage: tp [-hnbfpxtd?] [FILE]...\n";

short hex    = FALSE,
      octal  = FALSE,
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
            case 'o' : octal  = TRUE; break;
            case 'h' : hex    = TRUE; octal = FALSE; break;
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
               printf( "%s", usage );
               puts( "  -h\tHex listing" );
               puts( "  -n\tDisplay line numbers" );
               puts( "  -b\tDisplay filename at begining of text" );
               puts( "  -f\tDisplay formfeed at end of text" );
               printf( "  -p\tPrompt after every %d lines\n", PROMPTLINES );
               puts( "  -x\tFilter non-printable characters" );
               puts( "  -t\tShow only tail of text" );
               puts( "  -d\tRemove tab characters" );
               puts( "  -?\tDisplay this text and exit" );
               puts( "\nExamples:" );
               puts( "  tp -ph f.bin\tOutput hex dump of f.bin to standard out and prompt every 22 lines" );
               puts( "  tp -xf f.txt\tOutput contents of f.txt to standard out, filter unprintable text, then send a formfeed" );
               return( FALSE );
            }
            default  : printf( "\nIllegal Option: %lc\n", av[i][j] );
         }
      }
      else retcode = TRUE;

   if ( ( ac < 2 ) || ( !retcode ) )
   {
      fputs( usage, stderr );
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
      if ( hex || octal )
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

   if ( hex || octal )
   {
      line--;
      x = numb = FALSE;
   }

   while ( ( ch = fgetc( in ) ) != -1 )              /* Maine loop! */
   {
      if ( ( numb ) && ( pch == '\n' ) )  printf( "%6ld  ", line );

      pch = ch;
      col ++;

      if ( hex || octal )
      {
         hbuff[bpos++] = ch;
         if ( bpos == 16 )
         {
            if ( hex ) printf( "%08lx  ", line ) ; else printf( "%08lo  ", line );
            bpos = 0;
            line += 16;
            pline++;
            for ( i = 0; i < 8; i++ )  if ( hex ) printf( "%02x ", hbuff[i] ); else printf( "%06o ", hbuff[i] );
            fputc( ' ', stdout );
            for ( i = 8; i < 16; i++ )  if ( hex ) printf( "%02x ", hbuff[i] ); else printf( "%06o ", hbuff[i] );
            if ( hex )
            {
               fputc( ' ', stdout );
               fputc( '|', stdout );
               for ( i = 0; i < 16; i++ )  fputc( filter( hbuff[i] ), stdout );
               fputc( '|', stdout );
            }
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
            for ( i = 0; i < (int)strlen( buff ); i++ )
            {
               buff[i] = (char)tolower( buff[i] );
               switch ( buff[i] )
               {
                  case 'n' : numb   = !numb;   break;
                  case 'o' : octal  = !octal;  break;
                  case 'h' : hex    = !hex;    break;
                  case 'b' : b      = !b;      break;
                  case 'f' : form   = !form;   break;
                  case 'p' : prompt = !prompt; break;
                  case 'x' : x      = !x;      break;
                  case 'd' : detab  = !detab;
               }
            }
            if ( buff[0] == 'q' )  break;
         }
      }
   }

   if ( ( hex || octal ) && ( buff[0] != 'n' ) && ( bpos != 0 ) )
   {
      if ( hex ) printf( "%08lx  ", line ); else printf( "%08lo  ", line );
      for ( i = 0; i < bpos; i++)  if ( hex ) printf( "%02x ", hbuff[i] ); else printf( "%06o ", hbuff[i] );
      fputc( ' ', stdout );
      for ( i = 0; i < ( 3 * ( 16 - bpos ) ); i++)  fputc( ' ', stdout );
      if ( hex )
      {
         fputc( ' ', stdout );
         fputc( '|', stdout );
         for ( i = 0; i < bpos; i++ )  fputc( filter( hbuff[i] ), stdout );
         for ( i = 0; i < bpos; i++ )  fputc( ' ', stdout );
         fputc( '|', stdout );
      }
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
