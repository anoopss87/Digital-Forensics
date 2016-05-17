/*
#include "cmntdfs.h"

CHAR	_s8;
UCHAR	_u8;
INT2	_s16;
UINT2 	_u16;
INT4	_s32;
UINT4	_u32;
INT8	_s64;
UINT8	_u64;
*/
/*
 * Structure of a blocks group descriptor
 */
#ifndef SUPERBLOCK_H
#define SUPERBLOCK_H

typedef struct Superblock_t  {
   UINT4 s_inodes_count;         //!< Total number of inodes
   UINT4 s_blocks_count;         //!< Filesystem size in blocks
   UINT4 s_r_blocks_count;       /* Number of reserved blocks */
   UINT4 s_free_blocks_count;    /* Free blocks count */
   UINT4 s_free_inodes_count;    /* Free inodes count */
   UINT4 s_first_data_block;     /* First Data Block */
   UINT4 s_log_block_size;       /* Block size */
   UINT4 s_log_frag_size;        /* Fragment size */ // was s32(?)
   UINT4 s_blocks_per_group;     //!< Number of blocks per group
   UINT4 s_frags_per_group;      /* # Fragments per group */
   UINT4 s_inodes_per_group;     //!< Number of inodes per group */
   UINT4 s_mtime;                /* Mount time */
   UINT4 s_wtime;                /* Write time */
   _u16 s_mnt_count;            /* Mount count */
   _s16 s_max_mnt_count;        /* Maximal mount count */
 
   /*!\brief Magic signature
    *
        * This is the signature of the partition. The ext2 and ext3 partitions
        * have 0xEF53 at this place.
        */
   _u16 s_magic;                /* Magic signature */
   _u16 s_state;                /* File system state */
   _u16 s_errors;               /* Behaviour when detecting errors */
   _u16 s_minor_rev_level;      /* Minor revision level */
   UINT4 s_lastcheck;            /* time of last check */
   UINT4 s_checkinterval;        /* max. time between checks */
   UINT4 s_creator_os;           /* OS */
   UINT4 s_rev_level;            /* Revision level */
   _u16 s_def_resuid;           /* Default uid for reserved blocks */
   _u16 s_def_resgid;           /* Default gid for reserved blocks */

   /* These fields are for EXT2_DYNAMIC_REV superblocks only. */
   UINT4 s_first_ino;            /* First non-reserved inode */
   _u16 s_inode_size;           /* size of inode structure */
   _u16 s_block_group_nr;       /* block group # of this superblock */
   UINT4 s_feature_compat;       /* compatible feature set */
   UINT4 s_feature_incompat;     /* incompatible feature set */
   UINT4 s_feature_ro_compat;    /* readonly-compatible feature set */
/*68*/  _u8   s_uuid[16];             /* 128-bit uuid for volume */
/*78*/  char  s_volume_name[16];      /* volume name */
/*88*/  char  s_last_mounted[64];     /* directory where last mounted */
/*C8*/  UINT4  s_algorithm_usage_bitmap; /* For compression */
        /*
         * Performance hints.  Directory preallocation should only
         * happen if the EXT3_FEATURE_COMPAT_DIR_PREALLOC flag is on.
         */
        _u8   s_prealloc_blocks;      /* Nr of blocks to try to preallocate*/
        _u8   s_prealloc_dir_blocks;  /* Nr to preallocate for dirs */
        _u16  s_reserved_gdt_blocks;  /* Per group desc for online growth */
       /*
        * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
        */
/*D0*/  _u8   s_journal_uuid[16];     /* uuid of journal superblock */
/*E0*/  UINT4  s_journal_inum;         /* inode number of journal file */
        UINT4  s_journal_dev;          /* device number of journal file */
        UINT4  s_last_orphan;          /* start of list of inodes to delete */
        UINT4  s_hash_seed[4];         /* HTREE hash seed */
        _u8   s_def_hash_version;     /* Default hash version to use */
        _u8   s_reserved_char_pad;
        _u16  s_reserved_word_pad;
        UINT4  s_default_mount_opts;
        UINT4  s_first_meta_bg;        /* First metablock block group */
        UINT4  s_mkfs_time;            /* When the filesystem was created */
        UINT4  s_jnl_blocks[17];       /* Backup of the journal inode */
  /* 64bit support valid if EXT4_FEATURE_COMPAT_64BIT */
/*150*/ UINT4  s_blocks_count_hi;      /* Blocks count */
        UINT4  s_r_blocks_count_hi;    /* Reserved blocks count */
        UINT4  s_free_blocks_count_hi; /* Free blocks count */
        _u16  s_min_extra_isize;      /* All inodes have at least # bytes */
        _u16  s_want_extra_isize;     /* New inodes should reserve # bytes */
        UINT4  s_flags;                /* Miscellaneous flags */
        _u16  s_raid_stride;          /* RAID stride */
        _u16  s_mmp_interval;         /* # seconds to wait in MMP checking */
        _u64  s_mmp_block;            /* Block for multi-mount protection */
        UINT4  s_raid_stripe_width;    /* blocks on all data disks (N*stride)*/
        _u8   s_log_groups_per_flex;  /* FLEX_BG group size */
        _u8   s_reserved_char_pad2;
        _u16  s_reserved_pad;
        UINT4 s_pad[162];             /* Padding to the end of the block */
} tSuperblock  ;

#endif
