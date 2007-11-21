# Generated by h2py from /usr/include/sys/wait.h

# Included from standards.h
def _W_INT(i): return (i)

WUNTRACED = 0004
WNOHANG = 0100
_WSTOPPED = 0177
def WIFEXITED(stat): return ((_W_INT(stat)&0377)==0)

def WEXITSTATUS(stat): return ((_W_INT(stat)>>8)&0377)

def WTERMSIG(stat): return (_W_INT(stat)&0177)

def WSTOPSIG(stat): return ((_W_INT(stat)>>8)&0377)

WEXITED = 0001
WTRAPPED = 0002
WSTOPPED = 0004
WCONTINUED = 0010
WNOWAIT = 0200
WOPTMASK = (WEXITED|WTRAPPED|WSTOPPED|WCONTINUED|WNOHANG|WNOWAIT)
WSTOPFLG = 0177
WCONTFLG = 0177777
WCOREFLAG = 0200
WSIGMASK = 0177
def WWORD(stat): return (_W_INT(stat)&0177777)

def WIFCONTINUED(stat): return (WWORD(stat)==WCONTFLG)

def WCOREDUMP(stat): return (_W_INT(stat) & WCOREFLAG)


# Included from sys/types.h

# Included from sgidefs.h
_MIPS_ISA_MIPS1 = 1
_MIPS_ISA_MIPS2 = 2
_MIPS_ISA_MIPS3 = 3
_MIPS_ISA_MIPS4 = 4
_MIPS_SIM_ABI32 = 1
_MIPS_SIM_NABI32 = 2
_MIPS_SIM_ABI64 = 3
P_MYID = (-1)
P_MYHOSTID = (-1)

# Included from sys/bsd_types.h

# Included from sys/mkdev.h
ONBITSMAJOR = 7
ONBITSMINOR = 8
OMAXMAJ = 0x7f
OMAXMIN = 0xff
NBITSMAJOR = 14
NBITSMINOR = 18
MAXMAJ = 0x1ff
MAXMIN = 0x3ffff
OLDDEV = 0
NEWDEV = 1
MKDEV_VER = NEWDEV
def major(dev): return __major(MKDEV_VER, dev)

def minor(dev): return __minor(MKDEV_VER, dev)


# Included from sys/select.h
FD_SETSIZE = 1024
__NBBY = 8

# Included from string.h
NULL = 0L
NBBY = 8

# Included from sys/procset.h
P_INITPID = 1
P_INITUID = 0
P_INITPGID = 0

# Included from sys/signal.h
SIGHUP = 1
SIGINT = 2
SIGQUIT = 3
SIGILL = 4
SIGTRAP = 5
SIGIOT = 6
SIGABRT = 6
SIGEMT = 7
SIGFPE = 8
SIGKILL = 9
SIGBUS = 10
SIGSEGV = 11
SIGSYS = 12
SIGPIPE = 13
SIGALRM = 14
SIGTERM = 15
SIGUSR1 = 16
SIGUSR2 = 17
SIGCLD = 18
SIGCHLD = 18
SIGPWR = 19
SIGWINCH = 20
SIGURG = 21
SIGPOLL = 22
SIGIO = 22
SIGSTOP = 23
SIGTSTP = 24
SIGCONT = 25
SIGTTIN = 26
SIGTTOU = 27
SIGVTALRM = 28
SIGPROF = 29
SIGXCPU = 30
SIGXFSZ = 31
SIG32 = 32
SIGCKPT = 33
SIGRTMIN = 49
SIGRTMAX = 64
SIGPTINTR = 47
SIGPTRESCHED = 48
__sigargs = int
SIGEV_NONE = 128
SIGEV_SIGNAL = 129
SIGEV_CALLBACK = 130

# Included from sys/siginfo.h
ILL_ILLOPC = 1
ILL_ILLOPN = 2
ILL_ILLADR = 3
ILL_ILLTRP = 4
ILL_PRVOPC = 5
ILL_PRVREG = 6
ILL_COPROC = 7
ILL_BADSTK = 8
NSIGILL = 8
FPE_INTDIV = 1
FPE_INTOVF = 2
FPE_FLTDIV = 3
FPE_FLTOVF = 4
FPE_FLTUND = 5
FPE_FLTRES = 6
FPE_FLTINV = 7
FPE_FLTSUB = 8
NSIGFPE = 8
SEGV_MAPERR = 1
SEGV_ACCERR = 2
NSIGSEGV = 2
BUS_ADRALN = 1
BUS_ADRERR = 2
BUS_OBJERR = 3
NSIGBUS = 3
TRAP_BRKPT = 1
TRAP_TRACE = 2
NSIGTRAP = 2
CLD_EXITED = 1
CLD_KILLED = 2
CLD_DUMPED = 3
CLD_TRAPPED = 4
CLD_STOPPED = 5
CLD_CONTINUED = 6
NSIGCLD = 6
POLL_IN = 1
POLL_OUT = 2
POLL_MSG = 3
POLL_ERR = 4
POLL_PRI = 5
POLL_HUP = 6
NSIGPOLL = 6
SI_MAXSZ = 128
SI_USER = 0
SI_KILL = SI_USER
SI_QUEUE = -1
SI_ASYNCIO = -2
SI_TIMER = -3
SI_MESGQ = -4
SIG_NOP = 0
SIG_BLOCK = 1
SIG_UNBLOCK = 2
SIG_SETMASK = 3
SIG_SETMASK32 = 256
SA_ONSTACK = 0x00000001
SA_RESETHAND = 0x00000002
SA_RESTART = 0x00000004
SA_SIGINFO = 0x00000008
SA_NODEFER = 0x00000010
SA_NOCLDWAIT = 0x00010000
SA_NOCLDSTOP = 0x00020000
_SA_BSDCALL = 0x10000000
MINSIGSTKSZ = 512
SIGSTKSZ = 8192
SS_ONSTACK = 0x00000001
SS_DISABLE = 0x00000002

