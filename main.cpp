#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#include <sys/select.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <dirent.h>



#include <iostream>
#include <fstream>

#define PROC_DIRECTORY "/proc/"


typedef struct statstruct_proc {
  int           pid;                      /** The process id. **/
  char          exName [_POSIX_PATH_MAX]; /** The filename of the executable **/
  char          state; /** 1 **/          /** R is running, S is sleeping, 
			   D is sleeping in an uninterruptible wait,
			   Z is zombie, T is traced or stopped **/
  unsigned      euid,                      /** effective user id **/
                egid;                      /** effective group id */					     
  int           ppid;                     /** The pid of the parent. **/
  int           pgrp;                     /** The pgrp of the process. **/
  int           session;                  /** The session id of the process. **/
  int           tty;                      /** The tty the process uses **/
  int           tpgid;                    /** (too long) **/
  unsigned int	flags;                    /** The flags of the process. **/
  unsigned int	minflt;                   /** The number of minor faults **/
  unsigned int	cminflt;                  /** The number of minor faults with childs **/
  unsigned int	majflt;                   /** The number of major faults **/
  unsigned int  cmajflt;                  /** The number of major faults with childs **/
  int           utime;                    /** user mode jiffies **/
  int           stime;                    /** kernel mode jiffies **/
  int		cutime;                   /** user mode jiffies with childs **/
  int           cstime;                   /** kernel mode jiffies with childs **/
  int           counter;                  /** process's next timeslice **/
  int           priority;                 /** the standard nice value, plus fifteen **/
  unsigned int  timeout;                  /** The time in jiffies of the next timeout **/
  unsigned int  itrealvalue;              /** The time before the next SIGALRM is sent to the process **/
  int           starttime; /** 20 **/     /** Time the process started after system boot **/
  unsigned int  vsize;                    /** Virtual memory size **/
  unsigned int  rss;                      /** Resident Set Size **/
  unsigned int  rlim;                     /** Current limit in bytes on the rss **/
  unsigned int  startcode;                /** The address above which program text can run **/
  unsigned int	endcode;                  /** The address below which program text can run **/
  unsigned int  startstack;               /** The address of the start of the stack **/
  unsigned int  kstkesp;                  /** The current value of ESP **/
  unsigned int  kstkeip;                 /** The current value of EIP **/
  int		signal;                   /** The bitmap of pending signals **/
  int           blocked; /** 30 **/       /** The bitmap of blocked signals **/
  int           sigignore;                /** The bitmap of ignored signals **/
  int           sigcatch;                 /** The bitmap of catched signals **/
  unsigned int  wchan;  /** 33 **/        /** (too long) **/
  int		sched, 		  /** scheduler **/
                sched_priority;		  /** scheduler priority **/
		
} procinfo;


struct cpust{
    char pid_name[256];
	unsigned long utime;
	unsigned long stime;
	unsigned long um;
	unsigned long nm;
	unsigned long sm;
};


struct cpusg{
	float u;
	float s;
};

void gcpusg(struct cpusg *usg,struct cpust *st1,struct cpust *st2)
{
		if(st2->um == st1->um)
			usg->u = (st2->utime - st1->utime) / 100;
		else{
			printf("st2->utime: %ld st1->utime: %ld st2->um: %ld st1->um: %ld\n",
				st2->utime,st1->utime,st2->um,st1->um);
			usg->u = (st2->utime - st1->utime) / (float)((st2->um - st1->um) * 100);
		}

		if(st2->sm == st1->sm)
			usg->s = (st2->stime - st1->stime) / 100;
		else
			usg->s = (st2->stime - st1->stime) / (float)((st2->sm - st1->sm) * 100);
}

void clear_cpust(struct cpust *st)
{
	st->utime = 0;
	st->stime = 0;
	st->um = 0;
	st->nm = 0;
	st->sm = 0;
}

