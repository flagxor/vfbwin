#! /usr/bin/env gforth

s" dev/fb0" r/w open-file throw constant fb

1024 constant width
768 constant height
width height * 4 * constant display-size
display-size allocate throw constant display
hex ff000000 decimal constant alpha

variable pos

: plot ( c x y )
  width * + 4 * display + swap alpha or swap l!
;

: flip   0 0 fb reposition-file throw
         display display-size fb write-file throw ;

: pattern-plot ( x y )
  2dup pos @ + * -rot plot
;

: pattern
  height 0 do width 0 do
    i j pattern-plot
  loop loop
;

: animate
  pattern display flip
  1 pos +!
;

: main
  begin animate again
;
main

bye
