//
//  wxTestPanel.h
//  youmenu
//
//  Created by 孙瑞 on 14-4-27.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef __youmenu__wxwxTestPage__
#define __youmenu__wxwxTestPage__

#include <wx/artprov.h>
#include <wx/xrc/xmlres.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/statline.h>
#include <wx/statbox.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/checkbox.h>
#include <wx/frame.h>
#include <wx/notebook.h>

#include "client.h"
#include "server.h"

class wxTestPage : public wxNotebookPage, public wxTreeItemData
{
public:
	wxTestPage(wxWindow *parent,
				const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize);
	~wxTestPage();
	
	enum LogType
	{
		LogServer,
		LogClient,
		LogCook
	};
	
	void appendLog(LogType type, wxString log);
	
protected:
	void onCheckServer(wxCommandEvent& event);
	void onCheckClient(wxCommandEvent& event);
	void onCheckCook(wxCommandEvent& event);
	
	void onButtonLogin(wxCommandEvent& event);
	void onButtonCreate(wxCommandEvent& event);
	void onButtonModify(wxCommandEvent& event);
	void onButtonLogout(wxCommandEvent& event);
	void onButtonList(wxCommandEvent& event);
	void onButtonDelete(wxCommandEvent& event);
	
	void onMenuBook(wxCommandEvent& event);
	
protected:
	wxStaticText* m_staticText1;
	wxTextCtrl* m_text_userName;
	wxStaticText* m_staticText2;
	wxTextCtrl* m_text_userPwd;
	wxStaticText* m_staticText21;
	wxComboBox* m_combo_userCategory;
	wxStaticText* m_staticText8;
	wxStaticText* m_staticText81;
	wxTextCtrl* m_text_userNick;
	wxTextCtrl* m_text_userComment;
	wxButton* m_button_Login;
	wxButton* m_button_create;
	wxButton* m_button_modify;
	wxButton* m_button_exit;
	wxStaticLine* m_staticline1;
	wxComboBox* m_combo_userList;
	wxButton* m_button_fetchUser;
	wxButton* m_button_userDelete;
	wxListCtrl* m_list_menu;
	wxStaticText* m_staticText17;
	wxTextCtrl* m_text_bookNumbers;
	wxStaticText* m_staticText18;
	wxTextCtrl* m_text_bookComment;
	wxButton* m_button_menuBook;
	wxListCtrl* m_list_menuBook;
	wxTextCtrl* m_text_changeNumbers;
	wxButton* m_button_changeNumbers;
	wxStaticLine* m_staticline4;
	wxButton* m_button_cancel;
	wxButton* m_button_notifyCook;
	wxButton* m_button_checkOut;
	wxListCtrl* m_list_cook;
	wxButton* m_button_cookAccept;
	wxButton* m_button_cookFinish;
	wxButton* m_button_cookFailed;
	wxListCtrl* m_list_msgLog;
	wxCheckBox* m_check_server;
	wxCheckBox* m_check_menu;
	wxCheckBox* m_check_cook;
	wxCheckBox* m_check_cashier;

private:
	DECLARE_EVENT_TABLE()

	friend void client_disconnect_cb(void * opaque);
	connect_t * m_connect;
};

#endif /* defined(__youmenu__wxTestPanel__) */
