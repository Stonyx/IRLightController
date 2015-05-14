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
  for (var i = 2; i <= TIMER_SCHEDULE_COUNT; ++i)
  { 
    // Clone the first schedule and change heading text and various IDs
    var schedule = $("#schedule-1").clone();
    schedule.attr("id", "schedule-" + i);
    schedule.find("#heading-1").html("<strong>Timer Schedule #" + i + "</strong>");
    schedule.find("#heading-1").attr("id", "heading-" + i);
    schedule.find("label[for='button-1']").attr("for", "button-" + i);
    schedule.find("#button-1").attr("id", "button-" + i);
    schedule.find("label[for='weekday-1']").attr("for", "weekday-" + i);
    schedule.find("#weekday-1").attr("id", "weekday-" + i);
    schedule.find("label[for='hour-1']").attr("for", "hour-" + i);
    schedule.find("#hour-1").attr("id", "hour-" + i);
    schedule.find("label[for='minute-1']").attr("for", "minute-" + i);
    schedule.find("#minute-1").attr("id", "minute-" + i);
    schedule.find("label[for='second-1']").attr("for", "second-" + i);
    schedule.find("#second-1").attr("id", "second-" + i);
    schedule.find("#line-1").attr("id", "line-" + i);

    // Check if this is the last schedule
    if (i == TIMER_SCHEDULE_COUNT)
      schedule.find("#line-" + i).remove();

    // Add the schedule
    schedule.appendTo("#schedules");
  }

  // Attach to the button
  $("button").on("click", function()
  {
    // Create the data array
    var data = new Uint8Array(SIZE_OF_TIMER_SCHEDULE * TIMER_SCHEDULE_COUNT);

    // Loop through all the schedules
    for (var i = 1; i <= TIMER_SCHEDULE_COUNT; ++i)
    {
      // Get the data for this schedule
      var button = $("#button-" + i).val();
      var weekday = $("#weekday-" + i).val();
      var startHour = $("#hour-" + i).val();
      var startMinute = $("#minute-" + i).val();
      var startSecond = $("#second-" + i).val();

      // Calculate the time since midnight and duration
      var timeSinceMidnight = startHour * 3600 /* 60 * 60 */ + startMinute * 60 + startSecond;

      // Add the data to the array
      var baseLocation = SIZE_OF_TIMER_SCHEDULE * (i - 1);
      data[baseLocation] = button;
      data[baseLocation + 1] = weekday;
      for (var j = baseLocation + 2; j < baseLocation + 6; ++j)
      {
        data[j] = timeSinceMidnight & 0x000000FF;
        timeSinceMidnight = timeSinceMidnight << 8;
      }
    }

    // Send the data
    $.ajax({
      url: "savets",
      type: "GET",
      dataType: "arraybuffer",
      processData: "false",
      data: data
    }).done(function(data, textStatus, jqXHR)
    {
      // Redirect to the index page
      window.location.href = 'index.htm';
    });
  });
});