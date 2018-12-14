/* Copyright 1990-2007, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Symbol Table                                                            */

#include "j.h"


/* a symbol table aka locale is a type INT vector                          */
/* the length is prime and is one of ptab[i]                               */
/* zero elements mean unused entry                                         */
/* non-zero elements are indices in the global symbol pool and             */
/*     are head pointers to a linked list                                  */
/* the first element is symbol pool index for locale info                  */

/* the global symbol pool is a type INT matrix                             */
/* the number of columns is symcol=ceiling(sizeof(L)/sizeof(I))            */
/* elements are interpreted per type L (see jtype.h)                       */
/*  A name - A name on LHS of assignment or locale name                    */
/*  A val  - value        or locale search path                            */
/*  I sn   - script index                                                  */
/*  I flag - various flags                                                 */
/*  I next - pointer to   successor in linked list                         */
/*  I prev - pointer to predecessor in linked list                         */
/*           if no predecessor then pointer to hash table entry, and       */
/*           flag will include LHEAD                                       */
/* a stack of free entries is kept using the next pointer                  */
/* jt->symp:     symbol pool array                                         */
/* jt->sympv:    symbol pool array pointer, (L*)AV(jt->symp)               */
/* jt->symindex: symbol table index (monotonically increasing)             */

/* numbered locales:                                                       */
/* jt->stnum: -1 means free; others are numbers in use                     */
/* jt->stptr:  0 means free; others are symbol tables                      */
/* jt->stused: # entries in stnum/stptr in use                             */
/* jt->stmax:  1 + maximum number extant                                   */

/* named locales:                                                          */
/* jt->stloc:  locales symbol table                                        */


static I symcol=(sizeof(L)+SZI-1)/SZI;

B jtsymext(J jt,B b){A x,y;I j,m,n,s[2],*v,xn,yn;L*u;
 if(b){y=jt->symp; j=allosize(y)+NORMAH*SZI; n=*AS(y); yn=AN(y);}  // extract allo size from header (approx)
 else {            j=((I)1)<<12;            n=1;      yn=0;    }
 m=j<<1;                              /* new size in bytes           */
 m-=SZI*(NORMAH+2);                  /* less array overhead         */
 m/=symcol*SZI;                             /* new # rows                  */
 s[0]=m; s[1]=symcol; xn=m*symcol;          /* new pool array shape        */
 GATV(x,INT,xn,2,s); v=AV(x);                 /* new pool array              */
 if(b)ICPY(v,AV(y),yn);                     /* copy old data to new array  */
 memset(v+yn,C0,SZI*(xn-yn));               /* 0 unused area for safety    */
 u=n+(L*)v; j=1+n;
 DO(m-n-1, u++->next=j++;);                 /* build free list extension   */
 if(b)u->next=jt->sympv->next;              /* push extension onto stack   */
 ((L*)v)->next=n;                           /* new stack top               */
 ras(x); jt->symp=x;                           /* preserve new array          */
 jt->sympv=(L*)AV(x);                       /* new array value ptr         */
 if(b)fa(y);                                /* release old array           */
 R 1;
}    /* 0: initialize (no old array); 1: extend old array */

// ht->hashtable slot; allocate new symbol, install as head/tail of hash chain, with previous chain appended
// if tailx==0, append at head (immediately after *hv); if tailx!=0, append to tail.  If queue is empty, tail is always 0
// result is new symbol
L* jtsymnew(J jt,I*hv, I tailx){I j;L*u,*v;
 while(!(j=jt->sympv->next))RZ(symext(1));  /* extend pool if req'd        */
 jt->sympv->next=(j+jt->sympv)->next;       /* new top of stack            */
 u=j+jt->sympv;  // the new symbol.  u points to it, j is its index
 if(tailx) {L *t=tailx+jt->sympv;
  // appending to tail.  Queue is known to be nonempty
  u->next=0;u->prev=tailx;t->next=j;  // it's always the end: point to next & prev, and chain from prev
 }else{
  // appending to head.
  if(u->next=*hv){v=*hv+jt->sympv; v->prev=j; v->flag^=LHEAD;}  // chain old queue to u; if not empty, backchain old head to new one, clear old head flag, 
  u->prev=(I)hv; u->flag=LHEAD;  // backchain pointer in head points to the hashtable slot; new element is the head
  *hv=j;   // set new head
 }
 R u;
}    /* allocate a new pool entry and insert into hash table entry hv */