# Included from sys/ucontext.h
NGREG = 36
NGREG = 37
GETCONTEXT = 0
SETCONTEXT = 1
UC_SIGMASK = 001
UC_STACK = 002
UC_CPU = 004
UC_MAU = 010
UC_MCONTEXT = (UC_CPU|UC_MAU)
UC_ALL = (UC_SIGMASK|UC_STACK|UC_MCONTEXT)
CTX_R0 = 0
CTX_AT = 1
CTX_V0 = 2
CTX_V1 = 3
CTX_A0 = 4
CTX_A1 = 5
CTX_A2 = 6
CTX_A3 = 7
CTX_T0 = 8
CTX_T1 = 9
CTX_T2 = 10
CTX_T3 = 11
CTX_T4 = 12
CTX_T5 = 13
CTX_T6 = 14
CTX_T7 = 15
CTX_A4 = 8
CTX_A5 = 9
CTX_A6 = 10
CTX_A7 = 11
CTX_T0 = 12
CTX_T1 = 13
CTX_T2 = 14
CTX_T3 = 15
CTX_S0 = 16
CTX_S1 = 17
CTX_S2 = 18
CTX_S3 = 19
CTX_S4 = 20
CTX_S5 = 21
CTX_S6 = 22
CTX_S7 = 23
CTX_T8 = 24
CTX_T9 = 25
CTX_K0 = 26
CTX_K1 = 27
CTX_GP = 28
CTX_SP = 29
CTX_S8 = 30
CTX_RA = 31
CTX_MDLO = 32
CTX_MDHI = 33
CTX_CAUSE = 34
CTX_EPC = 35
CTX_SR = 36
CXT_R0 = CTX_R0
CXT_AT = CTX_AT
CXT_V0 = CTX_V0
CXT_V1 = CTX_V1
CXT_A0 = CTX_A0
CXT_A1 = CTX_A1
CXT_A2 = CTX_A2
CXT_A3 = CTX_A3
CXT_T0 = CTX_T0
CXT_T1 = CTX_T1
CXT_T2 = CTX_T2
CXT_T3 = CTX_T3
CXT_T4 = CTX_T4
CXT_T5 = CTX_T5
CXT_T6 = CTX_T6
CXT_T7 = CTX_T7
CXT_S0 = CTX_S0
CXT_S1 = CTX_S1
CXT_S2 = CTX_S2
CXT_S3 = CTX_S3
CXT_S4 = CTX_S4
CXT_S5 = CTX_S5
CXT_S6 = CTX_S6
CXT_S7 = CTX_S7
CXT_T8 = CTX_T8
CXT_T9 = CTX_T9
CXT_K0 = CTX_K0
CXT_K1 = CTX_K1
CXT_GP = CTX_GP
CXT_SP = CTX_SP
CXT_S8 = CTX_S8
CXT_RA = CTX_RA
CXT_MDLO = CTX_MDLO
CXT_MDHI = CTX_MDHI
CXT_CAUSE = CTX_CAUSE
CXT_EPC = CTX_EPC
CXT_SR = CTX_SR
SV_ONSTACK = 0x0001
SV_INTERRUPT = 0x0002
NUMBSDSIGS = (32)
def sigmask(sig): return (1L << ((sig)-1))

def sigmask(sig): return (1L << ((sig)-1))

SIG_ERR = (-1)
SIG_IGN = (1)
SIG_HOLD = (2)
SIG_DFL = (0)
NSIG = 65
MAXSIG = (NSIG-1)
NUMSIGS = (NSIG-1)
BRK_USERBP = 0
BRK_KERNELBP = 1
BRK_ABORT = 2
BRK_BD_TAKEN = 3
BRK_BD_NOTTAKEN = 4
BRK_SSTEPBP = 5
BRK_OVERFLOW = 6
BRK_DIVZERO = 7
BRK_RANGE = 8
BRK_PSEUDO_OP_BIT = 0x80
BRK_PSEUDO_OP_MAX = 0x3
BRK_CACHE_SYNC = 0x80
BRK_SWASH_FLUSH = 0x81
BRK_SWASH_SWTCH = 0x82
BRK_MULOVF = 1023

# Included from sys/resource.h
PRIO_MIN = -20
PRIO_MAX = 20
PRIO_PROCESS = 0
PRIO_PGRP = 1
PRIO_USER = 2
RUSAGE_SELF = 0
RUSAGE_CHILDREN = -1
RLIMIT_CPU = 0
RLIMIT_FSIZE = 1
RLIMIT_DATA = 2
RLIMIT_STACK = 3
RLIMIT_CORE = 4
RLIMIT_NOFILE = 5
RLIMIT_VMEM = 6
RLIMIT_RSS = 7
RLIMIT_AS = RLIMIT_VMEM
RLIM_NLIMITS = 8
RLIM32_INFINITY = 0x7fffffff
RLIM_INFINITY = 0x7fffffff
