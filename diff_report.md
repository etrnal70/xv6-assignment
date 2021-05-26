# Project 2
## UIDs and GIDs and PPIDs

```diff
diff --git a/p2-test.c b/p2-test.c
index 6f1ce25..f783f16 100755
--- a/p2-test.c
+++ b/p2-test.c
@@ -9,9 +9,9 @@
 // comment out tests for features the student doesn't have implemented
 // Note the CPUTIME_TEST requires GETPROCS_TEST
 #define UIDGIDPPID_TEST
-#define CPUTIME_TEST
-#define GETPROCS_TEST
-#define TIME_TEST
+// #define CPUTIME_TEST
+// #define GETPROCS_TEST
+// #define TIME_TEST
 
 
 #ifdef GETPROCS_TEST
diff --git a/proc.c b/proc.c
index 4aade9e..c3abe89 100644
--- a/proc.c
+++ b/proc.c
@@ -179,6 +179,10 @@ userinit(void)
   p->tf->eflags = FL_IF;
   p->tf->esp = PGSIZE;
   p->tf->eip = 0;  // beginning of initcode.S
+  #ifdef CS333_P2
+  p->uid = DEFAULT_UID;
+  p->gid = DEFAULT_GID;
+  #endif
 
   safestrcpy(p->name, "initcode", sizeof(p->name));
   p->cwd = namei("/");
@@ -236,6 +240,10 @@ fork(void)
     np->state = UNUSED;
     return -1;
   }
+  #ifdef CS333_P2
+  np->uid = curproc->uid;
+  np->gid = curproc->gid;
+  #endif
   np->sz = curproc->sz;
   np->parent = curproc;
   *np->tf = *curproc->tf;
diff --git a/proc.h b/proc.h
index 142d7ad..4d10858 100644
--- a/proc.h
+++ b/proc.h
@@ -41,7 +41,9 @@ struct proc {
   char *kstack;                // Bottom of kernel stack for this process
   enum procstate state;        // Process state
   uint pid;                    // Process ID
-  struct proc *parent;         // Parent process. NULL indicates no parent
+  uint uid;                    // Process UID
+  uint gid;                    // Process GID
+  struct proc *parent;         // Parent process. NULL indicates no parent 
   struct trapframe *tf;        // Trap frame for current syscall
   struct context *context;     // swtch() here to run process
   void *chan;                  // If non-zero, sleeping on chan
diff --git a/syscall.c b/syscall.c
index 1072be7..447611a 100644
--- a/syscall.c
+++ b/syscall.c
@@ -109,6 +109,13 @@ extern int sys_halt(void);
 #ifdef CS333_P1
 extern int sys_date(void);
 #endif
+#ifdef CS333_P2
+extern int sys_getuid(void);
+extern int sys_getgid(void);
+extern int sys_getppid(void);
+extern int sys_setuid(void);
+extern int sys_setgid(void);
+#endif
 
 static int (*syscalls[])(void) = {
 [SYS_fork]    sys_fork,
@@ -138,6 +145,13 @@ static int (*syscalls[])(void) = {
 #ifdef CS333_P1
 [SYS_date]    sys_date,
 #endif
+#ifdef CS333_P2
+[SYS_getuid]    sys_getuid,
+[SYS_getgid]    sys_getgid,
+[SYS_getppid]   sys_getppid,
+[SYS_setuid]    sys_setuid,
+[SYS_setgid]    sys_setgid,
+#endif
 };
 
 #ifdef PRINT_SYSCALLS
@@ -169,6 +183,13 @@ static char *syscallnames[] = {
 #ifdef CS333_P1
   [SYS_date]    "date",
 #endif
+#ifdef CS333_P2
+  [SYS_getuid]  "getuid",
+  [SYS_getgid]  "getgid",
+  [SYS_getppid] "getppid",
+  [SYS_setuid]  "setuid",
+  [SYS_setgid]  "setgid",
+#endif
 };
 #endif // PRINT_SYSCALLS
 
diff --git a/syscall.h b/syscall.h
index 14f3e17..b7e1459 100644
--- a/syscall.h
+++ b/syscall.h
@@ -23,3 +23,8 @@
 #define SYS_halt    SYS_close+1
 // student system calls begin here. Follow the existing pattern.
 #define SYS_date    SYS_halt+1
+#define SYS_getuid  SYS_date+1
+#define SYS_getgid  SYS_getuid+1
+#define SYS_getppid SYS_getgid+1
+#define SYS_setuid  SYS_getppid+1
+#define SYS_setgid  SYS_setuid+1
diff --git a/sysproc.c b/sysproc.c
index c5435df..e993f06 100644
--- a/sysproc.c
+++ b/sysproc.c
@@ -111,3 +111,46 @@ sys_date(void)
   return 0;
 }
 #endif
+
+#ifdef CS333_P2
+int
+sys_getuid(void)
+{
+  return myproc()->uid;
+}
+int
+sys_getgid(void)
+{
+  return myproc()->gid;
+}
+int
+sys_getppid(void)
+{
+  // Return current PID if current PPID is 
+  // NULL for e.g. init process
+  if (myproc()->parent->pid == NULL){
+    return myproc()->pid;
+  }
+  return myproc()->parent->pid;
+}
+int
+sys_setuid(void)
+{
+  int uid;
+
+  if(argint(0, &uid) < 0 || uid < 0 || uid > 32767)
+    return -1;
+  myproc()->uid = (uint)uid;
+  return 0;
+}
+int
+sys_setgid(void)
+{
+  int gid;
+
+  if(argint(0, &gid) < 0 || gid < 0 || gid > 32767)
+    return -1;
+  myproc()->gid = (uint)gid;
+  return 0;
+}
+#endif
diff --git a/testsetuid.c b/testsetuid.c
new file mode 100644
index 0000000..c43fa2a
--- /dev/null
+++ b/testsetuid.c
@@ -0,0 +1,11 @@
+#ifdef CS333_P2
+#include "types.h"
+#include "user.h"
+
+int
+main(int argc, char *argv[])
+{
+  printf(1, "***** In %s: my uid is %d\n\n", argv[0], getuid());
+  exit();
+}
+#endif
diff --git a/user.h b/user.h
index f236eff..eb480cf 100644
--- a/user.h
+++ b/user.h
@@ -28,6 +28,13 @@ int halt(void);
 #ifdef CS333_P1
 int date (struct rtcdate*);
 #endif // CS333_P1
+#ifdef CS333_P2
+uint getuid(void);
+uint getgid(void);
+uint getppid(void);
+int  setuid(uint);
+int  setgid(uint);
+#endif // CS333_P2
 
 // ulib.c
 int stat(char*, struct stat*);
diff --git a/usys.S b/usys.S
index 84bd80b..090de9a 100644
--- a/usys.S
+++ b/usys.S
@@ -31,3 +31,8 @@ SYSCALL(sleep)
 SYSCALL(uptime)
 SYSCALL(halt)
 SYSCALL(date)
+SYSCALL(getuid)
+SYSCALL(getgid)
+SYSCALL(getppid)
+SYSCALL(setuid)
+SYSCALL(setgid)
```

