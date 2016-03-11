/*
 * squashfs-info - A dummy squashfs superblock information dumper
 *
 * Copyright (C) 2016 Savoir-faire Linux, Inc.
 *
 * Author:
 *   Sebastien Bourdelin <sebastien.bourdelin@gmail.com>
 *
 */

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>

#define VERSION "0.01"
#define SQUASH_MAGIC 0x73717368

/* structure taken from squashfs-tools */
struct squashfs_super_block {
	uint32_t s_magic;
	uint32_t inodes;
	uint32_t mkfs_time;
	uint32_t block_size;
	uint32_t fragments;
	uint16_t compresultsion;
	uint16_t block_log;
	uint16_t flags;
	uint16_t no_ids;
	uint16_t s_major;
	uint16_t s_minor;
	uint64_t root_inode;
	uint64_t bytes_used;
	uint64_t id_table_start;
	uint64_t xattr_id_table_start;
	uint64_t inode_table_start;
	uint64_t directory_table_start;
	uint64_t fragment_table_start;
	uint64_t lookup_table_start;
} __attribute__ ((packed));

void print_usage(void)
{
	fprintf(stderr, "usage : squashfs-info <squashfs_img|device>\n"
		"Options:\n"
		"\t-v\tversion\n"
		"\t-h\tprint this help message\n");
}

void dump_superblock(const struct squashfs_super_block *const sb) {
	fprintf(stdout, "s_magic:                %" PRIu32 "\n"
			"inodes:                 %" PRIu32 "\n"
			"mkfs_time:              %" PRIu32 "\n"
			"block_size:             %" PRIu32 "\n"
			"fragments:              %" PRIu32 "\n"
			"compresultsion:         %" PRIu16 "\n"
			"block_log:              %" PRIu16 "\n"
			"flags:                  %" PRIu16 "\n"
			"no_ids:                 %" PRIu16 "\n"
			"s_major:                %" PRIu16 "\n"
			"s_minor:                %" PRIu16 "\n"
			"root_inode:             %" PRIu64 "\n"
			"bytes_used:             %" PRIu64 "\n"
			"id_table_start:         %" PRIu64 "\n"
			"xattr_id_table_start:   %" PRIu64 "\n"
			"inode_table_start:      %" PRIu64 "\n"
			"directory_table_start:  %" PRIu64 "\n"
			"fragment_table_start:   %" PRIu64 "\n"
			"lookup_table_start:     %" PRIu64 "\n\n",
			sb->s_magic, sb->inodes, sb->mkfs_time, sb->block_size,
			sb->fragments, sb->compresultsion, sb->block_log,
			sb->flags, sb->no_ids, sb->s_major, sb->s_minor,
			sb->root_inode, sb->bytes_used, sb->id_table_start,
			sb->xattr_id_table_start, sb->inode_table_start,
			sb->directory_table_start, sb->fragment_table_start,
			sb->lookup_table_start);
}

int main(int argc, char** argv)
{
	int fd = -1;
	int err = 0;
	struct stat st;
	struct squashfs_super_block *sb = NULL;
	uint64_t img_size = 0;
	int option;

	if (argc < 2) {
		print_usage();
		return -1;
	}

	while ((option = getopt(argc, argv,"vhr")) != -1) {
		switch (option) {
		case 'v':
			fprintf(stdout, "squashfs-info version %s\n", VERSION);
			return 0;
		case 'h':
			print_usage();
			return 0;
		default:
			print_usage();
			return -1;
		}
	}

	/* Open the image */
	if ((fd = open(argv[optind], O_RDONLY)) == -1) {
		perror("open");
		err = errno;
		goto end;
	}

	/* Obtain size of the image */
	if (fstat(fd, &st) == -1) {
		perror("fstat");
		err = errno;
		goto end;
	}

	/* Is it a Block Device or a Regular File ? */
	if (S_ISBLK(st.st_mode)) {
		if (ioctl(fd, BLKGETSIZE64, &img_size) < 0) {
			perror("ioctl");
			err = errno;
			goto end;
		}
	} else if (S_ISREG(st.st_mode)) {
		img_size = st.st_size;
	} else {
		fprintf(stderr, "Error: You must specify a file or block device\n");
		err = -1;
		goto end;
	}

	/* Size must not be Null */
	if (img_size == 0) {
		fprintf(stderr, "Error: size must be greater then 0\n");
		err = -1;
		goto end;
	}

	/* Memory map the superblock */
	if ((sb = mmap(NULL, sizeof(struct squashfs_super_block), PROT_READ,
					MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
		perror("mmap");
		err = errno;
		goto end;
	}

	/* Is it a squashfs super block ? */
	if (sb->s_magic != SQUASH_MAGIC) {
		fprintf(stderr, "fatal: squashfs magic not found in image file\n");
		goto end;
	}

	dump_superblock(sb);

end:
	if (sb)
		munmap(sb, sizeof(struct squashfs_super_block));

	if (fd >= 0)
		close(fd);

	return err;
}
