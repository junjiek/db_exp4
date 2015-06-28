$(document).ready(function() {
    var map;
    var markers = [];

    var init_latlng = new google.maps.LatLng(1.352083, 103.81983600000001);

    var mapOptions = {
        zoom: 14,
        center: init_latlng,
        mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    map = new google.maps.Map(document.getElementById('map_canvas'),
                              mapOptions);

    var rectangle = new google.maps.Rectangle();
    var rectOptions = {
                        strokeColor: '#FF0000',
                        strokeOpacity: 0.8,
                        strokeWeight: 2,
                        fillColor: '#FF0000',
                        fillOpacity: 0.35,
                        map: map,
                        bounds: null
    };
    rectangle.setOptions(rectOptions);
    rectangle.setVisible(true);
    var editing = false;
    function setPoint(p1, p2) {
        var x = new google.maps.LatLng(p1.lat() < p2.lat() ? p1.lat() : p2.lat(),
                                       p1.lng() < p2.lng() ? p1.lng() : p2.lng());
        var y = new google.maps.LatLng(p1.lat() > p2.lat() ? p1.lat() : p2.lat(),
                                       p1.lng() > p2.lng() ? p1.lng() : p2.lng());
        rectangle.setBounds(new google.maps.LatLngBounds(x, y));
    }
    function clk(e) {
        console.log(e);
        if (editing) {
            editing = false;
            pt2 = e.latLng;
            setPoint(pt1, pt2);
            showSearch();
        } else {
            editing = true;
            pt1 = pt2 = e.latLng;
            setPoint(pt1, pt2);
        }
    };
    function move(e) {
        if (editing) {
            pt2 = e.latLng;
            setPoint(pt1, pt2);
        }
    }
    google.maps.event.addListener(map, 'click', clk);
    google.maps.event.addListener(rectangle, 'click', clk);
    google.maps.event.addListener(map, 'mousemove', move);
    google.maps.event.addListener(rectangle, 'mousemove', move);

    document.getElementById('num').value = "10";

    function deleteMarkers() {
        for (var i = 0; i < markers.length; ++i) {
            markers[i].setMap(null);
        }
        markers = [];
    }

    function addMarker(lat, lng) {
        marker = new google.maps.Marker({
                        position: new google.maps.LatLng(lat, lng),
                        map: map
                    })
        markers.push(marker);
        google.maps.event.addListener(marker, 'click', function() {
            calcRoute(center_marker.getPosition(), marker.getPosition());
        });
    }

    function genResults(time, tags) {
        var ret = "";
        ret += '<div id="time_show">' + 
                tags.length + ' tags found in ' + parseFloat(time).toFixed(6) + ' seconds.' + 
               '</div>';

        for (var i = 0; i < tags.length; ++i) {
            var loc = tags[i];
            ret += '<div class="result_item" id="result_item_' + i + '">';
            ret += '<span class="name">'+ loc[0] + '</span><br>';
            ret += '<span class="count"> count: ' + loc[1]+ '</span>';
            ret += '</div>';
        }
        document.getElementById("result").innerHTML = ret;
    };

    function showSearch() {

        deleteMarkers();
        var num = parseInt(document.getElementById('num').value);
        if (isNaN(num)) {
            document.getElementById("result").innerHTML="";
            return;
        }
        document.getElementById('num').value = num;
        var xmlhttp;
        if (rectangle.getBounds() == null) {
            document.getElementById("result").innerHTML="";
            return;
        }
        if (window.XMLHttpRequest) {
            // code for IE7+, Firefox, Chrome, Opera, Safari
            xmlhttp = new XMLHttpRequest();
        } else {
            // code for IE6, IE5
            xmlhttp = new ActiveXObject("Microsoft.XMLHTTP");
        }
        xmlhttp.onreadystatechange = function() {
            if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
                console.log(xmlhttp.responseText);
                var split = xmlhttp.responseText.indexOf(';');
                var time = '';
                if (split > 0) {
                    time = xmlhttp.responseText.substr(0, split);
                }

                var tags = eval("("+xmlhttp.responseText.substr(split+1)+")");
                genResults(time, tags);

            }
        }
        var bound = rectangle.getBounds();
        url = '/search?';
        url += 'num=' + num;
        url += '&xmin=' + bound.getSouthWest().lat();
        url += '&ymin=' + bound.getSouthWest().lng();
        url += '&xmax=' + bound.getNorthEast().lat();
        url += '&ymax=' + bound.getNorthEast().lng();
        xmlhttp.open("GET", url, true);
        xmlhttp.send();
    }
    function clear() {
        document.getElementById('num').value = "";
        document.getElementById("result").innerHTML="";
        rectangle.setBounds(null);
    }

    $('#clearButton').on("click", function() { clear(); });
    $("#num").on("input", function() { showSearch(); });

});
