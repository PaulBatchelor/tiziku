//Step s => dac;
//s.next(0.3);

//SinOsc s => dac;
//1::second => now;

Sporth s => dac;

s.parse("440 0.1 sine");
1::second => now;
