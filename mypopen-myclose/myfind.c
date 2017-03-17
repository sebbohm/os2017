/**
* @file myfind.c
* Betriebssysteme myfind File.
* Beispiel 1
*
* @author Alexander Pirka <ic16b031@technikum-wien.at>
* @author Maria Kanikova <ic16b002@technikum-wien.at>
* @author Christian Fuhry <ic16b055@technikum-wien.at>
* @author Sebastian Boehm <ic16b032@technikum-wien.at>
* @date 2017/03/17
*
* @version 2
*
* @todo God help us all!
*
*/

/*
* -------------------------------------------------------------- includes --
*/

#include <stdio.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <grp.h>
#include <time.h>
#include <fnmatch.h>
#include <libgen.h>

/*
* --------------------------------------------------------------- defines --
*/

/*
* -------------------------------------------------------------- typedefs --
*/
# define BUFFER 80
/*
* --------------------------------------------------------------- globals --
*/

/*
* ------------------------------------------------------------- functions --
*/


static int do_check(const char* const* parms);

static void do_file(const char* file_name, const char* const* parms);
static void do_dir(const char* dir_name, const char* const* parms);

static int do_name(const char* file_name, const char* parms);
static int do_type(const char* parms, const struct stat buf);
static int do_path(const char* file_name, const char *parms);
static int do_userOrGroup(const char * userparms, const struct stat buf, char *userOrGroup);
static int do_no_userOrGroup(const char* file_name, const char* const* parms, const struct stat buf, char *userOrGroup);

static void do_print(const char* file_name);
static void do_ls_print(const char* file_name, const char* const* parms, const struct stat buf);

static void do_usage_print(const char* const* parms);


/**
*
* \brief The start of myfind
*
* This is the main entry point for this myfind C program.
* Checks if sufficent ammount of parms have been entered.
*
* \param argc the number of arguments
* \param argv the arguments itselves (including the program name in argv[0])
*
* \return 0 if successful, 1 if erroneous
* \retval EXIT_SUCCESS if successful
* \retval EXIT_FAILURE if erroneous
*
*/
int main(int argc, const char *argv[])
{
	if (argc < 2)
	{
		do_usage_print(argv);
		return EXIT_FAILURE;
	}
	else
	{
		if (do_check(argv) == 0)
		{
			do_file(argv[1], argv);
		}
		else
		{
			do_usage_print(argv);
			exit(EXIT_FAILURE);
		}
	}
	if (fflush(stdout) == EOF)
	{
		fprintf(stderr, "Unable to flush stdout!: %s\n", strerror(errno));
	}
	return EXIT_SUCCESS;
}

