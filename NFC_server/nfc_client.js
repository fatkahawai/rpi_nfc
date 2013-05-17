/**
 * NFC_CLIENT.JS
 * a JSON API request - an ajax request to a HTTP server which returns a JSON object 
 * 
 * When a user browses to http://localhost:3000, index.html is loaded, which then 
 * loads and executes this code
 */

window.onload = function() {
    var url, 
        response,
        data,
        jqxhr;  

    var customerId = prompt("Enter a customer ID");

    // Encode the user's input as query parameters in a URL
    var url = document.URL+'customers/'+customerId;       

    // send request as url
    jqxhr = $.getJSON( url, function(data) {
        console.log('API response received');    
        $('#customer').html('customer name is '+data['first']+' '+data['last']);        
      } 
    ); 
}; 




