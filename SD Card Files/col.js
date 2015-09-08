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
  // Show the AJAX animation
  $("#ajax-loader").show();

  // Send the AJAX request
  $.ajax({
    url: "/get?cv",
    type: "GET",
    dataType: "arraybuffer",
    processData: "false"
  }).
  done(function(data, textStatus, jqXHR)
  {
    // Convert the data to a unsigned byte array
    data = new Uint8Array(data);

    // Loop through all the color values
    for (var i = 1; i <= COLOR_VALUES_COUNT; ++i)
    {
      // Get the color data from the array
      var baseLocation = SIZE_OF_COLOR_VALUES * (i - 1)
      var red = data[baseLocation];
      var green = data[baseLocation + 1];
      var blue = data[baseLocation + 2];
      var white = data[baseLocation + 3];

      // Set the fields
      $("#red-" + i).val(red);
      $("#green-" + i).val(green);
      $("#blue-" + i).val(blue);
      $("#white-" + i).val(white);
    }

    // Hide the AJAX animation
    $("#ajax-loader").hide();
  }).
  fail(function(jqXHR, textStatus, errorThrown)
  {
    // Hide the AJAX animation
    $("#ajax-loader").hide();

    // Show the error message
    alert("Failed to retrieve color values (" + textStatus + " - " + errorThrown + ").")
  });

  // Attach to the Save button
  $(".btn-green").on("click", function()
  {
    // Show the AJAX animation
    $("#ajax-loader").show();

    // Create the data array
    var data = new Uint8Array(SIZE_OF_COLOR_VALUES * COLOR_VALUES_COUNT);

    // Loop through all the color values
    for (var i = 1; i <= COLOR_VALUES_COUNT; ++i)
    {
      // Get the color data
      var red = parseInt($("#red-" + i).val());
      var green = parseInt($("#green-" + i).val());
      var blue = parseInt($("#blue-" + i).val());
      var white = parseInt($("#white-" + i).val());

      // Add the data to the array
      var baseLocation = SIZE_OF_COLOR_VALUES * (i - 1);
      data[baseLocation] = red;
      data[baseLocation + 1] = green;
      data[baseLocation + 2] = blue;
      data[baseLocation + 3] = white;
    }

    // Send the data
    $.ajax({
      url: "/set?cv",
      type: "POST",
      dataType: "arraybuffer",
      processData: "false",
      data: data
    }).
    done(function(data, textStatus, jqXHR)
    {
      // Hide the AJAX animation
      $("#ajax-loader").hide();

      // Redirect to the index page
      window.location.href = "/index.htm";
    }).
    fail(function(jqXHR, textStatus, errorThrown)
    {
      // Hide the AJAX animation
      $("#ajax-loader").hide();

      // Show the error message
      alert("Failed to save color values (" + textStatus + " - " + errorThrown + ").");
    });
  });
});
