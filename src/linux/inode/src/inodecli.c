/*******************************************************************
 *
 * File         : inodecli.c
 * Author       : Naveen Raj Selvaraj
 * Date created : March 28, 2016
 * Description  : This file contains the functions related to the CLI Module
 *
 *******************************************************************/
#include "inodeinc.h"
#include "inodecli.h"

/* List of CLI commands */
tCmd gCommands[CMD_ID_MAX] = {
                {.Name = CMD_HELP, .HelpStr = CMD_HELP_HELP},
                {.Name = CMD_CLEAR, .HelpStr = CMD_CLEAR_HELP},
                {.Name = CMD_EXIT, .HelpStr = CMD_EXIT_HELP},
                {.Name = CMD_WRITE, .HelpStr = CMD_WRITE_HELP},
                {.Name = CMD_INODE, .HelpStr = CMD_INODE_HELP},
                {.Name = CMD_READ, .HelpStr = CMD_READ_HELP},
                {.Name = CMD_DIR, .HelpStr = CMD_DIR_HELP},
                {.Name = CMD_TREE, .HelpStr = CMD_TREE_HELP},
                {.Name = CMD_CORRUPT, .HelpStr = CMD_CORRUPT_HELP},
                {.Name = CMD_FIX, .HelpStr = CMD_FIX_HELP},
                {.Name = CMD_RECOVER, .HelpStr = CMD_RECOVER_HELP},
                /* Add a new command above this line and add a command ID in inodecli.h */
                };
UINT1 gu1Exit;  /* flag to check CLI exit */

/*! *************************************************************************
 * Function Name : InodeCliStart 
 *
 * Description   : This Function is used to start the command line interface.
 *                 This function reads the command input from the user and 
 *                 initiates the action for the command.
 *
 * Input         : None 
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID InodeCliStart()
{
    gu1Exit = FALSE;
    CHAR Input[256];
    do
    {
        memset(Input, 0, sizeof(Input));
        INT1 i1ReadStatus = getLine(CLI_PROMPT, Input, sizeof(Input));
        if (i1ReadStatus != NO_INPUT)
        {
            if (strlen(Input) != 0)
            {
                InodeCliHandleCmd(Input);
            }
        }
    } while (gu1Exit == FALSE);
}

/*! *************************************************************************
 * Function Name : InodeCliHandle
 *
 * Description   : This Function tries to parse the command input and if it
 *                 succeeds then calls the high level action handler function 
 *                 with the parsed command.
 *
 * Input         : pCmd - Command string input from the user
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID InodeCliHandleCmd(CHAR *pCmd)
{
    if (pCmd == NULL)
        return;
    tCommand cmd;
    memset(&cmd, 0, sizeof(cmd));
    if (InodeCliParseCmd(pCmd, &cmd) == TRUE)
    {
        InodeCliCmdAction(cmd);
    }
}

/*! *************************************************************************
 * Function Name : InodeCliCmdAction 
 *
 * Description   : This Function is the overall action handler. This function
 *                 identifies the command and the action to be taken for the 
 *                 command.
 *
 * Input         : cmd - structure containing the parsed command and arguments
 *
 * Output        : None
 * 
 * Returns       : None
 *
 **************************************************************************/
VOID InodeCliCmdAction(tCommand cmd)
{
    UINT4 u4InodeNo;
    UINT4 u4Index = 0;

    /* TODO this switch case can be avoided/optimized by having a function
     * pointer in tCmd */
    switch (cmd.u4CmdId)
    {
        case CMD_ID_HELP:
            for (u4Index = CMD_ID_MIN; u4Index < CMD_ID_MAX; u4Index++)
            {
                printf(" %s\n", gCommands[u4Index].HelpStr);
            }
            break;

        case CMD_ID_EXIT:
            gu1Exit = TRUE;
            break;

        case CMD_ID_CLEAR:
            /* Clear the screen */
            printf("\033[2J\033[1;1H");
            break;

        case CMD_ID_TREE:
            u4InodeNo = atoi(cmd.Arg);
            InodeCmdTreeHandler(u4InodeNo);
            break;

        case CMD_ID_INODE:
            u4InodeNo = atoi(cmd.Arg);
            InodeUtilPrintInode(u4InodeNo);
            break;

        case CMD_ID_DIR:
            u4InodeNo = atoi(cmd.Arg);
            InodeUtilPrintDirDataBlock(u4InodeNo);
            break;

        case CMD_ID_READ:
            u4InodeNo = atoi(cmd.Arg);
            InodeCmdReadHandler(u4InodeNo);
            break;

        case CMD_ID_CORRUPT: 
            u4InodeNo = atoi(cmd.Arg);
            InodeUtilCorruptAndRecover(u4InodeNo, INODE_CORRUPT);
            break;

        case CMD_ID_FIX: 
            u4InodeNo = atoi(cmd.Arg);
            InodeUtilCorruptAndRecover(u4InodeNo, INODE_FIX);
            break;

        case CMD_ID_RECOVER:
            InodeFixExecute();
            break;

        default:
            printf(" Command currently not supported ! Name: %s Options: %s Arg: %s\n",
                    cmd.Command, cmd.Options, cmd.Arg);
    }
}

