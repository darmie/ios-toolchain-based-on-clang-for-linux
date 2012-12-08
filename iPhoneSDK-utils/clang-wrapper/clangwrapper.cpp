#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <libxml/parser.h> 
#include <libxml/tree.h> 

using namespace std;

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

//default arch to armv7
string default_arch = "armv7";
//config file location 
string config_file = string(getenv("HOME")) + "/.iphonesdk";


string trim(string s) {
  if (s.length() == 0) return s;
  size_t beg = s.find_first_not_of(" \a\b\f\n\r\t\v");
  size_t end = s.find_last_not_of(" \a\b\f\n\r\t\v");
  if (beg == string::npos) return "";
  return string(s, beg, end - beg + 1);
}

bool beginWith(const string str,const string needle) {
        return (!str.compare(0,needle.length(),needle));
}

bool endWith(const string str,const string needle) {
   if (str.length() >= needle.length()) {
      return (0 == str.compare (str.length() - needle.length(), needle.length(), needle));
    }
    return false;
}

//create a array based on PATH env var.
vector<string> fillpath(void)
{
    vector<string> pathdir;
    char *key=NULL, *tok=NULL, *pathcp, *path = getenv("PATH");
    int i = 0;

    if (!path)
        return pathdir;
    pathcp = strdup(path);

    for (tok = strtok_r(pathcp, ":", &key); tok;
         tok = strtok_r(NULL, ":", &key)) {
         pathdir.push_back(strdup(tok));
    }
    free(pathcp);
    return pathdir;
}

