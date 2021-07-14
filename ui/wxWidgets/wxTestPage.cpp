//
//  wxTestPanel.cpp
//  youmenu
//
//  Created by 孙瑞 on 14-4-27.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "wxTestPage.h"

#include <sys/time.h>

enum wxID
{
	wxID_USER_NAME=1000,
	wxID_USER_PWD,
	wxID_USER_TYPE,
	wxID_USER_NICK,
	wxID_UESR_COMMENT,
	wxID_BUTTON_LOGIN,
	wxID_BUTTON_CREATE,
	wxID_BUTTON_MODIFY,
	wxID_BUTTON_LOGOUT,
	wxID_USER_LIST,
	wxID_BUTTON_LIST,
	wxID_BUTTON_DELETE,
	wxID_MENU_LIST_MENU,
	wxID_MENU_BOOK_NUMBERS,
	wxID_MENU_BOOK_COMMENT,
	wxID_MENU_BOOK,
	wxID_MENU_LIST_BOOK,
	wxID_MENU_EDIT_NUMBERS,
	wxID_MENU_BUTTON_CHANGE_NUMBERS,
	wxID_MENU_BUTTON_CANCEL,
	wxID_MENU_BUTTON_NOTIFY,
	wxID_MENU_BUTTON_CHECKOUT,
	wxID_COOK_LIST,
	wxID_COOK_BUTTON_ACCEPT,
	wxID_COOK_BUTTON_FINISH,
	wxID_COOK_BUTTON_FAILED,
	wxID_MSG_LOG,
	wxID_CHECK_SERVER,
	wxID_CHECK_MENU,
	wxID_CHECK_COOK,
	wxID_CHECK_CASHIER
};

wxString LogTypeToString(wxTestPage::LogType type)
{
	switch (type)
	{
		case wxTestPage::LogType::LogServer:
			return "Server";
		case wxTestPage::LogType::LogClient:
			return "Client";
		case wxTestPage::LogType::LogCook:
			return "Cook";
		default:
			return "Unkown";
	}
}

wxString GetCurrentTime()
{
	struct timeval tv;
	struct tm * tm;
	wxString time;
	
	gettimeofday(&tv, NULL);
	tm = localtime(&tv.tv_sec);
	
	time = wxString::Format("%02d:%02d:%02d.%03d  ",
							tm->tm_hour, tm->tm_min, tm->tm_sec, tv.tv_usec / 1000);
	
	return time;
}

BEGIN_EVENT_TABLE(wxTestPage, wxNotebookPage)
EVT_CHECKBOX(wxID_CHECK_SERVER, wxTestPage::onCheckServer)
EVT_CHECKBOX(wxID_CHECK_MENU, wxTestPage::onCheckClient)
EVT_CHECKBOX(wxID_CHECK_COOK, wxTestPage::onCheckCook)
EVT_BUTTON(wxID_BUTTON_LOGIN, wxTestPage::onButtonLogin)
EVT_BUTTON(wxID_BUTTON_CREATE, wxTestPage::onButtonCreate)
EVT_BUTTON(wxID_BUTTON_MODIFY, wxTestPage::onButtonModify)
EVT_BUTTON(wxID_BUTTON_LOGOUT, wxTestPage::onButtonLogout)
EVT_BUTTON(wxID_BUTTON_LIST, wxTestPage::onButtonList)
EVT_BUTTON(wxID_BUTTON_DELETE, wxTestPage::onButtonDelete)
EVT_BUTTON(wxID_MENU_BOOK, wxTestPage::onMenuBook)
END_EVENT_TABLE()

