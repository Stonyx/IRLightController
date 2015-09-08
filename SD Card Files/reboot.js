// IR Light Controller
// Copyright (C) 2015 Stonyx
//
// This software is licensed under the Creative Commons Attribution-NonCommercial-ShareAlike 4.0
// International License.
//
// You can redistribute and/or modify this software for non-commerical purposes under the terms
// of the Creative Commons Attribution-NonCommercial-ShareAlike 4.0 International License.
//
// This software is provided "as is" without express or implied warranty.

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
      url: "/reboot",
      type: "GET"
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
      alert("Failed to trigger reboot (" + textStatus + " - " + errorThrown + ").");
    });
  });
});
