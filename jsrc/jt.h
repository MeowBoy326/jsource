/* Copyright 1990-2010, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Definitions for jt ("jthis")                                            */

#if !SY_WINCE && (SYS & SYS_DOS)
#include <sys/stat.h>
#endif

#if  (SYS & SYS_UNIX)   /*   IVL   */
#include <sys/stat.h>
#endif

/*
All allocated and variable data for a J instance is accessed through
its JST structure.

Only simple constant data can be global and are shared by J instances.

Constants such as mtm are created in the dll process_attach. They are
the same for all instances and are never freed.  The dll is never
allowed to unload as we don't have code to free mtm et. al.

j.map must be checked to ensure no 'bad' global data exists. In
windows the global data that needs scrutiny is in the 0003: section.
*/

typedef struct rngparms {
  UF   rngF;          /* RNG: function to get the next random number     */
  I    rngS;          /* RNG: seeds                                      */
  UI*  rngV;          /* RNG: state vectors                              */
  UI4  rngM;          /* RNG: moduli                                     */
  S    rngI;          /* RNG: indices                                    */
 } RNGPARMS;

// should align this to cacheline bdy
typedef struct rngdata {
 RNGPARMS rngparms[5];  // parms for RNGs when used signly
 UI*  rngv;             /* RNG: rngV[rng]                                  */
 UI*  rngfxsv;          /* RNG: rngv for fixed seed (?.)                   */
 A    rngseed;          /* RNG: array seed                                 */
 S    rngi;             // RNG: current index into state array
 C    rngw;             /* RNG: # bits in a random #                       */
 C    rng;              /* RNG: generator selector                         */
 RNGPARMS rngparms0[5];  // parms for RNG 0
 } RNG;



