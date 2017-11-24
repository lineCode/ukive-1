﻿#ifndef UKIVE_TEXT_INPUT_CONNECTION_H_
#define UKIVE_TEXT_INPUT_CONNECTION_H_

#include <msctf.h>

#include "ukive/utils/com_ptr.h"
#include "ukive/utils/string_utils.h"


namespace ukive {

    class TextView;
    class TsfEditor;
    class TsfManager;

    class InputConnection
    {
    private:
        TextView *mTextView;

        TsfEditor *mTsfEditor;
        TfEditCookie mEditorCookie;
        ComPtr<ITfContext> mEditorContext;
        ComPtr<ITfDocumentMgr> mDocumentMgr;
        ComPtr<ITfContextOwnerCompositionServices> mCompServices;

        bool mIsInitialized;
        bool mIsEditorPushed;

    public:
        InputConnection(TextView *textView);
        ~InputConnection();

        HRESULT initialization(TsfManager *tsfMgr);

        void pushEditor();
        void popEditor();

        bool mount(TsfManager *tsfMgr);
        bool unmount(TsfManager *tsfMgr);
        bool terminateComposition();

        void notifyStatusChanged(DWORD flags);
        void notifyTextChanged(bool correction, long start, long oldEnd, long newEnd);
        void notifyTextLayoutChanged(TsLayoutCode reason);
        void notifyTextSelectionChanged();


        void onBeginProcess();
        void onEndProcess();

        bool isReadOnly();
        void determineInsert(
            long start, long end, unsigned long repLength,
            long *resStart, long *resEnd);

        bool getSelection(
            unsigned long startIndex, unsigned long maxCount,
            TS_SELECTION_ACP *selections, unsigned long *fetchedCount);
        bool setSelection(unsigned long count, const TS_SELECTION_ACP *selections);

        std::wstring getText(long start, long end, long maxLength);
        void setText(long start, long end, std::wstring newText);

        long getTextLength();
        bool getTextPositionAtPoint(const POINT *pt, DWORD dwFlags, long *pacp);
        bool getTextBound(long start, long end, RECT *prc, BOOL *pfClipped);

        void getTextViewBound(RECT *prc);

        void insertTextAtSelection(
            DWORD dwFlags, std::wstring text,
            LONG *pacpStart, LONG *pacpEnd, TS_TEXTCHANGE *pChange);

        HWND getWindowHandle();
    };

}

#endif  // UKIVE_TEXT_INPUT_CONNECTION_H_