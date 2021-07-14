//
//  wxMainFrame.cpp
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-8.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "wxMainFrame.h"
#include "wxTestPage.h"

enum wxMainFrame_ID
{
};

wxBEGIN_EVENT_TABLE(wxMainFrame, wxFrame)
wxEND_EVENT_TABLE()

wxMainFrame::wxMainFrame(wxWindow *parent,
						 const wxString& title,
						 const wxPoint& pos,
						 const wxSize& size)
: wxFrame(parent, wxID_ANY, title, pos, size)
{
	SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxNotebook * noteBook = new wxNotebook(this, -1, wxDefaultPosition, wxDefaultSize, wxNB_FLAT);
	wxNotebookPage * page1 = new wxTestPage(noteBook);
	wxNotebookPage * page2 = new wxNotebookPage(noteBook, -1);
	wxNotebookPage * page3 = new wxNotebookPage(noteBook, -1);
	wxNotebookPage * page4 = new wxNotebookPage(noteBook, -1);
	
	noteBook->AddPage(page1, wxT("整体测试"));
	noteBook->AddPage(page2, wxT("管理页面"));
	noteBook->AddPage(page3, wxT("测试用例"));
	noteBook->AddPage(page4, wxT("其它"));
	noteBook->Layout();
	
	Center();
}