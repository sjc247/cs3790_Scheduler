#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


int pid[10];			//Array holding PID's of children
int numProcess = 0;		//Number of processes running
int current = 0;		//Current process running

struct sigaction newhandler, oldhandler;
sigset_t sig;

void setblock( int fd ,bool block)
{
   static int blockf, nonblockf;
   static bool first = true;

   int flags;

   if (first) {
      first = false;
      if (( flags = fcntl(fd,F_GETFL,0)) == -1)
      {
         fprintf(stderr," fcntl - error \n");
         exit(1);
      }
     blockf = flags & ~ O_NDELAY;
     nonblockf = flags | O_NDELAY;
  }
  if ( fcntl(fd,F_SETFL, block ? blockf : nonblockf) == -1 )
      {
         fprintf(stderr," fcntl2 - error \n");
         exit(1);
      }
}

void next( int code) //signal handler
{
	if(numProcess > 0){

		kill(pid[current],SIGSTOP);
    		current++;
    		if(current == numProcess)
			current = 0;

    		kill(pid[current],SIGCONT);
	}
}

void execute(char* prog){

	pid[numProcess] = fork();

	if(pid[numProcess] == 0){

		execl("./write", "write", prog, NULL);
	}

	if(numProcess != 0)
	// If process is NOT the first to be scheduled, stop it prematurely.
	// This prevents new procs from starting before they've been scheduled
	// to run.
		kill(pid[numProcess], SIGSTOP);


	numProcess++;
}

int main()
{

    	char mess[80];
    	int fd;
    	int numch;


        fd = open("/dev/tty",O_RDWR);
        setblock(fd,false);

        
	sigemptyset(&sig);  /* clear the signal mask */
        newhandler.sa_handler = next;
        newhandler.sa_mask = sig;
        newhandler.sa_flags = 0;

        if ( sigaction(SIGALRM,&newhandler,&oldhandler) == -1 )
          {
            printf("1 - can't install signal handler \n");
            exit(-1);
          }
        

	while (1) {

	 switch  ( numch = read(fd,mess,80))  {
          case -1 :
          case 0 :
              break;
          default:

		mess[numch - 1] = '\0';
	        execute(mess);
          }

//	  fprintf(stderr, "PARENT IS AWAKE!\n");


          alarm(3); 		//alarm will send signal after 3 seconds
          pause(); 		//do nothing until alarm's off

	  fprintf(stderr, "\n");

//	  fprintf(stderr,"PARENT IS FINISHED SLEEPING!\n");
        }

	return 0;
}




