typedef int (*Vtbl_ptr)() ;

struct Vtbl {
    short    d ;
    short    i ;
    Vtbl_ptr f ;
};


enum Enum1 { ZERO, ONE, TEN, HUNDRED, THOUSAND } ;


/*-------------------------------------------------------*/

struct Base {
          int   Base__privateData ;
          int   Base__protectedData ;
          int   Base__publicData ;
   struct Vtbl *Base__Vtbl ;
} ;


// extern        char *malloc() ;
extern struct Vtbl *Ptbl[] ;


static struct
Base* Base__constructor( struct Base* this, int arg ) {

    if ( this || ( this = ( struct Base * ) malloc( sizeof ( struct Base ) ) ) )

      ( ( ( ( this->Base__Vtbl = (struct Vtbl *) Ptbl[0] ),
            ( this->Base__privateData = ( arg + 1000 ) ) ),
          ( this->Base__protectedData = ( arg + 100 ) ) ),
        ( this->Base__publicData = arg ) ) ;

    return this ;
}


static char
Base__destructor( struct Base *this, int freeFlag ) {
   if ( this ) {
      this->Base__Vtbl = (struct Vtbl *) Ptbl[0] ;
      if ( this )
         if ( freeFlag & 1 )
            free( (char *) this ) ;
    }
}


static int
Base__getPrivateData( struct Base *this ) {
    return ( this->Base__privateData ) ;
}


static int
Base__F2( struct Base *this ) {
    return ( this->Base__privateData ) ;
}


/*-------------------------------------------------------*/
struct Derived {
          int   Base__privateData ;
          int   Base__protectedData ;
          int   Base__publicData ;

   struct Vtbl *Base__Vtbl ;
          int   Derived__privateData ;
          int   Derived__publicData ;
} ;


static struct
Derived* Derived__constructor( struct Derived* this, int arg ) {

   if ( this || ( this = (struct Derived *) malloc( sizeof ( struct Derived ) ) ) )

      ( ( ( ( this = (struct Derived *) Base__constructor( ( (struct Base *) this ), arg ) ),
            ( this->Base__Vtbl = (struct Vtbl *) Ptbl[1] ) ),
          ( this->Derived__privateData = arg ) ),
        ( this->Derived__publicData = arg ) ) ;

    return this ;
}


static char
Derived__destructor( struct Derived* this, int freeFlag ) {
   if ( this ) {
      this->Base__Vtbl = (struct Vtbl *) Ptbl[1] ;
      if ( this ) {
         Base__destructor( ( (struct Base *) this ), (int )0 ) ;
         if ( freeFlag & 1 )
            free( (char * ) this ) ;
      }
   }
}


static int
Derived__getPrivateData( struct Derived* this ) {
    return this->Derived__privateData ;
}


static int
Derived__F1( struct Derived* this ) {
    return ( this->Base__protectedData ) ;
}


static int
Derived__F2( struct Derived* this ) {
    return ( ( this->Base__protectedData + 10 ) + 1 );
}


// extern int printf() ;

/*-------------------------------------------------------*/
int main() {

          int     i ;

   struct Derived d1 ;
   struct Derived d2 ;

   Derived__constructor( &d1, (int)0 ) ;
   Derived__constructor( &d2, 2 ) ;

   printf( (char *)"%d\n", d1.Derived__publicData ) ;
   d1.Derived__publicData = 10 ;
   printf( (char *)"%d\n", d1.Derived__publicData ) ;

   i = Base__getPrivateData( (struct Base *) ( &d1 ) ) ;
   printf( (char *)"%d\n", i ) ;

   i = Derived__getPrivateData( &d1 ) ;
   printf( (char *)"%d\n", i ) ;

   i = Derived__F1( &d1 ) ;
   printf( (char *)"%d\n", i ) ;

   i = Derived__F2( &d1 ) ;
   printf( (char *)"%d\n", i ) ;

   Derived__destructor( &d2, 2 ) ;
   Derived__destructor( &d1, 2 ) ;

   exit( 0 ) ;
}


/*-------------------------------------------------------*/
struct Vtbl Derived__Vtbl[] = {
    0,
    0,
    0,
    0,
    0,
    ( Vtbl_ptr ) Derived__F1,
    0,
    0,
    ( Vtbl_ptr ) Derived__F2,
    0,
    0,
    0
};

char pure_virtual_called()
{
   printf( (char *)"pure_virtual_called\n" ) ;
   exit( 1 );
}

struct Vtbl Base__Vtbl[] = {
    0,
    0,
    0,
    0,
    0,
    ( Vtbl_ptr ) pure_virtual_called,
    0,
    0,
    ( Vtbl_ptr ) Base__F2,
    0,
    0,
    0
};

struct Vtbl *Ptbl[] = {
    Base__Vtbl,
    Derived__Vtbl,
} ;
