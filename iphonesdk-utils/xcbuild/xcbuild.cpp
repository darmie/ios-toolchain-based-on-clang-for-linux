#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <list>
#include "pbxprojdef.h"
#include <stdio.h>
#include <libgen.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

using namespace std;

//this is a wrapper for system, if error happened when compilation, then exit.

void runCommand(const char * command)
{
  int ret = system(command);
  if(ret != 0)
    exit(1);
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

  //for static lib
  string symroot;
  string public_header_path;
  string private_header_path;
};

class PBXProj {
public:
  PBXProj();
  void loadProj(string path);
  int getTargetCount();
  vector<PBXNativeTarget> getTargets();

private:
  void getAllFilesFromMainGroup(const PBXBlock * block, string current_path);
  string getBuildSettings(const PBXBlock *block);
  string getProductName(const PBXBlock *block);
  string getSymRoot(const PBXBlock *block);
  string getPublicHeaderPath(const PBXBlock *block);
  string getPrivateHeaderPath(const PBXBlock *block);
  string getInfoPlist(const PBXBlock *block);
  void initTargets();
  vector<PBXNativeTarget> targets;
  PBXFile *pDoc;
  map<string,string> allFiles;
  int targetcount;
};

PBXProj::PBXProj()
{
  pDoc = NULL;
  allFiles.clear();
  targetcount = 0;
  targets.clear();
}

void PBXProj::loadProj(string path)
{
  string projectDir(path);
  string projectFile(projectDir);
  loadProject(projectFile.c_str(), &pDoc);

  //loading all files in project when opened.
  const PBXValueRef* ref = dynamic_cast<const PBXValueRef*>(pDoc->valueForKeyPath("rootObject.mainGroup"));
  const PBXValue *value = pDoc->deref(ref);
  const PBXBlock *block = PBXBlock::cast(value);

  getAllFilesFromMainGroup(block, ".");
  initTargets();
}

int PBXProj::getTargetCount()
{
  return this->targetcount;
}

vector<PBXNativeTarget> PBXProj::getTargets()
{
  return this->targets;
}
void PBXProj::initTargets()
{
  const PBXArray *target_arr = dynamic_cast<const PBXArray*>(pDoc->valueForKeyPath("rootObject.targets"));
  if(!target_arr)
    return;

  this->targetcount =  target_arr->count();

  PBXValueList::const_iterator itor = target_arr->begin();
  PBXValueList::const_iterator end  = target_arr->end();
  for (; itor != end; ++itor) {
    const PBXValueRef* ref     = dynamic_cast<const PBXValueRef*>(*itor);
    const PBXValue *value = pDoc->deref(ref);
    const PBXBlock *target_blk = PBXBlock::cast(value);

    PBXNativeTarget target_detail;

    //ignore PBXAggregateTarget
    const PBXText * isa =  dynamic_cast<const PBXText*>(target_blk->valueForKey("isa"));
    if(isa->text() == string("PBXAggregateTarget")) {
      this->targetcount -= 1; 
      continue;  
    }
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
    target_detail.symroot = m_replace(getSymRoot(bcl_blk), "$(SRCROOT)", ".", -1);
    target_detail.symroot = m_replace(target_detail.symroot,"\"","",-1);
    if(target_detail.symroot.empty())
      target_detail.symroot = ".";
    target_detail.public_header_path = target_detail.symroot +"/";
    if(getPublicHeaderPath(bcl_blk).empty())
      target_detail.public_header_path += "build/include";
    else
      target_detail.public_header_path += getPublicHeaderPath(bcl_blk);
    target_detail.public_header_path = m_replace(target_detail.public_header_path, "\"","",-1);
    target_detail.private_header_path = m_replace(getPrivateHeaderPath(bcl_blk), "$(PUBLIC_HEADERS_FOLDER_PATH)",target_detail.public_header_path,-1);
    target_detail.private_header_path = m_replace(target_detail.private_header_path,"\"","",-1);
    
    const PBXValueRef *pr_ref = dynamic_cast<const PBXValueRef*>(target_blk->valueForKey("productReference"));
    const PBXValue *pr_value = pDoc->deref(pr_ref);
    const PBXBlock *pr_blk = PBXBlock::cast(pr_value);
    
    const PBXText * explicitFileType =  dynamic_cast<const PBXText*>(pr_blk->valueForKey("explicitFileType"));
    if(explicitFileType)
      target_detail.resulttype = explicitFileType->text();
    
    const PBXText * path = dynamic_cast<const PBXText*>(pr_blk->valueForKey("path"));
    if(path) {
      string result = path->text();
      result = m_replace(result," ", "_", -1);
      result = m_replace(result,"\"", "", -1);
      target_detail.result = result;//path->text();
    }
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
		cflag = m_replace(compiler_flags->text(),"\"","",-1);
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
		  if(this->allFiles.find(local_path) != this->allFiles.end())
		    full_path = this->allFiles.find(local_path)->second;
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
		if(this->allFiles.find(local_path) != this->allFiles.end())
		  full_path = this->allFiles.find(local_path)->second;
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

    this->targets.push_back(target_detail);
  }
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
    allFiles.insert(pair<string,string>(path->text(), local_path));
    //    this->allFiles.push_back(local_path);
  } else if (block_type == "PBXGroup") {
    const PBXArray *arr = dynamic_cast<const PBXArray *>(block->valueForKey("children"));
    if(!arr)
      return;
    PBXValueList::const_iterator itor = arr->begin();
    PBXValueList::const_iterator end  = arr->end();
    for(; itor != end; itor++){
      const PBXValueRef * ref = dynamic_cast<const PBXValueRef*>(*itor);
      const PBXValue *value = pDoc->deref(ref);
      const PBXBlock *blk = PBXBlock::cast(value);
      if(!blk)
        return;
      this->getAllFilesFromMainGroup(blk, local_path);
    }
  }
}

