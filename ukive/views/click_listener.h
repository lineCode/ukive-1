#ifndef UKIVE_VIEWS_CLICK_LISTENER_H_
#define UKIVE_VIEWS_CLICK_LISTENER_H_


namespace ukive {

    class View;

    class OnClickListener
    {
    public:
        virtual ~OnClickListener() = default;

        virtual void onClick(View *v) = 0;
    };

}

#endif  // UKIVE_VIEWS_CLICK_LISTENER_H_