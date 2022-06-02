prolog './g3x12.ijs'
NB. 3!:12 ----------------------------------------------------------------

ToLower=: 3 : 0`((((97+i.26){a.)(65+i.26)}a.) {~ a. i. ])@.(2 = 3!:0)
x=. I. 26 > n=. ((65+i.26){a.) i. t=. ,y
($y) $ ((x{n) { (97+i.26){a.) x}t
)

ToUpper=: 3 : 0`((((65+i.26){a.)(97+i.26)}a.) {~ a. i. ])@.(2 = 3!:0)
x=. I. 26 > n=. ((97+i.26){a.) i. t=. ,y
($y) $ ((x{n) { (65+i.26){a.) x}t
)

(ToLower -: 0&(3!:12)) a.
(ToLower -: 0&(3!:12)) 123 456$a.
(ToLower -: 0&(3!:12)) u: a.
(ToLower -: 0&(3!:12)) u: 123 456$a.
(ToLower -: 0&(3!:12)) 10&u: a.
(ToLower -: 0&(3!:12)) 10&u: 123 456$a.
(ToLower -: 0&(3!:12)) i. 3 4
(ToLower -: 0&(3!:12)) 0.5 + i. 3 4
(ToLower -: 0&(3!:12)) 1j1 + i. 3 4
(ToLower -: 0&(3!:12)) x: i. 3 4
(ToLower -: 0&(3!:12)) <a.
(ToLower -: 0&(3!:12)) <123 456$a.
(ToLower -: 0&(3!:12)) 3 4$<a.
(ToLower -: 0&(3!:12)) 3 4$<123 456$a.
(ToLower -: 0&(3!:12))&> 3 4$<a.
(ToLower -: 0&(3!:12))&> 3 4$<123 456$a.

(ToUpper -: 1&(3!:12)) a.
(ToUpper -: 1&(3!:12)) 123 456$a.
(ToUpper -: 1&(3!:12)) u: a.
(ToUpper -: 1&(3!:12)) u: 123 456$a.
(ToUpper -: 1&(3!:12)) 10&u: a.
(ToUpper -: 1&(3!:12)) 10&u: 123 456$a.
(ToUpper -: 1&(3!:12)) i. 3 4
(ToUpper -: 1&(3!:12)) 0.5 + i. 3 4
(ToUpper -: 1&(3!:12)) 1j1 + i. 3 4
(ToUpper -: 1&(3!:12)) x: i. 3 4
(ToUpper -: 1&(3!:12)) <a.
(ToUpper -: 1&(3!:12)) <123 456$a.
(ToUpper -: 1&(3!:12)) 3 4$<a.
(ToUpper -: 1&(3!:12)) 3 4$<123 456$a.
(ToUpper -: 1&(3!:12))&> 3 4$<a.
(ToUpper -: 1&(3!:12))&> 3 4$<123 456$a.

'domain error' -: _1&(3!:12) etx ''
'domain error' -: 2&(3!:12) etx ''
'nonce error' -: 0&(3!:12) etx $. i. 3 4
'nonce error' -: 1&(3!:12) etx $. i. 3 4

4!:55 ;:'ToLower ToUpper'

epilog''
