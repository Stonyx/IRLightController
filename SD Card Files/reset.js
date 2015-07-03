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
    }).
    done(function(data, textStatus, jqXHR) 
    {
      // Wait 1 minute
      setTimeout(function()
      {
        // Hide the AJAX animation
        $("#ajax-loader").hide();

        // Redirect to the index page
        window.location.href = "/index.htm";
      }, 60000);
    }).
    fail(function(jqXHR, textStatus, errorThrown)
    {
      // Hide the AJAX animation
      $("#ajax-loader").hide();

      // Show the error message
      alert("Failed to trigger reset.")
    });
  });
});