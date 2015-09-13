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
    url: "/get?ip",
    type: "GET",
    dataType: "arraybuffer",
    processData: "false"
  }).
  done(function(data, textStatus, jqXHR)
  {
    // Convert the data to a unsigned byte array
    data = new Uint8Array(data);

    // Loop through all the addresses
    for (var i = 1; i <= ADDRESSES_COUNT; ++i)
    {
      // Get the address data from the array and set the fields
      var baseLocation = (i == 1 ? 0 : SIZE_OF_MAC_ADDRESS + SIZE_OF_IP_ADDRESS * (i - 2));
      $("#octet-1-" + i).val(data[baseLocation].toString(i == 1 ? 16 : 10).toUpperCase());
      $("#octet-2-" + i).val(data[baseLocation + 1].toString(i == 1 ? 16 : 10).toUpperCase());
      $("#octet-3-" + i).val(data[baseLocation + 2].toString(i == 1 ? 16 : 10).toUpperCase());
      $("#octet-4-" + i).val(data[baseLocation + 3].toString(i == 1 ? 16 : 10).toUpperCase());
      if (i == 1)
      {
        $("#octet-5-" + i).val(data[baseLocation + 4].toString(16).toUpperCase());
        $("#octet-6-" + i).val(data[baseLocation + 5].toString(16).toUpperCase());
      }
    }

    // Hide the AJAX animation
    $("#ajax-loader").hide();
  }).
  fail(function(jqXHR, textStatus, errorThrown)
  {
    // Hide the AJAX animation
    $("#ajax-loader").hide();

    // Show the error message
    alert("Failed to retrieve network settings (" + textStatus + " - " + errorThrown + ").")
  });

  // Attach to the Save button
  $(".btn-green").on("click", function()
  {
    // Show the AJAX animation
    $("#ajax-loader").show();

    // Create the data array
    var data = new Uint8Array(SIZE_OF_MAC_ADDRESS + SIZE_OF_IP_ADDRESS * (ADDRESSES_COUNT - 1));

    // Loop through all the addresses
    for (var i = 1; i <= ADDRESSES_COUNT; ++i)
    {
      // Get the address data and add it to the array
      var baseLocation = i == 1 ? 0 : SIZE_OF_MAC_ADDRESS + SIZE_OF_IP_ADDRESS * (i - 2);
      data[baseLocation] = parseInt($("#octet-1-" + i).val(), i == 1 ? 16 : 10);
      data[baseLocation + 1] = parseInt($("#octet-2-" + i).val(), i == 1 ? 16 : 10);
      data[baseLocation + 2] = parseInt($("#octet-3-" + i).val(), i == 1 ? 16 : 10);
      data[baseLocation + 3] = parseInt($("#octet-4-" + i).val(), i == 1 ? 16 : 10);
      if (i == 1)
      {
        data[baseLocation + 4] = parseInt($("#octet-5-1").val(), 16);
        data[baseLocation + 5] = parseInt($("#octet-6-1").val(), 16);
      }
    }

    // Send the data
    $.ajax({
      url: "/set?ip",
      type: "POST",
      dataType: "arraybuffer",
      processData: "false",
      data: data
    }).
    done(function(data, textStatus, jqXHR)
    {
      // Wait 30 seconds
      setTimeout(function()
      {
        // Hide the AJAX animation
        $("#ajax-loader").hide();

        // Redirect to the index page
        window.location.href = "/index.htm";
      }, 30000);
    }).
    fail(function(jqXHR, textStatus, errorThrown)
    {
      // Hide the AJAX animation
      $("#ajax-loader").hide();

      // Show the error message
      alert("Failed to save network settings (" + textStatus + " - " + errorThrown + ").");
    });
  });
});
