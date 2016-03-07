Sporth s => dac.left;
Impulse imp[4] => LPF filt[4];

filt[0] => dac;
filt[0].freq(1000);
filt[0].gain(0.4);
filt[0].Q(10);

filt[1] => dac;
filt[1].freq(4000);
filt[1].gain(0.4);
filt[1].Q(10);

filt[2] => dac;
filt[2].freq(800);
filt[3].gain(0.8);
filt[2].Q(20);

filt[3] => dac;
filt[3].freq(600);
filt[3].gain(0.9);
filt[3].Q(40);

s.parse(
"'orch' 'orch.wav' loadfile " + 
"'scl' '12 7 4 11 7 4 2' gen_vals " + 
"55 '+2(++)+++----' prop 0.8 maytrig tick + dup 0.02 0.1 1.3 tenv " +
"swap 0 'scl' tseq 45 + mtof 0.001 port 0.3 1 1.002 1.3 fm * " +
"0 'orch' tbldur 7 randi 0.3 1 sine 0 'orch' tbldur biscale 0 1 0.05 randi cf 0 0.05 0.2 randi 4 'orch' mincer 1000 butlp + " +
" 1 p 0.5 2 thresh 0.9 0.3 0.9 tenv 0 0.2 3000 randh * 500 butlp + " +
"0 'orch' tbldur 0.1 randi 0.5 0.25 'orch' mincer + " +
"dup 0.9 2.1 0.04 0.001 0.5 jitter + 4 vdelay 0.4 * 800 500 butbp + " +
"dup dup 0.93 8000 revsc drop 0.2 * +"
);


[0, 2, 4, 7, 11, 12, 14] @=> int scale[];

fun int tick(int val)
{
    if(val == 0) {
        1 => val;
    } else {
        0 => val;
    }

    return val;
}

0 => int val;

fun void run() 
{
    while(1) {
        tick(val) => val;
        s.pset(1, val);
        Tiziku.set(0, val);
        s.pset(0, scale[Std.rand2(0, scale.cap() - 1)] + 45);
        8 * Math.random2(1, 4)::second => now;
    }
}

fun void glitch(int num) 
{
    0 => int blink;

    while(1) {
        Std.rand2f(5, 30)::second => now;
        Std.rand2(20, 60) => int rate;
        repeat(2 * Std.rand2(2, 8)) {
            tick(blink) => blink;
            Tiziku.set(num + 1, blink);
            1 => imp[num].next;
            rate::ms => now;
        }
    }
}


spork ~ run();

spork ~ glitch(0);
spork ~ glitch(1);
spork ~ glitch(2);
spork ~ glitch(3);

while(1) second => now;
