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

var igcFormat = new ol.format.IGC({});
var geojsonFormat = new ol.format.GeoJSON();

