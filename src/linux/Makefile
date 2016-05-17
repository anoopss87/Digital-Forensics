#include the Product make.h
include ./make.h
.PHONY: all clean mbr gbd inode superblock gui
all:
	make -C $(BASE_DIR)/mbr
	make -C $(BASE_DIR)/gbd
	make -C $(BASE_DIR)/inode
	make -C $(BASE_DIR)/superblock
	make -C $(BASE_DIR)/gui
	cp $(BASE_DIR)/mbr/exe/* $(BASE_DIR)/bin
	cp $(BASE_DIR)/gbd/exe/* $(BASE_DIR)/bin
	cp $(BASE_DIR)/inode/exe/* $(BASE_DIR)/bin
	cp $(BASE_DIR)/superblock/exe/* $(BASE_DIR)/bin
	cp $(BASE_DIR)/gui/exe/* $(BASE_DIR)/bin

mbr:
	make -w -C $(BASE_DIR)/mbr
	cp $(BASE_DIR)/mbr/exe/* $(BASE_DIR)/bin

gbd:
	make -w -C $(BASE_DIR)/gbd
	cp $(BASE_DIR)/gbd/exe/* $(BASE_DIR)/bin

inode:
	make -C $(BASE_DIR)/inode
	cp $(BASE_DIR)/inode/exe/* $(BASE_DIR)/bin

superblock:
	make -C $(BASE_DIR)/superblock
	cp $(BASE_DIR)/superblock/exe/* $(BASE_DIR)/bin

gui:
	make -C $(BASE_DIR)/gui
	cp $(BASE_DIR)/gui/exe/* $(BASE_DIR)/bin

clean:
	make -w -C $(BASE_DIR)/mbr clean
	make -w -C $(BASE_DIR)/gbd clean
	make -w -C $(BASE_DIR)/inode clean
	make -w -C $(BASE_DIR)/superblock clean
	make -w -C $(BASE_DIR)/gui clean
	rm $(BASE_DIR)/bin/*

debug:
	make -w -C $(BASE_DIR)/common
	make -w -C $(BASE_DIR)/node
	cp $(BASE_DIR)/node/exe/* $(BASE_DIR)/bin

