#include "ukive/security/crypto/rsa.h"

#include "ukive/log.h"


namespace ukive {
namespace crypto {

    BigIntegerClass RSA::getPrime() {
        auto init = BigIntegerClass::TWO;
        init.pow(BigIntegerClass::fromU16(1023));
        if (init.isEven()) {
            init.add(BigIntegerClass::ONE);
        }

        int i = 0;
        while (!isPrime(init)) {
            init.add(BigIntegerClass::TWO);
            ++i;
        }

        // Release
        // Desktop: 1:50 (8), 0:27 (16),
        // Laptop:  2:31 (8), 0:37 (16),

        // Debug
        // Desktop: 2:04 (16)
        LOG(Log::INFO) << "Prime retry: " << i;
        return init;
    }

    bool RSA::isPrime(const BigIntegerClass& bi) {
        auto result = BigIntegerClass::TWO;
        auto bi_1 = bi - BigIntegerClass::ONE;
        auto exp = bi_1;

        while (exp.isEven()) {
            exp.shr(1);
            result.powMod(exp, bi);
            if (result == bi_1) {
                break;
            }
            if (result == BigIntegerClass::ONE) {
                continue;
            }
            return false;
        }
        return true;
    }

}
}