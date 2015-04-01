
; begin code at beginning of program memory
org 00h

; switch off other ports
; possibly necessary to compensate
; for the power loss on the other activated LEDs
clr P1.0
mov P0, #0

end
