#include <Arduino.h>
#include "monutil.h"

static Cmds *cmdTable;

/*                      ATOI
                        ====
  Abstract:

    This routine converts a number represented in ASCII to an integer.
*/
bool xatoi(char *str, int *val)
{
    char    ch;

    *val = 0;
    while ((ch = *str++) != 0) {
        if (ch >= '0' && ch <= '9') {
            *val = *val * 10 + ch - '0';
        }
        else {
            return false;
        }
    }

    return true;
}

/*eject*/
/*                      HATOI
                        =====
  Abstract:

    This routine is equivalent to atoi except that the string pointed at
    by 'str' contains hex-digits instead of normal digits.
*/
bool hatoi(char *str, int *val)
{
    char    ch;

    *val = 0;

    while ((ch = *str++) != 0) {
        if (ch >= '0' && ch <= '9') {
            *val = *val * 16 + ch - '0';
        }
        else {
            if (ch >= 'a' && ch <= 'f') {
                *val = *val * 16 + ch - 'a' + 10;
            }
            else if (ch >= 'A' && ch <= 'F') {
                *val = *val * 16 + ch - 'A' + 10;
            }
            else {
                return false;
            }
        }
    }

    return true;
}

/*eject*/
/*                      WORDLEN
                        =======
  Abstract:

    Determine the length of a word (delimited by white space characters)
*/
int wordlen(char *str)
{
    char    ch;
    int     len = 0;
                                    /* Skip white spaces                    */
    while ((ch = *str) != 0 && (ch == ' ' || ch == '\t')) {
        str++;
    }
                                    /* Determine word length                */
    while ((ch = *str) != 0 && ch != ' ' && ch != '\t') {
        len++;
        str++;
    }

    return len;
}

/*
                DoByte
                =======
  Abstract:

    Set memory address argv[1] and up to bytes argv[2/3/...].
*/
void DoByte(int argc, char *argv[])
{
    unsigned char   *p;
    int             val, i, addr;

    if (!hatoi(argv[1], &addr)) {
        Serial.printf("Invalid address\n");
        return;
    }

    p = (unsigned char *)addr;

    if (argc == 2) {
        Serial.printf("%p = %x\n", p, *p);
        return;
    }

    for (i = 2; i < argc; i++) {
        if (!hatoi(argv[i], &val)) {
            Serial.printf("Invalid value\n");
            return;
        }
        *p++ = (unsigned char) val;
    }
}

/*
                DoWord
                =======
  Abstract.
    Set memory address argv[1] to word argv[2].
*/
void DoWord(int argc, char *argv[])
{
    int *p, val, addr;

    if (!hatoi(argv[1], &addr)) {
        Serial.printf("Invalid address\n");
        return;
    }

    p = (int *)addr;

    if (argc == 2) {
        Serial.printf("%p = %x\n", p, *p);
        return;
    }

    if (!hatoi(argv[2], &val)) {
        Serial.printf("Invalid value\n");
        return;
    }

    *p = val;
}

/*
                DoMemFill
                =========
  Abstract:

    Fill argv[3] bytes starting at argv[1] with the value argv[2].
*/
void DoMemFill(int argc, char *argv[])
{
    char    *p;
    int     addr, val, len;

    if (!hatoi(argv[1], &addr)) {
        Serial.printf("Invalid address\n");
        return;
    }
    p = (char *)addr;

    if (!hatoi(argv[2], &val)) {
        Serial.printf("Invalid value\n");
        return;
    }

    if (!hatoi(argv[3], &len)) {
        Serial.printf("Invalid value\n");
        return;
    }

    while (len-- > 0) {
        *p++ = (char) val;
    }
}

/*
                DispMembytes
                ============
  Abstract:

*/
//extern cregister volatile unsigned int CSR;
void DispMembytes(uint8_t *buf, uint32_t addr, int len, int header)
{
    uint8_t ch;
    int     pos, i;
    //char    AddBuf[10];

    if (header) {                       /* Display header above dump? */
        Serial.printf("         00 01 02 03 04 05 06 07  08 09 0A 0B 0C 0D 0E 0F\n");
    }

    while (len > 0) {
                                        /* Display memory address */
        Serial.printf("%08lx ", addr);

        pos = 0;
        for (i = 0; i < len && i < 16; i++) {/* Display the bytes for this line */
            Serial.printf("%02x ", buf[i]);
            if (pos++ == 7) {                 /* Hole between byte 7 and 8 */
                Serial.printf(" ");
            }
        }

                                        /* Display bytes in ascii */
        Serial.printf("   ");

        for (i = 0; i < len && i < 16; i++) {
            if ((ch = buf[i]) < 0x20 || ch > 0x7e) {
                ch = '.';
            }

            Serial.printf("%c", ch);
        }

        Serial.printf("\n");

        len  -= 16;
        addr += 16;
        buf  += 16;
    }
}

