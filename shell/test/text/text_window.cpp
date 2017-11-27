#include "text_window.h"

#include "ukive/views/button.h"
#include "ukive/views/text_view.h"
#include "ukive/views/layout/linear_layout.h"
#include "ukive/views/layout/linear_layout_params.h"
#include "ukive/views/layout/restraint_layout.h"
#include "ukive/views/layout/restraint_layout_params.h"
#include "ukive/views/layout/base_layout_params.h"
#include "ukive/drawable/color_drawable.h"
#include "ukive/graphics/color.h"
#include "ukive/drawable/ripple_drawable.h"
#include "ukive/menu/context_menu.h"
#include "ukive/menu/menu.h"


namespace shell {

    TextWindow::TextWindow()
        :Window()
    {
    }

    TextWindow::~TextWindow()
    {
    }


    void TextWindow::onCreate()
    {
        Window::onCreate();

        typedef ukive::RestraintLayoutParams Rlp;

        mTBMCallback = new ToolbarMenuCallback(this);

        //root view.
        ukive::RestraintLayout *rootLayout = new ukive::RestraintLayout(this, ID_LAYOUT_ROOT);
        ukive::BaseLayoutParams *rightViewLp = new ukive::BaseLayoutParams(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::MATCH_PARENT);
        rootLayout->setLayoutParams(rightViewLp);

        setContentView(rootLayout);

        //Toolbar.
        /*auto toolbar = inflateToolbar(rootLayout);

        RestraintLayoutParams *toolbarLp = new RestraintLayoutParams(
            ULayoutParams::MATCH_PARENT, ULayoutParams::FIT_CONTENT);
        toolbarLp
            ->startHandle(ID_LAYOUT_ROOT, RestraintLayoutParams::EDGE_START)
            ->topHandle(ID_LAYOUT_ROOT, RestraintLayoutParams::EDGE_TOP)
            ->endHandle(ID_LAYOUT_ROOT, RestraintLayoutParams::EDGE_END);

        rootLayout->addWidget(toolbar, toolbarLp);*/


        //编辑器。
        ukive::TextView *editorTV = new ukive::TextView(this, ID_TV_EDITOR);
        editorTV->setIsEditable(true);
        editorTV->setIsSelectable(true);
        editorTV->setFontFamilyName(L"Consolas");
        editorTV->setTextSize(15);
        editorTV->setPadding(18, 18, 18, 18);
        editorTV->setLineSpacing(true, 1.2f);
        editorTV->autoWrap(false);
        editorTV->requestFocus();

        Rlp *editorTVLp = Rlp::Builder(
            ukive::LayoutParams::MATCH_PARENT, ukive::LayoutParams::MATCH_PARENT)
            .start(ID_LAYOUT_ROOT, Rlp::START)
            .top(ID_LAYOUT_ROOT, Rlp::TOP)
            .end(ID_LAYOUT_ROOT, Rlp::END)
            .bottom(ID_LAYOUT_ROOT, Rlp::BOTTOM).build();

        rootLayout->addWidget(editorTV, editorTVLp);
    }


    bool TextWindow::onMoving(ukive::Rect *rect)
    {
        ::OutputDebugStringW(L"onMoving()\n");
        return Window::onMoving(rect);
    }

    bool TextWindow::onResizing(int edge, ukive::Rect *rect)
    {
        ::OutputDebugStringW(L"onResizing()\n");
        return Window::onResizing(edge, rect);
    }

    void TextWindow::onMove(int x, int y)
    {
        ::OutputDebugStringW(L"onMove()\n");
        Window::onMove(x, y);
    }

    void TextWindow::onResize(
        int param, int width, int height,
        int clientWidth, int clientHeight)
    {
        ::OutputDebugStringW(L"onResize()\n");
        Window::onResize(param, width, height, clientWidth, clientHeight);
    }


    ukive::View *TextWindow::inflateToolbar(ukive::View *parent)
    {
        ukive::LinearLayout *toolbar = new ukive::LinearLayout(this, ID_TOOLBAR);
        toolbar->setElevation(1);
        toolbar->setOrientation(ukive::LinearLayout::HORIZONTAL);
        toolbar->setBackground(new ukive::ColorDrawable(ukive::Color::White));

        ukive::TextView *font = new ukive::TextView(this, ID_TOOLBAR_ITEM_FONT);
        font->setText(L"字体");
        font->setTextSize(13);
        font->setPadding(16, 8, 16, 8);
        font->setBackground(new ukive::RippleDrawable(this));
        font->setOnClickListener(new FontItemClickListener(this));
        font->setFocusable(true);

        ukive::LinearLayoutParams *fontParams
            = new ukive::LinearLayoutParams(
                ukive::LinearLayoutParams::FIT_CONTENT, ukive::LinearLayoutParams::MATCH_PARENT);
        toolbar->addWidget(font, fontParams);

        ukive::TextView *format = new ukive::TextView(this, ID_TOOLBAR_ITEM_FORMAT);
        format->setText(L"格式");
        format->setTextSize(13);
        format->setPadding(16, 8, 16, 8);
        format->setBackground(new ukive::RippleDrawable(this));
        format->setOnClickListener(new FormatItemClickListener(this));
        format->setFocusable(true);

        ukive::LinearLayoutParams *formatParams
            = new ukive::LinearLayoutParams(
                ukive::LinearLayoutParams::FIT_CONTENT, ukive::LinearLayoutParams::MATCH_PARENT);
        toolbar->addWidget(format, formatParams);

        return toolbar;
    }


    bool TextWindow::ToolbarMenuCallback::onCreateContextMenu(
        ukive::ContextMenu *contextMenu, ukive::Menu *menu)
    {
        menu->addItem(0, 0, L"Test");
        menu->addItem(1, 0, L"Test");
        menu->addItem(2, 0, L"Test");
        menu->addItem(3, 0, L"Test");

        return true;
    }

    bool TextWindow::ToolbarMenuCallback::onPrepareContextMenu(
        ukive::ContextMenu *contextMenu, ukive::Menu *menu)
    {
        return true;
    }

    bool TextWindow::ToolbarMenuCallback::onContextMenuItemClicked(
        ukive::ContextMenu *contextMenu, ukive::MenuItem *item)
    {
        contextMenu->close();
        return false;
    }

    void TextWindow::ToolbarMenuCallback::onDestroyContextMenu(
        ukive::ContextMenu *contextMenu)
    {
        mWindow->mContextMenu = nullptr;
    }


    void TextWindow::FontItemClickListener::onClick(ukive::View *widget)
    {
        if (mWindow->mContextMenu == nullptr)
        {
            mWindow->mContextMenu = mWindow->startContextMenu(
                mWindow->mTBMCallback, widget, ukive::Gravity::LEFT);

            mWindow->findViewById(TextWindow::ID_TOOLBAR_ITEM_FONT)->requestFocus();
        }
    }

    void TextWindow::FormatItemClickListener::onClick(ukive::View *widget)
    {
        if (mWindow->mContextMenu == nullptr)
        {
            mWindow->mContextMenu = mWindow->startContextMenu(
                mWindow->mTBMCallback, widget, ukive::Gravity::LEFT);

            mWindow->findViewById(TextWindow::ID_TOOLBAR_ITEM_FORMAT)->requestFocus();
        }
    }

}