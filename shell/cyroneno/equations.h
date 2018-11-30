#ifndef CYRONENO_EQUATIONS_H_
#define CYRONENO_EQUATIONS_H_


namespace cyro {

    class Quadratic {
    public:
        Quadratic(double a, double b, double c);
        Quadratic(const Quadratic& rhs);

        int getRootCount() const;

        double a_, b_, c_;
        double drt_;
        double root1_, root2_;
    };

}

#endif  // CYRONENO_EQUATIONS_H_