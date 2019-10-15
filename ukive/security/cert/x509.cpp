#include "ukive/security/cert/x509.h"


namespace ukive {
namespace cert {

    X509::X509() {}

    bool X509::parse(const std::string& payload) {
        ASN1Reader reader(payload);
        if (reader.beginSequence()) {
            parseTBSCertificate(reader);
            parseAlgorithmIdentifier(reader);
            parseSignatureValue(reader);

            reader.endSequence();
        }
        return true;
    }

    bool X509::parseTBSCertificate(ASN1Reader& reader) {
        if (reader.beginSequence()) {
            if (!reader.nextValue()) {
                return false;
            }

            uint64_t ver;
            if (!reader.getInteger(&ver)) {
                return false;
            }

            std::string serial;
            if (!reader.getBigInteger(&serial)) {
                return false;
            }

            AlgorithmIdentifier algo_id;
            if (!parseAlgorithmIdentifier(reader, &algo_id)) {
                return false;
            }

            Name issuer;
            if (!parseName(reader, &issuer)) {
                return false;
            }

            // Validity
            {
                if (!reader.beginSequence()) {
                    return false;
                }

                std::string time_str;
                if (!reader.getTime(&time_str)) {
                    return false;
                }

                std::string time_str2;
                if (!reader.getTime(&time_str2)) {
                    return false;
                }

                reader.endSequence();
            }

            Name subject;
            if (!parseName(reader, &subject)) {
                return false;
            }

            // SubjectPublicKeyInfo
            {
                if (!reader.beginSequence()) {
                    return false;
                }

                SubjectPublicKeyInfo info;
                if (!parseAlgorithmIdentifier(reader, &info.algorithm)) {
                    return false;
                }

                uint8_t unused = 0;
                reader.getBitString(
                    ASN1Reader::UniversalTags::BitString, &info.subject_public_key, &unused);

                reader.endSequence();
            }

            reader.nextValue();
            reader.nextValue();

            reader.endSequence();
        }
        return true;
    }

    bool X509::parseAlgorithmIdentifier(ASN1Reader& reader) {
        if (reader.beginSequence()) {
            reader.nextValue();
            reader.endSequence();
        }
        return true;
    }

    bool X509::parseSignatureValue(ASN1Reader& reader) {
        if (reader.beginSequence()) {
            reader.nextValue();
            reader.endSequence();
        }
        return true;
    }

    bool X509::parseName(ASN1Reader& reader, Name* out) {
        if (!reader.beginSequence()) {
            return false;
        }

        while (!reader.isOutOfBounds()) {
            std::vector<AttributeTypeAndValue> attrs;
            if (!reader.beginSet()) {
                return false;
            }

            do {
                if (!reader.beginSequence()) {
                    return false;
                }

                AttributeTypeAndValue attr;
                if (!reader.getObjectID(&attr.type)) {
                    return false;
                }
                if (!reader.getAny(&attr.value)) {
                    return false;
                }
                attrs.push_back(std::move(attr));

                reader.endSequence();
            } while (!reader.isOutOfBounds());

            reader.endSet();
            out->rdn_sequence.push_back(std::move(attrs));
        }

        reader.endSequence();
        return true;
    }

    bool X509::parseAlgorithmIdentifier(ASN1Reader& reader, AlgorithmIdentifier* out) {
        if (!reader.beginSequence()) {
            return false;
        }

        if (!reader.getObjectID(&out->algorithm)) {
            return false;
        }

        if (!reader.getAny(&out->parameters)) {
            return false;
        }

        reader.endSequence();
        return true;
    }

}
}