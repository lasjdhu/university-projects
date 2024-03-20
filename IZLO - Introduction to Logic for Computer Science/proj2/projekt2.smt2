(set-logic NIA)

(set-option :produce-models true)
(set-option :incremental true)

; Deklarace promennych pro vstupy
; ===============================

; Parametry
(declare-fun A () Int)
(declare-fun B () Int)
(declare-fun C () Int)
(declare-fun D () Int)
(declare-fun E () Int)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;; START OF SOLUTION ;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;; if (D <= 0 || E <= 0) {
;;     return false;
;; }
(assert
    (and
        (> D 0)
        (> E 0)
    )
)

;; int x = A*B*2;
(define-fun x ((A Int) (B Int)) Int
    (* (* A B) 2)
)

;; int y;
;; if (x < E) {
;;     y = x + 5*B;
;; } else {
;;     y = x - C;
;; }
(define-fun y ((A Int) (B Int) (C Int) (E Int)) Int
    (ite
        (< (x A B) E)
        (+ (x A B) (* 5 B))
        (- (x A B) C)
    )
)

;; int z;
;; if (y + 2 < D) {
;;     z = x*A - y*B;
;; } else {
;;     z = x*B + y*A;
;; }
(define-fun z ((A Int) (B Int) (C Int) (D Int) (E Int)) Int
    (ite
        (< (+ (y A B C E) 2) D)
        (- (* (x A B) A) (* (y A B C E) B))
        (+ (* (x A B) B) (* (y A B C E) A))
    )
)

;; if (z < E+D) {
;;     return true;
;; } else {
;;     return false;
;; }
(define-fun f ((A Int) (B Int) (C Int) (D Int) (E Int)) Bool
    (< (z A B C D E) (+ D E))
)

;; bool f(int A, int B, int C, int D, int E)
(assert
    (f A B C D E)
)

;; součet D + E je nejmenší možný
(assert
    (forall ((D1 Int) (E1 Int))
        (=>
            (f A B C D1 E1)
            (>= (+ D1 E1) (+ D E))
        )
    )
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;; END OF SOLUTION ;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; Testovaci vstupy
; ================

(echo "Test 1 - vstup A=1, B=1, C=3")
(echo "a) Ocekavany vystup je sat a D+E se rovna 2")
(check-sat-assuming (
  (= A 1) (= B 1) (= C 3)
))
(get-value (D E (+ D E)))
(echo "b) Neexistuje jine reseni nez 2, ocekavany vystup je unsat")
(check-sat-assuming (
  (= A 1) (= B 1) (= C 3) (distinct (+ D E) 2)
))


(echo "Test 2 - vstup A=5, B=2, C=5")
(echo "a) Ocekavany vystup je sat a D+E se rovna 54")
(check-sat-assuming (
  (= A 5) (= B 2) (= C 5)
))
(get-value (D E (+ D E)))
(echo "b) Neexistuje jine reseni nez 54, ocekavany vystup je unsat")
(check-sat-assuming (
  (= A 5) (= B 2) (= C 5) (distinct (+ D E) 54)
))

(echo "Test 3 - vstup A=100, B=15, C=1")
(echo "a) Ocekavany vystup je sat a D+E se rovna 253876")
(check-sat-assuming (
  (= A 100) (= B 15) (= C 1)
))
(get-value (D E (+ D E)))
(echo "b) Neexistuje jine reseni nez 253876, ocekavany vystup je unsat")
(check-sat-assuming (
  (= A 100) (= B 15) (= C 1) (distinct (+ D E) 253876)
))

(echo "Test 4 - vstup A=5, B=5, C=3")
(echo "a) Ocekavany vystup je sat a D+E se rovna 51")
(check-sat-assuming (
  (= A 5) (= B 5) (= C 3)
))
(get-value (D E (+ D E)))
(echo "b) Neexistuje jine reseni nez 51, ocekavany vystup je unsat")
(check-sat-assuming (
  (= A 5) (= B 5) (= C 3) (distinct (+ D E) 51)
))

(echo "Test 5 - vstup A=2, B=1, C=2")
(echo "a) Ocekavany vystup je sat a D+E se rovna 7")
(check-sat-assuming (
  (= A 2) (= B 1) (= C 2)
))
(get-value (D E (+ D E)))
(echo "b) Neexistuje jine reseni nez 7, ocekavany vystup je unsat")
(check-sat-assuming (
  (= A 2) (= B 1) (= C 2) (distinct (+ D E) 7)
))
