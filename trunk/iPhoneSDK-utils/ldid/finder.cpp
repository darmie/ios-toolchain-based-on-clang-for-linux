#include <string>
#include <dirent.h>
#include "finder.h"
//for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "xalloc.h"
#include <vector>

using namespace std;

vector<string> fillpath(void)
{
    vector<string> pathdir;
    char *key=NULL, *tok=NULL, *pathcp, *path = getenv("PATH");
    int i = 0;

    if (!path)
        return pathdir;
    pathcp = xstrdup(path);

    for (tok = strtok_r(pathcp, ":", &key); tok;
         tok = strtok_r(NULL, ":", &key)) {
         pathdir.push_back(xstrdup(tok));
    }
    free(pathcp);
    return pathdir;
}


bool beginWith(const std::string str,const std::string needle){
        return (!str.compare(0,needle.length(),needle));
}

bool endWith(const std::string str,const std::string needle){
   if (str.length() >= needle.length()) {
      return (0 == str.compare (str.length() - needle.length(), needle.length(), needle));
    }
    return false;
}

string find_cmd_end_with_in_dir(string dir, string begin, string end)
{
    string target_file;
    DIR *dirptr=NULL;
    struct dirent *entry;
    if((dirptr = opendir(dir.c_str()))==NULL)
        return "";
    while(entry=readdir(dirptr))
    {
        string filename = entry->d_name;
        if(endWith(filename, end) && beginWith(filename, begin)) {
            target_file = filename;
            break;
        }
    }
    closedir(dirptr);
    return target_file;
}


string find_cmd_full_path(string cmd, string begin, string end)
{
    string cmd_fullpath; 
    
    vector<string> pathdir = fillpath();
    for(int i = 0 ; i < pathdir.size(); i++) {
        string cmd_fullpath = pathdir[i] + "/" + cmd;
        if(access(cmd_fullpath.c_str(), X_OK) == 0)
            return cmd_fullpath;
    }
   
    for(int i = 0 ; i < pathdir.size(); i++) {
        cmd_fullpath = find_cmd_end_with_in_dir(pathdir[i], begin, end);
        if(!cmd_fullpath.empty())
            return pathdir[i] +"/"+cmd_fullpath;
    }
    return cmd_fullpath;
}

int tmain(int argc, char **argv)
{
    string cmd = "arm-apple-darwin10-codesign_allocate";
    string cmd_begin = "arm-apple";
    string cmd_end = "codesign_allocate";
    string cmd_fullpath = find_cmd_full_path(cmd,cmd_begin,cmd_end);
    if(!cmd_fullpath.empty())
        printf("%s\n",cmd_fullpath.c_str());
}
