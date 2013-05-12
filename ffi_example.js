/*
 *ffi_example.js
 * 
 * an example using the node ffi library to access foreign C libraries.
 * run this example by executing the following command line 
 * > node ffi_example
 *
 * (C) 2013 Ceeb
 */

// import the FFI library
var ffi = require('ffi');

// declare the libm C dynamic library 
var libm = ffi.Library('libm', {
  'ceil': [ 'double', [ 'double' ] ]
});

// try calling one of the libm C library functions
console.log('ceil(1.5)= '+libm.ceil(1.5)); // 2

// You can also access just functions in the current process by passing a null
var current = ffi.Library(null, {
  'atoi': [ 'int', [ 'string' ] ]
});
console.log('atoi(1234)= '+current.atoi('1234')); // 1234


