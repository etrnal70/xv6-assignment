#ifdef CS333_P2
#include "types.h"
#include "user.h"

char*
getFront(int len, char *init[]){
  char *ret = init[0];

  for(int i = 0; i < len; i++){
    init[i]=init[i+1];
  }

  if(len > 1)
    init[len] = (char*)NULL;

  return ret;
}

void
time(char *bin, int len, char *param[])
{
  char* newbin = getFront(len-1, param);

  // Initialize current ticks
  int init = uptime();

  int pid = fork();
  if(pid < 0){
    printf(1, "fork failed\n");
    exit();
  }

  if(pid == 0){
    // Handle recursive time command
    if (strcmp(newbin,"time") == 0){
      time(newbin, len-1, param);
    }else{
      if(exec(newbin, param) < 0){
        exit();
      }
    }
  }

  wait();

  int final = uptime();

  printf(1, "%s ran in ", newbin); 

  if ((final-init) < 10){
    printf(1, "0.00%d seconds.\n", final-init);
  }else if ((final-init) < 100){
    printf(1, "0.0%d seconds.\n", final-init);
  }else if ((final-init) < 1000){
    printf(1, "0.%d seconds.\n", final-init);
  }else{
    printf(1, "%d.%d seconds.\n", (final-init)/1000, (final-init)%1000);
  }

  exit();
}

int
main(int argc, char *argv[])
{
  char* bin = getFront(argc, argv);
  time(bin, argc-1, argv);

  exit();
}
#endif // CS333_P2
