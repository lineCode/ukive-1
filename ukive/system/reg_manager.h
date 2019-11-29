#ifndef UKIVE_SYSTEM_REG_MANAGER_H_
#define UKIVE_SYSTEM_REG_MANAGER_H_

#include "utils/string_utils.h"


namespace ukive {

    class RegManager {
    public:
        static bool registerApplication();
        static bool unregisterApplication();

        static bool registerProgId(const string16& progid, const string16& cmd);
        static bool unregisterProgId(const string16& progid);

        static bool associateExtName(
            const string16& ext_name, const string16& progid);
        static bool unassociateExtName(
            const string16& ext_name, const string16& progid);

        static bool hasProgId(const string16& progid);
        static bool hasProgIdInExt(const string16& ext_name, const string16& progid);

    private:
        static bool createProgId(const string16& progid, const string16& cmd);
        static bool deleteProgId(const string16& progid);

        static bool addProgIdToExt(const string16& ext_name, const string16& progid);
        static bool removeProgIdFromExt(const string16& ext_name, const string16& progid);
    };

}

#endif  // UKIVE_SYSTEM_REG_MANAGER_H_