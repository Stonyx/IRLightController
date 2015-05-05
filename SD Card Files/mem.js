// Run this when ready
$(document).ready(function() 
{
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