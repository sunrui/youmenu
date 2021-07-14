 //
//  wxMenuPanel.cpp
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-8.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#include "wxPanelMenu.h"
#include "wxMainFrame.h"

#include "wx/wx.h"
#include "wx/dataview.h"
#include "wx/treectrl.h"
#include "wx/notebook.h"

enum wxMenuID
{
	wxID_btnVersionCheck,
	wxID_btnLoginFailed,
	wxID_btnLogin,
	wxID_btnLoginByToken,
	wxID_btnLoginCreateAccount,
    wxID_btnLoginDeleteAccount,
	wxID_btnLoginModifyPwd,
	wxID_btnLoginOut,
	wxID_btnLoginList,
    wxID_btnMenuFetch,
    wxID_btnMenuBook,
    wxID_btnMenuList,
};

BEGIN_EVENT_TABLE(wxPanelMenu, wxPanel)
EVT_BUTTON(wxID_btnVersionCheck, wxPanelMenu::onBtnVersionCheck)
EVT_BUTTON(wxID_btnLogin, wxPanelMenu::onBtnLogin)
EVT_BUTTON(wxID_btnLoginByToken, wxPanelMenu::onBtnLoginByToken)
EVT_BUTTON(wxID_btnLoginCreateAccount, wxPanelMenu::onBtnLoginCreateAccount)
EVT_BUTTON(wxID_btnLoginDeleteAccount, wxPanelMenu::onBtnLoginDeleteAccount)
EVT_BUTTON(wxID_btnLoginOut, wxPanelMenu::onBtnLoginOut)
EVT_BUTTON(wxID_btnLoginList, wxPanelMenu::onBtnLoginList)
EVT_BUTTON(wxID_btnMenuFetch, wxPanelMenu::onBtnMenuFetch)
EVT_BUTTON(wxID_btnMenuBook, wxPanelMenu::onBtnMenuBook)
EVT_BUTTON(wxID_btnMenuList, wxPanelMenu::onBtnMenuList)
END_EVENT_TABLE()

wxPanelMenu::wxPanelMenu(wxFrame *parent,
						 const wxPoint& pos,
						 const wxSize& size)
