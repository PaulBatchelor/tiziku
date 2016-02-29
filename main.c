#include <stdio.h>
//#include "chuckwrap.hpp"
//extern "C" {
#include "chuckwrap.h"
//}

int main()
{
    the_chuckwrap cw;
    
    //TheChucK *chuck = TheChucK::instance();
    //chuck->initialize( MY_SRATE, MY_BUFFERSIZE, MY_CHANNELS_IN, MY_CHANNELS_OUT, 0, NULL);
    
    chuckwrap_init(&cw, MY_SRATE, MY_BUFFERSIZE, MY_CHANNELS_IN, MY_CHANNELS_OUT);

    //int r = chuck->compileFile( "test.ck" );
    chuckwrap_compile(&cw, "test.ck");

    int n = 0, i = 0;
    float out[MY_BUFFERSIZE];
    //chuck->onOutput(out, MY_BUFFERSIZE);
    chuckwrap_compute(&cw, out, MY_BUFFERSIZE);

    for(n = 0; n < MY_BUFFERSIZE; n ++) {
        /* print 4096 samples of Chuck */
        printf("%d %g\n", n, out[n]);
    }

    /* do we need to explicitly call destructor? */ 
    //delete chuck;
    return 0;
}
