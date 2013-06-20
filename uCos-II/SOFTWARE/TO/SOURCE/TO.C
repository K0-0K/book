/*$TITLE=Command to jump 'TO' a specified project*/
/*
*********************************************************************************************************
*                                               TO
*
*                              Command to jump TO a specified project
*
*
*
* Filename      : TO.C
* Programmer(s) : Jean J. Labrosse
*********************************************************************************************************
*
* This program allows the user to change directory by simply specifying a name which is associated with
* the directory's path.
*
*
* Program use:
*
*        TO name [path]
*
*        where :
*
*             name               Is the name associated with a path
*             [path]             Is an optional path used to specify the path associated with the project
*
*        Notes : TO assumes the presence of the file TO.TBL on the root directory of the current drive
*                TO.TBL is an ASCII file which has the following format:
*
*                       name, path
*                       name, path
*                        .     .
*                        .     .
*                       name, path
*
*                where, 'name' is the name associated with 'path'
*********************************************************************************************************
*/
/*$PAGE*/
/*
*********************************************************************************************************
*                                         INCLUDE FILES
*********************************************************************************************************
*/

#include <DIR.H>
#include <STDIO.H>
#include <STRING.H>
#include <CTYPE.H>
#include <CONIO.H>

/*
*********************************************************************************************************
*                                           CONSTANTS
*********************************************************************************************************
*/

#define  NUL         0x00
#define  TO_NOT_EOF     0
#define  TO_EOF        -1

#define  TRUE           1
#define  FALSE          0

/*
*********************************************************************************************************
*                                      FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  main(int argc, char *argv[]);
static  void  ToDispUsage(void);
static  void  ToErrNoFile(void);
static  void  ToFindPath(char *project);
static  int   ToAddPath(char *project, char *path);
static  int   ToRdLine(void);

/*
*********************************************************************************************************
*                                       GLOBAL VARIABLES
*********************************************************************************************************
*/

static  char  ToName[100];
static  char  ToPath[200];
static  char  ToLine[300];
static  char  ToLineRd[300];
static  FILE *ToFilePtr;
static  FILE *ToTempFilePtr;

/*$PAGE*/
/*
*********************************************************************************************************
*                                         TO ENTRY POINT
*********************************************************************************************************
*/

