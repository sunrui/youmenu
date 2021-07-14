//
//  wxMenuPanel.h
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-8.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef __wx_youmenu__wxMenuPanel__
#define __wx_youmenu__wxMenuPanel__

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <wx/notebook.h>

#include "client.h"

class wxPanelMenu : public wxNotebookPage, public wxTreeItemData
{
public:
	wxPanelMenu(wxFrame *parent,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize);
public:
	void appendLog(wxString log);

protected:
	void onBtnVersionCheck(wxCommandEvent& event);
	void onBtnLogin(wxCommandEvent& event);
	void onBtnLoginByToken(wxCommandEvent& event);
	void onBtnLoginCreateAccount(wxCommandEvent& event);
	void onBtnLoginDeleteAccount(wxCommandEvent& event);
	void onBtnLoginModifyPwd(wxCommandEvent& event);
	void onBtnLoginOut(wxCommandEvent& event);
	void onBtnLoginList(wxCommandEvent& event);
    void onBtnMenuFetch(wxCommandEvent& event);
    void onBtnMenuBook(wxCommandEvent& event);
    void onBtnMenuList(wxCommandEvent& event);
	
protected:
	wxTreeCtrl* m_treeMenuList;
	wxTreeCtrl* m_treeBookList;
	wxCheckListBox* m_checkList2;
	wxButton* m_button14;
	wxCheckListBox* m_checkList4;
	wxCheckListBox* m_checkList5;
	wxCheckBox* m_checkBox3;
	wxCheckBox* m_checkBox2;
	wxCheckBox* m_checkBox1;
	
private:
	DECLARE_EVENT_TABLE()
	
public:
	connect_t * m_client;
};

#endif /* defined(__wx_youmenu__wxMenuPanel__) */