: wxNotebookPage(parent, wxID_ANY)
{
//	this->SetBackgroundColour(wxColour("white"));
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* menuSizer;
	menuSizer = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer_diancan;
	bSizer_diancan = new wxBoxSizer( wxHORIZONTAL );
	
	// 登录测试部分
	{
		wxStaticBoxSizer * loginSizer;
		loginSizer = new wxStaticBoxSizer( wxHORIZONTAL, this, wxT("登录测试") );
		
		wxButton * btnVersionCheck = new wxButton( this, wxID_btnVersionCheck, wxT("更新"), wxDefaultPosition, wxDefaultSize, 0 );
		wxButton * btnLogin = new wxButton( this, wxID_btnLogin, wxT("登录"), wxDefaultPosition, wxDefaultSize, 0 );
		wxButton * btnLoginByToken = new wxButton( this, wxID_btnLoginByToken, wxT("记忆登录"), wxDefaultPosition, wxDefaultSize, 0 );
		wxButton * btnLoginCreate = new wxButton( this, wxID_btnLoginCreateAccount, wxT("创建"), wxDefaultPosition, wxDefaultSize, 0 );
		wxButton * btnLoginDelete = new wxButton( this, wxID_btnLoginDeleteAccount, wxT("删除"), wxDefaultPosition, wxDefaultSize, 0 );
		wxButton * btnLoginOut = new wxButton( this, wxID_btnLoginOut, wxT("退出"), wxDefaultPosition, wxDefaultSize, 0 );
		wxButton * btnLoginList = new wxButton( this, wxID_btnLoginList, wxT("列表"), wxDefaultPosition, wxDefaultSize, 0 );
		
		loginSizer->Add(btnVersionCheck, 0, wxALL|wxEXPAND, 0 );
		loginSizer->Add(btnLogin, 0, wxALL|wxEXPAND, 0 );
		loginSizer->Add(btnLoginByToken, 0, wxALL|wxEXPAND, 0 );
		loginSizer->Add(btnLoginCreate, 0, wxALL|wxEXPAND, 0 );
		loginSizer->Add(btnLoginDelete, 0, wxALL|wxEXPAND, 0 );
		loginSizer->Add(btnLoginOut, 0, wxALL|wxEXPAND, 0 );
		loginSizer->Add(btnLoginList, 0, wxALL|wxEXPAND, 0 );
		
		menuSizer->Add(loginSizer);
	}
	
    // 下拉的菜单
    {
        wxStaticBoxSizer * serverMenuSizer;
        serverMenuSizer = new wxStaticBoxSizer( wxHORIZONTAL, this, wxT("服务器菜单") );
        
        wxBoxSizer* bSizer5;
        bSizer5 = new wxBoxSizer( wxVERTICAL );
        
        m_treeMenuList = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT );
        bSizer5->Add( m_treeMenuList, 1, wxALL|wxFIXED_MINSIZE|wxEXPAND, 5 );

        wxBoxSizer* bSizer8;
        bSizer8 = new wxBoxSizer( wxVERTICAL );
        
        wxBoxSizer* serverMenuBtnSizer;
        serverMenuBtnSizer = new wxBoxSizer( wxHORIZONTAL );
        
        wxButton* m_btnMenuFetch;
        m_btnMenuFetch = new wxButton( this, wxID_btnMenuFetch, wxT("获取"), wxDefaultPosition, wxDefaultSize, 0 );
        serverMenuBtnSizer->Add( m_btnMenuFetch, 0, wxALL, 5 );
        
        wxButton* m_btnMenuBook;
        m_btnMenuBook = new wxButton( this, wxID_btnMenuBook, wxT("点餐"), wxDefaultPosition, wxDefaultSize, 0 );
        serverMenuBtnSizer->Add( m_btnMenuBook, 0, wxALL, 5 );
        
        wxButton* m_btnMenuList;
        m_btnMenuList = new wxButton( this, wxID_btnMenuList, wxT("已点餐"), wxDefaultPosition, wxDefaultSize, 0 );
        serverMenuBtnSizer->Add( m_btnMenuList, 0, wxALL, 5 );

        bSizer8->Add( serverMenuBtnSizer, 0, wxALL|wxEXPAND, 5 );
        bSizer5->Add( bSizer8, 0, wxEXPAND, 5 );
        serverMenuSizer->Add( bSizer5, 1, wxEXPAND, 5 );
        bSizer_diancan->Add( serverMenuSizer, 1, wxEXPAND, 5 );
    }
    
    {
        wxStaticBoxSizer* sbSizer3;
        sbSizer3 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("已点订单") ), wxVERTICAL );
        
        m_treeBookList = new wxTreeCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxTR_HIDE_ROOT );
        sbSizer3->Add( m_treeBookList, 1, wxALL|wxEXPAND, 5 );
        
        wxStaticBoxSizer* sbSizer6;
        sbSizer6 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("测试案例") ), wxHORIZONTAL );
        
        wxButton* m_button9;
        wxButton* m_button11;
        wxButton* m_button8;
        m_button9 = new wxButton( this, wxID_ANY, wxT("修改份数成功"), wxDefaultPosition, wxDefaultSize, 0 );
        sbSizer6->Add( m_button9, 0, wxALL, 5 );
        
        m_button11 = new wxButton( this, wxID_ANY, wxT("退订在做"), wxDefaultPosition, wxDefaultSize, 0 );
        sbSizer6->Add( m_button11, 0, wxALL, 5 );
        
        m_button8 = new wxButton( this, wxID_ANY, wxT("退订成功"), wxDefaultPosition, wxDefaultSize, 0 );
        sbSizer6->Add( m_button8, 0, wxALL, 5 );
        
        sbSizer3->Add( sbSizer6, 0, wxEXPAND, 5 );
        bSizer_diancan->Add( sbSizer3, 1, wxEXPAND, 5 );
        menuSizer->Add( bSizer_diancan, 1, wxEXPAND, 5 );
    }
	
	wxStaticBoxSizer* sbSizer_chushi;
	sbSizer_chushi = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("厨师列表") ), wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer7;
	sbSizer7 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("当前的厨师及在线情况") ), wxHORIZONTAL );
	
	wxBoxSizer* bSizer18;
	bSizer18 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer21;
	bSizer21 = new wxBoxSizer( wxVERTICAL );
	
	wxArrayString m_checkList2Choices;
	m_checkList2 = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList2Choices, 0 );
	bSizer21->Add( m_checkList2, 1, wxALL|wxEXPAND, 5 );
	
	m_button14 = new wxButton( this, wxID_ANY, wxT("邀请对话查询"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer21->Add( m_button14, 0, wxALL|wxEXPAND, 5 );
	
	bSizer18->Add( bSizer21, 1, wxEXPAND, 5 );
	
	
	sbSizer7->Add( bSizer18, 1, wxEXPAND, 5 );
	
	
	sbSizer_chushi->Add( sbSizer7, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer16;
	bSizer16 = new wxBoxSizer( wxHORIZONTAL );
	
	wxStaticBoxSizer* sbSizer5;
	sbSizer5 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, wxT("收到厨师反馈") ), wxHORIZONTAL );
	
	wxArrayString m_checkList5Choices;
	m_checkList5 = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList5Choices, 0 );
	sbSizer5->Add( m_checkList5, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer20;
	bSizer20 = new wxBoxSizer( wxVERTICAL );
	
	m_checkBox3 = new wxCheckBox( this, wxID_ANY, wxT("某菜已经做好准备上菜"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_checkBox3, 0, wxALL, 5 );
	
	m_checkBox2 = new wxCheckBox( this, wxID_ANY, wxT("某菜缺失需要取消订单"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_checkBox2, 0, wxALL, 5 );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, wxT("菜已经备齐"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer20->Add( m_checkBox1, 0, wxALL, 5 );
	
	
	sbSizer5->Add( bSizer20, 0, wxEXPAND, 5 );
	
	
	bSizer16->Add( sbSizer5, 1, wxEXPAND, 5 );
	
	
	sbSizer_chushi->Add( bSizer16, 1, wxEXPAND, 5 );
	
	
	menuSizer->Add( sbSizer_chushi, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer17;
	bSizer17 = new wxBoxSizer( wxVERTICAL );
	
	
	menuSizer->Add( bSizer17, 1, wxEXPAND, 5 );
	
	this->SetSizer( menuSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
	
	m_client = NULL;
}

void wxPanelMenu::appendLog(wxString log)
{
	wxMainFrame * frame = (wxMainFrame *)GetParent();
	frame->appendLog(log);
}

void appendAccountResult(wxPanelMenu * menu, account_result result)
{
	switch (result)
	{
        case account_r_delete_last_admin:
            menu->appendLog("account_r_delete_last_admin");
            break;
		case account_r_ok:
            menu->appendLog("account_r_ok");
			break;
		case account_r_login_ok_conlict_another_address:
			menu->appendLog("account_r_login_ok_conlict_another_address");
			break;
		case account_r_login_already_login:
			menu->appendLog("account_r_login_already_login");
			break;
		case account_r_login_user_not_exist:
			menu->appendLog("account_r_login_user_not_exist");
			break;
		case account_r_login_invalid_user_or_pwd:
			menu->appendLog("account_r_login_invalid_user_or_pwd");
			break;
        case account_r_create_uid_exist:
            menu->appendLog("account_r_create_uid_exist");
            break;
        case account_r_old_pwd_incorrect:
            menu->appendLog("account_r_old_pwd_incorrect");
            break;
        case account_r_modify_pwd_same:
            menu->appendLog("account_r_modify_pwd_same");
            break;
        case account_r_no_permission:
            menu->appendLog("account_r_no_permission");
            break;
		case account_r_session_expired:
			menu->appendLog("account_r_login_session_expired");
			break;
        case account_r_illegal_parameter:
            menu->appendLog("account_r_illegal_parameter");
            break;
        case account_r_service_unavailable:
            menu->appendLog("account_r_service_unavailable");
            break;
		case account_r_no_response:
			menu->appendLog("account_r_no_response");
			break;
	}
}

void appendDishResult(wxPanelMenu * menu, dish_result result)
{
    switch (result)
    {
        case dish_r_no_permission:
        {
            menu->appendLog("dish_r_no_permission");
            break;
        }
        case dish_r_modify_cook_accept:
        {
            menu->appendLog("dish_r_canel_cook_accept");
            break;
        }
        case dish_r_modify_no_exist:
        {
            menu->appendLog("dish_r_canel_no_exist");
            break;
        }
        case dish_r_seat_id_invalid:
        {
            menu->appendLog("dish_r_seat_id_invalid");
            break;
        }
        case dish_r_dish_id_invalid:
        {
            menu->appendLog("dish_r_book_dish_id_invalid");
            break;
        }
        case dish_r_book_ignore_conflict:
        {
            menu->appendLog("dish_r_book_ignore_conflict");
            break;
        }
        case dish_r_ok:
        {
            menu->appendLog("dish_r_ok");
            break;
        }
        case dish_r_illegal_parameter:
        {
            menu->appendLog("dish_r_illegal_parameter");
            break;
        }
        case dish_r_service_unavailable:
        {
            menu->appendLog("dish_r_service_unavailable");
            break;
        }
        case dish_r_no_response:
        {
            menu->appendLog("dish_r_no_response");
            break;
        }
    }
}

void appendSeatResult(wxPanelMenu * menu, seat_result result)
{
	switch (result)
	{
		case seat_r_ok:
			menu->appendLog("seat_r_ok");
			break;
		case seat_r_no_permission:
			menu->appendLog("seat_r_no_permission");
			break;
		case seat_r_illegal_parameter:
			menu->appendLog("seat_r_illegal_parameter");
			break;
		case seat_r_service_unavailable:
			menu->appendLog("seat_r_service_unavailable");
			break;
		case seat_r_no_response:
			menu->appendLog("seat_r_no_response");
			break;
	}
}

void wxPanelMenu::onBtnMenuFetch(wxCommandEvent& event)
{
    dish_t ** dish;
    dish_result result;
    int count;
    
    client_dish_fetch(m_client, dish_category_hot, &dish, &count, &result);
    appendDishResult(this, result);
    if (result == dish_r_ok)
    {
        appendLog("fetchDish 拉取成功");
        
        wxTreeItemId itemRoot;
        
        if (m_treeMenuList->GetRootItem() != NULL)
        {
            itemRoot = m_treeMenuList->GetRootItem();
            m_treeMenuList->Delete(itemRoot);
        }

        itemRoot = m_treeMenuList->AddRoot(wxString(), 0);
    
        for (int i = 0; i < count; i++)
        {
            wxTreeItemId dishOne;
            
            dishOne = m_treeMenuList->AppendItem(itemRoot, dish[i]->name, 0);
            m_treeMenuList->InsertItem(dishOne, 0, wxString::Format(wxT("id: %d"), dish[i]->id), 0);
            m_treeMenuList->InsertItem(dishOne, 1, wxString::Format(wxT("category: %d"), dish[i]->category), 0);
            m_treeMenuList->InsertItem(dishOne, 2, wxString::Format(wxT("summary: %s"), dish[i]->summary), 0);
            m_treeMenuList->InsertItem(dishOne, 3, wxString::Format(wxT("thumbnail: %s"), dish[i]->thumbnail), 0);
            m_treeMenuList->InsertItem(dishOne, 4, wxString::Format(wxT("price: %d"), dish[i]->price), 0);
            
            m_treeMenuList->Expand(dishOne);
        }
    }
}

void wxPanelMenu::onBtnMenuBook(wxCommandEvent& event)
{
    dish_result result;
    
    client_dish_book(m_client, 0, 0, 1, &result);
    appendDishResult(this, result);
}

void wxPanelMenu::onBtnMenuList(wxCommandEvent& event)
{
    dish_result result;
    book_t * book;
    int count;
    
    client_dish_list(m_client, 0, &book, &count, &result);
    appendDishResult(this, result);
    if (result == dish_r_ok)
    {
        appendLog("MenuList 拉取成功");
        
        wxTreeItemId itemRoot;
        
        if (m_treeBookList->GetRootItem() != NULL)
        {
            itemRoot = m_treeBookList->GetRootItem();
            m_treeBookList->Delete(itemRoot);
        }

        itemRoot = m_treeBookList->AddRoot(wxString(), 0);

        for (int i = 0; i < count; i++)
        {
            wxTreeItemId dishOne;
            
            dishOne = m_treeBookList->AppendItem(itemRoot, wxString::Format(wxT("seat_id: %d"), book[i].seat_id), 0);
            m_treeBookList->InsertItem(dishOne, 0, wxString::Format(wxT("dish_id: %d"), book[i].dish_id), 0);
            m_treeBookList->InsertItem(dishOne, 1, wxString::Format(wxT("book_numbers: %d"), book[i].book_numbers), 0);
            m_treeBookList->InsertItem(dishOne, 1, wxString::Format(wxT("accept_numbers: %d"), book[i].accept_numbers), 0);
            m_treeBookList->Expand(dishOne);
        }

		client_dish_list_free(book);
    }
}

void wxPanelMenu::onBtnVersionCheck(wxCommandEvent& event)
{
	version_status_t status;
	int r;
	
	if (m_client == NULL)
	{
		appendLog("检测版本失败，客户端没有连接服务器");
		return;
	}
	
	r = client_check_version(m_client, platfrom_mac_osx, 0, &status);
	if (r == 1)
	{
		wxString log;
		
		log = wxString::Format("检测版本更新: 本地版本(%d), 服务器版本(%d), 升级日志(%s)",
				   0,
				   status.new_ver.ver_id,
				   status.update_logs);

		appendLog(log);
	}
	else
	{
		appendLog("检测版本更新失败，连接不上服务器");
	}
}

void wxPanelMenu::onBtnLogin(wxCommandEvent& event)
{
    client_session_t session;
	if (m_client == NULL)
	{
		appendLog("检测版本失败，客户端没有连接服务器");
		return;
	}
	
    appendLog("开始登录 testid123");
	client_account_login(m_client, "testid", "123", 0, &session);
	appendAccountResult(this, session.result);
	
	appendLog("开始登录 youmenu");
	client_account_login(m_client, "youmenu", "youmenu", 1, &session);
	appendAccountResult(this, session.result);
}

void wxPanelMenu::onBtnLoginByToken(wxCommandEvent& event)
{
    client_session_t session;
    char token[38 + 1];
    
    {
        int r;

        client_session_t s;
        r = client_account_get_session(m_client, &s);
        if (r == 0)
        {
            appendLog("用户没有 token");
            return;
        }
        
        strcpy(token, s.token);
    }
    
    client_account_login2(m_client, token, &session);
	appendAccountResult(this, session.result);
}

void wxPanelMenu::onBtnLoginCreateAccount(wxCommandEvent& event)
{
	account_result r;
	account_t a;
	
	appendLog("开始创建 testid");
	
	strcpy(a.uid, "testid");
	strcpy(a.pwd, "123");
	a.type = account_menu;
	strcpy(a.nick, "nick");
	strcpy(a.comment, "comment");
	
	client_account_create(m_client, &a, &r);
	appendAccountResult(this, r);
}

void wxPanelMenu::onBtnLoginDeleteAccount(wxCommandEvent& event)
{
	account_result r;
    
	appendLog("开始删除 testid 使用错误密码");
    client_account_delete(m_client, "testid", "1234", &r);
    appendAccountResult(this, r);
    
	appendLog("开始删除 testid 使用正确密码");
    client_account_delete(m_client, "testid", "123", &r);
    appendAccountResult(this, r);
}

void wxPanelMenu::onBtnLoginOut(wxCommandEvent& event)
{
	account_result r;
    
    if (m_client == NULL)
    {
        appendLog("用户未登录");
        return;
    }
	
	client_account_logout(m_client, &r);
	
	appendAccountResult(this, r);
}

void wxPanelMenu::onBtnLoginList(wxCommandEvent& event)
{
	{
		account_result result;
		account_t * account;
		int count;
		
		client_account_list(m_client, &account, &count, &result);
		appendLog("当前用户列表");
		appendAccountResult(this, result);
		if (result == account_r_ok)
		{
			for (int i = 0; i < count; i++)
			{
				const char * uid = account[i].uid;
				
				appendLog(wxString::Format(wxT("%s"), uid));
			}
			
			client_account_list_free(account);
		}
	}
	
	{
		book_t * book;
		int count;
		dish_result result;
		
		appendLog("当前 0 桌的点餐信息");
		client_dish_list(m_client, 0, &book, &count, &result);
		appendDishResult(this, result);
		if (result == dish_r_ok)
		{
			for (int i = 0; i < count; i++)
			{
				appendLog(wxString::Format(wxT("id: %d"), book[i].dish_id));
			}
			
			client_dish_list_free(book);
		}
	}
	
	{
		seat_t * seat;
		int count;
		seat_result result;
		
		appendLog("当前所有桌的信息");
		client_seat_list(m_client, &seat, &count, &result);
		appendSeatResult(this, result);
		if (result == seat_r_ok)
		{
			for (int i = 0; i < count; i++)
			{
				appendLog(wxString::Format(wxT("id: %d"), seat[i].id));
			}
			
			client_seat_list_free(seat);
		}
	}
}
