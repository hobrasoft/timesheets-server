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

        this.storeSessionExpires = function(x) {
            if (x == null) { return; }
            var date = new Date(x);
            sessionStorage.setItem("SessionExpires", date.getTime());
            }

        this.get = function(url, params) {
            var rq = new XMLHttpRequest();
            var todleto = this;
            rq.onerror = function() {
                console.log("error2: " + rq.responseText);
                todleto.onError(rq.responseText);
                };
            rq.onreadystatechange = function() {
                todleto.storeSessionExpires(rq.getResponseHeader("X-Session-Expires"));
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
        this.categoriestree = function (category, maxdepth) { 
                    maxdepth = typeof maxdepth == 'undefined' ? 0 : maxdepth;
                    category = typeof category == 'undefined' ? 0 : category;
                    this.get("categoriestree/"+category,"maxdepth=" + maxdepth); 
                    }
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
        this.employees = function (employee) { if (typeof employee !== 'undefined') { this.get("employees/" + employee); } else { this.get("employees"); } }
        this.saveEmployee = function (e) { this.put("employees/", JSON.stringify(e)); }
        this.removeEmployee = function(e) { this.delete("employees/" + e); }
        this.doors = function (door) { if (typeof door !== 'undefined') { this.get("doors/" + door); } else { this.get("doors"); } }
        this.saveDoor = function (d) { this.put("doors/", JSON.stringify(d)); }
        this.removeDoor = function(d) { this.delete("doors/" + d); }
        this.doorEmployees = function (door) { this.get("doors/" + door + "/employees"); }
        this.saveDoorEmployee = function (x) { this.put("doors/" + x.door + "/employees", JSON.stringify(x)); }
        this.removeDoorEmployee = function (door, employee) { this.delete("doors/" + door + "/employees/" + employee); }
        this.departments = function (department) { if (typeof department !== 'undefined') { this.get("departments/" + department); } else { this.get("departments"); } }
        this.saveDepartment = function (d) { this.put("departments/", JSON.stringify(d)); }
        this.removeDepartment = function(d) { this.delete("departments/" + d); }
        this.departmentEmployees = function (department) { this.get("departments/" + department + "/employees"); }
        this.saveDepartmentEmployee = function (x) { this.put("departments/" + x.department + "/employees", JSON.stringify(x)); }
        this.removeDepartmentEmployee = function (department, employee) { this.delete("departments/" + department + "/employees/" + employee); }
        this.departmentManagers = function (department) { this.get("departments/" + department + "/managers"); }
        this.saveDepartmentManager = function (x) { this.put("departments/" + x.department + "/managers", JSON.stringify(x)); }
        this.removeDepartmentManager = function (department, user) { this.delete("departments/" + department + "/managers/" + user); }
        this.rfids = function (rfid) { if (typeof rfid !== 'undefined') { this.get("rfids/" + rfid); } else { this.get("rfids"); } }
        this.saveRfid = function (r) { this.put("rfids/", JSON.stringify(r)); }
        this.removeRfid = function(r) { this.delete("rfids/" + r); }
        this.rfidEmployees = function (rfid) { this.get("rfids/" + rfid + "/employees"); }
        this.saveRfidEmployee = function (x) { this.put("rfids/" + x.rfid + "/employees", JSON.stringify(x)); }
        this.removeRfidEmployee = function (rfid, employee) { this.delete("rfids/" + rfid + "/employees/" + employee); }
        this.holidays = function(date) { if (typeof date !== 'undefined') { this.get("holidays/" + date); } else { this.get("holidays"); } }
        this.saveHoliday = function(h) { this.put("holidays/", JSON.stringify(h)); }
        this.removeHoliday = function(d) { this.delete("holidays/" + d); }
        this.copyHolidays = function(fy, ty) { this.get("holidays/copy/" + fy + "/" + ty); }
        this.workCalendar = function(period) { if (typeof period !== 'undefined') { this.get("workcalendar/" + period); } else { this.get("workcalendar"); } }
        this.saveWorkCalendar = function(w) { this.put("workcalendar/", JSON.stringify(w)); }
        this.removeWorkCalendar = function(p) { this.delete("workcalendar/" + p); }
        this.events = function(params) { if (typeof params === 'undefined') { this.get("events"); } else { this.get("events", params); } }
        this.event = function(e) { this.get("events/" + e); }
        this.saveEvent = function(e) { this.put("events/", JSON.stringify(e)); }
        this.removeEvent = function(e) { this.delete("events/" + e); }
        this.saveTicket = function(t) { this.put("ticketsvw/", JSON.stringify(t)); }
        this.startTimesheet = function(t) { this.get("timesheet/start/" + t); }
        this.stopTimesheet = function(t) { this.get("timesheet/stop/" + t); }
        this.eventtypes = function (et) { if (typeof et !== "undefined") { this.get("eventtypes/" + et); } else { this.get("eventtypes"); } }
        this.saveEventType = function (e) { this.put("eventtypes/", JSON.stringify(e)); }
        this.removeEventType = function (e) { this.delete("eventtypes/" + e); }
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
        this.categoriesOverview = function (statuses) {
                    this.get("overview/categories", "statuses=" + statuses.join(","));
                    }
        this.overview = function (category, statuses) {
                    if (typeof category !== 'undefined')
                        { this.get("overview/" + category,  "statuses=" + statuses.join(",")); }
                        else
                        { this.get("overview"); }
                    }

        }
}


