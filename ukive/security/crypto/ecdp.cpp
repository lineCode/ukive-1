#include "ukive/security/crypto/ecdp.h"

#include "ukive/log.h"


namespace ukive {
namespace crypto {

    void ECDP::secp192k1(
        BigInteger* p,
        BigInteger* a, BigInteger* b,
        BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h)
    {
        *p = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFEE37", 16);
        *a = BigInteger::fromU32(0);
        *b = BigInteger::fromU32(3);
        *Gx = BigInteger::fromString("DB4FF10E C057E9AE 26B07D02 80B7F434 1DA5D1B1 EAE06C7D", 16);
        *Gy = BigInteger::fromString("9B2F2F6D 9C5628A7 844163D0 15BE8634 4082AA88 D95E2F9D", 16);
        *n = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFE 26F2FC17 0F69466A 74DEFD8D", 16);
        *h = 1;
    }

    void ECDP::secp192r1(
        BigInteger* p,
        BigInteger* a, BigInteger* b, BigInteger* S,
        BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h)
    {
        *p = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF FFFFFFFF", 16);
        *a = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF FFFFFFFC", 16);
        *b = BigInteger::fromString("64210519 E59C80E7 0FA7E9AB 72243049 FEB8DEEC C146B9B1", 16);
        *S = BigInteger::fromString("3045AE6F C8422F64 ED579528 D38120EA E12196D5", 16);
        *Gx = BigInteger::fromString("188DA80E B03090F6 7CBF20EB 43A18800 F4FF0AFD 82FF1012", 16);
        *Gy = BigInteger::fromString("07192B95 FFC8DA78 631011ED 6B24CDD5 73F977A1 1E794811", 16);
        *n = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF 99DEF836 146BC9B1 B4D22831", 16);
        *h = 1;
    }

    void ECDP::secp224k1(
        BigInteger* p,
        BigInteger* a, BigInteger* b,
        BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h)
    {
        *p = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFE56D", 16);
        *a = BigInteger::fromU32(0);
        *b = BigInteger::fromU32(5);
        *Gx = BigInteger::fromString("A1455B33 4DF099DF 30FC28A1 69A467E9 E47075A9 0F7E650E B6B7A45C", 16);
        *Gy = BigInteger::fromString("7E089FED 7FBA3442 82CAFBD6 F7E319F7 C0B0BD59 E2CA4BDB 556D61A5", 16);
        *n = BigInteger::fromString("01 00000000 00000000 00000000 0001DCE8 D2EC6184 CAF0A971 769FB1F7", 16);
        *h = 1;
    }

    void ECDP::secp224r1(
        BigInteger* p,
        BigInteger* a, BigInteger* b, BigInteger* S,
        BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h)
    {
        *p = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF 00000000 00000000 00000001", 16);
        *a = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF FFFFFFFF FFFFFFFE", 16);
        *b = BigInteger::fromString("B4050A85 0C04B3AB F5413256 5044B0B7 D7BFD8BA 270B3943 2355FFB4", 16);
        *S = BigInteger::fromString("BD713447 99D5C7FC DC45B59F A3B9AB8F 6A948BC5", 16);
        *Gx = BigInteger::fromString("B70E0CBD 6BB4BF7F 321390B9 4A03C1D3 56C21122 343280D6 115C1D21", 16);
        *Gy = BigInteger::fromString("BD376388 B5F723FB 4C22DFE6 CD4375A0 5A074764 44D58199 85007E34", 16);
        *n = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFF16A2 E0B8F03E 13DD2945 5C5C2A3D", 16);
        *h = 1;
    }

    void ECDP::secp256k1(
        BigInteger* p,
        BigInteger* a, BigInteger* b,
        BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h)
    {
        *p = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFC2F", 16);
        *a = BigInteger::fromU32(0);
        *b = BigInteger::fromU32(7);
        *Gx = BigInteger::fromString("79BE667E F9DCBBAC 55A06295 CE870B07 029BFCDB 2DCE28D9 59F2815B 16F81798", 16);
        *Gy = BigInteger::fromString("483ADA77 26A3C465 5DA4FBFC 0E1108A8 FD17B448 A6855419 9C47D08F FB10D4B8", 16);
        *n = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE BAAEDCE6 AF48A03B BFD25E8C D0364141", 16);
        *h = 1;
    }

