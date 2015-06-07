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

    document.getElementById('num').value = "5";
    document.getElementById('search_bar').value = "";

    function deleteMarkers() {
        for (var i = 0; i < markers.length; ++i) {
            markers[i].setMap(null);
        }
        markers = [];
    }

    function addMarker(lat, lng) {
        markers.push(new google.maps.Marker({
                     position: new google.maps.LatLng(lat, lng),
                     map: map
        }));
    }
    var genResults = function(time, pois) {
        var ret = "";
        ret += '<div id="time_show">' + 
               'Total ' + pois.length + ' results (' + parseFloat(time).toFixed(6) + ' seconds)' + 
               '</div>';

        for (var i = 0; i < pois.length; ++i) {
            var loc = pois[i];
            ret += '<div class="result_item" id="result_item_' + i + '">';
            ret += '<span class="name">'+ loc[0] + '</span><br>';
            ret += '<span class="latlng">' + loc[1] + ", " + loc[2] + '</span><br>';
            ret += '<span class="addr">' + loc[3] + '</span>';
            ret += '</div>';
            addMarker(loc[1], loc[2]);
        }
        document.getElementById("result").innerHTML = ret;

        $(".result_item").click(function() {
            var id = $(this).attr("id").substr(12);
            id = parseInt(id);
            markers[id].setAnimation(google.maps.Animation.DROP);
        });
    };

    function showSearch(pan) {
        deleteMarkers();
        var num = parseInt(document.getElementById('num').value);
        if (isNaN(num)) {
            return;
        }
        document.getElementById('num').value = num;
        var str = document.getElementById('search_bar').value;

        var xmlhttp;
        if (str.length == 0) {
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
                // order: name, lat, lng, addr
                var split = xmlhttp.responseText.indexOf(';');
                var time = '';
                if (split > 0) {
                    time = xmlhttp.responseText.substr(0, split);
                }

                var pois = eval("("+xmlhttp.responseText.substr(split+1)+")");
                
                if (pan && pois.length > 0) {
                    var lat = pois[0][1];
                    var lng = pois[0][2];
                    map.panTo(new google.maps.LatLng(lat, lng));
                }
                genResults(time, pois);

            }
        }
        url = '/search?';
        url += 'q=' + str;
        url += '&num=' + num;
        url += '&lat=' + map.getCenter().lat();
        url += '&lng=' + map.getCenter().lng();
        xmlhttp.open("GET", url, true);
        xmlhttp.send();
    }
    $("#search_bar").on("input", function() { showSearch(true); });
    $("#num").on("input", function() { showSearch(true); });
    google.maps.event.addListener(map, 'dragend', function() { showSearch(false); });
    google.maps.event.addListener(map, 'zoom_changed', function() { showSearch(false); });


});
