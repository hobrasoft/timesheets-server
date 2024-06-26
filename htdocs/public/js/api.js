class Api {
    constructor() {
        this.apiPath = "/api/v1/";
        this.onFinished = function(data) { }
        this.onError = function(errorText) { console.log("error1: " + errorText); }
        this.completeUrl = function(url, params) {
            // Android && KDE part
            if (typeof settings !== 'undefined') {
                if (typeof params === 'undefined') {
                    return (settings.useSSL ? "https://" : "http://") + settings.serverName + settings.apiPath + url + "?user=" + settings.username + "&password=" + settings.password;
                  } else {
                    return (settings.useSSL ? "https://" : "http://") + settings.serverName + settings.apiPath + url + "?user=" + settings.username + "&password=" + settings.password + params;
                    }
                }

            if (typeof settings === 'undefined') {
                if (typeof params === 'undefined') {
                    return this.apiPath + url;
                  } else {
                    return this.apiPath + url + params;
                    }
                }
            }

        this.initpageAll = function() {
            if (typeof initpage === 'undefined') { return true; }
            if (typeof initpage !== 'undefined') { return initpage.all(); }
            }

        this.initpageUserid = function() {
            if (typeof initpage !== 'undefined') { return initpage.userid; }
            if (typeof initpage === 'undefined') { return userId(); }
            }


        this.get = function(url, params) {
            var rq = new XMLHttpRequest();
            var todleto = this;
            rq.onerror = function() {
                console.log("error2: " + rq.responseText);
                todleto.onError(rq.responseText);
                };
            rq.onreadystatechange = function() {
                if (rq.readyState === XMLHttpRequest.DONE && (rq.status == 200 || rq.status == 204)) {
                    todleto.onFinished(JSON.parse(rq.responseText));
                    }
                if (rq.readyState === XMLHttpRequest.DONE && rq.status == 401) {
                    todleto.onError('Unauthorized');
                    }
                if (rq.readyState === XMLHttpRequest.DONE && rq.status == 404) {
                    todleto.onError('4040 Not found');
                    }
                };
            params = typeof params === 'undefined' ? '' : '?'+params;
            rq.open("GET", todleto.completeUrl(url,params), true);
            rq.send();
            }

        this.put = function(url, data) {
            var rq = new XMLHttpRequest();
            var todleto = this;
            rq.onerror = function() {
                console.log("error3: " + rq.responseText);
                todleto.onError(rq.responseText);
                };
            rq.onreadystatechange = function() {
                if (rq.readyState === XMLHttpRequest.DONE && (rq.status == 200 || rq.status == 204)) {
                    todleto.onFinished(JSON.parse(rq.responseText));
                    }
                if (rq.readyState === XMLHttpRequest.DONE && rq.status == 401) {
                    todleto.onError('Unauthorized');
                    }
                };
            rq.open("PUT", todleto.completeUrl(url), true);
            rq.setRequestHeader("Content-Type", "application/json; charset=UTF-8");
            rq.send(data);
            }

        this.delete = function(url) {
            var rq = new XMLHttpRequest();
            var todleto = this;
            rq.onerror = function() {
                console.log("error4: " + rq.responseText);
                todleto.onError(rq.responseText);
                };
            rq.onreadystatechange = function() {
                if (rq.readyState === XMLHttpRequest.DONE && (rq.status == 200 || rq.status == 201)) {
                    todleto.onFinished(JSON.parse(rq.responseText));
                    }
                if (rq.readyState === XMLHttpRequest.DONE && rq.status == 401) {
                    todleto.onError('Unauthorized');
                    }
                };
            // var completeUrl = (settings.useSSL ? "https://" : "http://") + settings.serverName + settings.apiPath + url + "?user=" + settings.username + "&password=" + settings.password;
            rq.open("DELETE", todleto.completeUrl(url), true);
            rq.send();
            }

        this.category = function (c) { this.get("categories/"+c); }
        this.categories = function (category) { 
                    if (typeof category === 'undefined') {
                        this.get("categories"); 
                      } else {
                        this.get("categories/"+category); 
                        }
                    }
        this.categoriesToRoot = function (c) { this.get("categoriestoroot/"+c); }
        this.categoriestree = function (category) { this.get("categoriestree/"+category,"maxdepth=0"); }
        this.categoriessiblings = function (category) { this.get("categoriessiblings/"+category); }
        this.ticketsvw = function (category) { this.get("ticketsvw", "category=" + category + "&all=" + this.initpageAll()); }
        this.ticketsvwall = function () { this.get("ticketsvw", "all=" + this.initpageAll()); }
        this.ticketvw = function (ticket) { this.get("ticketsvw/"+ticket+"?all=true"); }
        this.saveCategory = function (c) { this.put("categories/", JSON.stringify(c)); }
        this.status = function (status) { this.get("statuses/" + status); }
        this.statuses = function (category, prevstatus) {
                    if (typeof category !== 'undefined') {
                        this.get("statuses", "category="+category+"&previousStatuses="+JSON.stringify(prevstatus));
                      } else {
                        this.get("statuses");
                        }
                    }
        this.saveStatus = function (x) { this.put("statuses/", JSON.stringify(x)); }
        this.removeStatus = function (x) { this.delete("statuses/" + x); }
        this.users = function (user) { if (typeof user !== 'undefined') { this.get("users/" + user); } else { this.get("users"); } }
        this.saveUser = function (u) { this.put("users/", JSON.stringify(u)); }
        this.changePassword = function (data) { this.put("users/set-password", JSON.stringify(data)); }
        this.removeUser = function(u) { this.delete("users/" + u); }
        this.saveTicket = function(t) { this.put("ticketsvw/", JSON.stringify(t)); }
        this.startTimesheet = function(t) { this.get("timesheet/start/" + t); }
        this.stopTimesheet = function(t) { this.get("timesheet/stop/" + t); }
        this.removeTicket = function(t) { this.delete("tickets/" + t); }
        this.removeCategory = function(c) { this.delete("categories/" + c); }
        this.unauthenticate = function(user, password) { this.get("unauthenticate"); }
        this.authenticate = function(user, password) { this.get("authenticate?user=" + user + "&password=" + password); }
        this.overview = function (category, statuses) { this.get("overview/" + category,  "statuses=" + statuses.join(",")); }
        this.appendStatus = function (c) { c.user = this.initpageUserid(); c.date = new Date(); this.put("ticketstatus/", JSON.stringify(c)); }
        this.serverAbout = function () { this.get("server/about"); }
        this.saveServer = function (s) { this.put("server/about", JSON.stringify(s)); }
        this.removeOverview = function(o) { this.delete("overview/" + o); }
        this.usersCategories = function(c) { this.get("userscategories?category=" + c); }
        this.overview = function (category, statuses) {
                    if (typeof category !== 'undefined')
                        { this.get("overview/" + category,  "statuses=" + statuses.join(",")); }
                        else
                        { this.get("overview"); }
                    }

        }
}


