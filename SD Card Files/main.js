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