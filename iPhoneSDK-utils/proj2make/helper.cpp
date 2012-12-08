#include <string>
#include <dirent.h>
#include "helper.h"
//for stat()
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

bool endWith(const std::string str,const std::string needle){
   if (str.length() >= needle.length()) {
      return (0 == str.compare (str.length() - needle.length(), needle.length(), needle));
    }
    return false;
}

string find_file_end_with(string end)
{
    string target_file;
    DIR *dirptr=NULL;
    struct dirent *entry;
    if((dirptr = opendir("."))==NULL)
    {
    printf("opendir failed!");
    return "";
    }
    while(entry=readdir(dirptr))
    {
        string filename = entry->d_name;
        if(endWith(filename,end)) {
            target_file = filename;
            break;
        }
    }
    closedir(dirptr);
    return target_file;
}

string find_dir_end_with(string end)
{
    string target_file = find_file_end_with(end);
    struct stat st;
    stat( target_file.c_str(), &st );

    if(!target_file.empty() && S_ISDIR(st.st_mode))
        return target_file;
    
    return "";
}


std::string m_replace(std::string str,std::string pattern,std::string dstPattern,int count)
{
    std::string retStr="";
    string::size_type pos;

    int szStr=str.length();
    int szPattern=pattern.size();
    int i=0;
    int l_count=0;
    if(-1 == count) // replace all
        count = szStr;

    for(i=0; i<szStr; i++)
    {
        pos=str.find(pattern,i);

        if(std::string::npos == pos)
            break;
        if(pos < szStr)
        {
            std::string s=str.substr(i,pos-i);
            retStr += s;
            retStr += dstPattern;
            i=pos+pattern.length()-1;
            if(++l_count >= count)
            {
                i++;
                break;
            }
        }
    }
    retStr += str.substr(i);
    return retStr;
}
