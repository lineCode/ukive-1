#ifndef UKIVE_VIEWS_SEEK_VALUE_CHANGED_LISTENER_H_
#define UKIVE_VIEWS_SEEK_VALUE_CHANGED_LISTENER_H_


namespace ukive {

    class SeekBar;

    class OnSeekValueChangedListener {
    public:
        virtual ~OnSeekValueChangedListener() = default;

        virtual void onSeekValueChanged(SeekBar *seekBar, float value) = 0;
        virtual void onSeekIntegerValueChanged(SeekBar *seekBar, int value) = 0;
    };

}

#endif  // UKIVE_VIEWS_SEEK_VALUE_CHANGED_LISTENER_H_