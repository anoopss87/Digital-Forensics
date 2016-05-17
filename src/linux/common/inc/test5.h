typedef signed char _s8;
typedef unsigned char _u8;
typedef unsigned char byte;

typedef signed short _s16;
typedef unsigned short _u16;

typedef signed int _s32;
typedef unsigned int _u32;

typedef long long _s64;
typedef unsigned long long _u64;

/*
 * Structure of a blocks group descriptor
*/
 typedef struct
 {
         _u32  bg_block_bitmap;                /* Blocks bitmap block */
         _u32  bg_inode_bitmap;                /* Inodes bitmap block */
         _u32  bg_inode_table;         /* Inodes table block */
         _u16  bg_free_blocks_count;   /* Free blocks count */
         _u16  bg_free_inodes_count;   /* Free inodes count */
         _u16  bg_used_dirs_count;     /* Directories count */
         _u16   bg_pad;
         _u32  bg_reserved[3];
 } ext2_group_desc_t;


typedef struct
{
   _u32 s_inodes_count;         //!< Total number of inodes
   _u32 s_blocks_count;         //!< Filesystem size in blocks
   _u32 s_r_blocks_count;       /* Number of reserved blocks */
   _u32 s_free_blocks_count;    /* Free blocks count */
   _u32 s_free_inodes_count;    /* Free inodes count */
   _u32 s_first_data_block;     /* First Data Block */
   _u32 s_log_block_size;       /* Block size */
   _s32 s_log_frag_size;        /* Fragment size */
   _u32 s_blocks_per_group;     //!< Number of blocks per group
   _u32 s_frags_per_group;      /* # Fragments per group */
   _u32 s_inodes_per_group;     //!< Number of inodes per group */
   _u32 s_mtime;                /* Mount time */
   _u32 s_wtime;                /* Write time */
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
   _u32 s_lastcheck;            /* time of last check */
   _u32 s_checkinterval;        /* max. time between checks */
   _u32 s_creator_os;           /* OS */
   _u32 s_rev_level;            /* Revision level */
   _u16 s_def_resuid;           /* Default uid for reserved blocks */
   _u16 s_def_resgid;           /* Default gid for reserved blocks */

   /* These fields are for EXT2_DYNAMIC_REV superblocks only. */
   _u32 s_first_ino;            /* First non-reserved inode */
   _u16 s_inode_size;           /* size of inode structure */
   _u16 s_block_group_nr;       /* block group # of this superblock */
   _u32 s_feature_compat;       /* compatible feature set */
   _u32 s_feature_incompat;     /* incompatible feature set */
   _u32 s_feature_ro_compat;    /* readonly-compatible feature set */
/*68*/  _u8    s_uuid[16];             /* 128-bit uuid for volume */
/*78*/  char    s_volume_name[16];      /* volume name */
/*88*/  char    s_last_mounted[64];     /* directory where last mounted */
/*C8*/  _u32  s_algorithm_usage_bitmap; /* For compression */
        /*
         * Performance hints.  Directory preallocation should only
         * happen if the EXT3_FEATURE_COMPAT_DIR_PREALLOC flag is on.
         */
        _u8    s_prealloc_blocks;      /* Nr of blocks to try to preallocate*/
        _u8    s_prealloc_dir_blocks;  /* Nr to preallocate for dirs */
        _u16  s_reserved_gdt_blocks;  /* Per group desc for online growth */
       /*
        * Journaling support valid if EXT3_FEATURE_COMPAT_HAS_JOURNAL set.
        */
/*D0*/  _u8    s_journal_uuid[16];     /* uuid of journal superblock */
/*E0*/  _u32  s_journal_inum;         /* inode number of journal file */
        _u32  s_journal_dev;          /* device number of journal file */
        _u32  s_last_orphan;          /* start of list of inodes to delete */
        _u32  s_hash_seed[4];         /* HTREE hash seed */
        _u8    s_def_hash_version;     /* Default hash version to use */
        _u8    s_reserved_char_pad;
        _u16   s_reserved_word_pad;
        _u32  s_default_mount_opts;
        _u32  s_first_meta_bg;        /* First metablock block group */
        _u32  s_mkfs_time;            /* When the filesystem was created */
        _u32  s_jnl_blocks[17];       /* Backup of the journal inode */
  /* 64bit support valid if EXT4_FEATURE_COMPAT_64BIT */
/*150*/ _u32  s_blocks_count_hi;      /* Blocks count */
        _u32  s_r_blocks_count_hi;    /* Reserved blocks count */
        _u32  s_free_blocks_count_hi; /* Free blocks count */
        _u16  s_min_extra_isize;      /* All inodes have at least # bytes */
        _u16  s_want_extra_isize;     /* New inodes should reserve # bytes */
        _u32  s_flags;                /* Miscellaneous flags */
        _u16  s_raid_stride;          /* RAID stride */
        _u16  s_mmp_interval;         /* # seconds to wait in MMP checking */
        _u64  s_mmp_block;            /* Block for multi-mount protection */
        _u32  s_raid_stripe_width;    /* blocks on all data disks (N*stride)*/
        _u8    s_log_groups_per_flex;  /* FLEX_BG group size */
        _u8    s_reserved_char_pad2;
        _u16  s_reserved_pad;
        _u32 s_pad[162];             /* Padding to the end of the block */
}
super_block_t;
