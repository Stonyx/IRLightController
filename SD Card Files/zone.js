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
    url: "/get?tz",
    type: "GET",
    dataType: "arraybuffer",
    processData: "false"
  }).
  done(function(data, textStatus, jqXHR)
  {
    // Convert the data to a unsigned byte array
    data = new Uint8Array(data);

    // Get the time zone data from the array
    var offset = 0x0000;
    for (var i = 0; i < 2; ++i)
    {
      offset = offset | (data[i] << (8 * i));
    }
    if ((offset & 0x8000) > 0)
      offset = offset - 0x10000;
    var dstOffset = 0x0000;
    for (var i = 2; i < 4; ++i)
    {
      dstOffset = dstOffset | (data[i] << (8 * (i - 2)));
    }
    if ((dstOffset & 0x8000) > 0)
      dstOffset = dstOffset - 0x10000;
    var dstStartMonth = data[4];
    var dstEndMonth = data[5];
    var dstStartWeekdayNumber = data[6];
    var dstEndWeekdayNumber = data[7];
    var dstStartWeekday = data[8];
    var dstEndWeekday = data[9];
    var dstStartMinutes = 0x0000;
    for (var i = 10; i < 12; ++i)
    {
      dstStartMinutes = dstStartMinutes | (data[i] * (8 * (i - 10)));
    }
    var dstEndMinutes = 0x0000;
    for (var i = 12; i < 14; ++i)
    {
      dstEndMinutes = dstEndMinutes | (data[i] * (8 * (i - 12)));
    }
    
    // Calculate the time zone, DST offset, DST start time, and DST end time
    offset = offset / 30;
    dstOffset = dstOffset / 30;
    var dstStartTime = dstStartMinutes / 30;
    var dstEndTime = dstEndMinutes / 30;
    
    // Set the fields
    $("#time-zone").val(offset);
    $("#dst-offset").val(dstOffset);
    $("#dst-start-month").val(dstStartMonth);
    $("#dst-start-weekday-number").val(dstStartWeekdayNumber);
    $("#dst-start-weekday").val(dstStartWeekday);
    $("#dst-start-time").val(dstStartTime);
    $("#dst-end-month").val(dstEndMonth);
    $("#dst-end-weekday-number").val(dstEndWeekdayNumber);
    $("#dst-end-weekday").val(dstEndWeekday);
    $("#dst-end-time").val(dstEndTime);

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
    var data = new Uint8Array(SIZE_OF_TIME_ZONE);

    // Get the time zone data
    var offset = parseInt($("#offset"));
    var dstOffset = parseInt($("#dst-offset"));
    var dstStartMonth = parseInt($("#dst-start-month"));
    var dstStartWeekdayNumber = parseInt($("#dst-start-weekday-number"));
    var dstStartWeekday = parseInt($("#dst-start-weekday"));
    var dstStartTime = parseInt($("#dst-start-time"));
    var dstEndMonth = parseInt($("#dst-end-month"));
    var dstEndWeekdayNumber = parseInt($("#dst-end-weekday-number"));
    var dstEndWeekday = parseInt($("#dst-end-weekday"));
    var dstEndTime = parseInt($("#dst-end-time"));
        
    // Calculate the time zone, DST offset, DST start time, and DST end time
    offset = offset * 30;
    dstOffset = dstOffset * 30;
    var dstStartMinutes = dstStartTime * 30;
    var dstEndMinutes = dstEndTime * 30;

    // Add the data to the array
    for (var i = 0; i < 2; ++i)
    {
      data[i] = (offset >> (8 * i)) & 0x00FF;
    }
    for (var i = 2; i < 4; ++i)
    {
      data[i] = (dstOffset >> (8 * (i - 2))) & 0x00FF;
    }
    data[4] = dstStartMonth;
    data[5] = dstEndMonth;
    data[6] = dstStartWeekdayNumber;
    data[7] = dstEndWeekdayNumber;
    data[8] = dstStartWeekday;
    data[9] = dstEndWeekday;
    for (var i = 10; i < 12; ++i)
    {
      data[i] = (dstStartMinutes >> (8 * (i - 10))) & 0x00FF;
    }
    for (var i = 12; i < 14; ++i)
    {
      data[i] = (dstEndMinutes >> (8 * (i - 12))) & 0x00FF;
    }

    // Send the data
    $.ajax({
      url: "/set?tz",
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