int get_proc_info(pid_t pid, procinfo * pinfo)
{
  char szFileName [_POSIX_PATH_MAX],
    szStatStr [2048],
    *s, *t;
  FILE *fp;
  struct stat st;
  
  if (NULL == pinfo) {
    errno = EINVAL;
    return -1;
  }

  sprintf (szFileName, "/proc/%u/stat", (unsigned) pid);
  
  if (-1 == access (szFileName, R_OK)) {
    return (pinfo->pid = -1);
  } /** if **/

  if (-1 != stat (szFileName, &st)) {
  	pinfo->euid = st.st_uid;
  	pinfo->egid = st.st_gid;
  } else {
  	pinfo->euid = pinfo->egid = -1;
  }
  
  
  if ((fp = fopen (szFileName, "r")) == NULL) {
    return (pinfo->pid = -1);
  } /** IF_NULL **/
  
  if ((s = fgets (szStatStr, 2048, fp)) == NULL) {
    fclose (fp);
    return (pinfo->pid = -1);
  }

  /** pid **/
  sscanf (szStatStr, "%u", &(pinfo->pid));
  s = strchr (szStatStr, '(') + 1;
  t = strchr (szStatStr, ')');
  strncpy (pinfo->exName, s, t - s);
  pinfo->exName [t - s] = '\0';
  
  sscanf (t + 2, "%c %d %d %d %d %d %u %u %u %u %u %d %d %d %d %d %d %u %u %d %u %u %u %u %u %u %u %u %d %d %d %d %u",
	  /*       1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33*/
	  &(pinfo->state),
	  &(pinfo->ppid),
	  &(pinfo->pgrp),
	  &(pinfo->session),
	  &(pinfo->tty),
	  &(pinfo->tpgid),
	  &(pinfo->flags),
	  &(pinfo->minflt),
	  &(pinfo->cminflt),
	  &(pinfo->majflt),
	  &(pinfo->cmajflt),
	  &(pinfo->utime),
	  &(pinfo->stime),
	  &(pinfo->cutime),
	  &(pinfo->cstime),
	  &(pinfo->counter),
	  &(pinfo->priority),
	  &(pinfo->timeout),
	  &(pinfo->itrealvalue),
	  &(pinfo->starttime),
	  &(pinfo->vsize),
	  &(pinfo->rss),
	  &(pinfo->rlim),
	  &(pinfo->startcode),
	  &(pinfo->endcode),
	  &(pinfo->startstack),
	  &(pinfo->kstkesp),
	  &(pinfo->kstkeip),
	  &(pinfo->signal),
	  &(pinfo->blocked),
	  &(pinfo->sigignore),
	  &(pinfo->sigcatch),
	  &(pinfo->wchan));
  fclose (fp);
  return 0;
}

void read_pid_st(struct cpust *st)
{
	FILE *fp;
	fp = fopen("/proc/stat ","r");
	int pid,ppid,pgrp,session,tty_nr,tpgid;
	unsigned int flags;
	unsigned long minflt,cminflt,majflt,cmajflt;
	char cm[255],stat;
	if(fscanf(fp,"%d %s %c %d %d %d %d %d %u %lu %lu %lu %lu %lu %lu",
		&pid,cm,&stat,&ppid,&pgrp,&session,&tty_nr,&tpgid,&flags,
		&minflt,&cminflt,&majflt,&cmajflt,&(st->utime),&(st->stime)) == EOF)
		perror("fscanf");
	printf("pid: %d command name: %s utime: %ld stime: %ld\n",
		pid,cm,st->utime,st->stime);
	fclose(fp);
}

void read_st(struct cpust *st)
{
	char cpun[255];
	FILE *fp = fopen("/proc/3063/stat","r");
	if(fscanf(fp,"%s %ld %ld %ld",cpun,&(st->um),&(st->nm),&(st->sm)) == EOF)
		perror("fscanf");
	printf("cpu: %s user mode: %ld nice mode: %ld system mode: %ld\n",
		cpun,st->um,st->nm,st->sm);
	fclose(fp);
}

int IsNumeric(const char* ccharptr_CharacterList)
{
    for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
        if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
            return 0; // false
    return 1; // true
}

int read_proc()
{
    struct dirent* dirEntity = NULL ;
    DIR* dir_proc = NULL ;

    dir_proc = opendir(PROC_DIRECTORY) ;
    if (dir_proc == NULL)
    {
        perror("Couldn't open the " PROC_DIRECTORY " directory") ;
        return -1 ;
    }

    while ((dirEntity = readdir(dir_proc)) != 0) {
        if (dirEntity->d_type == DT_DIR) {
            if (IsNumeric(dirEntity->d_name)) {
                std::string path = std::string(PROC_DIRECTORY) + dirEntity->d_name + "/cmdline";

                std::ifstream cmdLineFile(path.c_str());
                std::string cmdLine;
                if (cmdLineFile) {

                    std::getline(cmdLineFile, cmdLine);

                }
                std:: cout << "pid: " << dirEntity->d_name << " " << cmdLine << std::endl;
            }
        }
    }
    closedir(dir_proc);
       
}

int main(int ac, char **av)
{

    read_proc();
    
    pid_t target_pid = atoi(av[1]);
    procinfo pinfo_target;
    get_proc_info(target_pid, &pinfo_target);
    size_t cnt = 0;
    
    printf("tty %d\n", pinfo_target.tty);
    printf("session %d\n", pinfo_target.session);
    printf("ppid %d\n", pinfo_target.ppid);
    
   
	struct cpust st1,st2;
	struct cpusg usg;
	while(cnt++ < 40)
    {
		read_pid_st(&st1);
		read_st(&st1);
		sleep(1);
		read_pid_st(&st2);
		read_st(&st2);
		gcpusg(&usg,&st1,&st2);
		printf("cnt: %zu user mode: %f %% system mode: %f %%\n\n",
        cnt,usg.u,usg.s);
	}
    return 0;
}
