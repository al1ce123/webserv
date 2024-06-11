#include <string>
#include <map>

class Request;

class CGIHandler
{
private:
    std::map<std::string, std::string> _env;
    std::string _path; // Path to the cgi script

public:
    // Constructors
    CGIHandler();
    CGIHandler(CGIHandler& src);
    ~CGIHandler();
    CGIHandler& operator=(const CGIHandler& rhs);

    // Member functions
    void initEnv(Request& req);
    void execCgiScript(std::string& script_path, Request& req);
};