// Define constants
var COLOR_VALUES_COUNT = 5; // Power, Red, Green, Blue, and White
var MEMORY_SCHEDULE_COUNT = 50; // Must be multiples of 2
var TIMER_SCHEDULE_COUNT = 50; // Must be multiples of 5

// Register an AJAX transport for receiving array buffer data
$.ajaxTransport("+*", function(options, originalOptions, jqXHR)
{
  // Check if the data type is set to an array buffer
  if (options.dataType == 'arraybuffer')
  {
    return {
      send: function(headers, completeCallback)
      {
        // Create the request object
        var xhr = new XMLHttpRequest();
        
        // Add the load event listener
        xhr.addEventListener('load', function()
        {
          // Call the callback method
          var response = {};
          response[options.dataType] = xhr.response;
          completeCallback(xhr.status, xhr.statusText, response, xhr.getAllResponseHeaders());
        });

        // Send the request
        xhr.open(options.type, options.url, options.async);
        xhr.responseType = options.dataType;
        xhr.send();
      },
      abort: function()
      {
        jqXHR.abort();
      }
    };
  }
});