

class Context {
public:
    bool isMgr;
};

class Bonus {
public:
    double CalcBonus(Context &ctx) {
        double bonus = 0.0;
        bonus += CalcMonthBonus(ctx);
        bonus += CalcSumBonus(ctx);
        if (ctx.isMgr) {
            bonus += CalcGroupBonus(ctx);
        }
        return bonus;
    }

private:
    double CalcMonthBonus(Context &ctx) {
        double bonus;
        return bonus;
    }

    double CalcSumBonus(Context &ctx) {
        double bonus;
        return bonus;
    }

    double CalcGroupBonus(Context &ctx) {
        double bonus;
        return bonus;
    }
};