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

// Define constants
var ADDRESSES_COUNT = 5; // MAC, IP, DNS, Gateway, Subnet
var COLOR_VALUES_COUNT = 5; // Power, Red, Green, Blue, and White
var MEMORY_SCHEDULE_COUNT = 50; // Must be multiples of 2
var TIMER_SCHEDULE_COUNT = 50; // Must be multiples of 5
var SIZE_OF_MAC_ADDRESS = 6;
var SIZE_OF_IP_ADDRESS = 4;
var SIZE_OF_TIME_ZONE = 14;
var SIZE_OF_COLOR_VALUES = 4;
var SIZE_OF_MEMORY_SCHEDULE = 10;
var SIZE_OF_TIMER_SCHEDULE = 6;

// Run this code when ready
$(document).ready(function()
{
  // Attach to the dropdown menu achor tag in the navigation bar
  $("#navbar-items .dropdown > a").on("click", function()
  {
    // Remove focus
    $(this).blur();
  })
});
