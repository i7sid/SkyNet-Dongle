/*
 * vis-utils.js
 * Copyright (C) 2016 Michael Zapf <michael.zapf@fau.de>
 */

// check if file API is available
if (window.File && window.FileReader && window.FileList && window.Blob) {
    // all good
} else {
    alert('The File APIs are not fully supported in this browser. ' +
            'Please be aware that you won\'t be able to load any files.');
}

var readLocalIGCFile = function(path) {
    var reader = new FileReader();
    reader.onload = function(e) {
        readFeatureText(e.target.result);
        map.render();
        drawCanvas();
    };
    reader.readAsText(path);
};

var readLocalJsonFile = function(path) {
    var reader = new FileReader();
    reader.onload = function(e) {
        wind_data = [];
        eval(e.target.result);

        wind_data.forEach(function(json) {
            var features = geojsonFormat.readFeatures(json,
                                            {featureProjection: 'EPSG:3857'});
            readWindData(features);
        });
        map.render();
    };
    reader.readAsText(path);
};


$(document).ready(function() {
    $("#controls,#map")
        .on('dragover', dragover)
        .on('dragenter', dragover);

    $('#controls,#map').on(
        'drop',
        function(e) {
            if (e.originalEvent.dataTransfer) {
                if (e.originalEvent.dataTransfer.files.length) {
                    e.preventDefault();
                    e.stopPropagation();

                    for (var i = 0; i < e.originalEvent.dataTransfer.files.length; ++i) {
                        var ext = e.originalEvent.dataTransfer.files[i].name.split('.').pop();

                        if (ext == 'igc') {
                            readLocalIGCFile(e.originalEvent.dataTransfer.files[i]);
                        }
                        else if (ext == 'json') {
                            readLocalJsonFile(e.originalEvent.dataTransfer.files[i]);
                        }

                        // TODO Lade Wind-Daten
                    }
                }
            }
        }
    );

});

var dragover = function(e) {
    e.preventDefault();
    e.stopPropagation();
    e.originalEvent.dataTransfer.dropEffect = 'copy';
};

