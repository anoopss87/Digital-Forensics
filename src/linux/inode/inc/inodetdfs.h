/********************************************************************
 *
 * File         : inodetdfs.h
 * Author       : Naveen Raj Selvaraj
 * Date         : March 17, 2016
 * Description  : This file contains type definitions        
 *                related to inode module
 *
 *******************************************************************/
typedef enum fileFilter_e
{
    FILE_TYPE_DIRS,
    FILE_TYPE_FILES,
    FILE_TYPE_ALL
}tFileFilter;

enum
{
    FILE_TYPE_UNKNOWN = 0,
    FILE_TYPE_FILE = 1,
    FILE_TYPE_DIR = 2
};

enum
{
	INODE_CORRUPT =1,
	INODE_FIX =2
};
