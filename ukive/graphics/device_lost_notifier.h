#ifndef UKIVE_GRAPHICS_DEVICE_LOST_NOTIFIER_H_
#define UKIVE_GRAPHICS_DEVICE_LOST_NOTIFIER_H_


namespace ukive {

    class DeviceLostNotifier {
    public:
        virtual ~DeviceLostNotifier() = default;

        virtual void onDeviceLost() = 0;
        virtual void onDeviceRestored() = 0;
    };

}

#endif  // UKIVE_GRAPHICS_DEVICE_LOST_NOTIFIER_H_