string PBXProj::getSymRoot(const PBXBlock *block)
{
  string symroot;
  const PBXText * type = dynamic_cast<const PBXText *>(block->valueForKey("isa"));
  string btype = type->text();
  if(!type || btype != "XCConfigurationList")
    return symroot;
  
  const PBXText * defaults = dynamic_cast<const PBXText *>(block->valueForKey("defaultConfigurationName"));
  if(!defaults)
    return symroot;
  string bdefaults = defaults->text();
  const PBXArray * confs = dynamic_cast<const PBXArray *>(block->valueForKey("buildConfigurations"));
  if(!confs)
    return symroot;

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
    const PBXText * sRoot = dynamic_cast<const PBXText *>(settings->valueForKey("SYMROOT"));
    if(sRoot){
      symroot = sRoot->text();
      return symroot;
    }
  }  
  return symroot;
}

string PBXProj::getPublicHeaderPath(const PBXBlock *block)
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
    const PBXText * pName = dynamic_cast<const PBXText *>(settings->valueForKey("PUBLIC_HEADERS_FOLDER_PATH"));
    if(pName){
      productname = pName->text();
      return productname;
    }
  }  
  return productname;
}



string PBXProj::getPrivateHeaderPath(const PBXBlock *block)
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
    const PBXText * pName = dynamic_cast<const PBXText *>(settings->valueForKey("PRIVATE_HEADERS_FOLDER_PATH"));
    if(pName){
      productname = pName->text();
      return productname;
    }
  }  
  return productname;
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
      m_replace(productname, " ", "_", -1);
      m_replace(productname, "\"", "", -1);
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
  //search allFiles, find .h, add the path to buildargs;
  map <string, string>::iterator allFiles_Iter;
  for ( allFiles_Iter = allFiles.begin( ); allFiles_Iter != allFiles.end( ); allFiles_Iter++ ) {
    string file = allFiles_Iter->first;
    if(endWith(file, ".h")){
      string fullpath = allFiles_Iter->second;
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

  string header_cmd = "rm -rf "+ target.public_header_path + " && mkdir -p " + target.public_header_path + "&& mkdir -p build/include";
  cout << header_cmd <<endl;
  runCommand(header_cmd.c_str());

  cout << header_cmd <<endl;   
  for(int i = 0; i < target.headers.size(); i++) {
    string copy_header_cmd = "cp -r " + target.headers[i].path + " " + target.public_header_path; 
    cout << copy_header_cmd <<endl;
    runCommand(copy_header_cmd.c_str());
  }

  string compiler = "ios-clang";
  string buildargs = target.buildargs;

  buildargs += " -I.";
  /*buildargs += " -I" + target.public_header_path;
  buildargs += " -I" + string(::dirname(strdup(target.public_header_path.c_str())));*/
  vector<string> headerpaths;
  for(int i = 0; i < target.headerpaths.size(); i++) {
    headerpaths.push_back(target.headerpaths[i].path);
  }
  sort(headerpaths.begin(), headerpaths.end());
  headerpaths.erase(unique(headerpaths.begin(), headerpaths.end()), headerpaths.end());


  for(int i = 0; i < headerpaths.size(); i++) {
    buildargs = buildargs + " -I" + headerpaths[i];
  }

  string output = target.result.substr(0,target.result.find(".app"));

  vector<string> objects;

  for(int i = 0; i < target.sources.size(); i++) {
    string compile_cmd = compiler + " " + buildargs +" -c " + target.sources[i].path + " "+ target.sources[i].cflag +" "+" -o "+ m_replace(target.sources[i].path,".m",".o",-1);
    objects.push_back( m_replace(target.sources[i].path,".m",".o",-1));
    cout <<compile_cmd<<endl;
    runCommand(compile_cmd.c_str());
  }  
  
  string ar_command = "arm-apple-darwin11-ar cr build/" + output + " ";
  for(int i = 0; i < objects.size(); i++ ) {
    ar_command = ar_command + " "+ objects[i] ;
  }  
  cout <<ar_command<<endl;
  runCommand(ar_command.c_str());

  //rename headers if headers not in build dir;
  if(!beginWith(target.public_header_path, "./build")) {
    string rename_header_command = "mv " + target.public_header_path +  " build/include";
    cout <<rename_header_command <<endl;
    runCommand(rename_header_command.c_str());
  }
}

void convertStaticLib(PBXNativeTarget target)
{
  cout <<"not implement yet"<<endl; 
  
}

void convertApp(PBXNativeTarget target)
{

  string compiler = "ios-clang";
  string buildargs = target.buildargs;
  string output = target.result.substr(0,target.result.find(".app"));

  
  vector<string> headerpaths;
  for(int i = 0; i < target.headerpaths.size(); i++) {
    headerpaths.push_back(target.headerpaths[i].path);
  }
  sort(headerpaths.begin(), headerpaths.end());
  headerpaths.erase(unique(headerpaths.begin(), headerpaths.end()), headerpaths.end());


  for(int i = 0; i < headerpaths.size(); i++) {
    buildargs = buildargs + " -I" + headerpaths[i];
  }

  ofstream makefile("./Makefile");
  makefile << "IPHONE_IP:=" <<endl;
  makefile << "PROJECTNAME:="<<target.productName<<endl;
  makefile << "APPFOLDER:=$(PROJECTNAME).app" <<endl;
  makefile << "INSTALLFOLDER:=$(PROJECTNAME).app"<<endl;
  makefile << endl;
  makefile << "CC:=ios-clang" <<endl;
  makefile << "CPP:=ios-clang++" <<endl;
  makefile << endl;
  makefile << "CFLAGS +="<<buildargs<<endl;
  makefile << endl;
  makefile << "CPPFLAGS +="<<buildargs<<endl;
  makefile << endl;
  for(int i = 0; i < target.frameworks.size(); i++) {
   makefile <<"LDFLAGS += -framework " + target.frameworks[i].path <<endl;
  }
  makefile << endl;
 
  vector<string> sourcepaths;
  for(int i = 0; i < target.sources.size(); i++) {
    sourcepaths.push_back(::dirname(strdup(target.sources[i].path.c_str())));
  }
  
  sort(sourcepaths.begin(), sourcepaths.end());
  sourcepaths.erase(unique(sourcepaths.begin(), sourcepaths.end()), sourcepaths.end());
  
  for(int i = 0; i < sourcepaths.size(); i++) {
    makefile << "SRCDIR"<< i<< "="<< sourcepaths[i]<<endl;
    makefile << "OBJS+=$(patsubst \%.m,\%.o,$(wildcard $(SRCDIR"<<i<<")/*.m))"<<endl;
    makefile << "OBJS+=$(patsubst \%.c,\%.o,$(wildcard $(SRCDIR"<<i<<")/*.c))"<<endl;
    makefile << "OBJS+=$(patsubst %.cpp,%.o,$(wildcard $(SRCDIR"<<i<<")/*.cpp))"<<endl;
    makefile << endl;
  }
  
  makefile << "all:  $(PROJECTNAME)"<<endl<<endl;;
  makefile << "$(PROJECTNAME): $(OBJS)"<<endl;
  makefile << "\t$(CC) $(CFLAGS) $(LDFLAGS) $(filter %.o,$^) -o $@"<<endl<<endl;;
  makefile << "\%.o:  \%.m"<<endl;
  makefile << "\t$(CC) -c $(CFLAGS) $< -o $@"<<endl<<endl;
  makefile <<endl;
  makefile << "\%.o:  \%.c"<<endl;
  makefile << "\t$(CC) -c $(CFLAGS) $< -o $@"<<endl<<endl;
  makefile <<endl;
  makefile << "\%.o:  \%.cpp" <<endl;
  makefile << "\t$(CPP) -c $(CPPFLAGS) $< -o $@"<<endl<<endl;
  

  makefile << "INFOPLIST:=" <<target.infoplist<<endl <<endl;


  vector<string> resources;
  for(int i = 0; i < target.resources.size(); i++) {
    resources.push_back(target.resources[i].path);
  }
  sort(resources.begin(), resources.end());
  resources.erase(unique(resources.begin(), resources.end()), resources.end());

  makefile << "RESOURCES += \\" <<endl;
  for(int i = 0; i < resources.size(); i++) {
    if(i == resources.size()-1)
      makefile << "\t"<< resources[i] <<endl;
    else
      makefile << "\t"<< resources[i] <<" \\"<<endl;
  }
  makefile << endl;

  makefile << "dist: $(PROJECTNAME)"<<endl;
  makefile << "\tmkdir -p $(APPFOLDER)"<<endl;
  makefile << "\tcp -r $(RESOURCES) $(APPFOLDER)"<<endl;
  makefile << "\tcp $(INFOPLIST) $(APPFOLDER)/Info.plist"<<endl;
  makefile << "\tcp $(PROJECTNAME) $(APPFOLDER)"<<endl;
  makefile << "\tsed -i 's|$${EXECUTABLE_NAME}|" << output << "|g' " << "$(APPFOLDER)/Info.plist"<<endl;
  makefile << "\tsed -i 's|$${PRODUCT_NAME}|" << output << "|g' " << "$(APPFOLDER)/Info.plist"<<endl;
  transform(output.begin(), output.end(), output.begin(), ::tolower);
  makefile << "\tsed -i 's|$${PRODUCT_NAME:identifier}|" << output << "|g' " << "$(APPFOLDER)/Info.plist"<<endl;
  makefile << "\tsed -i 's|$${PRODUCT_NAME:rfc1034identifier}|" << output << "|g' " << "$(APPFOLDER)/Info.plist"<<endl;
  makefile << "\tfind $(APPFOLDER) -name \\*.png|xargs ios-pngcrush -c"<<endl;
  makefile << "\tfind $(APPFOLDER) -name \\*.plist|xargs ios-plutil -c"<<endl;
  makefile << "\tfind $(APPFOLDER) -name \\*.strings|xargs ios-plutil -c"<<endl;
  makefile << endl;
  makefile << "langs:"<<endl;
  makefile << "\tios-genLocalization"<<endl<<endl;

  makefile << "install: dist"<<endl;
  makefile << "ifeq ($(IPHONE_IP),)"<<endl;
  makefile << "\techo \"Please set IPHONE_IP\""<<endl;
  makefile << "else"<<endl;
  makefile << "\tssh root@$(IPHONE_IP) 'rm -fr /Applications/$(INSTALLFOLDER)'"<<endl;
  makefile << "\tscp -r $(APPFOLDER) root@$(IPHONE_IP):/Applications/$(INSTALLFOLDER)"<<endl;
  makefile << "\techo \"Application $(INSTALLFOLDER) installed\""<<endl;
  makefile << "\tssh mobile@$(IPHONE_IP) 'uicache'"<<endl;
  makefile << "endif" <<endl <<endl;

  makefile << "uninstall:"<<endl;
  makefile << "ifeq ($(IPHONE_IP),)"<<endl;
  makefile << "\techo \"Please set IPHONE_IP\""<<endl;
  makefile << "else"<<endl;
  makefile << "\tssh root@$(IPHONE_IP) 'rm -fr /Applications/$(INSTALLFOLDER)'"<<endl;
  makefile << "\techo \"Application $(INSTALLFOLDER) uninstalled\"" <<endl;
  makefile << "endif" <<endl <<endl;

  makefile << "clean:"<<endl;
  makefile << "\tfind . -name \\*.o|xargs rm -rf"<<endl;
  makefile << "\trm -rf $(APPFOLDER)"<<endl;
  makefile << "\trm -f $(PROJECTNAME)"<<endl<<endl;
  
  makefile << ".PHONY: all dist install uninstall clean"<<endl;
 
  makefile.close();  
  cout <<"Makefile generated."<<endl; 
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

  vector<string> headerpaths;
  for(int i = 0; i < target.headerpaths.size(); i++) {
    headerpaths.push_back(target.headerpaths[i].path);
  }
  sort(headerpaths.begin(), headerpaths.end());
  headerpaths.erase(unique(headerpaths.begin(), headerpaths.end()), headerpaths.end());


  for(int i = 0; i < headerpaths.size(); i++) {
    buildargs = buildargs + " -I" + headerpaths[i];
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
  runCommand(prepare_command.c_str());

  cout <<compile_command <<endl;
  runCommand(compile_command.c_str());

  cout <<resources_command<<endl; 
  runCommand(resources_command.c_str());

  cout <<plist_command<<endl;
  runCommand(plist_command.c_str());

  cout <<fix_infoplist_command<<endl;
  runCommand(fix_infoplist_command.c_str());

  cout <<pngcrush_command<<endl;
  runCommand(pngcrush_command.c_str());

  cout <<compile_localization_command<<endl;
  runCommand(compile_localization_command.c_str());

  cout <<compile_plist_command<<endl;
  runCommand(compile_plist_command.c_str());

  
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
  int willcompile = 0;
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
    cout <<"There are more than one targets in project files"<<endl;
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

