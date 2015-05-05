

// Run this when ready
$(document).ready(function() 
{
  // Copy the schedule HTML
  for (var i = 1; i < MEMORY_SCHEDULE_COUNT; ++i)
  { 
    // Clone the first schedule and change various IDs
    var schedule = $("#schedule-0").clone();
    schedule.attr("id", "schedule-" + i);
    schedule.find("label[for='memory-0']").attr("for", "memory-" + i);
    schedule.find("#memory-0").attr("id", "memory-" + i);
    schedule.find("label[for='weekday-0']").attr("for", "weekday-" + i);
    schedule.find("#weekday-0").attr("id", "weekday-" + i);
    schedule.find("label[for='start-hour-0']").attr("for", "start-hour-" + i);
    schedule.find("#start-hour-0").attr("id", "start-hour-" + i);
    schedule.find("label[for='start-minute-0']").attr("for", "start-minute-" + i);
    schedule.find("#start-minute-0").attr("id", "start-minute-" + i);
    schedule.find("label[for='start-second-0']").attr("for", "start-second-" + i);
    schedule.find("#start-second-0").attr("id", "start-second-" + i);
    schedule.find("label[for='end-hour-0']").attr("for", "end-hour-" + i);
    schedule.find("#end-hour-0").attr("id", "end-hour-" + i);
    schedule.find("label[for='end-minute-0']").attr("for", "end-minute-" + i);
    schedule.find("#end-minute-0").attr("id", "end-minute-" + i);
    schedule.find("label[for='end-second-0']").attr("for", "end-second-" + i);
    schedule.find("#end-second-0").attr("id", "end-second-" + i);
    schedule.find("#line-0").attr("id", "line-" + i);

    // Check if this is the last schedule
    if (i == MEMORY_SCHEDULE_COUNT - 1)
      schedule.find("#line-" + i).remove();

    // Add the schedule
    schedule.appendTo("#schedules");
  }

  // Attach to the button
  $("button").on("click", function()
  {
    $.ajax({
      url: "setms",
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