/*
                DoMemDump
                =========
  Abstract.
 */
void DoMemDump(int argc, char *argv[])
{
    static  uint8_t *addr = 0;
    static  int     saveLen = 256;
    int             len, a;
    bool            first = true;

    if (argc >= 2) {
        if (!hatoi(argv[1], &a)) {
            Serial.printf("Invalid address\n");
            return;
        }

        addr = (uint8_t *)a;
    }

    if (argc == 3) {
        if (!hatoi(argv[2], &saveLen)) {
            Serial.printf("Invalid length\n");
            return;
        }
    }

    len = saveLen;
    while (len > 0) {
        DispMembytes(addr, (uint32_t)addr, ((len > 16) ? 16 : len), first);
        first = false;
        addr += 16;
        len  -= 16;
    }
}

void DoHelp(int argc, char *argv[])
{
    int     t, longList;
    Cmds    *p;

    longList = (*argv[0] != '?');       /* Long or short help               */

    /* Display for the general and level0 commands                          */

    p = cmdTable;

    for (t = 1; p->cmd; p++, t++) {
        Serial.printf("%-12s ", p->cmd);
        if (longList) {
            Serial.printf("%s\n", p->syntax);
        }
        else if (t % 6 == 0) {
            Serial.printf("\n");
        }
    }

    if (!longList) {
        Serial.printf("\n");
    }
}

/*eject*/
/*                      GetArgs
                        =======
  Abstract:

    Get the arguments of a command line.
*/
static void GetArgs(char *cmd, int *argc, char **argv)
{
    char    ch;
    int     len;

    *argc = 0;

    /* For all characters in the command buffer */
    while (*cmd) {
                                        /* Skip spaces                      */
        while ((ch = *cmd) != 0 && (ch == ' ' || ch == '\t')) {
            cmd++;
        }

        argv[*argc] = cmd;              /* New argument                     */

        if ((len = wordlen(cmd)) != 0) { /* Determine argument length, any ? */
            cmd += len;                 /* Skip word                        */
            (*argc)++;                  /* One more argument                */
            if (*cmd) {                  /* Not at end of input ?            */
                *cmd++ = 0;             /* Make argument a string           */
            }
        }
    }
}

/*                      CmdDispatcher
                        =============
  Abstract:

    Dispatch a command. Scan the supplied table with commands for the
    first matching command and call the associate routine.

    Return values:
      -2: too many commands found that match 'cmd'
      -1: command not found
      0: command found in table and command handling routine called
      >0: syntax error (incorrect number of arguments), return is index+1
        in command table, so command with syntax can be displayed
*/
static char *argv[20];
static int CmdDispatcher(const Cmds *cmds, char *cmd, int cmdLen)
{
    int         argc, len, i = 1, idx;
    int         found = 0;
    const Cmds  *p0, *p;

    if ((len = wordlen(cmd)) == 0) {
        return (0);
    }

    for (p0 = cmds; p0->cmd; p0++, i++) {
        if (!strncmp(cmd, p0->cmd, len)) { /* Candidate ?                      */
            found++;
            p   = p0;
            idx = i;
        }
    }

    if (!found) {                        /* Command not found ?              */
        return -1;
    }

    if (found > 1) {                     /* Too many cmds found ?            */
        return -2;
    }

    /* Cmd found, try to execute */

                                    /* Get arguments                        */
    GetArgs(cmd, &argc, argv);
                                    /* Correct usage ?                      */
    argc--;
    if ((argc >= p->min_argc && argc <= p->max_argc) ||
        (p->min_argc == 0 && p->max_argc == -1) ||
        (argc >= p->min_argc && p->max_argc == -1) ) {
        (*p->proc)(argc + 1, argv);
        return 0;
    }
                                    /* Syntax error, return index+1 in      */
    return idx;                     /* cmd table.                           */
}

int HdlCmd(Cmds *cmdTab, char *cmd)
{
    int     cmdLen, i;
    Cmds    *p;

    cmdTable = cmdTab;

    cmdLen = strlen(cmd);

    if (!cmdLen) {                  /* Empty command ?                      */
        return 0;
    }

                                    /* Try to dispatch the cmd, check result */
    switch ((i = CmdDispatcher(cmdTab, cmd, cmdLen))) {
    case -2:                    /* Too many commands found that match   */
        Serial.printf("Too many commands found that match\n");
        return 0;

    case -1:                    /* Unknown command                      */
        break;

    case 0:                     /* Command handled                      */
        return 0;

    default:
        i--;
        p = cmdTab;
        p += i;
        Serial.printf ("Syntax error, usage: %s %s\n", p->cmd, p->syntax);
        return 0;
    }

    Serial.printf("Unknown command [%s], type <help> or <?> for command overview\n", cmd);
    return 0;
}
