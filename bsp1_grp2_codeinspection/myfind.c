/**
 * @file myfind.c
 * Betriebssysteme
 * Beispiel 1
 *
 * @author Stammgruppe2
 * @author Peter Hohensasser-Hottowy  	ic16b039 <ic16b039@technikum-wien.at>
 * @author Wolfgang Suchy      			ic16b058 <ic16b058@technikum-wien.at>
 * @author Florian Schmidt		    	ic16b504 <ic16b504@technikum-wien.at>
 * @date 2017/03/13
 *
 */

/*
 *---------------------------------------------------------------------includes-
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <fnmatch.h>
#include <libgen.h>
#include <sys/types.h>
#include <unistd.h>



/*
 * -------------------------------------------------------------------globals-
 */

#define BUFFER_SIZE 4096

/*
 *--------------------------------------------------------------------typedef-
 */

/*
 *------------------------------------------------------------------functions-
 */
static void do_file(const char * file_name, const char * const * parms);

static void do_dir(const char * dir_name, const char * const * parms);

static void do_ls(struct stat statbuffer,const char * file_name, const char * const * parms);

static int typematch(const char * const type, struct stat statbuffer, const char * const * parms);

static void do_parameter_check(const char * file_name, const char * const * parms);

/**
 *
 * \brief A simple version of "find" - to be tested against bic-myfind-with-all-checks *
 * This is the main entry point for any C program.
 *
 * \param argc the number of arguments
 * \param argv the arguments itselves (including the program name in argv[0])
 *
 * \return always "success"
 * \retval 0 always
 *
 */
int main (int argc, const char * const * argv)
{
	if (argc<=1) 
		{ 
			if (printf("%s No path given. This implementation of find requires a path\n",argv[0])<0) fprintf(stderr,"%s: Error printf\n",argv[0]); 
			exit (EXIT_SUCCESS);
		}
	
/*
 * ###14: <Gute Idee den Suchpfad zu kopieren um nichts zu ändern/gefährden.
 *		   Aber was passiert wenn nach den actions noch ein Pfad oder Name folgt?>
 *
*/
	
	char * searchpath = malloc(sizeof(char) * ((strlen(argv[1]))+1));
		
	if (searchpath == NULL) { fprintf(stderr, "%s: Error allocating memory for searchpach - Exiting\n",argv[0]); exit(EXIT_FAILURE);}
	
	errno = 0;
	strcpy(searchpath,argv[1]);

	struct stat statbuffer;
	
	if (lstat(searchpath,&statbuffer) == -1) 
		{fprintf(stderr, "%s: Error opening path '%s': %s\n",argv[0],searchpath,strerror(errno));
			return EXIT_FAILURE;}
	else do_parameter_check(searchpath,argv);
	
	free(searchpath);
	searchpath = NULL;
	
	if(fflush(stdout) == EOF) fprintf(stderr,"%s: Error flush stdout\n", argv[0]);
	
	
	return EXIT_SUCCESS;
}

/**
 *
 * \brief Reads directories, creates new paths and calls do_file to filter and output them. Gets called by do_file if a new path is a directory
 * 
 *
 * \param dir_name the current searchpath
 * \param parms complete argv
 *
 * \return void
 *
 */