## Process Execution Time

```diff
diff --git a/proc.c b/proc.c
index c3abe89..1d4fb1e 100644
--- a/proc.c
+++ b/proc.c
@@ -153,6 +153,10 @@ allocproc(void)
   // ticks global variable
   p->start_ticks = ticks;
 
+  // Initialize when program was allocated
+  p->cpu_ticks_in = 0;
+  p->cpu_ticks_total = 0;
+
   return p;
 }
 
@@ -401,6 +405,8 @@ scheduler(void)
       c->proc = p;
       switchuvm(p);
       p->state = RUNNING;
+      // Save value when process enter CPU
+      p->cpu_ticks_in = ticks;
       swtch(&(c->scheduler), p->context);
       switchkvm();
 
@@ -441,6 +447,8 @@ sched(void)
   if(readeflags()&FL_IF)
     panic("sched interruptible");
   intena = mycpu()->intena;
+  // Save CPU elapsed time
+  p->cpu_ticks_total += ticks - (p->cpu_ticks_in);
   swtch(&p->context, mycpu()->scheduler);
   mycpu()->intena = intena;
 }
diff --git a/proc.h b/proc.h
index 4d10858..649565d 100644
--- a/proc.h
+++ b/proc.h
@@ -52,6 +52,8 @@ struct proc {
   struct inode *cwd;           // Current directory
   char name[16];               // Process name (debugging)
   uint start_ticks;            // Store initial time when allocated
+  uint cpu_ticks_total;        // Total elapsed ticks in CPU
+  uint cpu_ticks_in;           // Ticks when scheduled
 };
 
 // Process memory is laid out contiguously, low addresses first:
```

