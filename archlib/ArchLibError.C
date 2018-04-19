// sccs id:  @(#)ArchLibError.C	1.1        12/04/03

// ArchLibError.C
//
// Warren Carithers
//      December 2003
//

#include <ArchLibError.h>

using namespace std;

ArchLibError::ArchLibError( const char * what_arg ) :
    runtime_error(what_arg) {
}