static void do_dir(const char * dir_name, const char * const * parms)
{
	DIR *pDir;
	struct dirent *current_dir;
	errno = 0;
	
	pDir=opendir(dir_name);
	if (pDir == NULL) {fprintf(stderr, "%s: Error opening path '%s': %s\n",parms[0],dir_name,strerror(errno)); return;}
	errno = 0;
	 
	while ((current_dir = readdir(pDir)) != NULL)
		{ 
			if (((strcmp(current_dir->d_name,"."))&&(strcmp(current_dir->d_name,"..")))!=0)
			{
				char *next_path=malloc(sizeof(char)*((strlen(dir_name))+1));
				if (next_path == NULL) { fprintf(stderr, "%s: Error allocating memory for searchpach - Exiting\n",parms[0]); exit(EXIT_FAILURE);}
				
				if (dir_name[strlen(dir_name)-1]!='/')  next_path=realloc(next_path,sizeof(char)*(strlen(current_dir->d_name)+3+strlen(dir_name)));
				else next_path=realloc(next_path,sizeof(char)*(strlen(current_dir->d_name)+2+strlen(dir_name)));
				
				if (next_path == NULL) { fprintf(stderr, "%s: Error allocating memory for searchpach - Exiting\n",parms[0]); exit(EXIT_FAILURE);}
				
				strcpy(next_path,dir_name);				
				if (next_path[strlen(next_path)-1]!='/') strcat(next_path,"/"); 
				strcat(next_path,current_dir->d_name);
				do_file(next_path,parms);
				
				free(next_path);
				next_path=NULL;
				
			}
			
			errno = 0;
		}
	
	if (errno !=0) {fprintf(stderr, "%s: Error reading Directory %s : %s ",parms[0],dir_name, strerror(errno)) ; return;}
	errno = 0;	
			
	
	if(closedir(pDir) != 0) fprintf(stderr,"%s: Error at closedir: %s",parms[0],strerror(errno));
	
	return;
}
/**
 *
 * \brief Based on do_file. Looks for input syntax errors and passes all input variables on to do_file.
 * 
 *
 * \param dir_name the current searchpath
 * \param parms complete argv
 *
 *
 * \return void
 *
 */
static void do_parameter_check(const char * file_name, const char * const * parms)
{

	int i = 2;
	
	
	struct stat statbuffer;
	if (lstat(file_name,&statbuffer) == -1) 
		{fprintf(stderr, "%s: Error opening path '%s': %s\n",parms[0],file_name,strerror(errno));
			errno = 0;
		}		
		
	while (parms[i]!= NULL)
	
		{ 
			if (strcmp(parms[i],"-name")!=0&&strcmp(parms[i],"-user")!=0&& strcmp(parms[i],"-print")!=0&& strcmp(parms[i],"-ls")!=0 && strcmp(parms[i],"-nouser")!=0&& strcmp(parms[i],"-path")!=0&& strcmp(parms[i],"-type")!=0)
			
			{
				fprintf(stderr,"%s input: -name Name -user User -path Pfad -type Typ -nouser -print -ls\n",parms[0]);
				exit (EXIT_FAILURE);
			}
			
			
			if (strcmp(parms[i],"-name")==0)
			
			{
				if (parms[i+1] == NULL)
					{
						fprintf(stderr, "%s: missing arument to '%s'\n",parms[0], parms[i]);
						exit (EXIT_FAILURE);
					}	
				
				else 
					{
						
						i++;
					}	
			}
			
			
			else if (strcmp(parms[i],"-path")==0)
			
			{
				if (parms[i+1] == NULL)
					{
						fprintf(stderr, "%s: missing arument to '%s'\n",parms[0], parms[i]);
						exit (EXIT_FAILURE);
					}	
				
				else 
					{
												
						 
						
						i++;
					}	
			}
			
			else if (strcmp(parms[i],"-type")==0)
			
			{
				if (parms[i+1] == NULL)
					{
						fprintf(stderr, "%s: missing arument to '%s'\n",parms[0], parms[i]);
						exit (EXIT_FAILURE);
					}	
				
				else 
					{
												
						
						
						i++;
					}	
			}
			
			
			
			
			
			else if (strcmp(parms[i],"-user")==0)
			
				{
				if (parms[i+1] == NULL)
					{
						fprintf(stderr, "%s: missing arument to '%s'\n",parms[0], parms[i]);
						exit (EXIT_FAILURE);
					}	
				
				else 
					{	errno=0;
						struct passwd *name = getpwnam(parms[i+1]);
						if (name == NULL)
							{
								if (errno !=0)
									{
										fprintf(stderr,"%s: Error finding user %s: %s\n",parms[0],parms[i+1],strerror(errno));
										exit(EXIT_FAILURE);
									}
								
								else
									{
										char *uidendptr;
										errno=0;
										int uidnum = strtol(parms[i+1], &uidendptr, 10);
										if (errno!=0) fprintf(stderr,"%s: error at strtol: %s",parms[0],strerror(errno));
										errno=0;
										if (uidendptr[0] != '\0') 
											{
												fprintf(stderr,"%s: '%s'  is not the name of a known user.\n",parms[0],parms[i+1]);
												exit(EXIT_FAILURE);
											}
										
										errno=0;		
										struct passwd *uid = getpwuid(uidnum);
										if (uid == NULL)
											{
											
												if (errno !=0) 
													{
														fprintf(stderr,"%s: Error finding user %s: %s\n",parms[0],parms[i+1],strerror(errno));
														exit(EXIT_FAILURE);
													}	
													
												else 
													{
													fprintf(stdout,"%s: '%s'  is not the name of a known user\n",parms[0],parms[i+1]);
													exit(EXIT_FAILURE);
													}	
											}
										
										
											
											
										
									}
							}
							
						
								
							
						i++;
					}	
				}
			
			
		
			
			
			
		 i++;
		 
		}
		
			
	do_file(file_name,parms);
	
	return;
	
}



