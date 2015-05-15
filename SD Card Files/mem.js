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

  // Attach to the button
  $("button").on("click", function()
  {
    // Show the AJAX animation
    $("#ajax").show();

    // Create the data array
    var data = new Uint8Array(SIZE_OF_MEMORY_SCHEDULE * MEMORY_SCHEDULE_COUNT);

    // Loop through all the schedules
    for (var i = 1; i <= MEMORY_SCHEDULE_COUNT; ++i)
    {
      // Get the data for this schedule
      var button = $("#button-" + i).val();
      var weekday = $("#weekday-" + i).val();
      var startHour = $("#start-hour-" + i).val();
      var startMinute = $("#start-minute-" + i).val();
      var startSecond = $("#start-second-" + i).val();
      var stopHour = $("#stop-hour-" + i).val();
      var stopMinute = $("#stop-minute-" + i).val();
      var stopSecond = $("#stop-second-" + i).val();

      // Calculate the time since midnight and duration
      var timeSinceMidnight = startHour * 3600 /* 60 * 60 */ + startMinute * 60 + startSecond;
      var duration = stopHour * 3600 /* 60 * 60 */ + stopMinute * 60 + stopSecond;
      if (duration > timeSinceMidnight)
        duration = duration - timeSinceMidnight;
      else
        duration = 86400 /* 24 * 60 * 60 */ - timeSinceMidnight + duration;

      // Add the data to the array
      var baseLocation = SIZE_OF_MEMORY_SCHEDULE * (i - 1);
      data[baseLocation] = button;
      data[baseLocation + 1] = weekday;
      for (var j = baseLocation + 2; j < baseLocation + 6; ++j)
      {
        data[j] = timeSinceMidnight & 0x000000FF;
        timeSinceMidnight = timeSinceMidnight << 8;
      }
      for (var j = baseLocation + 6; j < baseLocation + 10; ++j)
      {
        data[j] = duration & 0x000000FF;
        duration = duration << 8;
      }
    }

    // Send the data
    $.ajax({
      url: "savems",
      type: "GET",
      dataType: "arraybuffer",
      processData: "false",
      data: data
    }).done(function(data, textStatus, jqXHR)
    {
      // Hide the AJAX animation and redirect to the index page
      $("#ajax").hide();
      window.location.href = 'index.htm';
    });
  });
});