/**
*
* \brief parameter_check compares entered parms with set parms
*
* This function compares the arguments entered with the set parms.
* If returned unsuccessful, usage_print shall be called.
*
* \param parms is list of parms typed as parms of function
*
* \return 0 if successful 1 if unsuccessful
*
*/
static int do_check(const char* const* parms)
{
	int offset = 2;

	while (parms[offset] != NULL)
	{
		if (strcmp(parms[offset], "-user") == 0 ||
			strcmp(parms[offset], "-name") == 0 ||
			strcmp(parms[offset], "-type") == 0 ||
			strcmp(parms[offset], "-path") == 0 ||
			strcmp(parms[offset], "-group") == 0)
		{
			if (parms[offset + 1] == NULL)
			{
				printf("%s: missing argument to `%s'\n", *parms, strerror(errno));
				return EXIT_FAILURE;
			}
			offset += 2;
		}
		else if (strcmp(parms[offset], "-print") == 0 ||
			strcmp(parms[offset], "-ls") == 0 ||
			strcmp(parms[offset], "-nouser") == 0 ||
			strcmp(parms[offset], "-nogroup") == 0)
		{
			offset += 1;
		}
		else
		{
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

/**
* \brief do_file compares parms with set parms and prints if corresponding
*
* Compares the arguments/parms with the parms defined in the function.
* Prints the file name, directory as set in the parameter functions.
* If file_name is a directory the function calls do_dir.
*
* \param file_name name of directory or file
* \param parms pointer to start of parms
*
* \return nada
*
*/
static void do_file(const char* file_name, const char* const* parms)
{
	struct stat buf; //metadata (attribute)
	int offset = 1; //helper variable to choose array element
	int printed = 0;
	int match = 0;

	if (lstat(file_name, &buf) == -1)
	{
		fprintf(stderr, "%s: unable to read lstat '%s' - %s\n", *parms, file_name, strerror(errno));
		return;
	}
	while (parms[offset] != NULL)
	{
		if (strcmp(parms[offset], "-name") == 0)
		{
			if (parms[offset + 1] != NULL)
			{
				match = do_name(file_name, parms[offset + 1]);
				offset++;
			}
			else
			{
				fprintf(stderr, "%s: -name needs another argument `%s'\n", file_name, *parms);
				exit(EXIT_FAILURE);
			} 
		}
		else if (strcmp(parms[offset], "-type") == 0)
		{
			if (parms[offset + 1] != NULL)
			{
				match = do_type(parms[offset + 1], buf);
				offset++;
			}
			else
			{
				fprintf(stderr, "%s: -type needs another argument `%s'\n", file_name, *parms);
				exit(EXIT_FAILURE);
			}
		}
		else if (strcmp(parms[offset], "-path") == 0)
		{

			if (parms[offset + 1] != NULL)
			{
				match = do_path(file_name, parms[offset + 1]);
				offset++;
			}
			else
			{
				fprintf(stderr, "%s: -path needs another argument `%s'\n", file_name, *parms);
				exit(EXIT_FAILURE);
			}
		}
		else if (strcmp(parms[offset], "-user") == 0)
		{

			if (parms[offset + 1] != NULL)
			{
				match = do_userOrGroup(parms[offset + 1], buf, "user");
				offset++;
			}
			else
			{
				fprintf(stderr, "%s: -user needs another argument `%s'\n", file_name, *parms);
				exit(EXIT_FAILURE);
			}
		}
		else if (strcmp(parms[offset], "-group") == 0)
		{

			if (parms[offset + 1] != NULL)
			{
				match = do_userOrGroup(parms[offset + 1], buf, "group");
				offset++;
			}
			else
			{
				fprintf(stderr, "%s: -group needs another argument `%s'\n", file_name, *parms);
				exit(EXIT_FAILURE);
			}
		}
		else if (strcmp(parms[offset], "-nouser") == 0)
		{
			match = do_no_userOrGroup(file_name, parms, buf, "user");		
		}
		else if (strcmp(parms[offset], "-nogroup") == 0)
		{
			match = do_no_userOrGroup(file_name, parms, buf, "group");			
		}
		else if (strcmp(parms[offset], "-print") == 0)
		{
			do_print(file_name);
			printed = 1;			
		}
		else if (strcmp(parms[offset], "-ls") == 0)
		{
			do_ls_print(file_name, parms, buf);
			printed = 1;			
		}
		else if (((parms[1]) != NULL) && ((parms[2]) == NULL) && (match == 0))
		{
			match = 1;
		}
		offset++;
	}
	if (match == 1 && printed != 1)
	{
		do_print(file_name);
		printed = 1;
		match = 0;
	}

	if (S_ISDIR(buf.st_mode)) //checks if file is a directory
	{
		do_dir(file_name, parms);
	}
}

/**
* \brief do_dir works through directory and opens do_file for files
*
* Reads the entries (via dirent) of the given directory and calls do_file for every entry.
* Adds / at the end of directory name if missing to get full path name.
*
* \param file_name name of directory
* \param parms pointer to start of parms
*
* \return nothing babe
*
*/
static void do_dir(const char* dir_name, const char* const* parms)
{
	DIR *dirp = NULL;
	const struct dirent *dirent;
	int offset = 1; //helper variable to choose array element

	dirp = opendir(dir_name);
	if (dirp == NULL)
	{
		fprintf(stderr, "%s: can't open directory `%s'\n", *parms, strerror(errno));
		return;
	}

	errno = 0;					//reset errno

	while ((dirent = readdir(dirp)) != NULL)
	{
		if (errno != 0)
		{
			fprintf(stderr, "%s: can't read directory `%s'\n", *parms, strerror(errno));
			errno = 0;			//reset errno
			continue;
		}

		if (strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0)
		{
			char tempPath[strlen(dir_name) + strlen(dirent->d_name)];
			if (dir_name[strlen(dir_name) - 1] != '/')
			{
				strcpy(tempPath, dir_name);
				strcat(tempPath, "/");
			}
			strcat(tempPath, dirent->d_name);
			do_file(tempPath, parms);
		}
		offset++;
	}
	int closedir(DIR *dirp);

	if (closedir(dirp) != 0)
	{
		fprintf(stderr, "%s: can't close directory `%s'\n", *parms, strerror(errno));
		return;
	}

}

/**
*
* \brief do_name compares file name with argument.
*
* Compares enteres parameter with presented file name.
*
* \param file_name name of the file to compare with
* \param parms argument for -name
*
* \return 1 if successful 0 if unsuccessful
* \retval 1 if successful and print needed
* \retval 0 if no comparison but no error
*
*/
static int do_name(const char* file_name, const char* parms)
{
	char temp_name[strlen(file_name) + 1]; // basename() may modify the contents of path, so it may be desirable to pass a copy when calling one of these functions.
	char* base = NULL;
	strcpy(temp_name, file_name);
	base = basename(temp_name);
	
	int match_name = fnmatch(parms, base, FNM_NOESCAPE);

	if (match_name == 0)
	{
		return 1;
	}
	else if (match_name == FNM_NOMATCH)
	{
		exit(EXIT_SUCCESS);
	}
	else if (match_name != 0) //nonzero value if there is an error
		exit(EXIT_FAILURE);

	exit(EXIT_SUCCESS);
}

/**
*
* \brief do_type compares files with selected input with the file system.
*
* This function compares the file_name before "-type" and
* the argument after "-type" with the file system. (parms)
* Allowed arguments after "-type":
* <b>	 if the file is a block special file (a device like a disk)
* <c>	 if the file is a character special file
* <d>	 if the file is a directory
* <p>	 if the file is a FIFO special file, or a pipe
* <f>	 if the file is a regular file
* <l>	 if the file is a symbolic link
* <s>	 if the file is a socket (generalized interprocess communication channel)
* It returns 1 if the comparation is successful and 0 if unsuccessful.
*
*
* \param parms needed for matching with the file system.
* \param stat buf is the buffer with the metadata created by lstat.
*
* return 1 if successful 0 if unsuccessful
*/
static int do_type(const char* parms, const struct stat buf)
{
	int match = -1;

	if (strcmp(parms, "b") == 0) match = S_ISBLK(buf.st_mode);
	else if (strcmp(parms, "c") == 0) match = S_ISCHR(buf.st_mode);
	else if (strcmp(parms, "d") == 0) match = S_ISDIR(buf.st_mode);
	else if (strcmp(parms, "p") == 0) match = S_ISFIFO(buf.st_mode);
	else if (strcmp(parms, "f") == 0) match = S_ISREG(buf.st_mode);
	else if (strcmp(parms, "l") == 0) match = S_ISLNK(buf.st_mode);
	else if (strcmp(parms, "s") == 0) match = S_ISSOCK(buf.st_mode);
	else
	{
		fprintf(stderr, "%s: unknown argument\n", parms);
		exit(EXIT_FAILURE);
	}
	if (match == 0)
	{
		exit(EXIT_SUCCESS);
	}

	return 1;
}

/**
*
* \brief do_path compares files with a path with the file system.
* This function compares the file_name before "-path" and
* the argument after "-path" with the file system. (parms)
* It returns 1 if the comparation is successful and 0 if unsuccessful.
* int fnmatch(const char *pattern, const char *string, int flags);
* Flag FNM_NOESCAPE > treat backslash as an ordinary character*
*
* \param file_name is the name of the path to be checked.
* \param parms needed for matching with the file system.
*
* return 1 if successful 0 if unsuccessful
*/
static int do_path(const char* file_name, const char *parms)
{
	int match = -1;

	match = fnmatch(parms, file_name, FNM_NOESCAPE);

	if (match != 0)
	{
		exit(EXIT_SUCCESS);
	}
	return 1;
}

/**
*
* \brief do_comp_userOrGroup compares parameter with uid/gid and uname/grpname
*
* This function compares the argument after "-user" od "-group" with the uid/gid of the file.
* It returns 1 if the comparation is successful and 0 if unsuccessful.
*
* \param userparms is the argument typed by the user after -user or -group.
* \param stat buf is the buffer with the metadata created by lstat.
* \param userOrGroup hardcoded to distinguish betweed -user and -group search action.
*
* \return 1 if successful 0 if unsuccessful
* \retval 1 if successful and print needed
* \retval 0 if no comparison but no error
*
*/
static int do_userOrGroup(const char * userparms, const struct stat buf, char *userOrGroup)
{
	struct passwd *pwd = NULL;
	char *endptr = NULL;
	unsigned int id = 0;
	pwd = getpwnam(userparms);
	errno = 0;

	if (pwd != NULL)
	{
		if (strcmp(userOrGroup, "user") == 0)
		{
			if (pwd->pw_uid == buf.st_uid)
			{
				return 1;
			}
		}
		else if (strcmp(userOrGroup, "group") == 0)
		{
			if (pwd->pw_gid == buf.st_gid)
			{
				return 1;
			}
		}
	}
	else
	{
		if (errno != 0)
		{
			exit(EXIT_FAILURE);
		}
		if (strcmp(userOrGroup, "user") == 0)
		{
			id = strtol(userparms, &endptr, 10);
			if (strcmp(endptr, "/0") == 0)
			{
				exit(EXIT_FAILURE); //strtol couldnt finish converting}
			}
			if (userparms != endptr)
			{
				if (buf.st_uid == id)
				{
					return 1;
				}
			}
			else
			{
				fprintf(stderr, "myfind: %s is not the name of a known %s\n", userparms, userOrGroup);
				exit(EXIT_FAILURE);
			}
		}
		else if (strcmp(userOrGroup, "group") == 0)
		{
			id = strtol(userparms, &endptr, 10);
			if (strcmp(endptr, "/0") == 0)
			{
				exit(EXIT_FAILURE); //strtol couldnt finish converting}
			}
			if (userparms != endptr)
			{
				if (buf.st_gid == id)
				{
					return 1;
				}
			}
			else
			{
				fprintf(stderr, "myfind: %s is not the name of a known %s\n", userparms, userOrGroup);
				exit(EXIT_FAILURE);
			}
		}
	}
	exit(EXIT_SUCCESS);
}

/**
*
* \brief do_no_userOrGroup checks if a file is user- or groupless
*
* This function checks if a file or directory currently has a user or group assigned.
* It returns 1 if the user or group parameter is not set in file.
*
* \param file_name is the name of the file to be checked.
* \param parameter needed for error handling.
* \param stat buf is the buffer with the metadata created by lstat.
* \param userOrGroup hardcoded to distinguish betweed -nouser and -nogroup  action.
*
* \return 1 if user- or groupless 0 if user or group present
* \retval 1 if successful and print needed
* \retval 0 if no comparison but no error
*
*/
static int do_no_userOrGroup(const char* file_name, const char* const* parms, const struct stat buf, char *userOrGroup)
{
	const struct passwd *pwd = NULL;
	const struct group *gid = NULL;
	errno = 0;
	if (strcmp(userOrGroup, "user") == 0)
	{
		pwd = getpwuid(buf.st_uid);
		if ((pwd == NULL) && (errno == 0))
		{
			return 1;
		}
		else if (errno != 0)
		{
			fprintf(stderr, "myfind: %s uid %s is not known %s\n", *parms, file_name, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	else if (strcmp(userOrGroup, "group") == 0)
	{
		gid = getgrgid(buf.st_gid);
		if ((gid == NULL) && (errno == 0))
		{
			return 1;
		}
		else if (errno != 0)
		{
			fprintf(stderr, "myfind: %s gid %s is not known %s\n", *parms, file_name, strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	exit(EXIT_SUCCESS);
}

/**
*
* \brief do_print prints the all matches.
*
* Prints files or directory on the standard output.
*
* \param file_name Name of the file as defined by the calling function.
*
* \return njet
*
*/
static void do_print(const char* file_name)
{
	if (printf("%s\n", file_name) < 0)
	{
		printf("do_comp_print error");
	}
}

/**
*
* \This funktion list current file in ls -dils format on standard output.
*
* The block counts are of 1K blocks, unless the environment variable POSIXLY_CORRECT is set, in which case 512-byte blocks are used.
* char *getenv(const char *name) searches for the environment string pointed to by name and returns the associated value to the string.
* Acronym: P ortable O perating S ystem I nterface UniX
*
* strftime - format date and time
*  %b The abbreviated month name according to the current locale. (Calculated from tm_mon.)
*  %e Like %d, the day of the month as a decimal number, but a leading zero is replaced by a space.
*
*
* checks:
* <->	 if the file is a regular file
* <d>	 if the file is a directory
* <c>	 if the file is a character special file
* <b>	 if the file is a block special file (a device like a disk)
* <p>	 if the file is a FIFO special file, or a pipe
* <l>	 if the file is a symbolic link
* <s>	 if the file is a socket (generalized interprocess communication channel)
*
* <buf.st_mode & S_IRUSR>							'r' user readable
* <buf.st_mode & S_IWUSR>							'w' user writeable
* <buf.st_mode & S_IXUSR && !buf.st_mode & S_ISUID>	'x' user executable without sticky
* <buf.st_mode & S_IXUSR>							's' user executable
* <buf.st_mode & S_ISUID>							'S' user not executable with sticky
* <buf.st_mode & S_IRGRP>							'r' group readable
* <buf.st_mode & S_IWGRP>							'w' group writeable
* <buf.st_mode & S_IXGRP && !buf.st_mode & S_ISGID>	'x' group executable without sticky
* <buf.st_mode & S_IXGRP>							's' group executable
* <buf.st_mode & S_ISGID>							'S' group not executable with sticky
* <buf.st_mode & S_IROTH>							'r' others readable
* <buf.st_mode & S_IWOTH>							'w' others writeable
* <buf.st_mode & S_IXOTH && !buf.st_mode & S_ISVTX>	'x' others executable without sticky
* <buf.st_mode & S_IXOTH>							't' others executable
* <buf.st_mode & S_ISVTX>							'T' others save swapped test after use (sticky)
*
* \param file_name is the name to print.
* \param parms needed for error handling.
* \param stat buf is the buffer with the metadata created by lstat.
*/
static void do_ls_print(const char* file_name, const char* const* parms, const struct stat buf)
{
	const struct passwd * user = NULL;
	const struct group* group = NULL;
	const  time_t* time = &(buf.st_mtime);

	unsigned int blocks = 0;
	char mode[] = { "?---------" };

	int symb_link_length = 0;
	char do_name[strlen(file_name) + buf.st_size + 1];
	char symb_link_string[buf.st_size];
	char arrow[] = { " -> " };

	errno = 0;

	char* do_user = "";
	char* do_group = "";

	char uid[BUFFER] = { 0 };
	char gid[BUFFER] = { 0 };

	char do_time[BUFFER] = { 0 };

	if (S_ISREG(buf.st_mode))									mode[0] = '-';
	else if (S_ISDIR(buf.st_mode))									mode[0] = 'd';
	else if (S_ISCHR(buf.st_mode))									mode[0] = 'c';
	else if (S_ISBLK(buf.st_mode))									mode[0] = 'b';
	else if (S_ISFIFO(buf.st_mode))									mode[0] = 'f';
	else if (S_ISLNK(buf.st_mode))									mode[0] = 'l';
	else if (S_ISSOCK(buf.st_mode))									mode[0] = 's';
	else															mode[0] = '?';


	if (buf.st_mode & S_IRUSR)									mode[1] = 'r';
	if (buf.st_mode & S_IWUSR)									mode[2] = 'w';
	if ((buf.st_mode & S_IXUSR) && !(buf.st_mode & S_ISUID))	mode[3] = 'x';
	else if (buf.st_mode & S_IXUSR)									mode[3] = 's';
	else if (buf.st_mode & S_ISUID)									mode[3] = 'S';

	if (buf.st_mode & S_IRGRP)									mode[4] = 'r';
	if (buf.st_mode & S_IWGRP)									mode[5] = 'w';
	if ((buf.st_mode & S_IXGRP) && !(buf.st_mode & S_ISGID))	mode[6] = 'x';
	else if (buf.st_mode & S_IXGRP)									mode[6] = 's';
	else if (buf.st_mode & S_ISGID)									mode[6] = 'S';

	if (buf.st_mode & S_IROTH)									mode[7] = 'r';
	if (buf.st_mode & S_IWOTH)									mode[8] = 'w';
	if ((buf.st_mode & S_IXOTH) && !(buf.st_mode & S_ISVTX))	mode[9] = 'x';
	else if (buf.st_mode & S_IXOTH)									mode[9] = 't';
	else if (buf.st_mode & S_ISVTX)									mode[9] = 'T';

	strcpy(do_name, file_name);

	if (mode[0] != 'l')
	{
		blocks = (unsigned int)buf.st_blocks;

		if (getenv("POSIXLY_CORRECT") == NULL)
		{
			blocks = (unsigned int)buf.st_blocks;
			blocks = blocks / 2 + buf.st_blocks % 2;
		}
	}

	if (mode[0] == 'l')
	{
		symb_link_length = readlink(do_name, symb_link_string, buf.st_size);
		symb_link_length += 1;
		symb_link_string[symb_link_length - 1] = '\0';


		if (symb_link_length != -1)
		{
			strcat(do_name, arrow);
			strcat(do_name, symb_link_string);
		}
		else
		{
			fprintf(stderr, "%s: symbolic link error: %s", *parms, strerror(errno));
		}
	}

	errno = 0;
	if ((user = getpwuid(buf.st_uid)) == NULL || user->pw_name == NULL)
	{
		if (errno != 0)
		{
			fprintf(stderr, "%s: user error %s", *parms, strerror(errno));
		}
		else
		{
			snprintf(uid, sizeof(uid), "%d", buf.st_uid);
			do_user = uid;
		}
	}
	else
	{
		do_user = user->pw_name;
	}

	errno = 0;
	if ((group = getgrgid(buf.st_gid)) == NULL || (group->gr_name == NULL))
	{
		if (errno != 0)
		{
			fprintf(stderr, "%s: group error %s", *parms, strerror(errno));
		}
		else
		{
			snprintf(gid, sizeof(gid), "%d", buf.st_gid);
			do_group = gid;
		}
	}
	else
	{
		do_group = group->gr_name;
	}
	errno = 0;

	if (strftime(do_time, BUFFER, "%b %e %H:%M", localtime(time)) == 0)
	{
		fprintf(stderr, "%s: time error %s", *parms, strerror(errno));
		return;
	}

	if (printf("%6u %4u %s%4.0d%9s%9s %8u %s %s\n", (unsigned int)buf.st_ino, blocks, mode, buf.st_nlink, do_user, do_group, (unsigned int)buf.st_size, do_time, do_name) < 0)
	{
		fprintf(stderr, "%s: -ls error %s", *parms, strerror(errno));
		return EXIT_FAILURE;
	}
}

/**
*
* \brief usage_print explains usage of myfind
*
* This function lists the parms needed for successful myfind execution.
*
* \param parms is list of parms typed as parms of function
* \return nothing
*
*/
static void do_usage_print(const char* const* parms)
{
	fprintf(stderr, "Usage: %s <file or directory> <aktion> \n"
		"-type <bcdpfls>\n"
		"-path <pattern>\n"
		"-name <pattern>\n"
		"-user <name/uid>\n"
		"-nouser\n"
		"-group <name/gid>\n"
		"-nogroup\n"
		"-print\n"
		"-ls\n",
		*parms);
	exit(EXIT_FAILURE);
}


/*
* =================================================================== eof ==
*/
