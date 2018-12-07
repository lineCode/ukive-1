#include "ukive/system/reg_manager.h"

#include <Windows.h>
#include <ShlObj.h>

#include "ukive/log.h"


namespace ukive {

    bool RegManager::registerApplication() {
        return false;
    }

    bool RegManager::unregisterApplication() {
        return false;
    }

    bool RegManager::associateExtName(
        const string16& ext_name, const string16& progid, const string16& cmd)
    {
        if (!createProgId(progid, cmd)) {
            return false;
        }

        if (!addProgIdToExt(ext_name, progid)) {
            return false;
        }

        ::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

        return true;
    }

    bool RegManager::unassociateExtName(const string16& ext_name, const string16& progid) {
        if (!deleteProgId(progid)) {
            return false;
        }

        if (!removeProgIdFromExt(ext_name, progid)) {
            return false;
        }

        ::SHChangeNotify(SHCNE_ASSOCCHANGED, SHCNF_IDLIST, nullptr, nullptr);

        return true;
    }

    bool RegManager::createProgId(const string16& progid, const string16& cmd) {
        string16 class_path = L"SOFTWARE\\Classes\\";

        // 创建 ProgID
        HKEY progid_key = nullptr;
        string16 progid_path = class_path + progid;
        auto ls = ::RegCreateKeyExW(
            HKEY_CURRENT_USER, progid_path.c_str(),
            0, nullptr, 0, KEY_WRITE, nullptr, &progid_key, nullptr);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        string16 friendly_name = L"XPicture Image Viewer";
        ls = ::RegSetValueExW(
            progid_key, L"", 0, REG_SZ,
            reinterpret_cast<const BYTE*>(friendly_name.data()),
            friendly_name.size() * sizeof(wchar_t) + 1);
        bool result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(progid_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        // Shell
        HKEY cmd_key = nullptr;
        string16 command_path = progid_path + L"\\Shell\\Open\\Command";
        ls = ::RegCreateKeyExW(
            HKEY_CURRENT_USER, command_path.c_str(),
            0, nullptr, 0, KEY_WRITE, nullptr, &cmd_key, nullptr);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        ls = ::RegSetValueExW(
            cmd_key, L"", 0, REG_SZ,
            reinterpret_cast<const BYTE*>(cmd.data()),
            cmd.size() * sizeof(wchar_t) + 1);
        result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(cmd_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        return true;
    }

    bool RegManager::deleteProgId(const string16& progid) {
        string16 class_path = L"SOFTWARE\\Classes\\";

        // 删除 ProgID
        HKEY progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(HKEY_CURRENT_USER, class_path.c_str(), 0, KEY_ALL_ACCESS, &progid_key);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        ls = ::RegDeleteTreeW(progid_key, progid.c_str());
        bool result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(progid_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        return true;
    }

    bool RegManager::hasProgId(const string16& progid) {
        string16 progid_path = L"SOFTWARE\\Classes\\" + progid;

        // 尝试打开 ProgID
        HKEY progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(HKEY_CURRENT_USER, progid_path.c_str(), 0, KEY_READ, &progid_key);
        if (ls == ERROR_SUCCESS) {
            return true;
        } else if (ls == ERROR_FILE_NOT_FOUND) {
            return false;
        } else {
            // TODO:
            return false;
        }
    }

    bool RegManager::addProgIdToExt(const string16& ext_name, const string16& progid) {
        string16 class_path = L"SOFTWARE\\Classes\\";
        string16 ext_progid_path = class_path + ext_name + L"\\OpenWithProgids";

        // 先在对应扩展名的 OpenWithProgids 子键中创建一个值
        HKEY ext_progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(
            HKEY_CURRENT_USER, ext_progid_path.c_str(), 0, KEY_SET_VALUE, &ext_progid_key);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        ls = ::RegSetValueExW(
            ext_progid_key,
            progid.c_str(),
            0, REG_SZ, nullptr, 0);
        bool result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(ext_progid_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        return true;
    }

    bool RegManager::removeProgIdFromExt(const string16& ext_name, const string16& progid) {
        string16 class_path = L"SOFTWARE\\Classes\\";
        string16 ext_progid_path = class_path + ext_name + L"\\OpenWithProgids";

        // 删除对应扩展名的 OpenWithProgids 子键中的值
        HKEY ext_progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(
            HKEY_CURRENT_USER, ext_progid_path.c_str(), 0, KEY_ALL_ACCESS, &ext_progid_key);
        if (ls != ERROR_SUCCESS) {
            DCHECK(false);
            return false;
        }

        ls = ::RegDeleteValueW(
            ext_progid_key,
            progid.c_str());
        bool result = (ls == ERROR_SUCCESS);

        ls = ::RegCloseKey(ext_progid_key);
        result &= (ls == ERROR_SUCCESS);
        if (!result) {
            DCHECK(false);
            return false;
        }

        return true;
    }

    bool RegManager::hasProgIdInExt(const string16& ext_name, const string16& progid) {
        string16 class_path = L"SOFTWARE\\Classes\\";
        string16 ext_progid_path = class_path + ext_name + L"\\OpenWithProgids";

        // 打开对应扩展名的 OpenWithProgids 子键
        HKEY ext_progid_key = nullptr;
        auto ls = ::RegOpenKeyExW(
            HKEY_CURRENT_USER, ext_progid_path.c_str(), 0, KEY_READ, &ext_progid_key);
        if (ls != ERROR_SUCCESS) {
            if (ls == ERROR_FILE_NOT_FOUND) {
                return false;
            } else {
                // TODO:
                return false;
            }
        }

        ls = ::RegGetValueW(
            ext_progid_key, nullptr, progid.c_str(), RRF_RT_REG_SZ,
            nullptr, nullptr, nullptr);
        if (ls != ERROR_SUCCESS) {
            if (ls == ERROR_FILE_NOT_FOUND) {
                return false;
            } else {
                // TODO:
                return false;
            }
        }

        return true;
    }
}