// u points to an L; free it, and do fa() on the name and value.
// If the entry is LPERMANENT, don't free it; just fa() the value and clear val pointer to 0
// Return 0 on error; otherwise 2, and turn on bit 0 if an ACV was freed, provided it was not xyuvmn
B jtsymfree(J jt,L*u){I q; B z=2;
 if(!(u->name->flag&NMDOT)&&u->val&&AT(u->val)&(VERB|ADV|CONJ))z|=1;  // flag if we are deleting an ACV
 if(!(u->flag&LPERMANENT)){
  // If the symbol is not PERMANENT, unchain it from its hashchain, install as head of free list, clear the name
  q=u->next;
  if(q)(q+jt->sympv)->prev=u->prev;
  if(LHEAD&u->flag){*(I*)u->prev=q; if(q)(q+jt->sympv)->flag|=LHEAD;}
  else (u->prev+jt->sympv)->next=q;
  u->next=jt->sympv->next;                   /* point to old top of stack   */
  jt->sympv->next=u-jt->sympv;               /* new          top of stack   */
  fa(u->name); u->name=0;                    /* zero out data fields        */
  u->sn=u->flag=u->prev=0;      // zero other fields when returning to free pool
 }
 // For all symbols, free the value, clear the pointer to it
 fa(u->val);u->val=0; 
 R z;
}    /* free pool entry pointed to by u */



static SYMWALK(jtsymfreehax, B,B01,100,1, 1, {I mod=symfree(d); RZ(mod); jt->arg|=mod;})   /* free pool table entries      */
// This visits every symbol and frees it.  BUT it doesn't visit symbols that do not have nonnull
// name and value fields.  But this is OK: You can't have a value without a name; and the only
// way to have a name without a value is if the name is PERMANENT and either uninitialized or deleted.
// In either case, we need to leave the name undisturbed.
// If we freed a non-DOT ACV, invalidate the names
F1(jtsymfreeha){jt->arg=0; A z=jtsymfreehax(jt,w); jt->modifiercounter+=jt->arg&1; R z;}

B jtsymfreeh(J jt,A w,L*v){I*wv;L*u;
 wv=AV(w);
 ASSERTSYS(*wv,"symfreeh");
 u=*wv+jt->sympv; 
 RZ(symfree(u));   // free the locale name and path.  Since these are never modifiers we don't need to increment moodifiercounter
 RZ(symfreeha(w));
 memset(wv,C0,AN(w)*SZI);
 fa(w);
 if(v){v->val=0; RZ(symfree(v));}  // no need to inc modifiercounter when freeing a NAME
 R 1;
}    /* free entire hash table w, (optional) pointed by v */


static SYMWALK(jtsympoola, I,INT,100,1, 1, *zv++=j;)