/**
 *
 * \brief Parses the arguments, applies them to the directory entries being passed by do_dir and outputs those if applicable. 
 *
 * \param file_name the object which is being handled
 * \param parms complete argv
 *
 * \return void
 *
 */
 static void do_file(const char * file_name, const char * const * parms)
{
	
	int i = 2;
	int match=1;
	int print=0;
	int ls=0;
	
	struct stat statbuffer;
	if (lstat(file_name,&statbuffer) == -1) 
		{fprintf(stderr, "%s: Error opening path '%s': %s\n",parms[0],file_name,strerror(errno));
			errno = 0;
		}
		
		
	while (parms[i]!= NULL)
	
		{ 
			if (strcmp(parms[i],"-name")!=0&&strcmp(parms[i],"-user")!=0&& strcmp(parms[i],"-print")!=0&& strcmp(parms[i],"-ls")!=0 && strcmp(parms[i],"-nouser")!=0&& strcmp(parms[i],"-path")!=0&& strcmp(parms[i],"-type")!=0)
			
			{
				fprintf(stderr,"%s input: -name Name -user User -path Pfad -type Typ -nouser -print -ls\n",parms[0]);
				exit (EXIT_FAILURE);
			}
			
			
			if (strcmp(parms[i],"-name")==0)
			
			{
				
				int buffer_max=(strlen(file_name)+1);
						
				char * base_name = malloc(sizeof(char) * buffer_max);
				if (base_name == 0) fprintf(stderr, "%s: Error allocating memory. Exiting\n",parms[0]);
				strcpy(base_name,file_name);
						
				(fnmatch(parms[i+1],basename(base_name),0))?(match=0):(match=match);
				 free(base_name);
				 base_name = NULL;
				i++;
						
			}
			
			
			else if (strcmp(parms[i],"-path")==0)
			
			{
				
				(fnmatch(parms[i+1],file_name,0))?(match=0):(match=match);
				i++;
					
			}
			
			else if (strcmp(parms[i],"-type")==0)
			
			{
				
			if(typematch(parms[i+1],statbuffer,parms)==0) match = 0;
			i++;
				
			}
			
			
			
			
/*
 * ###14: <Direkte (1 zu 1) Kopie des Codes aus do_parameter_check.>
 *
*/			
			else if (strcmp(parms[i],"-user")==0)
			
				{
				
				
					errno=0;
						struct passwd *name = getpwnam(parms[i+1]);
						if (name == NULL)
							{
								if (errno !=0)
									{
										fprintf(stderr,"%s: Error finding user %s: %s\n",parms[0],parms[i+1],strerror(errno));
										exit(EXIT_FAILURE);
									}
								
								else
									{
										char *uidendptr;
										int uidnum = strtol(parms[i+1], &uidendptr, 10);
										errno=0;
										if (uidendptr[0] != '\0') 
											{
												fprintf(stderr,"%s: '%s'  is not the name of a known user.\n",parms[0],parms[i+1]);
												exit(EXIT_FAILURE);
											}
										
										errno=0;		
										struct passwd *uid = getpwuid(uidnum);
										if (uid == NULL)
											{
											
												if (errno !=0) 
													{
														fprintf(stderr,"%s: Error finding user %s: %s\n",parms[0],parms[i+1],strerror(errno));
														exit(EXIT_FAILURE);
													}	
													
												else 
													{
													fprintf(stdout,"%s: '%s'  is not the name of a known user\n",parms[0],parms[i+1]);
													exit(EXIT_FAILURE);
													}	
											}
										
										else if ((uid->pw_uid) != (statbuffer.st_uid)) match=0;
											
											
										
									}
							}
							
						else if (name->pw_uid != statbuffer.st_uid) match=0;
								
							
						i++;
						
				}
			
			
			else if (strcmp(parms[i],"-print")==0)
			
			{
				print=1;
				if (match ==1) 
				{
					if (printf("%s\n",file_name)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
				}
			}
			
				else if (strcmp(parms[i],"-ls")==0)
			
			{
				ls=1;
				if (match ==1) do_ls(statbuffer,file_name,parms);
				
			}
			
			else if (strcmp(parms[i],"-nouser")==0)
			
			{
				errno = 0;
				struct passwd *user = getpwuid(statbuffer.st_uid);
				if (errno !=0) 
					{
						fprintf(stderr,"%s: Error getting uid: %s\n",parms[0],strerror(errno));
						exit(EXIT_FAILURE);
					}
				if (user != NULL) match =0;
			}
			
			
		 i++;
		 
		}
		
			
	if (( match == 1 )&&( print == 0)&&(ls == 0)) 
	{
		if (printf("%s\n",file_name)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
	}

	if (S_ISDIR(statbuffer.st_mode)==1) do_dir(file_name,parms);
	
	return;
	
}
/**
 *
 * \brief Outputs long form of directory entries
 *
 * \param statbuffer output of lstat for current directory entry
 * \param file_name the object which is being handled
 * \param parms complete argv
 *
 * \return void
 *
 */
 
static void do_ls(struct stat statbuffer,const char * file_name, const char * const * parms)
{
	
    char timestring[BUFFER_SIZE];
	struct passwd * user = getpwuid(statbuffer.st_uid);
	if (errno !=0) 
		{
			fprintf(stderr,"%s: Error getting uid : %s\n",parms[0],strerror(errno));
			exit(EXIT_FAILURE);
		}
	errno=0;
	
	 if (user == NULL && errno != 0)
    {
        fprintf(stderr,"%s: Error getting user for %s: %s \n", parms[0], file_name, strerror(errno));
        if (errno != EMFILE) exit(EXIT_FAILURE);
    }
	errno =0;
	
	struct group * file_group = getgrgid(statbuffer.st_gid);
	
		 if (file_group == NULL && errno != 0)
    {
        fprintf(stderr,"%s: Error getting group for %s: %s \n", parms[0], file_name, strerror(errno));
         if (errno != EMFILE) exit(EXIT_FAILURE);
    }
    errno=0;
		
	
    
     if (strftime(timestring, BUFFER_SIZE, "%b %d %H:%M", localtime(&statbuffer.st_mtime))==0)
     {
		 fprintf(stderr,"%s: Error getting time: %s\n",parms[0],strerror(errno));
	 }
	 
	 errno=0;
	 
	
/*
 * ###14: <Fprints und Fehlerprints würden zusammengefast die Lesbarkeit fördern und Codezeilen sparen!>
*/	
	
	 
  if (printf("  %6lu ",statbuffer.st_ino)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
/*
 * ###14: <Blocksize bei Symbolic Links wird nicht behandelt.>
*/		
	
    if (printf("%6lu ",statbuffer.st_blocks/2)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(S_ISDIR(statbuffer.st_mode) ? "d" : S_ISLNK(statbuffer.st_mode)? "l":"-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_IRUSR ? "r" : "-")<0)fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_IWUSR ? "w" : "-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_ISUID ? (statbuffer.st_mode & S_IXUSR ? "s" : "S" ): statbuffer.st_mode & S_IXUSR ? "x" : "-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_IRGRP ? "r" : "-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_IWGRP ? "w" : "-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_ISGID ? (statbuffer.st_mode & S_IXGRP ? "s" : "S" ): statbuffer.st_mode & S_IXGRP ? "x" : "-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_IROTH ? "r" : "-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_IWOTH ? "w" : "-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(statbuffer.st_mode & S_IXOTH ? statbuffer.st_mode & S_ISVTX ? "t" : "x" :statbuffer.st_mode & S_ISVTX ? "T" : "-")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf(" ")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf("%3lu ",(unsigned long) statbuffer.st_nlink)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (user != NULL)
	{
		if (printf("%4s ", user->pw_name)<0) fprintf(stderr,"%s: Error printf\n",parms[0]); 
	}
		else
		{
			
			if (printf("%9u ", statbuffer.st_uid)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
		}
		
    if (file_group!=NULL) 
		{
			if (printf("%9s ", file_group->gr_name)<0) fprintf(stderr,"%s: Error printf\n",parms[0]); 
		}	
			else 
			{
				if (printf("%9s", " ")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
			}
    if (printf("%12lu ", statbuffer.st_size)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf("%s ",timestring)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    if (printf("%s ", file_name)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);

	 if(S_ISLNK(statbuffer.st_mode))
    {
		char linkdest[statbuffer.st_size+1];
		
		
	
        long linksize = readlink(file_name, linkdest, (statbuffer.st_size));
        linkdest[linksize]='\0';

        if (errno != 0 || linksize < 0)
        {
            fprintf(stderr,"%s: Error durring readlink:%s\n", parms[0], strerror(errno));
        }
		else
		{
			if (printf("-> %s", linkdest)<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
		}
    }

    if (printf("\n")<0) fprintf(stderr,"%s: Error printf\n",parms[0]);
    
}
/**
 *
 * \brief Matches type of directory entry with the specified type being looked for.
 *
 * \param type file type specified by user
 * \param statbuffer output of lstat for current directory entry
 * \param parms complete argv
 *
 * \return 1 if match - 0 if not
 *
 */
static int typematch(const char * const type, struct stat statbuffer, const char * const * parms)
{
	
	if (strcmp(type,"b")==0) return(S_ISBLK(statbuffer.st_mode)) ? 1 : 0;
	else if (strcmp(type,"c")==0)return (S_ISCHR(statbuffer.st_mode)) ? 1 : 0;
	else if (strcmp(type,"d")==0) return (S_ISDIR(statbuffer.st_mode)) ? 1 : 0;
	else if (strcmp(type,"p")==0) return (S_ISFIFO(statbuffer.st_mode)) ? 1 : 0;
	else if (strcmp(type,"f")==0) return (S_ISREG(statbuffer.st_mode)) ? 1 : 0;
	else if (strcmp(type,"l")==0) return (S_ISLNK(statbuffer.st_mode)) ? 1 : 0;
	else if (strcmp(type,"s")==0) return (S_ISSOCK(statbuffer.st_mode)) ? 1 : 0;
	else {fprintf(stderr,"%s: Invalid parameter for -type. Allowed Are: b, c, d, p, f, l, and s \n",parms[0]); exit(EXIT_FAILURE);}
	
}


/*
 * =================================================================== eof ==
 */
