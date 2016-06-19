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

fvariable fx fvariable fy

: f>col ( f -- n ) 255e f* 0e fmax 255e fmin f>s ;
: f>rgb ( f f f --- rbb ) f>col f>col 256 * + f>col 256 * 256 * + ;

: 4spire-color
  fx f@ fx f@ 23e f* fsin 2e f/ fy f@ fmax f/ fsin
  fy f@ fx f@ 23e f* fsin 2e f/ fy f@ fmax f/ fsin
  fover fover f/ fsin f>rgb
;

: 4spire-plot ( x y )
  2dup s>f height s>f f/ fy 1e fswap f- f!
       pos @ + s>f width s>f f/ fx f!
  4spire-color -rot plot
;

: 4spire
  height 0 do width 0 do
    i j 4spire-plot
  loop loop
;

: animate
  4spire display flip
  100 pos +!
;

: main
  begin animate again
;
main

bye