## The *"ps"* command

```diff
diff --git a/defs.h b/defs.h
index f85557d..f2f1025 100644
--- a/defs.h
+++ b/defs.h
@@ -4,6 +4,7 @@ struct file;
 struct inode;
 struct pipe;
 struct proc;
+struct uproc;
 struct rtcdate;
 struct spinlock;
 struct sleeplock;
@@ -124,6 +125,9 @@ void            userinit(void);
 int             wait(void);
 void            wakeup(void*);
 void            yield(void);
+#ifdef CS333_P2
+int             copyProc(int, struct uproc*);
+#endif // CS333_p2
 #ifdef CS333_P3
 void            printFreeList(void);
 void            printList(int);
diff --git a/p2-test.c b/p2-test.c
index f783f16..c614add 100755
--- a/p2-test.c
+++ b/p2-test.c
@@ -9,8 +9,8 @@
 // comment out tests for features the student doesn't have implemented
 // Note the CPUTIME_TEST requires GETPROCS_TEST
 #define UIDGIDPPID_TEST
-// #define CPUTIME_TEST
-// #define GETPROCS_TEST
+#define CPUTIME_TEST
+#define GETPROCS_TEST
 // #define TIME_TEST
 
 
diff --git a/proc.c b/proc.c
index 1d4fb1e..1317052 100644
--- a/proc.c
+++ b/proc.c
@@ -5,6 +5,7 @@
 #include "mmu.h"
 #include "x86.h"
 #include "proc.h"
+#include "uproc.h"
 #include "spinlock.h"
 
 static char *states[] = {
@@ -645,6 +646,47 @@ procdump(void)
 #endif // CS333_P1
 }
 
+#ifdef CS333_P2
+// Helper function to access ptable for sys_getprocs
+int
+copyProc(int max, struct uproc* up)
+{
+  int count = 0;
+  struct proc* tp;
+  acquire(&ptable.lock);
+
+  for(tp = ptable.proc; tp < &ptable.proc[NPROC]; tp++){
+    if (count == max)
+      break;
+    if (tp->state == UNUSED || tp->state == EMBRYO){
+      continue;
+    }else if(tp->state == SLEEPING || tp->state == RUNNABLE || tp->state == RUNNING || tp->state == ZOMBIE) {
+      up[count].pid = tp->pid;
+      up[count].uid = tp->uid;
+      up[count].gid = tp->gid;
+
+      // Handle init PPID
+      if (tp->pid == 1){
+        up[count].ppid = tp->pid;
+      }else{
+        up[count].ppid = tp->parent->pid;
+      }
+
+      up[count].elapsed_ticks = ticks - tp->start_ticks;
+      up[count].CPU_total_ticks = tp->cpu_ticks_total;
+      safestrcpy(up[count].state, states[tp->state], sizeof(up[count].state));
+      up[count].size = tp->sz;
+      safestrcpy(up[count].name, (char*)tp->name, sizeof(tp->name));
+  
+      count++;
+    }
+  }
+
+  release(&ptable.lock);
+  return count;
+}
+#endif // CS333_P2
+
 #if defined(CS333_P3)
 // list management helper functions
 static void
diff --git a/ps.c b/ps.c
new file mode 100644
index 0000000..7b5e7d2
--- /dev/null
+++ b/ps.c
@@ -0,0 +1,68 @@
+#ifdef CS333_P2
+
+#define MAX_ENTRIES 16
+
+#include "types.h"
+#include "user.h"
+#include "uproc.h"
+
+int
+main(int argc, char *argv[])
+{
+  struct uproc* data;
+
+  data = malloc(sizeof(struct uproc) * MAX_ENTRIES);
+
+  if (getprocs(MAX_ENTRIES, data) < 0) {
+    printf(2,"Error: getprocs call failed. %s at line %d\n",
+        __FILE__, __LINE__);
+    exit();
+  }
+
+  printf(1, "\nPID\tName\t\tUID\tGID\tPPID\tElapsed\tCPU\tState\tSize\n");
+
+  for (int i = 0; i < MAX_ENTRIES; i++){
+    if (data[i].pid == NULL)
+      break;
+    printf(1, "%d\t%s\t\t%d\t%d\t%d\t", 
+      data[i].pid,
+      data[i].name,
+      data[i].uid,
+      data[i].gid,
+      data[i].ppid
+    );
+
+    // Special case for floating point
+    // Elapsed Ticks
+    if (data[i].elapsed_ticks < 10){
+      printf(1, "0.00%d\t", data[i].elapsed_ticks);
+    } else if (data[i].elapsed_ticks < 100){
+      printf(1, "0.0%d\t", data[i].elapsed_ticks);
+    } else if (data[i].elapsed_ticks < 1000){
+      printf(1, "0.%d\t", data[i].elapsed_ticks);
+    }else{
+      printf(1, "%d.%d\t", data[i].elapsed_ticks/1000, data[i].elapsed_ticks%1000);
+    }
+
+    // CPU Total Ticks
+    if (data[i].CPU_total_ticks < 10){
+      printf(1, "0.00%d", data[i].CPU_total_ticks);
+    } else if (data[i].CPU_total_ticks < 100){
+      printf(1, "0.0%d", data[i].CPU_total_ticks);
+    } else if (data[i].CPU_total_ticks < 1000){
+      printf(1, "0.%d", data[i].CPU_total_ticks);
+    }else{
+      printf(1, "%d.%d", data[i].CPU_total_ticks/1000, data[i].CPU_total_ticks%1000);
+    }
+
+    printf(1, "\t%s\t%d\n",
+      data[i].state,
+      data[i].size
+    );
+  }
+
+  printf(1, "\n");
+  exit();
+}
+
+#endif // CS333_P2
diff --git a/runoff.list b/runoff.list
index 81930d9..58bf47a 100644
--- a/runoff.list
+++ b/runoff.list
@@ -85,6 +85,7 @@ halt.c
 uptime.c
 date.h
 date.c
+ps.c
 uproc.h
 testuidgid.c
 testuidgid.txt
diff --git a/syscall.c b/syscall.c
index 447611a..2a72562 100644
--- a/syscall.c
+++ b/syscall.c
@@ -115,6 +115,7 @@ extern int sys_getgid(void);
 extern int sys_getppid(void);
 extern int sys_setuid(void);
 extern int sys_setgid(void);
+extern int sys_getprocs(void);
 #endif
 
 static int (*syscalls[])(void) = {
@@ -151,6 +152,7 @@ static int (*syscalls[])(void) = {
 [SYS_getppid]   sys_getppid,
 [SYS_setuid]    sys_setuid,
 [SYS_setgid]    sys_setgid,
+[SYS_getprocs]  sys_getprocs
 #endif
 };
 
@@ -189,6 +191,7 @@ static char *syscallnames[] = {
   [SYS_getppid] "getppid",
   [SYS_setuid]  "setuid",
   [SYS_setgid]  "setgid",
+  [SYS_getprocs] "getprocs",
 #endif
 };
 #endif // PRINT_SYSCALLS
diff --git a/syscall.h b/syscall.h
index b7e1459..a7fd353 100644
--- a/syscall.h
+++ b/syscall.h
@@ -28,3 +28,4 @@
 #define SYS_getppid SYS_getgid+1
 #define SYS_setuid  SYS_getppid+1
 #define SYS_setgid  SYS_setuid+1
+#define SYS_getprocs SYS_setgid+1
diff --git a/sysproc.c b/sysproc.c
index e993f06..f351c87 100644
--- a/sysproc.c
+++ b/sysproc.c
@@ -6,6 +6,9 @@
 #include "memlayout.h"
 #include "mmu.h"
 #include "proc.h"
+#ifdef CS333_P2
+#include "uproc.h"
+#endif
 #ifdef PDX_XV6
 #include "pdx-kernel.h"
 #endif // PDX_XV6
@@ -153,4 +156,16 @@ sys_setgid(void)
   myproc()->gid = (uint)gid;
   return 0;
 }
-#endif
+
+int
+sys_getprocs(void)
+{
+  int max;
+  struct uproc* up;
+
+  if (argint(0, &max) < 0 || argptr(1, (void*)&up, sizeof(struct uproc) * max) < 0)
+    return -1;
+
+  return copyProc(max, up);
+}
+#endif // CS333_P2
diff --git a/user.h b/user.h
index eb480cf..2065a78 100644
--- a/user.h
+++ b/user.h
@@ -34,6 +34,7 @@ uint getgid(void);
 uint getppid(void);
 int  setuid(uint);
 int  setgid(uint);
+int  getprocs(uint, struct uproc*);
 #endif // CS333_P2
 
 // ulib.c
diff --git a/usys.S b/usys.S
index 090de9a..6ee47b8 100644
--- a/usys.S
+++ b/usys.S
@@ -36,3 +36,4 @@ SYSCALL(getgid)
 SYSCALL(getppid)
 SYSCALL(setuid)
 SYSCALL(setgid)
+SYSCALL(getprocs)
```

