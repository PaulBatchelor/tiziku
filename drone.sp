# main sporth patch code, extracted from run.ck
# has most of the music, minus the glitches

_tk var tick _tk set
0 4 pset
'orch' 'orch.wav' loadfile
'scl' '12 7 4 11 7 4 2' gen_vals
50 '+2(++)+++----' prop 0.8 maytrig _tk get + dup 0.02 0.1 1.3 tenv
swap 0 'scl' tseq 45 + mtof 0.001 port 0.3 1 1.002 1.1 fm * 100 buthp

# key fades

_tk get 0.001 30 1 expon *

dup 0.5 * 4 p + 4 pset

0 'orch' tbldur 7 randi 0.3 1 sine 0 'orch' tbldur biscale 0 1 0.05 randi cf 0 0.05 0.2 randi 4 2048 'orch' mincer 1000 butlp
dup 0.5 * 4 p + 4 pset +
1 p 0.5 2 thresh 1.1 0.3 0.9 tenv 0 0.2 3000 randh * 500 butlp 
dup 0.6 * 4 p + 4 pset +
0 'orch' tbldur 0.1 randi 0.5 0.25 2048 'orch' mincer +

dup 0.9 2.1 0.04 0.001 0.5 jitter + 4 vdelay 0.8 * 800 500 butbp +
4 p dup 0.95 8000 revsc drop 0.2 * +

3 p 'val' print 0.5 2 thresh dup 1 10 0 line 1 swap switch *