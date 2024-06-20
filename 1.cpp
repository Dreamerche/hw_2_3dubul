#include <iostream>
#include <functional>
#include <string>

std::function<bool(unsigned int)> prime = [](unsigned int n) -> bool {//po-kratuk mein
        if (n <= 1) return false;
        for (size_t i = 2; i <= n / 2; i++) {
            if (n % i == 0) {
                return false;
            }
        }
        return true;
    };
class Set {
public:
    virtual bool isElementInSet(unsigned int n) const = 0;
    virtual void describe(std::ostream& os) const = 0;
    virtual ~Set() = default;
};
    class Union;
    class Intersect;

class Prazno: public Set {
public:
    bool isElementInSet(unsigned int n) const override {
        return false;
    }

    void describe(std::ostream& os) const override {
        os << "{nistu niama}";//moje i {}
    }
    ~Prazno(){};
};

class Estestveni: public Set {
public:
    bool isElementInSet(unsigned int n) const override {
        return true;
    }

    void describe(std::ostream& os) const override {
        os << "[0, +∞)";
    }
    ~Estestveni(){};
};

class Interval: public Set{
public:
    Interval(unsigned int a, unsigned int b) : a(a), b(b) {
        if(a>b){//proverka dali e korekten intervalut
            this->a=b;
            this->b=a;
        }
    }
    bool isElementInSet(unsigned int n) const override {
        return a<=n && n<=b;//a<=n<=b
    }

    void describe(std::ostream& os) const override {
        os << "["<<a<<", "<<b<<"]";
    }
    ~Interval(){};

    private:
    unsigned a,b;
};

class Union: public Set{
    public:
    Union(const Set& a, const Set& b) : first(a), second(b) {}
    bool isElementInSet(unsigned int n) const override {
        return first.isElementInSet(n) || second.isElementInSet(n);//n∈first ili second
    }

    void describe(std::ostream& os) const override {
        first.describe(std::cout);
        os <<" U ";
        second.describe(std::cout);
        os<<std::endl;
    }
    ~Union(){};

    private:
    const Set& first; 
    const Set& second;//samo dve mnojestva po uslovie
};

class Intersect: public Set{
    public:
    Intersect(const Set&a,const Set& b) : first(a), second(b) {};
    bool isElementInSet(unsigned int n) const override {
        return first.isElementInSet(n) && second.isElementInSet(n);//n∈first i second
    }

    void describe(std::ostream& os) const override {
        first.describe(std::cout);
        os <<" ∩ X";//simvol za sechenie ne izlzia i slagam X
        second.describe(std::cout);
        os<<std::endl;
    }
    ~Intersect(){};

    private:
    const Set& first; 
    const Set& second;//samo dve mnojestva po uslovie
};
class Filter : public Set {
    Set* setche;//iskah s &, ama ne stana zaradi meina;(
    std::function<bool(unsigned int)> funkcia;
    std::string description;
public:
    Filter(Set* a, std::function<bool(unsigned int)> b, std::string desc)
        : setche(a), funkcia(b), description(desc) {}
    ~Filter() { delete setche; }

    bool isElementInSet(unsigned int n) const override {
        return setche->isElementInSet(n) && funkcia(n);
    }

    void describe(std::ostream& os) const override {
        os << "{ a ∈ ";
        setche->describe(os);
        os << " | " << description << " }\n";
    }
};

class Series : public Set {
public:
    Series(Set* a, std::function<unsigned int(unsigned int)> b, std::string text)
        : setche(a), funkcia(b), description(text) {}

    bool isElementInSet(unsigned int n) const override {//n=i*i i i∈setche , no mislih, che moje i da e za n∈setche i 

        for (unsigned int i = 0; setche->isElementInSet(i); i++) {//priateli predlojiha, malko mi e stranno i triabva da se opravi
            if (funkcia(i) == n) return true;
        }
        return false;
    }

    void describe(std::ostream& os) const override {
        os << "{ " << description << " | n ∈ ";
        setche->describe(os);
        os << " }";
        os<<"\n";
    }
     private:
    Set* setche;
    std::function<unsigned int(unsigned int)> funkcia;
    std::string description;
};

class Family : public Set {

public:
    Family(const std::vector<Set*> s):sets(s){};

    void addSet(Set* s) {
        sets.push_back(s);
    }

    Family& operator+=(Set* s) {
        addSet(s);
        return *this;
    }

    Family& operator+(Set* s) {
        addSet(s);
        return *this;
    }


    //намиране на сечението и обединението на всички множества от фамилията
    //shtom e v uslovieto, shte go napisha da gi izvejda, ne pishe da se zapazvat niakude
    //realno move da proverim za sechenie:
    //nai-malkia interval, da go zasechem crez objojdane, posle chrez isElementInSet da si izvlechem vsichski elementi
    //koito uchastvat vuv vsichki mnojestva
    //za obedinenie shte vzemem vsichki elementi ot intervalite i shte vidim dali vseki ot tiah si uchastva v nqkoe
    //mnojestvo
    void findUnionOfTheFamilySets(){
    std::cout<< "{";
        bool first = true;
        for (const auto& set : sets) {
            if (!first){
                 std::cout << " ∩ ";
            }
            set->describe(std::cout);
            first = false;
        }
    std::cout << "}\n";
    }
    void findIntersectionOfTheFamilySets(){
    std::cout<< "{";
        bool first = true;
        for (const auto& set : sets) {
            if (!first){
                 std::cout << " U ";
            }
            set->describe(std::cout);
            first = false;
        }
    std::cout << "}\n";
    }



    bool isElementInSet(unsigned int n) const override {
        for (auto set : sets) {
            if (set->isElementInSet(n)) return true;
        }
        return false;
    }
    void describe(std::ostream& os) const override {//pak priateli pomagat
        os << "{";
        bool first = true;
        for (const auto& set : sets) {
            if (!first){
                 os << " , ";
            }
            set->describe(std::cout);
            first = false;
        }
        os << "}\n";
    }
    ~Family() {
        for (auto s : sets) {
            delete s;
        }
    }


    private:
    std::vector<Set*> sets;
};

Set* operator||(const Set& first, const Set& second) {
    return new Union(first, second);
}

Set* operator&&(const Set& first, const Set& second) {
    return new Intersect(first, second);
}
int main(){
    Set* interval1 = new Interval(0, 100);
    Filter* primes = new Filter(interval1, prime, "a is prime");
    Filter* evens = new Filter(interval1, [](unsigned a) {return a % 4 == 0;}, "4 divides a ");
    Set* interval2 = new Interval(0, 10);
    Series* squares = new Series(interval2, [](unsigned n) { return n * n; }, "n*n");

    Set* even_or_square = *primes || *squares;
    Set* even_and_square = *evens && *squares;

    primes->describe(std::cout);
    evens->describe(std::cout);
    squares->describe(std::cout);
    even_or_square->describe(std::cout);
    even_and_square->describe(std::cout);

    Family* family = new Family(std::vector<Set*>());
    family->operator+=(even_or_square);
    family->operator+= (even_and_square);
    family->describe(std::cout);

    delete interval1;
    delete primes;
    delete evens;
    delete interval2; 
    delete squares;
    delete family;

    // ne namiram otkude idva segmentation fault-a, no da sme jivi i zdravi, izviniavam se za kusnoto predavane,
    //imah malka greshka, zaradi koiato meina ne trugvashe
    return 0;
}