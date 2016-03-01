//class dummy {
//    fun void set(int x, float y) {
//
//    }
//}
//
//dummy Tiziku;

Sporth s => dac;

s.parse(
"1 p 0.5 2 thresh 0.01 0.1 0.4 tenv " +
"0 p mtof 0.5 1 1 1 fm * " +
"dup 0.7 1.5 delay 0.6 * 1000 butlp + " +
"dup dup 0.97 10000 revsc drop 0.4 * +"
);

[0, 2, 3, 5, 7, 10] @=> int scale[];

fun int tick(int val)
{
    if(val == 0) {
        1 => val;
    } else {
        0 => val;
    }

    s.pset(1, val);
    return val;
}

0 => int val;

while(1) {
    tick(val) => val;
    Tiziku.set(0, Std.rand2f(0.1, 1));
    s.pset(0, scale[Std.rand2(0, scale.cap() - 1)] + 60);
    4 * Std.rand2(1, 3)::second => now;
}
