#include <fs/virtual_filesystem.h>
#include <std/map.h>
#include <std/int.h>
#include <std/string.h>
#include <fs/fs.h>
#include <mem/alloc.h>

#include <shell/shell.h>

#define MAX_FILES 256

map_t mounts = NULL;
F files[MAX_FILES];

void root_init() {
	mounts = hashmap_new();
	for (int i=0; i<MAX_FILES; i++) {
		files[i].__open__ = 0;
	}
}

F *next_available_file(int *id) {
	*id = 0;
	for(int i=1; i<MAX_FILES; i++) {
		if (files[i].__open__ == 0) {
			files[i].__open__ = 1;
			*id = i;
			return &files[i];
		}
	}
	return NULL;
}


int mount(char *path, filesystem_t *fs) {
	return hashmap_put(mounts, path, fs);
}

int open(char *path, uint16_t FLAGS) {
	if (path[0] != '/') {
		return 0;
	}

	char **p = split(path, '/');

	filesystem_t *fs;
	int fid = 0;
	F *f = next_available_file(&fid);
	
	F_err (*OPEN)(F *, char *, uint16_t) = NULL;
	char *PATH;

	switch(splitlen(p)) {
		case 0: /* Empty string, already checked, so it can't actually happen */
		case 1: /* Empty string, already checked, so it can't actually happen */
			goto error;
		case 2: /* Directly open a mount point, as a directory */
			if (hashmap_get(mounts, p[1], (void **)&fs)) {
				goto error;
			}
			OPEN = fs->d_open;
			PATH = "";
			break;
		default: /* Open some file or directory inside a mount */
			if (hashmap_get(mounts, p[1], (void **)&fs)) {
				goto error;
			}
			PATH = &path[strlen(p[1]) + 1];
			switch(fs->node_type(PATH)) {
				case FILE:
					OPEN = fs->f_open;
					break;
				case DIRECTORY:
					OPEN = fs->d_open;
					break;
				default:
					goto error;
			}
	}
	if (OPEN(f, PATH, FLAGS)) {
		goto error;
	}
	f->fs = fs;
	goto success;

error:
	kprintf("failed to open file (%05i) %04s\n", splitlen(p), path);
	split_free(p);
	return 0;

success:
	split_free(p);
	return fid;
}

int read(int fd, void *dest, uint64_t bytes) {
	if (!files[fd].__open__) {
		return 0;
	}

	uint64_t read;
	if (files[fd].fs->f_read(&files[fd], dest, bytes, &read)) {
		return 0;
	}

	return read;
}

int write(int fd, void *src, uint64_t bytes) {
	if (!files[fd].__open__) {
		return 0;
	}
	uint64_t wrote;
	if (files[fd].fs->f_write(&files[fd], src, bytes, &wrote)) {
		return 0;
	}

	return wrote;
}

int close(int fd) {
	if (files[fd].__open__) {
		return files[fd].fs->f_close(&files[fd]);
	}
	return 1;
}

int mkdir(char *dir) {
	if (!dir) {
		return 0;
	}

	if (dir[0] != '/') {
		return 0;
	}

	char *parent = dirname(dir);
	if (!parent) {
		return 0;
	}

	char *child = basename(dir);
	if (!child) {
		goto kill_parent;
	}

	int fd = open(parent, 0);
	int ret = 0;
	if (!fd) {
		goto failed;
	}

	if (files[fd].fs->d_mkdir(&files[fd], child)) {
		ret = 1;
	}
	
	close(fd);

failed:
	kfree(child);

kill_parent:
	kfree(parent);
}

int scan_dir(int fd, char **name) {
	if (files[fd].__open__) {
		return files[fd].fs->d_next(&files[fd], name);
	}
	return 1;
}


int countback_to(char *s, int len, char x) {
	int ct = 0;
	while(len && s[len] != x) {
		len--;
		ct++;
	}
	return ct;
}

char *basename(char *path) {
	int s = strlen(path);
	int baselen = countback_to(path, s-1, '/');
	char *result = kcmalloc(sizeof(char), baselen+1);
	if (!result) {
		return NULL;
	}

	memcpy(result, &path[s-baselen], baselen);
	return result;
}

char *dirname(char *path) {
	int s = strlen(path);
	int baselen = countback_to(path, s-1, '/');
	char *result = kcmalloc(sizeof(char), s-baselen);
	if (!result) {
		return NULL;
	}

	memcpy(result, path, s-baselen-1);
	result[s-baselen-1] = 0;
	return result;
}