void main(int argc, char *argv[])
{
    int stat;


    switch (argc) {
        case 1:  ToDispUsage();                                      /* TO                             */
                 break;

        case 2:  ToFilePtr = fopen("\\TO.TBL", "r");                 /* TO <name>                      */
                 if (ToFilePtr != NULL) {
                     ToFindPath(argv[1]);                            /* Find path assoicated with name */
                     fclose(ToFilePtr);
                 } else {
                     ToErrNoFile();                                  /* \TO.TBL does not exist         */
                 }
                 break;

        case 3:  ToFilePtr     = fopen("\\TO.TBL", "r");             /* TO <name> <path>               */
                 ToTempFilePtr = fopen("ZZZZZZZZ.ZZZ", "w");         /* Create temporary file          */
                 if (ToFilePtr != NULL && ToTempFilePtr != NULL) {
                     stat = ToAddPath(argv[1], argv[2]);             /* Add new entry in \TO.TBL       */
                     fclose(ToFilePtr);
                     fclose(ToTempFilePtr);
                     if (stat == 1) {                                /* See if unique name ...         */
                         ToFilePtr     = fopen("\\TO.TBL", "w");     /* ... ZZZZZZZZ.ZZZ -> \TO.TBL    */
                         ToTempFilePtr = fopen("ZZZZZZZZ.ZZZ", "r");
                         while (fgets(ToLine, 100, ToTempFilePtr) != NULL) {
                             fputs(ToLine, ToFilePtr);
                         }
                         fclose(ToFilePtr);
                         fclose(ToTempFilePtr);
                     }
                     remove("ZZZZZZZZ.ZZZ");                         /* Delete temporary file          */
                 } else {
                     ToErrNoFile();
                 }
                 break;

        default: ToDispUsage();                                      /* default                        */
                 break;

    }
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                DISPLAY THE USAGE OF THIS PROGRAM
*********************************************************************************************************
*/

static void ToDispUsage(void)
{
    printf("TO.EXE\n\n");
    printf("Program to change directory:\n");
    printf("   This program reads a file in the ROOT directory called \"\\TO.TBL\".\n");
    printf("   This file is an ASCII file.\n");
    printf("   This file contains a table of names and their path.\n");
    printf("   This name is separated from the directory by a comma.\n\n");
    printf("Usage:  TO <name> [path]\n\n");
    printf("   To add a new name:\n");
    printf("       Type TO followed by the name and finally, its path\n");
}


/*
*********************************************************************************************************
*                         DISPLAY ERROR MESSAGE SINCE \TO.TBL NOT FOUND
*********************************************************************************************************
*/

static void ToErrNoFile(void)
{
    printf("Error:\n");
    printf("   \"\\TO.TBL\" does not exist.\n");
    printf("   \"\\TO.TBL\" is an ASCII file that contains a name and a path.\n");
    printf("   The file is organized as follows:\n");
    printf("      <name>, <path>\n");
}

/*$PAGE*/
/*
*********************************************************************************************************
*                                  FIND PATH ASSOCIATED WITH NAME
*********************************************************************************************************
*/

static void ToFindPath(char *name)
{
    int  stat;
    char *ptr;


    ptr = name;                                  /* Convert name to upper case                         */
    while (*ptr) {
        *ptr = toupper(*ptr);
        ptr++;
    }
    do {
        stat = ToRdLine();                       /* Read line from \TO.TBL and extract name & path     */
        if (strcmp(name, ToName) == 0) {         /* See if found desired name                          */
            clrscr();                            /* Clear the screen                                   */
            chdir(ToPath);                       /* Change directory to path associated with name      */
            return;
        }
    } while (stat != TO_EOF);                    /* Read all lines from \TO.TBL                        */
    printf("Invalid NAME.\n");                   /* Name not found in \TO.TBL                          */
}

/*$PAGE*/
/*
*********************************************************************************************************
*                            ADD PATH ASSOCIATED WITH NAME TO \TO.TBL
*********************************************************************************************************
*/

static int ToAddPath(char *name, char *path)
{
    char *ptr;
    char  buf[20];


    ptr = name;                                            /* Convert name name to upper case          */
    while (*ptr) {
        *ptr = toupper(*ptr);
        ptr++;
    }
    while (ToRdLine() != TO_EOF) {                         /* Read line from \TO.TBL                   */
        if (strcmp(name, ToName) == 0) {                   /* See if name already in \TO.TBL           */
            clrscr();                                      /* Clear the screen                         */
            printf("Name <%s> already exist.\n", name);    /* Indicate that already exist              */
            return (0);
        }
        fprintf(ToTempFilePtr, "%s", ToLineRd);
    }
    strcpy(buf, "            ");                           /* Place name name within 11 characters     */
    ptr = &buf[0];
    while (*name)
        *ptr++ = *name++;
    *ptr++ = ',';
    ptr    = path;                                         /* Convert path to upper case characters    */
    while (*ptr) {
        *ptr = toupper(*ptr);
        ptr++;
    }
    fprintf(ToTempFilePtr, "%s%s\n", buf, path);           /* Add new name & path at the end of TO.TBL */
    return (1);
}

/*$PAGE*/
/*
*********************************************************************************************************
*                            READ A LINE FROM \TO.TBL AND SEPARATE TOKENS
*********************************************************************************************************
*/

static int ToRdLine(void)
{
    char *ptr;
    char *source;
    char *desti;


    if (fgets(ToLine, 100, ToFilePtr) != NULL) {
        strcpy(ToLineRd, ToLine);
        ptr = strchr(ToLine, '\n');                        /* Remove the newline character             */
        if (ptr != NULL) {
            *ptr = NUL;
        }
        ptr    = strtok(ToLine, ",");                      /* Get name                                 */
        source = ptr;                                      /* Remove any spaces                        */
        desti  = &ToName[0];
        while (*source) {
            if (*source != ' ') {
                *desti++ = toupper(*source);
                source++;
            } else {
                source++;
            }
        }
        *desti = NUL;
        ptr    = strtok(NULL, ",");                        /* Get path                                 */
        source = ptr;                                      /* Remove any spaces                        */
        desti  = &ToPath[0];
        while (*source) {
            if (*source != ' ') {
                *desti++ = *source++;
            } else {
                source++;
            }
        }
        *desti = NUL;
        return (TO_NOT_EOF);
    } else {
        return (TO_EOF);
    }
}
