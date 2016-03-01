#include "chuckwrap.hpp"
extern "C" {
#include "chuckwrap.h"
}
#include "chuck_system.h"
#include "chuck_dl.h"
#include "util_thread.h"


TheChucK * TheChucK::ourInstance = NULL;

static float global_stack[10];

//------------------------------------------------------------------------------
// name: initialize()
// desc: initialize chuck system
//------------------------------------------------------------------------------
bool TheChucK::initialize( int srate, int bufferSize, int channelsIn,
                           int channelsOut, int argc, const char ** argv )
{
    // bigger of two
    int biggerChannel = std::max( channelsIn, channelsOut );
    // allocate
    inputBuffer = new float[bufferSize*biggerChannel];
    // remember
    m_channelsIn = channelsIn;
    m_channelsOut = channelsOut;
    
    // initialize!
    t_CKBOOL val = m_system->clientInitialize(
        srate, bufferSize, channelsIn, channelsOut, argc, argv );
    
    // binding
    m_system->bind( tz_query, "Tiziku" );

    for(int i = 0; i < 10; i++) {
        global_stack[i] = 0;
    }

    // done
    return val;
}




//------------------------------------------------------------------------------
// name: compileFile()
// desc: compile a file
//------------------------------------------------------------------------------
bool TheChucK::compileFile( const std::string & path, const std::string & args )
{
    // compile
    //return m_system->compileFile( ofToDataPath(path), args );
    return m_system->compileFile( path, args );
}




//------------------------------------------------------------------------------
// name: compileCode()
// desc: compile code
//------------------------------------------------------------------------------
//bool TheChucK::compileCode( const std::string & path, const std::string & args )
//{
//    // compile
//    return FALSE;
//}




//------------------------------------------------------------------------------
// name: vm()
// desc: get VM
//------------------------------------------------------------------------------
Chuck_VM * TheChucK::vm()
{
    // compile
    return m_system->vm();
}




//------------------------------------------------------------------------------
// name: compiler()
// desc: get compiler
//------------------------------------------------------------------------------
Chuck_Compiler * TheChucK::compiler()
{
    // compile
    return m_system->compiler();
}




//------------------------------------------------------------------------------
// name: db()
// desc: get OFCK database
//------------------------------------------------------------------------------
//OFCKDB * TheChucK::db()
//{
//    // HACK: using singleton
//    return OFCKDB::instance();
//}




//------------------------------------------------------------------------------
// name: onInput()
// desc: audio input
//------------------------------------------------------------------------------
void TheChucK::onInput( float * input, int numFrames )
{
    // copy
    memcpy( inputBuffer, input, numFrames*m_channelsIn*sizeof(float) );
}




//------------------------------------------------------------------------------
// name: onOutput()
// desc: audio output
//------------------------------------------------------------------------------
void TheChucK::onOutput( float * output, int numFrames )
{
    // run with input and output
    m_system->run( inputBuffer, output, numFrames );
}




//------------------------------------------------------------------------------
// name: getInstance()
// desc: this is a singleton class (second instance would break everything)
//------------------------------------------------------------------------------
TheChucK * TheChucK::instance()
{
    // check
    //if( ourInstance == NULL )
    //{
        // instantiate!
        ourInstance = new TheChucK;
    //}
    
    return ourInstance;
}




//------------------------------------------------------------------------------
// name: TheChucK()
// desc: constructor
//------------------------------------------------------------------------------
TheChucK::TheChucK()
{
    // instantiate
    m_system = new Chuck_System;
    inputBuffer = NULL;
    m_channelsIn = 0;
    m_channelsOut = 0;
}




//------------------------------------------------------------------------------
// name: ~TheChucK()
// desc: destructor
//------------------------------------------------------------------------------
TheChucK::~TheChucK()
{
    // done
    m_system->clientShutdown();
    delete m_system;
    // zero out
    printf("destroying...\n");
    m_system = NULL;
}



//------------------------------------------------------------------------------
// name: displaySync()
// desc: trigger a display sync from oF; triggering ChucK event
//------------------------------------------------------------------------------
//void TheChucK::displaySync()
//{
//    // get the DB
//    OFCKDB * db = OFCKDB::instance();
//    // broadcast on the event
//    db->displaySync.queue_broadcast( db->m_eventBuffer );
//    // flush eval queue (defered eval commands here to minimize locking)
//    db->flushEval();
//}
//


float* TheChucK::getStack()
{
    return global_stack;
}

extern "C" {
void chuckwrap_init(the_chuckwrap *cw, int sr, int bufsize, int in, int out)
{
    cw->chuck = (TheChucK *) TheChucK::instance();
    TheChucK *chuck = (TheChucK *)cw->chuck;
    chuck->initialize(sr, bufsize, in, out, 0, NULL);
    cw->stack = chuck->getStack();
}

void chuckwrap_compile(the_chuckwrap *cw, const char *filename)
{
    TheChucK *chuck = (TheChucK *)cw->chuck;
    chuck->compileFile(filename);
}

void chuckwrap_compute(the_chuckwrap *cw, float *out, int bufsize)
{
    TheChucK *chuck = (TheChucK *)cw->chuck;
    chuck->onOutput(out, bufsize);
}

void chuckwrap_destroy(the_chuckwrap *cw)
{
    TheChucK *chuck = (TheChucK *)cw->chuck;
    delete chuck;
}

}

CK_DLL_SFUN( cktz_foo )
{
    RETURN->v_int = 0;
}

CK_DLL_SFUN( cktz_set )
{
    t_CKINT pos = GET_CK_INT(ARGS);
    t_CKFLOAT val = *((t_CKFLOAT *)ARGS + 1);
    global_stack[pos] = (float) val;
    RETURN->v_int = 0;
}

DLL_QUERY tz_query( Chuck_DL_Query * QUERY )
{
    Chuck_Env *env = Chuck_Env::instance();
    QUERY->setname(QUERY, "Tiziku");

    QUERY->begin_class(QUERY, "Tiziku", "Object");
    QUERY->add_sfun(QUERY, cktz_foo, "int", "foo");

    QUERY->add_sfun(QUERY, cktz_set, "int", "set");
    QUERY->add_arg(QUERY, "int", "pos");
    QUERY->add_arg(QUERY, "float", "val");

    QUERY->end_class(QUERY);
    
    return TRUE;
}