## *time* command

## Modifying the Console (procdumpP2P3P4)
```diff
diff --git a/proc.c b/proc.c
index 1317052..00520ff 100644
--- a/proc.c
+++ b/proc.c
@@ -7,6 +7,7 @@
 #include "proc.h"
 #include "uproc.h"
 #include "spinlock.h"
+#include <stdio.h>
 
 static char *states[] = {
   [UNUSED]    "unused",
@@ -577,7 +578,41 @@ kill(int pid)
 void
 procdumpP2P3P4(struct proc *p, char *state_string)
 {
-  cprintf("TODO for Project 2, delete this line and implement procdumpP2P3P4() in proc.c to print a row\n");
+  cprintf("%d\t%s\t\t%d\t%d\t%d\t",
+    p->pid,
+    p->name,
+    p->uid,
+    p->gid,
+    p->pid == 1 ? p->pid : p->parent->pid
+  );
+
+  // Elapsed
+  if ((ticks - (p->start_ticks)) < 10){
+    cprintf("0.00%d\t",(ticks - (p->start_ticks)) );
+  } else if((ticks - (p->start_ticks)) < 100) {
+    cprintf("0.0%d\t",(ticks - (p->start_ticks)) );
+  } else if((ticks - (p->start_ticks)) < 1000) {
+    cprintf("0.%d\t",(ticks - (p->start_ticks)) );
+  } else{
+    cprintf("%d.%d\t",(ticks - (p->start_ticks))/1000,(ticks - (p->start_ticks))%1000);
+  }
+
+  // CPU ticks
+  if (p->cpu_ticks_total < 10){
+    cprintf("0.00%d", p->cpu_ticks_total );
+  } else if(p->cpu_ticks_total < 100) {
+    cprintf("0.0%d",p->cpu_ticks_total );
+  } else if(p->cpu_ticks_total < 1000) {
+    cprintf("0.%d",p->cpu_ticks_total );
+  } else{
+    cprintf("%d.%d",p->cpu_ticks_total/1000,p->cpu_ticks_total%1000);
+  }
+
+  cprintf("\t%s\t%d\t",
+    states[p->state],
+    p->sz
+  );
+
   return;
 }
 #elif defined(CS333_P1)
```
