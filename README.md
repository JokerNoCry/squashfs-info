# squashfs-info

squashfs-info is a dummy squashfs superblock informations dumper "à la tune2fs -l" which work with squashfs images or block devices.

The [squashfs-tools](https://github.com/plougher/squashfs-tools) doesn't seems to currently provide a way to get all this informations in raw.

This tool is particularly useful to know the real bytes size used by the filesystem, even if the tool unsquashfs -s provides a way to display the superblock informations, it only gives a rounded value of the bytes used which is sometime not enough.

#### How it works:
```
# squashfs-info /path/to/image.squashfs
s_magic:                1936814952
inodes:                 10091
mkfs_time:              1457453227
block_size:             131072
fragments:              552
compresultsion:         3
block_log:              17
flags:                  704
no_ids:                 4
s_major:                4
s_minor:                0
root_inode:             8162970095
bytes_used:             151622805
id_table_start:         151622797
xattr_id_table_start:   18446744073709551615
inode_table_start:      151363862
directory_table_start:  151489418
fragment_table_start:   151592532
lookup_table_start:     151622699

```

```
# squashfs-info /dev/mmcblk0p1 | awk '/^bytes_used.*/ {print $2}'
151622805
```
