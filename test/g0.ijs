1:@:(dbr bind Debug)@:(9!:19)2^_44[(echo^:ECHOFILENAME) './g0.ijs'
NB. handling -0 (-0 on some systems) ------------------------------------

test=: 3 : 0
 assert. 1 1 1 -: 0=y
 assert. 1 1 1 -: y=0
 assert. (3 3$1) -: =/~ y
 assert. (3 3$1) -: =!.0/~ y
 assert. 0 0 0 -:    y
 assert. 0 0 0 -:!.0 y
 assert. 0 0 0 -: i.    ~ y
 assert. 0 0 0 -: i.!.0 ~ y
 assert. 0 0 0 -: y i.    ,y
 assert. 0 0 0 -: y i.!.0 ,y
 assert. 1 1 1 -: y e.    [0
 assert. 1 1 1 -: y e.!.0 [0
 assert. '' -: y -.   [ 0
 assert. '' -: y -.!.0[ 0
 assert. '0 0 0' -: ":y
 1
)

mac =: 3 = 9!:12 ''

test    0 % 3 _4 _7
test }: 0 % 3 _4 _7 3j4
test ". mac{'_1e_307 0 1e_307 % 1e307',:'0 0 0'
test ". mac{'}:_1e_307 0 1e_307 123j456 % 1e307',:'0 0 0'
test ". mac{'_1e_307 0 1e_307 * 1e_307',:'0 0 0'

NB. -0 now allowed '0' = , 6}.2 (3!:3) 0.07 _0
NB. -0 now allowed '0' = , 7}.2 (3!:3) 3j47 _0
NB. -0 now allowed '0' = , 6}.2 (3!:3) _1 ". '0.07 _0'
NB. -0 now allowed '0' = , 7}.2 (3!:3) _1 ". '3j47 _0'

f=: 3 : 0
 t=. ($y)$-~0.5
 assert. -. (3!:3 t) -: 3!:3 y
 assert. 0=y
 assert. 0=!.0 y
 assert. t -:!.0 y
 NB. not changed any more assert. (3!:3 t) -: 3!:3 y  NB. y changed to true 0 in place
 1
)

g=: 3 : 0
 t=. ($y)$-~0.5
 assert. -. (3!:3 t) -: 3!:3 y
 assert. 0=y
 assert. 0=!.0 y
 assert. (0,#y) -: y i.!.0 o. 0 2
NB. not changed nay more  assert. (3!:3 t) -: 3!:3 y  NB. y changed to true 0 in place by i.
 1
)

f x=: 0 %  __ _5 0 5 _
f x=: 5 5 _5 _5 % _ __ _ __
g x=: 0 %  __ _5 0 5 _
g x=: 5 5 _5 _5 % _ __ _ __

__ __ 0 _ _ (-:!.0) 0 %~ __ _5 0 5 _


NB. 0: to 9: ------------------------------------------------------------

