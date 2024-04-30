$(document).ready(function(){
    var userinfo = JSON.parse(localStorage.getItem("userinfo"));
    if (userinfo != null) {
        $("#navbar-username").html(userinfo.name);
        $("#navbar-servername").html(userinfo.server_name);
        }
    });

function userAdmin() {
    var userinfo = JSON.parse(localStorage.getItem("userinfo"));
    if (userinfo == null) { return false; }
    return userinfo.admin;
}

function userId() {
    var userinfo = JSON.parse(localStorage.getItem("userinfo"));
    if (userinfo == null) { return 0; }
    return userinfo.userid;
}

function userName() {
    var userinfo = JSON.parse(localStorage.getItem("userinfo"));
    if (userinfo == null) { return ""; }
    return userinfo.name;
}


Number.prototype.pad = function(size) {
    var s = String(this);
    while (s.length < (size || 2)) { s = "0" + s; }
    return s;
    };

Number.prototype.formatHHMM = function() {
    if (isNaN(this)) { return ""; }
    var hh = Math.floor(this/3600);
    var mm = Math.floor((this - hh*3600)/60);
    var ss = Math.floor(this%60);
    return hh.pad(2)+":"+mm.pad(2);
    };

Number.prototype.floor = function() {
    return Math.floor(this);
    }

Number.prototype.formatHHMMSS = function() {
    if (isNaN(this)) { return ""; }
    var hh = Math.floor(this/3600);
    var mm = Math.floor((this - hh*3600)/60);
    var ss = Math.floor(this%60);
    return hh.pad(2)+":"+mm.pad(2)+":"+ss.pad(2);
    };

Number.prototype.toLatitude = function() {
    if (isNaN(this)) { return ""; }
    var hh = Math.floor(this);
    var mm = Math.floor((this - hh)*60);
    var ss = Math.round(10*(this*60 - Math.floor(this*60)) * 60)/10;;
    return hh.pad(2) + "°" + mm.pad(2) + "'" + ss.pad(2) + '"' + ((this>0) ? " N" : " S");
    };

Number.prototype.toLongitude = function() {
    if (isNaN(this)) { return ""; }
    var hh = Math.floor(this);
    var mm = Math.floor((this - hh)*60);
    var ss = Math.round(10*(this*60 - Math.floor(this*60)) * 60)/10;;
    return hh.pad(2) + "°" + mm.pad(2) + "'" + ss.pad(2) + '"' + ((this>0) ? " E" : " W");
    };

Date.prototype.formatYYYYMMDDHHMM = function() {
    if (isNaN(this.getTime())) { return ""; }
    var yy = this.getFullYear();
    var mt = this.getMonth() + 1;
    var dd = this.getDate();
    var hh = this.getHours();
    var mm = this.getMinutes();
    return yy.pad(4)+"-"+mt.pad(2)+"-"+dd.pad(2)+" "+hh.pad(2)+":"+mm.pad(2);
    }

Date.prototype.formatHHMM = function() {
    if (isNaN(this.getTime())) { return ""; }
    var hh = this.getHours();
    var mm = this.getMinutes();
    return hh.pad(2)+":"+mm.pad(2);
    }

Date.prototype.formatHHMMSS = function() {
    if (isNaN(this.getTime())) { return ""; }
    var hh = this.getHours();
    var mm = this.getMinutes();
    var ss = this.getSeconds();
    return hh.pad(2)+":"+mm.pad(2)+":"+ss.pad(2);
    }

Date.prototype.secsTo = function (date2) {
    if (isNaN(this.getTime())) { return ""; }
    if (isNaN(date2.getTime())) { return ""; }
    var d1 = this.getTime();
    var d2 = date2.getTime();
    return (d2 - d1) / 1000;
    }

String.prototype.secsTo = function (date2) {
    var date1date = new Date(this);
    var date2date = new Date(date2);
    if (isNaN(date1date.getTime())) { return ""; }
    if (isNaN(date2date.getTime())) { return ""; }
    var d1 = date1date.getTime();
    var d2 = date2date.getTime();
    return (d2 - d1) / 1000;
    }

String.prototype.formatYYYYMMDDHHMM = function() {
    var date = new Date(this);
    return date.formatYYYYMMDDHHMM();
    }

Boolean.prototype.toString = function() {
    return  (this) ? "true" : "false";
    }


function rgb2hsl(r, g, b) {
    // see https://en.wikipedia.org/wiki/HSL_and_HSV#Formal_derivation
    // convert r,g,b [0,255] range to [0,1]
    r = r / 255,
    g = g / 255,
    b = b / 255;
    // get the min and max of r,g,b
    var max = Math.max(r, g, b);
    var min = Math.min(r, g, b);
    // lightness is the average of the largest and smallest color components
    var lum = (max + min) / 2;
    var hue;
    var sat;
    if (max == min) { // no saturation
        hue = 0;
        sat = 0;
    } else {
        var c = max - min; // chroma
        // saturation is simply the chroma scaled to fill
        // the interval [0, 1] for every combination of hue and lightness
        sat = c / (1 - Math.abs(2 * lum - 1));
        switch(max) {
            case r:
                // hue = (g - b) / c;
                // hue = ((g - b) / c) % 6;
                // hue = (g - b) / c + (g < b ? 6 : 0);
                break;
            case g:
                hue = (b - r) / c + 2;
                break;
            case b:
                hue = (r - g) / c + 4;
                break;
        }
    }
    hue = Math.round(hue * 60); // °
    sat = Math.round(sat * 100); // %
    lum = Math.round(lum * 100); // %
    return [hue, sat, lum];
}


String.prototype.foregroundColor = function () {
    if (this == "transparent") {
        return "black"; 
        }
    var color = parseCSSColor(this);
    if (color == null) { return "black"; }
    var hsl = rgb2hsl(color[0], color[1], color[2]);
    return (hsl[2] > 50) ? "black" : "white";
}


function foregroundColor(text) {
    return text.foregroundColor();
}


/**
 * @brief Vrací parametr GET požadavku stránky
 */
$.urlParam = function(name) {
    var results = new RegExp('[\?&]' + name + '=([^&#]*)').exec(window.location.href);
    if (results==null) {
        return null;
      } else {
        return results[1] || 0;
        }
};