// per-thread area.  Align on a 256B boundary to leave low 8 bits for flags (JTFLAGMSK is the list of bits)
// The first 2 cache lines is stacked on task switch
 struct __attribute__((aligned(JTFLAGMSK+1))) JTTstruct {
// task-initialized values
// these values are stacked and restored over the execution of a task
 A global;           // global symbol table inherit for task
 D cct;               // complementary comparison tolerance inherit for task
 I4 currslistx;    // index into slist of the current script being executed (or -1 if none) inherit for task
 union {  // this union is 4 bytes long
  UI4 ui4;    // all 4 flags at once, access as ui4
  struct {
   union {
    US cx_us;       // accessing both flags at once
    struct {
     C    pmctr;     // set when PM is running inherit
     UC   db;               /* debug flag; see 13!:0 inherit                          */
    } cx_c;        // accessing as bytes
   } cx;   // flags needed by unquote and jtxdefn
// ** here starts the part that is initialized after it is stacked
   union {
    US uq_us;       // accessing both flags at once
    struct {
     C    bstkreqd;   // set if we MUST create a stack entry for each named call clear for task
     B    spfreeneeded;     // When set, we should perform a garbage-collection pass clear for task
    } uq_c;        // accessing as bytes
   } uq;   // flags needed only by unquote
  } us;   // access as US
 } uflags;   // 
 UI4 ranks;            // low half: rank of w high half: rank of a  for IRS init for task
 S etxn;             // strlen(etx) but set negative to freeze changes to the error line
 S etxn1;            // last non-zero etxn                 migrated clear for task
 UI cstackmin;        // red warning for C stack pointer init for task
 struct ASGINFO {
  L *assignsym;       // symbol-table entry for the symbol about to be assigned
  A zombieval;    // the value that the verb result will be assigned to, if the assignment is safe and has inplaceable usecount and not read-only
 } asginfo;
 A locsyms;  // local symbol table, or dummy empty symbol table if none init for task
// end of cacheline 0
 A filler;            // UNUSED
 S nthreads;  // number of threads to use for primitives, or 0 if we haven't checked init for task
 S ntasks;     // number of hiprecs allowed, 0 if none
 I4 threadrecip16;  // reciprocal of nthreads, 16 bits of fraction init for task
 UI cstackinit;       // C stack pointer at beginning of execution init for task
 I bytes;            // bytes currently in use - used only during 7!:1 clear for task
 I bytesmax;         // high-water mark of "bytes" - used only during 7!:1 clear for task
 B foldrunning;      // 1 if fold is running (allows Z:) clear for task
 UC jerr;             // error number (0 means no error)    migrated  clear for task
 C recurstate;       // state of recursions through JDo migrated init for task
#define RECSTATEIDLE    0  // JE is inactive, waiting for work
#define RECSTATEBUSY    1  // JE is running a call from JDo
#define RECSTATEPROMPT  2  // JE is running, and is suspended having called the host for input
#define RECSTATERECUR   3  // JE is running and waiting for a prompt, and the host has made a recursive call to JDo (which must not prompt)
 C namecaching;     // 1=for script 2=on  clear for task
 B iepdo;            // 1 iff do iep clear for task
 UC jerr1;            // last non-zero jerr                 migrated  clear for task
// ** end of initialized part
 C xmode;            // extended integer operating mode inherit for task
 C boxpos;           // boxed output x-y positioning, low bits xxyy00 inherit for task
 A xmod;             // extended integer: the m in m&|@f inherit for task
 C pp[7];            // print precision (sprintf field for numeric output) inherit for task
 C glock;            // 0=unlocked, 1=perm lock, 2=temp lock inherit for task
// end of cacheline 1 (little used)
// *** end of the region saved at task startup

// everything after here persists over the life of the thread

// things needed for memory allocation
 A*   tnextpushp;       // pointer to empty slot in allocated-block stack.  When low bits are 00..00, pointer to previous block of pointers.  Chain in first block is 0
 struct {
  I ballo;              // negative number of bytes in free pool, but with zero-point biased so that - means needs garbage collection 
  A pool;             // pointer to first free block
 }    mfree[-PMINL+PLIML+1];      // pool info.  Use struct to keep cache footprint small
// cacheline 2 ends inside the pool struct (3 qwords extra)

// things needed by name lookup (unquote)
 LS *callstack;   // [1+NFCALL]; // named fn calls: stack.  Usually only a little is used
 I4 callstacknext;    /* named fn calls: current depth                   */
 I4 fcalln;           /* named fn calls: maximum permissible depth   migrated    */
 A curname;          // current name, an A block containing an NM
 A nvra;             // data blocks that are in execution somewhere - always non-virtual, always rank 1, AS[0] holds current pointer
 I4 parsercalls;      /* # times parser was called          migrated             */
 C fillv0len;   // length of fill installed in fillv0
// 3 bytes free
// end of cacheline 3
 I shapesink[SY_64?2:4];     // garbage area used as load/store targets of operations we don't want to branch around
// things needed for allocation of large blocks
 I mfreegenallo;        // Amount allocated through malloc, biased
 I malloctotal;    // net total of malloc/free performed in m.c only

// things needed by parsing
 PFRAME parserstackframe;  // 4 words  
// end of cacheline 4

// things needed by execution of certain verbs
 A idothash0;        // 2-byte hash table for use by i.    migrated
 A idothash1;        // 4-byte hash table for use by i.    migrated
 A fill;             // user fill atom
 C* fillv;            /* fill value                                      */
 C fillv0[sizeof(Z)];/* default fill value                              */
 RNG *rngdata;    // separately allocated block for RNG

// seldom-used fields
 I malloctotalhwmk;  // highest value since most recent 7!:1
// end of cacheline 5
 A* tstacknext;       // if not 0, points to the recently-used tstack buffer, whose chain field points to tstacknext  
 A* tstackcurr;       // current allocation, holding NTSTACK bytes+1 block for alignment.  First entry points to next-lower allocation   
 C *etx;  // [1+NETX];      // display text for last error (+1 for trailing 0)
 void *dtoa;             /* use internally by dtoa.c                        */
 I deprecct;  // number of deprecation  errors to display, -1 to emsg
 A deprecex;  // list of INTs of messages not to display
 PSTK initparserstack[1];  // stack used for messages when we don't have a real one

// debugging info
 DC sitop;            /* pointer to top of SI stack                                 */
 I getlasterror;     /* DLL stuff                                       */
 I dlllasterror;     /* DLL stuff                                       */
//  end of cacheline 6
// stats I totalpops;
// stats I nonnullpops;
#if !C_CRC32C
 I    hin;              /* used in dyad i. & i:                            */
 I*   hiv;              /* used in dyad i. & i:                            */
#endif
#if !(C_CRC32C && SY_64)
 I    min;              /* the r result from irange                        */
#endif
#if !(C_CRC32C && SY_64)
 UIL  ctmask;           /* 1 iff significant wrt ct; for i. and i:         */
#endif
#if !USECSTACK
 I4   fdepi;            /* fn calls: current depth                         */
 I4   fdepn;            /* fn calls: maximum permissible depth             */
#else
#endif

};
typedef struct JTTstruct JTT;
typedef JTT* JJ;  // thread-specific part of struct
#define LGTHREADBLKSIZE 9  // log2 of threaddata