0 -: 0: 1=?2 3 4$2
0 -: 0: a.{~? 10##a.
0 -: 0: _20+?3 2$40
0 -: 0: o. _20+?13$40
0 -: 0: r.?13$40

0 -: (1=?2 3 4$2   ) 0: 1=?2 3 4$2
0 -: (1=?2 3 4$2   ) 0: a.{~? 10##a.
0 -: (1=?2 3 4$2   ) 0: _20+?3 2$40
0 -: (1=?2 3 4$2   ) 0: o. _20+?13$40
0 -: (1=?2 3 4$2   ) 0: r.?13$40

0 -: (a.{~? 10##a. ) 0: 1=?2 3 4$2
0 -: (a.{~? 10##a. ) 0: a.{~? 10##a.
0 -: (a.{~? 10##a. ) 0: _20+?3 2$40
0 -: (a.{~? 10##a. ) 0: o. _20+?13$40
0 -: (a.{~? 10##a. ) 0: r.?13$40

0 -: (_20+?3 2$40  ) 0: 1=?2 3 4$2
0 -: (_20+?3 2$40  ) 0: a.{~? 10##a.
0 -: (_20+?3 2$40  ) 0: _20+?3 2$40
0 -: (_20+?3 2$40  ) 0: o. _20+?13$40
0 -: (_20+?3 2$40  ) 0: r.?13$40

0 -: (o. _20+?13$40) 0: 1=?2 3 4$2
0 -: (o. _20+?13$40) 0: a.{~? 10##a.
0 -: (o. _20+?13$40) 0: _20+?3 2$40
0 -: (o. _20+?13$40) 0: o. _20+?13$40
0 -: (o. _20+?13$40) 0: r.?13$40

0 -: (r.?13$40     ) 0: 1=?2 3 4$2
0 -: (r.?13$40     ) 0: a.{~? 10##a.
0 -: (r.?13$40     ) 0: _20+?3 2$40
0 -: (r.?13$40     ) 0: o. _20+?13$40
0 -: (r.?13$40     ) 0: r.?13$40

1 -: 1: 1=?2 3 4$2
1 -: 1: a.{~? 10##a.
1 -: 1: _20+?3 2$40
1 -: 1: o. _20+?13$40
1 -: 1: r.?13$40

1 -: (1=?2 3 4$2   ) 1: 1=?2 3 4$2
1 -: (1=?2 3 4$2   ) 1: a.{~? 10##a.
1 -: (1=?2 3 4$2   ) 1: _20+?3 2$40
1 -: (1=?2 3 4$2   ) 1: o. _20+?13$40
1 -: (1=?2 3 4$2   ) 1: r.?13$40

1 -: (a.{~? 10##a. ) 1: 1=?2 3 4$2
1 -: (a.{~? 10##a. ) 1: a.{~? 10##a.
1 -: (a.{~? 10##a. ) 1: _20+?3 2$40
1 -: (a.{~? 10##a. ) 1: o. _20+?13$40
1 -: (a.{~? 10##a. ) 1: r.?13$40

1 -: (_20+?3 2$40  ) 1: 1=?2 3 4$2
1 -: (_20+?3 2$40  ) 1: a.{~? 10##a.
1 -: (_20+?3 2$40  ) 1: _20+?3 2$40
1 -: (_20+?3 2$40  ) 1: o. _20+?13$40
1 -: (_20+?3 2$40  ) 1: r.?13$40

1 -: (o. _20+?13$40) 1: 1=?2 3 4$2
1 -: (o. _20+?13$40) 1: a.{~? 10##a.
1 -: (o. _20+?13$40) 1: _20+?3 2$40
1 -: (o. _20+?13$40) 1: o. _20+?13$40
1 -: (o. _20+?13$40) 1: r.?13$40

1 -: (r.?13$40     ) 1: 1=?2 3 4$2
1 -: (r.?13$40     ) 1: a.{~? 10##a.
1 -: (r.?13$40     ) 1: _20+?3 2$40
1 -: (r.?13$40     ) 1: o. _20+?13$40
1 -: (r.?13$40     ) 1: r.?13$40

2 -: 2: 1=?2 3 4$2
2 -: 2: a.{~? 10##a.
2 -: 2: _20+?3 2$40
2 -: 2: o. _20+?13$40
2 -: 2: r.?13$40

2 -: (1=?2 3 4$2   ) 2: 1=?2 3 4$2
2 -: (1=?2 3 4$2   ) 2: a.{~? 10##a.
2 -: (1=?2 3 4$2   ) 2: _20+?3 2$40
2 -: (1=?2 3 4$2   ) 2: o. _20+?13$40
2 -: (1=?2 3 4$2   ) 2: r.?13$40

2 -: (a.{~? 10##a. ) 2: 1=?2 3 4$2
2 -: (a.{~? 10##a. ) 2: a.{~? 10##a.
2 -: (a.{~? 10##a. ) 2: _20+?3 2$40
2 -: (a.{~? 10##a. ) 2: o. _20+?13$40
2 -: (a.{~? 10##a. ) 2: r.?13$40

2 -: (_20+?3 2$40  ) 2: 1=?2 3 4$2
2 -: (_20+?3 2$40  ) 2: a.{~? 10##a.
2 -: (_20+?3 2$40  ) 2: _20+?3 2$40
2 -: (_20+?3 2$40  ) 2: o. _20+?13$40
2 -: (_20+?3 2$40  ) 2: r.?13$40

2 -: (o. _20+?13$40) 2: 1=?2 3 4$2
2 -: (o. _20+?13$40) 2: a.{~? 10##a.
2 -: (o. _20+?13$40) 2: _20+?3 2$40
2 -: (o. _20+?13$40) 2: o. _20+?13$40
2 -: (o. _20+?13$40) 2: r.?13$40

2 -: (r.?13$40     ) 2: 1=?2 3 4$2
2 -: (r.?13$40     ) 2: a.{~? 10##a.
2 -: (r.?13$40     ) 2: _20+?3 2$40
2 -: (r.?13$40     ) 2: o. _20+?13$40
2 -: (r.?13$40     ) 2: r.?13$40

9 -: 9: 1=?2 3 4$2
9 -: 9: a.{~? 10##a.
9 -: 9: _20+?3 2$40
9 -: 9: o. _20+?13$40
9 -: 9: r.?13$40

9 -: (1=?2 3 4$2   ) 9: 1=?2 3 4$2
9 -: (1=?2 3 4$2   ) 9: a.{~? 10##a.
9 -: (1=?2 3 4$2   ) 9: _20+?3 2$40
9 -: (1=?2 3 4$2   ) 9: o. _20+?13$40
9 -: (1=?2 3 4$2   ) 9: r.?13$40

9 -: (a.{~? 10##a. ) 9: 1=?2 3 4$2
9 -: (a.{~? 10##a. ) 9: a.{~? 10##a.
9 -: (a.{~? 10##a. ) 9: _20+?3 2$40
9 -: (a.{~? 10##a. ) 9: o. _20+?13$40
9 -: (a.{~? 10##a. ) 9: r.?13$40

9 -: (_20+?3 2$40  ) 9: 1=?2 3 4$2
9 -: (_20+?3 2$40  ) 9: a.{~? 10##a.
9 -: (_20+?3 2$40  ) 9: _20+?3 2$40
9 -: (_20+?3 2$40  ) 9: o. _20+?13$40
9 -: (_20+?3 2$40  ) 9: r.?13$40

9 -: (o. _20+?13$40) 9: 1=?2 3 4$2
9 -: (o. _20+?13$40) 9: a.{~? 10##a.
9 -: (o. _20+?13$40) 9: _20+?3 2$40
9 -: (o. _20+?13$40) 9: o. _20+?13$40
9 -: (o. _20+?13$40) 9: r.?13$40

9 -: (r.?13$40     ) 9: 1=?2 3 4$2
9 -: (r.?13$40     ) 9: a.{~? 10##a.
9 -: (r.?13$40     ) 9: _20+?3 2$40
9 -: (r.?13$40     ) 9: o. _20+?13$40
9 -: (r.?13$40     ) 9: r.?13$40

f=: 3: 4: 5:
4 -: 'abc' f ?3 4$1000


NB. 0: to 9: encore -----------------------------------------------------

dr  =: 5!:2
rk  =: 1 : ('+"u b. 0')

_ _ _ -: 0: rk
_ _ _ -: 1: rk
_ _ _ -: 2: rk
_ _ _ -: 3: rk
_ _ _ -: 4: rk
_ _ _ -: 5: rk
_ _ _ -: 6: rk
_ _ _ -: 7: rk
_ _ _ -: 8: rk
_ _ _ -: 9: rk

fx =: 5!:0
ar =: 5!:1
dr =: 5!:2
tr =: 5!:4
lr =: 5!:5

eq =: 2 : '''u'' -:&ar&< ''v'''

(,&':'@":&.>i.10) -: 0:`1:`2:`3:`4:`5:`6:`7:`8:`9:

f =: 1 : 'dr <''u'''

(,<'_9:') -: _9: f
(,<'_1:') -: _1: f
(,<'0:' ) -: 0:  f
(,<'1:' ) -: 1:  f
(,<'2:' ) -: 2:  f
(,<'3:' ) -: 3:  f
(,<'4:' ) -: 4:  f
(,<'5:' ) -: 5:  f
(,<'6:' ) -: 6:  f
(,<'7:' ) -: 7:  f
(,<'8:' ) -: 8:  f
(,<'9:' ) -: 9:  f

f =: 1 : '5!:5 <''u'''

'_9:' -: _9: f
'_1:' -: _1: f
'0:'  -: 0: f
'1:'  -: 1: f
'9:'  -: 9: f

(ar <'zero' ) fx eq zero  =: 0:
(ar <'one'  ) fx eq one   =: 1:
(ar <'two'  ) fx eq two   =: 2:
(ar <'three') fx eq three =: 3:
(ar <'four' ) fx eq four  =: 4:
(ar <'five' ) fx eq five  =: 5:
(ar <'six'  ) fx eq six   =: 6:
(ar <'seven') fx eq seven =: 7:
(ar <'eight') fx eq eight =: 8:
(ar <'nine' ) fx eq nine  =: 9:

f =: 1 : 'lr <''u'''

'0:' -: 0: f
'1:' -: 1: f
'2:' -: 2: f
'3:' -: 3: f
'4:' -: 4: f
'5:' -: 5: f
'6:' -: 6: f
'7:' -: 7: f
'8:' -: 8: f
'9:' -: 9: f

g =: 3: 4: 5:
('3:';'4:';'5:') -: dr <'g'
'3: 4: 5:' -: lr <'g'

1 = 3!:0 0: 'a'
1 = 3!:0 1: 'a'
4 = 3!:0 2: 'a'

f =: ++&
(;:'++&') -: 5!:2 <'f'
f =: ///
(;:'///') -: 5!:2 <'f'
f =: &&&
(;:'&&&') -: 5!:2 <'f'

4!:55 ;:'ar dr eight eq f five four fx g lr '
4!:55 ;:'mac nine one rk seven six test three tr two x z zero '


