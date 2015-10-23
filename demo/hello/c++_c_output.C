
extern "C" { extern int printf( char *, ... )  ; }

enum { ZERO, ONE, TEN = 10, HUNDRED = 100, THOUSAND = 1000 } ;


//-------------------------------------------------------//

class Base {
private:
  int Base__privateData ;

protected:
  int Base__protectedData ;

public:
  int Base__publicData ;

  Base( int arg = ONE ) : Base__privateData   ( arg + THOUSAND ),
                          Base__protectedData ( arg + HUNDRED  ),
                          Base__publicData    ( arg            )  {}
  ~Base() {}

  int Base__getPrivateData() { return Base__privateData ; }

  virtual int F1() = 0 ;
  virtual int F2() { return Base__privateData ; }

} ;

//-------------------------------------------------------//

class Derived : public Base {
private:
  int Derived__privateData ;

public:
  int Derived__publicData ;

  Derived( int arg = ZERO ) : Derived__privateData ( arg ),
                              Derived__publicData  ( arg ),
                              Base                 ( arg )  {}
  ~Derived() {}

  int Derived__getPrivateData() { return Derived__privateData ; }

  int F1()   { return Base__protectedData             ; }
  int F2()   { return Base__protectedData + TEN + ONE ; }
} ;


//-------------------------------------------------------//

int main() {
  int     i ;
  Derived d1 ;
  Derived d2( 2 ) ;

  printf( "%d\n", d1.Derived__publicData  ) ;
  d1.Derived__publicData = TEN ;
  printf( "%d\n", d1.Derived__publicData ) ;

  i = d1.Base__getPrivateData() ;
  printf( "%d\n", i ) ;

  i = d1.Derived__getPrivateData() ;
  printf( "%d\n", i ) ;

  i = d1.F1() ;
  printf( "%d\n", i ) ;

  i = d1.F2() ;
  printf( "%d\n", i ) ;
}
