
/* State update */
var timer_id = 0;
function update(stop) {
    if (stop == 1) {
        clearTimeout(timer_id);
        $('#update_button').html("update");
        $('#stop_button').html("")
        return;
    }
    timer_id = setTimeout("update(0)", 100);
    $('#update_button').html("");
    $('#stop_button').html("stop update");

    $.getJSON("/update",{}, function (flags) {
        for (var flag in flags) {
            if (flags[flag] == 1) {
                $('#'+flag).css("background-color","magenta");
            } else {
                $('#'+flag).css("background-color", "cyan");
            }
        }
    });
    $.getJSON("/update_loc",{}, function (data) {
        for (var item in data) {
            $('#'+item).html(data[item]);
        }
    });
    refresh_chart();
};

var speed_log = [0, 0];
var time_log = [0, 1];

function update_speed() {
    "use strict";
    if (time_log.length == 2) {
        $.getJSON("/get_speed_array",{}, function (data) {
             speed_log = data['speed_array'];
             time_log = data['time_array'];
        });
    }
    $.getJSON("/update_speed",{}, function (data) {
         speed_log.shift();
         time_log.shift();
         speed_log.push(data['speed']);
         time_log.push(data['time']);
         $('#oht_speed').html(data['speed'])
    });
}
