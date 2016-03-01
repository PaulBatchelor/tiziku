Sporth s => dac;

s.parse(
"1 p 0.5 2 thresh 0.01 0.1 0.3 tenv " +
"0 p mtof 0.5 tri * " +
"dup dup 0.97 10000 revsc drop 0.1 * +"
);

[0, 2, 4, 5, 9] @=> int scale[];

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
    0.5 * Std.rand2(1, 3)::second => now;
}