F1(jtsympool){A aa,*pu,q,x,y,*yv,z,*zv;I i,j,n,*u,*v,*xv;L*pv;
 RZ(w); 
 ASSERT(1==AR(w),EVRANK); 
 ASSERT(!AN(w),EVLENGTH);
 GAT(z,BOX,3,1,0); zv=AAV(z);
 n=*AS(jt->symp); pv=jt->sympv;
 GATV(x,INT,n*6,2,0); *AS(x)=n; *(1+AS(x))=6; xv= AV(x); zv[0]=x;
 GATV(y,BOX,n,  1,0);                         yv=AAV(y); zv[1]=y;
 for(i=0;i<n;++i,++pv){         /* per pool entry       */
  *xv++=i;   // sym number
  *xv++=(q=pv->val)?LOWESTBIT(AT(pv->val)):0;  // type: only the lowest bit.  Must allow SYMB through
  *xv++=pv->flag;  // flag
  *xv++=pv->sn;    
  *xv++=pv->next;
  *xv++=pv->prev;
  RZ(*yv++=(q=pv->name)?rifvs(sfn(1,q)):mtv);
 }
 // Allocate box 3: locale name
 GATV(y,BOX,n,1,0); yv=AAV(y); zv[2]=y;
 DO(n, yv[i]=mtv;);
 n=AN(jt->stloc); v=AV(jt->stloc); 
 for(i=0;i<n;++i)if(j=v[i]){    /* per named locales    */
  x=(j+jt->sympv)->val; 
  RZ(yv[j]=yv[*AV(x)]=aa=rifvs(sfn(1,LOCNAME(x))));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 n=AN(jt->stptr); pu=AAV(jt->stptr);
 for(i=0;i<n;++i)if(x=pu[i]){   /* per numbered locales */
  RZ(      yv[*AV(x)]=aa=rifvs(sfn(1,LOCNAME(x))));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 if(x=jt->local){               /* per local table      */
  RZ(      yv[*AV(x)]=aa=rifvs(cstr("**local**")));
  RZ(q=sympoola(x)); u=AV(q); DO(AN(q), yv[u[i]]=aa;);
 }
 RETF(z);
}    /* 18!:31 symbol pool */

// a is A for name, g is symbol table
// result is L* address of the symbol-table entry for the name, or 0 if not found
L*jtprobe(J jt,I l,C*string,UI4 hash,A g){
 RZ(g);
 I symx=AV(g)[SYMHASH(hash,AN(g)-SYMLINFOSIZE)];  // get index of start of chain
 while(1){
  if(!symx)R 0;  // if chain empty or ended, not found
  L *sym=jt->sympv+symx;
  if(l==NAV(sym->name)->m&&!memcmp(string,NAV(sym->name)->s,l))R sym->val?sym:0;     // (1) exact match - if there is a value, use this slot, else say not found
  symx=sym->next;   // mismatch - step to next
 }
}

// a is A for name; result is L* address of the symbol-table entry in the local symbol table, if there is one
// If the value is empty, return 0 for not found
L *jtprobelocal(J jt,A a){NM*u;I b,bx;
 // If there is bucket information, there must be a local symbol table, so search it
 RZ(a);u=NAV(a);  // u->NM block
 if(b = u->bucket){
  I lx = AV(jt->local)[b];  // index of first block if any
  if(0 > (bx = ~u->bucketx)){
   // positive bucketx (now negative); that means skip that many items and then do name search
   I m=u->m; C* s=u->s;  // length/addr of name from name block
   while(0>++bx){lx = jt->sympv[lx].next;}
   // Now lx is the index of the first name that might match.  Do the compares
   while(lx) {L* l = lx+jt->sympv;  // symbol entry
    if(m==NAV(l->name)->m&&!memcmp(s,NAV(l->name)->s,m))
     {R l->val?l : 0;}
    lx = l->next;
   }
   R 0;  // no match.
  } else {L* l = lx+jt->sympv;  // fetch hashchain headptr, point to L for first symbol
   // negative bucketx (now positive); skip that many items, and then you're at the right place
   while(bx--){l = l->next+jt->sympv;}
   R l->val?l:0;
  }
 } else {
  // No bucket information, do full search
  R jt->local?probe(NAV(a)->m,NAV(a)->s,NAV(a)->hash,jt->local) : 0;
 }
}

// a is A for name; result is L* address of the symbol-table entry in the local symbol table
// If not found, one is created
L *jtprobeislocal(J jt,A a){NM*u;I b,bx;
 // If there is bucket information, there must be a local symbol table, so search it
 RZ(a);u=NAV(a);  // u->NM block
 if(b = u->bucket){
  I lx = AV(jt->local)[b];  // index of first block if any
  if(0 > (bx = ~u->bucketx)){
   // positive bucketx (now negative); that means skip that many items and then do name search
   I m=u->m; C* s=u->s;  // length/addr of name from name block
   I tx = lx;  // tx will hold the address of the last item in the chain, in case we have to add a new symbol
   L* l;

   while(0>++bx){tx = lx; lx = jt->sympv[lx].next;}
   // Now lx is the index of the first name that might match.  Do the compares
   while(lx) {
    l = lx+jt->sympv;  // symbol entry
    if(m==NAV(l->name)->m&&!memcmp(s,NAV(l->name)->s,m)){R l;}  // return if name match
    tx = lx; lx = l->next;
   }
   // not found, create new symbol.  If tx is 0, the queue is empty, so adding at the head is OK; otherwise add after tx
   RZ(l=symnew(&AV(jt->local)[b],tx)); 
   ras(a); l->name=a;  // point symbol table to the name block, and increment its use count accordingly
   R l;
  } else {L* l = lx+jt->sympv;  // fetch hashchain headptr, point to L for first symbol
   // negative bucketx (now positive); skip that many items, and then you're at the right place
   while(bx--){l = l->next+jt->sympv;}
   R l;  // return 
  }
 } else {
  // No bucket information, do full search
  R probeis(a,jt->local);
 }
}



// a is A for name
// g is symbol table to use
// result is L* symbol-table entry to use
// if not found, one is created
L*jtprobeis(J jt,A a,A g){C*s;I*hv,m,tx;L*v;NM*u;
 u=NAV(a); m=u->m; s=u->s; hv=AV(g)+SYMHASH(u->hash,AN(g)-SYMLINFOSIZE);  // get bucket number among the hash tables
 if(tx=*hv){                                 /* !*hv means (0) empty slot    */
  v=tx+jt->sympv;
  while(1){                               
   u=NAV(v->name);
   if(m==u->m&&!memcmp(s,u->s,m))R v;    // (1) exact match - may or may not have value
   if(!v->next)break;                                /* (2) link list end */
   v=(tx=v->next)+jt->sympv;
  }
 }
 // not found, create new symbol.  If tx is 0, the queue is empty, so adding at the head is OK; otherwise add after tx
 RZ(v=symnew(hv,tx)); 
 ras(a); v->name=a;  // point symbol table to the name block, and increment its use count accordingly
 R v;
}    /* probe for assignment */

// look up a non-locative name using the locale path
// g is the current locale, l/string=length/name, hash is the hash for it
// result is L* symbol-table slot for the name, or 0 if none
L*jtsyrd1(J jt,I l,C *string,UI4 hash,A g){A*v,x,y;L*e;
// if(b&&jt->local&&(e=probe(NAV(a)->m,NAV(a)->s,NAV(a)->hash,jt->local))){av=NAV(a); R e;}  // return if found local
 RZ(g);  // make sure there is a locale...
 if(e=probe(l,string,hash,g))R e;  // and if the name is defined there, use it
 RZ(y = LOCPATH(g));   // Not found in locale.  We must use the path
 v=AAV(y); 
 // in LOCPATH the 'shape' of each string is used to store the bucketx
 DO(AN(y), x=v[i]; if(e=probe(l,string,hash,stfindcre(AN(x),CAV(x),AS(x)[0])))break;);  // return when name found.  Create path locale if it does not exist
 R e;  // fall through: not found
}    /* find name a where the current locale is g */ 

// u is address of indirect locative: in a__b__c, it points to the b
// n is the length of the entire locative (4 in this example)
// result is address of symbol table to use for name lookup (if not found, it is created)
static A jtlocindirect(J jt,I n,C*u,UI4 hash){A x,y;C*s,*v,*xv;I k,xn;
 A g;  // current locale.
 L *e=0;  // value looked up
 s=n+u;   // s->end+1 of name
 while(u<s){
  v=s; while('_'!=*--v); ++v;  // v->start of last indirect locative
  k=s-v; s=v-2;    // k=length of indirect locative; s->end+1 of next name if any
  if(!e){  // first time through
   if(jt->local)e=probe(k,v,hash,jt->local);  // look up local first
   if(!e)e=syrd1(k,v,hash,jt->global);
  }else e=syrd1(k,v,(UI4)nmhash(k,v),g);   // look up later indirect locatives, yielding an A block for a locative
  ASSERTN(e,EVVALUE,nfs(k,v));  // verify found
  y=e->val;    // y->A block for locale
  ASSERTN(!AR(y),EVRANK,nfs(k,v));   // verify atomic
  if(AT(y)&((INT|B01)/C_LE)){R stfindcre(-1,0,IAV(y)[0]);}  // if atomic integer, look it up (but don't create) - will fail if locale not found
  ASSERTN(BOX&AT(y),EVDOMAIN,nfs(k,v));  // verify box
  I bucketx;
  x=AAV0(y); if(!AR(x)&&AT(x)&((INT|B01)/C_LE)) {
   // Boxed integer - use that as bucketx
   xn=-1; bucketx=IAV(x)[0];  // signal numeric locale, fetch the number
  }else{
   xn=AN(x); xv=CAV(x);   // x->boxed contents, xn=length, xv->string
   ASSERTN(1>=AR(x),EVRANK,nfs(k,v));   // verify list (or atom)
   ASSERTN(xn,EVLENGTH,nfs(k,v));   // verify not empty
   ASSERTN(LIT&AT(x),EVDOMAIN,nfs(k,v));  // verify string
   ASSERTN(vlocnm(xn,xv),EVILNAME,nfs(k,v));  // verify legal name
   bucketx=BUCKETXLOC(xn,xv);
  }
  RZ(g=stfindcre(xn,xv,bucketx));  // find st for the name
 }
 R g;
}

// look up known locative name; return starting locale of locative, or 0 if error
A jtsybaseloc(J jt,A a) {I m,n;NM*v;
 n=AN(a); v=NAV(a); m=v->m;
 // Locative: find the indirect locale to start on, or the named locale, creating the locale if not found
 R NMILOC&v->flag?locindirect(n-m-2,2+m+v->s,(UI4)v->bucketx):stfindcre(n-m-2,1+m+v->s,v->bucketx);
}

// like syrd, but starting with the locale looked up.  Does not set the local-name flag in the result
// all obsolete, subsumed into unquote
L* jtsyrdfromloc(J jt, A a,A g) {
 if((I)g&1) {L *e;
  // If there is a local symbol table, search it first
  if(e = probelocal(a)){R e;}
  g=jt->global;  // Start with the current locale
 }
 R syrd1(NAV(a)->m,NAV(a)->s,NAV(a)->hash,g);  // Not local: look up the name starting in locale g
}

// look up a name (either simple or locative) using the full name resolution
// result is symbol-table slot for the name if found, or 0 if not found
L*jtsyrd(J jt,A a){A g;
 RZ(a);
 if(!(NAV(a)->flag&(NMLOC|NMILOC))){L *e;
  // If there is a local symbol table, search it first
  if(e = probelocal(a)){R e;}  // return flagging the result if local
  g=jt->global;  // Start with the current locale
 } else RZ(g=sybaseloc(a));
 R syrd1(NAV(a)->m,NAV(a)->s,NAV(a)->hash,g);  // Not local: look up the name starting in locale g
}


static A jtdllsymaddr(J jt,A w,C flag){A*wv,x,y,z;I i,n,*zv;L*v;
 RZ(w);
 n=AN(w); wv=AAV(w); 
 ASSERT(!n||BOX&AT(w),EVDOMAIN);
 GATV(z,INT,n,AR(w),AS(w)); zv=AV(z); 
 for(i=0;i<n;++i){
  x=wv[i]; v=syrd(nfs(AN(x),CAV(x))); 
  ASSERT(v,EVVALUE);
  y=v->val;
  ASSERT(NOUN&AT(y),EVDOMAIN);
  zv[i]=flag?(I)AV(y):(I)v;
 }
 RETF(z);
}    /* 15!:6 (0=flag) or 15!:14 (1=flag) */

F1(jtdllsymget){R dllsymaddr(w,0);}
F1(jtdllsymdat){R dllsymaddr(w,1);}

// look up the name w using full name resolution.  Return the value if found, abort if not found or invalid name
F1(jtsymbrd){L*v; RZ(w); ASSERTN(v=syrd(w),EVVALUE,w); R v->val;}

// look up name w, return value unless locked or undefined; then return just the name
F1(jtsymbrdlocknovalerr){A y;L *v;
 if(!(v=syrd(w))){
  // no value.  Could be undefined name (no error) or some other error including value error, which means error looking up an indirect locative
  // If error, abort with it; if undefined, return a reference to the undefined name
  RE(0);   // if not simple undefined, error
  R nameref(w);  // return reference to undefined name
 }
 // no error.  Return the value unless locked function
 y=v->val;
 R FUNC&AT(y)&&(jt->uflags.us.cx.cx_c.glock||VLOCK&FAV(y)->flag)?nameref(w):y;
}

// Same, but value error if name not defined
F1(jtsymbrdlock){A y;
 RZ(y=symbrd(w));
 R FUNC&AT(y)&&(jt->uflags.us.cx.cx_c.glock||VLOCK&FAV(y)->flag)?nameref(w):y;
}


// w is a value, v is the symbol-table entry about to be assigned
// return 0 if there is an attempt to reassign the currently-executing name
B jtredef(J jt,A w,L*v){A f;DC c,d;
 // find the most recent DCCALL, exit if none
 d=jt->sitop; while(d&&!(DCCALL==d->dctype&&d->dcj))d=d->dclnk; if(!(d&&DCCALL==d->dctype&&d->dcj))R 1;
 if(v==(L*)d->dcn){  // if the saved jt->cursymb (from the unquote lookup) matches the name being assigned...
  // insist that the redefinition have the same type, and the same explicit character
  f=d->dcf;
  ASSERTN(TYPESEQ(AT(f),AT(w))&&(CCOLON==FAV(f)->id)==(CCOLON==FAV(w)->id),EVSTACK,d->dca);
  d->dcf=w;
  // If we are redefining the executing explicit definition during debug, remember that.  We will use it to reload the definition.
  // Reassignment outside of debug waits until the name is off the stack
  if(CCOLON==FAV(w)->id){jt->redefined=(I)v; jt->cxspecials=1;}
  // Erase any stack entries after the redefined call
  c=jt->sitop; while(c&&DCCALL!=c->dctype){c->dctype=DCJUNK; c=c->dclnk;}
 }
 // Don't allow redefinition of a name that is suspended higher up on the stack
 c=d; while(c=c->dclnk){ ASSERTN(!(DCCALL==c->dctype&&v==(L*)c->dcn),EVSTACK,c->dca);}
 R 1;
}    /* check for changes to stack */

// find symbol entry for name a in symbol table g; this is known to be global assignment
// the name a may require lookup through the path; once we find the locale, we search only in it
// Result is &symbol-table entry, if one exists
L* jtprobeisquiet(J jt,A a){A g;
 I n=AN(a); NM* v=NAV(a); I m=v->m;  // n is length of name, v points to string value of name, m is length of non-locale part of name
 if(n==m){g=jt->global;}   // if not locative, define in default locale
 else{C* s=1+m+v->s; if(!(g=NMILOC&v->flag?locindirect(n-m-2,1+s,(UI4)v->bucketx):stfindcre(n-m-2,s,v->bucketx))){RESETERR; R 0;}}  // if locative, find the locale for the assignment; error is not fatal
 R probeis(a, g);  // return pointer to slot, creating one if not found
}


// assign symbol: assign name a in symbol table g to the value w (but g is special if jt->assignsym is nonnull)
// Non-error result is unused (mark)
A jtsymbis(J jt,A a,A w,A g){A x;I m,n,wn,wr,wt;NM*v;L*e;V*wv;
 RZ(a&&w&&g);
 // If we have an assignsym, we have looked this name up already, so just use the symbol-table entry found then
 // in this case g is the type field of the name being assigned; and jt->local must exist, since it comes from
 // an explicit definition
 if(jt->assignsym) {
  ASSERT(((I)g&ASGNLOCAL||!probelocal(a)),EVDOMAIN)  //  if global assignment, verify non locally defined
  e = jt->assignsym;   // point to the symbol-table entry being assigned
  CLEARZOMBIE   // clear until next use.
 } else {A jtlocal=jt->local;
  n=AN(a); v=NAV(a); m=v->m;  // n is length of name, v points to string value of name, m is length of non-locale part of name
  if(n==m)ASSERT(!(jtlocal&&g!=jtlocal&&probelocal(a)),EVDOMAIN)  // if non-locative, give error if there is a local
    // symbol table, and we are assigning to the global symbol table, and the name is defined in the local table
  else{C*s=1+m+v->s; RZ(g=NMILOC&v->flag?locindirect(n-m-2,1+s,(UI4)v->bucketx):stfindcre(n-m-2,s,v->bucketx));}
    // locative: s is the length of name_.  Find the symbol table to use, creating one if none found
  // Now g has the symbol table to look in
  RZ(e=g==jtlocal?probeislocal(a) : probeis(a,g));   // set e to symbol-table slot to use
  if(AT(w)&FUNC&&(wv=FAV(w),wv->fgh[0])){if(wv->id==CCOLON)wv->flag|=VNAMED; if(jt->uflags.us.cx.cx_c.glock)wv->flag|=VLOCK;}
   // If the value is a function created by n : m, this becomes a named function; if running a locked function, this is locked too.
   // kludge  these flags are modified in the input area (w), which means they will be improperly set in the result of the
   // assignment (ex: (nm =: 3 : '...') y).  There seems to be no ill effect, because VNAMED isn't used much.
 }
 if(jt->uflags.us.cx.cx_c.db)RZ(redef(w,e));  // if debug, check for changes to stack
 x=e->val;   // if x is 0, this name has not been assigned yet; if nonzero, x points to the value
 I xaf = AFNVRUNFREED;  // If name is not assigned, indicate that it is not read-only or memory-mapped.  Also set 'impossible' code of unfreed+not NVR
 I xt=0;  // If not assigned, use empty type
 if(x){xaf=AFLAG(x); xt=AT(x);} else {xaf = AFNVRUNFREED; xt=0;}  // if assigned, get the actual flags
 if(!((AFRO|AFNJA)&xaf)){
  // If we are assigning the same data block that's already there, don't bother with changing use counts or checking for relative
  // addressing - if there was any, it should have been fixed when the original assignment was made.
  // It is possible that a name in an upper execution refers to the block, but we can't do anything about that.
  if(x!=w){
   // When we assign to, or reassign, a modifier, invalidate all the lookups of modifiers that are extant
   // It's a pity that we have to do this for ALL assignments, even assignments to uv.  If we don't, a reference to a local modifier may get passed in, and
   // it will still be considered valid even though the local names have disappeared.  Maybe we could avoid this if the local namespace has no defined modifiers - but then we'd have to keep up with that...
   if((xt|AT(w))&(VERB|CONJ|ADV))++jt->modifiercounter;
   // Increment the use count of the value being assigned, to reflect the fact that the assigned name will refer to it.
   // This realizes any virtual value, and makes the usecount recursive if the type is recursible
   realizeifvirtual(w); ra(w);
   // If this is a reassignment, we need to decrement the use count in the old name, since that value is no longer used.
   // But if the value of the name is 'out there' in the sentence (coming from an earlier reference), we'd better not delete
   // that value until its last use.
   if(!(xaf&AFNVRUNFREED)){fa(x);} else if(xaf&AFNVR) {AFLAG(x)=(xaf&=~AFNVRUNFREED);}  // x may be 0.  Free if not protected; then unprotect
   e->val=w;   // install the new value
  } else {ACIPNO(w);}  // Set that this value cannot be in-place assigned - needed if the usecount was not incremented above
    // kludge this should not be required, since the incumbent value should never be inplaceable
   // ra() also removes inplaceability
 } else {  // x exists, and is either read-only or memory-mapped
  ASSERT(!(AFRO&xaf),EVRO);   // error if read-only value
  if(x!=w){  // replacing name with different mapped data.  If data is the same, just leave it alone
   realizeifvirtual(w);  // realize if virtual.  The copy stored in the mapped array must be real
   wt=AT(w); wn=AN(w); wr=AR(w); m=wn*bp(wt);
   ASSERT(wt&DIRECT,EVDOMAIN);
   ASSERT(allosize(x)>=m,EVALLOC);
   AT(x)=wt; AN(x)=wn; AR(x)=(RANKT)wr; MCIS(AS(x),AS(w),wr); MC(AV(x),AV(w),m);
  }
 }
 e->sn=jt->slisti;  // Save the script in which this name was defined
 if(jt->stch&&(m<n||jt->local!=g&&jt->stloc!=g))e->flag|=LCH;  // update 'changed' flag if enabled
 R w;   // Return used during testing
}    /* a: name; w: value; g: symbol table */