    void ECDP::secp256r1(
        BigInteger* p,
        BigInteger* a, BigInteger* b, BigInteger* S,
        BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h)
    {
        *p = BigInteger::fromString("FFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFF", 16);
        *a = BigInteger::fromString("FFFFFFFF 00000001 00000000 00000000 00000000 FFFFFFFF FFFFFFFF FFFFFFFC", 16);
        *b = BigInteger::fromString("5AC635D8 AA3A93E7 B3EBBD55 769886BC 651D06B0 CC53B0F6 3BCE3C3E 27D2604B", 16);
        *S = BigInteger::fromString("C49D3608 86E70493 6A6678E1 139D26B7 819F7E90", 16);
        *Gx = BigInteger::fromString("6B17D1F2 E12C4247 F8BCE6E5 63A440F2 77037D81 2DEB33A0 F4A13945 D898C296", 16);
        *Gy = BigInteger::fromString("4FE342E2 FE1A7F9B 8EE7EB4A 7C0F9E16 2BCE3357 6B315ECE CBB64068 37BF51F5", 16);
        *n = BigInteger::fromString("FFFFFFFF 00000000 FFFFFFFF FFFFFFFF BCE6FAAD A7179E84 F3B9CAC2 FC632551", 16);
        *h = 1;
    }

