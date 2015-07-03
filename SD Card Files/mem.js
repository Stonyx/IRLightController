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

  // Copy the schedule HTML
  for (var i = 2; i <= MEMORY_SCHEDULE_COUNT; ++i)
  { 
    // Clone the first schedule and change heading text and various IDs
    var schedule = $("#schedule-1").clone();
    schedule.attr("id", "schedule-" + i);
    schedule.find("#heading-1").html("<strong>Memory Schedule #" + i + "</strong>");
    schedule.find("#heading-1").attr("id", "heading-" + i);
    schedule.find("label[for='button-1']").attr("for", "button-" + i);
    schedule.find("#button-1").attr("id", "button-" + i);
    schedule.find("label[for='weekday-1']").attr("for", "weekday-" + i);
    schedule.find("#weekday-1").attr("id", "weekday-" + i);
    schedule.find("label[for='start-hour-1']").attr("for", "start-hour-" + i);
    schedule.find("#start-hour-1").attr("id", "start-hour-" + i);
    schedule.find("label[for='start-minute-1']").attr("for", "start-minute-" + i);
    schedule.find("#start-minute-1").attr("id", "start-minute-" + i);
    schedule.find("label[for='start-second-1']").attr("for", "start-second-" + i);
    schedule.find("#start-second-1").attr("id", "start-second-" + i);
    schedule.find("label[for='stop-hour-1']").attr("for", "stop-hour-" + i);
    schedule.find("#stop-hour-1").attr("id", "stop-hour-" + i);
    schedule.find("label[for='stop-minute-1']").attr("for", "stop-minute-" + i);
    schedule.find("#stop-minute-1").attr("id", "stop-minute-" + i);
    schedule.find("label[for='stop-second-1']").attr("for", "stop-second-" + i);
    schedule.find("#stop-second-1").attr("id", "stop-second-" + i);
    schedule.find("#line-1").attr("id", "line-" + i);

    // Check if this is the last schedule
    if (i == MEMORY_SCHEDULE_COUNT)
      schedule.find("#line-" + i).remove();

    // Add the schedule
    schedule.appendTo("#schedules");
  }

  // Send the AJAX request
  $.ajax({
    url: "/getms",
    type: "GET",
    dataType: "arraybuffer",
    processData: "false",
  }).
  done(function(data, textStatus, jqXHR)
  {
    // Convert the data to a unsigned byte array
    data = new Uint8Array(data);

    // Loop through all the schedules
    for (var i = 1; i <= MEMORY_SCHEDULE_COUNT; ++i)
    {
      // Get the schedule data from the array
      var baseLocation = SIZE_OF_MEMORY_SCHEDULE * (i - 1);
      var button = data[baseLocation];
      var weekday = data[baseLocation + 1];
      var startTime = 0x00000000;
      for (var j = baseLocation + 2; j < baseLocation + 6; ++j)
      {
        startTime = startTime | (data[j] << (8 * (j - (baseLocation + 2))));
      }
      var duration = 0x00000000;
      for (var j = baseLocation + 6; j < baseLocation + 10; ++j)
      {
        duration = duration | (data[j] << (8 * (j - (baseLocation + 6))));
      }

      // Calculate the start and stop times
      var startHour = startTime / 3600 /* 60 * 60 */ | 0x00000000;
      var startMinute = (startTime - startHour * 3600 /* 60 * 60 */) / 60 | 0x00000000;
      var startSecond = startTime - startHour * 3600 /* 60 * 60 */ - startMinute * 60;
      var stopTime = startTime + duration;
      if (stopTime > 86400 /* 24 * 60 * 60 */)
        stopTime = stopTime - 86400;
      stopHour = stopTime / 3600 /* 60 * 60 */ | 0x00000000;
      stopMinute = (stopTime - stopHour * 3600 /* 60 * 60 */) / 60 | 0x00000000;
      stopSecond = stopTime - stopHour * 3600 /* 60 * 60 */ - stopMinute * 60;

      // Set the fields
      $("#button-" + i).val(button);
      $("#weekday-" + i).val(weekday);
      $("#start-hour-" + i).val(startHour);
      $("#start-minute-" + i).val(startMinute);
      $("#start-second-" + i).val(startSecond);
      $("#stop-hour-" + i).val(stopHour);
      $("#stop-minute-" + i).val(stopMinute);
      $("#stop-second-" + i).val(stopSecond);
    }

    // Hide the AJAX animation
    $("#ajax-loader").hide();
  }).
  fail(function(jqXHR, textStatus, errorThrown)
  {
    // Hide the AJAX animation
    $("#ajax-loader").hide();

    // Show the error message
    alert("Failed to retrieve memory schedules.")
  });

  // Attach to the Save button
  $(".btn-green").on("click", function()
  {
    // Show the AJAX animation
    $("#ajax-loader").show();

    // Create the data array
    var data = new Uint8Array(SIZE_OF_MEMORY_SCHEDULE * MEMORY_SCHEDULE_COUNT);

    // Loop through all the schedules
    for (var i = 1; i <= MEMORY_SCHEDULE_COUNT; ++i)
    {
      // Get the data for this schedule
      var button = parseInt($("#button-" + i).val());
      var weekday = parseInt($("#weekday-" + i).val());
      var startHour = parseInt($("#start-hour-" + i).val());
      var startMinute = parseInt($("#start-minute-" + i).val());
      var startSecond = parseInt($("#start-second-" + i).val());
      var stopHour = parseInt($("#stop-hour-" + i).val());
      var stopMinute = parseInt($("#stop-minute-" + i).val());
      var stopSecond = parseInt($("#stop-second-" + i).val());

      // Calculate the time since midnight and duration
      var startTime = startHour * 3600 /* 60 * 60 */ + startMinute * 60 + startSecond;
      var stopTime = stopHour * 3600 /* 60 * 60 */ + stopMinute * 60 + stopSecond;
      var duration = stopTime - startTime;
      if (duration < 0)
        duration = duration + 86400 /* 24 * 60 * 60 */;

      // Add the data to the array
      var baseLocation = SIZE_OF_MEMORY_SCHEDULE * (i - 1);
      data[baseLocation] = button;
      data[baseLocation + 1] = weekday;
      for (var j = baseLocation + 2; j < baseLocation + 6; ++j)
      {
        data[j] = (startTime >> (8 * (j - (baseLocation + 2)))) & 0x000000FF;
      }
      for (var j = baseLocation + 6; j < baseLocation + 10; ++j)
      {
        data[j] = (duration >> (8 * (j - (baseLocation + 6)))) & 0x000000FF;
      }
    }

    // Send the data
    $.ajax({
      url: "/setms",
      type: "GET",
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
      alert("Failed to save memory schedules.")
    });
  });
});