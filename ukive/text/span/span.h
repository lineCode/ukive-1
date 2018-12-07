#ifndef UKIVE_TEXT_SPAN_SPAN_H_
#define UKIVE_TEXT_SPAN_SPAN_H_


namespace ukive {

    class Span {
    public:
        Span(unsigned int start, unsigned int end);
        virtual ~Span();

        enum BaseType {
            TEXT = 0x1,
            INTERACTABLE = 0x2,
            EFFECT = 0x4,
            INLINEOBJECT = 0x8,
        };

        enum Type {
            TEXT_UNDERLINE = 0x100,
            EFFECT_NORMAL = 0x101,
        };

        virtual int getType() const = 0;
        virtual int getBaseType() const = 0;

        void resize(unsigned int start, unsigned int end);

        unsigned int getStart() const;
        unsigned int getEnd() const;

    private:
        unsigned int start_;
        unsigned int end_;
    };

}

#endif  // UKIVE_TEXT_SPAN_SPAN_H_