#ifndef UKIVE_SECURITY_CERT_X509_H_
#define UKIVE_SECURITY_CERT_X509_H_

#include "utils/string_utils.h"

#include "ukive/security/cert/asn1_reader.h"


namespace ukive {
namespace cert {

    class ASN1Reader;

    // RFC 8446
    // https://tools.ietf.org/html/rfc5280

    // https://docs.microsoft.com/zh-cn/windows/win32/api/wincrypt/nf-wincrypt-certgetcertificatechain

    // https://www.itu.int/rec/T-REC-X/en

    class X509 {
    public:
        enum class Version : uint8_t {
            V1 = 0,
            V2 = 1,
            V3 = 2,
        };

        X509();

        bool parse(const std::string& payload);

    private:
        struct Time {
            bool is_utc;
            std::string utc_time;
            std::string general_time;
        };

        struct AttributeTypeAndValue {
            ASN1Reader::ObjectID type;
            std::string value;
        };

        struct Name {
            std::vector<std::vector<AttributeTypeAndValue>> rdn_sequence;
        };

        struct AlgorithmIdentifier {
            ASN1Reader::ObjectID algorithm;
            std::string parameters;
        };

        struct Validity {
            Time not_before;
            Time not_after;
        };

        struct SubjectPublicKeyInfo {
            AlgorithmIdentifier algorithm;
            std::string subject_public_key;
        };

        struct Extension {
            ASN1Reader::ObjectID extn_id;
            bool critical = false;
            std::string extn_value;
        };

        struct TBSCertificate {
            Version version = Version::V1;
            std::string serial_number;
            AlgorithmIdentifier signature;
            Name issuer;
            Validity validity;
            Name subject;
            SubjectPublicKeyInfo subject_public_key_info;
            std::string issuer_unique_id;
            std::string subject_unique_id;
            std::vector<Extension> extensions;
        };

        struct Certificate {
            TBSCertificate tbs_certificate;
            AlgorithmIdentifier signature_algorithm;
            std::string signature_value;
        };

        bool parseTBSCertificate(ASN1Reader& reader);
        bool parseAlgorithmIdentifier(ASN1Reader& reader);
        bool parseSignatureValue(ASN1Reader& reader);

        bool parseName(ASN1Reader& reader, Name* out);
        bool parseAlgorithmIdentifier(ASN1Reader& reader, AlgorithmIdentifier* out);
    };

}
}

#endif  // UKIVE_SECURITY_CERT_X509_H_