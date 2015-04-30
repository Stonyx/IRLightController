// Run this when ready
$(document).ready(function() 
{
  // Create the AJAX request object
  var xhr = new XMLHttpRequest();
  xhr.open('GET', 'get?st', true);
  xhr.responseType = 'arraybuffer';
  xhr.onload = function(event)
  {
    // Check if we received an OK response
    if (this.status == 200)
    {
      // Get the time and the color values data
      var time = new Uint32Array(this.response).subarray(0, 1);
      var values = new Uint8Array(this.response).subarray(4);

      // Create a date object
      var date = new Date(time[0] * 1000);

      // Display the time and color values      
      $("#time").html();
      $("#red").html(values[0]);
      $("#green").html(values[1]);
      $("#blue").html(values[2]);
      $("#white").html(values[3]));
    }
  };

  // Send the AJAX request
  xhr.send();
});