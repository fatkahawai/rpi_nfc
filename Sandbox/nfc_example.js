
/**
 * NFC_EXAMPLE.JS
 * 
 * An example accessing the libnfc NFC device library (a C library) from javascript 
 * using the NODE-FFI (Foreign Function Interface) Javascript library from RBRANSON 
 * at github/node-ffi 
 *
 * The primary public interface to the NODE-FFI library is through the Library function, 
 * which has a prototype like this:
 *   ffi.Library(libraryFile, { functionSymbol: [ returnType, [ arg1Type, arg2Type, ... ], ... ]);
 * Use this to call any function in your external target library file
 *
 * Copyright (C) 2013 Ceeb
 */

// Module dependencies.
var fs = require('fs')
  , ref = require('ref')
  , ffi = require('ffi') 
  , Struct = require('ref-struct');

//
// typedef
//

var unknownObj = ref.types.void;
var voidPointer = ref.refType(unknownObj);

// types defined in nfc-types.h
//
// a struct containing details of the NFC device. nfc_connect() returns a pointer to this type of struct
var nfc_device_t = Struct({
  'pdc' : 'pointer', // fcn ptr to callbacks
  'acName' : 'string', // name of device, maxlen DEVICE_NAME_LENGTH
  'nc': 'int', // int or long or short?? its actually typedef enum{NC_PN531=0x10,NC_PN532=0x20,NC_PN533=0x30} nfc_chip_t;
  'nds' : 'pointer',  // // Device connection specification - typdedef void ptr
  'bActive' : 'bool',
  'bCrc' : 'bool',
  'bPar' : 'bool',
  'bEasyFraming' : 'bool',
  'bAutoIso14443_4' : 'bool',
  'ui8TxBits' : 'byte',     // actually typedef uint8_t 
  'ui8Parameters' : 'byte', // also a uint8_t
  'btSupportByte' : 'byte',
  'iLastError' : 'int'
});
var nfc_device_t_ptr = ref.refType(nfc_device_t); 

// a union of all the nfc device types
// nfc_target_info_t

//
// Declare public functions of the libnfc C library libnfc and libnfcutils
//
var myLibNfc = ffi.Library('libnfc', {
  "nfc_version": [ "string", [] ]
});

// Display libnfc version
var nfcVersion = myLibNfc.nfc_version();
console.log('this program uses libnfc library version ' + nfcVersion );






/* ==== EXAMPLE

double    do_some_number_fudging(double a, int b);
myobj *   create_object();
double    do_stuff_with_object(myobj *obj);
void      use_string_with_object(myobj *obj, char *value);
void      delete_object(myobj *obj);

// -----------------------------------------------------
// Our C code would be something like this:

#include "mylibrary.h"
int main()
{
    myobj *fun_object;
    double res, fun;

    res = do_some_number_fudging(1.5, 5);
    fun_object = create_object();

    if (fun_object == NULL) {
      printf("Oh no! Couldn't create object!\n");
      exit(2);
    }

    use_string_with_object(fun_object, "Hello World!");
    fun = do_stuff_with_object(fun_object);
    delete_object(fun_object);
}
// ----------------------------------------------------
// The JavaScript code to wrap this library would be:

var ffi = require("ffi");

// typedefs
var myobj = ref.types.void // we don't know what the layout of "myobj" looks like

var libmylibrary = ffi.Library('libmylibrary', {
  "do_some_number_fudging": [ 'double', [ 'double', 'int' ] ],
  "create_object": [ "pointer", [] ],
  "do_stuff_with_object": [ "double", [ "pointer" ] ],
  "use_string_with_object": [ "void", [ "pointer", "string" ] ],
  "delete_object": [ "void", [ "pointer" ] ]
});

// -----------------------------------------------------
// We could then use it from JavaScript:

var res = MyLibrary.do_some_number_fudging(1.5, 5);
var fun_object = MyLibrary.create_object();

if (fun_object.isNull()) {
    console.log("Oh no! Couldn't create object!\n");
} else {
    MyLibrary.use_string_with_object(fun_object, "Hello World!");
    var fun = MyLibrary.do_stuff_with_object(fun_object);
    MyLibrary.delete_object(fun_object);
}

*/
