import sublime, sublime_plugin  
from os.path import expanduser
from threading import Thread

class EventDump(sublime_plugin.EventListener): 
    def thread_write(self):
        viewid = -1
        (x_viewport, y_viewport) = self.view.viewport_position()
        (x_caret, y_caret) = self.view.text_to_layout(self.view.sel()[0].begin())
        (width, height) = self.view.viewport_extent()
        line_height = self.view.line_height();
        if width == 428 and height == line_height:
          viewid = 1
        elif height == line_height :
          viewid = 2
        else :
          viewid = 3
        #print "[pos]\nx=" + str(x_caret-x_viewport)+"\ny=" + str(y_caret-y_viewport+line_height)
        temp_file = expanduser("~") + "/.sublime_im_position";
        posfile = open(temp_file,"w")
        posfile.write("[pos]\nx=" + str(x_caret - x_viewport)+"\ny=" + str(y_caret - y_viewport)+"\nid="+ str(viewid))
        posfile.close()
        
    def on_modified(self, view):
        self.view = view
        thread_id = Thread(target=self.thread_write,args=())
        thread_id.start() 
