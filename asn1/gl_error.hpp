#ifndef GL_ERROR_HPP
#define GL_ERROR_HPP

#include <string>

// Debugging tool.  After any openGL call, you can
// call the check() function to report errors that may have occured.

class GL_Error {
public:
    static void check(const std::string& msg);
};

#endif
