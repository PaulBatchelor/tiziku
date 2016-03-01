//Step s => dac;
//s.next(0.3);

//SinOsc s => dac;
//1::second => now;

Sporth s => dac;

s.parse("200 880 20 randh 0.5 sine");
10::second => now;
