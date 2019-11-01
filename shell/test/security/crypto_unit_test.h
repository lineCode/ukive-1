#ifndef SHELL_TEST_SECURITY_CRYPTO_UNIT_TEST_H_
#define SHELL_TEST_SECURITY_CRYPTO_UNIT_TEST_H_


namespace shell {
namespace test {

    /**
     * 该测试代码来自 FIPS PUB 197
     */
    int TEST_AES();

    int TEST_RSA();

    /**
     * 该测试代码来自 RFC7748
     * https://tools.ietf.org/html/rfc7748
     */
    void TEST_ECDP_X25519();

    /**
     * 该测试代码来自 RFC7748
     * https://tools.ietf.org/html/rfc7748
     */
    void TEST_ECDP_X448();

    /**
     * 该测试代码来自:
     * https://csrc.nist.gov/Projects/Cryptographic-Algorithm-Validation-Program/CAVP-TESTING-BLOCK-CIPHER-MODES
     */
    void TEST_AEAD_AES_GCM();

}
}

#endif  // SHELL_TEST_SECURITY_CRYPTO_UNIT_TEST_H_