// Must be aligned on a 256-byte boundary for flags; but better to align on a DRAM page boundary to avoid precharge
typedef struct JSTstruct {
// shared area.  To implement critical regions we use locks in this struct.  To get best use of cache, we put the lock for
// a function in the same cacheline as the data used by the function.  To avoid false cacheline sharing, we have to make sure that only
// very-seldom-referenced data inhabits a cacheline that contains a lock.
// Cacheline 0 is special, because it contains adbreak, which is checked very frequently by all threads.  Therefore, to keep this cacheline
// in S state we must have everything else in the line be essentially read-only.
 C* adbreak;		// must be first! pointer to mapped shared file break flag.  Inits to jst->breakbytes; switched to file area if a breakfile is created
 C* adbreakr;         // read location: same as adbreak, except that when we are ignoring interrupts it points to a read-only byte of 0
 S qlock;            // lock used for quiescing all tasks
 A implocref[2];     // references to 'u.'~ and 'v.'~, marked as implicit locatives
 A slist;            // boxed list of filenames used in right arg to 0!:, matches the entries made in sn field of L blocks
 B assert;           /* 1 iff evaluate assert. statements               */
 B stch;             /* enable setting of changed bit                   */
 C asgzomblevel;     // 0=do not assign zombie name before final assignment; 1=allow premature assignment of complete result; 2=allow premature assignment even of incomplete result   
 UC dbuser;           /* user-entered value for db          migrated             */
 US breakbytes;    // first byte: used for signals when there is no mapped breakfile.  Bit 0=ATTN request, bit 1=BREAK request
 void *heap;            // heap handle for large allocations
 I mmax;             /* space allocation limit                          */
// end of cacheline 0

// Cacheline 1: DLL variables
 A cdarg;            /* table of 15!:0 parsed left arguments            */
 A cdhash;           // hash table of cdstr strings into cdarg
 A cdhashl;          // hash table of cdstr strings into module index
 A cdstr;            // strings for cdarg/cdhashl
 S cdlock;           // r/w lock for cdarg/cdhashl
 // rest of cacheline used only in exceptional paths
// 6 bytes free
#if MEMAUDIT & 2
 C audittstackdisabled;   // set to 1 to disable auditing
#endif
 I* breakfh;          /* win break file handle                           */
 I* breakmh;          /* win break map handle                            */
 C *breakfn;  // [NPATH];   /* break file name                                 */
// end of cacheline 1

// Cacheline 2: J symbol pool
 L *sympv;           // symbol pool array.  This is offset LAV0 into the allocated block.  Symbol 0 is used as the root of the free chain
 S symlock;          // r/w lock for symbol pool
 // rest of cacheline used only in exceptional paths
// 6 bytes free
// front-end interface info
 C *capture;          // capture output for python->J etc.  scaf could be byte?
 void *smdowd;         /* sm.. sm/wd callbacks set by JSM()               */
 void *sminput;
 void *smoutput;
 I filler2[2];    // 2 words free
// end of cacheline 2

// Cacheline 3: Locales
 A stnum;            // numbered locale numbers or hash table - rank 1, holding symtab pointer for each entry.  0 means empty
 A stloc;            // named locales symbol table
 S stlock;           // r/w lock for stnum.  stloc is never modified, so we use the ->lock field of stloc to lock that table
 C locsize[2];       /* size indices for named and numbered locales     */
 C baselocale[4];    // will be "base"
 // rest of cacheline used only in exceptional paths
 void *smpoll;           /* re-used in wd                                   */
 void *opbstr;           /* com ptr to BSTR for captured output             */
 I filler3[2];    // 2 words free
// end of cacheline 3

// Cacheline 4: Files
 A flkd;             /* file lock data: number, index, length           */
 A fopafl;         // table of open filenames; in each one AM is the file handle and the lock is used
// obsolete  A fopf;             /* open files corresp. file numbers                */
 S flock;            // r/w lock for flkd/fopa/fopf
 // rest of cacheline used only in exceptional paths
 UC sm;               /* sm options set by JSM()                         */
 C smoption;         // wd options, see comment in jtwd
 UC int64rflag;       /* com flag for returning 64-bit integers          */
 UC transposeflag;    /* com flag for transposed arrays                  */
// 2 bytes free
 void *iomalloc;   // address of block, if any, allocated in io.c to be returned to the FE
 I iomalloclen;   // length of the allocated block (in case we can reuse it)
 UI qtstackinit;      // jqt front-end C stack pointer
 I filler4[2];      // 2 words free
// end of cacheline 4


// Cacheline 5: User symbols, also used for front-end locks
 A sbu;              /* SB data for each unique symbol                  */
 A sbhash;              // hashtable for symbols
 A sbstrings;          // string data for symbols
 S sblock;           // r/w lock for sbu
 S felock;           // r/w lock for host functions, accessed only at start/end of immex
 // rest of cacheline used only in exceptional paths
 I4 outmaxafter;      /* output: maximum # lines after truncation        */
 I4 outmaxbefore;     /* output: maximum # lines before truncation       */
 I4 outmaxlen;        /* output: maximum line length before truncation   */
 I peekdata;         /* our window into the interpreter                 */
 A iep;              /* immediate execution phrase                      */
// obsolete  A xep;              /* exit execution phrase                           */
 A pma;              /* perf. monitor: data area                        */
// end of cacheline 5

// Cacheline 6: debug, which is written so seldom that it can have read-only data
 A dbstops;          /* stops set by the user                           */
 A dbtrap;           // trap sentence, execute when going into suspension
 S dblock;           // lock on dbstops/dbtrap
 // rest of cacheline is essentially read-only
 B retcomm;          /* 1 iff retain comments and redundant spaces      */
 UC outeol;           /* output: EOL sequence code, 0, 1, or 2             */
 UI4 baselocalehash;   // name hash for base locale
 A evm;              /* event messages                                  */
 I igemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for integer matrix product.  _1 means 'never'   scaf could be shorter
 I dgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for float matrix product.  _1 means 'never'
 I zgemm_thres;      // used by cip.c: when m*n*p exceeds this, use BLAS for complex matrix product.  _1 means 'never'
 A emptylocale;      // locale with no symbols, used when not running explicits, or to avoid searching the local syms
// end of cacheline 6

// Cacheline 7: no locks, essentially read-only
 UI4 cachesizes[3];  // [0]: size of fastest cache  [1]: size of largest cache private to each core  [2]: size of largest cache shared by all cores
 B sesm;             /* whether there is a session manager             */
 C nfe;              /* 1 for J native front end                    */
 C oleop;            /* com flag to capture output                    */
 UC cstacktype;  /* cstackmin set during 0: jt init  1: passed in JSM  2: set in JDo  */
 UC seclev;           /* security level                                  */
 UC disp[7];          /* # different verb displays                       */
 C bx[11];               /* box drawing characters                          */
// 5 bytes free
 A zpath;    // path 'z', used for all initial paths
 A p4792;            // pointer to p: i. 4792, filled in on first use
 D tssbase;          /* initial 6!:0''                            */
// end of cacheline 7

 JTT threaddata[MAXTASKS] __attribute__((aligned(JTFLAGMSK+1)));
} JST;   // __attribute__((aligned(JTALIGNBDY))) not allowed
typedef JST* JS;  // shared part of struct

