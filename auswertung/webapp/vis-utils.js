/*
 * vis-utils.js
 * Copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

function get(url, cb) {
    var client = new XMLHttpRequest();
    client.open('GET', url);
    client.onload = function() { cb(client.responseText); };
    client.send();
}

function pad2(num) {
    var size = 2;
    var s = "00" + num;
    return s.substr(s.length-size);
}

var igcFormat = new ol.format.IGC({});
var geojsonFormat = new ol.format.GeoJSON();

