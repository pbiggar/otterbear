define i32 @main() {
  %ptr = alloca i32, i32 12 ; allocate memory for the virtual registers

;; j = 5
;assign _1 5
  store i32 5, i32* %ptr[1]

;write j _1



;; i = j + 1
;read _2 j
;assign _3 1
;add _4 _2 _3
;write i _4
;
;; i = add(i, j)
;read _5 i
;read _6 j
;call _7 add _5 _6
;write i _7
;
;; print (i)
;read _8 i
;call _9 print _8
;
;; return 0
;assign _10 0
;return _10



  ret i32 0
}