#if 0 // used only for direct locale numbering
 I*   numlocdelqh;      // head of deleted queue, waiting for realloc
 I    numlocdelqn;      // number of blocks on the deleted queue  could be UI4
 I*   numlocdelqt;       // tail of deleted queue
 I*   numloctbl;         // pointer to data area for locale-number to locale translation
 UI4  numlocsize;       // AN(JT(jt,stnum))
#endif

#undef J
#define J JJ
#if MAXTASKS>1  // for multithreading
#define JJTOJ(jj) ((JS)((I)(jj)&-JTALIGNBDY))
#else
#define JJTOJ(jj) ((JS)((I)(jj)-offsetof(struct JSTstruct,threaddata)))
#endif
#define JT(p,n) JJTOJ(p)->n
#define INITJT(p,n) (p)->n   // in init functions, jjt points to the JS block and we use this to reference components
#define MTHREAD(jt) (&jt->threaddata[0])   // master thread for shared jt
#define THREADID(jt) ((((I)(jt)&(JTALIGNBDY-1))>>LGTHREADBLKSIZE)-(offsetof(struct JSTstruct, threaddata[0])>>LGTHREADBLKSIZE))  // thread number from jt
enum {xxxx = 1/(offsetof(struct JSTstruct, threaddata[MAXTASKS])<=JTALIGNBDY) };  // assert not too many threads
enum {xxxxx = 1/(offsetof(struct JSTstruct, threaddata[1])-offsetof(struct JSTstruct, threaddata[0])==((I)1<<LGTHREADBLKSIZE)) };  // assert size of threaddata what we expected
