#include <iostream>
#include <string>
#include <algorithm>

#include <list>
#include "pbxprojdef.h"
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;



bool endWith(const string str,const string needle) {
   if (str.length() >= needle.length()) {
      return (0 == str.compare (str.length() - needle.length(), needle.length(), needle));
    }
    return false;
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


class File
{
public:
  string isa;
  string lastKnownFileType;
  string name;
  string path;
  string cflag;
};

class PBXNativeTarget
{
public:
  //from target;
  string name;
  string productName;
  string productType;

  //from buildPhases
  vector<File> sources;
  vector<File> headers;
  vector<File> headerpaths;
  vector<File> frameworks;
  vector<File> resources;

  //from productReference;
  string result;
  string resulttype;
  
  //from buildConfigurationList;
  string buildargs;
  string infoplist;
  /*const PBXValueRef * buildConfigurationList;
  const PBXValueRef * productReference;
  const PBXArray * buildPhases;
  const PBXValueRef * mainGroup;*/

};

class PBXProj {
public:
  PBXProj();
  void loadProj(string path);
  int getTargetCount();
  vector<PBXNativeTarget> getTargets();
  void getAllFilesFromMainGroup(const PBXBlock * block, string current_path);
  string getBuildSettings(const PBXBlock *block);
  string getProductName(const PBXBlock *block);
  string getInfoPlist(const PBXBlock *block);

private:
  PBXFile *pDoc;
  map<string,string> allfiles;
};

PBXProj::PBXProj()
{
  pDoc = NULL;
  allfiles.clear();
}

void PBXProj::loadProj(string path)
{
  string projectDir(path);
  string projectFile(projectDir);
  loadProject(projectFile.c_str(), &pDoc);

  const PBXValueRef* ref = dynamic_cast<const PBXValueRef*>(pDoc->valueForKeyPath("rootObject.mainGroup"));
  const PBXValue *value = pDoc->deref(ref);
  const PBXBlock *block = PBXBlock::cast(value);

  getAllFilesFromMainGroup(block, ".");
}

int PBXProj::getTargetCount()
{
  const PBXArray *target = dynamic_cast<const PBXArray*>(pDoc->valueForKeyPath("rootObject.targets"));
  if(target) 
    return target->count();
  else
    return 0;
}

vector<PBXNativeTarget> PBXProj::getTargets()
{
  vector<PBXNativeTarget> targets;
  const PBXArray *target_arr = dynamic_cast<const PBXArray*>(pDoc->valueForKeyPath("rootObject.targets"));
  if(!target_arr)
    return targets;

  PBXValueList::const_iterator itor = target_arr->begin();
  PBXValueList::const_iterator end  = target_arr->end();
  for (; itor != end; ++itor) {
    const PBXValueRef* ref     = dynamic_cast<const PBXValueRef*>(*itor);
    const PBXValue *value = pDoc->deref(ref);
    const PBXBlock *target_blk = PBXBlock::cast(value);

    PBXItemList::const_iterator itor_blk = target_blk->begin();
    PBXItemList::const_iterator end_blk  = target_blk->end();

    PBXNativeTarget target_detail;
    
    const PBXText * name = dynamic_cast<const PBXText*> (target_blk->valueForKey("name"));
    if(name)
      target_detail.name = name->text();
    
    /*    const PBXText *productName = dynamic_cast<const PBXText*> (target_blk->valueForKey("productName"));
    if(productName)
      target_detail.productName = productName->text();
    */
    const PBXText *productType = dynamic_cast<const PBXText*> (target_blk->valueForKey("productType"));
    if(productType)
      target_detail.productType = productType->text();
      
    const PBXValueRef *bcl_ref = dynamic_cast<const PBXValueRef*>(target_blk->valueForKey("buildConfigurationList"));
    const PBXValue * bcl_value = dynamic_cast<const PBXValue *>(pDoc->deref(bcl_ref));
    const PBXBlock *bcl_blk = dynamic_cast<const PBXBlock*>(bcl_value);
    target_detail.buildargs = getBuildSettings(bcl_blk);
    target_detail.infoplist = getInfoPlist(bcl_blk);
    target_detail.productName =getProductName(bcl_blk);
    
    const PBXValueRef *pr_ref = dynamic_cast<const PBXValueRef*>(target_blk->valueForKey("productReference"));
    const PBXValue *pr_value = pDoc->deref(pr_ref);
    const PBXBlock *pr_blk = PBXBlock::cast(pr_value);
    
    const PBXText * explicitFileType =  dynamic_cast<const PBXText*>(pr_blk->valueForKey("explicitFileType"));
    if(explicitFileType)
      target_detail.resulttype = explicitFileType->text();
    
    const PBXText * path = dynamic_cast<const PBXText*>(pr_blk->valueForKey("path"));
    if(path)
      target_detail.result = path->text();
    
    const PBXArray *bp_arr = dynamic_cast<const PBXArray*>(target_blk->valueForKey("buildPhases"));
    if(bp_arr) {
      PBXValueList::const_iterator bp_itor = bp_arr->begin();
      PBXValueList::const_iterator bp_end  = bp_arr->end();
      for(; bp_itor != bp_end; bp_itor++){
	const PBXValueRef* bp_ref     = dynamic_cast<const PBXValueRef*>(*bp_itor);
	const PBXValue *bp_value = pDoc->deref(bp_ref);
	const PBXBlock *bp_blk = PBXBlock::cast(bp_value);
	  
	string bp_blk_type = "";	  
	const PBXText* bp_isa = dynamic_cast<const PBXText*>(bp_blk->valueForKey("isa"));
	if(bp_isa)
	  bp_blk_type = bp_isa->text();

	const PBXArray* files_arr = dynamic_cast<const PBXArray*>(bp_blk->valueForKey("files"));
	if(files_arr) {
	  PBXValueList::const_iterator files_itor = files_arr->begin();
	  PBXValueList::const_iterator files_end  = files_arr->end();
	  for(; files_itor != files_end; files_itor++){
	    const PBXValueRef * files_ref = dynamic_cast<const PBXValueRef*>(*files_itor);
	    const PBXValue *files_value = pDoc->deref(files_ref);
	    const PBXBlock *files_blk = PBXBlock::cast(files_value);

	    string cflag = "";
	    const PBXBlock * file_settings = dynamic_cast<const PBXBlock*>(files_blk->valueForKey("settings"));
	    if(file_settings) {
	      const PBXText * compiler_flags = dynamic_cast<const PBXText *> (file_settings->valueForKey("COMPILER_FLAGS"));
	      if(compiler_flags)
		cflag = compiler_flags->text();
	    }
	      
	    const PBXValueRef *file_ref = dynamic_cast<const PBXValueRef*>(files_blk->valueForKey("fileRef") );
	    
	    const PBXValue *file_value = pDoc->deref(file_ref);
	    const PBXBlock *file_blk = PBXBlock::cast(file_value);
	    
	    const PBXText * file_isa = dynamic_cast<const PBXText*>(file_blk->valueForKey("isa"));
	    string isa = file_isa->text();
	    File file;
	    if(isa == "PBXVariantGroup") {
	      //Group in resources seems mean a directory that should be keep its structure.
	      //So we try to get the directory name;
	      const PBXArray * children_arr = dynamic_cast<const PBXArray*>(file_blk->valueForKey("children"));
	      PBXValueList::const_iterator children_itor = children_arr->begin();
	      PBXValueList::const_iterator children_end  = children_arr->end();
	      for(; children_itor != children_end; children_itor++) {
		const PBXValueRef * c_ref = dynamic_cast<const PBXValueRef*>(*children_itor);
		const PBXValue * c_value = pDoc->deref(c_ref);
		const PBXBlock * c_blk = PBXBlock::cast(c_value);
		const PBXText * file_isa = dynamic_cast<const PBXText*>(c_blk->valueForKey("isa"));
		if(file_isa)
		  file.isa = file_isa->text();
		const PBXText * file_lastKnownFileType = dynamic_cast<const PBXText*>(c_blk->valueForKey("lastKnownFileType"));
		if(file_lastKnownFileType)
		  file.lastKnownFileType = file_lastKnownFileType->text();	
		const PBXText * file_name = dynamic_cast<const PBXText*>(c_blk->valueForKey("name"));
		if(file_name)
		  file.name = file_name->text();
		
		const PBXText * file_path = dynamic_cast<const PBXText*>(c_blk->valueForKey("path"));
		if(file_path) {
		  string local_path = file_path->text();
		  string full_path = local_path;
		  if(this->allfiles.find(local_path) != this->allfiles.end())
		    full_path = this->allfiles.find(local_path)->second;
		  full_path = ::dirname(strdup(full_path.c_str()));
		  file.path = full_path;
		}
	      }
	    }
	    else if(isa == "PBXFileReference") {
	      file.isa = file_isa->text();
	      file.cflag = cflag;
	      const PBXText * file_lastKnownFileType = dynamic_cast<const PBXText*>(file_blk->valueForKey("lastKnownFileType"));
	      if(file_lastKnownFileType)
		file.lastKnownFileType = file_lastKnownFileType->text();
	      const PBXText * file_name = dynamic_cast<const PBXText*>(file_blk->valueForKey("name"));
	      if(file_name)
		file.name = file_name->text();
		
	      const PBXText * file_path = dynamic_cast<const PBXText*>(file_blk->valueForKey("path"));
	      if(file_path) {
		string local_path = file_path->text();
		string full_path =local_path;
		if(this->allfiles.find(local_path) != this->allfiles.end())
		  full_path = this->allfiles.find(local_path)->second;
		file.path = full_path;
	      }
	    }

	    if(bp_blk_type == "PBXFrameworksBuildPhase" && !file.path.empty()) {
	      string framework = file.path;
	      char * frameworkpath = strdup(framework.c_str());
	      framework = ::basename(frameworkpath);
	      framework = framework.substr(0,framework.find(".framework"));
	      file.path = framework;
	      target_detail.frameworks.push_back(file);
	    }
	    else if(bp_blk_type == "PBXResourcesBuildPhase" && !file.path.empty())
	      target_detail.resources.push_back(file);
	    else if(bp_blk_type == "PBXSourcesBuildPhase" && !file.path.empty())
	      target_detail.sources.push_back(file);
	    else if(bp_blk_type == "PBXHeadersBuildPhase" && !file.path.empty()){
	      target_detail.headers.push_back(file); 
	      string headerpath = file.path;
	      char * headerpath_c = strdup(headerpath.c_str());
	      headerpath = ::dirname(headerpath_c);
	      file.path = m_replace(headerpath,"\"","",-1);
	      target_detail.headerpaths.push_back(file);
	    }
	      
	  }
	}
      }
    }

    targets.push_back(target_detail);
  }


  return targets;
}

void PBXProj::getAllFilesFromMainGroup(const PBXBlock *block, string current_path)
{
  string local_path = current_path;
  string block_type;

  const PBXText *isa =dynamic_cast<const PBXText*>(block->valueForKey("isa"));
  if(isa)
    block_type = isa->text();
  
  const PBXText * path = dynamic_cast<const PBXText*>(block->valueForKey("path"));
  if(path)
    local_path = local_path+"/"+path->text();

  if(block_type == "PBXFileReference") {
    // cout<<local_path<<endl;
    allfiles.insert(pair<string,string>(path->text(), local_path));
    //    this->allfiles.push_back(local_path);
  } else if (block_type == "PBXGroup") {
    const PBXArray *arr = dynamic_cast<const PBXArray *>(block->valueForKey("children"));
    PBXValueList::const_iterator itor = arr->begin();
    PBXValueList::const_iterator end  = arr->end();
    for(; itor != end; itor++){
      const PBXValueRef * ref = dynamic_cast<const PBXValueRef*>(*itor);
      const PBXValue *value = pDoc->deref(ref);
      const PBXBlock *blk = PBXBlock::cast(value);
      this->getAllFilesFromMainGroup(blk, local_path);
    }
  }
}

string PBXProj::getProductName(const PBXBlock *block)
{
  string productname;
  const PBXText * type = dynamic_cast<const PBXText *>(block->valueForKey("isa"));
  string btype = type->text();
  if(!type || btype != "XCConfigurationList")
    return productname;
  
  const PBXText * defaults = dynamic_cast<const PBXText *>(block->valueForKey("defaultConfigurationName"));
  if(!defaults)
    return productname;
  string bdefaults = defaults->text();
  const PBXArray * confs = dynamic_cast<const PBXArray *>(block->valueForKey("buildConfigurations"));
  if(!confs)
    return productname;

  PBXValueList::const_iterator itor = confs->begin();
  PBXValueList::const_iterator end  = confs->end();  

  for(; itor != end; itor++) {
    const PBXValueRef * ref = dynamic_cast<const PBXValueRef*>(*itor);
    const PBXValue *value = pDoc->deref(ref);
    const PBXBlock *blk = PBXBlock::cast(value); 
    const PBXText * name = dynamic_cast<const PBXText *>(blk->valueForKey("name"));
    string bname = name->text();
    if(bname != bdefaults )
      continue;
    const PBXBlock * settings = dynamic_cast<const PBXBlock *>(blk->valueForKey("buildSettings"));
    if(!settings)
      continue;
    const PBXText * pName = dynamic_cast<const PBXText *>(settings->valueForKey("PRODUCT_NAME"));
    if(pName){
      productname = pName->text();
      return productname;
    }
  }  
  return productname;
}

string PBXProj::getInfoPlist(const PBXBlock *block)
{
  string infoplist;
  const PBXText * type = dynamic_cast<const PBXText *>(block->valueForKey("isa"));
  string btype = type->text();
  if(!type || btype != "XCConfigurationList")
    return infoplist;
  
  const PBXText * defaults = dynamic_cast<const PBXText *>(block->valueForKey("defaultConfigurationName"));
  if(!defaults)
    return infoplist;
  string bdefaults = defaults->text();
  const PBXArray * confs = dynamic_cast<const PBXArray *>(block->valueForKey("buildConfigurations"));
  if(!confs)
    return infoplist;

  PBXValueList::const_iterator itor = confs->begin();
  PBXValueList::const_iterator end  = confs->end();  

  for(; itor != end; itor++) {
    const PBXValueRef * ref = dynamic_cast<const PBXValueRef*>(*itor);
    const PBXValue *value = pDoc->deref(ref);
    const PBXBlock *blk = PBXBlock::cast(value); 
    const PBXText * name = dynamic_cast<const PBXText *>(blk->valueForKey("name"));
    string bname = name->text();
    if(bname != bdefaults )
      continue;
    const PBXBlock * settings = dynamic_cast<const PBXBlock *>(blk->valueForKey("buildSettings"));
    if(!settings)
      continue;
    const PBXText * pList = dynamic_cast<const PBXText *>(settings->valueForKey("INFOPLIST_FILE"));
    if(pList){
      infoplist = pList->text();
      return infoplist;
    }
  }  
  return infoplist;
}

string PBXProj::getBuildSettings(const PBXBlock *block)
{
  string buildargs = "";
  const PBXText * type = dynamic_cast<const PBXText *>(block->valueForKey("isa"));
  string btype = type->text();
  if(!type || btype != "XCConfigurationList")
    return buildargs;
  
  const PBXText * defaults = dynamic_cast<const PBXText *>(block->valueForKey("defaultConfigurationName"));
  if(!defaults)
    return buildargs;
  string bdefaults = defaults->text();
  const PBXArray * confs = dynamic_cast<const PBXArray *>(block->valueForKey("buildConfigurations"));
  if(!confs)
    return buildargs;

  PBXValueList::const_iterator itor = confs->begin();
  PBXValueList::const_iterator end  = confs->end();  

  for(; itor != end; itor++) {
    const PBXValueRef * ref = dynamic_cast<const PBXValueRef*>(*itor);
    const PBXValue *value = pDoc->deref(ref);
    const PBXBlock *blk = PBXBlock::cast(value); 
    const PBXText * name = dynamic_cast<const PBXText *>(blk->valueForKey("name"));
    string bname = name->text();
    if(bname != bdefaults )
      continue;
    const PBXBlock * settings = dynamic_cast<const PBXBlock *>(blk->valueForKey("buildSettings"));
    if(!settings)
      continue;
    const PBXText * arc = dynamic_cast<const PBXText *>(settings->valueForKey("CLANG_ENABLE_OBJC_ARC"));
    if(arc && arc->text() == string("YES"))
      buildargs = buildargs + " -fobjc-arc";
    const PBXText * pch = dynamic_cast<const PBXText *>(settings->valueForKey("GCC_PRECOMPILE_PREFIX_HEADER"));
    if(pch && pch->text() == string("YES")) {
      const PBXText *pch_path = dynamic_cast<const PBXText *>(settings->valueForKey("GCC_PREFIX_HEADER"));
      if(pch_path)
	buildargs = buildargs + " -include " + pch_path->text();
    }   
  }
  
  vector<string> local_header_path;
  //search allfiles, find .h, add the path to buildargs;
  map <string, string>::iterator allfiles_Iter;
  for ( allfiles_Iter = allfiles.begin( ); allfiles_Iter != allfiles.end( ); allfiles_Iter++ ) {
    string file = allfiles_Iter->first;
    if(endWith(file, ".h")){
      string fullpath = allfiles_Iter->second;
      fullpath = ::dirname(strdup(fullpath.c_str()));
      local_header_path.push_back(fullpath);
    }
  }
  //remove duplicated one
  sort(local_header_path.begin(), local_header_path.end());
  local_header_path.erase(unique(local_header_path.begin(), local_header_path.end()), local_header_path.end()); 
  
  for(int i = 0; i < local_header_path.size(); i++) {
    buildargs = buildargs + " -I" + local_header_path[i];
  }
  return buildargs;
}


static void printHelp(const char* cmd);

void buildStaticLib(PBXNativeTarget target)
{
  cout <<"not implement yet"<<endl; 
}
void convertStaticLib(PBXNativeTarget target)
{
  cout <<"not implement yet"<<endl; 
  
}

void convertApp(PBXNativeTarget target)
{
  cout <<"not implement yet"<<endl; 
}

void buildApp(PBXNativeTarget target)
{
  string resultdir = target.result;
  
  string compiler = "ios-clang";
  string buildargs = target.buildargs;
  string output = target.result.substr(0,target.result.find(".app"));
  string sources = "";

  for(int i = 0; i < target.frameworks.size(); i++) {
    buildargs = buildargs + " -framework " + target.frameworks[i].path;
  }

  for(int i = 0; i < target.headerpaths.size(); i++) {
    buildargs = buildargs + " -I" + target.headerpaths[i].path;
  } 
 
  for(int i = 0; i < target.sources.size(); i++) {
    sources = sources + " " + target.sources[i].path;
  }

  string prepare_command = "rm -rf "+ resultdir + "&& mkdir " + resultdir;

  string compile_command = compiler + buildargs + sources + " -o " + resultdir+"/"+output;

  vector<string> resources;
  for(int i = 0; i < target.resources.size(); i++) {
    resources.push_back(target.resources[i].path);
  }
  sort(resources.begin(), resources.end());
  resources.erase(unique(resources.begin(), resources.end()), resources.end());
 
  string resources_command = "cp -r ";
  for(int i = 0; i < resources.size(); i++)
    resources_command = resources_command + " " + resources[i];
  resources_command = resources_command + " " + resultdir;

  string plist_command = "cp -r " + target.infoplist + " " + resultdir + "/Info.plist";

  string pngcrush_command = "find " + resultdir +" -name \\*.png|xargs ios-pngcrush -c";
  string compile_localization_command = "find " + resultdir +" -name \\*.strings|xargs ios-plutil -c";
  string compile_plist_command = "find " + resultdir +" -name \\*.plist|xargs ios-plutil -c";

  string fix_infoplist_command = "sed -i 's|${EXECUTABLE_NAME}|"+ output +"|g' " + resultdir + "/Info.plist";
  fix_infoplist_command += string(" && ") + string("sed -i 's|${PRODUCT_NAME}|")+ output +"|g' " + resultdir + "/Info.plist";
  //convert it to lower case.
  transform(output.begin(), output.end(), output.begin(), ::tolower);
  fix_infoplist_command += string(" && ") + string("sed -i 's|${PRODUCT_NAME:identifier}|")+ output +"|g' " + resultdir +"/Info.plist";
  fix_infoplist_command += string(" && ") + string("sed -i 's|${PRODUCT_NAME:rfc1034identifier}|")+ output +"|g' " + resultdir +"/Info.plist";
  
  cout <<prepare_command<<endl;
  system(prepare_command.c_str());

  cout <<compile_command <<endl;
  system(compile_command.c_str());

  cout <<resources_command<<endl; 
  system(resources_command.c_str());

  cout <<plist_command<<endl;
  system(plist_command.c_str());

  cout <<fix_infoplist_command<<endl;
  system(fix_infoplist_command.c_str());

  cout <<pngcrush_command<<endl;
  system(pngcrush_command.c_str());

  cout <<compile_localization_command<<endl;
  system(compile_localization_command.c_str());

  cout <<compile_plist_command<<endl;
  system(compile_plist_command.c_str());

  
}

void buildTarget(PBXNativeTarget target)
{
  if(target.resulttype == "archive.ar")
    buildStaticLib(target);
  else if(target.resulttype == "wrapper.application")
    buildApp(target);
  else
    cout <<"Not supported yet."<<endl;
}

void convertTarget(PBXNativeTarget target)
{
  if(target.resulttype == "archive.ar")
    convertStaticLib(target);
  else if(target.resulttype == "wrapper.application")
    convertApp(target);
  else
    cout <<"Not supported yet."<<endl;
}



int main(int argc, char* argv[])
{
  
  const char * cmd = argv[0]; 
  int willcompile = -1;
  if(argc == 1)
    printHelp(cmd);

  char optflag;
  while((optflag = getopt(argc, argv, "hcb")) != -1) {
    switch(optflag) {
    case 'c':
      willcompile = 0;
      break;
    case 'b':
      willcompile = 1;
      break;
    case 'h':
    default:
      printHelp(cmd);
    }
  }

  argc = argc - optind;
  argv = argv + optind;

  if(::access(argv[0],R_OK) != 0)
    printHelp(cmd);

  PBXProj *pbx = new PBXProj();
  pbx->loadProj(argv[0]);
  int target_count = pbx->getTargetCount();
  vector<PBXNativeTarget> targets = pbx->getTargets();
  if(target_count <= 0)
    cout <<"Error, no target found in project" <<endl;

  if(target_count >1) {
    cout <<"There is more than one targets in project files"<<endl;
    for(int i = 0; i < target_count; i++) {
      printf("%d, %s, %s, %s\n", i, targets[i].name.c_str(), targets[i].result.c_str(), targets[i].resulttype.c_str());
    }
    short input;
    do {
	cout <<"Please choose one:";
	cin>>input;
	if (cin.fail()) {
	  cin.clear();
	  cin.sync();
	}
    } while(input > target_count || input < 0);
    if(willcompile)
      buildTarget(targets[input]);
    else
      convertTarget(targets[input]);
  } else 
    if(willcompile)
      buildTarget(targets[0]);
    else
      convertTarget(targets[0]);
  
  return 0;
}


void printHelp(const char* cmd)
{
  printf("Usage: %s -c <project.pbxproj> : convert xcodeproj to make\n", cmd);
  printf("       %s -b <project.pbxproj> : build xcodeproj directly\n", cmd);
  exit(0);
}