wxTestPage::wxTestPage(wxWindow *parent, const wxPoint& pos, const wxSize& size)
: wxNotebookPage(parent, wxID_ANY)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	{
		wxStaticBoxSizer* staticBoxSizer1;
		staticBoxSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("测试账户部分") ), wxVERTICAL );
		
		wxBoxSizer* bSizer6;
		bSizer6 = new wxBoxSizer( wxVERTICAL );
		
		wxBoxSizer* bSizer7;
		bSizer7 = new wxBoxSizer( wxHORIZONTAL );
		
		m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("用户名"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText1->Wrap( -1 );
		bSizer7->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_text_userName = new wxTextCtrl( this, wxID_USER_NAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer7->Add( m_text_userName, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		m_text_userName->SetLabel("youmenu");
		
		m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("密码"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText2->Wrap( -1 );
		bSizer7->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_text_userPwd = new wxTextCtrl( this, wxID_USER_PWD, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer7->Add( m_text_userPwd, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		m_text_userPwd->SetLabel("youmenu");
		
		wxString choices[] = { "点餐", "厨师", "管理员" };
		m_combo_userCategory = new wxComboBox( this, wxID_USER_TYPE, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY);
		for (int i = 0; i < sizeof(choices) / sizeof(choices[0]); i++)
			m_combo_userCategory->Insert(choices[i], i);
		m_combo_userCategory->SetSelection(0);
		bSizer7->Add( m_combo_userCategory, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_staticText8 = new wxStaticText( this, wxID_ANY, wxT("昵称"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText8->Wrap( -1 );
		bSizer7->Add( m_staticText8, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_text_userNick = new wxTextCtrl( this, wxID_USER_NICK, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer7->Add( m_text_userNick, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		m_text_userNick->SetLabel("nick");
		
		m_staticText81 = new wxStaticText( this, wxID_ANY, wxT("备注"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText81->Wrap( -1 );
		bSizer7->Add( m_staticText81, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_text_userComment = new wxTextCtrl( this, wxID_UESR_COMMENT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer7->Add( m_text_userComment, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		m_text_userComment->SetLabel("comment");
		
		bSizer6->Add( bSizer7, 0, wxEXPAND, 5 );
		
		wxBoxSizer* bSizer8;
		bSizer8 = new wxBoxSizer( wxHORIZONTAL );
		
		m_button_Login = new wxButton( this, wxID_BUTTON_LOGIN, wxT("登录"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer8->Add( m_button_Login, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_button_create = new wxButton( this, wxID_BUTTON_CREATE, wxT("创建"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer8->Add( m_button_create, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_button_modify = new wxButton( this, wxID_BUTTON_MODIFY, wxT("修改密码"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer8->Add( m_button_modify, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_button_exit = new wxButton( this, wxID_BUTTON_LOGOUT, wxT("退出"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer8->Add( m_button_exit, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL );
		bSizer8->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
		
		m_combo_userList = new wxComboBox( this, wxID_USER_LIST, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY );
		bSizer8->Add( m_combo_userList, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_button_fetchUser = new wxButton( this, wxID_BUTTON_LIST, wxT("获取用户"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer8->Add( m_button_fetchUser, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		
		m_button_userDelete = new wxButton( this, wxID_BUTTON_DELETE, wxT("删除"), wxDefaultPosition, wxDefaultSize, 0 );
		
		bSizer8->Add( m_button_userDelete, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
		bSizer6->Add( bSizer8, 1, wxEXPAND, 5 );
		staticBoxSizer1->Add( bSizer6, 0, wxEXPAND, 5 );
		bSizer1->Add( staticBoxSizer1, 0, wxEXPAND, 5 );
	}
	
	{
		wxStaticBoxSizer* sbSizer6;
		sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("点餐部分") ), wxVERTICAL );
		
		wxBoxSizer* bSizer2;
		bSizer2 = new wxBoxSizer( wxHORIZONTAL );
		
		wxStaticBoxSizer* sbSizer8;
		sbSizer8 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("菜单部分") ), wxVERTICAL );
		
		wxBoxSizer* bSizer3;
		bSizer3 = new wxBoxSizer( wxVERTICAL );
		
		m_list_menu = new wxListCtrl( this, wxID_MENU_LIST_MENU, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
		m_list_menu->AppendColumn(wxT("id"), wxLIST_FORMAT_LEFT, 50);
		m_list_menu->AppendColumn(wxT("category"), wxLIST_FORMAT_LEFT, 70);
		m_list_menu->AppendColumn(wxT("name"), wxLIST_FORMAT_LEFT, 90);
		m_list_menu->AppendColumn(wxT("summary"), wxLIST_FORMAT_LEFT, 70);
		m_list_menu->AppendColumn(wxT("thumbnail"), wxLIST_FORMAT_LEFT, 70);
		m_list_menu->AppendColumn(wxT("price"), wxLIST_FORMAT_LEFT, 60);
		bSizer3->Add( m_list_menu, 1, wxALL|wxEXPAND, 5 );
		
		wxBoxSizer* bSizer4;
		bSizer4 = new wxBoxSizer( wxHORIZONTAL );
		
		m_staticText17 = new wxStaticText( this, wxID_ANY, wxT("份数"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText17->Wrap( -1 );
		bSizer4->Add( m_staticText17, 0, wxALL, 5 );
		
		m_text_bookNumbers = new wxTextCtrl( this, wxID_MENU_BOOK_NUMBERS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer4->Add( m_text_bookNumbers, 0, wxALL, 5 );
		m_text_bookNumbers->SetLabel("1");
		
		m_staticText18 = new wxStaticText( this, wxID_ANY, wxT("备注"), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText18->Wrap( -1 );
		bSizer4->Add( m_staticText18, 0, wxALL, 5 );
		
		m_text_bookComment = new wxTextCtrl( this, wxID_MENU_BOOK_COMMENT, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer4->Add( m_text_bookComment, 0, wxALL, 5 );
		
		m_button_menuBook = new wxButton( this, wxID_MENU_BOOK, wxT("点餐"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer4->Add( m_button_menuBook, 0, wxALL, 5 );
		
		bSizer3->Add( bSizer4, 0, wxEXPAND, 5 );
		sbSizer8->Add( bSizer3, 1, wxEXPAND, 5 );
		bSizer2->Add( sbSizer8, 1, wxEXPAND, 5 );
		
		wxStaticBoxSizer* sbSizer9;
		sbSizer9 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("订单部分") ), wxVERTICAL );
		
		wxBoxSizer* bSizer17;
		bSizer17 = new wxBoxSizer( wxHORIZONTAL );
		
		m_list_menuBook = new wxListCtrl( this, wxID_MENU_LIST_BOOK, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
		m_list_menuBook->AppendColumn(wxT("dish_id"), wxLIST_FORMAT_LEFT, 70);
		m_list_menuBook->AppendColumn(wxT("份数"), wxLIST_FORMAT_LEFT, 70);
		m_list_menuBook->AppendColumn(wxT("状态"), wxLIST_FORMAT_LEFT, 70);
		m_list_menuBook->AppendColumn(wxT("备注"), wxLIST_FORMAT_LEFT, 70);
		bSizer17->Add( m_list_menuBook, 1, wxALL|wxEXPAND, 5 );
		
		wxBoxSizer* bSizer18;
		bSizer18 = new wxBoxSizer( wxVERTICAL );
		
		m_text_changeNumbers = new wxTextCtrl( this, wxID_MENU_EDIT_NUMBERS, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
		bSizer18->Add( m_text_changeNumbers, 0, wxALL|wxEXPAND, 5 );
		
		m_button_changeNumbers = new wxButton( this, wxID_MENU_BUTTON_CHANGE_NUMBERS, wxT("修改份数"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer18->Add( m_button_changeNumbers, 0, wxALL|wxEXPAND, 5 );
		
		m_staticline4 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
		bSizer18->Add( m_staticline4, 0, wxEXPAND | wxALL, 5 );
		
		m_button_cancel = new wxButton( this, wxID_MENU_BUTTON_CANCEL, wxT("退订"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer18->Add( m_button_cancel, 0, wxALL|wxEXPAND, 5 );
		
		m_button_notifyCook = new wxButton( this, wxID_MENU_BUTTON_NOTIFY, wxT("催厨师"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer18->Add( m_button_notifyCook, 0, wxALL|wxEXPAND, 5 );
		
		m_button_checkOut = new wxButton( this, wxID_MENU_BUTTON_CHECKOUT, wxT("结算"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer18->Add( m_button_checkOut, 0, wxALL|wxEXPAND, 5 );
		
		bSizer17->Add( bSizer18, 0, wxEXPAND, 5 );
		sbSizer9->Add( bSizer17, 1, wxEXPAND, 5 );
		bSizer2->Add( sbSizer9, 1, wxEXPAND, 5 );
		sbSizer6->Add( bSizer2, 1, wxEXPAND, 5 );
		bSizer1->Add( sbSizer6, 0, wxEXPAND, 5 );
	}
	
	{
		wxStaticBoxSizer* sbSizer7;
		sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("后台部分") ), wxHORIZONTAL );
		
		wxStaticBoxSizer* sbSizer11;
		sbSizer11 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("厨师部分") ), wxHORIZONTAL );
		
		m_list_cook = new wxListCtrl( this, wxID_COOK_LIST, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
		m_list_cook->AppendColumn(wxT("dish_id"), wxLIST_FORMAT_LEFT, 70);
		m_list_cook->AppendColumn(wxT("seat_id"), wxLIST_FORMAT_LEFT, 70);
		m_list_cook->AppendColumn(wxT("状态"), wxLIST_FORMAT_LEFT, 70);
		m_list_cook->AppendColumn(wxT("备注"), wxLIST_FORMAT_LEFT, 70);
		sbSizer11->Add( m_list_cook, 1, wxALL|wxEXPAND, 5 );
		
		wxBoxSizer* bSizer19;
		bSizer19 = new wxBoxSizer( wxVERTICAL );
		
		m_button_cookAccept = new wxButton( this, wxID_COOK_BUTTON_ACCEPT, wxT("已接受"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer19->Add( m_button_cookAccept, 0, wxALL|wxEXPAND, 5 );
		
		m_button_cookFinish = new wxButton( this, wxID_COOK_BUTTON_FINISH, wxT("已做好"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer19->Add( m_button_cookFinish, 0, wxALL|wxEXPAND, 5 );
		
		m_button_cookFailed = new wxButton( this, wxID_COOK_BUTTON_FAILED, wxT("无法满足"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer19->Add( m_button_cookFailed, 0, wxALL|wxEXPAND, 5 );
		
		sbSizer11->Add( bSizer19, 0, wxEXPAND, 5 );
		sbSizer7->Add( sbSizer11, 1, wxEXPAND, 5 );
		
		wxStaticBoxSizer* sbSizer12;
		sbSizer12 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("收到推送、系统消息") ), wxVERTICAL );
		
		m_list_msgLog = new wxListCtrl( this, wxID_MSG_LOG, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
		m_list_msgLog->AppendColumn(wxT("时间"), wxLIST_FORMAT_LEFT, 120);
		m_list_msgLog->AppendColumn(wxT("类别"), wxLIST_FORMAT_LEFT, 70);
		m_list_msgLog->AppendColumn(wxT("消息"), wxLIST_FORMAT_LEFT, 300);
		
		sbSizer12->Add( m_list_msgLog, 1, wxALL|wxEXPAND, 5 );
		
		sbSizer7->Add( sbSizer12, 1, wxEXPAND, 5 );
		bSizer1->Add( sbSizer7, 1, wxEXPAND, 5 );
		
		wxBoxSizer* bSizer21;
		bSizer21 = new wxBoxSizer( wxHORIZONTAL );
		
		m_check_server = new wxCheckBox( this, wxID_CHECK_SERVER, wxT("服务器"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer21->Add( m_check_server, 0, wxALL, 5 );
		
		m_check_menu = new wxCheckBox( this, wxID_CHECK_MENU, wxT("点餐"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer21->Add( m_check_menu, 0, wxALL, 5 );
		
		m_check_cook = new wxCheckBox( this, wxID_CHECK_COOK, wxT("厨师"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer21->Add( m_check_cook, 0, wxALL, 5 );
		
		m_check_cashier = new wxCheckBox( this, wxID_CHECK_CASHIER, wxT("前台"), wxDefaultPosition, wxDefaultSize, 0 );
		bSizer21->Add( m_check_cashier, 0, wxALL, 5 );
		
		bSizer1->Add( bSizer21, 0, wxEXPAND, 5 );
	}
	
	SetSizer( bSizer1 );
	Layout();
	
	Centre( wxBOTH );
	
	m_connect = NULL;
	
	int r = server_startup(4455);
	appendLog(LogServer, r== 1 ? "服务器启动成功" : "服务器启动失败");
	if (r == 1)
		m_check_server->Set3StateValue(wxCHK_CHECKED);
}

wxTestPage::~wxTestPage()
{
}

void client_disconnect_cb(void * opaque)
{
	wxTestPage * pthis = (wxTestPage *)opaque;
	
	pthis->appendLog(wxTestPage::LogClient, "用户掉线了！！");
	client_disconnect(&pthis->m_connect);
}

void appendAccountResult(wxTestPage * pThis, account_result result)
{
	switch (result)
	{
        case account_r_cannot_delete_last_admin:
            pThis->appendLog(wxTestPage::LogClient, "account_r_cannot_delete_last_admin");
            break;
		case account_r_ok:
            pThis->appendLog(wxTestPage::LogClient, "account_r_ok");
			break;
		case account_r_login_ok_conlict_another_address:
			pThis->appendLog(wxTestPage::LogClient, "account_r_login_ok_conlict_another_address");
			break;
		case account_r_login_already_login:
			pThis->appendLog(wxTestPage::LogClient, "account_r_login_already_login");
			break;
		case account_r_login_user_not_exist:
			pThis->appendLog(wxTestPage::LogClient, "account_r_login_user_not_exist");
			break;
		case account_r_login_invalid_user_or_pwd:
			pThis->appendLog(wxTestPage::LogClient, "account_r_login_invalid_user_or_pwd");
			break;
        case account_r_create_uid_exist:
            pThis->appendLog(wxTestPage::LogClient, "account_r_create_uid_exist");
            break;
        case account_r_old_pwd_incorrect:
            pThis->appendLog(wxTestPage::LogClient, "account_r_old_pwd_incorrect");
            break;
        case account_r_modify_pwd_same:
            pThis->appendLog(wxTestPage::LogClient, "account_r_modify_pwd_same");
            break;
        case account_r_no_permission:
            pThis->appendLog(wxTestPage::LogClient, "account_r_no_permission");
            break;
		case account_r_session_expired:
			pThis->appendLog(wxTestPage::LogClient, "account_r_login_session_expired");
			break;
        case account_r_illegal_parameter:
            pThis->appendLog(wxTestPage::LogClient, "account_r_illegal_parameter");
            break;
        case account_r_service_unavailable:
            pThis->appendLog(wxTestPage::LogClient, "account_r_service_unavailable");
            break;
		case account_r_no_response:
			pThis->appendLog(wxTestPage::LogClient, "account_r_no_response");
			break;
	}
}

void appendConnectResult(wxTestPage * pThis, connect_result result)
{
	switch (result)
	{
		case connect_r_ok:
			pThis->appendLog(wxTestPage::LogClient, "connect_r_ok");
			break;
		case connect_r_socket_error:
			pThis->appendLog(wxTestPage::LogClient, "connect_r_socket_error");
			break;
		case connect_r_security_handshake_error:
			pThis->appendLog(wxTestPage::LogClient, "connect_r_security_handshake_error");
			break;
		case connect_r_module_init_error:
			pThis->appendLog(wxTestPage::LogClient, "connect_r_module_init_error");
			break;
		case connect_r_no_response:
			pThis->appendLog(wxTestPage::LogClient, "connect_r_no_response");
			break;
	}
}

void appendDishResult(wxTestPage * pThis, dish_result result)
{
	switch (result)
	{
		case dish_r_ok:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_ok");
			break;
		case dish_r_modify_no_exist:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_modify_no_exist");
			break;
		case dish_r_modify_cook_accept:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_modify_cook_accept");
			break;
		case dish_r_seat_id_invalid:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_seat_id_invalid");
			break;
		case dish_r_dish_id_invalid:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_dish_id_invalid");
			break;
		case dish_r_no_permission:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_no_permission");
			break;
		case dish_r_illegal_parameter:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_illegal_parameter");
			break;
		case dish_r_service_unavailable:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_service_unavailable");
			break;
		case dish_r_no_response:
			pThis->appendLog(wxTestPage::LogClient, "dish_r_no_response");
			break;
	}
}

wxString DishCategoryToString(dish_category category)
{
	switch (category)
	{
		case dish_category_hot:
			return "hot";
		case dish_category_cold:
			return "cold";
		case dish_category_wine:
			return "wine";
		case dish_category_drink:
			return "drink";
		case dish_category_food:
			return "food";
		case dish_category_other:
			return "other";
	}
	
}

void wxTestPage::appendLog(LogType type, wxString log)
{
	long index;
	
	index = m_list_msgLog->InsertItem(m_list_msgLog->GetItemCount() + 1, GetCurrentTime());
    m_list_msgLog->SetItem( index, 1, LogTypeToString(type));
    m_list_msgLog->SetItem( index, 2, log);
	m_list_msgLog->EnsureVisible(index);
}

void wxTestPage::onCheckServer(wxCommandEvent& event)
{
	int r;
	
	if (m_check_server->IsChecked())
	{
		r = server_startup(4455);
		appendLog(LogServer, r== 1 ? "服务器启动成功" : "服务器启动失败");
		if (r == 0)
			m_check_server->Set3StateValue(wxCHK_UNCHECKED);
	}
	else
	{
		r = server_stop();
		appendLog(LogServer, r== 1 ? "服务器停止" : "服务器停止错误");
	}
}

void wxTestPage::onCheckClient(wxCommandEvent& event)
{
	if (m_check_menu->IsChecked())
	{
		wxString userName = m_text_userName->GetLineText(0);
		wxString userPwd = m_text_userPwd->GetLineText(0);
		
		if (userName.IsEmpty() || userPwd.IsEmpty())
		{
			appendLog(LogClient, "用户名密码不能为空");
			return;
		}
		
		client_session_t session;
		
		if (m_connect != NULL)
		{
			int r;
			
			r = client_account_get_session(m_connect, NULL);
			if (r == 1)
			{
				appendLog(LogClient, "用户已经登录");
				return;
			}
		}
		
		connect_result result;
		
		if (m_connect == NULL)
		{
			m_connect = client_connect("127.0.0.1", 4455, &result, client_disconnect_cb, this);
			appendConnectResult(this, result);
			if (m_connect == NULL)
			{
				appendLog(LogClient, "连接不到服务器");
				m_check_menu->Set3StateValue(wxCHK_UNCHECKED);
				return;
			}
		}
		
		client_account_login(m_connect, userName.ToStdString().c_str(), userPwd.ToStdString().c_str(), 0, &session);
		appendAccountResult(this, session.result);
		if (session.result != account_r_ok)
		{
			m_check_menu->Set3StateValue(wxCHK_UNCHECKED);
			return;
		}
		
		dish_t * dish;
		int count;
		dish_result d_result;
		
		client_dish_fetch(m_connect, dish_category_hot,	&dish, &count, &d_result);
		appendDishResult(this, d_result);
		if (d_result != dish_r_ok)
			return;
		
		m_list_menu->DeleteAllItems();
		for (int i = 0; i < count; i++)
		{
			m_list_menu->InsertItem(i, wxString::Format(wxT("%d"), dish[i].id));
			m_list_menu->SetItem(i, 1, DishCategoryToString(dish[i].category));
			m_list_menu->SetItem(i, 2, dish[i].name);
			m_list_menu->SetItem(i, 3, dish[i].summary);
			m_list_menu->SetItem(i, 4, dish[i].thumbnail);
			m_list_menu->SetItem(i, 5, wxString::Format(wxT("%d"), dish[i].price));
		}
	}
	else
	{
		account_result result;
		client_account_logout(m_connect, &result);
		appendAccountResult(this, result);
		
		m_list_menu->DeleteAllItems();
		m_list_menuBook->DeleteAllItems();
	}
}

void wxTestPage::onCheckCook(wxCommandEvent& event)
{
}

void wxTestPage::onButtonLogin(wxCommandEvent& event)
{
	wxString userName = m_text_userName->GetLineText(0);
	wxString userPwd = m_text_userPwd->GetLineText(0);
	
	if (userName.IsEmpty() || userPwd.IsEmpty())
	{
		appendLog(LogClient, "用户名密码不能为空");
		return;
	}
	
	client_session_t session;

	if (m_connect != NULL)
	{
		int r;
		
		r = client_account_get_session(m_connect, NULL);
		if (r == 1)
		{
			appendLog(LogClient, "用户已经登录");
			return;
		}
	}
	
	connect_result result;
	
	if (m_connect == NULL)
	{
		m_connect = client_connect("127.0.0.1", 4455, &result, client_disconnect_cb, this);
		appendConnectResult(this, result);
		if (m_connect == NULL)
			return;
	}
	
	client_account_login(m_connect, userName.ToStdString().c_str(), userPwd.ToStdString().c_str(), 0, &session);
	appendAccountResult(this, session.result);
	if (session.result != account_r_ok)
		return;
	
	appendLog(LogClient, wxString::Format(wxT("用户名:%s"), session.account.uid));
	appendLog(LogClient, wxString::Format(wxT("密码:%s"), session.account.pwd));
	appendLog(LogClient, wxString::Format(wxT("昵称:%s"), session.account.nick));
	appendLog(LogClient, wxString::Format(wxT("备注:%s"), session.account.comment));
}

void wxTestPage::onButtonCreate(wxCommandEvent& event)
{
	wxString userName = m_text_userName->GetLineText(0);
	wxString userPwd = m_text_userPwd->GetLineText(0);
	wxString userNick = m_text_userNick->GetLineText(0);
	wxString userComment = m_text_userComment->GetLineText(0);

	account_t a;
	
	strcpy(a.uid, userName.ToStdString().c_str());
	strcpy(a.pwd, userPwd.ToStdString().c_str());
	a.type = account_admin;
	strcpy(a.nick, userNick.ToStdString().c_str());
	strcpy(a.comment, userComment.ToStdString().c_str());
	
	account_result result;
	client_account_create(m_connect, &a, &result);
	appendAccountResult(this, result);
}

void wxTestPage::onButtonModify(wxCommandEvent& event)
{
	wxString userName = m_text_userName->GetLineText(0);
	wxString userPwd = m_text_userPwd->GetLineText(0);
	
	account_result result;
	client_account_modify_pwd(m_connect, "youmenu", "youmenu1", &result);
	appendAccountResult(this, result);
}

void wxTestPage::onButtonLogout(wxCommandEvent& event)
{
	int is_login = 0;
	
	if (m_connect != NULL)
	{
		is_login = client_account_get_session(m_connect, NULL);
	}
	
	if (is_login == 0)
	{
		appendLog(LogClient, "用户没有登录");
		return;
	}
	
	account_result result;
	client_account_logout(m_connect, &result);
	appendAccountResult(this, result);
}

void wxTestPage::onButtonList(wxCommandEvent &event)
{
	account_t * account;
	int count;
	account_result result;
	
	client_account_list(m_connect, &account, &count, &result);
	appendAccountResult(this, result);
	
	m_combo_userList->Clear();
	for (int i = 0; i < count; i++)
		m_combo_userList->Insert(account[i].uid, 0);
	client_account_list_free(account);
	
	if (m_combo_userList->GetCount() > 0)
		m_combo_userList->SetSelection(0);
}

void wxTestPage::onButtonDelete(wxCommandEvent& event)
{
	if (m_combo_userList->GetCount() <= 0)
		return;
	
	wxString uid = m_combo_userList->GetStringSelection();
	account_result result;
	
	client_account_delete(m_connect, uid.ToStdString().c_str(), "youmenu", &result);
	appendAccountResult(this, result);
	if (result == account_r_ok)
	{
		m_combo_userList->Delete(m_combo_userList->GetCurrentSelection());
		
		if (m_combo_userList->GetCount() > 0)
			m_combo_userList->SetSelection(0);
	}
}

void wxTestPage::onMenuBook(wxCommandEvent& event)
{
	long itemIndex = -1;
	int one_select = 0;
	
	if (m_connect == NULL)
	{
		appendLog(LogClient, "没有连接");
		return;
	}
	
	for (;;)
	{
		itemIndex = m_list_menu->GetNextItem(itemIndex,
											 wxLIST_NEXT_ALL,
											 wxLIST_STATE_SELECTED);
		
		if (itemIndex == -1)
		{
			if (!one_select)
			{
				appendLog(LogClient, "没有选择任何项");
			}
			
			break;
		}
		
		one_select = 1;
		wxString id = m_list_menu->GetItemText(itemIndex);
		wxString number = m_text_bookNumbers->GetLineText(0);
		wxString comment = m_text_bookComment->GetLineText(0);
		
		if (number.IsEmpty())
		{
			appendLog(LogClient, "没有选择点餐数目");
			return;
		}
		
		dish_result result;
		
		client_dish_book(m_connect, 0, atoi(id.ToStdString().c_str()), atoi(number.ToStdString().c_str()),
						 comment.ToStdString().c_str(), &result);
		appendDishResult(this, result);
		
		if (result != dish_r_ok)
		{
			return;
		}
		
		long index = m_list_menuBook->InsertItem(0, id);
		m_list_menuBook->SetItem(index, 1, number);
		m_list_menuBook->SetItem(index, 3, comment);
	}
}



























