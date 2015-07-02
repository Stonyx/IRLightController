// Run this when ready
$(document).ready(function() 
{
  // Attach to the Yes button
  $(".btn-green").on("click", function()
  {
    // Show the AJAX animation
    $("#ajax-loader").show();

    // Send the AJAX request
    $.ajax({
      url: "/reset",
      type: "GET",
    }).done(function(data, textStatus, jqXHR) 
    {
      // After 1 minute hide the AJAX animation and redirect to the index page
      setTimeout(function()
      {
        $("#ajax-loader").hide();
        window.location.href = "/index.htm";
      }, 60000);
    }); 
  });
});