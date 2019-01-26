#ifndef UKIVE_TEXT_TEXT_ACTION_MODE_CALLBACK_H_
#define UKIVE_TEXT_TEXT_ACTION_MODE_CALLBACK_H_


namespace ukive {

    class Menu;
    class MenuItem;
    class TextActionMode;

    class TextActionModeCallback {
    public:
        virtual ~TextActionModeCallback() = default;

        virtual bool onCreateActionMode(TextActionMode* mode, Menu* menu) = 0;
        virtual bool onPrepareActionMode(TextActionMode* mode, Menu* menu) = 0;
        virtual bool onActionItemClicked(TextActionMode* mode, MenuItem* item) = 0;
        virtual void onDestroyActionMode(TextActionMode* mode) = 0;
        virtual void onGetContentPosition(int* x, int* y) = 0;
    };

}

#endif  // UKIVE_TEXT_TEXT_ACTION_MODE_CALLBACK_H_