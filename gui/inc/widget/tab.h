#pragma once

#include "../core/container.h"
#include "../core/custom_msg.h"
#include "../common/common_ctrl.h"

#include "../layout/tab.h"

NS_GUI_BEGIN

void reflect_tab_msg() {
	msg_reflector::reg(WM_NOTIFY, [](wnd_msg& msg) {
		NMHDR* hdr = msg.lp;
		if(hdr->code == TCN_SELCHANGE) {
			::PostMessage(hdr->hwndFrom, TAB_CHANGE, msg.wp, msg.lp);
		}
	});
}
CALL_BEFORE_MAIN(reflect_tab_msg);

namespace initor {
	struct tab : base<tab> {
		tab() {
			layout( new layout::tab );
		}
	};
}

struct tab;

template<>
struct wnd_traits<tab> {
	typedef initor::tab initor_t;
};

struct tab : container<event::base> {
	typedef tab self_t;
	typedef container<event::base> super_t;

	property::rw<int> active;

	tab() {
		INIT_P_RW(tab, active);
	}

	string class_name() {
		return "systabcontrol32";
	}

	void process_msg(wnd_msg& msg) {
		super_t::process_msg(msg);

		if(msg.type == TAB_CHANGE) {
			int index = get_active();
			active = index;
		}
	}

	virtual void create() {
		super_t::create();
		for(auto& c : children) {
			c->visible = false;
			add_tab_header(c->text);
		}
		active = 0;
	}

	int get_active() {
		return TabCtrl_GetCurSel(hwnd);
	}
	void set_active(int index) {
		if((int)children.size() <= index) return;
		wnd_ptr& act = children[index];
		static_cast<layout::tab*>(layout.get())->activate(act);
		TabCtrl_SetCurSel(hwnd, index);
	}

	int tab_count() {
		return TabCtrl_GetItemCount(hwnd);
	}
	
	void add_tab_header(const string& name) {
		int index = tab_count();

		TCITEM tie = {0};

		tie.mask = TCIF_TEXT;
		tie.pszText = (char*)name.c_str();
		tie.lParam = index;
		TabCtrl_InsertItem(hwnd, index, &tie);
	}

};


NS_GUI_END
