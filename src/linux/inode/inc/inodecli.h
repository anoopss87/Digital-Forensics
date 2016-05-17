/********************************************************************
 *
 * File         : inodecli.h
 * Author       : Naveen Raj Selvaraj
 * Date created : March 28, 2016
 * Description  : This file contains the constants, structures, enums and 
 *                function declarations required by the inodecmd.c
 *                In order to keep the CLI easily removable, all the MACRO,
 *                typedef and function declarations are kept in this single
 *                file instead of declaring them in multiple files. 
 *******************************************************************/
#include "readinput.h"

/*****************************************************************************/
/*                              MACRO Definitions                            */
/*****************************************************************************/
#define CLI_PROMPT "Inode Analyser# "

#define CMD_DELIMITER " "

#define CMD_HELP "help"
#define CMD_CLEAR "clear"
#define CMD_EXIT "exit"
#define CMD_WRITE  "write"
#define CMD_INODE "inode"
#define CMD_READ "read"
#define CMD_DIR "dir"
#define CMD_TREE "tree"
#define CMD_CORRUPT "corrupt"
#define CMD_FIX "uncorrupt"
#define CMD_RECOVER "recover"

#define CMD_OPT_KEY "-k"
#define CMD_OPT_NAME "-n"

#define CMD_HELP_HELP "help - display help"
#define CMD_CLEAR_HELP "clear- clear display"
#define CMD_EXIT_HELP "exit - exit the application"
#define CMD_READ_HELP "read <block number> - display the content of the data block in HEX"
#define CMD_WRITE_HELP "write <block number> <offset> <data> - writes data to a block starting at a particular offset"
#define CMD_INODE_HELP "inode <inode number> - read the inode specified by inode number"
#define CMD_DIR_HELP "dir <block number> - display the content of the data block of a directory"
#define CMD_TREE_HELP "tree [inode number] - display the directory tree starting at inode number"
#define CMD_CORRUPT_HELP "corrupt <inode number> - corrupt the data block pointer of the given inode"
#define CMD_FIX_HELP "uncorrupt <inode number> - restore the manually corrupted data block pointer of the given inode"
#define CMD_RECOVER_HELP "recover - run the recovery algorithm"

/*****************************************************************************/
/*                              Type Definitions                             */
/*****************************************************************************/
typedef struct Command_t
{
    CHAR Command[10];
    UINT4 u4CmdId;
    CHAR Options[5];
    CHAR Arg[100];
    UINT4 u4GetId;
}tCommand;

typedef struct Cmd_t
{
    const CHAR *Name;  /* Name of the command */
    const CHAR *HelpStr;  /* Help string for the command */
}tCmd;

enum
{
    CMD_ID_MIN,
    CMD_ID_HELP = CMD_ID_MIN,
    CMD_ID_CLEAR,
    CMD_ID_EXIT,
    CMD_ID_WRITE,
    CMD_ID_INODE,
    CMD_ID_READ,
    CMD_ID_DIR,
    CMD_ID_TREE,
    CMD_ID_CORRUPT,
    CMD_ID_FIX,
    CMD_ID_RECOVER,
    /* Add new command id above this line */
    CMD_ID_MAX,
    CMD_ID_INVALID = CMD_ID_MAX
};

enum
{
    CMD_ARGS_0,
    CMD_ARGS_1,
    CMD_ARGS_2
};

/*****************************************************************************/
/*                            Function Declarations                          */
/*****************************************************************************/
VOID InodeCliStart();
VOID InodeCliHandleCmd(CHAR *pCmd);
VOID InodeCliCmdAction(tCommand cmd);
INT1 InodeCliParseCmd(CHAR *pCmdStr, tCommand *pCmd);
INT1 InodeCliValidateCmd(tCommand *pCmd, UINT1 u1ArgsCount);

VOID InodeCmdReadHandler(UINT4 u4BlockNo);
VOID InodeCmdTreeHandler(UINT4 u4Inode);
