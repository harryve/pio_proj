/*
 (C) COPYRIGHT 2004 TECHNOLUTION BV, GOUDA NL
| =======          I                   ==          I    =
|    I             I                    I          I
|    I   ===   === I ===  I ===   ===   I  I    I ====  I   ===  I ===
|    I  /   \ I    I/   I I/   I I   I  I  I    I  I    I  I   I I/   I
|    I  ===== I    I    I I    I I   I  I  I    I  I    I  I   I I    I
|    I  \     I    I    I I    I I   I  I  I   /I  \    I  I   I I    I
|    I   ===   === I    I I    I  ===  ===  === I   ==  I   ===  I    I
|                 +---------------------------------------------------+
+----+            |  +++++++++++++++++++++++++++++++++++++++++++++++++|
     |            |             ++++++++++++++++++++++++++++++++++++++|
     +------------+                          +++++++++++++++++++++++++|
                                                        ++++++++++++++|
             A U T O M A T I O N     T E C H N O L O G Y         +++++|

    $Date: 2004/09/21 13:57:13 $
    $Author: harry $
    $Revision: 1.5 $
    $Log: MonUtil.h,v $
    Revision 1.5  2004/09/21 13:57:13  harry
    no message


*/
bool xatoi (char *str, int *val);
bool hatoi (char *str, int *val);
int wordlen (char *str);

void DoHelp (int argc, char *argv[]);

typedef struct
{
    const char  *cmd;                   /* Command string                       */
    void        (*proc)(int, char **);  /* Procedure handling the command       */
    int8_t      min_argc;               /* Minimum number of arguments, 0 ..    */
                                        /* Maximum number of arguments, -1      */
    int8_t      max_argc;               /* means unlimited                      */
    const char  *syntax;                /* Syntax of the command                */
} Cmds;

int HdlCmd (Cmds *cmdTab, char *cmd);
void DoByte (int argc, char *argv[]);
void DoWord (int argc, char *argv[]);
void DoMemFill (int argc, char *argv[]);
void DoMemDump(int argc, char *argv[]);
