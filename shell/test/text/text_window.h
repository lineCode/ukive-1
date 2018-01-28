﻿#ifndef SHELL_TEST_TEXT_TEXT_WINDOW_H_
#define SHELL_TEST_TEXT_TEXT_WINDOW_H_

#include "ukive/window/window.h"
#include "ukive/views/click_listener.h"
#include "ukive/menu/context_menu_callback.h"


namespace ukive {
    class Rect;
    class ContextMenu;
}

namespace shell {

    class TextWindow : public ukive::Window
    {
    private:
        static const int ID_TV_EDITOR = 0x1;
        static const int ID_LAYOUT_ROOT = 0x2;
        static const int ID_TOOLBAR = 0x3;
        static const int ID_TOOLBAR_ITEM_FONT = 0x10;
        static const int ID_TOOLBAR_ITEM_FORMAT = 0x11;

        class ToolbarMenuCallback : public ukive::ContextMenuCallback
        {
        private:
            TextWindow *mWindow;
        public:
            ToolbarMenuCallback(TextWindow *w) { mWindow = w; }

            bool onCreateContextMenu(ukive::ContextMenu *contextMenu, ukive::Menu *menu) override;
            bool onPrepareContextMenu(ukive::ContextMenu *contextMenu, ukive::Menu *menu) override;
            bool onContextMenuItemClicked(ukive::ContextMenu *contextMenu, ukive::MenuItem *item) override;
            void onDestroyContextMenu(ukive::ContextMenu *contextMenu) override;
        };

        class FontItemClickListener : public ukive::OnClickListener
        {
        private:
            TextWindow *mWindow;
        public:
            FontItemClickListener(TextWindow *w) { mWindow = w; }
            void onClick(ukive::View *v) override;
        };

        class FormatItemClickListener : public ukive::OnClickListener
        {
        private:
            TextWindow *mWindow;
        public:
            FormatItemClickListener(TextWindow *w) { mWindow = w; }
            void onClick(ukive::View *v) override;
        };

    private:
        ukive::ContextMenu *mContextMenu;
        ToolbarMenuCallback *mTBMCallback;

        ukive::View *inflateToolbar(ukive::View *parent);

    public:
        TextWindow();
        ~TextWindow();

        void onCreate() override;
        bool onMoving(ukive::Rect *rect) override;
        bool onResizing(int edge, ukive::Rect *rect) override;
        void onMove(int x, int y) override;
        void onResize(
            int param, int width, int height,
            int clientWidth, int clientHeight) override;
    };

}

#endif  // SHELL_TEST_TEXT_TEXT_WINDOW_H_