string find_cmd_end_with_in_dir(string dir, string begin, string end)
{
    string target_file;
    DIR *dirptr=NULL;
    struct dirent *entry;
    if((dirptr = opendir(dir.c_str()))==NULL)
        return "";
    while((entry=readdir(dirptr)))
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

string find_file_in_dir(string dir, string file, string begin, string end)
{
  string target_file;
  DIR *dirptr=NULL;
  struct dirent *entry;
  if((dirptr = opendir(dir.c_str()))==NULL)
    {
      printf("opendir failed!");
      return "";
    }
  while((entry=readdir(dirptr)))
    {
      string filename = entry->d_name;
      if(filename == file) {
	target_file = filename;
	break;
      }
      if(endWith(filename,end) && beginWith(filename, begin)) {
	target_file = filename;
	break;
      }
    }
  closedir(dirptr);
  return target_file;
}


string find_dir_in_dir(string dir, string dirname, string begin, string end)
{
  string target_file = find_file_in_dir(dir, dirname, begin, end);
  struct stat st;
  stat( (dir + "/" +target_file).c_str(), &st );

  if(!target_file.empty() && S_ISDIR(st.st_mode))
    return target_file;
  
  return "";
}



int get_value_of_key_from_plist(char *valuestr, const char *keystr, const char *plistfile)
{
  xmlDocPtr doc;   
  xmlNodePtr cur; 
   
  doc = xmlReadFile(plistfile, "UTF-8",XML_PARSE_RECOVER ); 

  if (doc == NULL ) { 
    fprintf(stderr,"Document not parsed successfully. \n"); 
    return 0; 
  } 
  cur = xmlDocGetRootElement(doc); 

  if (cur == NULL) { 
    fprintf(stderr,"empty document\n"); 
    xmlFreeDoc(doc); 
    return 0; 
  } 

  if (xmlStrcmp(cur->name, (const xmlChar *) "plist")) { 
    fprintf(stderr,"document of the wrong type, root node != plist"); 
    xmlFreeDoc(doc); 
    return 0; 
  } 
  
  cur = cur->xmlChildrenNode;


  while(cur != NULL) {
    if ((!xmlStrcmp(cur->name, (const xmlChar *)"dict"))) {
      
      xmlNodePtr keyNode = cur->xmlChildrenNode;
      xmlNodePtr valuePtr = NULL;
      xmlChar *key; 
      
      while (keyNode != NULL) {
    if((!xmlStrcmp(keyNode->name, (const xmlChar *)"key"))) {
      key = xmlNodeGetContent(keyNode);

      if(!strcmp((const char *)key, keystr)) {
        valuePtr = keyNode->next;
        while(xmlStrcmp(valuePtr->name, (const xmlChar *)"string")){
          valuePtr = valuePtr->next;
        }
        if(valuePtr) {
          xmlChar * value = xmlNodeGetContent(valuePtr);
          strcpy(valuestr, (const char *)value);
          xmlFree(value);
        }
      }
      xmlFree(key);
    }
    keyNode = keyNode->next;
      }
    } 
    cur = cur->next;
  }
  xmlFreeDoc(doc); 
  return 1;
}


string get_sdk_full_path()
{
 enterpath:
  
  cout<<"Please input your iPhone SDK absolute path : ";
  string sdk_fullpath;
  cin>>sdk_fullpath;
  if(!sdk_fullpath.empty()) {
    //I do not understand what you input.
    if(::access(sdk_fullpath.c_str(),R_OK) != 0) {
      cout<<RED "Wrong iPhone SDK path, Please enter the SDK path again!" NONE<<endl;
      goto enterpath;
    }
    //not absolute path
    if(!beginWith(sdk_fullpath, "/")) {
      cout<<RED "Wrong iPhone SDK path, Please enter the SDK path again!" NONE<<endl;
      goto enterpath;
    }

    //is dir?
    struct stat st;
    stat(sdk_fullpath.c_str(), &st);
    if(S_ISDIR(st.st_mode)) {
      //SDKSettings.plist should always be exists.
      //otherwise, this is must be wrong.
      string sdk_file = sdk_fullpath + "/SDKSettings.plist";
      if(::access(sdk_file.c_str(),R_OK) == 0)
	    return sdk_fullpath;
      else {
	    cout<<RED "Wrong iPhone SDK path, " GREEN "SDKSettings.plist" RED " not exists"<<endl 
	        <<"Please enter the SDK path again!" NONE<<endl;
	    goto enterpath;
      }
    }
  }
}

string get_sdk_version_from_sdk_file(string sdk_file)
{

  char version[10];
  bzero(version,10);
  if(get_value_of_key_from_plist(version, "Version", sdk_file.c_str()))
    return version;
  else
    return "";
}


void detect_sdk_and_write_configfile()
{
    string sdk_fullpath;
    string version;

    cout << LIGHT_BLUE "iPhone SDK Setup" NONE <<endl;
    // (2) detect iPhoneSDK
    cout << "\nThis is the first time you use iPhone toolchain for linux." <<endl;
    cout << "\n" <<endl;

    sdk_fullpath = find_dir_in_dir("/usr/share", "iPhoneOS5.0.sdk", "iPhoneOS", ".sdk");
    if(sdk_fullpath.empty())
      sdk_fullpath = get_sdk_full_path();
    else
      sdk_fullpath = "/usr/share/" + sdk_fullpath;

    string sdk_file = sdk_fullpath + "/SDKSettings.plist";

    version = get_sdk_version_from_sdk_file(sdk_file);
    if(!version.empty())
      cout<<GREEN "Find \"iPhoneOS"<<version<<".sdk\" in "
      << "\""+sdk_fullpath+"\"" << NONE
      << endl;
    else
      version = "5.0";

    ofstream config(config_file.c_str());
    config << "SDK_FULL_PATH=" << sdk_fullpath <<endl;
    config << "SDK_VERSION=" << version <<endl;
    config.close();
}

string read_sdkpath_from_configfile()
{
    string sdk_fullpath;
    ifstream fin(config_file.c_str());
    string line;
    while(getline(fin,line)) {
      if(beginWith(trim(line),"SDK_FULL_PATH="))
            sdk_fullpath = trim(line.substr(14,line.length()));
    }
    fin.close();
    return sdk_fullpath;
}

string read_sdkversion_from_configfile()
{
    string version;
    ifstream fin(config_file.c_str());
    string line;
    while(getline(fin,line)) {
      if(beginWith(trim(line), "SDK_VERSION="))
            version = trim(line.substr(12,line.length()));
    }
    fin.close();
    return version;
}

extern char ** environ;

int main(int argc, char **argv)
{


  // (1) detect toolchain.
  string clang = find_cmd_full_path("clang","clang","clang");
  string ldid = find_cmd_full_path("ldid","ldid","ldid");
  string as = find_cmd_full_path("arm-apple-darwin9-as","arm-apple","as");
  string target = as.substr(as.find("arm"), as.length()-3-as.find("arm"));

  
  if(clang.empty() || ldid.empty() || as.empty()) {
    cout <<"Can not find proper toolchain commands."<<endl;
    cout <<"You may need install clang, ldid, cctools" <<endl;
    cout <<"And setup PATH environment variable according to your installation" <<endl;
    exit(0);
  }


  string sdk_fullpath = "/usr/share/iPhoneOS5.0.sdk";
  string version = "5.0";

  int should_init = 0;
  if(endWith(string(argv[0]),"iosclangwrapper"))
    should_init = 1;

  //if ~/.iphonesdk not exists, detect it.  
  if(::access(config_file.c_str(),R_OK) != 0 || should_init)
    detect_sdk_and_write_configfile();

  sdk_fullpath = read_sdkpath_from_configfile();
  version = read_sdkversion_from_configfile();

  //check sdk path.
  if(::access(sdk_fullpath.c_str(), R_OK) != 0) {
    detect_sdk_and_write_configfile();
    sdk_fullpath = read_sdkpath_from_configfile();
    version = read_sdkversion_from_configfile();
  }


  // for SDK 4.x, set default arch to armv6
  if(beginWith(version,"4."))
       default_arch = "armv6";
    
  string command = "clang";
  string caller = argv[0];
  if(endWith(caller,"ios-clang"))
    command = clang;
  else if(endWith(caller,"ios-clang++"))
    command = clang + "++";
 
  //look in argv, if -arch had been setted, just use the settings.
  for(int i = 0; i < argc; i++) {
    if(strcmp(argv[i],"-arch") == 0 && (i+1) < argc) {
        string arch = argv[i+1];
        if(beginWith(arch,"arm"))
            default_arch = arch;
        break;
    }
  } 
     
  // cmd args for execvpe;
  char **cmd = (char **)malloc((7 + argc)*sizeof(char*));
  cmd[0] = strdup(command.c_str());
  cmd[1] = strdup("-target");
  cmd[2] = strdup(target.c_str());
  cmd[3] = strdup("-arch");
  cmd[4] = strdup(default_arch.c_str());
  cmd[5] = strdup("-isysroot");
  cmd[6] = strdup(sdk_fullpath.c_str());
  for (int i = 1; i < argc; i ++) {
    cmd[6+i] = argv[i];
  }
  cmd[6+argc] = (char *)0;

  // env for execvpe
  int count = 0;
  for(int i = 0; environ[i] != NULL; i++ ) {
    count++;
  }
  char ** env_l = (char **)malloc((count+1) * sizeof(char*));
  for(int i = 0; environ[i] != NULL; i++ ) {
    env_l[i] = environ[i];
  }
  string iphone_env = "IPHONEOS_DEPLOYMENT_TARGET=" + version;

  env_l[count] = (strdup(iphone_env.c_str()));
  env_l[count+1] = (char *)0;

  //run command.
  execvpe(command.c_str(), cmd, env_l);
  return 0;
}
