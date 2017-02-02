Sporth s => dac;
Impulse imp[4] => LPF filt[4];

true => int is_running;

filt[0] => dac.left;
filt[0].freq(1000);
filt[0].gain(0.4);
filt[0].Q(10);

filt[1] => dac.left;
filt[1].freq(4000);
filt[1].gain(0.4);
filt[1].Q(10);

filt[2] => dac.right;
filt[2].freq(800);
filt[3].gain(0.8);
filt[2].Q(20);

filt[3] => dac.right;
filt[3].freq(600);
filt[3].gain(0.9);
filt[3].Q(40);

s.parse(
"_tk var tick _tk set " + 
"0 4 pset " + 
"'orch' 'orch.wav' loadfile " + 
"'scl' '12 7 4 11 7 4 2' gen_vals " + 
"50 '+2(++)+++----' prop 0.8 maytrig _tk get + dup 0.02 0.1 1.3 tenv " +
"swap 0 'scl' tseq 45 + mtof 0.001 port 0.3 1 1.002 1.1 fm * 100 buthp " +
// key Fades
"_tk get 0.001 30 1 expon * " + 
"dup 0.5 * 4 p + 4 pset " +
"0 'orch' tbldur 7 randi 0.3 1 sine 0 'orch' tbldur biscale 0 1 0.05 randi cf 0 0.05 0.2 randi 4 2048 'orch' mincer 1000 butlp " +
"dup 0.5 * 4 p + 4 pset + " +
" 1 p 0.5 2 thresh 1.1 0.3 0.9 tenv 0 0.2 3000 randh * 500 butlp " +
"dup 0.6 * 4 p + 4 pset + " +
"0 'orch' tbldur 0.1 randi 0.5 0.25 2048 'orch' mincer + " +
"dup 0.9 2.1 0.04 0.001 0.5 jitter + 4 vdelay 0.8 * 800 500 butbp + " +
"4 p dup 0.95 8000 revsc drop 0.2 * + " +
"3 p 'val' print 0.5 2 thresh dup 1 10 0 line 1 swap switch * "
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
0 => int fade_out;
0 => int have_color;


fun void glitch(int num, int min) 
{
    0 => int blink;

    Std.rand2(20, 60) => int rate;
    repeat(2 * Std.rand2(2, 8) + min) {
        tick(blink) => blink;
        Tiziku.set(num + 1, blink);
        1 => imp[num].next;
        rate::ms => now;
    }
}

fun void glitchoff(int num) 
{
    0 => int blink;

    0 => int prev;
    Std.rand2(20, 60) => int rate;
    repeat(2 * Std.rand2(2, 8) + 1) {
        tick(blink) => blink;
        Tiziku.set(num + 1, blink);
        if(have_color != 0) { 
            1 => imp[num].next;
        }
        prev + Std.rand2(10, 60) => prev;
        prev::ms => now;
    }
}

fun void autoglitch(int num) 
{
    while(is_running) {
        Std.rand2f(5, 30)::second => now;
        glitch(num, 0);
    }

}
fun void autorun() 
{
    <<< "FADE IN", fade_out>>>;
    spork ~ autoglitch(0);
    spork ~ autoglitch(1);
    spork ~ autoglitch(2);
    spork ~ autoglitch(3);

    2 => int is_first;


    tick(val) => val;
    Tiziku.set(0, val);
    1::second => now;
    tick(val) => val;
    Tiziku.set(0, val);
    1::second => now;

    <<< "FADE" >>>;
    while(is_running) {
        tick(val) => val;
        s.pset(1, val);
        Tiziku.set(0, val);
        s.pset(0, scale[Std.rand2(0, scale.cap() - 1)] + 45);
        8 * Math.random2(1, 4)::second => now;
    }
}

/* UNCOMMENT FOR MIDI CONTROL
MidiIn min;
MidiMsg msg;
1 => int device;
if( !min.open(device)) me.exit();

0 => int chan;

fun void change_scale(int scale[]) {
    0 => float val;
    for(0 => int i; i < 7; i++) {
        scale[i] => val;
        s.tset(i, scale[i], "scl");
    }
}

fun void run() {
    while(is_running) {
        min => now;
        while(min.recv(msg)) {
            if(msg.data1 >> 0x4 == 0x9)  {
                msg.data1 & 0x0f => chan;
                if(chan < 4) {
                    spork ~ glitch(chan, 0);
                } else if (chan < 8) {
                    spork ~ glitchoff(chan % 4);
                } else if(chan == 12) {
                    <<< "FADE IN">>>;
                    tick(val) => val;
                    s.pset(1, val);
                    Tiziku.set(0, val);
                    s.pset(0, scale[Std.rand2(0, scale.cap() - 1)] + 45);
                    1 => have_color;
                } else if(chan == 15) {
                    tick(fade_out) => fade_out;
                    Tiziku.set(6, fade_out);
                    s.pset(3, fade_out);
                    <<< "FADE", fade_out>>>;
                } else if (chan == 8) {
                    <<< "CHANGING SCALE...">>>;
                    change_scale([12, 7, 4, 11, 7, 4, 2]);
                } else if(chan == 9) {
                    <<< "CHANGING SCALE...">>>;
                    change_scale([16, 14, 7, 12, 11, 7, 9]);
                } else if(chan == 10) {
                    <<< "CHANGING SCALE...">>>;
                    change_scale([19, 14, 12, 17, 16, 11, 12]);
                }

            }
            
            me.yield();
        }
    }
}
*/

// To run automatically
autorun();
// To run withi MIDI (quneo) control
//run();
