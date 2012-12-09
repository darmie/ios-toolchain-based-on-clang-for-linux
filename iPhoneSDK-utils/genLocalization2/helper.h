#ifndef _HELPER_H
#define _HELPER_H
#include <string>

#define NONE "\033[m"
#define RED "\033[0;32;31m"
#define LIGHT_RED "\033[1;31m"
#define GREEN "\033[0;32;32m"
#define LIGHT_GREEN "\033[1;32m"
#define BLUE "\033[0;32;34m"
#define LIGHT_BLUE "\033[1;34m"
#define DARY_GRAY "\033[1;30m"
#define CYAN "\033[0;36m"
#define LIGHT_CYAN "\033[1;36m"
#define PURPLE "\033[0;35m"
#define LIGHT_PURPLE "\033[1;35m"
#define BROWN "\033[0;33m"
#define YELLOW "\033[1;33m"
#define LIGHT_GRAY "\033[0;37m"
#define WHITE "\033[1;37m"

using namespace std;

string trim(string s);
bool beginWith(const string str,const string needle);
bool endWith(const string str,const string needle);
string find_cmd_full_path(string cmd, string begin, string end);
string get_sdk_full_path();
void detect_sdk_and_write_configfile();
string read_sdkpath_from_configfile();
string read_sdkversion_from_configfile();
string find_dir_in_dir(string dir, string dirname, string begin, string end);
string find_file_in_dir(string dir, string file, string begin, string end);
int get_value_of_key_from_plist(char *valuestr, const char *keystr, const char *plistfile);
#endif