    void ECDP::secp384r1(
        BigInteger* p,
        BigInteger* a, BigInteger* b, BigInteger* S,
        BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h)
    {
        *p = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF"
            "00000000 00000000 FFFFFFFF", 16);
        *a = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFFFF"
            "00000000 00000000 FFFFFFFC", 16);
        *b = BigInteger::fromString("B3312FA7 E23EE7E4 988E056B E3F82D19 181D9C6E FE814112 0314088F 5013875A C656398D"
            "8A2ED19D 2A85C8ED D3EC2AEF", 16);
        *S = BigInteger::fromString("A335926A A319A27A 1D00896A 6773A482 7ACDAC73", 16);
        *Gx = BigInteger::fromString("AA87CA22 BE8B0537 8EB1C71E F320AD74 6E1D3B62 8BA79B98 59F741E0 82542A38"
            "5502F25D BF55296C 3A545E38 72760AB7", 16);
        *Gy = BigInteger::fromString("3617DE4A 96262C6F 5D9E98BF 9292DC29 F8F41DBD 289A147C E9DA3113 B5F0B8C0"
            "0A60B1CE 1D7E819D 7A431D7C 90EA0E5F", 16);
        *n = BigInteger::fromString("FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF C7634D81 F4372DDF"
            "581A0DB2 48B0A77A ECEC196A CCC52973", 16);
        *h = 1;
    }

    void ECDP::secp521r1(
        BigInteger* p,
        BigInteger* a, BigInteger* b, BigInteger* S,
        BigInteger* Gx, BigInteger* Gy, BigInteger* n, uint8_t* h)
    {
        *p = BigInteger::fromString("01FF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF"
            "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF", 16);
        *a = BigInteger::fromString("01FF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF"
            "FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFC", 16);
        *b = BigInteger::fromString("0051 953EB961 8E1C9A1F 929A21A0 B68540EE A2DA725B 99B315F3"
            "B8B48991 8EF109E1 56193951 EC7E937B 1652C0BD 3BB1BF07 3573DF88 3D2C34F1 EF451FD4 6B503F00", 16);
        *S = BigInteger::fromString("D09E8800 291CB853 96CC6717 393284AA A0DA64BA", 16);
        *Gx = BigInteger::fromString("00C6858E 06B70404 E9CD9E3E CB662395 B4429C64 8139053F"
            "B521F828 AF606B4D 3DBAA14B 5E77EFE7 5928FE1D C127A2FF A8DE3348 B3C1856A 429BF97E 7E31C2E5 BD66", 16);
        *Gy = BigInteger::fromString("0118 39296A78 9A3BC004 5C8A5FB4 2C7D1BD9 98F54449"
            "579B4468 17AFBD17 273E662C 97EE7299 5EF42640 C550B901 3FAD0761 353C7086 A272C240 88BE9476 9FD16650", 16);
        *n = BigInteger::fromString("01FF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF"
            "FFFFFFFF FFFFFFFA 51868783 BF2F966B 7FCC0148 F709A5D0 3BB5C9B8 899C47AE BB6FB71E 91386409", 16);
        *h = 1;
    }

    void ECDP::curve25519(
        BigInteger* p,
        uint32_t* A, BigInteger* order, uint8_t* cofactor, uint8_t* Up, BigInteger* Vp)
    {
        *p = BigInteger::fromU32(1).mul2exp(255).sub(19);
        *A = 486662;
        *order = BigInteger::fromU32(1).mul2exp(252)
            .add(BigInteger::fromString("14def9dea2f79cd65812631a5cf5d3ed", 16));
        *cofactor = 8;
        *Up = 9;
        *Vp = BigInteger::fromString("1478161944758954479102059356840998688"
            "7264606134616475288964881837755586237401", 10);
    }

    void ECDP::curve448(
        BigInteger* p,
        uint32_t* A, BigInteger* order, uint8_t* cofactor, uint8_t* Up, BigInteger* Vp)
    {
        *p = BigInteger::fromU32(1).mul2exp(448).sub(BigInteger::fromU32(1).mul2exp(224)).sub(1);
        *A = 156326;
        *order = BigInteger::fromU32(1).mul2exp(446)
            .sub(BigInteger::fromString("8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d", 16));
        *cofactor = 4;
        *Up = 5;
        *Vp = BigInteger::fromString("355293926785568175264127502063783334808976399387714271831880898"
            "435169088786967410002932673765864550910142774147268105838985595290606362", 10);
    }

    void ECDP::edwards25519(
        BigInteger* p,
        BigInteger* d, BigInteger* order, uint8_t* cofactor, BigInteger* Xp, BigInteger* Yp)
    {
        *p = BigInteger::fromU32(1).mul2exp(255).sub(19);
        *d = BigInteger::fromString("370957059346694393431380835087545651895421138798432190163887855330"
            "85940283555", 10);
        *order = BigInteger::fromU32(1).mul2exp(252)
            .add(BigInteger::fromString("14def9dea2f79cd65812631a5cf5d3ed", 16));
        *cofactor = 8;
        *Xp = BigInteger::fromString("151122213495354007725011514095885315114540126930418572060461132"
            "83949847762202", 10);
        *Yp = BigInteger::fromString("463168356949264781694283940034751631413079938662562256157830336"
            "03165251855960", 10);
    }

    void ECDP::edwards448_1(
        BigInteger* p,
        BigInteger* d, BigInteger* order, uint8_t* cofactor, BigInteger* Xp, BigInteger* Yp)
    {
        *p = BigInteger::fromU32(1).mul2exp(448).sub(BigInteger::fromU32(1).mul2exp(224)).sub(1);
        *d = BigInteger::fromString("611975850744529176160423220965553317543219696871016626328968936415"
            "087860042636474891785599283666020414768678979989378147065462815545017", 10);
        *order = BigInteger::fromU32(2).mul2exp(446)
            .sub(BigInteger::fromString("8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d", 16));
        *cofactor = 4;
        *Xp = BigInteger::fromString("345397493039729516374008604150537410266655260075183290216406970"
            "281645695073672344430481787759340633221708391583424041788924124567700732", 10);
        *Yp = BigInteger::fromString("363419362147803445274661903944002267176820680343659030140745099"
            "590306164083365386343198191849338272965044442230921818680526749009182718", 10);
    }

    void ECDP::edwards448_2(
        BigInteger* p,
        BigInteger* d, BigInteger* order, uint8_t* cofactor, BigInteger* Xp, BigInteger* Yp)
    {
        *p = BigInteger::fromU32(1).mul2exp(448).sub(BigInteger::fromU32(1).mul2exp(224)).sub(1);
        *d = BigInteger::from32(-39081);
        *order = BigInteger::fromU32(1).mul2exp(446)
            .sub(BigInteger::fromString("8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d", 16));
        *cofactor = 4;
        *Xp = BigInteger::fromString("224580040295924300187604334099896036246789641632564134246125461"
            "686950415467406032909029192869357953282578032075146446173674602635247710", 10);
        *Yp = BigInteger::fromString("298819210078481492676017930443930673437544040154080242095928241"
            "372331506189835876003536878655418784733982303233503462500531545062832660", 10);
    }

    void ECDP::addPoint(
        const BigInteger& p, const BigInteger& a,
        const BigInteger& x1, const BigInteger& y1,
        BigInteger* x2, BigInteger* y2)
    {
        bool equal = (x1 == *x2) && (y1 == *y2);
        BigInteger lambda;
        if (equal) {
            BigInteger de(y1);
            de.mul2();
            de = de.invmod(p);

            BigInteger no(x1);
            no.exp2().mul(3).add(a).mul(de).mod(p);
            lambda = no;
        } else {
            BigInteger de(*x2);
            de.sub(x1);
            if (de.isMinus()) {
                de.add(p);
            }
            de = de.invmod(p);

            BigInteger no(*y2);
            no.sub(y1).mul(de).modP(p);
            lambda = no;
        }

        BigInteger xr(lambda);
        xr.exp2().sub(x1).sub(*x2).modP(p);

        BigInteger yr(x1);
        yr.sub(xr).mul(lambda).sub(y1).modP(p);

        *x2 = std::move(xr);
        *y2 = std::move(yr);
    }

    void ECDP::mulPoint(
        const BigInteger& p, const BigInteger& a,
        const BigInteger& d, BigInteger* x, BigInteger* y)
    {
        BigInteger rx(*x), ry(*y);
        int count = d.getBitCount();
        for (int i = count - 2; i >= 0; --i) {
            addPoint(p, a, rx, ry, &rx, &ry);
            if (d.getBit(i)) {
                addPoint(p, a, *x, *y, &rx, &ry);
            }
        }

        *x = std::move(rx);
        *y = std::move(ry);
    }

    bool ECDP::verifyPoint(
        const BigInteger& p,
        const BigInteger& a, const BigInteger& b,
        const BigInteger& x, const BigInteger& y)
    {
        BigInteger left(y);
        left.exp2().mod(p);

        BigInteger ax(a);
        ax.mul(x);

        BigInteger right(x);
        right.pow(3).add(ax).add(b).mod(p);

        return left == right;
    }

    void ECDP::X25519(
        const BigInteger& p, const BigInteger& k, const BigInteger& u,
        BigInteger* result)
    {
        X25519_448(p, k, u, 121665, result);
    }

    void ECDP::X448(
        const BigInteger& p, const BigInteger& k, const BigInteger& u,
        BigInteger* result)
    {
        X25519_448(p, k, u, 39081, result);
    }

    void ECDP::X25519_448(
        const BigInteger& p, const BigInteger& k, const BigInteger& u,
        uint32_t a24, BigInteger* result)
    {
        BigInteger x1(u);
        BigInteger x2 = BigInteger::ONE;
        BigInteger z2 = BigInteger::ZERO;
        BigInteger x3(u);
        BigInteger z3 = BigInteger::ONE;
        uint8_t swap = 0;

        int count = k.getBitCount();
        for (int t = count - 1; t >= 0; --t) {
            uint8_t kt = k.getBit(t);
            swap ^= kt;
            cswap(swap, &x2, &x3);
            cswap(swap, &z2, &z3);
            swap = kt;

            BigInteger A(x2 + z2); A.mod(p);
            BigInteger AA(A); AA.exp2().mod(p);
            BigInteger B(x2 - z2); B.mod(p);
            BigInteger BB(B); BB.exp2().mod(p);
            BigInteger E(AA - BB); E.mod(p);
            BigInteger C(x3 + z3); C.mod(p);
            BigInteger D(x3 - z3); D.mod(p);
            BigInteger DA(D * A); DA.mod(p);
            BigInteger CB(C * B); CB.mod(p);
            x3 = DA; x3.add(CB).exp2().mod(p);
            z3 = DA; z3.sub(CB).exp2().mul(x1).mod(p);
            x2 = AA * BB; x2.mod(p);
            z2 = E; z2.mul(BigInteger::fromU32(a24)).add(AA).mul(E).mod(p);
        }

        cswap(swap, &x2, &x3);
        cswap(swap, &z2, &z3);

        *result = z2;
        result->powMod(p - 2, p).mul(x2).mod(p);
    }

    void ECDP::cswap(uint8_t swap, BigInteger* x2, BigInteger* x3) {
        if (swap != 0) {
            x2->swap(*x3);
        }
    }

}
}