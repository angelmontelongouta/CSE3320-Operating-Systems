// The MIT License (MIT)
//
// Copyright (c) 2020 Trevor Bakker
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
/*
	Name: Angel Montelongo
	ID:	  1001665238
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <stdint.h>

#define MAX_NUM_ARGUMENTS 4

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

//
// GLOBAL VARIABLES
//

char    BS_OEMName[8];
int16_t BPB_BytsPerSec;
int8_t  BPB_SecPerClus;
int16_t BPB_RsvdSecCnt;
int8_t  BPB_NumFATs;
int16_t BPB_RootEntCnt;
char    BS_VolLab[11];
int32_t BPB_FATSz32;
int32_t BPB_RootClus;

char deleted_file[100];
uint8_t deleted_attribute;

int32_t RootDirSectors = 0;
int32_t FirstDataSector = 0;
int32_t FirstSectorofCluster = 0;

struct __attribute__((__packed__)) DirectoryEntry
{
  char     DIR_Name[11];
  uint8_t  DIR_Attr;
  uint8_t  Unused1[8];
  uint16_t DIR_FirstClusterHigh;
  uint8_t  Unused2[4];
  uint16_t DIR_FirstClusterLow;
  uint32_t DIR_FileSize;
};

struct DirectoryEntry dir[16];

FILE *fp;
FILE *ofp;

uint8_t buffer[512];
uint8_t buffer_read[512];

//
// SUBROUTINES
//

int compare(char *d_name, char *_input)
{
  char IMG_Name[11];

  char input[11];
  char expanded_name[12];

  strncpy( IMG_Name, d_name, 11);
  strncpy( input, _input, 11);

  if( strncmp(_input, "..", 2 ) != 0)
  {
    memset( expanded_name, ' ', 12 );

    char *token = strtok( input, "." );

    strncpy( expanded_name, token, strlen( token ) );

    token = strtok( NULL, "." );

    if( token )
    {
      strncpy( (char*)(expanded_name+8), token, strlen(token ) );
    }

    expanded_name[11] = '\0';

    int i;
    for( i = 0; i < 11; i++ )
    {
      expanded_name[i] = toupper( expanded_name[i] );
    }
  }
  else
  {
    strncpy(expanded_name, "..", 2);
    expanded_name[3] = '\0';
    if( strncmp( expanded_name, IMG_Name, 2 ) == 0 )
    {
      return 1;
    }

    return 0;
  }
  if( strncmp( expanded_name, IMG_Name, 11 ) == 0 )
  {
    return 1;
  }

  return 0;
}

int LBAtoOffset(int32_t sector)
{
  return (( sector - 2) * BPB_BytsPerSec) + (BPB_BytsPerSec * BPB_RsvdSecCnt)
  + (BPB_NumFATs * BPB_FATSz32 * BPB_BytsPerSec);
}

int16_t NextLB( uint32_t sector )
{
  uint32_t FATAddress = ( BPB_BytsPerSec * BPB_RsvdSecCnt) + ( sector *4);
  int16_t val;
  fseek(fp, FATAddress, SEEK_SET);
  fread(&val, 2, 1, fp);
  return val;
}

//
// MAIN
//

int main()
{

  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the mfs prompt
    printf ("mfs> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;

    // Pointer to point to the token
    // parsed by strsep
    char *arg_ptr;

    char *working_str  = strdup( cmd_str );

    // we are going to move the working_str pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *working_root = working_str;

    // Tokenize the input stringswith whitespace used as the delimiter
    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) &&
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    // Now print the tokenized input as a debug check
    // \TODO Remove this code and replace with your FAT32 functionality

    //Checking for enter key just pressed, will execute new line
    if(token[0] == NULL) continue;
    //Checking for Quit or Exit to close the program
    else if((strcmp(token[0], "exit") == 0)) exit(0);
  	else if((strcmp(token[0], "quit") == 0)) exit(0);
    //This Command will open a fat32 image
    else if(strcmp("open", token[0]) == 0)
    {
      if(token[1] == NULL)
      {
        printf("Missing a paramater.\n");
        continue;
      }

      if( fp == NULL )
      {
        fp = fopen(token[1], "r");
        if(fp == NULL)
        {
          printf("Error: File System image not found.\n");
          continue;
        }
        //Taking in the Variables from the files
        fseek(fp, 11, SEEK_SET);
        fread(&BPB_BytsPerSec, 2, 1, fp);

        fseek(fp, 13, SEEK_SET);
        fread(&BPB_SecPerClus, 1, 1, fp);

        fseek(fp, 14, SEEK_SET);
        fread(&BPB_RsvdSecCnt, 2, 1, fp);

        fseek(fp, 16, SEEK_SET);
        fread(&BPB_NumFATs, 1, 1, fp);

        fseek(fp, 36, SEEK_SET);
        fread(&BPB_FATSz32, 4, 1, fp);

        fseek(fp, BPB_NumFATs * (BPB_FATSz32 * BPB_BytsPerSec) +
        (BPB_RsvdSecCnt * BPB_BytsPerSec), SEEK_SET);
        fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);
      }
      else
      {
        printf("Error: File system image already open.\n");
        continue;
      }
    }
    //This command will close the fat32 image
    else if(strcmp("close", token[0]) == 0)
    {
      if( fp == NULL)
      {
        printf("Error: File system not open.\n");
        continue;
      }

      fclose(fp);
      fp = NULL;
    }
    //This command will print out information about the file System
    else if(strcmp("info", token[0]) == 0)
    {
      if( fp == NULL )
      {
        printf("Error: File System image must be opened first\n");
        continue;
      }

      printf("BPB_BytsPerSec: \t %d   %x\n", BPB_BytsPerSec, BPB_BytsPerSec);
      printf("BPB_SecPerClus: \t   %d     %x\n", BPB_SecPerClus, BPB_SecPerClus);
      printf("BPB_RsvdSecCnt: \t  %d    %x\n", BPB_RsvdSecCnt, BPB_RsvdSecCnt);
      printf("BPB_NumFATs   : \t   %d     %x\n", BPB_NumFATs, BPB_NumFATs);
      printf("BPB_FATSz32   : \t%d   %x\n", BPB_FATSz32, BPB_FATSz32);
    }
    //Command will print the attributes and starting cluster number
    //of the file or directory Name
    else if(strcmp("stat", token[0]) == 0)
    {
      if( fp == NULL )
      {
        printf("Error: File System image must be opened first\n");
        continue;
      }

      if(token[1] == NULL)
      {
        printf("Missing a paramater.\n");
        continue;
      }

      int i;
      int dir_check = 0;

      for(i = 0; i < 16; i++)
      {
        if(compare(dir[i].DIR_Name, token[1]))
        {
          dir_check = 1;
          break;
        }
      }

      if(dir_check)
      {
        printf("File Size:  %d\n", dir[i].DIR_FileSize);
        printf("First Cluster Low:   %d\n", dir[i].DIR_FirstClusterLow);
        printf("DIR_ATTR:  %d\n", dir[i].DIR_Attr);
        printf("First Cluster High:  %d\n", dir[i].DIR_FirstClusterHigh);
      }
      else
      {
        printf("Error: File not found.\n");
      }

    }
    //This cmmand is retrieve the fila from the FAT 32 image
    else if(strcmp("get", token[0]) == 0)
    {
      if( fp == NULL )
      {
        printf("Error: File System image must be opened first\n");
        continue;
      }

      if(token[1] == NULL)
      {
        printf("Missing a paramater.\n");
        continue;
      }

      int i;

      for(i = 0; i < 16; i++)
      {
        if(compare(dir[i].DIR_Name, token[1]))
        {
          int cluster = dir[i].DIR_FirstClusterLow;
          int offset = LBAtoOffset(cluster);

          fseek(fp, offset, SEEK_SET);
          ofp = fopen(token[1], "w");

          if(ofp == NULL)
          {
            printf("Error: File not found.\n");
            continue;
          }

          int size = 512;

          while(size >= BPB_BytsPerSec)
          {
            fread(buffer, 512, 1, fp);
            fwrite(buffer, 512, 1, ofp);

            size = size - BPB_BytsPerSec;

            cluster = NextLB(cluster);
            if(cluster > -1)
            {
              offset = LBAtoOffset(cluster);

              fseek(fp, offset, SEEK_SET);
            }
          }

          if( size > 0 )
          {
            fread( buffer, size, 1, fp);
            fwrite(buffer, size, 1, ofp);
          }
          fclose(ofp);
          break;
        }
      }
    }
    //This command will change the current working Directory
    else if(strcmp("cd", token[0]) == 0)
    {
      if( fp == NULL )
      {
        printf("Error: File System image must be opened first\n");
        continue;
      }

      int i;
      char * directory;

      directory = strtok(token[1], "/");

      for(i = 0; i < 16; i++)
      {
        if(compare(dir[i].DIR_Name, token[1]))
        {
          int cluster = dir[i].DIR_FirstClusterLow;
          if(cluster == 0)
          {
            cluster = 2;
          }
          int offset = LBAtoOffset(cluster);

          fseek(fp, offset, SEEK_SET);
          fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);
          break;
        }
       }

       while( directory == strtok(NULL, "/"))
       {
         int cluster = dir[i].DIR_FirstClusterLow;
         if(cluster == 0)
         {
           cluster = 2;
         }
          int offset = LBAtoOffset(cluster);

         fseek(fp, offset, SEEK_SET);
         fread(&dir[0], sizeof(struct DirectoryEntry), 16, fp);
        }

    }
    //This command lists the directory contents
    else if(strcmp("ls", token[0]) == 0)
    {
      if( fp == NULL )
      {
        printf("Error: File System image must be opened first\n");
        continue;
      }

      int i;

      for(i = 0; i < 16; i++)
      {
        if(((dir[i].DIR_Attr == 0x01) || (dir[i].DIR_Attr == 0x10) ||
        (dir[i].DIR_Attr == 0x20)) && dir[i].DIR_Name[0] != '?')
        {
            char name[12];
            memcpy(name, dir[i].DIR_Name, 11);
            name[11] = '\0';
            printf("%s\n", name);
        }
      }
    }
    //Reads from the given file at the position, in bytes, position
    else if(strcmp("read", token[0]) == 0)
    {
      if( fp == NULL )
      {
        printf("Error: File System image must be opened first\n");
        continue;
      }
      if((token[1] == NULL) || (token[2] == NULL) || (token[3] == NULL));
      {
        printf("Missing a paramater.\n");
        continue;
      }

      int i;

      int dir_check = 0;

      for(i = 0; i < 16; i++)
      {
        if(compare(dir[i].DIR_Name, token[1]))
        {
          dir_check = 1;
          break;
        }
      }
      if(dir_check)
      {
        int cluster = dir[i].DIR_FirstClusterLow;
        int offset = LBAtoOffset(cluster);

        char *a = token[2];
        char *b = token[3];

        int position;
        int num_of_bytes;

        position = atoi(a);
        num_of_bytes = atoi(b);

        fseek(fp, offset, SEEK_SET);

        fseek(fp, position, SEEK_CUR);
        fread(buffer_read, num_of_bytes, 1, fp);

        for(i = 0; i < num_of_bytes; i++)
        {
          printf("%x ", buffer_read[i]);
        }
        printf("\n");
      }
      else
      {
        printf("Error: File not found.\n");
      }
    }
    //Deletes the file from the file system
    else if(strcmp("del", token[0]) == 0)
    {
      if( fp == NULL )
      {
        printf("Error: File System image must be opened first\n");
        continue;
      }

      if(token[1] == NULL)
      {
        printf("Missing a paramater.\n");
        continue;
      }

      int i;
      int dir_check = 0;

      for(i = 0; i < 16; i++)
      {
        if(compare(dir[i].DIR_Name, token[1]))
        {
          dir_check = 1;
          break;
        }
      }

      if(dir_check)
      {
        deleted_attribute = dir[i].DIR_Attr;
        dir[i].DIR_Attr = 0xe5;
        strncpy(deleted_file, dir[i].DIR_Name, 11);
        strcpy(dir[i].DIR_Name, "?");
      }
      else
      {
        printf("Error: Directory/File not found.\n");
      }
    }
    //Un-deltes the file from the file System
    else if(strcmp("undel", token[0]) == 0)
    {
      if( fp == NULL )
      {
        printf("Error: File System image must be opened first\n");
        continue;
      }

      if(token[1] == NULL)
      {
        printf("Missing a paramater.\n");
        continue;
      }

      int i;
      int dir_check;

      for(i = 0; i < 16; i++)
      {
        if(strcmp(dir[i].DIR_Name, "?"))
        {
          dir_check = 1;
          break;
        }
      }

      if(dir_check)
      {
        strncpy(dir[i].DIR_Name, deleted_file, 11);
        dir[i].DIR_Attr = deleted_attribute;
      }
      else
      {
        printf("Error: Directory/File not found.\n");
      }

    }
    else
    {
      printf("Invalid command\n");
    }

    free( working_root );

  }
  return 0;
}
