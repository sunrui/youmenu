//
//  wxMainFrame.h
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-8.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef __wx_youmenu__wxMainFrame__
#define __wx_youmenu__wxMainFrame__

#include <wx/wx.h>

class wxMainFrame : public wxFrame
{
public:
	wxMainFrame(wxWindow *parent,
				const wxString& title,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize);
	
private:
	DECLARE_EVENT_TABLE()
};

#endif /* defined(__wx_youmenu__wxMainFrame__) */
