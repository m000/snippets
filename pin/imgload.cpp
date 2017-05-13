#include "pin.H"
#include <iostream>
#include <fstream>

// Globals
UINT32 imgCount = 0;
std::ostream *out = &std::cerr;

// Knobs
KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE,  "pintool",
    "o", "", "specify file name for the pin tool output");

// Helpers
INT32 Usage()
{
    cerr << "This tool prints tests image load callback. " << std::endl;
    cerr << KNOB_BASE::StringKnobSummary() << std::endl;
    return -1;
}

// Analysis routines
// ...

// Instrumentation callbacks
VOID ImageLoaded(IMG img, VOID *v)
{
    *out << "Image loaded: " << IMG_Name(img) << std::endl;
    imgCount++;
}

// Other callbacks
VOID Fini(INT32 code, VOID *v)
{
    *out <<  "===============================================" << std::endl;
    *out <<  "Pin tool analysis completed: " << std::endl;
    *out <<  "\t" << imgCount << " images were loaded." << std::endl;
    *out <<  "===============================================" << std::endl;
}

// main
int main(int argc, char *argv[])
{
    // Initialize Pin.
    PIN_InitSymbols();
    if( PIN_Init(argc,argv) ) { return Usage(); }

    // Initialize output stream.
    string fileName = KnobOutputFile.Value();
    if (!fileName.empty()) { out = new std::ofstream(fileName.c_str());}

    IMG_AddInstrumentFunction(ImageLoaded, NULL);
    PIN_AddFiniFunction(Fini, NULL);

    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
