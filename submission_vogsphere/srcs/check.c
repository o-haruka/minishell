#include "minishell.h"
#include <sys/stat.h>
#include <errno.h>

/*
** Check if the given path is a directory.
** Returns 1 if true, 0 if false.
*/
int	is_directory(const char *path)
{
	struct stat	statbuf;

	if (stat(path, &statbuf) != 0)
		return (0);
	return (S_ISDIR(statbuf.st_mode));
}