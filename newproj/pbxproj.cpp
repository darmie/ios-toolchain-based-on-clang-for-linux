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
  
  const PBXValueRef * buildConfigurationList;
  const PBXValueRef * productReference;
  const PBXArray * buildPhases;
  const PBXValueRef * mainGroup;

};

class PBXProj {
public:
  PBXProj();
  void loadProj(string path);
  int getTargetCount();
  vector<PBXNativeTarget> getTargets();
  void getAllFilesFromMainGroup(const PBXBlock * block, string current_path);
  string getBuildSettings(const PBXBlock *block);
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
    
    const PBXText *productName = dynamic_cast<const PBXText*> (target_blk->valueForKey("productName"));
    if(productName)
      target_detail.productName = productName->text();

    const PBXText *productType = dynamic_cast<const PBXText*> (target_blk->valueForKey("productType"));
    if(productType)
      target_detail.productType = productType->text();
      
    const PBXValueRef *bcl_ref = dynamic_cast<const PBXValueRef*>(target_blk->valueForKey("buildConfigurationList"));
    const PBXValue * bcl_value = dynamic_cast<const PBXValue *>(pDoc->deref(bcl_ref));
    const PBXBlock *bcl_blk = dynamic_cast<const PBXBlock*>(bcl_value);
    target_detail.buildargs = getBuildSettings(bcl_blk);
    
    
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
	    
	    if(isa == "PBXFileReference") {
	      File file;
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
		string full_path = this->allfiles.find(local_path)->second;
		file.path = full_path;
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


static void printHelp(int argc, const char* argv[]);


void buildApp(PBXNativeTarget target)
{
  string compiler = "ios-clang";
  string buildargs = target.buildargs;
  string output = target.result;
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

  string final_command = compiler + buildargs + sources + " -o " + output;
  cout << final_command <<endl;
}

int main(int argc, const char* argv[])
{
  if (argc < 2) {
    printHelp(argc, argv);
    return -1;
  }
  PBXProj *pbx = new PBXProj();
  pbx->loadProj(argv[1]);
  int target_count = pbx->getTargetCount();
  vector<PBXNativeTarget> targets = pbx->getTargets();
  buildApp(targets[0]);
  if(target_count >0) {
    cout <<"There is more than one targets in project files"<<endl;
    for(int i = 0; i < target_count; i++) {
      printf("%d, %s, %s, %s\n", i, targets[i].name.c_str(), targets[i].result.c_str(), targets[i].resulttype.c_str());
    }
    cout <<"Please choose one:"<<endl;
  }
    
  return 0;
}


void printHelp(int argc, const char* argv[])
{
  printf("Usage: %s <project.pbxproj>\n", argv[0]);
}

