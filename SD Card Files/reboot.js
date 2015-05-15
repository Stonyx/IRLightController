// Run this when ready
$(document).ready(function() 
{
  // Attach to the button
  $("button").on("click", function()
  {
    // Show the AJAX animation
    $("#ajax").show();

    // Send the AJAX request
    $.ajax({
      url: "reboot",
      type: "GET",
    }).done(function(data, textStatus, jqXHR) 
    {
      // After 1 minute hide the AJAX animation and redirect to the index page
      setTimeout(function()
      {
        $("#ajax").hide();
        window.location.href = 'index.htm';
      }, 60000);
    }); 
  });
});