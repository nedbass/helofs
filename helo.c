/*
 *  helo.c - Hello world filesystem for Linux
 */
#include <linux/fs.h>
#include <linux/dcache.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/statfs.h>
#include <linux/aio.h>
#include <asm/uaccess.h>

#define HELO_BLOCK_SIZE		4096
#define HELO_BLOCKS		128

int helo_statfs(struct dentry *d, struct kstatfs *buf)
{
	buf->f_bsize = HELO_BLOCK_SIZE;
	buf->f_blocks = HELO_BLOCKS;
	return 0;
}

ssize_t helo_read(struct kiocb *iocb, const struct iovec *iov,
		  unsigned long nr_segs, loff_t pos)
{
	char *str = "Hello, world!\n";
	int len = strlen(str);

	if (pos == 0) {
		(void)copy_to_user(iov->iov_base, (void *)str, len);
		iocb->ki_pos = len;
	}
	return pos == 0 ? len : 0;
}

const struct super_operations helo_super_operations = {
	statfs:		helo_statfs,
};

const struct file_operations helo_file_operations = {
	aio_read:	helo_read,
};

struct dentry *helo_lookup(struct inode *dip, struct dentry *d,
		      struct nameidata *namei)
{
	struct inode *inode = NULL;
	inode = new_inode(dip->i_sb);
	inode->i_fop = &helo_file_operations;
	d_instantiate(d, inode);
	return NULL;
}

const struct inode_operations helo_inode_operations = {
	lookup: helo_lookup,
};

static int helo_fill_super(struct super_block *sb, void *data, int silent)
{
	struct inode *root;

	root = new_inode(sb);
	root->i_sb = sb;
	root->i_mode = S_IFDIR;
	root->i_op = &helo_inode_operations;
	root->i_fop = &simple_dir_operations;
	sb->s_op = &helo_super_operations;
	sb->s_root = d_alloc_root(root);

	return 0;
}

void helo_killsb(struct super_block *sb)
{
	kill_litter_super(sb);
}

struct dentry *helo_mount(struct file_system_type *fs_type, int flags,
			  const char *dev, void *data)
{
	return mount_nodev(fs_type, flags, NULL, helo_fill_super);
}

struct file_system_type helo_fs_type = {
	.name		= "helo",
	.mount		= helo_mount,
	.kill_sb	= helo_killsb,
};

int helo_init(void)
{
	return register_filesystem(&helo_fs_type);
}

void helo_exit(void)
{
	unregister_filesystem(&helo_fs_type);
}

module_init(helo_init);
module_exit(helo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ned Bass <nedbass@gmail.com>");
MODULE_DESCRIPTION("helo: hello world filesystem");