/*! *************************************************************************
 * Function Name : InodeCliParseCmd
 *
 * Description   : This Function is used to parse the input command string into
 *                 the command and arguments.
 *
 * Input         : pCmdStr - input command string
 *
 * Output        : pCmd - structure containing parsed command and arguments 
 * 
 * Returns       : TRUE, if parsing and validation succeeds 
 *                 FALSE otherwise
 * 
 **************************************************************************/
INT1 InodeCliParseCmd(CHAR *pCmdStr, tCommand *pCmd)
{
    CHAR *pToken;
    UINT1 u1ArgC;
    UINT4 u4Index;

    /* get the first token from the command string */
    pToken = strtok(pCmdStr, CMD_DELIMITER);
    u1ArgC = CMD_ARGS_0;

    strcpy(pCmd->Command, pToken);
    pCmd->u4CmdId = CMD_ID_INVALID;

    /* Find the matching command from the available command list */
    for (u4Index = CMD_ID_MIN; u4Index < CMD_ID_MAX; u4Index++)
    {
        if (strcmp(pCmd->Command, gCommands[u4Index].Name) == 0)
        {
            pCmd->u4CmdId = u4Index;
        }
    }
    /* If there is no matching command, retrun error */
    if (pCmd->u4CmdId == CMD_ID_INVALID)
    {
        printf(" Invalid Command ! Try 'help' for available commands\n");
        return FALSE;
    }

    /* walk through other tokens */
    while(pToken != NULL) 
    {   
        switch(u1ArgC)
        {
            case CMD_ARGS_1:
                switch (pCmd->u4CmdId)
                {
                    case CMD_ID_WRITE:
                    case CMD_ID_INODE:
                    case CMD_ID_DIR:
                    case CMD_ID_TREE:
                    case CMD_ID_CORRUPT:
                    case CMD_ID_READ:
                    case CMD_ID_FIX:
                        strcpy(pCmd->Arg, pToken);
                        break;
                }
                break;
        }
        pToken = strtok(NULL, CMD_DELIMITER);
        u1ArgC++;
    }
    return InodeCliValidateCmd(pCmd, u1ArgC);
}

/*! *************************************************************************
 * Function Name : InodeCliValidateCmd
 *
 * Description   : This Function is used to validate the parsed command and 
 *                 arguments
 *
 * Input         : pCmd     - pointer to the parsed command structure
 *                 u1ArgC   - no. of arguments in the command input 
 *
 * Output        : None 
 * 
 * Returns       : TRUE, if validation succeeds 
 *                 FALSE otherwise
 * 
 **************************************************************************/
INT1 InodeCliValidateCmd(tCommand *pCmd, UINT1 u1ArgC)
{
    INT1 isValid = TRUE;
    switch (pCmd->u4CmdId)
    {
        case CMD_ID_HELP:
        case CMD_ID_CLEAR:
        case CMD_ID_EXIT:
        case CMD_ID_RECOVER:
            if (u1ArgC != 1)
            {
                printf(" Invalid no. of Arguments !\n Usage: %s\n",
                        gCommands[pCmd->u4CmdId].HelpStr);
                isValid = FALSE;
            }
            break;
        case CMD_ID_INODE:
        case CMD_ID_READ:
        case CMD_ID_DIR:
        case CMD_ID_CORRUPT:
        case CMD_ID_FIX:
        case CMD_ID_TREE:
            /* Validate the number of arguments */
            if ((pCmd->u4CmdId == CMD_ID_TREE) &&
                    (u1ArgC > 2))
            {
                printf(" Invalid no. of Arguments !\n Usage: %s\n", CMD_TREE_HELP);
                isValid = FALSE;
            }
            else if (((pCmd->u4CmdId != CMD_ID_TREE) &&
                        u1ArgC != 2))
            {
                printf(" Invalid no. of Arguments !\n Usage: %s\n",
                        gCommands[pCmd->u4CmdId].HelpStr);
                isValid = FALSE;
            }
            if (isValid == TRUE)
            {
                /* Command has valid no. of arguments, now validate the input */
                int i = 0;
                for (i = 0; pCmd->Arg[i]; i++)
                {
                    if (!isdigit(pCmd->Arg[i]))
                    {
                        printf(" Invalid Argument: Only Integer values accepted !\n");
                        isValid = FALSE;
                        break;
                    }
                }
            }
            break;
        case CMD_ID_WRITE:
            if (u1ArgC != 4)
            {
                printf(" Invalid Arguments !\nUsage: %s\n", CMD_WRITE_HELP);
                isValid = FALSE;
            }
            /* TODO 1. parse the arg to obtain block no, offset and data.
                    2. validate block no and offset for integer input.
                    3. validate data for length. 80 is a preferable length */
#if 0
            else if (strlen(pCmd->Arg) > MAX_LINE_LENGTH)
            {
               printf(" ERROR: Data can be 80 characters maximum\n");
            }
#endif
            break;

        default:
            pCmd->u4CmdId = CMD_ID_INVALID;
            isValid = FALSE;
    }
    return isValid;
}
